/*	synth-effect-main.c - startup/control for SynthEffect project
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
#define DV_ASM  0
#include <davroska.h>
#include <dv-stdio.h>

#include <dv-arm-bcm2835-gpio.h>
#include <dv-arm-bcm2835-pcm.h>

#include <synth-config.h>
#include <midi.h>
#include <wave.h>
#include <effect.h>
#include <effect-adc.h>
#include <effect-dac.h>
#include <effect-synth.h>

/* Fixed stages
 *
 * Note: the index does not imply the order.
*/
#define EFFECT_STAGE_ADC	0
#define EFFECT_STAGE_DAC	1
#define EFFECT_STAGE_CTRL	2		/* For single-core */
#define EFFECT_STAGE_SYNTH	3

struct effect_s effect_stage[N_EFFECT_STAGES];

#if 0
/* effect_synthcontrol() - a dummy effect stage to handle MIDI input, console I/O etc.
*/
struct synthcontrol_s
{
	int dot_time;
	int dot_line_count;
	unsigned current_note;
	int passthru;
};

struct synthcontrol_s synthcontrol;

/* Init function ... */
	synthcontrol.dot_time = 0;
	synthcontrol.dot_line_count = 0;
	synthcontrol.current_note = 256;
	synthcontrol.passthru = 0;
	effect_stage[EFFECT_STAGE_CTRL].func = &effect_synthcontrol;
	effect_stage[EFFECT_STAGE_CTRL].control = &synthcontrol;
	effect_stage[EFFECT_STAGE_CTRL].name = "synthcontrol";
/* End of init function */

dv_i64_t effect_synthcontrol(struct effect_s *e, dv_i64_t signal)
{
	struct synthcontrol_s *ctrl = (struct synthcontrol_s *)e->control;

	/* Alive indication.
	*/
	ctrl->dot_time ++;
	if ( ctrl->dot_time >= SAMPLES_PER_SEC )
	{
		dv_consoledriver.putc('.');			/* One dot per second */
		ctrl->dot_time = 0;
		ctrl->dot_line_count++;
		if ( ctrl->dot_line_count >= 60 )
		{
			dv_consoledriver.putc('\r');	/* One line of characters per minute */
			dv_consoledriver.putc('\n');
			ctrl->dot_line_count = 0;
		}
	}

	/* Monitor the midi input. Control the tome generator.
	*/
	unsigned *cmd = midi_scan();
	if ( cmd != DV_NULL )
	{
		/* Updating the generator data like this only works if generation is running in the
		 * same thread as the midi detection.
		 * If not, we need some synchronisation.
		*/
		if ( cmd[0] == 0x90 )		/* Note on */
		{
			dv_printf("start(%d)\n", cmd[1]);
			synth_start_note(&effect_stage[EFFECT_STAGE_SYNTH], cmd[1]);
		}
		else if ( cmd[0] == 0x80 )	/* Note off */
		{
			dv_printf("stop(%d)\n", cmd[1]);
			synth_stop_note(&effect_stage[EFFECT_STAGE_SYNTH], cmd[1]);
		}
		else if ( cmd[0] == 0xb0 )	/* Controller change */
		{
			dv_printf("ctrl(%d, %d)\n", cmd[1], cmd[2]);
			synth_control(&effect_stage[EFFECT_STAGE_SYNTH], cmd[1], cmd[2]);
		}
	}

	return ctrl->passthru ? signal : 0;
}
#endif

/* syntheffect_init() - the function that runs at startup
 *
 * Initialise all the synth data structures
*/
void syntheffect_init(void)
{
	dv_printf("syntheffect_init: started.\n");

	/* Initialise the waveform tables
	*/
	dv_printf("syntheffect_init: calling wave_init().\n");
	if ( wave_init() != 0 )
	{
		panic("wave_init", "Oops! wave buffer too small");
	}

	/* Generate a set of root waveforms
	*/
	dv_printf("syntheffect_init: calling wave_generate(SAW).\n");
	wave_generate(SAW);

	/* Initialise an initial set of effect stages
	*/
	dv_printf("syntheffect_init: calling effect_init().\n");
	effect_init();

	dv_printf("syntheffect_init: adding ADC effect\n");
	effect_adc_init(&effect_stage[EFFECT_STAGE_ADC]);
	effect_append(&effect_stage[EFFECT_STAGE_ADC]);

#if 0
	dv_printf("syntheffect_init: adding CTRL effect\n");
	effect_ctrl_init(&effect_stage[EFFECT_STAGE_CTRL]);
	effect_append(&effect_stage[EFFECT_STAGE_CTRL]);
#endif

	dv_printf("syntheffect_init: adding synth effect\n");
	effect_synth_init(&effect_stage[EFFECT_STAGE_SYNTH]);
	effect_append(&effect_stage[EFFECT_STAGE_SYNTH]);

	dv_printf("syntheffect_init: adding DAC effect\n");
	effect_dac_init(&effect_stage[EFFECT_STAGE_DAC]);		/* DAC output stage */
	effect_append(&effect_stage[EFFECT_STAGE_DAC]);

	{
		struct effect_s *e = effect_list.next;

		dv_printf("List of effect processors:\n");
		while ( e != DV_NULL )
		{
			if ( e->name == DV_NULL )
				dv_printf("***Unknown***\n");
			else
				dv_printf("%s\n", e->name);

			e = e->next;
		}
	}

	/* Initialise the PCM hardware for I2S
	*/
	dv_printf("syntheffect_init: calling dv_pcm_init_i2s()\n");
	dv_pcm_init_i2s();
}

void run_core1(void)
{
	/* Do the stuff!
	*/
	dv_printf("run_core1: calling effect_processor()\n");
	effect_processor();		/* Never returns */
}

void run_core2(void)
{
	/* Nothing to do yet
	*/
	dv_printf("run_core2: sleeping\n");
	for (;;)
	{
		__asm ("wfe");
	}
}

void run_core3(void)
{
	/* Nothing to do yet
	*/
	dv_printf("run_core3: sleeping\n");
	for (;;)
	{
		__asm ("wfe");
	}
}

void panic(char *func, char *msg)
{
	dv_disable();
	dv_printf("Panic in %s : %s\n\n", func, msg);
	for (;;) {}
}
