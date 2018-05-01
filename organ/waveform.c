/* waveform.c
 *
 * Simple sine-wave generator.
 *
 * The function waveform() generates a sine wave represented as a sequence of 24-bit integers.
 * One cycle of the sine wave is generated. This means that there will be a bit of distortion
 * caused by the rounding of the number of samples in the wavelength. If this distortion is
 * audible, a solution might be to generate waveforms or two or more cycles to reduce the error.
 *
 * (c) David Haworth
 *
 * License: GPL 3.0
*/
#include <organ.h>
#include <math.h>

#ifndef HOST_TEST
#define HOST_TEST 0
#endif

#if HOST_TEST
#include <stdio.h>
#endif

const double sample_freq = 48000.0;		/* 48 kHz */
const double scale = 8388608.0;			/* 24 bits */
const double pi = 3.14159265;

/* Frequencies of the octave starting at C8. Source: http://pages.mtu.edu/~suits/notefreqs.html
*/
const double freq8[12] =
{
	4186.01,
	4434.92,
	4698.63,
	4978.03,
	5274.04,
	5587.65,
	5919.91,
	6271.93,
	6644.88,
	7040.00,
	7458.62,
	7902.13
};

const double c8_to_cx = 512.0;

#define BUFLEN	52310					/* Determined by running the program with a much larger buffer */
tg_wave_t sine[12];						/* The sine waves */
tg_amplitude_t sine_buffer[BUFLEN];		/* Sample values for the sine waves. */

/* Calculate the number of samples in a single cycle of a given frequency.
*/
int wavelen(double sine_freq)
{
	return (int)( sample_freq / sine_freq + 0.5 );
}

/* Calculate a sine wave of the given frequency
*/
int waveform(tg_wave_t *wave, double sine_freq)
{
	double angle, amplitude;
	int nsamples = wavelen(sine_freq);
	int i;

#if HOST_TEST
	fprintf(stderr, "Waveform for %fHz ; nsamples = %d\n", sine_freq, nsamples);
#endif

	if ( wave->length == nsamples )
	{
		for ( i = 0; i < nsamples; i++ )
		{
			angle = ((double)i * 2 * pi ) / (double)nsamples;
			amplitude = sin(angle);
			wave->wave[i] = (tg_amplitude_t)(amplitude * scale);
		}
	}
	else
	{
#if HOST_TEST
		fprintf(stderr, "wave->length is incorrect: %d\n", wave->length);
#endif
		return 1;
	}
	return 0;
}

int gen_sine(void)
{
	int i;
	int next = 0;
	int nerr = 0;
	double freq;

	for ( i = 0; i < 12; i++ )
	{
		if ( next >= BUFLEN )
		{
#if HOST_TEST
        	fprintf(stderr, "Sine wave buffer is too small. Have %d, need at least %d\n", BUFLEN, next);
#endif
			return -1;
		}
		freq = freq8[i] / c8_to_cx;			/* The lowest frequency is the 16' subharmonic of c0 */
		sine[i].wave = &sine_buffer[next];
		sine[i].length = wavelen(freq);
		next += sine[i].length;
		nerr += waveform(&sine[i], freq);
	}
#if HOST_TEST
		fprintf(stderr, "Octave generated: %d errors, %d samples\n", nerr, next);
#endif
	return nerr;
}

/* =================================
 * Below here is all HOST_TEST stuff
 * =================================
*/

#if HOST_TEST

#define N_TEST	3

double wfreq[N_TEST] =
{	4186.01,	/* C8 */
	2093.005,	/* C7 */
	1046.5025	/* C6 */
};

tg_amplitude_t buffer[6000];

int main(int argc, char **argv)
{
	tg_wave_t wave;
	int t;
	wave.wave = buffer;

	for ( t = 0; t < N_TEST; t++ )
	{
		wave.length = wavelen(wfreq[t]);

		if ( waveform(&wave, wfreq[t]) == 0 )
		{
			int i;
			for ( i = 0; i < wave.length; i++ )
			{
				fprintf(stderr, "%5d = %d\n", i, wave.wave[i]);
			}
		}
	}

	gen_sine();

	return 0;
}
#endif
