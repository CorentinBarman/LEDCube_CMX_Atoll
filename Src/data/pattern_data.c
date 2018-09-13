/*
 * pattern_data.c
 *
 *  Created on: 25 juil. 2018
 *      Author: coren
 */

#include "data/pattern_data.h"

static PatternData_t patternDatas[MAX_PATTERNS_DATA];

static uint16_t dataIndex;
static uint16_t data[PATTERN_DATA_SIZE];
static uint16_t dataPosition;

void patternData_setDataIndex(uint16_t index)
{
	// New set of data, we start from 0 again
	if(index == 0)
	{
		dataPosition = 0;
	}

	dataIndex = index;

	// Reset the length and set the starting position
	patternDatas[index].size = 0;
	patternDatas[index].data = &(data[dataPosition]);
}


PatternData_t* patternData_getPatternFromIndex(uint16_t index)
{
	// We could warn the user there is something wrong instead
	if(index >= MAX_PATTERNS_DATA)
		return &(patternDatas[0]);

	return &(patternDatas[index]);
}


void patternData_AddPatternData(uint16_t index, uint16_t* values, uint16_t length)
{
	uint16_t i;

	patternData_setDataIndex(index);

	for(i = 0; i < length; i++)
	{
		patternData_addValueToCurrentPattern(values[i]);
	}
}


void patternData_addValueToCurrentPattern(uint16_t value)
{
	if(dataPosition < PATTERN_DATA_SIZE)
	{
		// Fill the global data array
		data[dataPosition] = value;
		dataPosition++;

		// Our pattern data only increment its size
		patternDatas[dataIndex].size++;
	}
	else
	{
		// TODO: Warn the user we don't have enough space
	}
}
