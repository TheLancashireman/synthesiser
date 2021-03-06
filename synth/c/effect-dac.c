/*	effect-dac.c - effect stage for sending output to DAC
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

#include <effect.h>
#include <effect-dac.h>

#include <dv-arm-bcm2835-pcm.h>

#include <monitor.h>

struct effect_dac_s effect_dac;

/* effect_dac_init() - intialise the DAC effect stage
*/
void effect_dac_init(struct effect_s *e)
{
	e->func = &effect_dac_output;
	e->control = &effect_dac;
	e->name = "dac";
	effect_dac.select = 1;
	effect_dac.max = 2147483647L;
	effect_dac.min = -2147483647L;
}

/* effect_dac_output() - write signal to DAC
 *
 * This pseudo-effect generator writes a signal to the DAC. It generates a zero output signal.
 *
 * Two DAC channels are written:
 *	- the selected channel is written with the input signal clipped to 32 bits
 *	- the other channel is written with zero
 *
 * Future: feed the unselected channel from a message queue so to permit two-channel operation.
*/
dv_i64_t effect_dac_output(struct effect_s *e, dv_i64_t signal)
{
	struct effect_dac_s *dac = (struct effect_dac_s *)e->control;
	dv_i32_t left = 0, right = 0;

	/* Clip the input so that it lies between min and max.
	*/
	if ( signal > dac->max )
		signal = dac->max;
	else if ( signal < dac->min )
		signal = dac->min;

	/* Select the signal into either right or left
	 *
	 * Temporary: divide the output values by 2 until I find out why the DAC doesn't see the first bit.
	*/
	if ( dac->select )
	{
		right = (dv_i32_t)(signal/2);
	}
	else
	{
		left = (dv_i32_t)(signal/2);
	}

	monitor_start(&core1_idle);

	/* Write the values to the DAC.
	*/
	dv_pcm_write(left);
	dv_pcm_write(right);

	monitor_elapsed(&core1_idle, monitor_frc());
	
	return 0;
}
