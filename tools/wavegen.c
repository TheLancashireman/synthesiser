/*	wavegen.c - wavetable generator
 *
 *	Copyright 2018 David Haworth
 *
 *	This file is part of $(NAME).
 *
 *	$(NAME) is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, either version 3 of the License, or
 *	(at your option) any later version.
 *
 *	$(NAME) is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with $(NAME).  If not, see <http://www.gnu.org/licenses/>.
*/
#include <stdio.h>

/* Note frequencies from C(-1) to B(-1)
*/
const long double note_freq[12] =
{	8.1758007813,
	8.661953125,
	9.1770117188,
	9.7227148438,
	10.300859375,
	10.9133789063,
	11.5623242188,
	12.2498632813,
	12.97828125,
	13.75,
	14.5676171875,
	15.4338476563
};

const char *note_name[12] =
{	"C",
	"C#",
	"D",
	"D#",
	"E",
	"F",
	"F#",
	"G",
	"G#",
	"A",
	"A#",
	"B"
};

const long double sample_rate = 48000.0;		/* 48 ksps */

void print_n_samples(void);

int main(int argc, char **argv)
{
	print_n_samples();

	return 0;
}

/* First question: how many samples for a single cycle of each note in the (-1) octave?
 *
 * Equation: n_samples = (samples/sec) * (sec/cycle)
 *                     = (samples/sec) / (cycles/sec)
 *                     = samnple_rate / freq_in_Hz
*/
void print_n_samples(void)
{
	int i;
	int total = 0;
	int total_1 = 0;
	int total_2 = 0;
	for ( i = 0; i < 12; i++ )
	{
		long double n_samp = sample_rate/note_freq[i];
		int n_samp_round = (int)(n_samp + 0.5);
		int n_samp_adj1;
		int n_samp_adj2;

		long double error = n_samp - (long double)n_samp_round;
		if ( error < 0.0 ) error = -error;
		if ( error > 0.33333333L )
			n_samp_adj1 = (int)((n_samp * 2.0) + 0.5);
		else
			n_samp_adj1 = n_samp;

		n_samp_adj2 = (int)(n_samp * 100.0 + 0.5);

		total += n_samp_round;
		total_1 += n_samp_adj1;
		total_2 += n_samp_adj2;

		printf("%2s   %Lf  %Lf  %d  %d  %d\n", note_name[i], n_samp, error, n_samp_round, n_samp_adj1, n_samp_adj2);
	}
	printf("Total space: %d samples (%d bytes)  %d  %d\n", total, total*4, total_1, total_2);
}
