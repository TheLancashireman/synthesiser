/*	wave.c - waveform generator
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
#include <project/h/wave.h>

#define TOTAL_SAMPLES	89138

const dv_i64_t maxi = 0x7fffffffL;
const dv_i64_t maxu = 0xffffffffL;

struct wavetable_s
{
	double f;
	dv_i32_t nsamp;
	dv_i32_t ncyc;
	dv_i32_t *wave;
};

/* The values in this table were calculated by the "wave" host program (in the host-calc
 * directory)
*/

struct wavetable_s wavetable[12] =					/*		48000/Freq		*/
{	{ 6.875,			6982,	1,	DV_NULL },		/*	A	6981.818182		*/
	{ 7.28380877372013,	6590,	1,	DV_NULL },		/*	Bb	6589.958838		*/
	{ 7.71692658212691,	6220,	1,	DV_NULL },		/*	B	6220.092868		*/
	{ 8.17579891564368,	5871,	1,	DV_NULL },		/*	C	5870.985881		*/
	{ 8.66195721802722,	11083,	2,	DV_NULL },		/*	C#	5541.472763		*/
	{ 9.17702399741896,	10461,	2,	DV_NULL },		/*	D	5230.453796		*/
	{ 9.722718241315,	4937,	1,	DV_NULL },		/*	Eb	4936.890981		*/
	{ 10.3008611535272,	4660,	1,	DV_NULL },		/*	E	4659.804582		*/
	{ 10.9133822322813,	8797,	2,	DV_NULL },		/*	F	4398.269847		*/
	{ 11.5623257097385, 8303,	2,	DV_NULL },		/*	F#	4151.413929		*/
	{ 12.2498573744296, 7837,	2,	DV_NULL },		/*	G	3918.412969		*/
	{ 12.9782717993732,	7397,	2,	DV_NULL }		/*	Ab	3698.489348		*/
};

dv_i32_t wave_buffer[TOTAL_SAMPLES];

struct tonegen_s
{
	struct wavetable_s *root;
	dv_i32_t harmonic;
	dv_i32_t position;
};

struct tonegen_s tonegen[N_TONEGEN];

/* wave_init() - initialise the wave tables for the 12 root waveforms.
 *
 * Returns non-zero (-1) if the wave buffer isn't big enough.
*/
int wave_init(void)
{
	int i, j=0;

	for ( i=0; i<12; i++ )
	{
		wavetable[i].wave = &wave_buffer[j];
		j += wavetable[i].nsamp;
	}
	if ( j > TOTAL_SAMPLES )
		return -1;
	return 0;
}

/* wave_generate() - generates all 12 root waveforms using the specified wave type (parameter)
 *
 * maxu and maxi are declared as 64-bit signed integers so that the computation of the
 * amplitude for each sample is performed in 64-bit arithmetic.
*/
void wave_generate(int wav)
{
	int i, j;

	for ( i=0; i<12; i++ )
	{
		for (j = 0; j < wavetable[i].nsamp; j++ )
		{
			if ( wav == SAW )
			{
				/* Sawtooth wave: generate a monotonically-increasing amplitude
				 * and use it (modulo full-scale)
				*/
				long amplitude = (maxu * j * wavetable[i].ncyc) / wavetable[i].nsamp;
				while ( amplitude > maxu )
					amplitude -= maxu;
				wavetable[i].wave[j] = amplitude - maxi;
			}
			else if ( wav == TRI || wav == SQU )
			{
				/* Triangle wave: generate a monotonically-increasing amplitude
				 * and fold it every time it exceeds the range.
				*/
				long amplitude = (maxu * 2 * j * wavetable[i].ncyc) / wavetable[i].nsamp;
				int folded;
				do {
					folded = 0;
					if ( amplitude > maxu )
					{
						amplitude = (2 * maxu ) - amplitude;
						folded = 1;
					}
					if ( amplitude < 0 )
					{
						amplitude = - amplitude;
						folded = 1;
					}
				} while (folded);

				wavetable[i].wave[j] = amplitude - maxi;

				/* Square wave is simply the sign of a triangle wave
				*/
				if ( wav == SQU )
				{
					wavetable[i].wave[j] = (wavetable[i].wave[j] < 0) ? -maxi : maxi;
				}
			}
			else if ( wav == SIN )
			{
				/* To do */
			}
		}
	}
}

/* wave_start_mono() - initialise a tone generator for a monophonic waveform
*/
void wave_start_mono(int wgen, int note, dv_i32_t harmonic)
{
	if ( wgen < 0 || wgen > N_TONEGEN || note < 0 || note >= 12 || harmonic < 0 )
		return;					/* Error */

	tonegen[wgen].position = -harmonic;
	tonegen[wgen].harmonic = harmonic;
	tonegen[wgen].root = &wavetable[note];
}

/* wave_start_mono() - initialise a tone generator for a monophonic waveform
*/
void wave_stop_mono(int wgen)
{
	if ( wgen < 0 || wgen > N_TONEGEN )
		return;					/* Error */

	tonegen[wgen].root = DV_NULL;
}

/* wave_play_simple() - play a single, monophonic waveform from a wavetable
 *
 * Returns the next sample in the waveform.
*/
dv_i32_t wave_play_mono(int wgen)
{
	if ( wgen < 0 || wgen > N_TONEGEN )
		return 0;				/* Error */

	if ( tonegen[wgen].root == DV_NULL )
		return 0;				/* Tone is OFF */

	tonegen[wgen].position += tonegen[wgen].harmonic;
	tonegen[wgen].position %= tonegen[wgen].root->nsamp;

	return tonegen[wgen].root->wave[tonegen[wgen].position];
}
