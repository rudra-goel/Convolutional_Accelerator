
#ifndef FCU_H
#define FCU_H


 //struct for the shift register
 //queue implemented as a linked list with head and tail pointers
 typedef struct {
     double data;
     struct shift_reg_node_s* next;
 } shift_reg_node_s;

 typedef struct {
    char* name;
     shift_reg_node_s* head;
     shift_reg_node_s* tail;
     shift_reg_node_s* middle;
 } queue_s;

void print_shift_reg(queue_s* queue);


//debugs
#define DEBUG_SHIFT_REGISTER 0
#define DEBUG_IMAGE_PIXELS 0
#define DEBUG_INPUT_ASSIGNEMNT 0
#define DEBUG_INPUT_SLIDING 0
#define DEBUG_FEATURE_MAP 0
#define DEBUG_FCU_OUTPUTS 0
#endif 

#include <stdio.h>
#include <stdlib.h>

//define a struct for the inputs for the FCU

typedef struct {
    double* x_0;
    double* x_1; 
    double* x_2;
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

typedef struct {
    fcu_coefficients_s* kernel_row_1;
    fcu_coefficients_s* kernel_row_2;
    fcu_coefficients_s* kernel_row_3;
} kernel_s;


//struct for one Fast Convolutional Unit (FCU)
//this struct simulates one layer of a the physical hardware that is 1/3 or the 3-parallel fcu
//there will be three of these structs in the simulation, one for each row
typedef struct {
    fcu_inputs_s* inputs;
    fcu_coefficients_s* h;
    queue_s* shift_reg_1;
    queue_s* shift_reg_2;
    fcu_outputs_s* outputs;
} fcu_s;

/**
 * Constants for the simulation program.
 */

/**
 * STRIDE - This is the "length" of the image data
 * Images in memory, like any data, is stored as an array rather than a 2D
 * vector. As such, if the kernel (square) during the convolution layer wanted
 * to pass through the image from left to write, it would have to set 
 * three adjacent elements from adjacent rows as its inputs. 
 * 
 * In this image processer, the kernel is made from three 1D row vectors that
 * each have three elements
 * 
 * Since this architecture employs three FCUs running in parallel (one for
 * each row vector of the kernel convolved with a three element row vector from the image), each input set to the FCU cannot overlap inputs for the
 * other FCUs as the kernel slides over and convolves
 */
const static int STRIDE = 3;



double multiplier(double x_0, double h_0);
double adder(double x_0, double x_1);
void enqueue(queue_s* queue, double value);
double dequeue(queue_s* queue);
fcu_outputs_s* three_parallel_fcu(  fcu_inputs_s* inputs, 
                                    fcu_coefficients_s* kernel, 
                                    queue_s* shift_reg_1, 
                                    queue_s* shift_reg_2
                                    );

void init_shift_reg(queue_s** queue, char* name);