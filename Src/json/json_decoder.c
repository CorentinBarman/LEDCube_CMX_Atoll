/*
 * json_decoding.c
 *
 *  Created on: 27 juin 2018
 *      Author: coren
 */

#include <json/json_decoder.h>
#include <json/vcp_communication.h>
#include <led.h>
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
	JSON_NUMBER_DECODING, JSON_END_OR_NEW_VALUE,
	JSON_ARRAY_CHOOSE_VALUE_TYPE, JSON_ARRAY_STRING_DECODING, JSON_ARRAY_NUMBER_DECODING, JSON_ARRAY_END_OR_NEW_VALUE};

// The currently decoded data value
static uint16_t jsonArrayValuePosition[JSON_MAX_LEVELS];
static uint32_t jsonNumberDecoded;
static uint8_t jsonStringValueDecodedPosition;
static char jsonStringValueDecoded[JSON_MAX_STRING_LENGTH];

// This will contain the decoded string used to determine which value will be modified by the JSON
static char jsonDecodedString[JSON_MAX_LEVELS][JSON_MAX_STRING_LENGTH];
static uint8_t jsonDecodedStringPosition;

// This contains the state for each level of decoding
static enum JsonState jsonObjectStates[JSON_MAX_LEVELS];

// This is the state of the main level of decoding, it detects the start and the end of the JSON and initiate the first object
static enum JsonState jsonMainDecoderState = JSON_IDLE;

// The current level increase with each '{' and decrease with each '}'
static uint8_t jsonCurrentLevel;

static JsonObject_t *currentObject;
static JsonObject_t *objectsStack[JSON_MAX_LEVELS];

static void jsonStringReceived()
{
	if(currentObject && currentObject->stringReceived)
		currentObject->stringReceived(jsonDecodedString[jsonCurrentLevel], jsonStringValueDecoded);
}

static void jsonNumberReceived()
{
	if(currentObject && currentObject->numberReceived)
		currentObject->numberReceived(jsonDecodedString[jsonCurrentLevel], jsonNumberDecoded);
}

static void jsonStartOfObject()
{
	objectsStack[jsonCurrentLevel] = currentObject;

	if(currentObject && currentObject->objectStart)
		currentObject = currentObject->objectStart(jsonDecodedString[jsonCurrentLevel]);
	else
		currentObject = NULL;
}


static void jsonArrayStringReceived()
{
	if(currentObject && currentObject->arrayStringReceived)
		currentObject->arrayStringReceived(jsonDecodedString[jsonCurrentLevel], jsonStringValueDecoded, jsonArrayValuePosition[jsonCurrentLevel]);
}

static void jsonArrayNumberReceived()
{
	if(currentObject && currentObject->arrayNumberReceived)
		currentObject->arrayNumberReceived(jsonDecodedString[jsonCurrentLevel], jsonNumberDecoded, jsonArrayValuePosition[jsonCurrentLevel]);
}

static void jsonArrayStartOfObject()
{
	objectsStack[jsonCurrentLevel] = currentObject;

	if(currentObject && currentObject->arrayObjectStart)
		currentObject = currentObject->arrayObjectStart(jsonDecodedString[jsonCurrentLevel], jsonArrayValuePosition[jsonCurrentLevel]);
	else
		currentObject = NULL;
}


// We will receive all characters since an object has been detected. The first character will be "{" and we have to detect the end and report back
static enum JsonObjectResult jsonParseObject(char c, uint8_t currentLevel)
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
			jsonObjectStates[currentLevel] = JSON_ARRAY_CHOOSE_VALUE_TYPE;
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


	case JSON_ARRAY_CHOOSE_VALUE_TYPE:
		if(c == '{')
		{
			jsonObjectStates[currentLevel] = JSON_ARRAY_END_OR_NEW_VALUE;
			return JSON_NEW_OBJECT;
		}
		else if(c == '"')
		{
			jsonObjectStates[currentLevel] = JSON_ARRAY_STRING_DECODING;
			jsonStringValueDecodedPosition = 0;
		}
		else if(c == '[')
		{
			// No arrays in arrays
		}
		else if(c >= '0' && c <= '9')
		{
			jsonObjectStates[currentLevel] = JSON_ARRAY_NUMBER_DECODING;
			// Store the first digit in the decoded value (-48 : refer to ASCII table)
			jsonNumberDecoded = c - 48;
		}
		break;

	case JSON_ARRAY_STRING_DECODING:
		if(c == '"')
		{
			// Add end of string character
			jsonStringValueDecoded[jsonStringValueDecodedPosition] = '\0';

			// Validate data
			jsonArrayStringReceived();

			jsonObjectStates[currentLevel] = JSON_ARRAY_END_OR_NEW_VALUE;
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

	case JSON_ARRAY_NUMBER_DECODING:
		// A new value will be given for this level
		if(c == ',')
		{
			// Validate number
			jsonArrayNumberReceived();
			jsonObjectStates[currentLevel] = JSON_ARRAY_CHOOSE_VALUE_TYPE;
			jsonArrayValuePosition[currentLevel]++;
		}
		else if(c == ']')
		{
			// Validate number
			jsonArrayNumberReceived();
			jsonObjectStates[currentLevel] = JSON_END_OR_NEW_VALUE;
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

	case JSON_ARRAY_END_OR_NEW_VALUE:

		if(c == ']')
		{
			jsonObjectStates[currentLevel] = JSON_END_OR_NEW_VALUE;
			jsonArrayValuePosition[currentLevel] = 0;
		}
		else if(c == ',')
		{
			jsonObjectStates[currentLevel] = JSON_ARRAY_CHOOSE_VALUE_TYPE;
			jsonArrayValuePosition[currentLevel]++;
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
				currentObject = &jsonMainObject;

				// Initialise the first object level
				jsonCurrentLevel = 0;
				jsonObjectStates[jsonCurrentLevel] = JSON_START;
				jsonArrayValuePosition[0] = 0;
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
					jsonCurrentLevel--;
					currentObject = objectsStack[jsonCurrentLevel];
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
					if(jsonObjectStates[jsonCurrentLevel] == JSON_ARRAY_END_OR_NEW_VALUE)
					{
						jsonArrayStartOfObject();
					}
					else
					{
						jsonStartOfObject();
					}

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
