#include <stdlib.h>

//Structs for the kernel, and input. 
//each struct has a 3x3 array of integers.
//the fcu design is performed on 1D vectors or arrays


typedef struct {
    int square[3][3];
} square_s;

typedef struct {
    int output[3];
} output_s;

//implement the 3-parallel FCU architecture




//adder unit
//takes in two sqaures and adds them together via column-wise vector addition

*square_s adder(square_s* kernel, square_s* input) {
    //create a new square to hold the result
    square_s* result = (square_s*)malloc(sizeof(square_s));
    //loop through the columns of the squares
    for (int i = 0; i < 3; i++) {
        //loop through the rows of the squares
        for (int j = 0; j < 3; j++) {
            //add the two squares together
            result->square[i][j] = kernel->square[i][j] + input->square[i][j];
        }
    }
    return result;
}