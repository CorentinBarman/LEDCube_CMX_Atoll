/*
 * json_main_object.c
 *
 *  Created on: 24 juil. 2018
 *      Author: coren
 */

#include <json/json_decoder.h>
#include <json/pattern_data.h>
#include <stdlib.h>
#include <string.h>

static JsonObject_t* objectStart(char* name);
static JsonObject_t* arrayObjectStart(char* name, uint16_t index);
static void numberReceived(char* name, uint32_t number);
static void stringReceived(char* name, char* string);
static void arrayNumberReceived(char* name, uint32_t number, uint16_t index);
static void arrayStringReceived(char* name, char* string, uint16_t index);

JsonObject_t jsonMainObject = {
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
	if(strcmp(name, "PatternDatas") == 0)
	{
		patternData_setDataIndex(index);
		return patternData_getJsonObject();
	}
	else if(strcmp(name, "Sequences") == 0)
	{

	}

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

}

static void arrayStringReceived(char* name, char* string, uint16_t index)
{

}
