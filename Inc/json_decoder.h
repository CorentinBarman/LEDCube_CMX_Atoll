/*
 * json_decoding.h
 *
 *  Created on: 27 juin 2018
 *      Author: coren
 */

#ifndef JSON_DECODER_H_
#define JSON_DECODER_H_

#include <stdint.h>

// This is the maximum size a string stored in the json received can be
#define JSON_MAX_STRING_LENGTH 50

typedef struct _JsonNumber32_t {
	char *name;
	uint32_t value;
	uint8_t flag;
} JsonNumber32_t;

typedef struct _JsonString_t {
	char *name;
	char value[JSON_MAX_STRING_LENGTH];
	uint8_t flag;
} JsonString_t;

typedef struct _JsonArray16_t {
	char *name;
	uint16_t *values;
	uint8_t flag;
} JsonArray16_t;

typedef struct _JsonObject_t {
	char *name;

	uint8_t numbers_count;
	JsonNumber32_t **numbers;
	uint8_t strings_count;
	JsonString_t **strings;
	uint8_t arrays_count;
	JsonArray16_t **arrays;

	uint8_t objects_count;
	struct _JsonObject_t **objects;

	// Function to call once an object is received
	void (*objectReceived)();
} JsonObject_t;

void jsonDecoding(uint8_t* buf, uint16_t len);

#endif /* JSON_DECODER_H_ */
