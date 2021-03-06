/*	effect-adc.c - effect statge to read input from ADC
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
#include <dv-config.h>
#include <davroska.h>
#include <synth-config.h>
#include <synth-stdio.h>

#include <effect.h>
#include <effect-adc.h>

#include <dv-arm-bcm2835-pcm.h>
#include <dv-arm-bcm2835-systimer.h>
#include <dv-arm-bcm2835-armtimer.h>

struct effect_adc_s effect_adc;

static dv_u32_t tmin;

/* effect_adc_init() - initialise the ADC input stage
*/
void effect_adc_init(struct effect_s *e)
{
	e->func = &effect_adc_input;
	e->control = &effect_adc;
	e->name = "adc";
	effect_adc.select = 1;
	effect_adc.pace = 0;
	tmin = 0xffffffff;
}

/* effect_adc_input() - read input from ADC
 *
 * This pseudo-effect generator reads input from the ADC. It ignores its own
 * input parameter.
 *
 * Two input channels are read. One is returned as the generator's output.
 * The other is ignored.
 *
 * Future: feed the second channel into a message queue so to permit two-channel
 * operation.
*/
dv_i64_t effect_adc_input(struct effect_s *e, dv_i64_t unused_signal)
{
	struct effect_adc_s *adc = (struct effect_adc_s *)e->control;
	dv_i32_t left, right;
	dv_u64_t tStart, tEnd;

#if 0
	adc->pace++;
	if ( (adc->pace % SAMPLES_PER_SEC) == 0 )
	{
		charbuf_putc(dv_get_coreidx(), '.');
		charbuf_putc(dv_get_coreidx(), '\0');		/* The NUL byte allows the scan loop to scan other chaannels */
	}
	if ( adc->pace >= (SAMPLES_PER_SEC * 60) )
	{
		charbuf_putc(dv_get_coreidx(), '\n');
		adc->pace = 0;
	}
#endif

	tStart = dv_arm_bcm2835_armtimer_read_frc();
	dv_pcm_read(&left);
	dv_pcm_read(&right);
	tEnd = dv_arm_bcm2835_armtimer_read_frc();

	if ( (tEnd - tStart) < tmin )
	{
		tmin = tEnd - tStart;
		sy_printf("idle: %u\n", tmin);
	}

	return (dv_i64_t)(adc->select ? right : left);
}
