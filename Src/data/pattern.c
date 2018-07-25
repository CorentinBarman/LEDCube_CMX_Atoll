/*
 * pattern.c
 *
 *  Created on: 4 juil. 2018
 *      Author: coren
 */

#include "data/pattern.h"
#include <memory.h>

#include "data/pattern_data.h"

static Pattern_t savedPatterns[MAX_PATTERNS];
static uint16_t currentPatternCount;
static uint16_t currentPatternIndex;

void pattern_setPatternIndex(uint16_t index)
{
	currentPatternIndex = index;
}

void pattern_resetPatternCount()
{
	currentPatternCount = 0;
}

void pattern_addNewPattern()
{
	if(currentPatternIndex < MAX_PATTERNS)
		currentPatternCount++;

	currentPatternIndex = currentPatternCount-1;
}

void pattern_setPatternDataFromIndex(uint16_t index)
{
	savedPatterns[currentPatternIndex].patternData = patternData_getPatternFromIndex(index);
}

void pattern_setPatternInterval(uint32_t interval)
{
	savedPatterns[currentPatternIndex].interval = interval;
}

void pattern_setPatternDuration(uint32_t duration)
{
	savedPatterns[currentPatternIndex].duration = duration;
}

void pattern_setPatternStartPosition(uint16_t startPosition)
{
	savedPatterns[currentPatternIndex].startPosition = startPosition;
}

void pattern_modifyPattern(uint16_t index, uint16_t dataIndex, uint32_t interval, uint32_t duration, uint16_t startPosition)
{
	pattern_setPatternIndex(index);
	pattern_setPatternDataFromIndex(dataIndex);
	pattern_setPatternInterval(interval);
	pattern_setPatternDuration(duration);
	pattern_setPatternStartPosition(startPosition);
}

Pattern_t* pattern_getCurrentPattern()
{
	return &(savedPatterns[currentPatternIndex]);
}

Pattern_t* pattern_getPattern(uint16_t index)
{
	if(index >= MAX_PATTERNS)
		return &(savedPatterns[0]);

	return &(savedPatterns[index]);
}
