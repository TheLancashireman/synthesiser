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

#define SAW	1
#define TRI	2
#define SQU	3
#define SIN 4

const char *const wavename[] = { NULL, "sawtooth", "triangle", "square", "sine" };
const double sample_freq = 48000.0;
const long maxi = 0x7fffffffL;
const long maxu = 0xffffffffL;

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

struct wavetable_s
{
	double f;
	int nsamp;
	int ncyc;
	int *wave;
};
struct wavetable_s wavetable[12];

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

		if ( wav > 0 && wav < 5 && note >= 0 && note < 12 )
		{
			printf("Generating %s wave table for note %d\n", wavename[wav], note);
		}
		else
		{
			printf("Usage: wave [type(1-4) note(0-11)]\n");
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
		wavetable[i].f = f;
		wavetable[i].nsamp = nsamp;
		wavetable[i].ncyc = ncyc;
		total += nsamp;

		if ( wav == 0 )
			printf("% 2d - %f - %d (%d)\n", i, f, nsamp, ncyc);

		wavetable[i].wave = calloc(nsamp, sizeof(int));

	}

	if ( wav == 0 )
		printf("Total: %d\n", total);
	else
	{
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
					if ( amplitude > maxu )
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
				}
			}
		}

		for (j = 0; j < wavetable[note].nsamp; j++ )
			printf("%d,%d,%d\n", note, j, wavetable[note].wave[j]);
	}
	return 0;
}
