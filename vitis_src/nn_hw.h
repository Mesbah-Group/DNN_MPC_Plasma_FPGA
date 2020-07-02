/*
 * header file for hardware setup
 *
 * Author: Kimberly Chan
 * Organization: Mesbah Lab at UC Berkeley
 */

#ifndef NN_HW_H
#define NN_HW_H

#include <stdbool.h>
#include "nn_sw.h"

int Setup_HW_Accelerator(float input[NUM_INPUTS], float output[NUM_OUTPUTS], bool scale01, int dma_size);
int Run_HW_Accelerator(float input[NUM_INPUTS], float output[NUM_OUTPUTS], bool scale01, int dma_size);


#endif
