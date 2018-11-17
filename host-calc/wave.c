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
					long amplitude = (maxu * j) / wavetable[i].nsamp;
					if ( amplitude > maxu )
						amplitude -= maxu;
					wavetable[i].wave[j] = amplitude - maxi;
				}
				else if ( wav == TRI )
				{
					long amplitude = (maxu * 2 * j) / wavetable[i].nsamp;
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
				}
				else if ( wav == SQU )
				{
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
