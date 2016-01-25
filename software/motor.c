/*
 * motor.c -- implementation of motor control functions
 *
 * (c) 2016 Prof Dr Andreas Mueller, HOchschule Rapperswil
 */
#include <motor.h>
#include <avr/io.h>

#define	MOTOR_ENABLE	PORTC2
#define MOTOR_MS1	PORTC4
#define MOTOR_MS2	PORTC5
#define MOTOR_MS3	PORTC6
#define MOTOR_RESET	PORTB7
#define	MOTOR_SLEEP	PORTB6
#define MOTOR_STEP	PORTB5
#define MOTOR_DIR	PORTB4

#define	MS_MASK		(_BV(PORTC4)|_BV(PORTC5)|_BV(PORTC6))

void	motor_set_step(unsigned char step) {
	unsigned char	c = PORTC;
	c = (c & ~MS_MASK) | ((step << 4) & MS_MASK);
	PORTC = c;
}

unsigned char	motor_get_step() {
	return (PORT & MS_MASK) >> 4;
}

static unsigned short	current;
static unsigned short	target;
static unsigned char	speed = SPEED_SLOW;
static unsigned char	slow;

static unsigned short	lastsaved;
static unsigned long	timelastchanged = 0;

/**
 * \brief Get the current target setting
 *
 * Get the current target value. The host software can detect whether
 * the motor is moving by checking whether the target and the current
 * position are different.
 */
unsigned short	motor_target() {
	return target;
}

#define	DIVISOR_FAST	2
#define DIVISOR_SLOW	16

/**
 * \brief Handler called from the timer interrupt
 *
 * This function is called from the timer interrupt. If the current position
 * differs from the target, an impuls to the stepper motor driver is generated.
 * The current position in the variable current is then incremented.
 */
void	motor_handle() {
	if (current == target) {
		if (timelastchanged < 0xffffffff) {
			timelastchanged++;
		}
		return;
	}
	// set the direction
	if (--slow) {
		if (target > current) {
			PORTB |= _BV(MOTOR_DIR);
			current++;
		} else {
			PORTB &= ~_BV(MOTOR_DIR);
			current--;
		}
		// send a pulse
		PORTB |= _BV(MOTOR_STEP);
		PORTB &= ~_BV(MOTOR_STEP);
		// set the speed divisor to the appropriate value
		slow = (speed == SPEED_FAST) ? DIVISOR_FAST : DIVISOR_SLOW;
		timelastchanged = 0;
	}
}

void	motor_moveto(unsigned short position, unsigned char _speed) {
	GlobalInterrupDisable();
	target = position;
	speed = _speed;
	GlobalInterrupEnable();
}

/**
 * \brief Stop the motor
 *
 * The motor is stopped by setting the target value to the current value,
 * which implies that there is no need to step any further.
 */
void	motor_stop() {
	GlobalInterrupDisable();
	if (current != target) {
		target = current;
	}
	GlobalInterrupEnable();
}

void	motor_setup(void) __attribute__ ((constructor));
/**
 * \brief Setup for the motor driver
 *
 * This constructor should be called during initialization to properly
 * initialize the outputs to the Pololu stepper driver.
 */
void	motor_setup(void) {
	motor_step_step(MOTOR_FULL);
	PORTC |= _BV(MOTOR_ENABLE);
	PORTB &= ~_BV(MOTOR_SLEEP);
	PORTB &= ~_BV(MOTOR_STEP);
	PORTB &= ~_BV(MOTOR_DIR);
	PORTB |= ~_BV(MOTOR_RESET);
	DDRC |= 0x74;
	DDRB |= 0xf0;
	// read the current value from the EEPROM
	current = eeprom_read_word(16);
	lastsaved = current;
	// set the target also to the current, so we don't move anything
	target = current;
}
