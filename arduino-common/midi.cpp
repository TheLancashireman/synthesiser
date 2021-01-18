/*	midi.c - Common MIDI input handling for Arduino-based controllers
 *
 *	Copyright 2018 David Haworth
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
#include "midi.h"

static uint8_t midi_command[3];		/* Buffer for receiving MIDI commands */
static uint8_t midi_idx = 0;
#if MIDI_OUT_RUNNING_STATUS
static uint8_t midi_status;
#endif

/* midi_in() - watch for midi commands in console input stream
 *
 * ToDo: 0xfx messages
 *	- System exclusive messages 0xf0 followed by a number of bytes and terminated by 0xf7
 *	- Single-byte messages 0xf1 to 0xf6 and 0xf8 to 0xff
*/
void midi_in(void)
{
	while ( Serial.available() > 0 )
	{
		/* Process all available bytes
		*/
		uint8_t c = Serial.read();

		if ( (c & 0x80) != 0 )
		{
			/* Start of a new midi message
			*/
			midi_command[0] = c;
			midi_idx = 1;
		}
		else if ( midi_idx == 0 || midi_idx == 1 )
		{
			/* Midi message with running status OR 2nd byte of a midi command.
			 * Re-use the previous status
			*/
			midi_command[1] = c;
			midi_idx = 2;

			if ( (midi_command[0] & 0xf0) == 0xc0 || (midi_command[0] & 0xf0) == 0xd0 )
			{
				/* 2-byte message (patch change/channel pressure)
				*/
				if ( !midi_for_me(midi_command, 2) )
					midi_out(midi_command, 2);
				midi_idx = 0;
			}
		}
		else
		{
			midi_command[midi_idx++] = c;
			if ( midi_idx >= 3 )
			{
				/* 3-byte message
				*/
				if ( !midi_for_me(midi_command, 3) )
					midi_out(midi_command, 3);
				midi_idx = 0;
			}
		}
	}
}

/* midi_out() - send midi commands from a buffer
 *
*/
void midi_out(uint8_t *mcmd, uint8_t n)
{
#if MIDI_OUT_RUNNING_STATUS
	if ( mcmd[0] == midi_status )
	{
		Serial.write(&mcmd[1], n-1);
		return;
	}
	midi_status = mcmd[0];
#endif
	Serial.write(mcmd, n);
}

/* midi_out() - send midi commands passed as parameters
 *
*/
void midi_out(uint8_t p0, uint8_t p1)
{
#if MIDI_OUT_RUNNING_STATUS
	if ( p0 == midi_status )
	{
		Serial.write(p1);
		return;
	}
	midi_status = p0;
#endif
	Serial.write(p0);
	Serial.write(p1);
}

void midi_out(uint8_t p0, uint8_t p1, uint8_t p2)
{
	midi_out(p0, p1);
	Serial.write(p2);
}
