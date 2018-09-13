/*
 * json_led.c
 *
 *  Created on: 30 juil. 2018
 *      Author: coren
 */



#include "json/json_led.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "led.h"

static JsonObject_t* objectStart(char* name);
static JsonObject_t* arrayObjectStart(char* name, uint16_t index);
static void numberReceived(char* name, uint32_t number);
static void stringReceived(char* name, char* string);
static void arrayNumberReceived(char* name, uint32_t number, uint16_t index);
static void arrayStringReceived(char* name, char* string, uint16_t index);

static JsonObject_t jsonObject_Led = {
		&objectStart,
		&arrayObjectStart,
		&numberReceived,
		&stringReceived,
		&arrayNumberReceived,
		&arrayStringReceived
};

static uint8_t ledIndex = 0;


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
	if(strcmp(name, "LedIndex") == 0)
	{
		ledIndex = number;
	}
	else if(strcmp(name, "Duty") == 0)
	{
		LED_changePWM_Duty(ledIndex, number);
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

JsonObject_t* jsonLed_getJsonObject()
{
	return &jsonObject_Led;
}
