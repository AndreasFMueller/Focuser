/*
 * timer.c -- timer implementation
 *
 * (c) 2016 Prof Dr Andreas Mueller, Hochschule Rapperswil
 */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */
#include <avr/wdt.h>
#include <avr/interrupt.h>
#include <avr/io.h>
#include <timer.h>

void	timer_start() {
	TIMSK1 |= _BV(OCIE1);
	wdt_enable();
}

void	timer_stop() {
	TIMSK1 &= ~_BV(OCIE1);
	wdt_disable();
}

void	timer_setup(void) __attribute__ ((constructor));
void	timer_setup(void) {
	// prescaler 8 or 1, default is 1, CTC
	TCCR1B = (0x2 << CS10) | (1 << WGM12); 
	TCCR1A = 0;
	OCR1A = 1000;
	TIMSK1 = _BV(OCIE1A);
}

extern void	motor_handler();

ISR(TIMER1_COMPA_vect) {
	motor_handler();
	recv_handler();
	wdt_reset();
}
