/*
 * eeprom.c -- stuff that goes into the EEPROM
 *
 * (c) 2018 Prof Dr Andreas Müller, Hochschule Rapperswil
 */
#include <eeprom.h>

uint32_t        EEMEM   position = 0x800000;

// CONFIGURATION: serial number
USB_Descriptor_String_t EEMEM SerialNumberString
	= USB_STRING_DESCRIPTOR(L"0000000");

