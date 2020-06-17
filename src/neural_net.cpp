/*
 * neural network evaluation function
 *
 * Author: Kimberly Chan
 * Organization: Mesbah Lab at UC Berkeley
 */

#include "neural_net.h"
#include "weights.h"

void neural_net(data_t input[NUM_INPUTS], data_t output[NUM_OUTPUTS], bool scale01) {
	// initialize output to zero
	for (int i=0; i<NUM_OUTPUTS; i++) {
		output[i] = 0;
	}

	// scale data to range(0,1)
	// initialize container for output
	data_t scaled_x[NUM_INPUTS] = {0};
	data_t new_max = 1;
	data_t new_min = 0;

	if (scale01) {
		for (int i=0; i<NUM_INPUTS; i++) {
			scaled_x[i] = (new_max - new_min) * (input[i] - xscale_min[i])/(xscale_max[i] - xscale_min[i]) + new_min;
		}
	} else {
		for (int i=0; i<NUM_INPUTS; i++) {
			scaled_x[i] = input[i];
		}
	}

	// scale data to range(-1, 1)
	data_t scaled_x2[NUM_INPUTS] = {0};
	data_t max = 1;
	data_t min = 0;
	new_min = -1;

	for (int i=0; i<NUM_INPUTS; i++) {
		scaled_x2[i] = (new_max - new_min) * (scaled_x[i] - min)/(max - min) + new_min;
	}

	// input layer
    // initialize container for layer output
	data_t out1[NUM_NODES] = {0};

	// perform affine transformation (X@W + b)
	for (int i=0; i<NUM_NODES; i++) {
		for (int j=0; j<NUM_INPUTS; j++) {
			out1[i] += scaled_x2[j] * input_weights[i][j];
		}
		out1[i] += input_biases[i];

		//perform activation (ReLU)
		out1[i] = relu(out1[i]);
	}

	// hidden layer 1
//	data_t *out2 = h_layer(out1, hlayer1_weights, hlayer1_biases);
    // initialize container for layer output
	data_t out2[NUM_NODES] = {0};

	// perform affine transformation (X@W + b)
	for (int i=0; i<NUM_NODES; i++) {
		for (int j=0; j<NUM_NODES; j++) {
			out2[i] += out1[j] * hlayer1_weights[i][j];
		}
		out2[i] += hlayer1_biases[i];

		//perform activation (ReLU)
		out2[i] = relu(out2[i]);
	}

	// hidden layer 2
//	data_t *out3 = h_layer(out2, hlayer2_weights, hlayer2_biases);
    // initialize container for layer output
	data_t out3[NUM_NODES] = {0};

	// perform affine transformation (X@W + b)
	for (int i=0; i<NUM_NODES; i++) {
		for (int j=0; j<NUM_NODES; j++) {
			out3[i] += out2[j] * hlayer2_weights[i][j];
		}
		out3[i] += hlayer2_biases[i];

		//perform activation (ReLU)
		out3[i] = relu(out3[i]);
	}

	// hidden layer 3
//	data_t *out4 = h_layer(out3, hlayer3_weights, hlayer3_biases);
    // initialize container for layer output
	data_t out4[NUM_NODES] = {0};

	// perform affine transformation (X@W + b)
	for (int i=0; i<NUM_NODES; i++) {
		for (int j=0; j<NUM_NODES; j++) {
			out4[i] += out3[j] * hlayer3_weights[i][j];
		}
		out4[i] += hlayer3_biases[i];

		//perform activation (ReLU)
		out4[i] = relu(out4[i]);
	}

	// hidden layer 4
//	data_t *out5 = h_layer(out4, hlayer4_weights, hlayer4_biases);
    // initialize container for layer output
	data_t out5[NUM_NODES] = {0};

	// perform affine transformation (X@W + b)
	for (int i=0; i<NUM_NODES; i++) {
		for (int j=0; j<NUM_NODES; j++) {
			out5[i] += out4[j] * hlayer4_weights[i][j];
		}
		out5[i] += hlayer4_biases[i];

		//perform activation (ReLU)
		out5[i] = relu(out5[i]);
	}

	// hidden layer 5
	// perform affine transformation (X@W + b)
	for (int i=0; i<NUM_OUTPUTS; i++) {
		for (int j=0; j<NUM_NODES; j++) {
			output[i] += out5[j] * hlayer5_weights[i][j];
		}
		output[i] += hlayer5_biases[i];
	}

	// unscale output [-1,1] --> [0,1]
	for (int i=0; i<NUM_OUTPUTS; i++) {
		output[i] = (output[i] - new_min) * (max - min)/(new_max - new_min) + min;
	}

	// unscale output [0,1] --> [real values]
	if (scale01) {
		new_min = -1;
		for (int i=0; i<NUM_OUTPUTS; i++) {
			output[i] = (output[i] - new_min) * (yscale_max[i] - yscale_min[i])/(new_max - new_min) + yscale_min[i];
		}
	}
}

data_t relu(data_t z) {
	return z > (data_t) 0 ? z : 0;
}

//data_t * scale(data_t input[], data_t x_max[], data_t x_min[], data_t new_max, data_t new_min) {
//	// initialize container for output
//	data_t scaled_x[NUM_INPUTS] = {0};
//
//	for (int i=0; i<NUM_INPUTS; i++) {
//		scaled_x[i] = (new_max - new_min) * (input[i] - x_min[i])/(x_max[i] - x_min[i]) + new_min;
//	}
//	return scaled_x;
//}
//
//data_t * h_layer(data_t input[], data_t weights[NUM_NODES][NUM_NODES], data_t biases[NUM_NODES]) {
//
//	// initialize container for layer output
//	static data_t out2[NUM_NODES] = {0};
//
//	// perform affine transformation (X@W + b)
//	for (int i=0; i<NUM_NODES; i++) {
//		for (int j=0; j<NUM_NODES; j++) {
//			out2[i] += input[j] * weights[j][i];
//		}
//		out2[i] += biases[i];
//
//		//perform activation (ReLU)
//		out2[i] = relu(out2[i]);
//	}
//
//	return out2;
//}
