/*
 * neural network evaluation function
 *
 * Author: Kimberly Chan
 * Organization: Mesbah Lab at UC Berkeley
 */

#include "neural_net.h"
#include "weights.h"

void neural_net(data_t input[NUM_INPUTS], data_t output[NUM_OUTPUTS], bool scale01) {
//	// initialize output to zero
//	init_out: for (int i=0; i<NUM_OUTPUTS; i++) {
//		output[i] = 0;
//	}

	// scale data to range(0,1)
	// initialize container for output
	data_t scaled_x[NUM_INPUTS] = {0};
	data_t new_max = 1;
	data_t new_min = 0;

	if (scale01) {
		scale01_loop1: for (int i=0; i<NUM_INPUTS; i++) {
			scaled_x[i] = (new_max - new_min) * (input[i] - xscale_min[i])/(xscale_max[i] - xscale_min[i]) + new_min;
		}
	} else {
		scale01_loop0: for (int i=0; i<NUM_INPUTS; i++) {
			scaled_x[i] = input[i];
		}
	}

	// scale data to range(-1, 1)
	data_t scaled_x2[NUM_INPUTS] = {0};
	data_t max = 1;
	data_t min = 0;
	new_min = -1;

	scale11_loop: for (int i=0; i<NUM_INPUTS; i++) {
		scaled_x2[i] = (new_max - new_min) * (scaled_x[i] - min)/(max - min) + new_min;
	}


    // initialize containers for layer outputs
	data_t out1[NUM_NODES], out2[NUM_NODES], out3[NUM_NODES];
	data_t out4[NUM_NODES], out5[NUM_NODES], out6[NUM_OUTPUTS];

	// input layer
	in_layer(scaled_x2, out1, input_weights, input_biases, 1);

	// hidden layer 1
	hidden_layer(out1, out2, hlayer1_weights, hlayer1_biases, 1);

	// hidden layer 2
	hidden_layer(out2, out3, hlayer2_weights, hlayer2_biases, 1);

	// hidden layer 3
	hidden_layer(out3, out4, hlayer3_weights, hlayer3_biases, 1);

	// hidden layer 4
	hidden_layer(out4, out5, hlayer4_weights, hlayer4_biases, 1);

	// hidden layer 5 (last layer)
	out_layer(out5, out6, hlayer5_weights, hlayer5_biases, 0);

	// unscale output [-1,1] --> [0,1]
	unscale11_loop: for (int i=0; i<NUM_OUTPUTS; i++) {
		out6[i] = (out6[i] - new_min) * (max - min)/(new_max - new_min) + min;
	}

	// unscale output [0,1] --> [real values]
	if (scale01) {
		new_min = -1;
		unscale01_loop1: for (int i=0; i<NUM_OUTPUTS; i++) {
			output[i] = (out6[i] - new_min) * (yscale_max[i] - yscale_min[i])/(new_max - new_min) + yscale_min[i];
		}
	} else {
		unscale01_loop0: for (int i=0; i<NUM_OUTPUTS; i++) {
			output[i] = out6[i];
		}
	}
}

void in_layer(data_t x[NUM_INPUTS], data_t out[NUM_NODES], data_t W[NUM_NODES][NUM_INPUTS], data_t b[NUM_NODES], int act) {
	layer_template<data_t, NUM_INPUTS, NUM_NODES>(x, out, W, b, act);
}
void hidden_layer(data_t x[NUM_NODES], data_t out[NUM_NODES], data_t W[NUM_NODES][NUM_NODES], data_t b[NUM_NODES], int act) {
	layer_template<data_t, NUM_NODES, NUM_NODES>(x, out, W, b, act);
}
void out_layer(data_t x[NUM_NODES], data_t out[NUM_OUTPUTS], data_t W[NUM_OUTPUTS][NUM_NODES], data_t b[NUM_NODES], int act) {
	layer_template<data_t, NUM_NODES, NUM_OUTPUTS>(x, out, W, b, act);
}
