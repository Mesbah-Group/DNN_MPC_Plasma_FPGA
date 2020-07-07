/*
 * neural network header file
 *
 * Author: Kimberly Chan
 * Organization: Mesbah Lab at UC Berkeley
 */
#ifndef NEURAL_NET_H
#define NEURAL_NET_H

//#include <stdlib.h>
//#include <math.h>

typedef float data_t;

#define NUM_LAYERS 5
#define NUM_NODES 6
#define NUM_INPUTS 3
#define NUM_OUTPUTS 2
#define N 1

// function prototypes
void neural_net(data_t input[NUM_INPUTS], data_t output[NUM_OUTPUTS], bool scale01);
void in_layer(data_t x[NUM_INPUTS], data_t out[NUM_NODES], data_t W[NUM_NODES][NUM_INPUTS], data_t b[NUM_NODES], int act);
void hidden_layer(data_t x[NUM_NODES], data_t out[NUM_NODES], data_t W[NUM_NODES][NUM_NODES], data_t b[NUM_NODES], int act);
void out_layer(data_t x[NUM_NODES], data_t out[NUM_OUTPUTS], data_t W[NUM_OUTPUTS][NUM_NODES], data_t b[NUM_NODES], int act);

// template functions
template <typename T>
T relu_template(T z) {
	return z > (T) 0 ? z : 0;
}

template <typename T, int DIM1, int DIM2>
void layer_template(T x[DIM1], T out[DIM2], T W[DIM2][DIM1], T b[DIM2], int act) {
	int i, j;
	// perform affine transformation (matrix multiplication)
	loop1: for (i=0; i<DIM2; i++) {
		out[i] = 0;
		loop2: for (j=0; j<DIM1; j++) {
			out[i] += x[j] * W[i][j];
		}
		out[i] += b[i];

		// perform activation
		// act == 1 is relu; otherwise linear
		if (act == 1) {
			out[i] = relu_template<T>(out[i]);
		}
	}
}
//data_t * scale(data_t input[], data_t x_max[], data_t x_min[], data_t new_max, data_t new_min);

#endif

//data_t xscale_max[NUM_INPUTS] = {3.968000, 18.640000, 2.000000};
//data_t xscale_min[NUM_INPUTS] = {-4.032000, -6.360000, 0.000000};
//data_t yscale_max[NUM_OUTPUTS] = {7, 3};
//data_t yscale_min[NUM_OUTPUTS] = {-1.200000, -1.500000};
//
//data_t input_weights[NUM_INPUTS][NUM_NODES] = {0.557561, 0.238126, -1.065138, 0.075556, 0.037525, -1.766544, -0.194898, -0.036449, -0.954724, 0.390840, 0.085885, 0.092333, 0.762900, 0.552193, -0.216761, -1.189162, 0.109874, 0.058024};
//data_t hlayer1_weights[NUM_NODES][NUM_NODES] = {0.066514, -2.161712, 0.146131, 0.169767, 0.048688, -0.022710, -0.923283, -0.895513, -0.387890, -0.549437, -0.812340, -0.363303, -1.135631, 1.124449, 0.402597, 0.355778, 0.922729, 0.226647, -0.077918, -0.841404, -0.958011, -0.464689, -0.602602, 0.110656, 0.238160, 0.031674, -0.724239, -0.774677, 1.226301, 0.053116, -0.502186, 0.468499, -0.701396, -0.152899, -0.397015, -0.871564};
//data_t hlayer2_weights[NUM_NODES][NUM_NODES] = {-0.977240, 0.657938, -0.972834, 0.235433, 0.522801, -0.005862, 1.474251, -1.149316, 0.155464, -0.365752, 0.074091, 0.095841, -0.140148, -0.151032, -0.961116, -0.640597, -0.769002, 0.451766, 0.700747, -0.411204, -0.489665, -0.454814, 0.211090, 0.447879, 0.877995, -0.443231, -0.245244, 0.409134, 0.599083, 0.880319, 0.880873, -0.688181, -0.905833, -0.785793, 0.963439, 0.203416};
//data_t hlayer3_weights[NUM_NODES][NUM_NODES] = {-0.778829, 1.744595, 0.794489, 0.956499, 0.941980, 0.353143, -0.076526, -0.826877, -0.849396, -0.823732, -0.106376, 0.431520, -0.256644, -0.801469, 0.228195, -0.714758, -0.118424, 0.324785, 0.627351, 0.210439, -0.891376, -0.664058, -0.262058, -0.251703, -0.981972, 0.494214, -0.718812, 0.724768, -0.901428, -0.991257, 0.019036, 0.751050, 0.333489, 0.300216, -0.114858, 0.061721};
//data_t hlayer4_weights[NUM_NODES][NUM_NODES] = {0.092048, -0.591548, -0.148950, -0.004035, -0.728449, -0.584791, 0.008160, -0.253381, 0.546699, 0.250105, 0.442019, -0.697678, -1.240249, -0.864905, -0.255065, 0.458144, -0.674143, 0.861596, -1.311777, -0.266194, -0.611869, -0.693283, -0.354811, 0.122650, -0.483488, -0.064922, 0.677451, 0.027155, -0.246795, 0.378483, -1.729767, -0.310943, -0.442797, 0.672852, 0.954071, -0.366224};
//data_t hlayer5_weights[NUM_NODES][NUM_OUTPUTS] = {-0.224257, -0.724718, -1.264875, 0.506074, -0.094843, -0.430125, 0.934389, 0.457247, -1.079528, -1.655030, -0.571797, 2.047846};
//
//data_t input_biases[NUM_NODES] = {-0.382213, 0.920598, 0.868042, -0.607707, 1.470846, 0.415163};
//data_t hlayer1_biases[NUM_NODES] = {0.331645, 0.932554, 0.304799, -0.659910, -0.021965, 0.720167};
//data_t hlayer2_biases[NUM_NODES] = {-0.625293, -0.757253, 0.838254, -0.865220, 0.951924, 0.700119};
//data_t hlayer3_biases[NUM_NODES] = {-1.064158, -0.717241, -0.622397, 0.471355, -0.478013, -0.779858};
//data_t hlayer4_biases[NUM_NODES] = {-0.324831, -0.682999, 1.201774, 1.281333, -0.053052, 1.284027};
//data_t hlayer5_biases[NUM_OUTPUTS] = {0.999184, 0.802333};
