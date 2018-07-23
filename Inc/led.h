/*
 * led_control.h
 *
 *  Created on: 2 juil. 2018
 *      Author: coren
 */

#ifndef LED_H_
#define LED_H_

#include <json_decoder.h>
#include <stdint.h>
#include <stdlib.h>

/**
 * @brief Used to link with the JSON decoder
 *
 * Contains the definition of each parameter, and the name that has
 * to be searched for while browsing the JSON.
 */
extern JsonObject_t ledJSON;

void LED_change_PWM_duty(uint8_t index, uint16_t duty);
void LED_change_pattern_data(uint8_t index, uint16_t length, uint16_t *data);
void LED_change_pattern_interval(uint8_t index, uint16_t interval);
void LED_timer_interval_irq();
void LED_init_for_measures();
void LED_object_received();

#endif /* LED_H_ */
