/*
 * neural network software implementation header file
 *
 * Author: Kimberly Chan
 * Organization: Mesbah Lab at UC Berkeley
 */

#ifndef NN_SW_H
#define NN_SW_H

#include <stdbool.h>
//#include <math.h>

typedef float data_t;

#define NUM_LAYERS 5
#define NUM_NODES 6
#define NUM_INPUTS 3
#define NUM_OUTPUTS 2
#define N 1

void neural_net(data_t input[NUM_INPUTS], data_t output[NUM_OUTPUTS], bool scale01);
data_t relu(data_t z);

#endif
