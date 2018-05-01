/*
 * eeprom.h -- Variables that go into the EEPROM
 *
 * (c) 2018 Prof Dr Andreas Müller, Hochschule Rapperswil
 */
#ifndef _eeprom_h
#define _eeprom_h

#include <avr/pgmspace.h>
#include <LUFA/Drivers/USB/USB.h>

extern uint32_t EEMEM	position;
extern const USB_Descriptor_String_t EEMEM	SerialNumberString;

#endif /* _eeprom_h */
