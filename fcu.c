#include <stdlib.h>

//Structs for the kernel, and input. 
typedef struct {
    int input[3];
} input_s;

typedef struct {
    int kernel[3][3];
} kernel_s;

typedef struct {
    int output[3];
} output_s;

//implement the 3-parallel FCU architecture
//takes in a signal of length three and a kernel of length three and puts it through the 3-parallel FCU architecture
output_s* FCU(kernel_s* kernel, int kernel_row, input_s* input) {
    //create a new output to hold the result
    output_s* result = (output_s*)malloc(sizeof(output_s));
    
    //the fast convolutional units are based on a 3x3 kernel size
    //thus, we will create a 3-parallel FCU architecture
    //the three parallel FCU architecutre is split into different stages based on the clock edge
    
    return result;
}

int multiplier (int a, int b) {
    //multiplier function to multiply two integers
    return a * b;
}

int adder(int a, int b) {
    //adder function to add two integers
    return a + b;
}



int main() {
    
}