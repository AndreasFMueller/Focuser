/*
 * serial.h -- functions to overwrite the serial number in EEPROM
 *
 * (c) 2016 Prof Dr Andreas Mueller, Hochschule Rapperswil
 */
#ifndef _serial_h
#define _serial_h

extern volatile unsigned char	newserial;
extern char	serialbuffer[8];

extern void	serial_write();
extern void	serial_read();

#endif /* _serial_h */
