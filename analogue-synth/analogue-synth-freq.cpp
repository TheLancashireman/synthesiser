/* analogue-synth-freq.cpp - frequency measurement using timer1
 *
 * (c) 2020 David Haworth
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
uint8_t n_cap;		// Number of captures since tthe last time we looked
uint16_t cap;		// Capture value of most recent capture event

ISR(TIMER1_CAPT_vect)
{
	cap = ICR1;		// Read the time of the capture
	n_cap++;
}

/* freq() - calculate the signal frequency
 *
 * Using the difference between the capture time (from the ISR) and the last known capture time,
 * along with the number of overflows, the interval can be calculated.
 * The number of captures in that interval is also known, so the average frequency can be calculated.
*/
uint16_t last_cap;
unsigned long update_interval;

double freq(unsigned long e)
{
	uint8_t nc, no;
	uint16_t v;

	update_interval += e;

	cli();
	nc = n_cap;
	if ( nc > 0 )		// If there's been at least one capture, read and reset the interrupt handlers' data
	{
		v = cap;
		no = n_oflo;
		n_cap = 0;
		n_oflo = 0;
	}
	sei();

	if ( nc > 0 )		// If there's been a capture, calculate and return the frequency
	{
		uint32_t t = (uint32_t)v - (uint32_t)last_cap + (uint32_t)no * 65536ul;
		last_cap = v;

		double f = ((double)nc * 16000000.0) / (double)t;

		if ( update_interval > MILLIS_TO_TICKS(500) )
		{
			display_freq(f);
			update_interval = 0;
		}

		return f;
	}
	else
	if ( update_interval > MILLIS_TO_TICKS(2000) )
	{
		// More than 2 seconds without a pulse; assume 0.0 Hz
		display_freq(0.0);
		update_interval = 0;
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
