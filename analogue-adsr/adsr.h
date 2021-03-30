/* adsr.h - a digital adsr envelope generator for an analogue synthesiser
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
#ifndef ADSR_H
#define ADSR_H		1

#include "tinylib.h"

#define gatepin			PB0
#define cvpin			PB1		// OC1A (PWM)

#define ADSR_TICK		1000				// 1 ms
#define GATE_DEBOUNCE	(20000/ADSR_TICK)	// 20 ms worth of ADSR ticks

#define ADSR_IDLE		0
#define ADSR_ATTACK		1
#define ADSR_DECAY		2
#define ADSR_SUSTAIN	3
#define ADSR_RELEASE	4

#define MAX_DAC			255

extern u16_t a_time;
extern u16_t d_time;
extern u16_t s_level;
extern u16_t r_time;

extern void adsr(void);
extern void gate_on(void);
extern void gate_off(void);
extern void set_dac(u16_t d);

#endif
