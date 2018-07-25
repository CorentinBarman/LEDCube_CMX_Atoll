/*
 * led_control.c
 *
 *  Created on: 2 juil. 2018
 *      Author: coren
 */


#include <json/vcp_communication.h>
#include <led.h>
#include "main.h"
#include <memory.h>

#define MAX_LED_CHANNELS 20

#define TEST_LED

typedef struct _led_t{
	uint16_t duty;
	uint16_t pattern_data[1000];
	uint32_t pattern_interval;
	uint16_t pattern_length;

	uint32_t pattern_interval_count;
	uint16_t pattern_position;

	uint8_t pattern_on;
} led_t;


static led_t leds[MAX_LED_CHANNELS];
uint8_t testMeasureLED;


void LED_init_for_measures()
{
	uint8_t i, j;

	for(j = 0; j < 20; j++)
	{
		for(i = 1; i <= 16; i++)
		{
			leds[j].pattern_data[i-1] = i * 4095;
		}

		leds[j].pattern_length = 16;
		leds[j].pattern_interval = 180000;
		leds[j].pattern_position = 0;
		leds[j].pattern_interval_count = 0;
		leds[j].pattern_on = 0;
	}
#ifdef TEST_LED
	testMeasureLED = 1;
	leds[testMeasureLED].pattern_on = 1;
	LED_change_PWM_duty(testMeasureLED, leds[testMeasureLED].pattern_data[0]);
#endif
}

void LED_measure_update()
{
#ifdef TEST_LED
	leds[testMeasureLED].pattern_on = 0;
	LED_change_PWM_duty(testMeasureLED, 0);
	if(testMeasureLED == 2)
	{
		testMeasureLED = 0;
		// Stop the test
		return;
	}
	else
	{
		testMeasureLED++;
	}

	leds[testMeasureLED].pattern_on = 1;
	LED_change_PWM_duty(testMeasureLED, leds[testMeasureLED].pattern_data[0]);
#endif
}

void LED_change_PWM_duty(uint8_t index, uint16_t duty)
{
	leds[index].duty = duty;
	change_PWM_duty(index, leds[index].duty);
}

void LED_change_pattern_data(uint8_t index, uint16_t length, uint16_t *data)
{
	if(length == 0)
	{
		leds[index].pattern_on = 0;
		LED_change_PWM_duty(index, 0);
	}
	else
	{
		memcpy(leds[index].pattern_data, data, length * sizeof(uint16_t));
		leds[index].pattern_on = 1;
		leds[index].pattern_length = length;
	}
}

void LED_change_pattern_interval(uint8_t index, uint16_t interval)
{
	leds[index].pattern_interval = interval;
}

void LED_timer_interval_irq()
{
	uint8_t i;
	for(i = 0; i < MAX_LED_CHANNELS; i++)
	{
		if(leds[i].pattern_on)
		{
			leds[i].pattern_interval_count++;
			if(leds[i].pattern_interval_count >= leds[i].pattern_interval)
			{
				leds[i].pattern_interval_count = 0;

				// Load next value
				leds[i].pattern_position++;
				if(leds[i].pattern_position >= leds[i].pattern_length)
				{
					leds[i].pattern_position = 0;

					LED_change_PWM_duty(i, leds[i].pattern_data[leds[i].pattern_position]);
					LED_measure_update();
				}
				else
				{
					LED_change_PWM_duty(i, leds[i].pattern_data[leds[i].pattern_position]);
				}
			}
		}
	}
}


