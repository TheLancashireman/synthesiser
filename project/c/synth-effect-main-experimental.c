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

#include <project/h/midi.h>
#include <project/h/wave.h>

/* prj_main() - the function that runs at startup
 *
 * This function runs in a thread, so it really shouldn't call dv_kprintf()
*/
void prj_main(void)
{
	const dv_i32_t samples_per_sec = 48000;
	unsigned current_note = 256;
	dv_kprintf("prj_main: started.\n");

	/* Initialise the PCM hardware for I2S
	*/
	dv_pcm_init_i2s();

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

	/* Timing for the dots
	*/
	int i = 0;
	int j = 0;

	for (;;)
	{
		dv_i32_t sLeft, sRight;
		dv_i32_t sGen;

		/* Read (and discard) analogue inputs.
		*/
		dv_pcm_read(&sLeft);
		dv_pcm_read(&sRight);

		/* Play the note on the left channel. Right channel fixed at 0
		*/
		sGen = wave_play_mono(0)/2;
		dv_pcm_write(sGen);
		dv_pcm_write(0);

		/* Print the "alive indication" dots
		*/
		i++;
		if ( i >= samples_per_sec )
		{
			dv_consoledriver.putc('.');
			i = 0;
			j++;
			if ( j >= 60 )
			{
				/* One line of characters per minute */
				dv_consoledriver.putc('\r');
				dv_consoledriver.putc('\n');
				j = 0;
			}
		}

		/* Monitor the midi input. Control the tome generator.
		*/
		unsigned *cmd = midi_scan();
		if ( cmd != DV_NULL )
		{
			/* Quick hack of a monophonic synthesiser. Each new note replaces the old one.
			 * Note off only recognised if it's the current note.
			*/
			/* Updating the generator data like this only works if generation is running in the
			 * same thread as the midi detection.
			 * If not, we need some synchronisation.
			*/
			if ( cmd[0] == 0x90 )	/* Note on */
			{
				if ( current_note < 128 )
				{
					wave_stop_mono(0);
					dv_kprintf("stop(0)\n");
				}
				current_note = cmd[1];
				/* Midi note 0 is C 8.175 Hz (i.e. the C of our root table, index 3)
				*/
				int n = (current_note+3)%12;
				int h = 1 << ((current_note+3)/12);
				wave_start_mono(0, n, h);
				dv_kprintf("start(0, %d, %d)\n", n, h);
			}
			else if ( cmd[0] == 0x80 && current_note == cmd[1] )	/* Note off, current note */
			{
				current_note = 256;
				wave_stop_mono(0);
				dv_kprintf("stop(0)\n");
			}
		}
	}
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

