/*
 * motor.h -- interface to the Pololu stepper motor driver
 *
 * (c) 2016 Prof Dr Andreas Mueller, Hochschule Rapperswil
 */
#ifndef _motor_h
#define _motor_h

#define	MOTOR_SLOW	0
#define MOTOR_FAST	1

#define STEP_FULL	0x0
#define STEP_HALF	0x1
#define STEP_QUARTER	0x2
#define STEP_EIGHTH	0x3
#define STEP_SIXTEENTH	0x7

void	motor_set_stepping(unsigned char stepping);
unsigned char	motor_get_stepping();

void	motor_moveto(unsigned short position, unsigned char speed);
unsigned short	motor_current();
void	motor_stop();
unsigned short	motor_target();

#endif /* _motor_h */
