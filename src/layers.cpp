/***************

Author: Kimberly Chan
Organization: Mesbah Lab at UC Berkeley
****************/
#include <iostream>
using namespace std;
#include "layers.h"

class feedForward {
    float W[3][3], b[3];

public:
    void set_weights(float);
    void set_biases(float);
    float forward(float);
};

void feedForward::set_weights(float W_trained[3][3]) {
    for (int i=0; i<3; i++) {
        for (int j=0; j<3; j++) {
            W[i][j] = W_trained[i][j];
        }
    }
}

void feedForward::set_biases(float b_trained[3]) {
    for (int i=0; i<3; i++) {
        b[i] = b_trained[i];
    }
}

float feedForward::forward(float input) {
    float out[3][3];
    // initialize output matrix elements to zero
    for (int i=0; i<3; i++) {
        for (int j=0; j<3; j++) {
            out[i][j] = 0;
        }
    }

    for (int i=0; i<3; i++) {
        for (int j=0; j<3; j++) {
            for (int k=0; k<3; k++) {
                out[i][j] += input[i][k] * W[k][j];
            }
            out[i][j] += b[i];
        }
    }

    for (int i=0; i<3; i++) {
        for (int j=0; j<3; j++) {
            out[i][j] = relu(out[i][j]);
        }
    }
    return out;
}
