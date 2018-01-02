/* tonegen.c
 *
 * (c) David Haworth
*/
#include <organ.h>

/* NextSample() calculates the next sample of the output waveform
*/
tg_amplitude_t next_sample(void)
{
	tg_index_t i;
	tg_amplitude_t sample = 0;
	tg_wavegen_t *wg = &wavegen[0];

	for ( i = 0; i < (N_OCTAVES*12); i++ )
	{
		tg_sinewave_t *wave = wg->sinewave;
		tg_amplitude_t sineval = wave->wave[wg->index];

		sample += sineval * wg->multiplier;

		wg->index += wg->harmonic;
		if ( wg->index >= wave->length )
			wg->index -= wave->length;

		wg++;
	}

	return sample;
}
