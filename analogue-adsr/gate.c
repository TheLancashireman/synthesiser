/* read-gate.c - monitor the gate pin
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

#if DEBUG_REPEAT

#if DEBUG_PRINT
#include "tinyio.h"
#include <avr/pgmspace.h>
#endif

/* read_gate() - DEBUG VERSION ::: generate a repeating gate trigger for testing
 *
 * The trigger's timing adjusts itself to the envelope timing
*/
void read_gate(void)
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

#if DEBUG_PRINT
	static u16_t ioctr;
	static u8_t param;

	ioctr++;

	if ( ioctr >= 1000 )
	{
		ioctr = 0;
		char ch; u16_t val;
		switch (param)
		{
		case 0:		ch = 'A';	val = a_time;	break;
		case 1:		ch = 'D';	val = d_time;	break;
		case 2:		ch = 'S';	val = s_level;	break;
		case 3:		ch = 'R';	val = r_time;	break;
		default:
			param = 0;
			printf(PSTR("------\n"));
			return;
		}
		printf(PSTR("%c: %u\n"), ch, val);
		param++;
	}
#endif
}

/* init_gate() - DEBUG VERSION ::: nothing
*/
void init_gate(void)
{
#if DEBUG_PRINT
	async_init();
	printf(PSTR("Hello, world!\n"));
#endif
}

#else
/* read_gate() - monitor the gate input pin and trigger the envelope generator
*/
static u8_t gate_state;
static u8_t gate_debounce;

void read_gate(void)
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

/* init_gate() - initialise the gate inputS
*/
void init_gate(void)
{
	pin_mode(gatepin, PULLUP);
}

#endif
