/*
 * descriptor.h -- declarations of descriptors
 *
 * (c) 2015 Prof Dr Andreas Mueller, Hochschule Rapperswil
 */
#ifndef _descriptor_h
#define _descriptor_h

#include <avr/pgmspace.h>
#include <LUFA/Drivers/USB/USB.h>
#include <eeprom.h>

typedef struct {
	USB_Descriptor_Configuration_Header_t Config;
} USB_Descriptor_Configuration_t;

enum StringDescriptors_t {
	STRING_ID_Language     = 0,
	STRING_ID_Manufacturer = 1,
	STRING_ID_Product      = 2,
	STRING_ID_SerialNumber = 3,
};

#endif /* _descriptor_h */
