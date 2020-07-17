############################################################
## This file is generated automatically by Vivado HLS.
## Please DO NOT edit it.
## Copyright (C) 1986-2019 Xilinx, Inc. All Rights Reserved.
############################################################
set_directive_interface -mode s_axilite "nn_accel"
set_directive_interface -mode axis -register -register_mode both "nn_accel" INPUT_STREAM
set_directive_interface -mode axis -register -register_mode both "nn_accel" OUTPUT_STREAM
set_directive_inline "neural_net"
set_directive_inline "pop_stream"
set_directive_inline "push_stream"
set_directive_inline "wrapped_NN"
set_directive_unroll "wrapped_NN/stream_in"
set_directive_unroll "wrapped_NN/stream_out"
set_directive_unroll "neural_net/scale01_loop1"
set_directive_unroll "neural_net/scale01_loop0"
set_directive_unroll "neural_net/scale11_loop"
set_directive_unroll "neural_net/unscale11_loop"
set_directive_unroll "neural_net/unscale01_loop1"
set_directive_unroll "neural_net/unscale01_loop0"
set_directive_inline "layer_template"
set_directive_inline "relu_template"
set_directive_pipeline "layer_template/loop1"
