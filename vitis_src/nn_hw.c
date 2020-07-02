/*
 * functions to run the hardware implementation of neural network
 *
 * Author: Kimberly Chan
 * Organization: Mesbah Lab at UC Berkeley
 */

#include "platform.h"
#include "xparameters.h"
#include "xaxidma.h"
#include "xneural_net_neural_net.h"
#include "xscugic.h"
#include "nn_hw.h"
#include "xil_printf.h"

volatile static int RunExample = 0;
volatile static int ResultExample = 0;

XNeural_net_neural_net xnn_dev;
XNeural_net_neural_net_Config xnn_config = {
		0,
		XPAR_NEURAL_NET_0_S_AXI_AXILITES_BASEADDR
};

// Interrupt Controller Instance
XScuGic ScuGic;

extern XAxiDma AxiDma;

int XNnSetup() {
	return XNeural_net_neural_net_CfgInitialize(&xnn_dev, &xnn_config);
}

void XNnStart(void *InstancePtr) {
	XNeural_net_neural_net *pExample = (XNeural_net_neural_net *) InstancePtr;
	XNeural_net_neural_net_InterruptEnable(pExample, 1);
	XNeural_net_neural_net_InterruptGlobalEnable(pExample);
	XNeural_net_neural_net_Start(pExample);
}

void XNnIsr(void *InstancePtr) {
	XNeural_net_neural_net *pExample = (XNeural_net_neural_net *) InstancePtr;
	// Disable Global Interrupt
	XNeural_net_neural_net_InterruptGlobalDisable(pExample);
	// Disable Local Interrupt
	XNeural_net_neural_net_InterruptDisable(pExample, 0xffffffff);
	// clear local interrupt
	XNeural_net_neural_net_InterruptClear(pExample, 1);

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
		printf("Interrupt Configuration Lookup Failed\n\r");
		return XST_FAILURE;
	}
	result = XScuGic_CfgInitialize(&ScuGic, pCfg->CpuBaseAddress);
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
	// ...

	return XST_SUCCESS;
}


int Setup_HW_Accelerator(float input[NUM_INPUTS], float output[NUM_OUTPUTS], bool scale01, int dma_size) {
	// set up the HLS Block
	int status = XNnSetup();
	if (status != XST_SUCCESS) {
		printf("Error: Example Setup Failed\n");
		return XST_FAILURE;
	}
	status = XNnSetupInterrupt();
	if (status != XST_SUCCESS) {
		printf("Error: Interrupt Setup Failed\n");
		return XST_FAILURE;
	}

	XNeural_net_neural_net_Set_scale01(&xnn_dev, scale01);
	XNnStart(&xnn_dev);

	// flush the cache
	Xil_DCacheFlushRange((unsigned int)input, dma_size);
	Xil_DCacheFlushRange((unsigned int)output, dma_size);
//	Xil_DCacheFlushRange((unsinged int scale01, dma_size));
	printf("\rCache Cleared\n\r");

	return 0;
}

void Start_HW_Accelerator() {
	int status = XNnSetup();
	if (status != XST_SUCCESS) {
		printf("Error: Example Setup Failed\n");
		return XST_FAILURE;
	}
	status = XNnSetupInterrupt();
	if (status != XST_SUCCESS) {
		printf("Error: Interrupt Setup Failed\n");
		return XST_FAILURE;
	}
	XNnStart(&xnn_dev);
}


int Run_HW_Accelerator(float input[NUM_INPUTS], float output[NUM_OUTPUTS], bool scale01, int dma_size) {
	// transfer input to HLS block
	printf("Transmitting data to HLS...");
	int status = XAxiDma_SimpleTransfer(&AxiDma, (unsigned int) input, dma_size, XAXIDMA_DMA_TO_DEVICE);
	if (status != XST_SUCCESS) {
		printf("Error: DMA transfer TO HLS block failed\n");
		return XST_FAILURE;
	}

	// wait for transfer to be done
	while (XAxiDma_Busy(&Axi_Dma, XAXIDMA_DMA_TO_DEVICE));

	// get results from HLS block
	printf("Receiving data from HLS...");
	status = XAxiDma_SimpleTransfer(&AxiDma, (unsigned int) output, dma_size, XAXIDMA_DEVICE_TO_DMA);
	if (status != XST_SUCCESS) {
		printf("Error: DMA transfer FROM HLS block failed\n");
		return XST_FAILURE;
	}

	// wait for transfer to be done
	while (XAxiDma_Busy(&Axi_Dma, XAXIDMA_DEVICE_TO_DMA));

	// ensure all transfers are done
	while (XAxiDma_Busy(&Axi_Dma, XAXIDMA_DEVICE_TO_DMA) || XAxiDma_Busy(&Axi_Dma, XAXIDMA_DMA_TO_DEVICE));

	return 0;
}

