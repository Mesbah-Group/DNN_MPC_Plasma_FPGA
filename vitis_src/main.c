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
#include <stdbool.h>
#include "platform.h"
#include "xil_printf.h"
#include "xparameters.h"
#include "xaxidma.h"
#include "xtmrctr.h"
#include "xstatus.h"
#include "weights_sw.h"
#include "nn_hw.h"

/***** SW COMPARISON FUNCTION DECLARATIONS ******/
void neural_net(data_t input[NUM_INPUTS], data_t output[NUM_OUTPUTS], bool scale01);
data_t relu(data_t z);

/*********** MACROS ************/
#define DMA_DEV_ID XPAR_AXIDMA_0_DEVICE_ID
#define TIMER_DEV_ID XPAR_AXI_TIMER_0_DEVICE_ID
#define NUM_TESTS 1
#define EPS 0.0001

/*********** VARIABLE DEFINITIONS **********/
u16 DeviceId = DMA_DEV_ID;
XAxiDma AxiDma; // AXI DMA Instance
XAxiDma *InstancePtr = &AxiDma;
XTmrCtr timer_dev; // Timer Instance
float in[64][NUM_INPUTS] = {0.329648, 0.721062, 0.496215, 0.675957, 0.936821, 0.580465, 0.784999, 0.576272, 0.963488, 0.536648, 0.080186, 0.844873, 0.004447, 0.413825, 0.753107, 0.198611, 0.180845, 0.703050, 0.438725, 0.995631, 0.246624, 0.558291, 0.520386, 0.039941, 0.385129, 0.885283, 0.462113, 0.646859, 0.648252, 0.117374, 0.585674, 0.466254, 0.161620, 0.766331, 0.095281, 0.419756, 0.174635, 0.967821, 0.666115, 0.503115, 0.620111, 0.973154, 0.382675, 0.156003, 0.279650, 0.977198, 0.398367, 0.531393, 0.936084, 0.882505, 0.930693, 0.892737, 0.539014, 0.166662, 0.443530, 0.543676, 0.789250, 0.751527, 0.442453, 0.102965, 0.909403, 0.183745, 0.883623, 0.373318, 0.249231, 0.522238, 0.116291, 0.285112, 0.974126, 0.525121, 0.529510, 0.756104, 0.341125, 0.559837, 0.722570, 0.849403, 0.415146, 0.002601, 0.186232, 0.905680, 0.205614, 0.208745, 0.305143, 0.260369, 0.530431, 0.649770, 0.079955, 0.084285, 0.288782, 0.735328, 0.240770, 0.255251, 0.822651, 0.634553, 0.358183, 0.343391, 0.111076, 0.805918, 0.136138, 0.607345, 0.538927, 0.665889, 0.889758, 0.590064, 0.830441, 0.490686, 0.231142, 0.673621, 0.313508, 0.101946, 0.139459, 0.762140, 0.644644, 0.378827, 0.954526, 0.980139, 0.678363, 0.645884, 0.101722, 0.515925, 0.485320, 0.187866, 0.978055, 0.438182, 0.009385, 0.267725, 0.186382, 0.838303, 0.866911, 0.443134, 0.481342, 0.373755, 0.488120, 0.468530, 0.136630, 0.861131, 0.908550, 0.082692, 0.716456, 0.417147, 0.738824, 0.620333, 0.544138, 0.372068, 0.089774, 0.699009, 0.380516, 0.111761, 0.079050, 0.302174, 0.575589, 0.509416, 0.294525, 0.461657, 0.486898, 0.259814, 0.545449, 0.855880, 0.463475, 0.038012, 0.614405, 0.987852, 0.579286, 0.117415, 0.385076, 0.628978, 0.606873, 0.943234, 0.481927, 0.164180, 0.167001, 0.160527, 0.399026, 0.925004, 0.754379, 0.532357, 0.754337, 0.793496, 0.875211, 0.856599, 0.291369, 0.659039, 0.602387, 0.507608, 0.787626, 0.107367, 0.237992, 0.126025, 0.455472, 0.192685, 0.524974, 0.654590};
float out[64][NUM_OUTPUTS] = {0.136626, 0.758465, 0.497852, 0.589908, 0.999592, 0.901232, 0.954266, 0.707009, 0.307686, 0.680776, 0.141573, 0.750236, 0.123471, 0.780352, 0.066153, 0.875715, 0.151349, 0.733969, 0.112186, 0.799127, 0.088816, 0.838009, 0.132973, 0.764543, 0.180891, 0.685069, 0.999592, 0.901232, 0.034195, 0.928885, 0.409003, 0.616583, 0.999592, 0.901232, 0.239384, 0.718266, 0.999592, 0.901232, 0.110434, 0.802043, 0.999592, 0.901232, 0.107259, 0.807325, 0.531207, 0.517524, 0.999592, 0.901232, 0.467265, 0.571101, 0.109106, 0.804252, 0.051482, 0.900123, 0.014840, 0.961087, 0.079867, 0.852898, 0.247112, 0.682057, 0.649097, 0.529920, 0.112749, 0.798191, 0.012597, 0.964819, 0.374041, 0.621621, 0.999592, 0.901232, 0.165554, 0.710337, 0.000154, 0.985520, 0.189091, 0.682607, 0.996444, 0.870498, 0.143748, 0.746616, 0.919289, 0.571433, 0.026339, 0.941955, 0.999592, 0.901232, 0.099417, 0.820372, 0.062981, 0.880993, 0.535663, 0.687191, 0.942159, 0.660079, 0.136173, 0.759220, 0.064732, 0.878079, 0.021517, 0.949977, 0.111374, 0.800479, 0.077324, 0.857129, 0.174675, 0.695162, 0.707266, 0.499007, 0.090787, 0.834729, 0.999592, 0.901232, 0.030103, 0.935692, 0.687358, 0.499398, 0.999592, 0.901232, 0.999592, 0.901232, 0.153666, 0.730115, 0.093255, 0.830624, 0.056097, 0.892445, 0.139727, 0.753306};

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

	// single test or multiple tests from file
	// if testing multiple, ensure NUM_TESTS <= 64
	bool single_test = 0;

	float test_input[NUM_INPUTS];
	data_t test_in[NUM_INPUTS];
	data_t expected_output[NUM_OUTPUTS] = {0};
	float test_output[NUM_OUTPUTS] = {0};
	float out_hw[NUM_TESTS][NUM_OUTPUTS];
	float out_sw[NUM_TESTS][NUM_OUTPUTS];

	if (single_test) {
		test_input[0] = 0.329648;
		test_input[1] = 0.721062;
		test_input[2] = 0.496215;
		test_in[0] = 0.329648;
		test_in[1] = 0.721062;
		test_in[2] = 0.496215;
	}

	bool scale01 = 0;

    init_platform();

    print("\n\n\rHello World\n\r");

    int status;

    /*********** INITIALIZATIONS **********/
    status = init_dma();
    if (status != XST_SUCCESS) {
    	print("\rError: DMA init failed\n");
    	return XST_FAILURE;
    }

//    Xil_DCacheDisable(); // uncomment this as a last resort, which will avoid all cache-related issues, but at the expense of performance

    printf("\rsetup of DMA completed\r\n");

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
    printf("\rAxi Timer Clock Frequency: %dHz\n\r", XPAR_AXI_TIMER_0_CLOCK_FREQ_HZ);


    /*********** RUN SOFTWARE VERSION **********/
    printf("\r************Running Software Implementation of Algorithm**************\n\r");
    XTmrCtr_Reset(&timer_dev, TIMER_DEV_ID);
    begin_time = XTmrCtr_GetValue(&timer_dev, TIMER_DEV_ID);

    for (int i=0; i<NUM_TESTS; i++) {
    	if (!single_test) {
    		for (int j=0; j<NUM_INPUTS; j++) {
    			test_in[j] = in[i][j];
    		}
    	}

    	neural_net(test_in, expected_output, scale01);

    	if (!single_test) {
    		for (int j=0; j<NUM_OUTPUTS; j++) {
    			out_sw[i][j] = expected_output[j];
    		}
    	}
    }

    end_time = XTmrCtr_GetValue(&timer_dev, TIMER_DEV_ID);
    run_time_sw = end_time - begin_time - calibration;
    printf("\rExpected Output: [%f, %f]\n\r", expected_output[0], expected_output[1]);
    printf("\rSW Runtime was %d cycles over %d tests.\n\r", run_time_sw, NUM_TESTS);


    /*********** RUN HARDWARE VERSION **********/
    printf("\r************Running Hardware Implementation of Algorithm**************\n\r");
    XTmrCtr_Reset(&timer_dev, TIMER_DEV_ID);
    begin_time = XTmrCtr_GetValue(&timer_dev, TIMER_DEV_ID);

//    Setup_HW_Accelerator(test_input, test_output, scale01, dma_size);
    for (int i=0; i<NUM_TESTS; i++) {
    	if (!single_test) {
    	    for (int j=0; j<NUM_INPUTS; j++) {
    	    	test_input[j] = in[i][j];
    		}
    	}

    	status = Start_HW_Accelerator();
    	status = Run_HW_Accelerator(test_input, test_output, scale01, dma_size);

    	if (!single_test) {
    	    for (int j=0; j<NUM_OUTPUTS; j++) {
    			out_hw[i][j] = test_output[j];
       		}
       	}
    }

    end_time = XTmrCtr_GetValue(&timer_dev, TIMER_DEV_ID);
    run_time_hw = end_time - begin_time - calibration;
    printf("\rTest Output: [%f, %f]\n\r", test_output[0], test_output[1]);
    printf("\rHW (AXI DMA + Accelerator) Runtime is %d cycles over %d tests. \n\r", run_time_hw, NUM_TESTS);

    if (!single_test) {
    	float error = 0;
    	int count = 0;
    	printf("\rChecking outputs...\n\r");
    	for (int i=0; i<NUM_TESTS; i++) {
    		for (int j=0; j<NUM_OUTPUTS; j++) {
    			error = (out[i][j] - out_sw[i][j]);
    			error = error*error;
    	   		if (error > EPS) {
        			printf("\rSW output doesn't match!\n\r");
        			count++;
   	    		}
    	   		error = (out[i][j] - out_hw[i][j]);
    	   		error = error*error;
    	   		if (error > EPS) {
        		    printf("\rHW output doesn't match!\n\r");
        		    printf("\r%d HW output was %f.\n\r",i, out_hw[i][j]);
        		    count++;
   	    		}
   	    	}
   	    }
    	if (count==0) {
    		printf("\rAll outputs were within range! Test Successful!\n\r");
    	} else {
    		printf("\rTest complete. Some outputs did not match\n\r");
    	}

    }

    cleanup_platform();
    return 0;
}

/************ SOFTWARE FUNCTIONS **************/
void neural_net(data_t input[NUM_INPUTS], data_t output[NUM_OUTPUTS], bool scale01) {
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
    // initialize containers for layer outputs
	data_t out1[NUM_NODES], out2[NUM_NODES], out3[NUM_NODES], out4[NUM_NODES], out5[NUM_NODES];
	data_t out6[NUM_OUTPUTS];

	// perform affine transformation (X@W + b)
	for (int i=0; i<NUM_NODES; i++) {
		out1[i] = 0;
		for (int j=0; j<NUM_INPUTS; j++) {
			out1[i] += scaled_x2[j] * input_weights[i][j];
		}
		out1[i] += input_biases[i];

		//perform activation (ReLU)
		out1[i] = relu(out1[i]);
	}

	// hidden layer 1
	// perform affine transformation (X@W + b)
	for (int i=0; i<NUM_NODES; i++) {
		out2[i] = 0;
		for (int j=0; j<NUM_NODES; j++) {
			out2[i] += out1[j] * hlayer1_weights[i][j];
		}
		out2[i] += hlayer1_biases[i];

		//perform activation (ReLU)
		out2[i] = relu(out2[i]);
	}

	// hidden layer 2
	// perform affine transformation (X@W + b)
	for (int i=0; i<NUM_NODES; i++) {
		out3[i] = 0;
		for (int j=0; j<NUM_NODES; j++) {
			out3[i] += out2[j] * hlayer2_weights[i][j];
		}
		out3[i] += hlayer2_biases[i];

		//perform activation (ReLU)
		out3[i] = relu(out3[i]);
	}

	// hidden layer 3
	// perform affine transformation (X@W + b)
	for (int i=0; i<NUM_NODES; i++) {
		out4[i] = 0;
		for (int j=0; j<NUM_NODES; j++) {
			out4[i] += out3[j] * hlayer3_weights[i][j];
		}
		out4[i] += hlayer3_biases[i];

		//perform activation (ReLU)
		out4[i] = relu(out4[i]);
	}

	// hidden layer 4
	// perform affine transformation (X@W + b)
	for (int i=0; i<NUM_NODES; i++) {
		out5[i] = 0;
		for (int j=0; j<NUM_NODES; j++) {
			out5[i] += out4[j] * hlayer4_weights[i][j];
		}
		out5[i] += hlayer4_biases[i];

		//perform activation (ReLU)
		out5[i] = relu(out5[i]);
	}

	// hidden layer 5 (last layer)
	// perform affine transformation (X@W + b)
	for (int i=0; i<NUM_OUTPUTS; i++) {
		out6[i] = 0;
		for (int j=0; j<NUM_NODES; j++) {
			out6[i] += out5[j] * hlayer5_weights[i][j];
		}
		out6[i] += hlayer5_biases[i];
	}

	// unscale output [-1,1] --> [0,1]
	for (int i=0; i<NUM_OUTPUTS; i++) {
		out6[i] = (out6[i] - new_min) * (max - min)/(new_max - new_min) + min;
	}

	// unscale output [0,1] --> [real values]
	if (scale01) {
		new_min = -1;
		for (int i=0; i<NUM_OUTPUTS; i++) {
			output[i] = (out6[i] - new_min) * (yscale_max[i] - yscale_min[i])/(new_max - new_min) + yscale_min[i];
		}
	} else {
		for (int i=0; i<NUM_OUTPUTS; i++) {
			output[i] = out6[i];
		}
	}
}

data_t relu(data_t z) {
	return z > (data_t) 0 ? z : 0;
}
