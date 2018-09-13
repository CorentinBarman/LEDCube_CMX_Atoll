/*
 * led_cube.c
 *
 *  Created on: 19 juin 2018
 *      Author: Corentin Barman
 */

#include <controller.h>
#include <json/json_decoder.h>
#include <json/vcp_communication.h>
#include <led.h>
#include "main.h"
#include <stdint.h>
#include "usb/usbd_cdc_if.h"
#include "assert.h"

#include "data/sequence.h"
#include <stm32f4xx_hal.h>

#define VCP_BUFFER_SIZE 100

static uint32_t vcp_length;
static uint8_t vcp_data[VCP_BUFFER_SIZE] = "";

extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim3;
extern TIM_HandleTypeDef htim4;
extern TIM_HandleTypeDef htim5;
extern TIM_HandleTypeDef htim6;
extern TIM_HandleTypeDef htim8;
extern TIM_HandleTypeDef htim9;
extern TIM_HandleTypeDef htim12;

static void CreateSequencesForMeasure(uint32_t interval)
{
	uint8_t i, j;

	// Create 0 data pattern for waiting times
	patternData_setDataIndex(0);
	patternData_addValueToCurrentPattern(0);

	// Create sawtooth data pattern
	patternData_setDataIndex(1);
	for(i = 1; i <= 16; i++)
	{
		patternData_addValueToCurrentPattern(i * 4095);
	}

	// Create the actual patterns
	pattern_modifyPattern(0, 1, interval, interval*16, 0);
	pattern_modifyPattern(1, 0, 100, interval*16, 0);

	// Create the sequences
	for(i = 0; i < 16; i++)
	{
		sequence_setIndex(i);
		sequence_setLedIndex(i);

		for(j = 16 - i; j < 16; j++)
		{
			sequence_addPattern(pattern_getPattern(1));
		}

		sequence_addPattern(pattern_getPattern(0));
	}

	// Give the command to start the sequences
	for(i = 0; i < 20; i++)
	{
		LED_startLedSequence(i);
	}
}

void UserCode_Init()
{
	// LEDs
	HAL_GPIO_WritePin(LD5_GPIO_Port, LD5_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(LD6_GPIO_Port, LD6_Pin, GPIO_PIN_SET);

	// Enable timers for PWM
	HAL_TIM_Base_Start(&htim1);
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_3);
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_4);
	HAL_TIM_Base_Start(&htim2);
	HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2);
	HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_3);
	HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_4);
	HAL_TIM_Base_Start(&htim3);
	HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_2);
	HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_3);
	HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_4);
	HAL_TIM_Base_Start(&htim4);
	HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_2 );
	HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_3);
	HAL_TIM_Base_Start(&htim5);
	HAL_TIM_PWM_Start(&htim5, TIM_CHANNEL_4);
	HAL_TIM_Base_Start(&htim8);
	HAL_TIM_PWM_Start(&htim8, TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&htim8, TIM_CHANNEL_3);
	HAL_TIM_PWM_Start(&htim8, TIM_CHANNEL_4);
	HAL_TIM_Base_Start(&htim9);
	HAL_TIM_PWM_Start(&htim9, TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&htim9, TIM_CHANNEL_2);

	// Activate power output for the LEDs
	HAL_GPIO_WritePin(EN_15V_GPIO_Port, EN_15V_Pin, GPIO_PIN_SET);

	// Timer 1ms for updating the patterns
	HAL_TIM_Base_Start_IT(&htim6);

	//CreateSequencesForMeasure(180000);
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
