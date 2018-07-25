/*
 * pattern_data.c
 *
 *  Created on: 24 juil. 2018
 *      Author: coren
 */

#include <json/pattern_data.h>
#include <stdlib.h>
#include <string.h>


PatternData_t patternDatas[MAX_PATTERNS_DATA];

static uint16_t dataIndex;
static uint16_t data[PATTERN_DATA_SIZE];
static uint16_t dataPosition;


static void addValueToCurrentPattern(uint16_t value);

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
		addValueToCurrentPattern(number);
	}
}

static void arrayStringReceived(char* name, char* string, uint16_t index)
{

}

JsonObject_t* patternData_getJsonObject()
{
	return &jsonObject_PatternData;
}


void patternData_setDataIndex(uint16_t index)
{
	// New set of data, we start from 0 again
	if(index == 0)
	{
		dataPosition = 0;
	}

	dataIndex = index;

	// Reset the length and set the starting position
	patternDatas[index].size = 0;
	patternDatas[index].data = &(data[dataPosition]);
}

static void addValueToCurrentPattern(uint16_t value)
{
	if(dataPosition < PATTERN_DATA_SIZE)
	{
		// Fill the global data array
		data[dataPosition] = value;
		dataPosition++;

		// Our pattern data only increment its size
		patternDatas[dataIndex].size++;
	}
	else
	{
		// TODO: Warn the user we don't have enough space
	}
}
