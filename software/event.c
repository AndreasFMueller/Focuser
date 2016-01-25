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

/**
 * \brief RESET request
 *
 * The RESET request is handled by simply setting the whatdog timer to
 * such a short interval that it is bound to be triggered.
 */
void	process_reset() {
	Endpoint_ClearSETUP();
	Endpoint_ClearStatusStage();
	wdt_enable(WDTO_15MS);
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
	motor_moveto(USB_ControlRequest.wValue,
		(USB_ControlRequest.wIndex) ? SPEED_HIGH : SPEED_SLOW);
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
 * \brief Get the current LED state
 *
 * The GET request returns a single byte containing the state of all
 * the output ports.
 */
void	process_get() {
	Endpoint_ClearSETUP();
	unsigned short	v[2];
	v[0] = motor_current();
	v[1] = motor_target();
	Endpoint_Write_Control_Stream_LE((void *)&v, 4);
	Endpoint_ClearOUT();
}

/**
 * \brief GET RECV request
 *
 * retrieve the current state of the input pins from the RF receiver
 */
void	process_recv() {
	Endpoint_ClearSETUP();
	unsigned char	v = get_recv();
	Endpoint_Write_Control_Stream_LE((void *)&v, 1);
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
			case GUIDERPORT_RESET:
				process_reset();
				break;
			case GUIDERPORT_SET:
				process_set();
				break;
			case GUIDERPORT_LOCK:
				process_lock();
				break;
			}
		}
		if (is_outgoing()) {
			switch (USB_ControlRequest.bRequest) {
			case GUIDERPORT_GET:
				process_get();
				break;
			}
		}
	}
}

