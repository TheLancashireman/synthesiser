/* organ.c
 *
 * (c) David Haworth
*/
#include <stdio.h>
#include <organ.h>

tg_sinewave_t sine[12];
tg_wavegen_t wavegen[N_TONEGENS];
tg_drawbar_t drawbar[N_DRAWBARS];
tg_key_t key[N_KEYS];

static void init_sine(void);
static void init_tonegen(void);
static void init_drawbar(void);
static void init_keys(void);

static void play_note(tg_index_t length);
static void press_key(tg_index_t key);
static void release_key(tg_index_t key);

int main(int argc, char **argv)
{
	init_sine();
	init_tonegen();
	init_drawbar();
	init_keys();

	printf("; Sample Rate 48000\n");
	printf("; Channels 1\n");

	/* Set the drawbars.
	*/
	drawbar[2].level = 0;			/* Fundamental */
	drawbar[3].level = 0;			/* 2nd harmonic */
	drawbar[4].level = 0;			/* 3rd harmonic */
	drawbar[5].level = 0;			/* 4th harmonic */
	drawbar[6].level = 0;			/* 5th harmonic */
	drawbar[7].level = 0;			/* 6th harmonic */
	drawbar[8].level = 1;			/* 7th harmonic */
	drawbar[9].level = 0;			/* 8th harmonic */

	press_key(12);
	play_note(48000);
	press_key(16);
	play_note(48000);
	press_key(19);
	play_note(48000);
	press_key(24);
	play_note(48000*2);
	press_key(36);
	press_key(40);
	press_key(43);
	press_key(48);
	play_note(48000*4);

#if 0
	/* Set the "stops"
	*/
	wavegen[24].multiplier = 1;		/* Pure sine wave */
	wavegen[36].multiplier = 1;		/* 2nd harmonic */
	wavegen[43].multiplier = 1;		/* 3rd harmonic */

	play_note(48000);

	wavegen[28].multiplier = 1;		/* Pure sine wave */
	wavegen[40].multiplier = 1;		/* 2nd harmonic */
	wavegen[47].multiplier = 1;		/* 3rd harmonic */

	play_note(48000);

	wavegen[31].multiplier = 1;		/* Pure sine wave */
	wavegen[43].multiplier = 5;		/* 2nd harmonic - adds 1/2 to existing 1/3 */
	wavegen[50].multiplier = 1;		/* 3rd harmonic */

	play_note(48000);

	wavegen[36].multiplier = 3;		/* Pure sine wave - adds 1 to existing 1/2 */
	wavegen[48].multiplier = 1;		/* 2nd harmonic */
	wavegen[55].multiplier = 1;		/* 3rd harmonic */

	play_note(48000*2);

	wavegen[48].multiplier = 1;		/* Pure sine wave */
	wavegen[60].multiplier = 1;		/* 2nd harmonic */
	wavegen[67].multiplier = 1;		/* 3rd harmonic */

	wavegen[52].multiplier = 1;		/* Pure sine wave */
	wavegen[64].multiplier = 1;		/* 2nd harmonic */
	wavegen[71].multiplier = 1;		/* 3rd harmonic */

	wavegen[55].multiplier = 1;		/* Pure sine wave */
	wavegen[67].multiplier = 5;		/* 2nd harmonic - adds 1/2 to existing 1/3 */
	wavegen[74].multiplier = 1;		/* 3rd harmonic */

	wavegen[60].multiplier = 3;		/* Pure sine wave - adds 1 to existing 1/2 */
	wavegen[72].multiplier = 1;		/* 2nd harmonic */
	wavegen[79].multiplier = 1;		/* 3rd harmonic */

	play_note(48000*4);

#endif

	return 0;
}

static void init_sine(void)
{
	/* Generate the sine waveforms for the lowest octave.
	*/
	waveform(&sine[0], 27.5);			/* A	*/
	waveform(&sine[1], 29.1352);		/* Bb	*/
	waveform(&sine[2], 30.8677);		/* B	*/
	waveform(&sine[3], 32.7032);		/* C	*/
	waveform(&sine[4], 34.6478);		/* C#	*/
	waveform(&sine[5], 36.7081);		/* D	*/
	waveform(&sine[6], 38.8909);		/* Eb	*/
	waveform(&sine[7], 41.2034);		/* E	*/
	waveform(&sine[8], 43.6535);		/* F	*/
	waveform(&sine[9], 46.2493);		/* Gb	*/
	waveform(&sine[10], 48.9994);		/* G	*/
	waveform(&sine[11], 51.9131);		/* Ab	*/
}

/* init_tonegen() - initialise all the tone generators.
 *
 * This creates all the fundamental frequencies.
 *
 * Tone generation assumes that harmonics that are not a power of 2 (e.g. 3, 5, etc.) can be approximated
 * using a power-of-2 harmonic of some other note.
*/
static void init_tonegen(void)
{
	tg_index_t oct, note, harmonic;
	tg_wavegen_t *wg = &wavegen[0];

	harmonic = 1;

	for ( oct = 0; oct < N_OCTAVES; oct++ )
	{
		for ( note = 0; note < 12; note++ )
		{
			wg->sinewave = &sine[note];
			wg->harmonic = harmonic;
			wg->index = 0;
			wg->multiplier = 0;

			wg++;
		}

		harmonic = harmonic * 2;
	}
}

/* init_drawbar() - initialise all the drawbars
 *
 * The levels are set to 0 (off) and the offsets to a large number which always
 * results in an out-of-range tone generator index. If the tone generator index is out of range it
 * is ignored.
 *
 * Finally, the drawbars that are in use have their offsets configured.
*/
static void init_drawbar(void)
{
	tg_index_t i;

	for ( i = 0; i < N_DRAWBARS; i++ )
	{
		drawbar[i].level = 0;			/* Off */
		drawbar[i].offset = 999;		/* Out of range */
	}

	/* The sub-harmonics aren't used yet.
	*/

	drawbar[2].offset = 0;			/* Fundamental */
	drawbar[3].offset = 12;			/* 2nd harmonic */
	drawbar[4].offset = 19;			/* 3rd harmonic */
	drawbar[5].offset = 24;			/* 4th harmonic */
	drawbar[6].offset = 28;			/* 5th harmonic */
	drawbar[7].offset = 31;			/* 6th harmonic */
	drawbar[8].offset = 34;			/* 7th harmonic */
	drawbar[9].offset = 36;			/* 8th harmonic */
}

static void init_keys(void)
{
	tg_index_t i;
	tg_index_t f = 12;

	for ( i = 0; i < N_KEYS; i++ )
	{
		key[i].fundamental = f;
		key[i].is_pressed = 0;

		f++;
	}
}

static void press_key(tg_index_t k)
{
	tg_index_t f, tg, db;

	/* Ignore out-of-range key index.
	*/
	if ( k < 0 || k >= N_KEYS )
		return;

	/* Ignore if key is already pressed.
	*/
	if ( key[k].is_pressed )
		return;

	/* Set key state.
	*/
	key[k].is_pressed = 1;

	/* Add the key's contribution to all of its tone generators.
	*/
	f = key[k].fundamental;

	for ( db = 0; db < N_DRAWBARS; db++ )
	{
		tg = f + drawbar[db].offset;

		if ( tg >= 0 && tg < N_TONEGENS )
		{
			wavegen[tg].multiplier += drawbar[db].level;
		}
	}
}

static void release_key(tg_index_t k)
{
	tg_index_t f, tg, db;

	/* Ignore out-of-range key index.
	*/
	if ( k < 0 || k >= N_KEYS )
		return;

	/* Ignore if key is already pressed.
	*/
	if ( !key[k].is_pressed )
		return;

	/* Set key state.
	*/
	key[k].is_pressed = 0;

	/* Add the key's contribution to all of its tone generators.
	*/
	f = key[k].fundamental;

	for ( db = 0; db < N_DRAWBARS; db++ )
	{
		tg = f + drawbar[db].offset;

		if ( tg >= 0 && tg < N_TONEGENS )
		{
			wavegen[tg].multiplier -= drawbar[db].level;
			if ( wavegen[tg].multiplier < 0 )		/* Paranoia */
			{
				wavegen[tg].multiplier = 0;
			}
		}
	}
}

/* play_note() - plays the current combination of notes for a give time (temporary)
*/
static void play_note(tg_index_t length)
{
	tg_index_t i;
	tg_amplitude_t val;

	for ( i = 0; i < length; i++ )
	{
		val = next_sample();
		printf("%f  %f\n", ((double)i)/48000.0, ((double)val)/((double)(32768*64)));
	}
}
