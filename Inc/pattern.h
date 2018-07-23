/*
 * pattern.h
 *
 *  Created on: 4 juil. 2018
 *      Author: coren
 */

#ifndef PATTERN_H_
#define PATTERN_H_

#include <stdint.h>

typedef struct _patternData_t{
	uint16_t size;
	uint16_t *data;
} patternData_t;

typedef struct _pattern_t{

	// The raw data of the PWM values
	patternData_t *patternData;

	// Interval between two values of the pattern data
	uint32_t interval;

	// The total duration the pattern should be played, 0 for infinite
	uint32_t duration;

	// The start position in the pattern, used to implement phase
	uint16_t startPosition;

} pattern_t;


pattern_t* pattern_GetPattern(uint16_t index);
void pattern_ModifyPattern(uint16_t index, uint16_t patternIndex, uint32_t interval, uint32_t duration, uint16_t startPosition);
void pattern_AddPatternData(uint16_t index, uint16_t length, uint16_t *data);
void pattern_ClearAllPatternsData();

#endif /* PATTERN_H_ */
