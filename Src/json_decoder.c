/*
 * json_decoding.c
 *
 *  Created on: 27 juin 2018
 *      Author: coren
 */

#include <json_decoder.h>
#include <led.h>
#include "vcp_communication.h"
#include <stdlib.h>
#include <string.h>

// ----------------------------------
// JSON decoding states and variables



// This is the numbers of opening brackets in a row that can be used in a json
// Warning: No check is done for now to verify that the number of levels isn't exceeded
#define JSON_MAX_LEVELS 5

// Result of the decoding function posible values
enum JsonObjectResult { JSON_OK, JSON_END_OF_OBJECT, JSON_NEW_OBJECT};

// JSON state machine
enum JsonState { JSON_IDLE, JSON_MAIN_OBJECT,
	JSON_START, JSON_NAME_STRING_DECODING, JSON_STRING_DECODING, JSON_WAIT_SEPARATOR, JSON_CHOOSE_VALUE_TYPE,
	JSON_ARRAY_DECODING, JSON_NUMBER_DECODING, JSON_END_OR_NEW_VALUE};

// The currently decoded data value
uint16_t jsonArrayValuePosition[JSON_MAX_LEVELS];
uint32_t jsonNumberDecoded;
uint8_t jsonStringValueDecodedPosition;
char jsonStringValueDecoded[JSON_MAX_STRING_LENGTH];

// This will contain the decoded string used to determine which value will be modified by the JSON
char jsonDecodedString[JSON_MAX_LEVELS][JSON_MAX_STRING_LENGTH];
uint8_t jsonDecodedStringPosition;

// This contains the state for each level of decoding
enum JsonState jsonObjectStates[JSON_MAX_LEVELS];

// This is the state of the main level of decoding, it detects the start and the end of the JSON and initiate the first object
enum JsonState jsonMainDecoderState = JSON_IDLE;

static uint8_t jsonCurrentLevel;

// ------------------------------------------------------------------
// General 

typedef struct General_Decoded_t{
	char name[JSON_MAX_STRING_LENGTH];
	JsonNumber32_t pwm_period;
} GeneralConfig;

GeneralConfig generalConfig = {
	"General",
	{"PWM Period", 0, 0}
};

// Fill this array with the numbers uin16_t we want to monitor with JSON
JsonNumber32_t *jsonGeneralNumbers[] = {
	&(generalConfig.pwm_period)
};

void generalObjectReceived();

JsonObject_t generalJSON = {
	"General",
	1, jsonGeneralNumbers,
	0, NULL,
	0, NULL,
	0, NULL,
	generalObjectReceived
};


// ------------------------------------------------------------------
// Main Object

JsonObject_t *jsonMainObjects[] = {
	&ledJSON,
	&generalJSON
};

JsonObject_t mainJson = {
	"",
	0, NULL, 0, NULL, 0, NULL,
	2, jsonMainObjects
};

JsonObject_t *currentObject;
JsonObject_t *objectsStack[JSON_MAX_LEVELS];

// End of JSON objects initialisation
// ------------------------------------------------------------------



void generalObjectReceived()
{
	// Debug -> send back the data read
	VCP_SendString("General : ");

	VCP_SendJsonObjectReceivedValues(currentObject);
}


void jsonResetObjectsFlags(JsonObject_t *object)
{
	uint8_t i;

	for(i = 0; i < object->numbers_count; i++)
	{
		object->numbers[i]->flag = 0;
	}

	for(i = 0; i < object->strings_count; i++)
	{
		object->strings[i]->flag = 0;
	}

	for(i = 0; i < object->arrays_count; i++)
	{
		object->arrays[i]->flag = 0;
	}
}


void jsonVCPSendObjectStack()
{
	uint8_t i = 0;

	VCP_SendString(jsonDecodedString[0]);

	for(i = 1; i <= jsonCurrentLevel; i++)
	{
		VCP_SendString(", ");
		VCP_SendString(jsonDecodedString[i]);
	}

	VCP_SendString(" : ");
}


void jsonStringReceived()
{
	uint8_t i;

	if(!currentObject)
		return;

	for(i = 0; i < currentObject->strings_count; i++)
	{
		if(strcmp(jsonDecodedString[jsonCurrentLevel], currentObject->strings[i]->name) == 0)
		{
			strcpy(currentObject->strings[i]->value, jsonStringValueDecoded);
			currentObject->strings[i]->flag = 1;
		}
	}
}

void jsonNumberReceived()
{
	uint8_t i;

	if(!currentObject)
		return;

	for(i = 0; i < currentObject->numbers_count; i++)
	{
		if(strcmp(jsonDecodedString[jsonCurrentLevel], currentObject->numbers[i]->name) == 0)
		{
			currentObject->numbers[i]->value = jsonNumberDecoded;
			currentObject->numbers[i]->flag = 1;
		}
	}
}

void jsonArrayValueReceived()
{
	uint8_t i;

	if(!currentObject)
		return;

	for(i = 0; i < currentObject->arrays_count; i++)
	{
		if(strcmp(jsonDecodedString[jsonCurrentLevel], currentObject->arrays[i]->name) == 0)
		{
			currentObject->arrays[i]->values[jsonArrayValuePosition[jsonCurrentLevel]] = jsonNumberDecoded;
			currentObject->arrays[i]->flag = 1;
		}
	}
}

void jsonStartOfObject()
{
	uint8_t i;

	objectsStack[jsonCurrentLevel] = currentObject;

	if(!currentObject)
		return;

	for(i = 0; i < currentObject->objects_count; i++)
	{
		if(strcmp(jsonDecodedString[jsonCurrentLevel], currentObject->objects[i]->name) == 0)
		{
			currentObject = currentObject->objects[i];
			jsonResetObjectsFlags(currentObject);
			return;
		}
	}

	currentObject = NULL;
}


void jsonObjectReceived()
{
	// Call the object function handler
	if(currentObject && currentObject->objectReceived != NULL)
	{
		currentObject->objectReceived();
	}

	// Select the new active object
	currentObject = objectsStack[jsonCurrentLevel-1];
}


// We will receive all characters since an object has been detected. The first character will be "{" and we have to detect the end and report back
enum JsonObjectResult jsonParseObject(char c, uint8_t currentLevel)
{
	switch(jsonObjectStates[currentLevel])
	{
	// Wait for start of string
	case JSON_START:
		if(c == '"')
		{
			// Start of a string
			jsonObjectStates[currentLevel] = JSON_NAME_STRING_DECODING;
			jsonDecodedStringPosition = 0;
		}
		else if(c == '}')
		{
			return JSON_END_OF_OBJECT;
		}
		break;

	case JSON_NAME_STRING_DECODING:
		if(c == '"')
		{
			// Add end of string character
			jsonDecodedString[currentLevel][jsonDecodedStringPosition] = '\0';

			jsonObjectStates[currentLevel] = JSON_WAIT_SEPARATOR;
		}
		else
		{
			// Verify we do not overflow our text data
			if(jsonDecodedStringPosition < JSON_MAX_STRING_LENGTH - 1)
			{
				jsonDecodedString[currentLevel][jsonDecodedStringPosition] = c;
				jsonDecodedStringPosition++;
			}
		}
		break;

	case JSON_WAIT_SEPARATOR:
		if(c == ':')
		{
			jsonObjectStates[currentLevel] = JSON_CHOOSE_VALUE_TYPE;
		}
		break;

	case JSON_CHOOSE_VALUE_TYPE:
		if(c == '{')
		{
			jsonObjectStates[currentLevel] = JSON_END_OR_NEW_VALUE;
			return JSON_NEW_OBJECT;
		}
		else if(c == '"')
		{
			jsonObjectStates[currentLevel] = JSON_STRING_DECODING;
			jsonStringValueDecodedPosition = 0;
		}
		else if(c == '[')
		{
			jsonObjectStates[currentLevel] = JSON_ARRAY_DECODING;
			jsonArrayValuePosition[currentLevel] = 0;
			jsonNumberDecoded = 0;
		}
		else if(c >= '0' && c <= '9')
		{
			jsonObjectStates[currentLevel] = JSON_NUMBER_DECODING;
			// Store the first digit in the decoded value (-48 : refer to ASCII table)
			jsonNumberDecoded = c - 48;
		}
		break;

	case JSON_STRING_DECODING:
		if(c == '"')
		{
			// Add end of string character
			jsonStringValueDecoded[jsonStringValueDecodedPosition] = '\0';

			// Validate data
			jsonStringReceived();

			jsonObjectStates[currentLevel] = JSON_END_OR_NEW_VALUE;
		}
		else
		{
			// Verify we do not overflow our text data
			if(jsonStringValueDecodedPosition < JSON_MAX_STRING_LENGTH - 1)
			{
				jsonStringValueDecoded[jsonStringValueDecodedPosition] = c;
				jsonStringValueDecodedPosition++;
			}
		}
		break;

	case JSON_NUMBER_DECODING:
		// A new value will be given for this level
		if(c == ',')
		{
			// Validate number
			jsonNumberReceived();

			jsonObjectStates[currentLevel] = JSON_START;
		}
		// We arrived to the end of the object, we must go back a level
		else if(c == '}')
		{
			// Validate number
			jsonNumberReceived();

			jsonObjectStates[currentLevel] = JSON_START;
			return JSON_END_OF_OBJECT;
		}
		// Only the numbers characters are decoded as numbers
		// Parse only unsigned integers numbers, decimal point and sign are ignored
		else if(c >= '0' && c <= '9')
		{
			jsonNumberDecoded *= 10;
			// Subtract 48 to c to get the actual number value (ASCII table)
			jsonNumberDecoded += (c - 48);
		}
		break;

	// Only support number arrays decoding
	case JSON_ARRAY_DECODING:

		if(c == ']')
		{
			jsonArrayValueReceived();

			jsonObjectStates[currentLevel] = JSON_END_OR_NEW_VALUE;
		}
		else if(c == ',')
		{
			jsonArrayValueReceived();
			jsonArrayValuePosition[currentLevel]++;
			jsonNumberDecoded = 0;
		}
		// Only the numbers characters are decoded as numbers
		// Parse only unsigned integers numbers, decimal point and sign are ignored
		else if(c >= '0' && c <= '9')
		{
			jsonNumberDecoded *= 10;
			// Subtract 48 to c to get the actual number value (ASCII table)
			jsonNumberDecoded += (c - 48);
		}

		break;

	case JSON_END_OR_NEW_VALUE:
		// A new value will be given for this level
		if(c == ',')
		{
			jsonObjectStates[currentLevel] = JSON_START;
		}
		// We arrived to the end of the object, we must go back a level
		else if(c == '}')
		{
			jsonObjectStates[currentLevel] = JSON_START;
			return JSON_END_OF_OBJECT;
		}
		break;

	default:
		jsonObjectStates[currentLevel] = JSON_IDLE;
		break;
	}

	return JSON_OK;
}




void jsonDecoding(uint8_t* buf, uint16_t len)
{
	uint16_t index;

	// Decode the partial json data
	for(index = 0; index < len; index++)
	{
		// We only detect the main object at this stage, the decoding is done inside jsonParseObject
		char c = buf[index];
		uint8_t result;

		switch(jsonMainDecoderState)
		{
		case JSON_IDLE:
			if(c == '{')
			{
				// Start of a json file
				jsonMainDecoderState = JSON_MAIN_OBJECT;

				// Initialise the first object level
				jsonCurrentLevel = 0;
				jsonObjectStates[jsonCurrentLevel] = JSON_START;
				currentObject = &mainJson;
				jsonResetObjectsFlags(currentObject);
			}
			break;

		case JSON_MAIN_OBJECT:
			result = jsonParseObject(c, jsonCurrentLevel);

			// The current object has finished
			if(result == JSON_END_OF_OBJECT)
			{
				// End of JSON file
				if(jsonCurrentLevel == 0)
				{
					jsonMainDecoderState = JSON_IDLE;
				}
				else
				{
					jsonObjectReceived();
					jsonCurrentLevel--;
				}
			}
			// A new object has been detected
			else if(result == JSON_NEW_OBJECT)
			{
				if(jsonCurrentLevel == JSON_MAX_LEVELS-1)
				{
					// We can't process the file because we didn't allocate sufficient memory!
				}
				else
				{
					jsonStartOfObject();
					jsonCurrentLevel++;
					jsonObjectStates[jsonCurrentLevel] = JSON_START;
				}
			}

			break;

		default:
			jsonMainDecoderState = JSON_IDLE;
			break;
		}
	}
}
