/*
 * main.c: main function for testing neural network implementation
 *
 * This application configures UART 16550 to baud rate 9600.
 * PS7 UART (Zynq) is not initialized by this application, since
 * bootrom/bsp configures it to baud rate 115200
 *
 * ------------------------------------------------
 * | UART TYPE   BAUD RATE                        |
 * ------------------------------------------------
 *   uartns550   9600
 *   uartlite    Configurable only in HW design
 *   ps7_uart    115200 (configured by bootrom/bsp)
 *
 *
 */

#include <stdio.h>
#include "platform.h"
#include "xil_printf.h"
#include "xparameters.h"
#include "xaxidma.h"
#include "xtmrctr.h"
//#include "xneural_net_neural_net.h"
#include "xstatus.h"
#include "weights_sw.h"
#include "nn_sw.h"
#include "nn_hw.h"

/*********** MACROS ************/
#define DMA_DEV_ID XPAR_AXIDMA_0_DEVICE_ID
#define TIMER_DEV_ID XPAR_AXI_TIMER_0_DEVICE_ID


/*********** VARIABLE DEFINITIONS **********/
u16 DeviceId = DMA_DEV_ID;
XAxiDma AxiDma; // AXI DMA Instance
XAxiDma *InstancePtr = &AxiDma;
XTmrCtr timer_dev; // Timer Instance

/*********** INITIALIZE DMA ***********/
int init_dma() {
    XAxiDma_Config *CfgPtr;
    int status = XST_SUCCESS;

    // Initialize the XAxiDma device
    CfgPtr = XAxiDma_LookupConfig(DeviceId);
    if (!CfgPtr) {
    	xil_printf("No config found for %d\r\n", DeviceId);
    	return XST_FAILURE;
    }

    status = XAxiDma_CfgInitialize(InstancePtr, CfgPtr);
    if (status != XST_SUCCESS) {
    	xil_printf("Initialization failed %d\r\n", status);
    	return XST_FAILURE;
    }

    // check for scatter gather mode
    if (XAxiDma_HasSg(InstancePtr)) {
    	xil_printf("Device configured as SG mode\r\n");
    	return XST_FAILURE;
    }

    // Disable interrupts, use polling mode instead
    XAxiDma_IntrDisable(InstancePtr, XAXIDMA_IRQ_ALL_MASK, XAXIDMA_DEVICE_TO_DMA);
    XAxiDma_IntrDisable(InstancePtr, XAXIDMA_IRQ_ALL_MASK, XAXIDMA_DMA_TO_DEVICE);

    // Reset DMA
    XAxiDma_Reset(InstancePtr);
    while (!XAxiDma_ResetIsDone(InstancePtr));

    return XST_SUCCESS;
}


/*********** MAIN FUNCTION **********/
int main() {
	unsigned int init_time, curr_time, calibration;
	unsigned int begin_time, end_time;
	unsigned int run_time_sw, run_time_hw = 0;

	unsigned int dma_size = NUM_INPUTS * sizeof(float);

	float test_input[NUM_INPUTS] = {0};
	data_t test_in[NUM_INPUTS] = {0};
	data_t expected_output[NUM_OUTPUTS] = {0};
	float test_output[NUM_OUTPUTS] = {0};
	bool scale01 = 0;

    init_platform();

    print("\n\nHello World\n\r");

    int status;

    /*********** INITIALIZATIONS **********/
    status = init_dma();
    if (status != XST_SUCCESS) {
    	print("\rError: DMA init failed\n");
    	return XST_FAILURE;
    }

    //Xil_DCacheDisable(); // uncomment this as a last resort, which will avoid all cache-related issues, but at the expense of performance

    printf("setup of DMA completed\r\n");

    // Set up HW timer
    status = XTmrCtr_Initialize(&timer_dev, TIMER_DEV_ID);
    if (status != XST_SUCCESS) {
    	print("\rError: Timer setup failed\n");
    	return XST_FAILURE;
    }
    XTmrCtr_SetOptions(&timer_dev, TIMER_DEV_ID, XTC_ENABLE_ALL_OPTION);

    // Calibrate HW Timer
    XTmrCtr_Reset(&timer_dev, TIMER_DEV_ID);
    init_time = XTmrCtr_GetValue(&timer_dev, TIMER_DEV_ID);
    curr_time = XTmrCtr_GetValue(&timer_dev, TIMER_DEV_ID);
    calibration = curr_time - init_time;


    /*********** RUN SOFTWARE VERSION **********/
    printf("Running Software Implementation of Algorithm");
    XTmrCtr_Reset(&timer_dev, TIMER_DEV_ID);
    begin_time = XTmrCtr_GetValue(&timer_dev, TIMER_DEV_ID);

    neural_net(test_in, expected_output, scale01);

    end_time = XTmrCtr_GetValue(&timer_dev, TIMER_DEV_ID);
    run_time_sw = end_time - begin_time - calibration;
    printf("Expected Output: %f, %f\r\n", expected_output[0], expected_output[1]);
    printf("\rSW Runtime was %d cycles\n\r", run_time_sw);


    /*********** RUN HARDWARE VERSION **********/
    printf("Running Hardware Implementation of Algorithm");
    XTmrCtr_Reset(&timer_dev, TIMER_DEV_ID);
    begin_time = XTmrCtr_GetValue(&timer_dev, TIMER_DEV_ID);

    Setup_HW_Accelerator(test_input, test_output, scale01, dma_size);
    Run_HW_Accelerator(test_input, test_output, scale01, dma_size);

    end_time = XTmrCtr_GetValue(&timer_dev, TIMER_DEV_ID);
    run_time_hw = end_time - begin_time - calibration;
    printf("\rTest Output: %f, %f\n\r", test_output[0], test_output[1]);
    printf("\rHW (AXI DMA + Accelerator) Runtime is %d cycles\n\r", run_time_hw);

    cleanup_platform();
    return 0;
}

void neural_net(data_t input[NUM_INPUTS], data_t output[NUM_OUTPUTS], bool scale01) {
	// initialize output to zero
	for (int i=0; i<NUM_OUTPUTS; i++) {
		output[i] = 0;
	}

	// scale data to range(0,1)
	// initialize container for output
	data_t scaled_x[NUM_INPUTS] = {0};
	data_t new_max = 1;
	data_t new_min = 0;

	if (scale01) {
		for (int i=0; i<NUM_INPUTS; i++) {
			scaled_x[i] = (new_max - new_min) * (input[i] - xscale_min[i])/(xscale_max[i] - xscale_min[i]) + new_min;
		}
	} else {
		for (int i=0; i<NUM_INPUTS; i++) {
			scaled_x[i] = input[i];
		}
	}

	// scale data to range(-1, 1)
	data_t scaled_x2[NUM_INPUTS] = {0};
	data_t max = 1;
	data_t min = 0;
	new_min = -1;

	for (int i=0; i<NUM_INPUTS; i++) {
		scaled_x2[i] = (new_max - new_min) * (scaled_x[i] - min)/(max - min) + new_min;
	}

	// input layer
    // initialize container for layer output
	data_t out1[NUM_NODES] = {0};

	// perform affine transformation (X@W + b)
	for (int i=0; i<NUM_NODES; i++) {
		for (int j=0; j<NUM_INPUTS; j++) {
			out1[i] += scaled_x2[j] * input_weights[i][j];
		}
		out1[i] += input_biases[i];

		//perform activation (ReLU)
		out1[i] = relu(out1[i]);
	}

	// hidden layer 1
//	data_t *out2 = h_layer(out1, hlayer1_weights, hlayer1_biases);
    // initialize container for layer output
	data_t out2[NUM_NODES] = {0};

	// perform affine transformation (X@W + b)
	for (int i=0; i<NUM_NODES; i++) {
		for (int j=0; j<NUM_NODES; j++) {
			out2[i] += out1[j] * hlayer1_weights[i][j];
		}
		out2[i] += hlayer1_biases[i];

		//perform activation (ReLU)
		out2[i] = relu(out2[i]);
	}

	// hidden layer 2
//	data_t *out3 = h_layer(out2, hlayer2_weights, hlayer2_biases);
    // initialize container for layer output
	data_t out3[NUM_NODES] = {0};

	// perform affine transformation (X@W + b)
	for (int i=0; i<NUM_NODES; i++) {
		for (int j=0; j<NUM_NODES; j++) {
			out3[i] += out2[j] * hlayer2_weights[i][j];
		}
		out3[i] += hlayer2_biases[i];

		//perform activation (ReLU)
		out3[i] = relu(out3[i]);
	}

	// hidden layer 3
//	data_t *out4 = h_layer(out3, hlayer3_weights, hlayer3_biases);
    // initialize container for layer output
	data_t out4[NUM_NODES] = {0};

	// perform affine transformation (X@W + b)
	for (int i=0; i<NUM_NODES; i++) {
		for (int j=0; j<NUM_NODES; j++) {
			out4[i] += out3[j] * hlayer3_weights[i][j];
		}
		out4[i] += hlayer3_biases[i];

		//perform activation (ReLU)
		out4[i] = relu(out4[i]);
	}

	// hidden layer 4
//	data_t *out5 = h_layer(out4, hlayer4_weights, hlayer4_biases);
    // initialize container for layer output
	data_t out5[NUM_NODES] = {0};

	// perform affine transformation (X@W + b)
	for (int i=0; i<NUM_NODES; i++) {
		for (int j=0; j<NUM_NODES; j++) {
			out5[i] += out4[j] * hlayer4_weights[i][j];
		}
		out5[i] += hlayer4_biases[i];

		//perform activation (ReLU)
		out5[i] = relu(out5[i]);
	}

	// hidden layer 5
	// perform affine transformation (X@W + b)
	for (int i=0; i<NUM_OUTPUTS; i++) {
		for (int j=0; j<NUM_NODES; j++) {
			output[i] += out5[j] * hlayer5_weights[i][j];
		}
		output[i] += hlayer5_biases[i];
	}

	// unscale output [-1,1] --> [0,1]
	for (int i=0; i<NUM_OUTPUTS; i++) {
		output[i] = (output[i] - new_min) * (max - min)/(new_max - new_min) + min;
	}

	// unscale output [0,1] --> [real values]
	if (scale01) {
		new_min = -1;
		for (int i=0; i<NUM_OUTPUTS; i++) {
			output[i] = (output[i] - new_min) * (yscale_max[i] - yscale_min[i])/(new_max - new_min) + yscale_min[i];
		}
	}
}

data_t relu(data_t z) {
	return z > (data_t) 0 ? z : 0;
}

