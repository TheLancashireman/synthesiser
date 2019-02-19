/*	synth-config.h - configuration header file for SynthEffect
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
#ifndef SYNTH_CONFIG_H
#define SYNTH_CONFIG_H	1

/* Configuration settings:
 *	SAMPLES_PER_SEC must match the hardware sample rate
 *	MAX_POLYPHONIC is the number of simultaneous synthesized notes available
 *	MAX_EFFECT_STAGES is the number of "effects" available. Includes ADC and DAC.
 *	ADSR_xMAX are the values considered to be full range.
*/

#define SAMPLES_PER_SEC		48000	/* Fixed by the ADC/DAC clock */
#define MAX_POLYPHONIC		16		/* No. of note generators available */
#define N_EFFECT_STAGES		20		/* Total no. of effects */

#define ADSR_GMAX			128		/* Max output level. This value represents 1.0 */
#define ADSR_AMAX			128		/* a = 128 -> 1 sec */
#define ADSR_DMAX			128		/* d = 128 -> 1 sec */
#define ADSR_RMAX			128		/* r = 128 -> 1 sec */

#define SYNTH_GAIN1			1000	/* Unity gain */

/* Continuous controllers (MIDI command 0xb-)
*/
#define SYNTH_CTRL_ENVELOPE_A	0	/* Note attack time */
#define SYNTH_CTRL_ENVELOPE_D	1	/* Note decay time */
#define SYNTH_CTRL_ENVELOPE_S	2	/* Note sustain level */
#define SYNTH_CTRL_ENVELOPE_R	3	/* Note release time */

#define SYNTH_CTRL_N_POLY		128	/* No. of polyphonic channels */

/* Configuration of davroska-related features
*/
#define TICK_INTERVAL_ms		10
#define MONTIOR_INTERVAL_ms		1000

extern void syntheffect_init();
extern void panic(char *func, char *msg);
extern void run_core1(void);
extern void run_core2(void);
extern void run_core3(void);
extern void init_timing(void);

#endif
