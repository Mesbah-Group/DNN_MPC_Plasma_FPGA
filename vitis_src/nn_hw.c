/*
 * functions to run the hardware implementation of neural network
 *
 * Author: Kimberly Chan
 * Organization: Mesbah Lab at UC Berkeley
 */

#include "platform.h"
#include "xparameters.h"
#include "xaxidma.h"
#include "xnn_accel_nn_accel.h"
#include "xscugic.h"
#include "nn_hw.h"
#include "xil_printf.h"

volatile static int RunExample = 0;
volatile static int ResultExample = 0;

XNn_accel_nn_accel xnn_dev;
XNn_accel_nn_accel_Config xnn_config = {
		0,
		XPAR_NN_ACCEL_0_S_AXI_AXILITES_BASEADDR
};

// Interrupt Controller Instance
XScuGic ScuGic;

extern XAxiDma AxiDma;

int XNnSetup() {
	return XNn_accel_nn_accel_CfgInitialize(&xnn_dev, &xnn_config);
}

void XNnStart(void *InstancePtr) {
	XNn_accel_nn_accel *pExample = (XNn_accel_nn_accel *) InstancePtr;
	XNn_accel_nn_accel_InterruptEnable(pExample, 1);
	XNn_accel_nn_accel_InterruptGlobalEnable(pExample);
	XNn_accel_nn_accel_Start(pExample);
}

void XNnIsr(void *InstancePtr) {
	XNn_accel_nn_accel *pExample = (XNn_accel_nn_accel *) InstancePtr;
	// Disable Global Interrupt
	XNn_accel_nn_accel_InterruptGlobalDisable(pExample);
	// Disable Local Interrupt
	XNn_accel_nn_accel_InterruptDisable(pExample, 0xffffffff);
	// clear local interrupt
	XNn_accel_nn_accel_InterruptClear(pExample, 1);

	ResultExample = 1;
	// restart the core if it should run again
	if (RunExample) {
		XNnStart(pExample);
	}
}

// function to set up interrupt on the ARM
int XNnSetupInterrupt() {
	int result;
	XScuGic_Config *pCfg = XScuGic_LookupConfig(XPAR_SCUGIC_0_DEVICE_ID);
	if (pCfg == NULL) {
		print("Interrupt Configuration Lookup Failed\n\r");
		return XST_FAILURE;
	}
	result = XScuGic_CfgInitialize(&ScuGic, pCfg, pCfg->CpuBaseAddress);
	if (result != XST_SUCCESS) {
		return result;
	}
	result = XScuGic_SelfTest(&ScuGic);
	if (result != XST_SUCCESS) {
		return result;
	}

	// Initialize the exception handler
	Xil_ExceptionInit();
	// Register the exception handler
	Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_INT, (Xil_ExceptionHandler) XScuGic_InterruptHandler, &ScuGic);
	// Enable exception handler
	Xil_ExceptionEnable();

	// Connect the Adder ISR to the exception table
	result = XScuGic_Connect(&ScuGic, XPAR_FABRIC_NN_ACCEL_0_INTERRUPT_INTR, (Xil_InterruptHandler) XNnIsr, &xnn_dev);
	if (result != XST_SUCCESS) {
		return result;
	}

	// Enable the Adder ISR
	XScuGic_Enable(&ScuGic, XPAR_FABRIC_NN_ACCEL_0_INTERRUPT_INTR);

	return XST_SUCCESS;
}


int Setup_HW_Accelerator(float input[NUM_INPUTS], float output[NUM_OUTPUTS], bool scale01, int dma_size) {
	// set up the HLS Block
	int status = XNnSetup();
	if (status != XST_SUCCESS) {
		print("Error: Example Setup Failed\n");
		return XST_FAILURE;
	}
	status = XNnSetupInterrupt();
	if (status != XST_SUCCESS) {
		print("Error: Interrupt Setup Failed\n");
		return XST_FAILURE;
	}

	XNnStart(&xnn_dev);

	// flush the cache
	Xil_DCacheFlushRange((unsigned int)input, dma_size);
	Xil_DCacheFlushRange((unsigned int)output, dma_size);
	print("\rCache Cleared\n\r");

	return 0;
}

int Start_HW_Accelerator() {
	int status = XNnSetup();
	if (status != XST_SUCCESS) {
		print("Error: Example Setup Failed\n");
		return XST_FAILURE;
	}
	status = XNnSetupInterrupt();
	if (status != XST_SUCCESS) {
		print("Error: Interrupt Setup Failed\n");
		return XST_FAILURE;
	}
	XNnStart(&xnn_dev);

	return 0;
}


int Run_HW_Accelerator(float input[NUM_INPUTS], float output[NUM_OUTPUTS], bool scale01, int dma_size) {
	// transfer input to HLS block
//	print("Transmitting data to HLS...\n\r");
	int status = XAxiDma_SimpleTransfer(&AxiDma, (unsigned int) input, dma_size, XAXIDMA_DMA_TO_DEVICE);
	if (status != XST_SUCCESS) {
		print("Error: DMA transfer TO HLS block failed\n");
		return XST_FAILURE;
	}

	// wait for transfer to be done
	while (XAxiDma_Busy(&AxiDma, XAXIDMA_DMA_TO_DEVICE));

	// get results from HLS block
//	print("Receiving data from HLS...\n\r");
	status = XAxiDma_SimpleTransfer(&AxiDma, (unsigned int) output, dma_size, XAXIDMA_DEVICE_TO_DMA);
	if (status != XST_SUCCESS) {
		print("Error: DMA transfer FROM HLS block failed\n");
		return XST_FAILURE;
	}

	// wait for transfer to be done
	while (XAxiDma_Busy(&AxiDma, XAXIDMA_DEVICE_TO_DMA));

	// ensure all transfers are done
	while (XAxiDma_Busy(&AxiDma, XAXIDMA_DEVICE_TO_DMA) || XAxiDma_Busy(&AxiDma, XAXIDMA_DMA_TO_DEVICE));

	return 0;
}

