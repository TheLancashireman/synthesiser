/*	wave.c - waveform generator to run on host PC (for experimentation)
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
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

/* Include the wave generator but don't let it include davros files.
*/
#define dv_kconfig_h	1
#define dv_types_h		1

typedef int dv_i32_t;
typedef unsigned int dv_u32_t;
typedef long int dv_i64_t;
typedef unsigned long int dv_u64_t;
#define DV_NULL 0

#include "../project/c/wave.c"


const char *const wavename[] = { NULL, "sawtooth", "triangle", "square", "sine" };
const double sample_freq = 48000.0;

/* Note frequencies of the root wavetables.
 * These are sub-bass frequencies starting with the A that's two octaves below A0 (27.5 Hz).
 * I guess you'd call it A-2.
 *
 * Choosing frequencies this low might be a bit excessive.
*/
const double root_freq[] =
{
	6.875,
	7.28380877372013,
	7.71692658212691,
	8.17579891564368,
	8.66195721802722,
	9.17702399741896,
	9.722718241315,
	10.3008611535272,
	10.9133822322813,
	11.5623257097385,
	12.2498573744296,
	12.9782717993732
};

struct host_wavetable_s
{
	double f;
	int nsamp;
	int ncyc;
	int *wave;
};
struct host_wavetable_s host_wavetable[12];

int main(int argc, char** argv)
{
	int i, j;
	int total = 0;
	double f;
	double whole, frac;
	int nsamp;
	int ncyc;
	int wav = 0;
	int note = -1;

	printf("argc: %d\n", argc);
	if ( argc == 3 )
	{
		wav = atoi(argv[1]);
		note = atoi(argv[2]);

		if ( wav > 0 && wav < 5 && note >= 0 && note < 128 )
		{
			printf("Generating %s wave table for note %d\n", wavename[wav], note);
		}
		else
		{
			printf("Usage: wave [type(1-4) note(0-128)]\n");
			exit(1);
		}
	}

	/* Wavetable generation. First of all we need to know how many samples for each of the root frequencies.
	 *
	 * Work out the number of samples as a real number.
	 * Round to the nearest integer.
	 * To keep errors even smaller: if the fractional part is between 0.25 and 0.75, generate
	 * two cycles of the waveform. There's one frequency that has a fraction part of 0.26 but I hope
	 * the error won't result in any noticable off-key notes.
	 *
	 * The basic idea of tone generation is to play a harmonic x of the root note by sending every xth
	 * sample to the DAC. The trick being that when the output sample required is higher than the number
	 * of samples in the generated waveform, the generation subtracts the number of samples rather than
	 * starting again from zero. This means that the output samples reflect fairly accurately the
	 * true waveform and don't get quantized.
	*/
	for ( i=0; i<12; i++ )
	{
		f = sample_freq/root_freq[i];
		frac = modf(f, &whole);

		if ( frac < 0.25 )
		{
			nsamp = (int)whole;
			ncyc = 1;
		}
		else if ( frac < 0.75 )
		{
			nsamp = (int)(f*2.0) + 1;
			ncyc = 2;
		}
		else
		{
			nsamp = (int)whole + 1;
			ncyc = 1;
		}
		host_wavetable[i].f = f;
		host_wavetable[i].nsamp = nsamp;
		host_wavetable[i].ncyc = ncyc;
		total += nsamp;

		if ( wav == 0 )
			printf("% 2d - %f - %d (%d)\n", i, f, nsamp, ncyc);
	}

	if ( wav == 0 )
		printf("Total: %d\n", total);
	else
	{
		if ( wave_init() != 0 )
		{
			printf("wave_buffer is too small!\n");
			exit(1);
		}
		printf("prj_main: calling wave_generate(SAW).\n");
		wave_generate(wav);

		wave_start_mono(0, (note+3)%12, 1<<((note+3)/12));

		for ( i=0; i<8000; i++)
		{
			printf("%d,%d\n", i, wave_play_mono(0));
		}
	}
	return 0;
}
