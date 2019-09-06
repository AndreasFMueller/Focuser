/*
 * motor.c -- implementation of motor control functions
 *
 * (c) 2016 Prof Dr Andreas Mueller, HOchschule Rapperswil
 */
#include <motor.h>
#include <avr/io.h>
#include <LUFA/Platform/Platform.h>
#include <LUFA/Drivers/USB/USB.h>
#include <avr/eeprom.h>
#include <led.h>
#include <eeprom.h>

#define	MOTOR_ENABLE	PORTC2
#define MOTOR_MS1	PORTC4
#define MOTOR_MS2	PORTC5
#define MOTOR_MS3	PORTC6
#define MOTOR_RESET	PORTB7
#define	MOTOR_SLEEP	PORTB6
#define MOTOR_STEP	PORTB5
#define MOTOR_DIR	PORTB4

#define	MS_MASK		(_BV(PORTC4)|_BV(PORTC5)|_BV(PORTC6))

volatile unsigned char	saveneeded = 0;

/**
 * \brief Set the motor stepping mode
 */
void	motor_set_step(unsigned char step) {
	unsigned char	c = PORTC;
	c = (c & ~MS_MASK) | ((step << 4) & MS_MASK);
	PORTC = c;
}

/**
 * \brief Get the motor stepping mode
 */
unsigned char	motor_get_step() {
	return (PORTC & MS_MASK) >> 4;
}

static volatile uint32_t	current;
static volatile uint32_t	target;
static unsigned char	speed = SPEED_SLOW;
static unsigned char	divisor;
static unsigned char	microstep;
static unsigned char	stepsize;

/**
 * \brief Get the current motor position
 */
uint32_t	motor_current() {
	return current;
}

uint32_t	motor_speed() {
	return speed;
}

volatile uint32_t	lastsaved;
static volatile uint32_t	timelastchanged = 0;

/**
 * \brief Get the current target setting
 *
 * Get the current target value. The host software can detect whether
 * the motor is moving by checking whether the target and the current
 * position are different.
 */
uint32_t	motor_target() {
	return target;
}

#define	DIVISOR	2

/**
 * \brief save the current value
 */
void	motor_save() {
	GlobalInterruptDisable();
	eeprom_write_dword(&position, lastsaved);
	saveneeded = 0;
	GlobalInterruptEnable();
}

/**
 * \brief Handler called from the timer interrupt
 *
 * This function is called from the timer interrupt. If the current position
 * differs from the target, an impuls to the stepper motor driver is generated.
 * The current position in the variable current is then incremented.
 */
void	motor_handler() {
	if (current == target) {
		if (timelastchanged < 0xffffffff) {
			timelastchanged++;
		}
		if (timelastchanged == 120000) {
			if (lastsaved != current) {
				lastsaved = current;
				saveneeded = 1;
			}
		}
		return;
	}
	timelastchanged = 0;
	// set the direction
	if (0 == --divisor) {
		if (speed == SPEED_SLOW) {
			if (target > current) {
				microstep = (microstep + 1) % 16;
				if (0 == microstep) {
					PORTB |= _BV(MOTOR_DIR);
					current++;
				}
			} else {
				microstep = (microstep + 15) % 16;
				if (0 == microstep) {
					PORTB &= ~_BV(MOTOR_DIR);
					current--;
				}
			}
		} else {
			if (target > current) {
				microstep = (microstep + stepsize) % 16;
				if (0 == microstep) {
					PORTB |= _BV(MOTOR_DIR);
					current++;
				}
			} else {
				microstep = (microstep + (16 - stepsize)) % 16;
				if (0 == microstep) {
					PORTB &= ~_BV(MOTOR_DIR);
					current--;
				}
			}
		}
		// send a pulse
		PORTB |= _BV(MOTOR_STEP);
		PORTB &= ~_BV(MOTOR_STEP);
		// set the speed divisor to the appropriate value
		divisor = DIVISOR;
	}
}

void	motor_moveto(uint32_t position, unsigned char _speed) {
	GlobalInterruptDisable();
	target = position;
#if 0
	// if switching to slow speed, reset microstep counter
	if ((speed != _speed) && (_speed == SPEED_SLOW)) {
		microstep = 0;
	}
#else
	// always reset the microstep counter
	microstep = 0;
#endif
	speed = _speed;
	switch (speed) {
	case SPEED_FAST: {
			unsigned char	s = motor_faststep();
			motor_set_step(s);
			switch (s) {
				case 0:	stepsize = 16; break;
				case 1:	stepsize =  8; break;
				case 2:	stepsize =  4; break;
				case 3:	stepsize =  2; break;
			}
		}
		break;
	case SPEED_SLOW:
		motor_set_step(STEP_SIXTEENTH);
		break;
	}
	GlobalInterruptEnable();
}

void	motor_position(uint32_t position) {
	GlobalInterruptDisable();
	target = position;
	current = position;
	GlobalInterruptEnable();
}

/**
 * \brief Stop the motor
 *
 * The motor is stopped by setting the target value to the current value,
 * which implies that there is no need to step any further.
 */
void	motor_stop() {
	GlobalInterruptDisable();
	if (current != target) {
		target = current;
	}
	GlobalInterruptEnable();
}

void	motor_setup(void) __attribute__ ((constructor));
/**
 * \brief Setup for the motor driver
 *
 * This constructor should be called during initialization to properly
 * initialize the outputs to the Pololu stepper driver.
 */
void	motor_setup(void) {
	motor_set_step(STEP_SIXTEENTH);
	PORTC &= ~_BV(MOTOR_ENABLE);
	PORTB |= _BV(MOTOR_SLEEP);
	PORTB &= ~_BV(MOTOR_STEP);
	PORTB &= ~_BV(MOTOR_DIR);
	PORTB |= _BV(MOTOR_RESET);
	DDRC |= 0x74;
	DDRB |= 0xf0;
	// read the current value from the EEPROM
	current = eeprom_read_dword(&position);
	lastsaved = current;
	// set the target also to the current, so we don't move anything
	target = current;
	microstep = 0;
}

static uint8_t	lasttopspeed = 0xff;

/**
 * \brief Get the top speed byte (0 for fastest, 4 for slowest)
 *
 * \return	the top speed byte
 */
uint8_t	motor_get_topspeed() {
	if (lasttopspeed == 0xff) {
		lasttopspeed = eeprom_read_byte(&topspeed);
		if (lasttopspeed > 3) {
			lasttopspeed = 0;
			eeprom_write_byte(&topspeed, lasttopspeed);
		}
	}
	return lasttopspeed;
}

/**
 * \brief Set the top speed in the EEPROM
 *
 * \param settopspeed	the top speed value to remember
 */
void	motor_set_topspeed(uint8_t settopspeed) {
	if (settopspeed > 3) {
		return;
	}
	eeprom_write_byte(&topspeed, settopspeed);
	lasttopspeed = settopspeed;
}

/**
 * \brief Get the step configuration byte from the 
 *
 * \return	get the step configuration byte for the stepper driver
 */
uint8_t	motor_faststep() {
	switch (motor_get_topspeed()) {
	case 0:	return STEP_FULL;
	case 1:	return STEP_HALF;
	case 2:	return STEP_QUARTER;
	case 3: return STEP_EIGHTH;
	}
	/* by default, give the fastest speed (for compatibility) 	*/
	return STEP_FULL;
}
