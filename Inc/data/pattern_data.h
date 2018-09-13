/*
 * pattern_data.h
 *
 *  Created on: 25 juil. 2018
 *      Author: coren
 */

#ifndef DATA_PATTERN_DATA_H_
#define DATA_PATTERN_DATA_H_

#include <stdint.h>

#define MAX_PATTERNS_DATA 400
#define PATTERN_DATA_SIZE 50000

typedef struct _PatternData_t {
	uint16_t *data;
	uint16_t size;
} PatternData_t;

// Called by the json decoder
void patternData_setDataIndex(uint16_t index);
void patternData_addValueToCurrentPattern(uint16_t value);

// Used for debug, allow to quickly add a pattern
void patternData_AddPatternData(uint16_t index, uint16_t* values, uint16_t length);

PatternData_t* patternData_getPatternFromIndex(uint16_t index);

#endif /* DATA_PATTERN_DATA_H_ */
