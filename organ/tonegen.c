/* tonegen.c
 *
 * (c) David Haworth
 *
 * License: GPL v 3.0
*/
#include <organ.h>

/* The drawbars control the tones that are generated here.
 * See http://www.hammond-organ.com/product_support/drawbar_overview.jpg
 *
 * drawbar[0] is the octave subharmonic (Bourdon, 16')
 * drawbar[1] is the fundamental        (Principal, 8')
 * drawbar[2] is the 5th subharmonic    (Quint, 5.33')
 * drawbar[3] is the 8th                (Octave, 4')
 * drawbar[4] is the 12th               (Nazard, 2.66')
 * drawbar[5] is the 15th               (Blockfloete, 2')
 * drawbar[6] is the 17th               (Tierce, 1.6')
 * drawbar[7] is the 19th               (Largot, 1.33')
 * drawbar[8] is the 22nd               (Siffloete, 1')
*/
tg_drawbar_t drawbar[N_DRAWBARS];
tg_wave_t tone[N_TONES];				/* Waveform for the tones */
tg_amplitude_t tone_buffer[BUFLEN];		/* Sample values for the tones */

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

int gen_tone(void)
{
	int i;			/* Tone index */
	int s = 0;		/* Sinewave index */
	int next = 0;	/* Index for allocating buffer */

	for ( i = 0; i < N_TONES; i++ )
	{
		if ( tone[i].wave == NULL )
		{
			if ( next >= BUFLEN )
			{
#if HOST_TEST
        		fprintf(stderr, "Sine wave buffer is too small. Have %d, need at least %d\n", BUFLEN, next);
#endif
				return -1;
			}
			tone[i].wave = &tone_buffer[next];
			tone[i].length = sine[i].length;
			next += tone[i].length;
		}

		tonegen(i, s);
		s++;
		if ( s >= 12 )
			s = 0;
	}
}
