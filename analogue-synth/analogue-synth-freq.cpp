/* analogue-synth-freq.cpp - frequency measurement using timer1
 *
 * (c) David Haworth
 *
 *	This file is part of analogue-synth.
 *
 *	analogue-synth is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, either version 3 of the License, or
 *	(at your option) any later version.
 *
 *	analogue-synth is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with analogue-synth.  If not, see <http://www.gnu.org/licenses/>.
*/
#include <Arduino.h>
#include "timestamp.h"
#include "analogue-synth.h"

#define ICP1	8	// Input capture 1 is on pin 8/PB0

/* ISR(TIMER1_OVF_vect) - interrupt handler for the timer overflow
 *
 * Increment a counter
*/
uint8_t n_oflo;	// Number of overflows since the last time we looked

ISR(TIMER1_OVF_vect)
{
	n_oflo++;
}

/* ISR(TIMER1_CAPT_vect) - interrupt handler for the capture interrupt
 *
 * Store the capture time and increment a counter
*/
uint16_t cap;		// Capture value of most recent capture event
uint8_t cap_oflo;	// Overflow value of most recent capture event
uint8_t n_cap;		// Number of captures since tthe last time we looked

ISR(TIMER1_CAPT_vect)
{
	cap = ICR1;		// Read the time of the capture
	cap_oflo = n_oflo;
	n_cap++;
}

/* freq() - calculate the signal frequency
 *
 * Using the difference between the capture time (from the ISR) and the last known capture time,
 * along with the number of overflows, the interval can be calculated.
 * The number of captures in that interval is also known, so the average frequency can be calculated.
 *
 * The input parameter e is the number of timer ticks since the last call.
 * The input parameter update_interval is the required update interval for the display.
*/
uint16_t last_cap;
uint8_t last_oflo;
unsigned long update_elapsed;

unsigned long total_time;
unsigned total_cap;

double freq(unsigned long e, unsigned long update_interval)
{
	uint8_t nc, no;
	uint16_t v;

	update_elapsed += e;

	cli();
	nc = n_cap;
	if ( nc > 0 )		// If there's been at least one capture, read and reset the interrupt handlers' data
	{
		v = cap;
		no = cap_oflo;
		n_cap = 0;
	}
	sei();

	if ( nc > 0 )		// If there's been at least one capture, accumulate the time and no of captures.
	{
		total_time += (uint32_t)v  - (uint32_t)last_cap + ((uint32_t)((uint8_t)(no - last_oflo))) * 65536ul;
		total_cap += nc;
		last_cap = v;
		last_oflo = no;

		// Once per interval, calculate and display the frequency
		if ( update_elapsed > update_interval )
		{
			double f = ((double)total_cap * 16000000.0) / (double)total_time;
			display_freq(f);

			total_cap = 0;
			total_time = 0;
			update_elapsed = 0;

			return f;
		}
	}
	else if ( update_elapsed > MILLIS_TO_TICKS(2000) )
	{
		// More than 2 seconds without a pulse; assume 0.0 Hz
		display_freq(0.0);
		total_cap = 0;
		total_time = 0;
		update_elapsed = 0;
		return 0.0;
	}

	return -1.0;	// No frequency calculated
}

void freq_init(void)
{
	pinMode(ICP1, INPUT);		// Set up the T1 input capture pin for frequency measurement
	TCCR1B |= 0x40;				// Input capture on leading edge
	TIMSK1 |= 0x21;				// Enable input capture and overflow interrupts
}
