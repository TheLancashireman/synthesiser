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

static unsigned midi_command[3];		/* Buffer for receiving MIDI commands */
static int midi_idx = 0;

unsigned *midi_scan(void)
{
	if ( dv_consoledriver.isrx() )
	{
		int c = dv_consoledriver.getc();

		if ( (((unsigned)c) & 0x80) != 0 )
		{
			midi_command[0] = (unsigned)c;
			midi_idx = 1;
		}
		else if ( midi_idx > 0 )
		{
			midi_command[midi_idx++] = (unsigned)c;
			if ( midi_idx >= 3 )
			{
				midi_idx = 0;
				return midi_command;	/* Buffer is valid until next call */
			}
		}
	}

	return DV_NULL;
}
