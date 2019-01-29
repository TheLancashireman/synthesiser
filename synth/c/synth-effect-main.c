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
#include <kernel/h/dv-kconfig.h>
#include <kernel/h/dv-types.h>
#include <kernel/h/dv-error.h>
#include <kernel/h/dv-stdio.h>
#include <kernel/h/dv-api.h>
#include <kernel/h/dv-error.h>

#include DV_H_SYSTEMTIMER
#include DV_H_CONSOLEUART

#include <devices/h/dv-arm-bcm2835-gpio.h>
#include <devices/h/dv-arm-bcm2835-pcm.h>

#include <project/h/synth-config.h>
#include <project/h/midi.h>
#include <project/h/wave.h>
#include <project/h/effect.h>
#include <project/h/effect-adc.h>
#include <project/h/effect-dac.h>
#include <project/h/effect-synth.h>


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

struct effect_s effect_stage[N_EFFECT_STAGES];

/* Fixed stages
 *
 * Note: the index does not imply the order.
*/
#define EFFECT_STAGE_ADC	0
#define EFFECT_STAGE_DAC	1
#define EFFECT_STAGE_CTRL	2		/* For single-core */
#define EFFECT_STAGE_SYNTH	3

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
			dv_kprintf("start(%d)\n", cmd[1]);
			synth_start_note(&effect_stage[EFFECT_STAGE_SYNTH], cmd[1]);
		}
		else if ( cmd[0] == 0x80 )	/* Note off */
		{
			dv_kprintf("stop(%d)\n", cmd[1]);
			synth_stop_note(&effect_stage[EFFECT_STAGE_SYNTH], cmd[1]);
		}
		else if ( cmd[0] == 0xb0 )	/* Controller change */
		{
			dv_kprintf("ctrl(%d, %d)\n", cmd[1], cmd[2]);
			synth_control(&effect_stage[EFFECT_STAGE_SYNTH], cmd[1], cmd[2]);
		}
	}

	return ctrl->passthru ? signal : 0;
}

/* prj_main() - the function that runs at startup
 *
 * This function runs in a thread, so it really shouldn't call dv_kprintf()
*/
void prj_main(void)
{
	dv_kprintf("prj_main: started.\n");

	/* Initialise the waveform tables
	*/
	dv_kprintf("prj_main: calling wave_init().\n");
	if ( wave_init() != 0 )
	{
		dv_panic(dv_panic_unimplemented, "wave_init", "Oops! wave buffer too small");
	}

	/* Generate a set of root waveforms
	*/
	dv_kprintf("prj_main: calling wave_generate(SAW).\n");
	wave_generate(SAW);

	/* Initialise an initial set of effect stages
	*/
	dv_kprintf("prj_main: calling effect_init().\n");
	effect_init();

	dv_kprintf("prj_main: adding ADC effect\n");
	effect_adc_init(&effect_stage[EFFECT_STAGE_ADC]);
	effect_append(&effect_stage[EFFECT_STAGE_ADC]);

	dv_kprintf("prj_main: adding synth_control effect\n");
	synthcontrol.dot_time = 0;
	synthcontrol.dot_line_count = 0;
	synthcontrol.current_note = 256;
	synthcontrol.passthru = 0;
	effect_stage[EFFECT_STAGE_CTRL].func = &effect_synthcontrol;
	effect_stage[EFFECT_STAGE_CTRL].control = &synthcontrol;
	effect_stage[EFFECT_STAGE_CTRL].name = "synthcontrol";
	effect_append(&effect_stage[EFFECT_STAGE_CTRL]);

	dv_kprintf("prj_main: adding synth effect\n");
	effect_synth_init(&effect_stage[EFFECT_STAGE_SYNTH]);
	effect_append(&effect_stage[EFFECT_STAGE_SYNTH]);

	dv_kprintf("prj_main: adding DAC effect\n");
	effect_dac_init(&effect_stage[EFFECT_STAGE_DAC]);		/* DAC output stage */
	effect_append(&effect_stage[EFFECT_STAGE_DAC]);

	{
		struct effect_s *e = effect_list.next;

		dv_kprintf("List of effect processors:\n");
		while ( e != DV_NULL )
		{
			if ( e->name == DV_NULL )
				dv_kprintf("***Unknown***\n");
			else
				dv_kprintf("%s\n", e->name);

			e = e->next;
		}
	}

	/* Initialise the PCM hardware for I2S
	*/
	dv_kprintf("prj_main: calling dv_pcm_init_i2s()\n");
	dv_pcm_init_i2s();

	/* Do the stuff!
	*/
	dv_kprintf("prj_main: calling effect_processor()\n");
	effect_processor();		/* Never returns */
}


/* Handlers for all unimplemented exceptions.
 *
 * ToDo: remove these when davros provides handlers.
*/
void dv_catch_thread_fiq(dv_kernel_t *kvars)
{
	dv_panic(dv_panic_unimplemented, "dv_catch_thread_fiq", "Oops! An exception occurred");
}

void dv_catch_thread_syserror(dv_kernel_t *kvars)
{
	dv_panic(dv_panic_unimplemented, "dv_catch_thread_syserror", "Oops! An exception occurred");
}

void dv_catch_kernel_synchronous_exception(dv_kernel_t *kvars)
{
	dv_panic(dv_panic_unimplemented, "dv_catch_kernel_synchronous_exception", "Oops! An exception occurred");
}

void dv_catch_kernel_irq(dv_kernel_t *kvars)
{
	dv_panic(dv_panic_unimplemented, "dv_catch_kernel_irq", "Oops! An exception occurred");
}

void dv_catch_kernel_fiq(dv_kernel_t *kvars)
{
	dv_panic(dv_panic_unimplemented, "dv_catch_kernel_fiq", "Oops! An exception occurred");
}

void dv_catch_kernel_syserror(dv_kernel_t *kvars)
{
	dv_panic(dv_panic_unimplemented, "dv_catch_kernel_syserror", "Oops! An exception occurred");
}

void dv_catch_aarch32_synchronous_exception(dv_kernel_t *kvars)
{
	dv_panic(dv_panic_unimplemented, "dv_catch_aarch32_synchronous_exception", "Oops! An exception occurred");
}

void dv_catch_aarch32_irq(dv_kernel_t *kvars)
{
	dv_panic(dv_panic_unimplemented, "dv_catch_aarch32_irq", "Oops! An exception occurred");
}

void dv_catch_aarch32_fiq(dv_kernel_t *kvars)
{
	dv_panic(dv_panic_unimplemented, "dv_catch_aarch32_fiq", "Oops! An exception occurred");
}

void dv_catch_aarch32_syserror(dv_kernel_t *kvars)
{
	dv_panic(dv_panic_unimplemented, "dv_catch_aarch32_syserror", "Oops! An exception occurred");
}

/* dv_panic() - printf error message, stay in endless loop
 *
 * ToDo: remove this when davros provides the function.
*/
void dv_panic(dv_panic_t reason, char *function, char *message)
{
	dv_kprintf("Davros panic %d in %s: %s\n", reason, function, message);
	for (;;) {}
}

/* prj_exc_handler() - handle traps to EL2/EL3
*/
void prj_exc_handler(dv_u64_t x0, dv_u64_t x1, dv_u64_t x2, dv_u64_t x3)
{
    dv_kprintf("prj_exc_handler() invoked\n");
	for (;;) {}
}

