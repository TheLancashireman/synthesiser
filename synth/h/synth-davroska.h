/*	synth-davroska.h - declarations of davroska objects
 *
 *	Copyright 2019 David Haworth
 *
 *	This file is part of SynthEffect.
 *
 *	SynthEffect is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, either version 3 of the License, or
 *	(at your option) any later version.
 *
 *	SynthEffect is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with SynthEffect.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef SYNTH_DAVROSKA_H
#define SYNTH_DAVROSKA_H	1

#include <davroska.h>
#include <dv-arm-bcm2835-interruptcontroller.h>

/* Davroska objects
*/
extern dv_id_t Background;		/* Background task */
extern dv_id_t Timer;			/* Timer ISR - 10 ms tick */
extern dv_id_t Monitor;			/* System monitor task */
extern dv_id_t TickCounter;		/* Counter - counts timer interrupts */
extern dv_id_t MonitorAlarm;	/* Alarm to activate the Monitor task */

/* Task & ISR main functions
*/
extern void Background_main(void);
extern void Monitor_main(void);
extern void Timer_main(void);

/* Callout functions
*/
extern dv_u32_t af_MonitorAlarm(dv_id_t a);

/* Other identifiers
*/
#define hw_TimerInterruptId		dv_iid_timer

#endif
