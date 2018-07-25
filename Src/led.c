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

void LED_init_for_measures()
{/*
	uint8_t i, j;

	for(j = 0; j < 20; j++)
	{
		for(i = 1; i <= 16; i++)
		{
			leds[j].pattern_data[i-1] = i * 4095;
		}

		leds[j].pattern_length = 16;
		leds[j].pattern_interval = 180000;
		leds[j].pattern_position = 0;
		leds[j].pattern_interval_count = 0;
		leds[j].pattern_on = 0;
	}

	testMeasureLED = 1;
	leds[testMeasureLED].pattern_on = 1;
	LED_change_PWM_duty(testMeasureLED, leds[testMeasureLED].pattern_data[0]);*/

}

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

void LED_changePWM_Duty(uint8_t index, uint16_t duty)
{
	change_PWM_duty(index, duty);
}


static inline uint32_t getCurrentPatternInterval(uint8_t i)
{
	return leds[i].sequence->patterns[leds[i].current_pattern]->interval;
}

static inline uint16_t getCurrentPatternValue(uint8_t i)
{
	return leds[i].sequence->patterns[leds[i].current_pattern]->patternData->data[leds[i].pattern_position];
}

static inline uint16_t getCurrentPatternLength(uint8_t i)
{
	return leds[i].sequence->patterns[leds[i].current_pattern]->patternData->size;
}

static inline uint16_t getCurrentPatternStartingPosition(uint8_t i)
{
	return leds[i].sequence->patterns[leds[i].current_pattern]->startPosition;
}

static inline uint32_t getCurrentPatternDuration(uint8_t i)
{
	return leds[i].sequence->patterns[leds[i].current_pattern]->duration;
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
	}
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
				}
				else
				{
					leds[i].interval_count = 0;
					leds[i].pattern_position = getCurrentPatternStartingPosition(i);

					// Apply new value
					LED_changePWM_Duty(i, getCurrentPatternValue(i));
				}
			}

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


