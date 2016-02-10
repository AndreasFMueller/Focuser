/*
 * motor.h -- interface to the Pololu stepper motor driver
 *
 * (c) 2016 Prof Dr Andreas Mueller, Hochschule Rapperswil
 */
#ifndef _motor_h
#define _motor_h

#define	SPEED_SLOW	0
#define SPEED_FAST	1

#define STEP_FULL	0x0
#define STEP_HALF	0x1
#define STEP_QUARTER	0x2
#define STEP_EIGHTH	0x3
#define STEP_SIXTEENTH	0x7

extern void	motor_set_step(unsigned char step);
extern unsigned char	motor_get_stepping();

extern void	motor_moveto(unsigned short position, unsigned char speed);
extern unsigned short	motor_current();
extern void	motor_stop();
extern unsigned short	motor_target();
extern unsigned short	motor_speed();

extern volatile unsigned short	lastsaved;
extern volatile unsigned char	saveneeded;

extern void	motor_save();

#endif /* _motor_h */
