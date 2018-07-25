/*
 * json_pattern.c
 *
 *  Created on: 25 juil. 2018
 *      Author: coren
 */


#include "json/json_pattern.h"
#include "data/pattern.h"

#include <stdlib.h>
#include <string.h>

static JsonObject_t* objectStart(char* name);
static JsonObject_t* arrayObjectStart(char* name, uint16_t index);
static void numberReceived(char* name, uint32_t number);
static void stringReceived(char* name, char* string);
static void arrayNumberReceived(char* name, uint32_t number, uint16_t index);
static void arrayStringReceived(char* name, char* string, uint16_t index);

static JsonObject_t jsonObject_Pattern = {
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
	if(strcmp(name, "PatternDataIndex") == 0)
	{
		pattern_setPatternDataFromIndex(number);
	}
	else if(strcmp(name, "Interval") == 0)
	{
		pattern_setPatternInterval(number);
	}
	else if(strcmp(name, "StartPosition") == 0)
	{
		pattern_setPatternStartPosition(number);
	}
	else if(strcmp(name, "Duration") == 0)
	{
		pattern_setPatternDuration(number);
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

JsonObject_t* jsonPattern_getJsonObject()
{
	return &jsonObject_Pattern;
}


void jsonPattern_setPatternIndex(uint16_t index)
{
	pattern_setPatternIndex(index);
}

void jsonPattern_resetPatternCount()
{
	pattern_resetPatternCount();
}

void jsonPattern_addNewPattern()
{
	pattern_addNewPattern();
}

