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

/* prj_main() - the function that runs at startup
 *
 * This function runs in a thread, so it really shouldn't call dv_kprintf()
*/
void prj_main(void)
{
	dv_kprintf("prj_main: started.\n");

	/* 
	 * GPIO18 Alt0 = PCM_CLK
	 * GPIO19 Alt0 = PCM_FS
	 * GPIO20 Alt0 = PCM_DIN
	 * GPIO21 Alt0 = PCM_DOUT
	 * If the I2S interface is in slave mode, maybe best to set the pin modes after
	 * configuring the unit.
	*/
	dv_arm_bcm2835_gpio_pinconfig(18, DV_pinfunc_alt0, DV_pinpull_none);
	dv_arm_bcm2835_gpio_pinconfig(19, DV_pinfunc_alt0, DV_pinpull_none);
	dv_arm_bcm2835_gpio_pinconfig(20, DV_pinfunc_alt0, DV_pinpull_up);
	dv_arm_bcm2835_gpio_pinconfig(21, DV_pinfunc_alt0, DV_pinpull_none);

	/* Disable the clock
	*/
	dv_bcm2835_pcm.pcm_mode |= DV_PCM_MODE_CLK_DIS;		/* Disable the clock */

	/* Set master mode for clk and fs. Not PDM, not packed, not inverted.
	 * FS len = 32 (bits 9..0)
	 * Frame len = 64 (bits 19.10)
	*/
	dv_bcm2835_pcm.pcm_mode = DV_PCM_MODE_CLK_DIS | 32 | (64<<10);

	/* Set transmit config
	 * 32 bits (CH1WEX = CH2WEX = 1, CH1WID = CH2WID = 8)
	 * CH1 & CH2 enabled
	 * CH1 pos = 0, CH2 pos = 32
	*/
	dv_bcm2835_pcm.pcm_txc = DV_PCM_xXC_CH1WEX | DV_PCM_xXC_CH1EN | (0<<20) | (8<<16) |
							 DV_PCM_xXC_CH2WEX | DV_PCM_xXC_CH2EN | (32<<4) | (8<<0);

	dv_bcm2835_pcm.pcm_mode &= ~DV_PCM_MODE_CLK_DIS;

	int i = 0;
	int j = 0;
	int samples_per_dot = 88200;	/* CD sample rate * 2 */
	for (;;)
	{
		dv_pcm_write(0);
		i++;
		if ( i >= samples_per_dot )
		{
			dv_consoledriver.putc('.');
			i = 0;
			j++;
			if ( j >= 64 )
			{
				dv_consoledriver.putc('\r');
				dv_consoledriver.putc('\n');
				j = 0;
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

