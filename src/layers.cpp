/***************

Author: Kimberly Chan
Organization: Mesbah Lab at UC Berkeley
****************/
#include <iostream>
using namespace std;
#include "layers.h"

class feedForward {
    float W, b;

public:
    void set_weights(float);
    void set_biases(float);
    float forward(float);
};

void feedForward::set_weights(float W_trained) {
    for (int i=0; i<3; i++) {
        for (int j=0; j<3; j++) {
            W[i][j] = W_trained[i][j];
        }
    }
}

void feedForward::set_biases(float b_trained) {
    for (int i=0; i<3; i++) {
        b[i] = b_trained[i];
    }
}
