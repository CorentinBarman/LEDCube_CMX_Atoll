/*
 * pattern_data.c
 *
 *  Created on: 24 juil. 2018
 *      Author: coren
 */

#include <json/json_pattern_data.h>
#include <stdlib.h>
#include <string.h>

#include "data/pattern_data.h"

static JsonObject_t* objectStart(char* name);
static JsonObject_t* arrayObjectStart(char* name, uint16_t index);
static void numberReceived(char* name, uint32_t number);
static void stringReceived(char* name, char* string);
static void arrayNumberReceived(char* name, uint32_t number, uint16_t index);
static void arrayStringReceived(char* name, char* string, uint16_t index);

static JsonObject_t jsonObject_PatternData = {
		&objectStart,
		&arrayObjectStart,
		&numberReceived,
		&stringReceived,
		&arrayNumberReceived,
		&arrayStringReceived
};


static JsonObject_t* objectStart(char* name)
{
	return NULL;
}

static JsonObject_t* arrayObjectStart(char* name, uint16_t index)
{
	return NULL;
}

static void numberReceived(char* name, uint32_t number)
{

}

static void stringReceived(char* name, char* string)
{

}

static void arrayNumberReceived(char* name, uint32_t number, uint16_t index)
{
	if(strcmp(name, "Data") == 0)
	{
		patternData_addValueToCurrentPattern(number);
	}
}

static void arrayStringReceived(char* name, char* string, uint16_t index)
{

}

JsonObject_t* jsonPatternData_getJsonObject()
{
	return &jsonObject_PatternData;
}


void jsonPatternData_setDataIndex(uint16_t index)
{
	patternData_setDataIndex(index);
}
