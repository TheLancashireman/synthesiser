/* analogue-synth-midi - a midi interface for an analogue synthiesiser
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
#include "midi.h"
#include "analogue-synth.h"

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
	uint8_t channel;			/* Channel no. 0x00 to 0x0f */
};

struct dacvals_s notes[128];		/* One DAC value for each MIDI note */
struct notestate_s current_note;	/* Only one channel (at the moment) */

static void controller_change(uint8_t ctrl, uint8_t val);
static void start_note(uint8_t note);
static void stop_note(uint8_t note);

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
	current_note.midi_note = 0xff;		/* No note playing */
	current_note.channel = 0;			/* ToDo: load from eeprom */

	/* ToDo: load notes array from eeprom */

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
		tick_delay(MILLIS_TO_TICKS(20));	/* Allow time for ADSR trigger to recover */
	}

	current_note.midi_note = note;
	current_note.dac.coarse = notes[note].coarse;
	current_note.dac.fine = notes[note].fine;
	set_cv(current_note.dac.coarse, current_note.dac.fine);
	tick_delay(MILLIS_TO_TICKS(20));	/* Allow time for DAC to settle */

	current_note.gate = 1;
	set_gate(1);
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
		tick_delay(MILLIS_TO_TICKS(20));	/* Allow time for ADSR trigger to recover */
	}
}

/* controller_change() - process a controller change message
 *
 * Controls 0..3 turn off the current note but not the gate.
 * An external controller that uses these commands should first set the gate to a known state.
*/
static void controller_change(uint8_t ctrl, uint8_t val)
{
	switch ( ctrl )
	{
	case 0:
		current_note.dac.coarse = val;
		current_note.midi_note = 0xff;
		set_cv(current_note.dac.coarse, current_note.dac.fine);
		break;

	case 1:
		current_note.dac.coarse = val + 128;
		current_note.midi_note = 0xff;
		set_cv(current_note.dac.coarse, current_note.dac.fine);
		break;

	case 2:
		current_note.dac.fine = val;
		current_note.midi_note = 0xff;
		set_cv(current_note.dac.coarse, current_note.dac.fine);
		break;

	case 3:
		current_note.dac.fine = val + 128;
		current_note.midi_note = 0xff;
		set_cv(current_note.dac.coarse, current_note.dac.fine);
		break;

	case 4:
		current_note.gate = val & 0x01;
		current_note.midi_note = 0xff;
		set_gate(current_note.gate);
		break;

	default:
		break;
	}
}
