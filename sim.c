#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include "fcu.h"

kernel_s* init_kernel(kernel_s* kernel);
fcu_coefficients_s* init_fcu_coefficients(fcu_coefficients_s* h);
fcu_s* init_fcu(fcu_s* fcu, char* fcu_name);
void grab_next_ip_set(fcu_inputs_s* inputs); 
int init_pixel_inputs(int size);
int slide_inputs(fcu_s* fcu);


void printSimulatorStartMessage();
void print_kernel(kernel_s* kernel);
void print_fcu_outputs(fcu_outputs_s* outputs, int starting, int ending, int idx);
void print_shift_reg(queue_s* queue);
void print_image_pixels(double* pixels, int size);
void print_current_input_set();



double* image_pixels;
kernel_s* kernel;
int image_size;

//create an array of pointers to three parallel FCUs
fcu_s* fcu_array[3];


int main(int argc, char* argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <image_size>\n", argv[0]);
        return EXIT_FAILURE;
    }


    printSimulatorStartMessage();

    //initialize the kernel - ideally read from a file as ip without recompilation
    kernel = init_kernel(kernel);

    print_kernel(kernel);

    // Initialize pixel inputs
    image_size = atoi(argv[1]);

    image_size = init_pixel_inputs(image_size);
    print_image_pixels(image_pixels, image_size);

    //initialize each FCU to have inputs, ptr to kernel, shift regs, and op struct
    for (int i = 0; i < 3; i++) {
        char* name = (char*)malloc(7 * sizeof(char));
        // name = "FCU - ";  
        // char* tmp; *tmp = (char)(i);
        // strcat(name, tmp);

        fcu_array[i] = init_fcu(fcu_array[i], name);
    }

    //Each FCU has a set of FIR filter coefficients. These coefficients are stored in the variable 'kernel'
    //Basically assign each FCU's 'h' var to point to the correct set of filter coefficients
    fcu_array[0]->h = kernel->kernel_row_1;
    fcu_array[1]->h = kernel->kernel_row_2;
    fcu_array[2]->h = kernel->kernel_row_3;

    // assign inputs to the first set of image pixels
    //assign the first set of three pointers to the inputs struct
    //for each subsequent FCU, the ptrs to the inputs are the base plus the dimension offset for x_0

    for (int i = 0; i < 3; i++) {
        fcu_array[i]->inputs->x_0 = image_pixels + (image_size*i);
        fcu_array[i]->inputs->x_1 = image_pixels+1+(image_size*i);
        fcu_array[i]->inputs->x_2 = image_pixels+2+(image_size*i);
    }


    if (DEBUG_INPUT_ASSIGNEMNT) {
        printf("\n\nBEGIN Initial input assignments to FCUs\n");
        for (int i = 0; i < 3; i++) {
            printf("\tFCU #%d: ", i+1);
            printf("%.2f\t%.2f\t%.2f\n", *(fcu_array[i]->inputs->x_0),
                *(fcu_array[i]->inputs->x_1),
                *(fcu_array[i]->inputs->x_2));
        }
        printf("END Initial input assignments to FCUs\n");
    }
    //call the FCU algorithm on the input set
    int counter = 0;
    //slide the inputs over by the stride amount
    do {
        counter = counter+1;
        //call the FCU pipeline 

        //print the current inputs
        if (DEBUG_INPUT_ASSIGNEMNT) {
            printf("\nInput assignments to FCUs\n");
            for (int i = 0; i < 3; i++) {
                printf("\tFCU #%d: ", i+1);
                printf("%.2f\t%.2f\t%.2f\n", *(fcu_array[i]->inputs->x_0),
                    *(fcu_array[i]->inputs->x_1),
                    *(fcu_array[i]->inputs->x_2));
            }
            printf("Input assignments to FCUs\n");
        }

        if (DEBUG_INPUT_SLIDING) {
            printf("\t\t\tInput set %d\n", counter);
            print_current_input_set();
        }


    } while(slide_inputs(fcu_array[0]) &&
            slide_inputs(fcu_array[1]) &&
            slide_inputs(fcu_array[2]));

    
}

/**
 * Function to shift the inputs to the FCU over by the stride amount
 * 
 * We know when we will reach the end of a row for a FCU by finding the difference in their pointers
 * Since the pixel is stored in contiguous memory
 * 
 * 
 */
int slide_inputs(fcu_s* fcu) {
    //find the difference between the addr-of third input and the addr of the first pixel
    double* third_input = fcu->inputs->x_2;
    int diff = third_input - image_pixels + 1;

    //reached end of a row
    if (diff % (image_size) == 0) {

        //need to detect if we've reached the final row set for the entire image
        if (diff + (image_size*(STRIDE-1) + 1) > image_size*image_size) {
            return 0;
        }

        //if not reached end, then slide kernel as normal
        fcu->inputs->x_0 = fcu->inputs->x_0 + STRIDE + (image_size*(STRIDE-1));
        fcu->inputs->x_1 = fcu->inputs->x_1 + STRIDE + (image_size*(STRIDE-1));
        fcu->inputs->x_2 = fcu->inputs->x_2 + STRIDE + (image_size*(STRIDE-1));

    } else {
        // in the middle of a row so slide as normal
        fcu->inputs->x_0 = fcu->inputs->x_0 + STRIDE;
        fcu->inputs->x_1 = fcu->inputs->x_1 + STRIDE;
        fcu->inputs->x_2 = fcu->inputs->x_2 + STRIDE;
    }

    return 1;
}

//initialize an FCU
fcu_s* init_fcu(fcu_s* fcu, char* fcu_name) {
    //create a pointer to a fcu_s structure and assign it to the the value of the pointer passed into the arg
    fcu = (fcu_s*)malloc(sizeof(fcu_s));
    
    if (fcu == NULL) {
        fprintf(stderr, "Memory allocation failed for FCU\n");
        exit(EXIT_FAILURE);
    }

    // init the inpiuts struct
    (fcu)->inputs = (fcu_inputs_s*)malloc(sizeof(fcu_inputs_s));
    if ((fcu)->inputs == NULL) {
        fprintf(stderr, "Memory allocation failed for FCU inputs\n");
        exit(EXIT_FAILURE);
    }
    
    // Initialize coefficients
    (fcu)->h = (fcu_coefficients_s*)malloc(sizeof(fcu_coefficients_s));
    if ((fcu)->h == NULL) {
        fprintf(stderr, "Memory allocation failed for FCU coefficients\n");
        exit(EXIT_FAILURE);
    }
    
    // Initialize shift regs
    init_shift_reg(&((fcu)->shift_reg_1), strcat(fcu_name, "_sr_a"));
    init_shift_reg(&((fcu)->shift_reg_2), strcat(fcu_name, "_sr_b"));

    // Initialize outputs struct
    (fcu)->outputs = (fcu_outputs_s*)malloc(sizeof(fcu_outputs_s));
    if ((fcu)->outputs == NULL) {
        fprintf(stderr, "Memory allocation failed for FCU outputs\n");
        exit(EXIT_FAILURE);
    }

    return fcu;
}

/**
 * Print the kernel in a nice format
 * 
 * Assumes the kernel struct has already been initialized
 */
 void print_kernel(kernel_s* kernel) {
    if (kernel == NULL ||
        (*kernel).kernel_row_1 == NULL ||
        (*kernel).kernel_row_2 == NULL ||
        (*kernel).kernel_row_3 == NULL) {
            printf("Kernel not initizialed properly.\nCould not print\n");
    }

    printf("**************** Kernel ****************\n");
    printf("%f\t", (*kernel).kernel_row_1->h_0);
    printf("%f\t", (*kernel).kernel_row_1->h_1);
    printf("%f\t", (*kernel).kernel_row_1->h_2);
    printf("\n");
    printf("%f\t", (*kernel).kernel_row_2->h_0);
    printf("%f\t", (*kernel).kernel_row_2->h_1);
    printf("%f\t", (*kernel).kernel_row_2->h_2);
    printf("\n");
    printf("%f\t", (*kernel).kernel_row_3->h_0);
    printf("%f\t", (*kernel).kernel_row_3->h_1);
    printf("%f\t", (*kernel).kernel_row_3->h_2);
    printf("\n");
    printf("****************************************\n");
 }

/**
 * Initialize the kernel
 */
kernel_s* init_kernel(kernel_s* kernel) {
    kernel = (kernel_s*)malloc(sizeof(kernel_s*));

    if (kernel == NULL) {
        fprintf(stderr, "Memory allocation failed for kernel\n");
        exit(EXIT_FAILURE);
    }
    
    // (kernel)->kernel_row_1 = (fcu_coefficients_s*)malloc(sizeof(fcu_coefficients_s*));
    // (kernel)->kernel_row_3 = (fcu_coefficients_s*)malloc(sizeof(fcu_coefficients_s*));
    // (kernel)->kernel_row_2 = (fcu_coefficients_s*)malloc(sizeof(fcu_coefficients_s*));
    
    // if ((kernel)->kernel_row_1 == NULL || 
    //     (kernel)->kernel_row_2 == NULL ||
    //     (kernel)->kernel_row_3 == NULL) {
        
    //     fprintf(stderr, "Memory allocation failed for kernel row vectors\n");
    //     exit(EXIT_FAILURE);
    // }

    //pass a pointer to a ptr for each kernel's row vector
    (kernel)->kernel_row_1 = init_fcu_coefficients(((kernel)->kernel_row_1));
    (kernel)->kernel_row_2 = init_fcu_coefficients(((kernel)->kernel_row_2));
    (kernel)->kernel_row_3 = init_fcu_coefficients(((kernel)->kernel_row_3));
    
    return kernel;
}

/**
 * initialize a row vector in the kernel
 */
fcu_coefficients_s* init_fcu_coefficients(fcu_coefficients_s* h) {

    printf ("initializing the fcu impulse response coeff\n");

    h = (fcu_coefficients_s*)malloc((sizeof(fcu_coefficients_s)));
    
    if (h == NULL) {
        fprintf(stderr, "Mem alloc for Fcu Coefficients failed\n");
        exit(EXIT_FAILURE);
    }

    (h)->h_0 = (double)rand();
    while ((h)->h_0 > 1.0) {
        (h)->h_0 /= 10.0;
    }
    (h)->h_1 = (double)rand();
    while ((h)->h_1 > 1.0) {
        (h)->h_1 /= 10.0;
    }
    (h)->h_2 = (double)rand();
    while ((h)->h_2 > 1.0) {
        (h)->h_2 /= 10.0;
    }
    

    (h)->h_01 = (h)->h_0+(h)->h_1;
    (h)->h_12 = (h)->h_1+(h)->h_2;
    (h)->h_012 = (h)->h_0+(h)->h_1+(h)->h_2;

    return h;
}

//print all the pixel data in the image
void print_image_pixels(double* pixels, int size) {

    
    if (pixels == NULL) {
        printf("Pixels are NULL\n");
        return;
    }
    if (DEBUG_IMAGE_PIXELS) {
        printf("Begin printing raw image pixels\n");
        
        for(int i = 0; i < size * size; i++) {
            printf("%d:\t%f\n", i, pixels[i]);
            if ((i+1) % size == 0 && i != 0) {
                printf("\n\n");
            }
        }

        printf("Done printing raw image pixels\n");
    } else {
        printf("\n\t");
        // for (int i = 0; i < 5; i++){
        //     printf("--------");
        // }
        printf("Image Pixels\n");
        // for (int i = 0; i < 4; i++){
        //     printf("--------");
        // }
        printf("\n\t|\t");
        for (int i = 0; i < size * size; i++) {
            printf("%d \t", (int)pixels[i]);
            if ((i + 1) % size == 0 && i != (size * size - 1)) {
                printf("|\n\t|\t");
            }
        }
        // printf("|");
        // printf("\n\t");
        // for (int i = 0; i < 5; i++){
        //     printf("--------");
        // }
        printf("|\n\n\tImage Pixels\n");
        // for (int i = 0; i < 4; i++){
        //     printf("--------");
        // }
        // printf("-----");
    }
}

/**
 * Function that will initialize the testing pixel data with random values
 * 
 * @param size the width of the image in pixels.
 * 
 * Stored as an array that is size^2 long
 */
int init_pixel_inputs(int size) {

    //first determine an overall image size that is a multiple of the stride value
    int new_size = size;
    while (new_size % STRIDE != 0) {
        new_size = new_size + 1;
    }

    //keep track of how many zeros we need to pad for right align and bottom rown
    int padding_depth = new_size - size;

    // double* pixels = (double*)malloc(new_size * new_size * sizeof(double));
    image_pixels = (double*)malloc(new_size * new_size * sizeof(double));
    double* pixels = image_pixels;

    if (pixels == NULL) {
        fprintf(stderr, "Memory allocation failed for pixel inputs\n");
        exit(EXIT_FAILURE);
    }

    int counter = 0;

    //mod by 255 since pixels are 8-bit values
    for (int i = 0; i < new_size * new_size; i++) {
        //mem array is at the right edge of the original sizing
        if (counter == size) {
            //add zeros for padding_depth length
            int x;
            for (x = i; x < i + padding_depth; x++) {
                pixels[x] = 0.0;
            }
            //update i to be the correct position in the overall image's memory
            i = x;
            counter = 0;
        }
        double tmp = (double)(rand() % 255);
        pixels[i] = tmp == 0 ? (double)(rand() % 255) : tmp;
        counter = counter + 1;
    }

    return new_size;

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
    printf("  ################################################\n");
    printf("  #                                              #\n");
    printf("  #                                              #\n");
    printf("  #        Launching CNN Simulator               #\n");
    printf("  #                                              #\n");
    printf("  #                                              #\n");
    printf("  ################################################\n");
    printf("\n");
}

void print_current_input_set() {
    if (fcu_array == NULL) return;

    //print first row inputs
    printf("%.2f\t", *(fcu_array[0]->inputs->x_0));
    printf("%.2f\t", *(fcu_array[0]->inputs->x_1));
    printf("%.2f\t", *(fcu_array[0]->inputs->x_2));
    
    printf("\t\t");

    //print first row kernel
    printf("%.2f\t", kernel->kernel_row_1->h_0);
    printf("%.2f\t", kernel->kernel_row_1->h_1);
    printf("%.2f\t", kernel->kernel_row_1->h_2);

    printf("\n\n");
    
    //print second row inputs
    printf("%.2f\t", *(fcu_array[1]->inputs->x_0));
    printf("%.2f\t", *(fcu_array[1]->inputs->x_1));
    printf("%.2f\t", *(fcu_array[1]->inputs->x_2));
    
    printf("\t*\t");
    
    //print second row kernel
    printf("%.2f\t", kernel->kernel_row_1->h_0);
    printf("%.2f\t", kernel->kernel_row_1->h_1);
    printf("%.2f\t", kernel->kernel_row_1->h_2);
    
    printf("\n\n");

    //print third row inputs
    printf("%.2f\t", *(fcu_array[2]->inputs->x_0));
    printf("%.2f\t", *(fcu_array[2]->inputs->x_1));
    printf("%.2f\t", *(fcu_array[2]->inputs->x_2));
    
    printf("\t\t");

    //print third row kernel
    printf("%.2f\t", kernel->kernel_row_2->h_0);
    printf("%.2f\t", kernel->kernel_row_2->h_1);
    printf("%.2f\t", kernel->kernel_row_2->h_2);

    printf("\n\n");
    
}