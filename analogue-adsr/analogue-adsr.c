/* analogue-adrs.c - a digital adsr envelope generator for an analogue synthesiser
 *
 * (c) David Haworth
 *
 *  This file is part of analogue-adsr.
 *
 *  analogue-adsr is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  analogue-adsr is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with analogue-adsr.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "adsr.h"
#include "tinylib.h"
#include "tinyadc.h"

u8_t dac_value;

static void init_dac(void);

int main(void)
{
	timing_init();
	enable();

	init_gate();
	init_dac();
	init_pots();

	u32_t then = read_time_32();
	u32_t now;

	for (;;)
	{
		// Wait for the next ADSR tick
		do {
			now = read_time_32();
		} while ( (now - then) < ( ADSR_TICK / T0_RESOLUTION ) );
		then = now;

		// Maintain the ADSR state machine
		adsr();

		// Monitor the gate pin
		read_gate();

		// Monitor the envelope parameters
		read_pots();
	}

	return 0;
}

/* init_dac() - intitialise the PWM D/A converter using OC1A
*/
static void init_dac(void)
{
	pin_mode(cvpin, OUTPUT);
	TCCR1 = (1 << CS10) | (1 << COM1A1) | (1 << PWM1A);		// PWM mode; set OC1A at 0, clear OC1A at match
	OCR1A = 0;
	OCR1C = 0xff;
}

/* set_dac() - set the D/A conversion value
*/
void set_dac(u16_t d)
{
	if ( d > MAX_DAC )
		d = MAX_DAC;

	if ( dac_value != d )
	{
		dac_value = d;
		OCR1A = d;
	}
}
