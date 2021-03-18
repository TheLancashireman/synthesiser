/* analogue-synth-midi - a midi interface for an analogue synthiesiser
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
#include "midi.h"
#include "analogue-synth.h"

#define N_NOTES				128
#define TUNE_SETTLE_TIME	MILLIS_TO_TICKS(100)
#define TUNE_N_DISCARD		3
#define TUNE_N_AVERAGE		5
#define TUNE_F_STEP			10

#define UP_ARROW			((char)0x9e)
#define DOWN_ARROW			((char)0x9f)

struct dacvals_s
{
	uint8_t coarse;
	uint8_t fine;
};

struct notestate_s
{
	struct dacvals_s dac;		// Status of the DAC outputs
	uint8_t gate;				// Status of the gate output
	uint8_t midi_note;			// 0x00 to 0x7f (0 to 127) or 0xff if not a note
	uint8_t channel;			// Channel no. 0x00 to 0x0f
};

struct dacvals_s notes[N_NOTES];	// One DAC value for each MIDI note
struct notestate_s current_note;	// Only one channel (at the moment)
uint8_t n_notes;					// No. of notes that actually work

static void controller_change(uint8_t ctrl, uint8_t val);
static void start_note(uint8_t note);
static void stop_note(uint8_t note);
static double get_avg_freq(void);
void tune_keyboard(void);

/* midi_for_me() - handles a midi command
 *
 * If the command is addressed to my channel, process it and return 1.
 * If not for my channel, return 0.
*/
uint8_t midi_for_me(uint8_t *mcmd, uint8_t n)
{
	if ( (mcmd[0] & 0x0f) != current_note.channel )
		return 0;		// Not my channel; ignore (and pass on)

	switch ( mcmd[0] & 0xf0 )
	{
	case 0x90:	// Note on
		if ( n == 3 )
		{
			// Valid message.
			start_note(mcmd[1]);
		}
		break;

	case 0x80:	// Note off
		if ( n == 3 )
		{
			// Valid message.
			stop_note(mcmd[1]);
		}
		break;

	case 0xb0:	// Continuous controller change
		if ( n == 3 )
		{
			// Valid message.
			controller_change(mcmd[1], mcmd[2]);
		}
		break;

	default:
		// Ignore
		break;
	}

	return 1;
}

/* my_midi_init() - initialise the variables and controller
*/
void my_midi_init(void)
{
	current_note.dac.coarse = 0;
	current_note.dac.fine = 0;
	current_note.gate = 0;
	current_note.midi_note = 0xff;		// No note playing
	current_note.channel = 0;			// ToDo: load from eeprom

	display_note(0xff);
	display_coarse(0);
	display_fine(0);

	// ToDo: load notes array from eeprom

	set_cv(0, 0);
	set_gate(0);
}

/* start_note() - start a note playing
*/
static void start_note(uint8_t note)
{
	// If the gate is on, turn it off
	if ( current_note.gate )
	{
		set_gate(0);
		tick_delay(MILLIS_TO_TICKS(20));	// Allow time for ADSR trigger to recover
	}

	current_note.midi_note = note;
	current_note.dac.coarse = notes[note].coarse;
	current_note.dac.fine = notes[note].fine;
	set_cv(current_note.dac.coarse, current_note.dac.fine);
	display_coarse(current_note.dac.coarse);
	display_fine(current_note.dac.fine);
	tick_delay(MILLIS_TO_TICKS(20));	// Allow time for DAC to settle

	current_note.gate = 1;
	set_gate(1);

	display_note(note);
	display_gate(UP_ARROW);
}

/* stop_note() - stop playing a note
*/
static void stop_note(uint8_t note)
{
	// Turn off the gate if the note is playing. Otherwise ignore.
	if ( current_note.midi_note == note )
	{
		current_note.gate = 0;
		set_gate(0);
		tick_delay(MILLIS_TO_TICKS(20));	// Allow time for ADSR trigger to recover
		display_gate(DOWN_ARROW);
	}
}

/* controller_change() - process a controller change message
 *
 * Controls 0..3 turn off the current note but not the gate.
 * An external controller that uses these commands should first set the gate to a known state.
 *
 * TEMPORARY:
 * Control 127, value 127 starts automatic tuning
*/
static void controller_change(uint8_t ctrl, uint8_t val)
{
	switch ( ctrl )
	{
	case 0:
		current_note.dac.coarse = val;
		current_note.midi_note = 0xff;
		set_cv(current_note.dac.coarse, current_note.dac.fine);
		display_coarse(current_note.dac.coarse);
		break;

	case 1:
		current_note.dac.coarse = val + 128;
		current_note.midi_note = 0xff;
		set_cv(current_note.dac.coarse, current_note.dac.fine);
		display_coarse(current_note.dac.coarse);
		break;

	case 2:
		current_note.dac.fine = val;
		current_note.midi_note = 0xff;
		set_cv(current_note.dac.coarse, current_note.dac.fine);
		display_fine(current_note.dac.fine);
		break;

	case 3:
		current_note.dac.fine = val + 128;
		current_note.midi_note = 0xff;
		set_cv(current_note.dac.coarse, current_note.dac.fine);
		display_fine(current_note.dac.fine);
		break;

	case 4:
		current_note.gate = val & 0x01;
		current_note.midi_note = 0xff;
		set_gate(current_note.gate);
		display_gate(current_note.gate == 1 ? UP_ARROW : DOWN_ARROW);
		break;

	case 127:
		if ( val == 127 )
		{
			tune_keyboard();
		}
		break;

	default:
		break;
	}
}

/* tune_note() - tune a single note
 *
*/
uint8_t tune_note(uint8_t note, uint8_t initial_coarse, uint8_t initial_fine)
{
	if ( note >= N_NOTES )
		return note;

	double ftgt = get_note_frequency(note);
	double favg;

	// Set the current note
	current_note.midi_note = note;

	// Initialise the dac values
	current_note.dac.fine = initial_fine;
	current_note.dac.coarse = initial_coarse;

	// Display the current state
	display_coarse(current_note.dac.coarse);
	display_fine(current_note.dac.fine);
	display_note(note);
	display_gate('C');

	// Ramp up the coarse dac value until the frequency is above the target.
	do {
		current_note.dac.coarse++;

		if ( current_note.dac.coarse == 0 )
			break;					// Exit loop on overflow

		// Output the dac values and get an average frequency reading
		favg = get_avg_freq();

	} while ( favg < ftgt );

	// Reduce the coarse dac value by 1. 
	current_note.dac.coarse--;
	set_cv(current_note.dac.coarse, current_note.dac.fine);
	display_coarse(current_note.dac.coarse);
	tick_delay(TUNE_SETTLE_TIME);
	display_gate('F');

	// Ramp up the fine dac value in big steps until the frequency is above the target.
	do {
		current_note.dac.fine += TUNE_F_STEP;

		if ( current_note.dac.fine < TUNE_F_STEP )
			return 0xff;		// Exit loop on overflow

		// Output the dac values and get an average frequency reading
		favg = get_avg_freq();

	} while ( favg < ftgt );

	// Reduce the fine dac value by the big step. 
	current_note.dac.fine -= TUNE_F_STEP;
	set_cv(current_note.dac.coarse, current_note.dac.fine);
	display_fine(current_note.dac.fine);
	tick_delay(TUNE_SETTLE_TIME);

	// Ramp up the fine dac value until the frequency is above the target.
	do {
		current_note.dac.fine++;

		if ( current_note.dac.fine == 0 )
			return 0xff;		// Exit loop on overflow

		// Output the dac values and get an average frequency reading
		favg = get_avg_freq();

	} while ( favg < ftgt );

	return note;
}

/* tune() - tune the whole keyboard
*/
void tune_keyboard(void)
{
	uint8_t n;

	for ( n = 0; n < N_NOTES; n++ )
	{
		notes[n].coarse = 0xff;
		notes[n].fine = 0xff;
	}
	n_notes = 0;									// In case tuning fails completely

	n = tune_note(0, 0, 0);

	while ( n < N_NOTES )
	{
		notes[n].coarse = current_note.dac.coarse;
		notes[n].fine = current_note.dac.fine;
		n_notes = n+1;								// The last note that we tuned successfully, +1

		uint8_t c = current_note.dac.coarse + 1;
		uint8_t f = 0;
		if ( c < current_note.dac.coarse )
		{
			// Overflow
			c = 0xff;
			f = current_note.dac.fine + 1;
			if ( f < current_note.dac.fine )
			{
				// Overflow
				return;
			}
		}

		n = tune_note(n+1, c, f);
	}
	display_gate(' ');
}

static double get_avg_freq(void)
{
	unsigned long long t0, t;
	unsigned long elapsed;
	double f;
	double fsum = 0.0;

	// Put the new dac values into the hardware and update the display
	set_cv(current_note.dac.coarse, current_note.dac.fine);
	display_coarse(current_note.dac.coarse);
	display_fine(current_note.dac.fine);

	// Allow the dac and vco time to settle
	tick_delay(TUNE_SETTLE_TIME);

	t0 = read_ticks();

	// Discard the first few frequency measurements
	for ( uint8_t i = 0; i < TUNE_N_DISCARD; i++ )
	{
		// Wait for a frequency calculation.
		do {
			t = read_ticks();		// Keep the timestamp updated
			elapsed = t - t0;
			t0 = t;
		} while ( (f = freq(elapsed, 0)) < 0.0 );
	}

	// Take the average of the next few frequency measurements
	for ( uint8_t i = 0; i < TUNE_N_AVERAGE; i++ )
	{
		// Wait for a frequency calculation.
		do {
			t = read_ticks();		// Keep the timestamp updated
			elapsed = t - t0;
			t0 = t;
		} while ( (f = freq(elapsed, 0)) < 0.0 );

		fsum += f;
	}

	return fsum / (double)TUNE_N_AVERAGE;
}
