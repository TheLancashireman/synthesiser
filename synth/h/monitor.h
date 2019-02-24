/*	monitor.h - header file for synth status monitor
 *
 *	Copyright David Haworth
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
#ifndef MONITOR_H
#define MONITOR_H	1

#include <dv-config.h>
#include <synth-stdio.h>
#include <dv-arm-bcm2835-armtimer.h>

/* A structure for monitoring elapsed time
*/
struct monitor_elapsed_s
{
	dv_u32_t init;		/* Has been initialised */
	dv_u32_t last;		/* Last known value of timer */
	dv_u32_t min;		/* Shortest non-zero interval measured */
	dv_u32_t max;		/* Longest interval measured */
};

/* monitor_frc() - return a suitable timer value
*/
static inline dv_u32_t monitor_frc(void)
{
	return dv_arm_bcm2835_armtimer_read_frc();
}

/* monitor_start() - start a measurement
*/
static inline void monitor_start(struct monitor_elapsed_s *e)
{
	e->last = monitor_frc();
}

/* monitor_elapsed() - calculate the time since "last", record min and max
*/
static inline void monitor_elapsed(struct monitor_elapsed_s *e, dv_u32_t now)
{
	dv_u32_t t = now - e->last;
	e->last = now;

	if ( e->init )
	{
		if ( e->min > t )	e->min = t;
		if ( e->max < t )	e->max = t;
	}
	else
	{
		e->init = 1;
		e->min = t;
		e->max = t;
	}
}

static inline void print_elapsed(struct monitor_elapsed_s *e, char *s)
{
	(void)sy_printf("%s: min = %u, max = %u\n", s, e->min, e->max);
}

extern struct monitor_elapsed_s core1_loop;
extern struct monitor_elapsed_s core1_idle;

extern struct monitor_elapsed_s core2_loop;
extern struct monitor_elapsed_s core2_idle;

extern struct monitor_elapsed_s core3_loop;
extern struct monitor_elapsed_s core3_idle;

#endif
