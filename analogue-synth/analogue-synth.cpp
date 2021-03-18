/* analogue-synth - a midi interface for an analogue synthiesiser
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

/*
 * Control an analogue synthesiser by MIDI, using a DAC for the control voltage
 * and a digital gate output.
 *
 * See README.md for details.
 *
 * CAVEAT (for those that don't bother with README.md):
 * Timer0 runs at full speed (16 MHz on a Nano) to give the least ripple. This means that the normal
 * millis(), micros() etc. don't work. To avoid strange problems, a dummy init() function is linked so
 * that you get a link-time error if you inadvertently use something from wiring.c
 *
 * Timer1 also runs at full speed. This timer is used (by the common timerstamp.cpp functions)
 * to measure time. You have to make sure you read the timer at least once every 4ms or so, otherwise
 * time will drift. If there's some long computation going on, read the timer occasionally. No need to
 * save the result: "(void)read_ticks();" will do.
*/
#include <Arduino.h>
#include "timestamp.h"
#include "analogue-synth.h"
#include "midi.h"
#include <LiquidCrystal.h>

#define ICP1	8	// Input capture 1 is on pin 8/PB0

LiquidCrystal *lcd;

extern void init(void);

static void setup_t0(void);
static void setup_t1(void);
static void setup_t2(void);
static void print_spaces(uint8_t n);

int main(void)
{
	unsigned long long t;
	unsigned long long t0;
	unsigned long elapsed;
	init();
	sei();

	init_timestamp();
	setup_dac();					// The DAC could be TC0 or TC2
	pinMode(ICP1, INPUT_PULLUP);	// Set up the T1 input capture pin for frequency measurement

	Serial.begin(115200);			// For real MIDI change this to 31250

	freq_init();					// Initialise the frequency measurement

	// LCD controller in 4-bit mode without read
	lcd = new LiquidCrystal(lcd_rs, lcd_e, lcd_d4, lcd_d5, lcd_d6, lcd_d7);
	lcd->begin(16, 2);

	// Display a friendly greeting
	lcd->setCursor(0,0);
	lcd->print(F("AnalogueSynth"));
	lcd->setCursor(0,1);
	lcd->print(F("(c) dh   GPLv3"));
	tick_delay(MILLIS_TO_TICKS(2000));
	lcd->clear();

	my_midi_init();					// Initialise the tone generator

	t0 = read_ticks();

	for (;;)
	{
		t = read_ticks();		// Keep the timestamp updated
		elapsed = t - t0;
		t0 = t;

		(void)freq(elapsed, MILLIS_TO_TICKS(1000));	// Discard the result for now

		midi_in();				// Look for incoming midi messages
	}
}

void set_cv(uint8_t coarse, uint8_t fine)
{
	OCR0A = (uint8_t)coarse;
	OCR0B = (uint8_t)fine;
}

void set_gate(uint8_t g)
{
	// ToDo: Which pin?
}

void display_freq(double f)
{
	uint8_t np;
	lcd->setCursor(FREQ_COL, FREQ_ROW);
	np = lcd->print(f, ((f < 100.0) ? 3 : 2));
	np += lcd->print(F("Hz"));
	print_spaces(16 - FREQ_COL - np);
}

void display_note(uint8_t n)
{
	uint8_t np;
	lcd->setCursor(NOTE_COL, NOTE_ROW);
	np = lcd->print(F("n:"));
	if ( n <= 127 )
	{
		np += lcd->print(n);
	}
	
	print_spaces(6-np);
}

void display_coarse(uint8_t v)
{
	uint8_t np;
	lcd->setCursor(COARSE_COL, COARSE_ROW);
	np = lcd->print(F("c:"));
	np += lcd->print(v);
	print_spaces(6-np);
}

void display_fine(uint8_t v)
{
	uint8_t np;
	lcd->setCursor(FINE_COL, FINE_ROW);
	np = lcd->print(F("f:"));
	np += lcd->print(v);
	print_spaces(6-np);
}

void display_gate(char g)
{
	lcd->setCursor(GATE_COL, GATE_ROW);
	lcd->print(g);
}

static void print_spaces(uint8_t n)
{
	while ( n > 0 )
	{
		lcd->print(' ');
		n--;
	}
}

static void setup_t0(void)
{
	GTCCR = 0;
	TCCR0A = 0xa3;				// Fast PWM, non-inverting on outputs A and B
	TCCR0B = 0x01;				// Enable counter, prescaler = 1; WGM02 = 0
	TIMSK0 = 0;					// Disable all the interrupts
	TCNT0 = 0;
	TIFR0 = 0x07;				// Clear all pending interrupts
	OCR0A = 0x0;
	OCR0B = 0x0;

	DDRD |= (1<<5) | (1<<6);	// Set PD5/OC0B and PD6/OC0A to output
}

static void setup_t1(void)
{
	TCCR1A = 0;					// Normal port operation
	TCCR1B = 0x01;				// Enable counter, prescaler = 1; WGM12/3 = 0
	TCCR1C = 0;					// Probably not needed
	TIMSK1 = 0;					// Disable all the interrupts
	TCNT1 = 0;
	TIFR1 = 0x27;				// Clear all pending interrupts
}

static void setup_t2(void)
{
	TCCR2A = 0xa3;				// Fast PWM, non-inverting on outputs A and B
	TCCR2B = 0x01;				// Enable counter, prescaler = 1; WGM02 = 0
	TIMSK2 = 0;					// Disable all the interrupts
	TCNT2 = 0;
	TIFR2 = 0x07;				// Clear all pending interrupts
	OCR2A = 0x0;
	OCR2B = 0x0;

	DDRB |= (1<<3);				// Set PB3/OC2A to output
	DDRD |= (1<<3);				// Set PD3/OC2B to output
}
