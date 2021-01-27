/* analogue-synth - a midi interface for an analogue synthiesiser
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

/*
 * Control an analogue synthesiser by MIDI, using a DAC for the control voltage and a digital gate output.
 *
 * See README.md for details.
 *
 * CAVEAT (for those that don't bother with README.md):
 * Timer0 runs at full speed (16 MHz on a Nano) to give the least ripple. This means that the normal
 * millis(), micros() etc. don't work. To avoid strange problems, a dummy init() function is linked so
 * that you get a link-time error if you inadvertently use something from wiring.c
 *
*/
#include <Arduino.h>
#include "timestamp.h"
#include "analogue-synth.h"
#include "midi.h"

extern void init(void);

static void setup_t0(void);
static void setup_t1(void);

int main(void)
{
	unsigned long long t;
	unsigned long long t0;
	unsigned long elapsed;
	init();
	setup_t0();
	setup_t1();
	sei();

	Serial.begin(115200);		// For real MIDI change this to 31250

	my_midi_init();

	t0 = read_ticks();

	for (;;)
	{
		t = read_ticks();		// Keep the timestamp updated
		elapsed = t - t0;
		t0 = t;

		midi_in();				// Look for incoming midi messages
	}
}

void SetCV(uint8_t coarse, uint8_t fine)
{
	OCR0A = (uint8_t)coarse;
	OCR0B = (uint8_t)fine;
}

void SetGate(uint8_t g)
{
	// ToDo: Which pin?
}

static void setup_t0(void)
{
	GTCCR = 0;
	TCCR0A = 0xa3;				/* Fast PWM, non-inverting on outputs A and B */
	TCCR0B = 0x01;				/* Enable counter, prescaler = 1; WGM02 = 0 */
	TIMSK0 = 0;					/* Disable all the interrupts */
	TCNT0 = 0;
	OCR0A = 0x80;				/* 50% duty cycle */
	OCR0B = 0x40;				/* 25% duty cycle */
	TIFR0 = 0x07;				/* Clear all pending interrupts */

	DDRD |= (1<<5) | (1<<6);	/* Set PD5/OC0B and PD6/OC0A to output */
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
