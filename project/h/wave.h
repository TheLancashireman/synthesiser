/*	wave.h - waveform generator
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
#ifndef wave_h
#define wave_h	1

#include <kernel/h/dv-kconfig.h>
#include <kernel/h/dv-types.h>

#define SAW	1
#define TRI	2
#define SQU	3
#define SIN 4

#define N_NOTEGEN	1

/* A wave table contains an integer multiple of whole waveforms of the note.
 * There will always be a small error because the sample rate is not a whole-number
 * multiple of the fundamental frequency. The number of cycles is chosen to keep the error
 * small enough.
 * The actual value of the wave is held in a wave buffer; the wave member is the base address.
*/
struct wavetable_s
{
	double f;
	dv_i32_t nsamp;
	dv_i32_t ncyc;
	dv_i32_t *wave;
};

/* A tone generator is used to generate a constant tone of of a given frequency, based on a root
 * wave table. The harmonic is used to increment the position (modulo nsamp of the wave) on each
 * sample. Thus a tone generator can output any harmonic of the give root wave.
*/
struct tonegen_s
{
	struct wavetable_s *root;
	dv_i32_t harmonic;
	dv_i32_t position;
};

extern int wave_init(void);
extern void wave_generate(int wav);

extern void tone_start(struct tonegen_s *tg, int note, dv_i32_t harmonic);
extern void tone_stop(struct tonegen_s *tg);
extern dv_i32_t tone_play(struct tonegen_s *tg);

#endif
