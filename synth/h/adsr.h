/*  adsr.h - adsr envelope generation
 *
 *  Copyright 2018 David Haworth
 *
 *  This file is part of SynthEffect.
 *
 *  SynthEffect is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  SynthEffect is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with SynthEffect.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef ADSR_H
#define ADSR_H

#include <dv-config.h>
#include <davroska.h>
#include <synth-config.h>

/* adsr_s - structure that contains the envelope generator's parameters.
 *
 * Held separately from the generators themselves because each note generator is expected
 * to use the same envelope profile.
*/
struct adsr_s
{
	dv_i32_t a;			/* Attack time (raw raw) */
	dv_i32_t d;			/* Decay time (raw value) */
	dv_i32_t s;			/* Sustain level (raw value) */
	dv_i32_t r;			/* Release time (raw value) */

	dv_i32_t tAttack;	/* Attack time (samples) */
	dv_i32_t tDecay;	/* Decay time (samples) */
	dv_i32_t gSustain;	/* Sustain level (same units as gMax) */
	dv_i32_t tRelease;	/* Release time (samples) */

	dv_i32_t gDecay;	/* Level difference between gSustain and gMax */
	dv_i32_t tSustain;	/* tAttack + tDecay */
	dv_i32_t tTotal;	/* tAttack + tDecay + tRelease */
};

extern void adsr_init(struct adsr_s *adsr, dv_i32_t a, dv_i32_t d, dv_i32_t s, dv_i32_t r, dv_i32_t sps);
extern void adsr_set_a(struct adsr_s *adsr, dv_i32_t a, dv_i32_t sps);
extern void adsr_set_d(struct adsr_s *adsr, dv_i32_t d, dv_i32_t sps);
extern void adsr_set_s(struct adsr_s *adsr, dv_i32_t s, dv_i32_t sps);
extern void adsr_set_r(struct adsr_s *adsr, dv_i32_t r, dv_i32_t sps);

/* envelope_s - structure defining an envelope generator
*/
struct envelope_s
{
	struct adsr_s *adsr;		/* ADSR profile */
	int position;				/* Current position in the envelope */
};

dv_i32_t envelope_gen(struct envelope_s *env);

static inline void envelope_start(struct envelope_s *env)
{
	env->position = 0;
}

static inline void envelope_stop(struct envelope_s *env)
{
	/* Go straight to the release phase unless already released.
	 *
	 * This might be a bit drastic if the note has a low sustain level and is released
	 * during the attack or decay phase. We'll have to see.
	*/
	if ( env->position > 0  && env->position <= env->adsr->tSustain )
		env->position = env->adsr->tSustain + 1;
}

#endif
