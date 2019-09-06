/*
 * focuser.c -- main function for the focuser firmware
 *
 * (c) 2016 Prof Dr Andreas Mueller, Hochschule Rapperswil
 */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */
#include <LUFA/Platform/Platform.h>
#include <LUFA/Drivers/USB/USB.h>
#include <avr/wdt.h>
#include <util/delay.h>
#include <timer.h>
#include <led.h>
#include <motor.h>
#include <serial.h>
#include <descriptor.h>

/**
 * \brief Main function for the focuser firmware
 */
int	main(int argc, char *argv[]) {
	// initialize the 
	unsigned short	d = 10;
	led_on();
	while (d--) {
		_delay_ms(100);
		led_off();
		_delay_ms(100);
		led_on();
	}
	led_off();

	// copy the serial number string into RAM
	serial_read();

	// initialize USB, but USB requests will only be handled when
	// interrupts are enabled below
	USB_Init(USB_DEVICE_OPT_FULLSPEED);

	// start the timer, this also enables the watchdog timer
	timer_start();

	// enable interrupts so that USB processing can begin
	GlobalInterruptEnable();

	// do nothing	
	for (;;) {
		USB_USBTask();
		if (saveneeded) {
			motor_save();
		}
		if (newserial) {
			serial_write();
		}
	}
}

void	 wdt_init(void) __attribute((naked)) __attribute((section(".init3")));

/**
 * \brief method to initialize the watchdog timer
 *
 * The attributes set for this function above are designed so that
 * the function is automatically called in the startup sequence.
 */
void	wdt_init(void) {
	MCUSR = 0;
	wdt_disable();
}
