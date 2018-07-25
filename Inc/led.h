/*
 * led_control.h
 *
 *  Created on: 2 juil. 2018
 *      Author: coren
 */

#ifndef LED_H_
#define LED_H_

#include <json/json_decoder.h>
#include "data/sequence.h"
#include <stdint.h>
#include <stdlib.h>

/**
 * @brief Used to link with the JSON decoder
 *
 * Contains the definition of each parameter, and the name that has
 * to be searched for while browsing the JSON.
 */
extern JsonObject_t ledJSON;

void LED_changePWM_Duty(uint8_t index, uint16_t duty);
void LED_timer_interval_irq();
void LED_init_for_measures();
void LED_startLedSequence(uint8_t ledIndex);
void LED_setSequence(uint8_t index, Sequence_t *sequence);

#endif /* LED_H_ */
