/*
 * sequence.h
 *
 *  Created on: 4 juil. 2018
 *      Author: coren
 */

#ifndef SEQUENCE_H_
#define SEQUENCE_H_

#include "data/pattern.h"

#define MAX_SEQUENCE_PATTERNS 100

typedef struct _sequence_t{
	Pattern_t* patterns[MAX_SEQUENCE_PATTERNS];
	uint8_t ledIndex;
	uint8_t patterns_count;
} Sequence_t;

// Used by the json decoder
void sequence_setIndex(uint8_t index);
void sequence_setLedIndex(uint8_t ledIndex);
void sequence_addPattern(Pattern_t *pattern);

#endif /* SEQUENCE_H_ */
