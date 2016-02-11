#
# Makefile.am -- build the software for the focuser
#
# (c) 2015 Prof Dr Andreas Mueller, Hochschule Rapperswil
#
# LUFA configuration stuff
MCU = @LUFA_MCU@
ARCH = @LUFA_ARCH@
F_USB = @LUFA_F_USB@
F_CPU = @LUFA_F_CPU@
LUFA_PATH = @LUFA_PATH@/LUFA

# Construct LUFA module source variables
LUFA_SRC_USB_COMMON = \
	$(LUFA_PATH)/Drivers/USB/Core/AVR8/USBController_AVR8.c		\
	$(LUFA_PATH)/Drivers/USB/Core/AVR8/USBInterrupt_AVR8.c		\
	$(LUFA_PATH)/Drivers/USB/Core/ConfigDescriptors.c		\
	$(LUFA_PATH)/Drivers/USB/Core/Events.c				\
	$(LUFA_PATH)/Drivers/USB/Core/USBTask.c				\
	$(LUFA_PATH)/Drivers/USB/Class/Common/HIDParser.c

LUFA_SRC_USB_DEVICE = \
	$(LUFA_PATH)/Drivers/USB/Core/AVR8/Device_AVR8.c		\
	$(LUFA_PATH)/Drivers/USB/Core/AVR8/Endpoint_AVR8.c		\
	$(LUFA_PATH)/Drivers/USB/Core/AVR8/EndpointStream_AVR8.c	\
	$(LUFA_PATH)/Drivers/USB/Core/DeviceStandardReq.c		\
	$(LUFA_SRC_USB_COMMON)

noinst_PROGRAMS = focuser

AVRDUDE = avrdude -v -p usb162 -b 115200 -c avrispmkII -P usb

flash:	focuser.hex focuser.eeprom
	$(AVRDUDE) -U flash:w:focuser.hex -U eeprom:w:focuser.eeprom

focuser.hex: focuser
	avr-objcopy --remove-section=.eeprom --target=ihex \
		focuser focuser.hex
focuser.eeprom: focuser
	avr-objcopy --only-section=.eeprom --target=ihex \
		--change-section-lma .eeprom=0 \
		focuser focuser.eeprom

lib_LTLIBRARIES = libfocuser.la

noinst_HEADERS = led.h motor.h timer.h receiver.h descriptor.h event.h serial.h

libfocuser_la_SOURCES = led.c motor.c timer.c receiver.c descriptor.c event.c \
	serial.c \
	$(LUFA_SRC_USB_DEVICE)

focuser_SOURCES = focuser.c
focuser_DEPENDENCIES = libfocuser.la
focuser_LDFLAGS = -lfocuser

# setting the fuse values
# low fuse byte
# -------------
# CKDIV8        = 0             Clock divided by 8 (yes)
# CKOUT         =  1            Clock out enabled (no)
# SUT1..0       =   10          Startup time setting (fast rising power)
# CKSEL3..0     =     1111      Clock source setting (8 MHz crystal)
#                 ________
#                 01101111      = 0x6f
#
#
# high fuse byte
# --------------
# DWEN          = 1             DebugWIRE enabled (no)
# RSTDSBL       =  1            external reset disabled (no)
# SPIEN         =   0           Serial program and data download enabled (yes)
# WDTON         =    1          Watchdog timer always on (no)
# EESAVE        =     0         EEPROM preserved on chip erase (yes)
# BOOTSZ1..0    =      00       Select Boot size
# BOOTRST       =        1      Reset vector @0x0000
#                 ________
#                 11010001      = 0xd1
#
#
# extended fuse byte
# ------------------
# unused        = 1111          (reserved)
# HWBE          =     0         
# BODLEVEL2..0  =      111      Brown-out Detector trigger level (disabled)
#                 ________
#                 11110111      = 0xf7
#
# original fuse settings reported by avrdude
# lfuse reads as 5E
# hfuse reads as D9
# efuse reads as F4

fuses:
	$(AVRDUDE) -U lfuse:w:0x6f:m -U hfuse:w:0xd1:m -U efuse:w:0xf7:m

read:   readback.hex

readback.hex:
	$(AVRDUDE) -U flash:r:readback.hex

read:
	$(AVRDUDE) -U eeprom:r:eeprom.raw:r -u
read.hex:
	$(AVRDUDE) -U eeprom:r:read.hex:i

size:   focuser
	avr-size *.o focuser

