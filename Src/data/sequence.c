/*
 * sequence.c
 *
 *  Created on: 4 juil. 2018
 *      Author: coren
 */


#include "data/sequence.h"
#include "data/pattern.h"
#include "led.h"

#define MAX_SEQUENCES 20

static Sequence_t ledSequences[MAX_SEQUENCES];
static uint8_t sequenceIndex;

void sequence_setIndex(uint8_t index)
{
	if(index >= MAX_SEQUENCES)
		sequenceIndex = 0;
	else
		sequenceIndex = index;

	ledSequences[sequenceIndex].patterns_count = 0;
}

void sequence_setLedIndex(uint8_t ledIndex)
{
	ledSequences[sequenceIndex].ledIndex = ledIndex;

	// Assign our sequence to the LED
	LED_setSequence(ledIndex, &(ledSequences[sequenceIndex]));
}

void sequence_addPattern(Pattern_t *pattern)
{
	if(ledSequences[sequenceIndex].patterns_count < MAX_SEQUENCE_PATTERNS)
	{
		ledSequences[sequenceIndex].patterns[ledSequences[sequenceIndex].patterns_count] = pattern;
		ledSequences[sequenceIndex].patterns_count++;
	}
}
