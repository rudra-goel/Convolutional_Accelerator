#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "fcu.h"

void print_fcu_outputs(fcu_outputs_s* outputs, int starting, int ending, int idx);
void print_shift_reg(queue_s* queue);
void grab_next_ip_set(fcu_inputs_s* inputs); 
double* init_pixel_inputs(int size);
void print_image_pixels(double* pixels, int size);


queue_s* fcu_1_shift_reg_1;
queue_s* fcu_1_shift_reg_2;

double* image_pixels;
fcu_coefficients_s kernel = {0.1, 0.1, 0.1};


int main(int argc, char* argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <image_size>\n", argv[0]);
        return EXIT_FAILURE;
    }
    
    //initialize the shift registers
    init_shift_reg(&fcu_1_shift_reg_1, 'A');
    init_shift_reg(&fcu_1_shift_reg_2, 'B');
    
    // Initialize pixel inputs
    int image_size = atoi(argv[1]);
    image_pixels = init_pixel_inputs(image_size);
    print_image_pixels(image_pixels, image_size);

    // Initialize pixel inputs with random values
    fcu_inputs_s inputs; 
    inputs.x_0 = (double*)malloc(sizeof(double));
    inputs.x_1 = (double*)malloc(sizeof(double));
    inputs.x_2 = (double*)malloc(sizeof(double));
    
    inputs.x_0 = image_pixels; 
    inputs.x_1 = image_pixels + 1; 
    inputs.x_2 = image_pixels + 2; 


    //load in the first three pixel values
    grab_next_ip_set(&inputs);
    fcu_outputs_s* outputs;
    print_fcu_outputs(outputs, 1, 0, 0);
    
    for(int i = 0; i < (image_size*image_size) / 3; i++) {
        outputs = three_parallel_fcu(&inputs, &kernel, fcu_1_shift_reg_1, fcu_1_shift_reg_2);
        
        if (DEBUG_SHIFT_REGISTER) {
            print_shift_reg(fcu_1_shift_reg_1);
            print_shift_reg(fcu_1_shift_reg_2);
        }
        print_fcu_outputs(outputs, 0, 0, i);
        //free the outputs after printing
        free(outputs);
        //grab the next set of inputs
        grab_next_ip_set(&inputs);

        usleep(10000);
    }

    print_fcu_outputs(outputs, 0, 1, 0);
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