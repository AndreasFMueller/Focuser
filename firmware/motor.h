/*
 * motor.h -- interface to the Pololu stepper motor driver
 *
 * (c) 2016 Prof Dr Andreas Mueller, Hochschule Rapperswil
 */
#ifndef _motor_h
#define _motor_h

#include <stdint.h>

#define	SPEED_SLOW	0
#define SPEED_FAST	1

#define STEP_FULL	0x0
#define STEP_HALF	0x1
#define STEP_QUARTER	0x2
#define STEP_EIGHTH	0x3
#define STEP_SIXTEENTH	0x7

extern void	motor_set_step(unsigned char step);
extern unsigned char	motor_get_stepping();

extern void	motor_moveto(uint32_t position, unsigned char speed);
extern void	motor_position(uint32_t position);
extern uint32_t	motor_current();
extern void	motor_stop();
extern uint32_t	motor_target();
extern uint32_t	motor_speed();

extern void	motor_set_topspeed(uint8_t settopspeed);
extern uint8_t	motor_get_topspeed();
extern uint8_t	motor_faststep();

extern volatile uint32_t	lastsaved;
extern volatile unsigned char	saveneeded;

extern void	motor_save();

#endif /* _motor_h */
