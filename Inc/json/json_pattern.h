/*
 * json_pattern.h
 *
 *  Created on: 25 juil. 2018
 *      Author: coren
 */

#ifndef JSON_JSON_PATTERN_H_
#define JSON_JSON_PATTERN_H_

#include <json/json_decoder.h>

JsonObject_t* jsonPattern_getJsonObject();
void jsonPattern_setPatternIndex(uint16_t index);
void jsonPattern_addNewPattern();
void jsonPattern_resetPatternCount();

#endif /* JSON_JSON_PATTERN_H_ */
