/*
 * json_sequence.c
 *
 *  Created on: 25 juil. 2018
 *      Author: coren
 */

#include "json/json_sequence.h"
#include "json/json_pattern.h"
#include "data/sequence.h"

#include <stdlib.h>
#include <string.h>

static JsonObject_t* objectStart(char* name);
static JsonObject_t* arrayObjectStart(char* name, uint16_t index);
static void numberReceived(char* name, uint32_t number);
static void stringReceived(char* name, char* string);
static void arrayNumberReceived(char* name, uint32_t number, uint16_t index);
static void arrayStringReceived(char* name, char* string, uint16_t index);

static JsonObject_t jsonObject_Sequence = {
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
	if(strcmp(name, "Patterns") == 0)
	{
		// Add the new pattern to the main list of patterns
		jsonPattern_addNewPattern();

		// Specifically add this pattern to the sequence list of patterns
		sequence_addPattern(pattern_getCurrentPattern());

		return jsonPattern_getJsonObject();
	}

	return NULL;
}

static void numberReceived(char* name, uint32_t number)
{
	if(strcmp(name, "LedIndex") == 0)
	{
		sequence_setLedIndex(number);
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

JsonObject_t* jsonSequence_getJsonObject()
{
	return &jsonObject_Sequence;
}

void jsonSequence_setIndex(uint16_t index)
{
	sequence_setIndex(index);

	// If it is the first sequence, then we reset the pattern list too
	if(index == 0)
	{
		jsonPattern_resetPatternCount();
	}
}
