/* adsr.c - ADSR envelope generator
 *
 * (c) 2018 David Haworth
 *
 * This file is part of SynthEffect.
 *
 * SynthEffect is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * SynthEffect is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with SynthEffect  If not, see <http://www.gnu.org/licenses/>.
*/
#include <dv-config.h>
#include <davroska.h>
#include <adsr.h>
#include <synth-stdio.h>

/* adsr_init() - configures the specified adsr structure with its four parameters
*/
void adsr_init(struct adsr_s *adsr, dv_i32_t a, dv_i32_t d, dv_i32_t s, dv_i32_t r, dv_i32_t sps)
{
	adsr->a = a;
	adsr->d = d;
	adsr->s = s;
	adsr->r = r;

	/* Shouldn't need 64-bit computation for this. Worst is 48000*1024 at current estimates
	*/
	adsr->tAttack = (sps * a)/ADSR_AMAX;
	adsr->tDecay = (sps * d)/ADSR_DMAX;
	adsr->gSustain = (s > ADSR_GMAX) ? ADSR_GMAX : s;
	adsr->tRelease = (sps * r)/ADSR_RMAX;

	adsr->gDecay = ADSR_GMAX - adsr->gSustain;
	adsr->tSustain = adsr->tAttack + adsr->tDecay;
	adsr->tTotal = adsr->tAttack + adsr->tDecay + adsr->tRelease;

	sy_printf("adsr_init(): A = %d, D = %d, S = %d, R = %d\n",
				adsr->a, adsr->d, adsr->s, adsr->r);
	sy_printf("adsr_init(): tAttack = %d, tDecay = %d, gSustain = %d, tRelease = %d\n",
				adsr->tAttack, adsr->tDecay, adsr->gSustain, adsr->tRelease);
	sy_printf("adsr_init(): gDecay = %d, tSustain = %d, tTotal = %d\n",
				adsr->gDecay, adsr->tSustain, adsr->tTotal);
}

/* adsr_set_x() - four functions to set the adsr parameters individually
*/
void adsr_set_a(struct adsr_s *adsr, dv_i32_t a, dv_i32_t sps)
{
	adsr->a = a;
	adsr->tAttack = (sps * a)/ADSR_AMAX;
	adsr->tSustain = adsr->tAttack + adsr->tDecay;
	adsr->tTotal = adsr->tAttack + adsr->tDecay + adsr->tRelease;
}

void adsr_set_d(struct adsr_s *adsr, dv_i32_t d, dv_i32_t sps)
{
	adsr->d = d;
	adsr->tDecay = (sps * d)/ADSR_AMAX;
	adsr->tSustain = adsr->tAttack + adsr->tDecay;
	adsr->tTotal = adsr->tAttack + adsr->tDecay + adsr->tRelease;
}

void adsr_set_s(struct adsr_s *adsr, dv_i32_t s, dv_i32_t sps)
{
	adsr->s = s;
	adsr->gSustain = (s > ADSR_GMAX) ? ADSR_GMAX : s;
	adsr->gDecay = ADSR_GMAX - adsr->gSustain;
}

void adsr_set_r(struct adsr_s *adsr, dv_i32_t r, dv_i32_t sps)
{
	adsr->r = r;
	adsr->tRelease = (sps * r)/ADSR_RMAX;
	adsr->tTotal = adsr->tAttack + adsr->tDecay + adsr->tRelease;
}

/* envelope_gen() - generates an envelope signal when called once for every sample.
*/
dv_i32_t envelope_gen(struct envelope_s *env)
{
	if ( env->position < 0 )
	{
#if 0
		env->state = 'x';
#endif
		return 0;					/* Not in use */
	}

	struct adsr_s *adsr = env->adsr;

	if ( env->position < adsr->tAttack )
	{
		/* Attack phase
		*/
		env->position++;
		dv_i32_t v = (ADSR_GMAX * env->position) / adsr->tAttack;
#if 0
		if ( env->state != 'a' )
		{
			env->state = 'a';
			sy_printf("Attack: %d, %d\n", env->position, v);
		}
#endif
		return v;
	}

	if ( env->position < adsr->tSustain )
	{
		/* Attack phase
		*/
		env->position++;
		dv_i32_t v = adsr->gSustain + (adsr->gDecay * (env->position - adsr->tAttack)) / adsr->tDecay;
#if 0
		if ( env->state != 'd' )
		{
			env->state = 'd';
			sy_printf("Decay: %d, %d\n", env->position, v);
		}
#endif
		return v;
	}

	if ( env->position == adsr->tSustain )
	{
		/* Attack phase
		*/
#if 0
		if ( env->state != 's' )
		{
			env->state = 's';
			sy_printf("Sustain: %d\n", adsr->gSustain);
		}
#endif
		return adsr->gSustain;
	}

	env->position++;

	if ( env->position <= adsr->tTotal )
	{
		/* Release phase
		*/
		dv_i32_t v = adsr->gSustain * (env->position - adsr->tSustain) / adsr->tDecay;
#if 0
		if ( env->state != 'r' )
		{
			env->state = 'r';
			sy_printf("Release: %d\n", v);
		}
#endif
		return v;
	}

	/* End of release phase
	*/
#if 0
	sy_printf("Envelope finished %d\n", env->position);
	env->state = 'x';
#endif
	env->position = -1;
	return 0;
}
