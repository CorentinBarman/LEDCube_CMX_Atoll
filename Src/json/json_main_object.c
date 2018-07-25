/*
 * json_main_object.c
 *
 *  Created on: 24 juil. 2018
 *      Author: coren
 */

#include <json/json_decoder.h>
#include <json/json_pattern_data.h>
#include <json/json_sequence.h>
#include <led.h>
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
		jsonPatternData_setDataIndex(index);
		return jsonPatternData_getJsonObject();
	}
	else if(strcmp(name, "Sequences") == 0)
	{
		jsonSequence_setIndex(index);
		return jsonSequence_getJsonObject();
	}

	return NULL;
}

static void numberReceived(char* name, uint32_t number)
{
	if(strcmp(name, "StartSequence") == 0)
	{
		LED_startLedSequence(number);
	}
	else if(strcmp(name, "StartSequenceAll") == 0)
	{
		uint8_t i;
		for(i = 0; i < 20; i++)
		{
			LED_startLedSequence(i);
		}
	}
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
