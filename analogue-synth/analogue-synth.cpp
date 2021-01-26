/* analogue-synth - a midi interface for an analogue synthiesiser
 *
 * (c) 2020 David Haworth
 *
 *	This file is part of analogue.
 *
 *	analogue is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, either version 3 of the License, or
 *	(at your option) any later version.
 *
 *	analogue is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with analogue.  If not, see <http://www.gnu.org/licenses/>.
 * modular is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
*/

/*
 * Control an analogue synthesiser by MIDI, using a DAC for the control voltage and a digital gate output.
 *
 * Timer0 runs at full speed (16 MHz on a Nano) to give the least ripple. This means that the normal
 * millis(), micros() etc. don't work. To avoid strange problems, a dummy init() function is linked so
 * that you get a link-time error if you inadvertently use something from wiring.c
 *
 * The DAC is constructed from two PWM outputs, OC0A and OC0B. OC0A provides the coarse control
 * and OC0B the fine control. To convert the PWM outputs into a true analogue output, an RC network
 * with a 100 nF capacitor is used. Both of the PWM outputs are fed to the same lead of the capacitor
 * (summing junction) via resistors: 4k7 for OC0A (coarse) and 100k for OC0B (fine). This gives OC0B
 * about a 1:20 influence over the output signal and allows a range of over 4 V with a resolution of less
 * than a millivolt.
 * The other lead of the capacitor is is connected to Arduino ground.
 *
 * The summing junction is fed to the non-inverting input of an op-amp configured as a non-inverting
 * amplifier. The output of the op-amp is the control voltage. To acheive a reasonable range that can
 * be adjusted in software, a gain of about 2 is required. A potential divider consisting of two 10k resistors
 * with a 10k potentiometer between them is connected between the output and ground. The wipe of the potentionmeter
 * is connected to the inverting input of the op-amp, giving an adjustable gain of between 1.5 and 3
 *
 * Important: the op-amp must be supplied from a symmetrical power supply - e.g. 15-0-15. The arduino ground
 * is connected to the 0 rail of the analogue supply.
 *
 * MIDI operation
 * ==============
 *
 * The controller responds to note-on and note-off commands as usual. The DAC values are selected from a
 * table using the note number as index. Note-on commands set the DAC values, followed by the gate output
 * after a delay. Note-off commands turn off the gate but don't change the DAC output.
 *
 * In addition to the note-on and -off messages, some other messages are accepted:
 *	Continuous controller 0 - set ADC coarse to given value
 *	Continuous controller 1 - set ADC coarse to given value + 128
 *	Continuous controller 2 - set ADC fine to given value
 *	Continuous controller 3 - set ADC fine to given value + 128
 *	Continuous controller 4 - set gate on or off depending on least significant bit of value
 *
*/
#include <Arduino.h>
#include "timestamp.h"

extern void init(void);

static void setup_t0(void);
static void setup_t1(void);

struct dacvals_s
{
	uint8_t coarse;
	uint8_t fine;
};

struct notestate_s
{
	struct dacvals_s dac;		/* Status of the DAC outputs */
	uint8_t gate;				/* Status of the gate output */
	uint8_t midi_note;			/* 0x00 to 0x7f (0 to 127) or 0xff if not a note */



int main(void)
{
	int val = 0;
	int coarse = 0;
	int fine = 0;
	init();
	setup_t0();
	setup_t1();
	sei();

	Serial.begin(115200);
	Serial.println("Hello world!");
	outvals(coarse, fine);

	for (;;)
	{
		int ch = Serial.read();

		if ( ch == '+' )
		{
			if ( fine <= 245 )
			{
				fine += 10;
				outvals(coarse, fine);
				val = 0;
			}
		}
		else if ( ch == '-' )
		{
			if ( fine >= 10 )
			{
				fine -= 10;
				outvals(coarse, fine);
				val = 0;
			}
		}
		else if ( ch == '>' )
		{
			if ( fine < 255 )
			{
				fine += 1;
				outvals(coarse, fine);
				val = 0;
			}
		}
		else if ( ch == '<' )
		{
			if ( fine > 0 )
			{
				fine -= 1;
				outvals(coarse, fine);
				val = 0;
			}
		}
		else if ( ch == '\r' )
		{
			fine = 0;
			coarse = val;
			outvals(coarse, fine);
			val = 0;
		}
		else if ( ch >= '0' && ch <= '9' )
		{
			Serial.write((char)ch);
			val = val * 10 + (ch - '0');
		}
	}
}

static void outvals(uint8_t coarse, uint8_t fine)
{
	OCR0A = (uint8_t)coarse;
	OCR0B = (uint8_t)fine;
	Serial.print("\r\nCoarse = ");
	Serial.println(coarse);
	Serial.print("  Fine = ");
	Serial.println(fine);
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
