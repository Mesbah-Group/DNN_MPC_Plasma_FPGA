# solution3

This file has `tcl` files for the base case optimizations tested. The list of optimizations include:

* HLS PIPELINE for the input and output streams of the wrapped neural network
* HLS UNROLL for the scaling loops inside of the neural network evaluation
* HLS INLINE for all subfunctions
* HLS PIPELINE for the layer template function
* HLS INTERFACE for designating ports on the custom IP
* HLS ARRAY_PARTITION for all of the weight and scale arrays
