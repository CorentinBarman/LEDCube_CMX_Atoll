/*
 * vcp_communication.c
 *
 *  Created on: 27 juin 2018
 *      Author: coren
 */

#include <json/vcp_communication.h>
#include "usb/usbd_cdc_if.h"


static struct
{
    int pos_receive, pos_process; //pos_receive is the current position in buffer to save incoming data. pos_process is the index of data in buffer which has been processed.
    //if pos_receive=pos_process, it means all data were processed, waiting for new data coming
    unsigned char IsCommandDataReceived; //anynumber >0 means data were received. 0 means no data is available
    uint8_t UserRxBufferFS[MaxCommandsInBuffer][APP_RX_DATA_SIZE];//it could save <MaxCommandsInBuffer> number of commands
    uint8_t CommandsLens[MaxCommandsInBuffer]; //save the len of each command
} s_RxBuffers;

extern USBD_HandleTypeDef hUsbDeviceFS;


void VCP_SendString(char* string)
{
	uint16_t length = 0;

	// Count the number of characters
	while(string[length] != '\0')
	{
		length++;
	}

	VCP_SendBuffer((uint8_t*)string, length);
}

void VCP_SendBuffer(uint8_t* Buf, uint16_t Len)
{
	while(CDC_Transmit_FS(Buf, Len) != USBD_OK)
	{
		HAL_GPIO_WritePin(LD6_GPIO_Port, LD6_Pin, GPIO_PIN_SET);
	}

	HAL_GPIO_WritePin(LD6_GPIO_Port, LD6_Pin, GPIO_PIN_RESET);
}


/**
  * @brief  Data received over USB OUT endpoint are sent over CDC interface
  *         through this function.
  *
  *         @note
  *         This function will block any OUT packet reception on USB endpoint
  *         untill exiting this function. If you exit this function before transfer
  *         is complete on CDC interface (ie. using DMA controller) it will result
  *         in receiving more data while previous ones are still not sent.
  *
  * @param  Buf: Buffer of data to be received
  * @param  Len: Number of data received (in bytes)
  * @retval Result of the operation: USBD_OK if all operations are OK else USBD_FAIL
  */
int8_t VCP_Receive_FS(uint8_t* Buf, uint32_t *Len)
{
	// CDC_Receive_FS  is a callback function. When data were received, the system calls this function. The received data can be accessed via Buf,and *Len
	s_RxBuffers.IsCommandDataReceived=1; //indicates data were received
	s_RxBuffers.CommandsLens[s_RxBuffers.pos_receive]=*Len; //only set the length, data was directly saved to buffer

	s_RxBuffers.pos_receive++;//move to next position to receive data
	if(s_RxBuffers.pos_receive>=MaxCommandsInBuffer) //reach the last buffer, need to rewind to 0
	{
		s_RxBuffers.pos_receive=0;
	}

	// Prepare to receive the next data
	USBD_CDC_SetRxBuffer(&hUsbDeviceFS, s_RxBuffers.UserRxBufferFS[s_RxBuffers.pos_receive]);//Set the buffer to receive incoming data
	USBD_CDC_ReceivePacket(&hUsbDeviceFS);// Tell that you are ready to receive the next packet, otherwise you wouldn't be able to receive next data

	return USBD_OK;
}


/**
  * @brief  VCP_retrieveInputData, defined by user
  *         Call this function frequently to check if data is received.
  *
  *
  *
  * @param  Buf: Buffer of data to be received
  * @param  Len: Number of data received (in bytes)
  * @retval 0 means no data was received.
  */
int8_t VCP_retrieveInputData(uint8_t* Buf, uint32_t *Len)
{
    if(s_RxBuffers.IsCommandDataReceived==0)return 0; //no data received

    int index=s_RxBuffers.pos_process;

    *Len=s_RxBuffers.CommandsLens[index]; //return the length
    memcpy(Buf,s_RxBuffers.UserRxBufferFS[index],*Len);
    Buf[*Len]='\0'; //testing only. make sure there is ending char in the returned command string

    //check if all data were processed.
    s_RxBuffers.pos_process++;

    if(s_RxBuffers.pos_process>=MaxCommandsInBuffer) //reach the last buffer, need to rewind to 0
    {
        s_RxBuffers.pos_process=0;
    }

    if(s_RxBuffers.pos_process==s_RxBuffers.pos_receive)
    	s_RxBuffers.IsCommandDataReceived=0; //check if all data were processed

    return 1;
}
