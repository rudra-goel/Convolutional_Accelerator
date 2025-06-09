#include <stdlib.h>
#include <stdio.h>

#include "fcu.h"

queue_s* fcu_1_shift_reg_1;
queue_s* fcu_1_shift_reg_2;

void print_fcu_outputs(fcu_outputs_s* outputs);
void print_shift_reg(queue_s* queue, char reg_name);

int main() {
    //initialize the shift registers
    init_shift_reg(&fcu_1_shift_reg_1);
    init_shift_reg(&fcu_1_shift_reg_2);

    fcu_inputs_s inputs = {1.0, 2.0, 3.0};
    fcu_coefficients_s kernel = {0.5, 0.25, 0.125};

    fcu_outputs_s* outputs = three_parallel_fcu(&inputs, &kernel, fcu_1_shift_reg_1, fcu_1_shift_reg_2);
    print_shift_reg(fcu_1_shift_reg_1, 'A');
    print_shift_reg(fcu_1_shift_reg_2, 'B');
    print_fcu_outputs(outputs);
}


void print_fcu_outputs(fcu_outputs_s* outputs) {
    if (outputs == NULL) {
        printf("Outputs are NULL\n");
        return;
    }
    printf("Outputs:\n");
    printf("y_0: \t%f\n", outputs->y_0);
    printf("y_1: \t%f\n", outputs->y_1);
    printf("y_2: \t%f\n", outputs->y_2);
}

void print_shift_reg(queue_s* queue, char reg_name) {
    if (queue == NULL) {
        printf("Shift Reg is NULL\n");
        return;
    }
    printf("\n\t\t\t\t\tShift Register %c:\n", reg_name);
    printf("\t\t\t----------------------------------------------\n");
    printf("\t\t\t| %f | --> | %f | --> | %f |\n", queue->tail->data, queue->middle->data, queue->head->data);
    printf("\t\t\t----------------------------------------------\n");
}