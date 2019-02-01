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
#include <dv-config.h>
#include <davroska.h>
#include <dv-stdio.h>
#include <synth-davroska.h>

#include <midi.h>
#include <effect-notecontrol.h>

static dv_u32_t midi_command[3];		/* Buffer for receiving MIDI commands */
static int midi_idx = 0;

static void dispatch_midi_command(dv_u32_t *cmd);

/* midi_scan() - watch for midi commands in console input stream
 *
 * All 2- and 3-byte MIDI messages are filtered out of the input stream and dispatched.
 *
 * If there's a possibility that we receive system messages (0xf* in the command byte), they
 * may interfere with ascii command stream that this function forwards to a command interpreter.
*/
void midi_scan(void)
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
			if ( ( (midi_command[1] & 0xf0) == 0xd0 ) || midi_idx >= 3 )
			{
				midi_idx = 0;
				dispatch_midi_command(midi_command);
			}
		}
		else
		{
			/* ASCII characters received outside the context of a MIDI message.
			 * Echo them, place them in a ring buffer and notify a command interpreter when
			 * an enter key is received (\r or \n).
			*/
		}
	}
}

/* dispatch_midi_command() - dispatch a 2- or 3-byte midi command.
 *
 * Currently only 9 (note on), 8 (note off) and b (controller change) are recognised.
 * Any other commands are ignored.
 *
 * Note on and note off are sent to a note queue and will be picked up by the synth's main loop.
 * Controller changes are made immediately.
*/
static void dispatch_midi_command(dv_u32_t *cmd)
{
	dv_u32_t c = cmd[0] >> 4;		/* Midi command code */
	dv_u32_t ch = cmd[0] & 0x0f;		/* Midi channel */

	if ( c == 0x9 )			/* Note start */
	{
		dv_printf("start(%d, %d)\n", ch, cmd[1]);
		dv_u32_t note = NOTE_START | ( cmd[2] << 8 ) | cmd[1];
		synth_send_note(ch, note);
	}
	else if ( c == 0x8 )	/* Note stop */
	{
		dv_printf("stop(%d, %d)\n", ch, cmd[1]);
		dv_u32_t note = NOTE_STOP |  cmd[1];		/* Velocity is ignored */
		synth_send_note(ch, note);
	}
	else if ( c == 0xb )	/* Controller change */
	{
		dv_printf("controller(%d, %d, %d)\n", ch, cmd[1], cmd[2]);
		synth_controller_change(ch, cmd[1], cmd[2]);
	}
}
