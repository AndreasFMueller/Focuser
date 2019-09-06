#
# Makefile -- build the test client for the focuser interface
#
# (c) 2016 Prof Dr Andreas Mueller, Hochschule Rapperswil
#
fclient:	fclient.c version.c ../firmware/config.h
	gcc -std=gnu99 -Wall -O -g -o fclient fclient.c version.c -lusb-1.0
