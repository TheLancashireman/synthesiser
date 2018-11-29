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
 *	SAMPLES_PER_SEC must match the hardware sampleÂrate
 *	N_POLYPHONIC is the number of simultaneous synthesized notes
 *	N_EFFECT_STAGES is the number of "effects" available. Includes all notes, plus ADC, DAC and any others.
*/

#define SAMPLES_PER_SEC		48000
#define N_POLYPHONIC		16			/* No. of note generators */
#define N_EFFECT_STAGES		20			/* Total no. of effects */

#endif
