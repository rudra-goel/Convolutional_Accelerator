#include <stdlib.h>
#include <stdio.h>

#include "fcu.h"

/**
 * Multiplier function that takes two double values and returns their product.
 *
 * @param x_0 The first double value.
 * @param h_0 The second double value.
 * @return The product of x_0 and h_0.
 */
double multiplier(double x_0, double h_0) {
    return x_0 * h_0;
}


/**
 * Adder function that takes two double values and returns their sum.
 *
 * @param x_0 The first double value.
 * @param h_0 The second double value.
 * @return The sum of x_0 and h_0.
 */
double adder(double x_0, double h_0) {
    return x_0 + h_0;
}



 /**
 * Function that simulates the three-parallel Fast Convolutional Unit (FCU)
 * This function takes inputs and coefficients, and performs the convolution of the two vectors via parallel FIR.
 *
 *
 * It is the combonatinal logic that implements the architecture of the FCU
 *
 * @param inputs Pointer to the fcu_inputs_s structure containing input values.
 * @param kernel Pointer to the fcu_coefficients_s structure containing coefficients.
 */
void three_parallel_fcu(fcu_inputs_s* inputs, fcu_coefficients_s* kernel) {
    double x0h0 = multiplier(inputs->x_0, kernel->h_0);
    double x1h1 = multiplier(inputs->x_1, kernel->h_1);
    double x2h2 = multiplier(inputs->x_2, kernel->h_2);
    double x0_plus_x1 = adder(inputs->x_0, inputs->x_1);
    double x1_plus_x2 = adder(inputs->x_1, inputs->x_2);

    //second layer of combinational logic
    double x0_plus_x1_h01 = multiplier(x0_plus_x1, kernel->h_01);
    double x1_plus_x2_h12 = multiplier(x1_plus_x2, kernel->h_12);
    double x0_plus_x1_plus_x2 = adder(x0_plus_x1, inputs->x_2);
    //enqueue x2h2 into the shift register (3x shift register)


    //third layer
    double x0_plus_x1_plus_x2_h012 = multiplier(x0_plus_x1_plus_x2, kernel->h_012);
    double x0_plus_x1_h01_minus_x1h1 = adder(x0_plus_x1_h01, ((-1)*x1h1));
    double x1_plus_x2_h12_plus_x1h2 = adder(x1_plus_x2_h12, x1h1);



}