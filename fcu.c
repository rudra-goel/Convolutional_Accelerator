#include <stdlib.h>
#include <stdio.h>
#include <math.h>


#include "fcu.h"


/**
 * Multiplier function that takes two double values and returns their product.
 *
 * @param x_0 The first double value.
 * @param h_0 The second double value.
 * @return The product of x_0 and h_0.
 */
double multiplier(double x_0, double h_0) {
    double res = x_0 * h_0;
    if (isnan(res)) {
        fprintf(stderr, "Multiplication resulted in NaN\n\t x_0: %f\n\t h_0: %f\n", x_0, h_0);
        exit(EXIT_FAILURE);
    }
    return res;
}


/**
 * Adder function that takes two double values and returns their sum.
 *
 * @param x_0 The first double value.
 * @param h_0 The second double value.
 * @return The sum of x_0 and h_0.
 */
double adder(double x_0, double h_0) {
    double res = x_0 + h_0;
    if (isnan(res)) {
        fprintf(stderr, "Addition resulted in NaN\n");
        exit(EXIT_FAILURE);
    }

    return res;
}


/**
 * Functions that simulates pushing data into the shift register
 * 
 * Since shift registers are clocked, all data transfer happens in parallel
 * 
 * In this simulation, we assume that enqueueing data into the shift register is pushing data into the tail
 * Dequeueing from the shift register is popping data from the head
 * 
 * Dequeing is responsible for removing data from the head and shifting the rest of the data accordingly 
 * 
 * @param queue Pointer to the queue_s structure representing the shift register.
 * @param value The double value to be added to the queue.
 */
void enqueue(queue_s* queue, double value) {
    //simply assign the value passed in to the tail's data
    queue->tail->data = value;

    

    if (DEBUG_SHIFT_REGISTER) {
        printf("Enqueuing value: %f\n", value);
        print_shift_reg(queue);
    }
}

/**
 * Functiona that simulates popping data from the shift register
 * 
 * This function will:
 * ---> return the value at the head of the queue
 * ---> shift data @ middle to the head
 * ---> shift data @ tail to the middle
 * ---> Set the tail data to be 0.0
 * * @param queue Pointer to the queue_s structure representing the shift register.
 */
double dequeue(queue_s* queue) {
    //save value at the head
    double value = queue->head->data;

    //shift data from middle to head
    queue->head->data = queue->middle->data;

    //shift data from tail to middle
    queue->middle->data = queue->tail->data;

    //set tail data to 0.0
    queue->tail->data = 0.0;
    
    if (DEBUG_SHIFT_REGISTER) {
        printf("Dequeued value: %f\n", value);
        print_shift_reg(queue);
    }
    
    if (isnan(value)) {
        fprintf(stderr, "Dequeue resulted in NaN\n");
        exit(EXIT_FAILURE);
    }
    return value;
}

//accepts a pointer to a pointer to a queue_s structure
// This function initializes a shift register with three nodes (head, middle, tail).
void init_shift_reg (queue_s** queue, char name) {
    // Allocate memory for the queue structure
    *queue = (queue_s*)malloc(sizeof(queue_s));

    // Initialize the shift register with three nodes
    (*queue)->head = (shift_reg_node_s*)malloc(sizeof(shift_reg_node_s));
    (*queue)->middle = (shift_reg_node_s*)malloc(sizeof(shift_reg_node_s));
    (*queue)->tail = (shift_reg_node_s*)malloc(sizeof(shift_reg_node_s));
    (*queue)->name = name;

    if ((*queue)->head == NULL || (*queue)->middle == NULL || (*queue)->tail == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }

    // Initialize the data in the nodes to 0.0
    (*queue)->head->data = 0.0;
    (*queue)->middle->data = 0.0;
    (*queue)->tail->data = 0.0;

    // Set the next pointers to NULL
    (*queue)->head->next = NULL;
    (*queue)->middle->next = (*queue)->head;
    (*queue)->tail->next = (*queue)->middle;

    if (DEBUG_SHIFT_REGISTER) {
        printf("Shift register initialized with three nodes.\n");
        print_shift_reg(*queue);
    }

}


 /**
 * Function that simulates the three-parallel Fast Convolutional Unit (FCU)
 * This function takes inputs and coefficients, and performs the convolution of the two vectors via parallel FIR architecture
 *
 * It is the combonatinal logic that implements the architecture of the FCU
 * 
 * The order of execution here is important. Because hardware runs in parallel, all value in asynchronous / combonational logic will become stable with time (in between rising clock edges)
 *      However, in simulation / Clang, we need intermediate values to be stable before we can use them in the next layer of combinational logic, hence the order of execution is important
 *
 * @param inputs Pointer to the fcu_inputs_s structure containing input values.
 * @param kernel Pointer to the fcu_coefficients_s structure containing coefficients.
 */
fcu_outputs_s* three_parallel_fcu(
                        fcu_inputs_s* inputs, 
                        fcu_coefficients_s* kernel, 
                        queue_s* shift_reg_1, 
                        queue_s* shift_reg_2) {

    fcu_outputs_s* outputs = (fcu_outputs_s*)malloc(sizeof(fcu_outputs_s));
    if (outputs == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }

    //signal names are single character to make it more readable
    //there is a diagram in this repository that shows what intermediate signals have which names
    double a = multiplier(*inputs->x_0, kernel->h_0);
    double b = multiplier(*inputs->x_1, kernel->h_1);
    double c = multiplier(*inputs->x_2, kernel->h_2);
    double d = adder(*inputs->x_0, *inputs->x_1);
    double e = adder(*inputs->x_1, *inputs->x_2);

    //second layer of combinational logic
    double f = multiplier(d, kernel->h_01);
    double g = multiplier(e, kernel->h_12);
    double h = adder(d, *inputs->x_2);
    double j = adder(a, (-1) * dequeue(shift_reg_1));
    //need to do this after dequeueing from shift_reg_1
    //in hw, the SR would accept the value on the same clk edge that we dequeue from it
    enqueue(shift_reg_1, c); //enqueue x2h2 into the shift register (3x shift register)

    //third layer
    double m = multiplier(h, kernel->h_012);
    double k = adder(f, ((-1)*b));
    double l = adder(g, ((-1)*b));
    double y0 = adder(j, dequeue(shift_reg_2));

    
    //fourth layer
    double p  = adder(m, (-1)*k);
    double y1 = adder(k, (-1)*j);
    enqueue(shift_reg_2, l);

    //fifth layer
    double y2 = adder(p, (-1)*l);


    outputs->y_0 = y0;
    outputs->y_1 = y1;
    outputs->y_2 = y2;

    return outputs;
}