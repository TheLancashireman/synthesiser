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


static void read_adc(void);
static void print_adc(void);

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

	/* Now some code that generates a signal for testing.
	*/
	int i = 0;
	int j = 0;
	const int samples_per_sec = 48000;
	const int hz = 440;
	const int gain = 65536*256;
	int smax = (samples_per_sec/hz)/2;
	int s = 0;
	for (;;)
	{
		/* Write the sample to one channel, the inverse to the other
		*/
		dv_pcm_write(s*gain);
		dv_pcm_write((-s)*gain);

#if 1
		read_adc();
#endif

		s++;
		if ( s > smax )
		{
			s = -smax;
		}

		i++;
#if 0
		if ( i <= 8 )
		{
			dv_consoledriver.putc('*');
			j++;
		}
#endif
		if ( i >= samples_per_sec )
		{
			dv_consoledriver.putc('.');
			i = 0;
			j++;
			if ( j >= 10 )
			{
				/* One line of characters per minute */
				dv_consoledriver.putc('\r');
				dv_consoledriver.putc('\n');
				j = 0;
#if 1
				print_adc();			/* This will fsck up the timing bigly */
#endif
			}
		}
	}
}

dv_i32_t s1_min = 2147483647;
dv_i32_t s1_max = -2147483648;
dv_i32_t s2_min = 2147483647;
dv_i32_t s2_max = -2147483648;

static void read_adc(void)
{
	dv_i32_t s;

	dv_pcm_read(&s);

	if ( s > s1_max ) s1_max = s;
	if ( s < s1_min ) s1_min = s;

	dv_pcm_read(&s);

	if ( s > s2_max ) s2_max = s;
	if ( s < s2_min ) s2_min = s;
}

static void print_adc(void)
{
	dv_kprintf("max1, min1, max2, min2 = %d, %d, %d, %d\n", s1_max, s1_min, s2_max, s2_min);
	s1_min = 2147483647;
	s1_max = -2147483648;
	s2_min = 2147483647;
	s2_max = -2147483648;
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

