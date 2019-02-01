/*	background.c - background task
 *
 *	Copyright 2019 David Haworth
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

/* Background_main() - background task main function
 *
 * With a bitrate of 115.2 kBPS there are approximately 12 characters every millisecond.
 * The mini-uart doesn't seem to have a threshold control for the interrupt, so in the
 * worst case we'd get an interrupt every 80 or so microseconds.
 * On the other hand, polling the uart in a periodically-activated task would need the task
 * to run at intervals of around 600 microseconds, which is also quite an overhead.
 *
 * The best way is therefore to poll the uart in a background task. As long as the task is never
 * interrupted for longer than about 600 microseconds no characters will get lost.
 *
*/
void Background_main(void)
{
	for (;;)
	{
		/* Scan the incoming characters and split into MIDI and plain 7-bit ASCII streams.
		*/
		midi_scan();

		/* Anything else?
		*/
	}
}
