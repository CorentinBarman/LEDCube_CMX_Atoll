/*
 * sequence.c
 *
 *  Created on: 4 juil. 2018
 *      Author: coren
 */


#include "sequence.h"
#include "pattern.h"
#include "led.h"

#define MAX_PATTERNS 200

typedef struct _sequence_t{
	pattern_t* patterns[MAX_PATTERNS];

	uint16_t current_pattern;
	uint16_t pattern_counter;
	uint16_t patterns_size;

	uint32_t interval_counter;
	uint32_t duration_counter;

	uint8_t is_active;
} sequence_t;

sequence_t testSequence;

void sequence_InitTestSequence()
{
	uint8_t i;
	uint16_t patData1[] = {0, 1000, 2000, 3000, 4000, 5000, 6000, 7000, 8000, 9000};
	uint16_t patData2[] = {9000, 8000, 7000, 6000, 5000, 4000, 3000, 2000, 1000, 0};

	testSequence.current_pattern = 0;
	testSequence.pattern_counter = 0;
	testSequence.patterns_size = 6;
	testSequence.interval_counter = 0;
	testSequence.duration_counter = 0;
	testSequence.is_active = 1;

	pattern_AddPatternData(0, 10, patData1);
	pattern_AddPatternData(1, 10, patData2);

	pattern_ModifyPattern(0, 0, 100, 5000, 0);
	pattern_ModifyPattern(1, 0, 50, 5000, 0);
	pattern_ModifyPattern(2, 0, 25, 5000, 0);

	pattern_ModifyPattern(3, 1, 100, 5000, 0);
	pattern_ModifyPattern(4, 1, 50, 5000, 0);
	pattern_ModifyPattern(5, 1, 25, 5000, 0);

	for(i = 0; i < 6; i++)
	{
		testSequence.patterns[i] = pattern_GetPattern(i);
	}
}



void sequence_timer_IRQ()
{
	if(testSequence.is_active)
	{
		testSequence.interval_counter++;
		testSequence.duration_counter++;

		if(testSequence.duration_counter >= testSequence.patterns[testSequence.current_pattern]->duration)
		{
			testSequence.duration_counter = 0;
			testSequence.current_pattern++;

			if(testSequence.current_pattern >= testSequence.patterns_size)
			{
				testSequence.is_active = 0;
				LED_change_PWM_duty(1, 0);
				return;
			}
		}

		if(testSequence.interval_counter >= testSequence.patterns[testSequence.current_pattern]->interval)
		{
			testSequence.interval_counter = 0;
			testSequence.pattern_counter++;

			if(testSequence.pattern_counter >= testSequence.patterns[testSequence.current_pattern]->patternData->size)
			{
				testSequence.pattern_counter = 0;
			}

			LED_change_PWM_duty(1, testSequence.patterns[testSequence.current_pattern]->patternData->data[testSequence.pattern_counter]);
		}
	}
}
