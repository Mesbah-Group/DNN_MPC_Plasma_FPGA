############################################################
## This file is generated automatically by Vivado HLS.
## Please DO NOT edit it.
## Copyright (C) 1986-2019 Xilinx, Inc. All Rights Reserved.
############################################################
open_project neural_net
set_top nn_accel
add_files neural_net/solution1_no_optm/neural_net.cpp
add_files neural_net/solution1_no_optm/neural_net.h
add_files neural_net/solution1_no_optm/weights.h
add_files -tb neural_net/solution1_no_optm/neural_net_test.cpp -cflags "-Wno-unknown-pragmas" -csimflags "-Wno-unknown-pragmas"
open_solution "solution1_no_optm"
set_part {xc7z020-clg400-1}
create_clock -period 10 -name default
config_schedule -effort medium -enable_dsp_full_reg=0 -relax_ii_for_timing -verbose=0
config_compile -name_max_length 20 -no_signed_zeros=0 -pipeline_loops 0 -unsafe_math_optimizations=0
config_interface -clock_enable=0 -expose_global=0 -m_axi_addr64=0 -m_axi_offset off -register_io off -trim_dangling_port=0
config_rtl -encoding onehot -kernel_profile=0 -module_auto_prefix=0 -mult_keep_attribute=0 -reset control -reset_async=0 -reset_level high -verbose=0
config_export -format ip_catalog -rtl verilog -vivado_optimization_level 2 -vivado_phys_opt place -vivado_report_level 0
config_sdx -optimization_level none -target none
source "./neural_net/solution1_no_optm/directives.tcl"
csim_design
csynth_design
cosim_design
export_design -rtl verilog -format ip_catalog
