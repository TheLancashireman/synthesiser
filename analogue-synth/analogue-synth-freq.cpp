/* freq.cpp - frequency measurement using timer1
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

uint16_t oflo;

ISR(TIMER1_OVF_vect)
{
	oflo++;
}

uint16_t last_cap;		// Last value of capture register
uint16_t last_oflo;	// Value of overflow count at last capture

volatile uint8_t n_caps;		// No of captures since last pickup
volatile uint32_t period;		// Measured period in ticks

ISR(TIMER1_CAPT_vect)
{
	uint16_t cap = ICR1;		// Read the time of the capture
	uint16_t ofl = oflo;

	if ( n_caps == 0 )
	{
		period = ((uint32_t)cap - (uint32_t)last_cap) + ( ((uint32_t)(oflo - last_oflo)) << 16 );
	}

	n_caps++;
}

unsigned long update_interval;

double freq(unsigned long e)
{
	update_interval += e;

	if ( n_caps > 0 )
	{
		double f = 16000000.0/(double)period;

		if ( update_interval > MILLIS_TO_TICKS(500) )
		{
			display_freq(f);
			update_interval -= MILLIS_TO_TICKS(500);
		}

		n_caps = 0;

		return f;
	}

	return -1.0;
}

void freq_init(void)
{
	pinMode(ICP1, INPUT);		// Set up the T1 input capture pin for frequency measurement
}

static void setup_t1(void)
{
	TCCR1A = 0;					/* Normal port operation */
	TCCR1B = 0x01;				/* Enable counter, prescaler = 1; WGM12/3 = 0 */
	TCCR1C = 0;					/* Probably not needed */
	TIMSK1 = 0;					/* Disable all the interrupts */
	TCNT1 = 0;
	TIFR1 = 0x27;				/* Clear all pending interrupts */
}
