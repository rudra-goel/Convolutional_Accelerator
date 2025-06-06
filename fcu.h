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
     node* head;
     node* tail;
     node* middle;
 } queue_s;

 