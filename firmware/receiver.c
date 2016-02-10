/*
 * receiver.c -- functions to initialize and access the data from the receiver
 *
 * (c) 2016 Prof Dr Andreas Mueller, Hochschule Rapperswil
 */
#include <receiver.h>
#include <avr/io.h>
#include <led.h>
#include <motor.h>

static unsigned char	last = 0;
static unsigned char	locked = 0;

/**
 * \brief get the current button state output by the receiver
 */
unsigned char	recv_get() {
	return (PIN7 >> 3) | ((locked) ? 0x80 : 0x00);
}

void	recv_setup(void) __attribute__ ((constructor));
void	recv_setup(void) {
	// enable the pull up
	PORTD |= _BV(PORTD7)|_BV(PORT6)|_BV(PORT5)|_BV(PORT4)|_BV(PORT3);
	DDRD &= ~(_BV(DDD7)|_BV(DDD6)|_BV(DDD5)|_BV(DDD4)|_BV(DDD3));
}

/**
 * \brief Lock the device
 *
 * In the locked state, the focus position can only be changed after the
 * two buttons C and D have been pressed for at least 2 seconds. This
 * method is called from the handler of the USB LOCK request handler
 * when wValue is different from 0.
 */
void	recv_lock() {
	locked = 1;
	led_on();
}

/**
 * \brief Unlock the device
 *
 * This method allows to unlock the device. It is e.g. by the unlock
 * procedure (see the function recv_handle below) or by the USB LOCK
 * request with wValue = 0
 */
void	recv_unlock() {
	locked = 0;
	led_off();
}

// to unlock, press buttons C and D for at least two seconds, i.e. for
// at least 2000 counts of the unlock-counter
static unsigned short	unlockcounter = 0;

/**
 * \brief handler to be called during the timer interrupt
 *
 * The handler checks the state of the buttons and changes the behaviour
 * of the focuser accordingly
 */
void	recv_handler() {
	unsigned char	now = recv_get();
	// locking/unlocking
	if ((now & RECV_C) && (now & RECV_D)) {
		unlockcounter++;
		goto done;
	} else {
		if (unlockcounter > 2000) {
			locked = (locked) ? 0 : 1;
		}
		unlockcounter = 0;
		goto done;
	}
	// if the state has not changed, nothing needs to be done
	if (now == last) {
		return;
	}
	// handle all possible combinations 
	unsigned char	speed = (now & RECV_C) ? SPEED_FAST : SPEED_SLOW;
	switch (0x3 & now) {
	case 0x1:
		motor_moveto(0xffff, speed);
		break;
	case 0x2:
		motor_moveto(0, speed);
		break;
	case 0x0:
	case 0x3:
		motor_stop();
		break;
	}
done:
	last = now;
}
