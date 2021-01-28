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

#define DAC		0		/* Set to 0 to use TC0/OC0A/OC0B. Set to 2 to use TC2/OC2A/PC2B */

#if DAC == 0

#define DAC_COARSE	OCR0A
#define DAC_FINE	OCR0B
#define setup_dac()	setup_t0()

#elif DAC == 2

#define DAC_COARSE	OCR2A
#define DAC_FINE	OCR2B
#define setup_dac()	setup_t2()

#else
#error "Unsuupported DAC setting
#endif

extern void init(void);
extern void set_cv(uint8_t coarse, uint8_t fine);
extern void set_gate(uint8_t g);
extern void display_freq(double f);

extern uint8_t midi_for_me(uint8_t *mcmd, uint8_t n);
extern void my_midi_init(void);

extern double freq(unsigned long e);
extern void freq_init(void);

#endif
