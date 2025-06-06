#include <stdio.h>
#include <stdlib.h>

//define a struct for the inputs for the FCU

typedef struct {
    double x_0;
    double x_1; 
    double x_2;
} fcu_inputs_s;

//struct for the FIRs impulse response coefficients
typedef struct {
    double h_0;
    double h_1;
    double h_2;
    double h_01;
    double h_12;
    double h_012;
} fcu_coefficients_s;

//struct for the outputs of the FCU
typedef struct {
    double y_0;
    double y_1;
    double y_2;
} fcu_outputs_s;

/**
 * Constants for the simulation program.
 */


 //struct for the shift register
 //queue implemented as a linked list with head and tail pointers
 typedef struct {
     double data;
     struct node* next;
 } shift_reg_node_s;

 typedef struct {
     shift_reg_node_s* head;
     shift_reg_node_s* tail;
     shift_reg_node_s* middle;
 } queue_s;

double multiplier(double x_0, double h_0);
double adder(double x_0, double x_1);
void enqueue(queue_s* queue, double value);
double dequeue(queue_s* queue);
fcu_outputs_s* three_parallel_fcu   (
                                    fcu_inputs_s* inputs, 
                                    fcu_coefficients_s* kernel, 
                                    queue_s* shift_reg_1, 
                                    queue_s* shift_reg_2
                                    );