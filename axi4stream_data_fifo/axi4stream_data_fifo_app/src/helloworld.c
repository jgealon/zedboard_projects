#include <stdio.h>
#include "xparameters.h"
#include "xllfifo.h"
#include "xstatus.h"

#define FIFO_BASEADDR			XPAR_AXI_FIFO_MM_S_0_BASEADDR
#define WORD_SIZE 				4    // Size of a word in bytes
#define MAX_PACKET_LEN 			6
#define NO_OF_PACKETS 			8
#define MAX_DATA_BUFFER_SIZE	NO_OF_PACKETS * MAX_PACKET_LEN

XLlFifo FifoInstance;
uint32_t SrcBuffer[MAX_DATA_BUFFER_SIZE];
uint32_t DstBuffer[MAX_DATA_BUFFER_SIZE];

int FifoInit(XLlFifo *InstancePtr, UINTPTR BaseAddr);
int FifoTest(XLlFifo *InstancePtr);
int FifoSend(XLlFifo *InstancePtr, uint32_t *SrcAddr);
int FifoRecv(XLlFifo *InstancePtr, uint32_t *DstAddr);

int main()
{
	if (FifoInit(&FifoInstance, FIFO_BASEADDR) != XST_SUCCESS)
		printf("AXI-Stream FIFO initialization failed!\n");
	else
		printf("AXI-Stream FIFO initialization passed!\n");

	if (FifoTest(&FifoInstance) != XST_SUCCESS)
		printf("AXI-Stream FIFO polling test failed!\n");
	else
		printf("AXI-Stream FIFO polling test passed!\n");

    return 0;
}

int FifoInit(XLlFifo *InstancePtr, UINTPTR BaseAddr)
{
	XLlFifo_Config *Config;
	int Status = XST_SUCCESS;

	// *** Initialize FIFO ***
	// Note: Xilinx driver has typo - XLlFfio (double 'f') not XLlFifo
	// In SDT mode, XLlFfio_LookupConfig takes BaseAddress, not DeviceId
	Config = XLlFfio_LookupConfig(BaseAddr);
	if (!Config)
	{
		printf("No configuration found for base address 0x%08X\n", (unsigned int)BaseAddr);
		return XST_FAILURE;
	}
	Status = XLlFifo_CfgInitialize(InstancePtr, Config, Config->BaseAddress);
	if (Status != XST_SUCCESS)
	{
		printf("Initialization failed with status: %d\n", Status);
		return Status;
	}

	// *** Check for the reset value ***
	Status = XLlFifo_Status(InstancePtr);
	XLlFifo_IntClear(InstancePtr, 0xFFFFFFFF);
	Status = XLlFifo_Status(InstancePtr);
	if (Status != 0)
	{
		printf("ERROR: Reset value of ISR0: 0x%x\tExpected: 0x0\n",
			    (unsigned int)XLlFifo_Status(InstancePtr));
		return XST_FAILURE;
	}

	return Status;
}

int FifoTest(XLlFifo *InstancePtr)
{
	int Status = XST_SUCCESS;

	// *** Transmit the data ***
	Status = FifoSend(InstancePtr, SrcBuffer);
	if (Status != XST_SUCCESS)
	{
		printf("Transmission of data failed\n");
		return XST_FAILURE;
	}

	// *** Print the transmitted data ***
	for (int i = 0; i < MAX_DATA_BUFFER_SIZE; i++)
		printf("%d | ", (unsigned int)SrcBuffer[i]);
	printf("\n");

	// *** Receive the data ***
	Status = FifoRecv(InstancePtr, DstBuffer);
	if (Status != XST_SUCCESS)
	{
		printf("Receiving data failed\n");
		return XST_FAILURE;
	}

	// *** Print the received data ***
	for (int i = 0; i < MAX_DATA_BUFFER_SIZE; i++)
		printf("%d | ", (unsigned int)DstBuffer[i]);
	printf("\n");

	// *** Compare the transmitted and received data ***
	printf("Comparing data ...\n");
	for (int i = 0; i < MAX_DATA_BUFFER_SIZE; i++)
		if (SrcBuffer[i] != DstBuffer[i])
			return XST_FAILURE;

	return Status;
}

int FifoSend(XLlFifo *InstancePtr, uint32_t *SrcAddr)
{
	printf("Transmitting data ... \n");

	// *** Filling the source buffer with data ***
	for (int i = 0; i < MAX_DATA_BUFFER_SIZE; i++)
		SrcAddr[i] = i;

	// *** Writing the source buffer into the FIFO transmit buffer ***
	for (int i = 0; i < NO_OF_PACKETS; i++)
		for (int j = 0; j < MAX_PACKET_LEN; j++)
			if(XLlFifo_iTxVacancy(InstancePtr))
				XLlFifo_TxPutWord(InstancePtr, SrcAddr[(i * MAX_PACKET_LEN) + j]);

	// Start transmission by writing transmission length (in bytes) into the TLR
	XLlFifo_iTxSetLen(InstancePtr, (MAX_DATA_BUFFER_SIZE * WORD_SIZE));

	// Check for transmission completion
	while (!(XLlFifo_IsTxDone(InstancePtr)));

	return XST_SUCCESS;
}

int FifoRecv(XLlFifo *InstancePtr, uint32_t *DstAddr)
{
	int RecvLen = 0;
	int RecvWord = 0;
	int Status = TRUE;

	printf("Receiving data ...\n");

	// *** Wait for receive data to be available ***
	while (XLlFifo_iRxOccupancy(InstancePtr) == 0);

	// *** Read receive length ***
	RecvLen = XLlFifo_iRxGetLen(InstancePtr) / WORD_SIZE;

	// *** Read the data from FIFO receive buffer ***
	for (int i = 0; i < RecvLen; i++)
	{
		RecvWord = XLlFifo_RxGetWord(InstancePtr);
		DstAddr[i] = RecvWord;
	}

	// *** Check for receive completion ***
	Status = XLlFifo_IsRxDone(InstancePtr);
	if (Status != TRUE)
	{
		printf("Failing in receive complete ...\n");
		return XST_FAILURE;
	}

	return XST_SUCCESS;
}