/* noise-geenerator.c - a digital noise generator
 *
 * (c) David Haworth
 *
 *  This file is part of noise-geenerator.
 *
 *  noise-geenerator is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  noise-geenerator is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with noise-geenerator.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "tinylib.h"

#define NOISE_PIN	PB1

#define SR_INIT		0x123456789abcdef0
#define SR_POLY		0xE800000000000000  /* x^64 + x^63 + x^61 + x^60 + 1 */

int main(void)
{
	pin_mode(NOISE_PIN, OUTPUT);

	u64_t sr = SR_INIT;
	u8_t lsb;

	for (;;)
	{
		lsb = (u8_t)sr & 0x01;
		sr = sr >> 1;
		if ( lsb)
		{
			PORTB |= 0x1 << NOISE_PIN;
			sr = sr ^ SR_POLY;
		}
		else
		{
			PORTB &= ~(0x1 << NOISE_PIN);
			asm("nop");
		}
	}

	return 0;
}
