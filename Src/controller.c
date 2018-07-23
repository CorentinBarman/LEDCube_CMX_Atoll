/*
 * led_cube.c
 *
 *  Created on: 19 juin 2018
 *      Author: Corentin Barman
 */

#include <controller.h>
#include <json_decoder.h>
#include <led.h>
#include "main.h"
#include <stdint.h>
#include "usbd_cdc_if.h"
#include "assert.h"
#include "vcp_communication.h"
#include "sequence.h"

#define VCP_BUFFER_SIZE 100

uint32_t vcp_length;
uint8_t vcp_data[VCP_BUFFER_SIZE] = "";
uint8_t vcp_result;


void UserCode_Init()
{
	HAL_GPIO_WritePin(LD5_GPIO_Port, LD5_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(LD6_GPIO_Port, LD6_Pin, GPIO_PIN_SET);

	LED_init_for_measures();
	//sequence_InitTestSequence();
}

void UserCode_While()
{
	// Check if any data has been received on the USB Virtual Com Port (VCP)
	if(VCP_retrieveInputData(vcp_data, &vcp_length) != 0)
	{
		// Check that we have no memory overflow
		if(vcp_length > VCP_BUFFER_SIZE)
		{
			// THE VCP buffer is too small ! The program is already corrupt at this point !
			// Increase the VCP buffer size or find a way to limit the partial buffer size.
			assert(0);
		}

		// Echo the received data to the PC
		//VCP_SendBuffer(vcp_data, vcp_length);

		// Decode the partial JSON data
		jsonDecoding(vcp_data, vcp_length);
	}
}
