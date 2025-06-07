#include <stdlib.h>
#include <stdio.h>

#include "fcu.h"

queue_s* fcu_1_shift_reg_1;
queue_s* fcu_1_shift_reg_2;

void print_fcu_outputs(fcu_outputs_s* outputs);

int main() {
    //initialize the shift registers

    init_shift_reg(fcu_1_shift_reg_1);
    init_shift_reg(fcu_1_shift_reg_2);

    fcu_inputs_s inputs = {1.0, 2.0, 3.0};
    fcu_coefficients_s kernel = {0.5, 0.25, 0.125};

    fcu_outputs_s* outputs = three_parallel_fcu(&inputs, &kernel, fcu_1_shift_reg_1, fcu_1_shift_reg_2);
    print_fcu_outputs(outputs);
}


void print_fcu_outputs(fcu_outputs_s* outputs) {
    if (outputs == NULL) {
        printf("Outputs are NULL\n");
        return;
    }
    printf("Outputs:\n");
    printf("y_0: %f\n", outputs->y_0);
    printf("y_1: %f\n", outputs->y_1);
    printf("y_2: %f\n", outputs->y_2);
}