/*	midi.h - Common MIDI input handling for Arduino-based controllers
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
#ifndef MIDI_H
#define MIDI_H	1

#include <Arduino.h>

#define MIDI_OUT_RUNNING_STATUS	0	/* Set to nonzero to use running status on output */

extern void midi_in(void);
extern void midi_out(uint8_t *mcmd, uint8_t n);
extern void midi_out(uint8_t p0, uint8_t p1);
extern void midi_out(uint8_t p0, uint8_t p1, uint8_t p2);
extern uint8_t midi_for_me(uint8_t *mcmd, uint8_t n);

#endif
