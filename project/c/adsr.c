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
#include <kernel/h/dv-kconfig.h>
#include <kernel/h/dv-types.h>
#include <project/h/adsr.h>

/* adsr_init() - configures the specified adsr structure with its four parameters
*/
void adsr_init(struct adsr_s *adsr, dv_i32_t a, dv_i32_t d, dv_i32_t s, dv_i32_t r, dv_i32_t sps)
{
	adsr->a = a;
	adsr->d = d;
	adsr->s = s;
	adsr->r = r;

	/* Shouldn't need 64-bit computation for this. Worst us 48000*1024 at current estimates
	*/
	adsr->tAttack = (sps * a)/AMAX;
	adsr->tDecay = (sps * d)/AMAX;
	adsr->gSustain = (s > GMAX) ? GMAX : s;
	adsr->tRelease = (sps * r)/RMAX;

	adsr->gDecay = GMAX - adsr->gSustain;
	adsr->tSustain = adsr->tAttack + adsr->tDecay;
	adsr->tTotal = adsr->tAttack + adsr->tDecay + adsr->tRelease;
}

/* adsr_set_x() - four functions to set the adsr parameters individually
*/
void adsr_set_a(struct adsr_s *adsr, dv_i32_t a, dv_i32_t sps)
{
	adsr->a = a;
	adsr->tAttack = (sps * a)/AMAX;
	adsr->tSustain = adsr->tAttack + adsr->tDecay;
	adsr->tTotal = adsr->tAttack + adsr->tDecay + adsr->tRelease;
}

void adsr_set_d(struct adsr_s *adsr, dv_i32_t d, dv_i32_t sps)
{
	adsr->d = d;
	adsr->tDecay = (sps * d)/AMAX;
	adsr->tSustain = adsr->tAttack + adsr->tDecay;
	adsr->tTotal = adsr->tAttack + adsr->tDecay + adsr->tRelease;
}

void adsr_set_s(struct adsr_s *adsr, dv_i32_t s, dv_i32_t sps)
{
	adsr->s = s;
	adsr->gSustain = (s > GMAX) ? GMAX : s;
	adsr->gDecay = GMAX - adsr->gSustain;
}

void adsr_set_r(struct adsr_s *adsr, dv_i32_t r, dv_i32_t sps)
{
	adsr->r = r;
	adsr->tRelease = (sps * r)/RMAX;
	adsr->tTotal = adsr->tAttack + adsr->tDecay + adsr->tRelease;
}

/* envelope_gen() - generates an envelope signal when called once for every sample.
*/
dv_i32_t envelope_gen(struct envelope_s *env)
{
	if ( env->position < 0 )		
		return 0;					/* Not in use */

	struct adsr_s *adsr = env->adsr;

	if ( env->position < adsr->tAttack )
	{
		env->position++;
		return (GMAX * env->position) / adsr->tAttack;
	}

	if ( env->position < adsr->tSustain )
	{
		env->position++;
		return adsr->gSustain + (adsr->gDecay * (env->position - adsr->tAttack)) / adsr->tDecay;
	}

	if ( env->position == adsr->tSustain )
		return adsr->gSustain;

	env->position++;

	if ( env->position > adsr->tTotal )
	{
		env->position = -1;
		return 0;
	}

	return adsr->gSustain * (env->position - adsr->tSustain) / adsr->tDecay;
}
