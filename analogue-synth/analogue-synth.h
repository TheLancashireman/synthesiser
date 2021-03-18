/* analogue-synth - a midi interface for an analogue synthiesiser
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
#ifndef ANALOGUE_SYNTH_H
#define ANALOGUE_SYNTH_H	1

#define DAC		0		// Set to 0 to use TC0/OC0A/OC0B. Set to 2 to use TC2/OC2A/PC2B (untested)

#if DAC == 0

// DAC pins for nano/Timer0
#define DAC_COARSE	OCR0A
#define DAC_FINE	OCR0B
#define setup_dac()	setup_t0()

#elif DAC == 2

// DAC pins for nano/Timer2
#define DAC_COARSE	OCR2A
#define DAC_FINE	OCR2B
#define setup_dac()	setup_t2()

#else
#error "Unsuupported DAC setting
#endif

// LCD pin selections for nano
#define lcd_d4		14
#define lcd_d5		15
#define lcd_d6		16
#define lcd_d7		17
#define lcd_rs		9
#define lcd_e		13

// Display positions
#define NOTE_ROW	0
#define NOTE_COL	0
#define FREQ_ROW	0
#define FREQ_COL	6
#define COARSE_ROW	1
#define COARSE_COL	0
#define FINE_ROW	1
#define FINE_COL	6
#define GATE_ROW	1
#define GATE_COL	15

extern void init(void);
extern void set_cv(uint8_t coarse, uint8_t fine);
extern void set_gate(uint8_t g);

extern void display_freq(double f);
extern void display_note(uint8_t n);
extern void display_coarse(uint8_t v);
extern void display_fine(uint8_t v);
extern void display_gate(char g);

extern uint8_t midi_for_me(uint8_t *mcmd, uint8_t n);
extern void my_midi_init(void);

extern double freq(unsigned long e, unsigned long update_interval);
extern void freq_init(void);

extern double get_note_frequency(uint8_t note);

#endif
