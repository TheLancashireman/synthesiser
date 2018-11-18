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

#define N_TONEGEN	1

extern int wave_init(void);
extern void wave_generate(int wav);
extern void wave_start_mono(int wgen, int note, dv_i32_t harmonic);
extern void wave_stop_mono(int wgen);
extern dv_i32_t wave_play_mono(int wgen);

#endif
