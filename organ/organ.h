/* organ.h - types, prototypes etc.
 *
 * (c) David Haworth
*/
#ifndef ORGAN_H
#define ORGAN_H

#define N_KEYS		61

#define N_DRAWBARS	11
#define N_OCTAVES   8		/* No. of octaves in the tone generator */
#define N_TONEGENS	(N_OCTAVES*12)

#define	A0	0
#define	Bb0	1
#define	B0	2
#define	C0	3
#define	Cs0	4
#define	D0	5
#define	Eb0	6
#define	E0	7
#define	F0	8
#define	Fs0	9
#define	G0	10
#define	Ab0	11

#define A(n)	(A0+12*(n))
#define Bb(n)	(Bb0+12*(n))
#define B(n)	(B0+12*(n))
#define C(n)	(C0+12*(n))
#define Cs(n)	(Cs0+12*(n))
#define D(n)	(D0+12*(n))
#define Eb(n)	(Eb0+12*(n))
#define E(n)	(E0+12*(n))
#define F(n)	(F0+12*(n))
#define Fs(n)	(Fs0+12*(n))
#define G(n)	(G0+12*(n))
#define Ab(n)	(Ab0+12*(n))

typedef int tg_amplitude_t;
typedef int tg_index_t;
typedef int tg_level_t;
typedef int tg_boolean_t;

typedef struct tg_sinewave_s tg_sinewave_t;
typedef struct tg_wavegen_s tg_wavegen_t;
typedef struct tg_drawbar_s tg_drawbar_t;
typedef struct tg_key_s tg_key_t;

/* tg_key_s, tg_key_t - key representation and state
 *
 * For each key we need to know whic note it is and whether it is pressed or not.
 * There will be an array of these for each manual and one for the pedals.
*/
struct tg_key_s
{
	tg_index_t fundamental;
	tg_boolean_t is_pressed;
};

/* tg_drawbar_s, tg_drawbar_t - represents a drawbar
 *
 * Each drawbar controls a harmonic or sub-harmonic determined by the offset
 * The offset is how many to add to the fundamental generator to get the harmonic. Can be negative.
*/
struct tg_drawbar_s
{
	tg_index_t offset;		/* Offset from fundamental pitch - can be negative */
	tg_level_t level;		/* 0..8 (or more?) */
};

/* tg_sinewave_s, tg_sinewave_t - a sine wave. The space for the wave is alocated.
*/
struct tg_sinewave_s
{
	tg_amplitude_t *wave;
	tg_index_t length;
};

/* tg_wavegen_s, tg_wavegen_t - a structure used for generating composite waveforms.
*/
struct tg_wavegen_s
{
	tg_sinewave_t *sinewave;	/* The waveform for this sine wave */
	tg_index_t harmonic;		/* No. of samples to skip each time, for generating harmonics. */
	tg_index_t index;			/* Current position in the sinewave. */
	tg_level_t multiplier;		/* Gain */
};

extern tg_sinewave_t sine[12];
extern tg_wavegen_t wavegen[N_OCTAVES*12];

/* waveform() - calculate one complete waveform of a sinewave
 *
 * The frequency of the sinewave is passed as the second parameter. The sample rate is configured by SAMPLE_RATE.
*/
void waveform(tg_sinewave_t *wave, double sine_freq);

/* next_sample() - calculates the next output value
 *
 * This function calculates the next amplitude value of a complex waveform created by summing all the individual
 * sinewaves.
*/
tg_amplitude_t next_sample(void);

#endif
