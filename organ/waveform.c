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

/* Calculate the number of samples in a single cycle of a given frequency.
*/
int wavelen(double sine_freq)
{
	return (int)( sample_freq / sine_freq + 0.5 );
}

/* Calculate a sine wave of the given frequency
*/
int waveform(tg_sinewave_t *wave, double sine_freq)
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
		return -1;
	}
	return 0;
}

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
	tg_sinewave_t wave;
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
	return 0;
}
#endif
