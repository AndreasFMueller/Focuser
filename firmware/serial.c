/*
 * serial.c -- overwrite the serial number in EEPROM
 *
 * (c) 2016 Prof Dr Andreas Mueller, Hochschule Rapperswil
 */
#include <config.h>
#include <serial.h>
#include <descriptor.h>
#include <eeprom.h>

volatile unsigned char	newserial = 0;
char serialbuffer[8] = "0000000";

void	serial_write() {
	// make sure 
	if (!newserial) {
		return;
	}

	// find out how many characters there are in the string
	unsigned char	l = 0;
	while (0 != serialbuffer[l]) {
		l++;
	}

	// convert the serial string into a USB string
	unsigned char	buffer[20];
	USB_Descriptor_String_t	*descriptor = (USB_Descriptor_String_t *)buffer;
	descriptor->Header.Size = sizeof(USB_Descriptor_Header_t) + 2 * l;
	descriptor->Header.Type = DTYPE_String;
	for (unsigned char i = 0; i < l; i++) {
		descriptor->UnicodeString[i] = serialbuffer[i];
	}

	// write the string to EEPROM
	eeprom_write_block(descriptor, &SerialNumberString,
		descriptor->Header.Size);

	// read the serial number
	serial_read();
	
	// remember that the serial number has been written
	newserial = 0;
}

void	serial_read() {
// copy the serial number string into RAM
	uint16_t Size = eeprom_read_byte(&SerialNumberString.Header.Size);
	eeprom_read_block(&SerialNumberMemoryString, &SerialNumberString,
		2 + Size);
}
