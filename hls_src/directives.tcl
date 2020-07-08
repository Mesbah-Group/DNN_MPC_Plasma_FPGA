############################################################
## This file is generated automatically by Vivado HLS.
## Please DO NOT edit it.
## Copyright (C) 1986-2019 Xilinx, Inc. All Rights Reserved.
############################################################
set_directive_interface -mode s_axilite "neural_net"
set_directive_interface -mode axis -register -register_mode both "neural_net" input
set_directive_interface -mode axis -register -register_mode both "neural_net" output
set_directive_interface -mode s_axilite "neural_net" scale01
set_directive_unroll "neural_net/scale11_loop"
set_directive_unroll "neural_net/scale01_loop1"
set_directive_unroll "neural_net/scale01_loop0"
set_directive_unroll "neural_net/unscale11_loop"
set_directive_unroll "neural_net/unscale01_loop1"
set_directive_unroll "neural_net/unscale01_loop0"
set_directive_pipeline "layer_template/loop1"
set_directive_array_partition -type complete -dim 1 "layer_template" x
