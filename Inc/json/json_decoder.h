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

typedef struct _JsonObject_t {
	// Called when a new object is started, we want to get the new object pointer if it exists
	struct _JsonObject_t* (*objectStart)(char* name);
	struct _JsonObject_t* (*arrayObjectStart)(char* name, uint16_t index);

	// Called to indicate a new data has been received for this object
	void (*numberReceived)(char* name, uint32_t number);
	void (*stringReceived)(char* name, char* string);
	void (*arrayNumberReceived)(char* name, uint32_t number, uint16_t index);
	void (*arrayStringReceived)(char* name, char* string, uint16_t index);
} JsonObject_t;

extern JsonObject_t jsonMainObject;

void jsonDecoding(uint8_t* buf, uint16_t len);

#endif /* JSON_DECODER_H_ */
