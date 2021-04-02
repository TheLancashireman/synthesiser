/* pots.c - monitor the A/D/S/R potentiometers
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

u16_t a_val;
u16_t d_val;
u16_t s_val;
u16_t r_val;

static u8_t timer;
static u8_t pot;
static u8_t conv;

/* read_pots() - read the potentiometer values and convert to the ADSR parameters
 *
 * Reading the pots every 100 ms should give a reasonable response time.
 * That means we have to read one pot every 25 ms. That duration should give enough
 * time for the ADC input multiplexer to settle.
 *
 * The algorithm is:
 *		- when the time counter reaches 25 ms, set it to 0 and start the conversion
 *		- on subsequent ticks, monitor the conversion
 *		- when the conversion is complete, read the ADC then switch the multiplexer to the next pot
 *
 *
*/
void read_pots(void)
{
	timer++;

	if ( timer >= ADC_TIME )
	{
		timer = 0;
		conv = 1;
		start_conversion();
	}
	else if ( conv && !is_converting() )
	{
		conv = 0;
		u16_t adcval = read_adc();
		adc_clear_if();

		switch ( pot )
		{
		case a_pin:
			a_val = adcval;
			pot = d_pin;
			break;

		case d_pin:
			d_val = adcval;
			pot = s_pin;
			break;

		case s_pin:
			s_val = adcval;
			pot = r_pin;
			break;

		case r_pin:
			r_val = adcval;
			pot = a_pin;
			break;

		default:
			pot = a_pin;
			break;
		}

		select_adc_pin(pot);
	}
}

/* init_pots() - initialise the adc and the pots state machine
 *
 * ADC clock must be between 50 kHz and 200 kHz.
 * With 16 MHz CPU clock, /128 gives 125 kHz; /64 would be 250 kHz
*/
void init_pots(void)
{
	init_adc(ADMUX_VCC, ADCSR_PS_128);		// ADC clock = 16 MHz/128 = 125 kHz

	// All analogue pins to inputs, turn off the digital buffers
	pin_mode(a_pin, INPUT);
	pin_mode_analogue(a_pin, 1);
	pin_mode(d_pin, INPUT);
	pin_mode_analogue(d_pin, 1);
	pin_mode(s_pin, INPUT);
	pin_mode_analogue(s_pin, 1);
	pin_mode(r_pin, INPUT);
	pin_mode_analogue(r_pin, 1);

	// Start the state machine
	pot = a_pin;
	select_adc_pin(pot);
}
