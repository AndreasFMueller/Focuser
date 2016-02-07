/*
 * receiver.h -- interface to the RF receiver 
 *
 * (c) 2016 Prof Dr Andreas Mueller, Hochschule Rapperswil
 */
#ifndef _receiver_h
#define _receiver_h

#define RECV_D1_PIN	0
#define RECV_D2_PIN	1
#define RECV_D3_PIN	2
#define RECV_D4_PIN	3
#define RECV_VT_PIN	5

#define RECV_D1		0x01
#define RECV_D2		0x02
#define RECV_D3		0x04
#define RECV_D4		0x08
#define RECV_VT		0x10

#define	RECV_A		RECV_D1
#define	RECV_B		RECV_D2
#define	RECV_C		RECV_D3
#define	RECV_D		RECV_D4

unsigned char	recv_get();

void	recv_lock();
void	recv_unlock();

#endif /* _receiver_h */
