/***************
header file to define activation functions

Author: Kimberly Chan
Organization: Mesbah Lab at UC Berkeley
****************/

#ifndef ACTIVATIONS_H
#define ACTIVATIONS_H

float relu(float a)
{
	return a > 0 ? a : 0;
}

#endif
