/*
 * event.h -- function to handle USB events
 *
 * (c) 2015 Prof Dr Andreas Mueller, Hochschule Rapperswil
 */
#ifndef _event_h
#define _event_h

#include <LUFA/Drivers/USB/USB.h>

/**
 * \brief Command codes for the GuiderPort commands
 *
 * A description of the commands is given in event.c where the
 * commands are implemented.
 */
#define	FOCUSER_RESET	0
#define	FOCUSER_GET	1
#define FOCUSER_SET	2
#define FOCUSER_LOCK	3

/**
 * \brief Event handle for control requests
 *
 * This function is called when a control request arrives at the device.
 * It implements the four commands understood by the device. Note that
 * this function must be linked, and sometimes the -u linker option is
 * needed to force the linker to do that.
 */
extern void	EVENT_USB_Device_ControlRequest();

#endif /* _event_h */
