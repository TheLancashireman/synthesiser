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

#include <midi/midi.h>

static void calc_min_max(dv_i32_t sL, dv_i32_t sR);
static void print_min_max(void);

/* prj_main() - the function that runs at startup
 *
 * This function runs in a thread, so it really shouldn't call dv_kprintf()
*/
void prj_main(void)
{
	dv_kprintf("prj_main: started.\n");

	/* Initialise the PCM hardware for I2S
	*/
	dv_pcm_init_i2s();

	/* Timing for the dots
	*/
	int i = 0;
	int j = 0;

	/* Extremely crude signal generator.
	 * This signal is fed back to the ADC.
	*/
	const dv_i32_t samples_per_sec = 48000;
	const dv_i32_t hz = 440;
	const dv_i32_t gainGen = 65536*256;
	dv_i32_t smax = (samples_per_sec/hz)/2;
	dv_i32_t sGen = 0;

	/* Passthrough ADC-DAC
	*/
	dv_i32_t sLeft = 0;
	dv_i32_t sRight = 0;
	const dv_i32_t gainSig = 128;

	for (;;)
	{
		/* Write the sample to one channel, the inverse to the other
		*/
		dv_pcm_write(sGen*gainGen);
		dv_pcm_write(sLeft*gainSig);

		sGen++;
		if ( sGen > smax )
		{
			sGen = -smax;
		}

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
#if 0
				print_min_max();			/* This will fsck up the timing bigly */
#endif
			}
		}

		midi_scan();

		dv_pcm_read(&sLeft);
		dv_pcm_read(&sRight);

		calc_min_max(sLeft, sRight);
	}
}

dv_i32_t sL_min = 2147483647;
dv_i32_t sL_max = -2147483648;
dv_i32_t sR_min = 2147483647;
dv_i32_t sR_max = -2147483648;

static void calc_min_max(dv_i32_t sL, dv_i32_t sR)
{
	if ( sL > sL_max ) sL_max = sL;
	if ( sL < sL_min ) sL_min = sL;

	if ( sR > sR_max ) sR_max = sR;
	if ( sR < sR_min ) sR_min = sR;
}

static void print_min_max(void)
{
	dv_kprintf("max1, min1, max2, min2 = %d, %d, %d, %d\n", sL_max, sL_min, sR_max, sR_min);
	sL_min = 2147483647;
	sL_max = -2147483648;
	sR_min = 2147483647;
	sR_max = -2147483648;
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

