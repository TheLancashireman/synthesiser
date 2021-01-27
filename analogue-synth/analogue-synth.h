/* analogue-synth - a midi interface for an analogue synthiesiser
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
#ifndef ANALOGUE_SYNTH_H
#define ANALOGUE_SYNTH_H	1

extern void init(void);
extern void SetCV(uint8_t coarse, uint8_t fine);
extern void SetGate(uint8_t g);
extern uint8_t midi_for_me(uint8_t *mcmd, uint8_t n);
extern void my_midi_init(void);

#endif
