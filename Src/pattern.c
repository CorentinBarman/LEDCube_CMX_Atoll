/*
 * pattern.c
 *
 *  Created on: 4 juil. 2018
 *      Author: coren
 */

#include "pattern.h"
#include <memory.h>

#define MAX_ALLOCATED_DATA 10000
#define MAX_PATTERN_SIZE 1000
#define MAX_PATTERNS 256

static uint16_t rawData[MAX_ALLOCATED_DATA];
static uint16_t allocatedDataCount = 0;

static patternData_t savedPatternsData[MAX_PATTERNS];

static pattern_t savedPatterns[MAX_PATTERNS];

pattern_t* pattern_GetPattern(uint16_t index)
{
	return &(savedPatterns[index]);
}

void pattern_ClearAllPatternsData()
{
	uint16_t i;
	for(i = 0; i < MAX_PATTERNS; i++)
	{
		savedPatternsData[i].data = 0;
		savedPatternsData[i].size = 0;
	}

	allocatedDataCount = 0;
}

void pattern_AddPatternData(uint16_t index, uint16_t length, uint16_t *data)
{
	if(length != 0 && length < MAX_PATTERN_SIZE && index < MAX_PATTERNS && allocatedDataCount + length < MAX_ALLOCATED_DATA)
	{
		// Copy pattern data to rawData
		memcpy(&(rawData[allocatedDataCount]), data, length * sizeof(uint16_t));

		// Update the pattern selected by index
		savedPatternsData[index].data = &(rawData[allocatedDataCount]);
		savedPatternsData[index].size = length;

		allocatedDataCount += length;
	}
}

void pattern_ModifyPattern(uint16_t index, uint16_t patternIndex, uint32_t interval, uint32_t duration, uint16_t startPosition)
{
	if(index < MAX_PATTERNS && patternIndex < MAX_PATTERNS)
	{
		savedPatterns[index].patternData = &(savedPatternsData[patternIndex]);
		savedPatterns[index].duration = duration;
		savedPatterns[index].interval = interval;
		savedPatterns[index].startPosition = startPosition;
	}
}
