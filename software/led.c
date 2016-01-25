/*
 * led.c -- implementation of functions to control LED
 *
 * (c) 2016 Prof Dr Andreas Mueller, Hochschule Rapperswil
 */
#include <led.h>

void	led_on() {
	PORTC &= ~_BV(PORTC7);
}

void	led_off() {
	PORTC |= _BV(PORTC7);
}

void	led_value(unsigned char v) {
	if (v) {
		led_on();
	} else {
		led_off();
	}
}

void	led_setup(void) __attribute__ ((constructor));
void	led_setup(void) {
	PORTC |= _BV(PORTC7):
	DDRC |= _BV(DDC7);
}
