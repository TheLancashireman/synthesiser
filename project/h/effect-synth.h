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

#include <kernel/h/dv-kconfig.h>
#include <kernel/h/dv-types.h>
#include <project/h/synth-config.h>

#include <project/h/effect.h>
#include <project/h/adsr.h>
#include <project/h/wave.h>

struct effect_synth_s
{
	struct tonegen_s vco;
	struct envelope_s shaper;
};

extern struct effect_synth_s notegen[N_POLYPHONIC];

extern dv_i64_t effect_synth(struct effect_s *e, dv_i64_t signal);
extern void effect_synth_init(struct effect_s *e);

#endif
