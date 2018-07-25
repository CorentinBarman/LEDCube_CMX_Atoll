/*
 * pattern.h
 *
 *  Created on: 4 juil. 2018
 *      Author: coren
 */

#ifndef PATTERN_H_
#define PATTERN_H_

#include <stdint.h>
#include "data/pattern_data.h"

#define MAX_PATTERNS 500

typedef struct _pattern_t{

	// The raw data of the PWM values
	PatternData_t *patternData;

	// Interval between two values of the pattern data
	uint32_t interval;

	// The total duration the pattern should be played, 0 for infinite
	uint32_t duration;

	// The start position in the pattern, used to implement phase
	uint16_t startPosition;

} Pattern_t;


// Functions used to modify the pattern data (Called by the json decoder)
void pattern_setPatternIndex(uint16_t index);
void pattern_addNewPattern();
void pattern_resetPatternCount();
void pattern_setPatternDataFromIndex(uint16_t index);
void pattern_setPatternInterval(uint32_t interval);
void pattern_setPatternDuration(uint32_t duration);
void pattern_setPatternStartPosition(uint16_t startPosition);

// Used for debug purpose, to quickly add a pattern
void pattern_modifyPattern(uint16_t index, uint16_t dataIndex, uint32_t interval, uint32_t duration, uint16_t startPosition);

Pattern_t* pattern_getPattern(uint16_t index);
Pattern_t* pattern_getCurrentPattern();

#endif /* PATTERN_H_ */
