#
# process this file with autoconf to produce the configure script
#
# (c) 2016 Prof Dr Andreas Mueller, Hochschule Rapperswil
# $Id$
#
AC_PREREQ(2.57)
AC_REVISION([1.1])
AC_INIT
AC_CONFIG_SRCDIR([focuser.c])
AM_INIT_AUTOMAKE(focuser, 1.1)
AC_CONFIG_HEADERS(config.h)

AM_PROG_LIBTOOL
AC_SUBST(LIBTOOL_DEPS)

AC_PROG_CC

AC_HEADER_STDC
AC_CHECK_HEADERS([stdlib.h stdio.h])

AC_C_CONST

# LUFA configuration
AC_ARG_WITH(lufa,
[AS_HELP_STRING([--with-lufa=dir], [LUFA root directory])],
[
	LUFA_PATH=${with_lufa}
	if test -d "${LUFA_PATH}"
	then
		:
	else
		AC_MSG_ERROR([LUFA directory not found])
	fi
],[
	AC_MSG_ERROR([LUFA is required])
])
LUFA_MCU=at90usb162
LUFA_ARCH=AVR8
LUFA_F_USB=8000000
LUFA_F_CPU=1000000

AC_SUBST(LUFA_PATH)
AC_SUBST(LUFA_MCU)
AC_SUBST(LUFA_ARCH)
AC_SUBST(LUFA_F_USB)
AC_SUBST(LUFA_F_CPU)

CPPFLAGS="${CPPFLAGS} -std=gnu99 -I${LUFA_PATH} -DUSE_LUFA_CONFIG_HEADER -DARCH=ARCH_${LUFA_ARCH} -DF_USB=${LUFA_F_USB}UL -DF_CPU=${LUFA_F_CPU}UL"

CFLAGS="${CFLAGS} -std=gnu99 -mmcu=${LUFA_MCU} -fshort-enums -fno-inline-small-functions -fpack-struct -fno-strict-aliasing -funsigned-char -funsigned-bitfields -ffunction-sections"

# the -u flag is needed to ensure that the EVENT handler is linked. There
# already is a weak symbol of the same name, and the linker apparently sees
# no need to include the event handler, as the symbol can already be resolved
LDFLAGS="${LDFLAGS} -u EVENT_USB_Device_ControlRequest"

AC_CHECK_FUNCS([memset strdup])

AC_CONFIG_FILES([Makefile])

AC_OUTPUT
