/*
 * pattern_data.h
 *
 *  Created on: 24 juil. 2018
 *      Author: coren
 */

#ifndef PATTERN_DATA_H_
#define PATTERN_DATA_H_

#include <json/json_decoder.h>

JsonObject_t* jsonPatternData_getJsonObject();
void jsonPatternData_setDataIndex(uint16_t index);

#endif /* PATTERN_DATA_H_ */
