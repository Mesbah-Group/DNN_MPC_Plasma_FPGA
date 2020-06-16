/*
 * neural network evaluation function
 *
 * Author: Kimberly Chan
 * Organization: Mesbah Lab at UC Berkeley
 */

#include <stdio.h>
#include "neural_net.h"
//#include "hls_linear_algebra.h"

void neural_net(data_t input[NUM_INPUTS], data_t output[NUM_OUTPUTS]) {
	// input layer
	// scale data to range(0,1)
	input = scale(input, xscale_max, xscale_min, 1, 0);
	// scale data to range(-1, 1)
	data_t max[NUM_INPUTS] = {1,1,1};
	data_t min[NUM_INPUTS] = {1,1,1};
	input = scale(input, max, min, 1, -1);

    // initialize container for layer output
	data_t out1[NUM_NODES] = {0};

	// perform affine transformation (X@W + b)
	for (int i=0; i<NUM_NODES; i++) {
		for (int j=0; j<NUM_INPUTS; j++) {
			out1[i] += input[j] * input_weights[j][i];
		}
		out1[i] += input_biases[i];

		//perform activation (ReLU)
		out1[i] = relu(out1[i]);
	}

	// hidden layer 1
	data_t *out2 = h_layer(out1, hlayer1_weights, hlayer1_biases);

	// hidden layer 2
	data_t *out3 = h_layer(out2, hlayer2_weights, hlayer2_biases);

	// hidden layer 3
	data_t *out4 = h_layer(out3, hlayer3_weights, hlayer3_biases);

	// hidden layer 4
	data_t *out5 = h_layer(out4, hlayer4_weights, hlayer4_biases);

	// initialize output to zero
	for (int i=0; i<NUM_OUTPUTS; i++) {
		output[i] = 0;
	}

	// perform affine transformation (X@W + b)
	for (int i=0; i<NUM_OUTPUTS; i++) {
		for (int j=0; j<NUM_NODES; j++) {
			output[i] += out5[j] * hlayer5_weights[j][i];
		}
		output[i] += hlayer5_biases[i];

		// perform activation (ReLU)
		output[i] = relu(output[5]);
	}
}

data_t relu(data_t z) {
	return z > (data_t) 0 ? z : 0;
}

data_t * scale(data_t input[], data_t x_max[], data_t x_min[], data_t new_max, data_t new_min) {
	// initialize container for output
	static data_t scaled_x[NUM_INPUTS] = {0};

	for (int i=0; i<NUM_INPUTS; i++) {
		scaled_x[i] = (new_max - new_min) * (input[i] - x_min[i])/(x_max[i] - x_min[i]) + new_min;
	}
	return scaled_x;
}

data_t * h_layer(data_t input[], data_t weights[NUM_NODES][NUM_NODES], data_t biases[NUM_NODES]) {

	// initialize container for layer output
	static data_t out2[NUM_NODES] = {0};

	// perform affine transformation (X@W + b)
	for (int i=0; i<NUM_NODES; i++) {
		for (int j=0; j<NUM_NODES; j++) {
			out2[i] += input[j] * weights[j][i];
		}
		out2[i] += biases[i];

		//perform activation (ReLU)
		out2[i] = relu(out2[i]);
	}

	return out2;
}
