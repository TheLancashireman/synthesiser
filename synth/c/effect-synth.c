/*	effect-synth.c - synth note generator, written as an "effect"
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
#include <dv-config.h>
#include <davroska.h>
#include <synth-config.h>

#include <effect.h>
#include <effect-synth.h>
#include <adsr.h>
#include <wave.h>

struct adsr_s note_adsr;
struct effect_synth_s synth;
struct effect_synth_mono_s notegen[MAX_POLYPHONIC];

/* effect_synth() - sequence of note generators.
 *
 * The output of this note generator is added to the input signal and returned
 * In this way, multiple notes are combined.
 *
 * The number of simultaneous notes (up to MAX_POLYPHONIC) is controlled by the master program.
*/
dv_i64_t effect_synth(struct effect_s *e, dv_i64_t signal)
{
	struct effect_synth_s *synth = (struct effect_synth_s *)e->control;
	dv_i64_t my_signal = 0;

	for ( int i = 0; i < synth->n_polyphonic; i++ )
	{
		my_signal += synth_play_note(&notegen[i]);
	}

	return my_signal + signal;
}


/* effect_synth_init() - initialises the array of effect_synth_s structures
*/
void effect_synth_init(struct effect_s *e)
{
	e->func = &effect_synth;
	e->control = &synth;
	e->name = "synth";

	adsr_init(&note_adsr, 0, 0, ADSR_GMAX, 0, SAMPLES_PER_SEC);

	for ( int i = 0; i < MAX_POLYPHONIC; i++ )
	{
		notegen[i].vco.root = DV_NULL;
		notegen[i].envelope.adsr = &note_adsr;
		notegen[i].envelope.position = -1;
	}
}


/* synth_note() - generate the next sample of a single note
 *
 * This is where the work is really done.
 *
 *	- vco to generate the next sample of its configured waveform.
 *	- envelope generator is used to generate the gain to shape the note.
 *	- vca - the signal is multiplied by the envelope gain.
 *
 * ToDo:
 * 	- lfo to modulate the vco to produce vibrato effect (tricky) 
 *	- lfo to modulate the envelope input to the vca to give tremelo effect
 *	- vcf to filter the output - parameters depending on note played, etc.
 *	- lfo to modulate the vcf
 *	- another envelope generator to modulate the vcf (or to modulate the vcf's lfo)
 *	- etc etc.
*/
dv_i64_t synth_play_note(struct effect_synth_mono_s *notegen)
{
	if ( notegen->envelope.position < 0 )
		return 0;

	/* Do I care about overflow here? It happens after about 15 minutes.
	*/
	notegen->age++;

	/* Compute the ADSR gain.
	*/
	dv_i64_t gain = (dv_i64_t)envelope_gen(&notegen->envelope);

	/* Compute current raw waveform value.
	*/
	dv_i64_t sample = (dv_i64_t)tone_play(&notegen->vco);

	/* Signal is sample * gain.
	*/
	return (sample * gain) / 1000;
}


/* synth_start_note() - start playing a note
*/
void synth_start_note(struct effect_s *e, dv_i32_t midi_note)
{
	struct effect_synth_s *synth = (struct effect_synth_s *)e->control;
	struct effect_synth_mono_s *ng = synth_find_generator(synth, midi_note);

	ng->age = 0;
	ng->midi_note = midi_note;

	/* Midi note 0 is C @ 8.175 Hz (i.e. the C of our root table, index 3)
	*/
	tone_start(&ng->vco, (midi_note+3)%12, 1 << ((midi_note+3)/12));
	envelope_start(&ng->envelope);
}


/* synth_stop_note() - stop playing a note
 *
 * The note (if it's there) is set into its release phase.
 * The note doesn't actually stop playing until the end of its release phase.
*/
void synth_stop_note(struct effect_s *e, dv_i32_t midi_note)
{
	struct effect_synth_s *synth = (struct effect_synth_s *)e->control;

	for ( int i = 0; i < synth->n_polyphonic; i++ )
	{
		if ( notegen[i].midi_note == midi_note )
		{
			envelope_stop(&notegen[i].envelope);
			return;
		}
	}
}


/* synth_find_generator() - find the best note generator to use for a new note
 *
 * Criteria for "best":
 *		1. Not playing
 *		2. Playing same note
 *		3. Playing longest
*/
struct effect_synth_mono_s *synth_find_generator(struct effect_synth_s *s, dv_i32_t midi_note)
{
	struct effect_synth_mono_s *ng = &notegen[0];

	for ( int i = 0; i < s->n_polyphonic; i++ )
	{
		if ( notegen[i].envelope.position < 0 )
			return &notegen[i];						/* Return a free generator */

		if ( notegen[i].midi_note == midi_note )
			return &notegen[i];						/* Return a generator that's playing the same note */

		if ( notegen[i].age > ng->age ) 
		{
			ng = &notegen[i];						/* Remember an older generator */
		}
	}

	return ng;
}

/* synth_control() - control the synth with various parameters
 *
 * controllers 0 to 127 are midi controller values - see synth-config.h
 * controller 128 - number of polyphonic notes
*/
void synth_control(struct effect_s *e, dv_i32_t controller, dv_i32_t value)
{
	struct effect_synth_s *synth = (struct effect_synth_s *)e->control;

	switch ( controller )
	{
	case SYNTH_CTRL_ENVELOPE_A:
		adsr_set_a(&note_adsr, value, SAMPLES_PER_SEC);
		break;

	case SYNTH_CTRL_ENVELOPE_D:
		adsr_set_d(&note_adsr, value, SAMPLES_PER_SEC);
		break;

	case SYNTH_CTRL_ENVELOPE_S:
		adsr_set_s(&note_adsr, value, SAMPLES_PER_SEC);
		break;

	case SYNTH_CTRL_ENVELOPE_R:
		adsr_set_r(&note_adsr, value, SAMPLES_PER_SEC);
		break;

	case SYNTH_CTRL_N_POLY:
		if ( value > 0 && value <= MAX_POLYPHONIC )
			synth->n_polyphonic = value;
		break;

	default:
		break;
	}
}
