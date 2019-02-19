/*	timing.c - davroska timing functions
 *
 *	Copyright 2019 David Haworth
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
#include <synth-davroska.h>
#include <synth-config.h>
#include <dv-arm-bcm2835-armtimer.h>

/* Timer_main() - timer interrupt function
 *
 * The timer interrupt occurs every TICK_INTERVAL_ms milliseconds.
 *
 * The interrupt advances the tick counter, from which all periodic activations are derived.
*/
void Timer_main(void)
{
	dv_arm_bcm2835_armtimer_clr_irq();			/* Clear the interrupt */

	(void)dv_advancecounter(TickCounter, 1);
}

/* af_MonitorAlarm() - alarm function to activate the Monitor task
*/
dv_u32_t af_MonitorAlarm(dv_id_t a)
{
	(void)dv_activatetask(Monitor);

	return MONTIOR_INTERVAL_ms/TICK_INTERVAL_ms;	/* Period is MONTIOR_INTERVAL_ms milliseconds */
}

/* init_timing() - initialise the timing functionality
*/
void init_timing(void)
{
	dv_arm_bcm2835_armtimer_init(1);            /* Use a prescaler of 1 for high resolution (250 MHz) */
	dv_arm_bcm2835_armtimer_set_load(250000 * TICK_INTERVAL_ms);
	dv_enable_irq(hw_TimerInterruptId);

	dv_setalarm_rel(TickCounter, MonitorAlarm, MONTIOR_INTERVAL_ms/TICK_INTERVAL_ms);
}
