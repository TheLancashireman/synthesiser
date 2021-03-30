/* analogue-adsr.c - a digital adsr envelope generator for an analogue synthesiser
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

#define DEBUG_REPEAT	1

// Temporary initial values for testing
u16_t a_time = 100;
u16_t d_time = 50;
u16_t s_level = 128;
u16_t r_time = 200;

static u8_t dac_value;

#if !DEBUG_REPEAT
static u8_t gate_state;
static u8_t gate_debounce;

static void gate(void);
#endif

static void init_dac(void);


static void debug_gate(void)
{
	static u16_t dctr = 100;

	if ( dctr > 0 )
	{
		dctr--;
	}
	else
	{
		if ( dac_value == 0 )
		{
			gate_on();
			dctr = a_time + d_time + 200;	// Time for A, D and S phases
		}
		else
		{
			gate_off();
			dctr = r_time + 100;			// Time for R phase plus a gap
		}
	}
}

int main(void)
{
	timing_init();
	pin_mode(cvpin, OUTPUT);
	pin_mode(gatepin, PULLUP);
	init_dac();
	enable();

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

#if DEBUG_REPEAT
		// Initial debugging -- generate a waveform
		debug_gate();
#else
		// Monitor the gate pin
		gate();
#endif
	}

	return 0;
}

#if !DEBUG_REPEAT
static void gate(void)
{
	if ( gate_debounce == 0 )
	{
		if ( pin_get(gatepin) )
		{
			// Pin is high --> gate signal is inactive
			if ( gate_state != 0 )
			{
				gate_off();
				gate_state = 0;
				gate_debounce = GATE_DEBOUNCE;
			}
		}
		else
		{
			// Pin is low --> gate signal is active
			if ( gate_state == 0 )
			{
				gate_on();
				gate_state = 1;
				gate_debounce = GATE_DEBOUNCE;
			}
		}
	}
	else
		gate_debounce--;
}
#endif

static void init_dac(void)
{
	TCCR1 = (1 << CS10) | (1 << COM1A0) | (1 << PWM1A);		// PWM mode; set OC1A at 0, clear OC1A at match
	OCR1A = 0;
	OCR1C = 0xff;
}

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
