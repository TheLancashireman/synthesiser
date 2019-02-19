/*	effect-synth.h - header file for synth note generator
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
#ifndef EFFECT_SYNTH_H
#define EFFECT_SYNTH_H	1

#include <dv-config.h>
#include <davroska.h>
#include <synth-config.h>

#include <effect.h>
#include <adsr.h>
#include <wave.h>

struct effect_synth_mono_s
{
	struct tonegen_s vco;
	struct envelope_s envelope;
	dv_u32_t age;
	dv_i32_t midi_note;
};

struct effect_synth_s
{
	int n_polyphonic;
	int gain;
};

extern struct effect_synth_mono_s notegen[MAX_POLYPHONIC];
extern struct effect_synth_s synth;

extern dv_i64_t effect_synth(struct effect_s *e, dv_i64_t signal);
extern void effect_synth_init(struct effect_s *e);
extern dv_i64_t synth_play_note(struct effect_synth_mono_s *notegen);
extern void synth_control(dv_i32_t controller, dv_i32_t value);

#endif
