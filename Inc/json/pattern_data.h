/*
 * pattern_data.h
 *
 *  Created on: 24 juil. 2018
 *      Author: coren
 */

#ifndef PATTERN_DATA_H_
#define PATTERN_DATA_H_

#define MAX_PATTERNS_DATA 100
#define PATTERN_DATA_SIZE 10000

#include <json/json_decoder.h>

typedef struct _PatternData_t {
	uint16_t *data;
	uint16_t size;
} PatternData_t;

JsonObject_t* patternData_getJsonObject();
void patternData_setDataIndex(uint16_t index);

#endif /* PATTERN_DATA_H_ */
