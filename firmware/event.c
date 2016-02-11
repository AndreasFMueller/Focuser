/*
 * event.c -- enable event handling
 *
 * (c) 2015 Prof Dr Andreas Mueller, Hochschule Rapperswil
 */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */
#include <event.h>
#include <receiver.h>
#include <LUFA/Drivers/USB/Core/Events.h>
#include <avr/wdt.h>
#include <motor.h>
#include <timer.h>
#include <serial.h>

/**
 * \brief RESET request
 *
 * The RESET request is handled by simply setting the whatdog timer to
 * such a short interval that it is bound to be triggered.
 */
void	process_reset() {
	Endpoint_ClearSETUP();
	Endpoint_ClearStatusStage();
	// the usual method to set the watchdog timer timeout to
	// a short time does not really work, because interrupts happen
	// so often in this application. We use the reset flag from
	// the timer, which tells the timer interrupt no longer to reset
	// the watchdog timer, which will utlimately cause a watch dog timer
	// reset
	resetflag = 0;
}

/**
 * \brief SET request
 *
 * The SET request moves the focuser to a position indicated by the wValue
 * field in the request. If wIndex is nonzero, then the movement is done
 * in high speed mode, otherwise in slow speed.
 */
void	process_set() {
	Endpoint_ClearSETUP();
	Endpoint_ClearStatusStage();
	if ((USB_ControlRequest.wValue == 0)
		|| (USB_ControlRequest.wValue == 0xffff)) {
		return;
	}
	motor_moveto(USB_ControlRequest.wValue,
		(USB_ControlRequest.wIndex) ? SPEED_FAST : SPEED_SLOW);
}

/**
 * \brief LOCK request
 *
 * The LOCK request disables focus changes 
 */
void	process_lock() {
	Endpoint_ClearSETUP();
	Endpoint_ClearStatusStage();
	if (USB_ControlRequest.wIndex) {
		recv_lock();
	} else {
		recv_unlock();
	}
}

/**
 * \brief STOP request
 *
 * The stop request works by setting the target to the current counter state
 */
void	process_stop() {
	Endpoint_ClearSETUP();
	Endpoint_ClearStatusStage();
	motor_stop();
}

/**
 * \brief SERIAL request
 *
 * read the new serial number in ascii from the USB and copy it into the
 * 
 */
void	process_serial() {
	Endpoint_ClearSETUP();
	unsigned char	l = USB_ControlRequest.wLength;
	if (l <= 7) {
		Endpoint_Read_Control_Stream_LE(serialbuffer, l);
	}
	Endpoint_ClearIN();
	serialbuffer[l] = '\0';
	newserial = 1;
}

#define	is_control() \
	((USB_ControlRequest.bmRequestType & CONTROL_REQTYPE_TYPE) 	\
		== REQTYPE_VENDOR) 					\
	&&								\
	((USB_ControlRequest.bmRequestType & CONTROL_REQTYPE_RECIPIENT)\
		== REQREC_DEVICE)

#define	is_incoming() \
	((USB_ControlRequest.bmRequestType & CONTROL_REQTYPE_DIRECTION)	\
		== REQDIR_HOSTTODEVICE)

#define	is_outgoing() \
	((USB_ControlRequest.bmRequestType & CONTROL_REQTYPE_DIRECTION)	\
		== REQDIR_DEVICETOHOST)

/**
 * \brief Get the current focuser state
 *
 * The GET request returns a single byte containing the state of all
 * the output ports.
 */
void	process_get() {
	Endpoint_ClearSETUP();
	unsigned short	v[3];
	v[0] = motor_current();
	v[1] = motor_target();
	v[2] = motor_speed();
	Endpoint_Write_Control_Stream_LE((void *)v, 6);
	Endpoint_ClearOUT();
}

/**
 * \brief RCVR request implementation
 *
 * retrieve the current state of the input pins from the RF receiver
 */
void	process_rcvr() {
	Endpoint_ClearSETUP();
	unsigned char	v = recv_get();
	Endpoint_Write_Control_Stream_LE((void *)&v, 1);
	Endpoint_ClearOUT();
}

/**
 * \brief SAVED request implementation
 */
void	process_saved() {
	Endpoint_ClearSETUP();
	unsigned short	v = lastsaved;
	Endpoint_Write_Control_Stream_LE((void *)&v, 2);
	Endpoint_ClearOUT();
}

/**
 * \brief Control request event handler
 *
 * This implementation handles the control requests for the GuiderPort device
 */
void	EVENT_USB_Device_ControlRequest() {
	if (is_control()) {
		if (is_incoming()) {
			switch (USB_ControlRequest.bRequest) {
			case FOCUSER_RESET:
				process_reset();
				break;
			case FOCUSER_SET:
				process_set();
				break;
			case FOCUSER_LOCK:
				process_lock();
				break;
			case FOCUSER_STOP:
				process_stop();
				break;
			case FOCUSER_SERIAL:
				process_serial();
				break;
			}
		}
		if (is_outgoing()) {
			switch (USB_ControlRequest.bRequest) {
			case FOCUSER_GET:
				process_get();
				break;
			case FOCUSER_RCVR:
				process_rcvr();
				break;
			case FOCUSER_SAVED:
				process_saved();
				break;
			}
		}
	}
}

