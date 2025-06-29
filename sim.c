#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include "fcu.h"

void print_fcu_outputs(fcu_outputs_s* outputs, int starting, int ending, int idx);
void print_shift_reg(queue_s* queue);
void grab_next_ip_set(fcu_inputs_s* inputs); 
double* init_pixel_inputs(int size);
void print_image_pixels(double* pixels, int size);
void printSimulatorStartMessage();
void init_kernel(kernel_s** kernel);
void init_fcu_coefficients(fcu_coefficients_s** h);
void init_fcu(fcu_s** fcu, char* fcu_name);

double* image_pixels;
kernel_s* kernel;

//create an array of pointers to three parallel FCUs
fcu_s* fcu_array[3];


int main(int argc, char* argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <image_size>\n", argv[0]);
        return EXIT_FAILURE;
    }


    printSimulatorStartMessage();

    //initialize the kernel - ideally read from a file as ip without recompilation
    init_kernel(&kernel);
    
    // Initialize pixel inputs
    int image_size = atoi(argv[1]);
    image_pixels = init_pixel_inputs(image_size);
    print_image_pixels(image_pixels, image_size);

    //initialize each FCU to have inputs, ptr to kernel, shift regs, and op struct
    for (int i = 0; i < 3; i++) {
        char* name = (char*)malloc(7 * sizeof(char));
        name = "FCU - ";  
        char* tmp; *tmp = (char)(i);
        strcat(name, tmp);
        init_fcu(&fcu_array[i], name);
    }

    //Each FCU has a set of FIR filter coefficients. These coefficients are stored in the variable 'kernel'
    //Basically assign each FCU's 'h' var to point to the correct set of filter coefficients
    fcu_array[0]->h = kernel->kernel_row_1;
    fcu_array[1]->h = kernel->kernel_row_2;
    fcu_array[2]->h = kernel->kernel_row_3;


}

//initialize an FCU
void init_fcu(fcu_s** fcu, char* fcu_name) {
    //create a pointer to a fcu_s structure and assign it to the the value of the pointer passed into the arg
    *fcu = (fcu_s*)malloc(sizeof(fcu_s));
    
    if (*fcu == NULL) {
        fprintf(stderr, "Memory allocation failed for FCU\n");
        exit(EXIT_FAILURE);
    }

    // init the inpiuts struct
    (*fcu)->inputs = (fcu_inputs_s*)malloc(sizeof(fcu_inputs_s));
    if ((*fcu)->inputs == NULL) {
        fprintf(stderr, "Memory allocation failed for FCU inputs\n");
        exit(EXIT_FAILURE);
    }
    
    // Initialize coefficients
    (*fcu)->h = (fcu_coefficients_s*)malloc(sizeof(fcu_coefficients_s));
    if ((*fcu)->h == NULL) {
        fprintf(stderr, "Memory allocation failed for FCU coefficients\n");
        exit(EXIT_FAILURE);
    }
    
    // Initialize shift regs
    init_shift_reg(&((*fcu)->shift_reg_1), strcat(fcu_name, "_sr_a"));
    init_shift_reg(&((*fcu)->shift_reg_2), strcat(fcu_name, "_sr_b"));

    // Initialize outputs struct
    (*fcu)->outputs = (fcu_outputs_s*)malloc(sizeof(fcu_outputs_s));
    if ((*fcu)->outputs == NULL) {
        fprintf(stderr, "Memory allocation failed for FCU outputs\n");
        exit(EXIT_FAILURE);
    }
}

/**
 * Initialize the kernel
 */
void init_kernel(kernel_s** kernel) {
    *kernel = (kernel_s*)malloc(sizeof(kernel_s*));

    if (*kernel == NULL) {
        fprintf(stderr, "Memory allocation failed for kernel\n");
        exit(EXIT_FAILURE);
    }
    
    (*kernel)->kernel_row_1 = (fcu_coefficients_s*)malloc(sizeof(fcu_coefficients_s*));
    (*kernel)->kernel_row_3 = (fcu_coefficients_s*)malloc(sizeof(fcu_coefficients_s*));
    (*kernel)->kernel_row_2 = (fcu_coefficients_s*)malloc(sizeof(fcu_coefficients_s*));
    
    if ((*kernel)->kernel_row_1 == NULL || 
        (*kernel)->kernel_row_2 == NULL ||
        (*kernel)->kernel_row_3 == NULL) {
        
        fprintf(stderr, "Memory allocation failed for kernel row vectors\n");
        exit(EXIT_FAILURE);
    }

    //pass a pointer to a ptr for each kernel's row vector
    //pointer manipulation goes crazy
    init_fcu_coefficients(&((*kernel)->kernel_row_1));
    init_fcu_coefficients(&((*kernel)->kernel_row_3));
    init_fcu_coefficients(&((*kernel)->kernel_row_2));

}

/**
 * initialize a row vector in the kernel
 */
void init_fcu_coefficients(fcu_coefficients_s** h) {

    printf ("initializing the fcu impulse response coeff");


    (*h)->h_0 = (double)rand();
    while ((*h)->h_0 > 1.0) {
        (*h)->h_0 /= 10.0;
    }
    (*h)->h_1 = (double)rand();
    while ((*h)->h_1 > 1.0) {
        (*h)->h_1 /= 10.0;
    }
    (*h)->h_2 = (double)rand();
    while ((*h)->h_2 > 1.0) {
        (*h)->h_2 /= 10.0;
    }
    

    (*h)->h_01 = (*h)->h_0+(*h)->h_1;
    (*h)->h_12 = (*h)->h_1+(*h)->h_2;
    (*h)->h_012 = (*h)->h_0+(*h)->h_1+(*h)->h_2;
}

//print all the pixel data in the image
void print_image_pixels(double* pixels, int size) {
    if (pixels == NULL) {
        printf("Pixels are NULL\n");
        return;
    }
    printf("\n\t");
    for (int i = 0; i < 5; i++){
        printf("--------");
    }
    printf("Image Pixels");
    for (int i = 0; i < 4; i++){
        printf("--------");
    }
    printf("-----\n\t|\t");
    for (int i = 0; i < size * size; i++) {
        printf("%d \t", (int)pixels[i]);
        if ((i + 1) % size == 0 && i != (size * size - 1)) {
            printf("|\n\t|\t");
        }
    }
    printf("|");

    printf("\n\t");
    for (int i = 0; i < 5; i++){
        printf("--------");
    }
    printf("Image Pixels");
    for (int i = 0; i < 4; i++){
        printf("--------");
    }
    printf("-----");
}

/**
 * Function that will initialize the testing pixel data with random values
 * 
 * @param size the width of the image in pixels.
 * 
 * Stored as an array that is size^2 long
 */
double* init_pixel_inputs(int size) {
    double* pixels = (double*)malloc(size * size * sizeof(double));
    if (pixels == NULL) {
        fprintf(stderr, "Memory allocation failed for pixel inputs\n");
        exit(EXIT_FAILURE);
    }

    //mod by 255 since pixels are 8-bit values
    for (int i = 0; i < size * size; i++) {
        pixels[i] = (double)(rand() % 255);
    }

    return pixels;

}

/**
 * Function that will parse the overall pixel data and output three new values to the fcu_inputs_s struct
 * 
 * @param inputs Pointer to the fcu_inputs_s structure where the next input set will be stored.
 */

void grab_next_ip_set(fcu_inputs_s* inputs) {
    // Check if inputs is NULL
    if (inputs == NULL) {
        fprintf(stderr, "Inputs pointer is NULL\n");
        return;
    }

    //pointer arithmetic. Grab the next three by setting the poiunters three ahead of their original spot
    inputs->x_0 = inputs->x_0 + 3; 
    inputs->x_1 = inputs->x_1 + 3;
    inputs->x_2 = inputs->x_2 + 3;
}


void print_fcu_outputs(fcu_outputs_s* outputs, int starting, int ending, int idx) {
    if (outputs == NULL) {
        printf("Outputs are NULL\n");
        return;
    }

    int header_dash_count = 6; // Number of dashes in the header

    if (starting) {
        printf("\n\n\t\t\tOutputs\n");
        printf("\t");
        for (int i = 0; i < header_dash_count; i++){
            printf("--------");
        }
        printf("-\n");
    }
    
    if (!starting && !ending) {
        printf("%d:\t| Y_0: %.2f \t| Y_1: %.2f \t| Y_2: %.2f \t|\n", idx, outputs->y_0, outputs->y_1, outputs->y_2);
    }

    if (ending) {
        printf("\t");
        for (int i = 0; i < header_dash_count; i++){
            printf("--------");
        }
        printf("-\n");
    }
}

void print_shift_reg(queue_s* queue) {
    if (queue == NULL) {
        printf("Shift Reg is NULL\n");
        return;
    }
    printf("\n\t\t\t\tShift Register %c:\n", queue->name);
    printf("\t\t----------------------------------------------\n");
    printf("\t\t| %f | --> | %f | --> | %f |\n", queue->tail->data, queue->middle->data, queue->head->data);
    printf("\t\t----------------------------------------------\n");
}

void printSimulatorStartMessage() {
    printf("\n");
    printf("  #####################################################\n");
    printf("  #                                                   #\n");
    printf("  #                                                   #\n");
    printf("  #        Launching CNN Simulator v1.0               #\n");
    printf("  #                                                   #\n");
    printf("  #                                                   #\n");
    printf("  #####################################################\n");
    printf("\n");
}