/*	midi.c - MIDI input/control for SynthEffect project
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
#include <kernel/h/dv-kconfig.h>
#include <kernel/h/dv-stdio.h>

#include <project/h/midi.h>

static unsigned midi_command[3];		/* Buffer for receiving MIDI commands */
static int midi_idx = 0;

/* midi_scan() - watch for midi commands in console input stream
 *
 * Only 3-byte midi messages are considered. Those with 2 bytes stay in the buffer until the next
 * start-of-message (MSB set) arrives and will be discarded then.
 *
 * System exclusive (0xf0 ... 0xf7). First 3 bytes get handled as a message. Remaining data
 * bytes ignored. Terminator (0xf7) starts a new message but will be discarded as for a short message.
*/
unsigned *midi_scan(void)
{
	if ( dv_consoledriver.isrx() )
	{
		unsigned c = (unsigned)dv_consoledriver.getc();

		if ( (c & 0x80) != 0 )
		{
			midi_command[0] = c;
			midi_idx = 1;
		}
		else if ( midi_idx > 0 )
		{
			midi_command[midi_idx++] = c;
			if ( midi_idx >= 3 )
			{
				midi_idx = 0;
				return midi_command;	/* Buffer is valid until next call */
			}
		}
	}

	return DV_NULL;
}
