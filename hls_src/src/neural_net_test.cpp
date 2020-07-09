/*
 * neural network test bench file
 *
 * Author: Kimberly Chan
 * Organization: Mesbah Lab at UC Berkeley
 */

#include <stdio.h>
#include <stdlib.h>
#include "neural_net.h"

#define EPS 0.00001

int main() {
	data_t test_input[NUM_INPUTS] = {0.329648, 0.721062, 0.496215};
	data_t test_output[NUM_OUTPUTS];
	data_t test_output2[NUM_OUTPUTS];
	float verified_output[NUM_OUTPUTS] = {0.136626, 0.758465};

	standalone_nn(test_input, test_output, false);

//	FILE* verified_output_file = fopen("~/Desktop/out.txt", "r");
//	if (verified_output_file == NULL) {
//		printf("Couldn't open file.");
//		exit(1);
//	}
//
//	for (int i=0; i<NUM_OUTPUTS; i++) {
//		fscanf(verified_output_file, "%f", &verified_output[i]);
//	}
	AXI_VAL in_stream[NUM_INPUTS];
	AXI_VAL out_stream[NUM_OUTPUTS];

	// stream in input
	for (int i=0; i<NUM_INPUTS; i++) {
		in_stream[i] = push_stream<float,4,5,5>(test_input[i], i==NUM_INPUTS-1);
	}

	// call DUT
	nn_accel(in_stream, out_stream);

	// extract output
	for (int i=0; i<NUM_OUTPUTS; i++) {
		test_output2[i] = pop_stream<float,4,5,5>(out_stream[i]);
	}

	int num_correct = 0;
	int total = 0;

	printf("Checking DNN Evaluation ... \n");

	for (int i=0; i<NUM_OUTPUTS; i++) {
		total++;
		float error = (float)test_output[i] - verified_output[i];
		float error2 = (float)test_output2[i] - verified_output[i];
		error = error * error;
		error2 = error2 * error2;
		if (error > EPS || error2 > EPS) {
			printf("The value at the %d index does not match!\n", i);
			printf("%f\n", error);
			printf("%f, %f\n", test_output[0], test_output[1]);
		} else {
			num_correct++;
		}
	}
	printf("Done! %d out of %d were correct.\n", num_correct, total);

	if (num_correct == total) {
		return 0;
	} else {
		return 1;
	}
}
