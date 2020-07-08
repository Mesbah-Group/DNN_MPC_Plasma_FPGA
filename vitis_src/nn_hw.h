/*
 * header file for hardware setup
 *
 * Author: Kimberly Chan
 * Organization: Mesbah Lab at UC Berkeley
 */

#ifndef NN_HW_H
#define NN_HW_H

#include <stdbool.h>

#define NUM_LAYERS 5
#define NUM_NODES 6
#define NUM_INPUTS 3
#define NUM_OUTPUTS 2
#define N 1

void Start_HW_Accelerator();
int Setup_HW_Accelerator(float input[NUM_INPUTS], float output[NUM_OUTPUTS], bool scale01, int dma_size);
int Run_HW_Accelerator(float input[NUM_INPUTS], float output[NUM_OUTPUTS], bool scale01, int dma_size);


#endif
