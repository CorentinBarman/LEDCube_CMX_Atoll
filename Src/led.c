/*
 * led_control.c
 *
 *  Created on: 2 juil. 2018
 *      Author: coren
 */


#include <json/vcp_communication.h>
#include <led.h>
#include "main.h"
#include <memory.h>

#include "data/sequence.h"

#define MAX_LED_CHANNELS 20

extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim3;
extern TIM_HandleTypeDef htim4;
extern TIM_HandleTypeDef htim5;
extern TIM_HandleTypeDef htim6;
extern TIM_HandleTypeDef htim8;
extern TIM_HandleTypeDef htim9;
extern TIM_HandleTypeDef htim12;


typedef struct _led_t{
	Sequence_t* sequence;

	uint32_t pattern_position;
	uint16_t current_pattern;
	uint32_t interval_count;

	uint32_t pattern_duration_count;

	uint8_t sequence_on;
	uint8_t sequence_repeat;
} Led_t;


static Led_t leds[MAX_LED_CHANNELS];

void LED_setSequence(uint8_t index, Sequence_t *sequence)
{
	leds[index].sequence = sequence;
	leds[index].pattern_position = 0;
	leds[index].current_pattern = 0;
	leds[index].interval_count = 0;
	leds[index].pattern_duration_count = 0;

	leds[index].sequence_on = 0;
	leds[index].sequence_repeat = 0;
}


static inline uint32_t getCurrentPatternInterval(uint8_t index)
{
	return leds[index].sequence->patterns[leds[index].current_pattern]->interval;
}

static inline uint16_t getCurrentPatternValue(uint8_t index)
{
	return leds[index].sequence->patterns[leds[index].current_pattern]->patternData->data[leds[index].pattern_position];
}

static inline uint16_t getCurrentPatternLength(uint8_t index)
{
	return leds[index].sequence->patterns[leds[index].current_pattern]->patternData->size;
}

static inline uint16_t getCurrentPatternStartingPosition(uint8_t index)
{
	return leds[index].sequence->patterns[leds[index].current_pattern]->startPosition;
}

static inline uint32_t getCurrentPatternDuration(uint8_t index)
{
	return leds[index].sequence->patterns[leds[index].current_pattern]->duration;
}

void LED_startLedSequence(uint8_t ledIndex)
{
	if(leds[ledIndex].sequence != NULL)
	{
		leds[ledIndex].current_pattern = 0;
		leds[ledIndex].interval_count = 0;
		leds[ledIndex].pattern_duration_count = 0;
		leds[ledIndex].pattern_position = getCurrentPatternStartingPosition(ledIndex);

		leds[ledIndex].sequence_on = 1;
		leds[ledIndex].sequence_repeat = 0;


		LED_changePWM_Duty(ledIndex, getCurrentPatternValue(ledIndex));
	}
}

void LED_stopLedSequence(uint8_t ledIndex)
{
	if(leds[ledIndex].sequence != NULL)
	{
		leds[ledIndex].sequence_on = 0;
	}

	LED_changePWM_Duty(ledIndex, 0);
}

void LED_timer_interval_irq()
{
	uint8_t i;

	// Loop on all Leds
	for(i = 0; i < MAX_LED_CHANNELS; i++)
	{
		if(leds[i].sequence_on)
		{
			// Counter between patterns
			leds[i].pattern_duration_count++;

			if(leds[i].pattern_duration_count >= getCurrentPatternDuration(i))
			{
				leds[i].pattern_duration_count = 0;

				// Load the next pattern
				leds[i].current_pattern++;
				if(leds[i].current_pattern >= leds[i].sequence->patterns_count)
				{
					leds[i].sequence_on = 0;
					LED_changePWM_Duty(i, 0);
				}
				else
				{
					leds[i].interval_count = 0;
					leds[i].pattern_position = getCurrentPatternStartingPosition(i);

					// Apply new value
					LED_changePWM_Duty(i, getCurrentPatternValue(i));
				}
			}
			else
			{
				// Position in pattern
				leds[i].interval_count++;

				// Next pattern position
				if(leds[i].interval_count >= getCurrentPatternInterval(i))
				{
					leds[i].interval_count = 0;

					leds[i].pattern_position++;

					// Loop inside the pattern until it is completed
					if(leds[i].pattern_position >= getCurrentPatternLength(i))
					{
						leds[i].pattern_position = 0;
					}

					LED_changePWM_Duty(i, getCurrentPatternValue(i));
				}
			}
		}
	}
}



void LED_changePWM_Duty(uint8_t led, uint16_t duty)
{
	switch(led)
	{
	case 0:
		__HAL_TIM_SET_COMPARE(&htim8, TIM_CHANNEL_1, duty);
		break;
	case 1:
		__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, duty);
		break;
	case 2:
		__HAL_TIM_SET_COMPARE(&htim5, TIM_CHANNEL_4, duty);
		break;
	case 3:
		__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_3, duty);
		break;
	case 4:
		__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_3, duty);
		break;
	case 5:
		__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, duty);
		break;
	case 6:
		__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_3, duty);
		break;
	case 7:
		__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_4, duty);
		break;
	case 8:
		__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, duty);
		break;
	case 9:
		__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_4, duty);
		break;
	case 10:
		__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, duty);
		break;
	case 11:
		__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, duty);
		break;
	case 12:
		__HAL_TIM_SET_COMPARE(&htim8, TIM_CHANNEL_3, duty);
		break;
	case 13:
		__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_4, duty);
		break;
	case 14:
		__HAL_TIM_SET_COMPARE(&htim8, TIM_CHANNEL_4, duty);
		break;
	case 15:
		__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, duty);
		break;
	case 16:
		__HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_2, duty);
		break;
	case 17:
		__HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_3, duty);
		break;
	case 18:
		__HAL_TIM_SET_COMPARE(&htim9, TIM_CHANNEL_1, duty);
		break;
	case 19:
		__HAL_TIM_SET_COMPARE(&htim9, TIM_CHANNEL_2, duty);
		break;
	default:
		break;
	}
}
