# NN_FPGA

Project files for a neural network (NN) implementation on an FPGA using Vivado HLS. Implementation is based on the NN trained by Angelo at [this repository](https://github.com/adbonzanini/DNN-MPC-Plasma).

## Project Files

The 'src' directory provides the C/C++ files for use in Vivado HLS to synthesis a design for implementation.

The 'matlab' directory provides:
  - MATLAB code to convert trained NN data into text files.
  - MATLAB code to generate test bench data for HLS synthesis.
  - output text files generated from the previous two points to be used in the HLS synthesis.

The 'old_files' directory contains incomplete files generated previously and were not used in implementation.
