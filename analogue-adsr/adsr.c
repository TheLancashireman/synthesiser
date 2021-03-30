/* adsr.c - attack-decay-sustain-release state machine
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
#include <avr/pgmspace.h>

static u8_t adsr_state;
static u16_t adsr_timer;		// Counts ADSR_TICKS
static u16_t r_level;			// DAC value from which release starts

/* adsr() - the attack-decay-sustain-release state machine
 *
 * called once every ADSR_TICK
*/
void adsr(void)
{
	switch ( adsr_state )
	{
	case ADSR_ATTACK:
		adsr_timer++;
		if ( adsr_timer >= a_time )
		{
			set_dac(MAX_DAC);
			adsr_state = ADSR_DECAY;
			adsr_timer = 0;
		}
		else
		{
			// Round up
			set_dac((MAX_DAC * adsr_timer + a_time - 1)/a_time);
		}
		break;

	case ADSR_DECAY:
		adsr_timer++;
		if ( adsr_timer >= d_time )
		{
			set_dac(s_level);
			r_level = s_level;
			adsr_state = ADSR_SUSTAIN;
			adsr_timer = 0;
		}
		else
		{
			// Round up
			r_level = MAX_DAC - ((MAX_DAC - s_level) * adsr_timer + d_time - 1)/d_time;
			set_dac(r_level);
		}
		break;

	case ADSR_SUSTAIN:
		// Same level until gate is off
		set_dac(s_level);
		break;

	case ADSR_RELEASE:
		adsr_timer++;
		if ( adsr_timer >= r_time )
		{
			set_dac(0);
			r_level = 0;
			adsr_state = ADSR_IDLE;
			adsr_timer = 0;
		}
		else
		{
			set_dac(r_level - r_level * adsr_timer / r_time);
		}
		break;

	default:
		set_dac(0);
		break;
	}
}

void gate_on(void)
{
	adsr_state = ADSR_ATTACK;
	adsr_timer = 0;
}

void gate_off(void)
{
	adsr_state = ADSR_RELEASE;
	adsr_timer = 0;
}
