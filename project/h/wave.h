/*	wave.h - waveform generator
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
#ifndef wave_h
#define wave_h	1

#include <kernel/h/dv-kconfig.h>
#include <kernel/h/dv-types.h>

#define SAW	1
#define TRI	2
#define SQU	3
#define SIN 4

#define N_NOTEGEN	1

/* A wave table contains an integer multiple of whole waveforms of the note.
 * There will always be a small error because the sample rate is not a whole-number
 * multiple of the fundamental frequency. The number of cycles is chosen to keep the error
 * small enough.
 * The actual value of the wave is held in a wave buffer; the wave member is the base address.
*/
struct wavetable_s
{
	double f;
	dv_i32_t nsamp;
	dv_i32_t ncyc;
	dv_i32_t *wave;
};

/* A tone generator is used to generate a constant tone of of a given frequency, based on a root
 * wave table. The harmonic is used to increment the position (modulo nsamp of the wave) on each
 * sample. Thus a tone generator can output any harmonic of the give root wave.
*/
struct tonegen_s
{
	struct wavetable_s *root;
	dv_i32_t harmonic;
	dv_i32_t position;
};

/* An envelope generator shapes the amplitude of a note over time. The envelope has four stages;
 * attack, decay, sustain, release. The envelope starts when the note is played.
 *	- attack it the period during which the volume increases to the maximum
 *	- decay is the perion during which the volume decreases to the sustain level
 *	- sustain is the remainder of the tom the note is held (e.g. key remains pressed on keyboard)
 *	- release is the period during which the volume decreases to zero
 *
 * If the note ends before the sustain level is reached the attack/decay stages continue and the release
 * stage starts as soon as the sustain level is reached.
 *
 * Thus an envelope is defined by three time parameters (attack, decay, release) and one volume parameter
 * (sustain)
 *
 * This is a very simple envelope in which the rise and fall are linear. It may be possible to
 * construct curved attack/decay/release stages along with a gradual decay during the sustain phase
 * to generate more "realistic" sounds like plucked or struck instruments. Alternatively, for these
 * sounds, the sustain phase can be omitted, or the sustain level set to zero. This needs experimentation.
 *
 * All the envelope generators are controlled by a single envelope.
*/
struct envelope_s
{
	dv_i32_t attack;		/* No of samples during which volume increases from 0 to max */
	dv_i32_t decay;			/* No of samples during which volume decreases from max to sustain */
	dv_i32_t sustain;		/* Volume as long as key is held. */
	dv_i32_t release;		/* No of samples during which volume decreases from sustain to 0 */
};

struct envelopegen_s
{
	dv_i32_t position;		/* Increments once per sample until sustain is reached. */
};

/* A note generator consists of a tone generator and an envelope generator.
 * For a monophonic synthesiser there is a single note generator.
 * For full polyphonic operation, each note source (e.g. keyboard key) has its own note generator.
 * If managing a note generator for each key is not possible, a scheme where a free generator is used
 * for each keypress. If there is no free generator, the generator of the oldest note is stolen.
*/
struct notegen_s
{
	struct tonegen_s tone;
	struct envelopegen_s envelope;
};

extern int wave_init(void);
extern void wave_generate(int wav);
extern void wave_start_mono(int wgen, int note, dv_i32_t harmonic);
extern void wave_stop_mono(int wgen);
extern dv_i32_t wave_play_mono(int wgen);

#endif
