/*
 * version.c 
 *
 * (c) 2019 Prof Dr Andreas Müller, Hochschule Rapperswil
 */
#include <stdlib.h>
#include <stdio.h>
#include <libusb-1.0/libusb.h>
#include "../firmware/config.h"

void	show_version() {
	printf("fclient version: %s, built%s\n", VERSION, BUILDDATE);
	const struct libusb_version* version;
	version = libusb_get_version();
	printf("libusb version: %d.%d.%d.%d\n",
		version->major, version->minor, version->micro, version->nano);
}
