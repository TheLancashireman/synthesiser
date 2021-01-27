/* timestamp.cpp - monotonically-increasing timestamp
 *
 * (c) 2020 David Haworth
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
#include <Arduino.h>
#include "timestamp.h"

unsigned long long timestamp;
unsigned last_t1;

/* read_ticks() returns an ever increasing time
 *
 * The resolution of the time depends on the scaling of timer 1. With a prescaler of 1,
 * the resolution on a nano is 62.5 ns (16 MHz clock)
 *
 * For read_ticks() to work correctly, you have to call it at least once for each wrap-around of
 * timer 1. At 16 MHz, that's every 4096 us - a tad over 4 ms.
 *
 * Using raw ticks is a bit more efficient than using units like milliseconds and microseconds,
 * because usually you can persuade the compiler to do the computation for you.
*/
unsigned long long read_ticks(void)
{
	cli();
	unsigned t1 = TCNT1;
	unsigned diff = t1 - last_t1;
	unsigned long long retval = timestamp + diff;
	timestamp = retval;
	last_t1 = t1;
	sei();
	return retval;
}

/* tick_delay() does nothing for the specified number of ticks
*/
void tick_delay(unsigned long long dly)
{
	unsigned long long t0 = read_ticks();

	while ( (read_ticks() - t0) < dly )
	{
		/* Twiddle thumbs */
	}
}

/* Conversion functions - are these needed?
*/
unsigned long long micros_to_ticks(unsigned long long micros)
{
	return (micros * HZ) / 1000000;
}

unsigned long long millis_to_ticks(unsigned long millis)
{
	return micros_to_ticks((unsigned long long)millis * 1000);
}
