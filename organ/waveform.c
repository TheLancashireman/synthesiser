/* waveform.c
 *
 * Simple waveform generator
*/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <organ.h>

const double sample_freq = 48000.0;
const double scale = 32768.0;
const double pi = 3.14159265;

void waveform(tg_sinewave_t *wave, double sine_freq)
{
	double angle_step = (2.0 * pi * sine_freq) / sample_freq;
	double angle = 0.0;
	double amplitude = 1.0;
	int nsamples = (int)( sample_freq / sine_freq + 0.5 );
	int sample = 0;

	fprintf(stderr, "Wavform for %fHz ; nsamples = %d\n", sine_freq, nsamples);

	/* Allocate space for the waveform.
	*/
	wave->wave = (tg_amplitude_t *)calloc(nsamples, sizeof(tg_amplitude_t));
	if ( wave->wave == NULL )
	{
		fprintf(stderr, "Failed to allocate memory for sinewave\n");
		exit(1);
	}
	wave->length = nsamples;

	while ( sample < nsamples )
	{
		amplitude = sin(angle);
		wave->wave[sample] = (tg_amplitude_t)(amplitude * scale);
#if 0
		fprintf(stderr, "%3d : %e : %e (%d)\n", sample, angle, amplitude, wave->wave[sample]);
#endif
		angle += angle_step;
		sample++;
	}
}
