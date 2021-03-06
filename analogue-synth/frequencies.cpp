/* frequencies.cpp - standard note frequencies (A=440)
 *
 * (c) David Haworth
 *
 *	This file is part of analogue-synth.
 *
 *	analogue-synth is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, either version 3 of the License, or
 *	(at your option) any later version.
 *
 *	analogue-synth is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with analogue-synth.  If not, see <http://www.gnu.org/licenses/>.
*/
#include <Arduino.h>
#include "analogue-synth.h"

const double PROGMEM standard_freq[] =
{	8.176,			//	0	C-1
	8.662,			//	1	C#-1
	9.177,			//	2	D-1
	9.723,			//	3	D#-1
	10.301,			//	4	E-1
	10.913,			//	5	F-1
	11.562,			//	6	F#-1
	12.250,			//	7	G-1
	12.978,			//	8	G#-1
	13.750,			//	9	A-1
	14.568,			//	10	A#-1
	15.434,			//	11	B-1
	16.352,			//	12	C0
	17.324,			//	13	C#0
	18.354,			//	14	D0
	19.445,			//	15	D#0
	20.602,			//	16	E0
	21.827,			//	17	F0
	23.125,			//	18	F#0
	24.500,			//	19	G0
	25.957,			//	20	G#0
	27.500,			//	21	A0
	29.135,			//	22	A#0
	30.868,			//	23	B0
	32.703,			//	24	C1
	34.648,			//	25	C#1
	36.708,			//	26	D1
	38.891,			//	27	D#1
	41.203,			//	28	E1
	43.654,			//	29	F1
	46.249,			//	30	F#1
	48.999,			//	31	G1
	51.913,			//	32	G#1
	55.000,			//	33	A1
	58.270,			//	34	A#1
	61.735,			//	35	B1
	65.406,			//	36	C2
	69.296,			//	37	C#2
	73.416,			//	38	D2
	77.782,			//	39	D#2
	82.407,			//	40	E2
	87.307,			//	41	F2
	92.499,			//	42	F#2
	97.999,			//	43	G2
	103.826,		//	44	G#2
	110.000,		//	45	A2
	116.541,		//	46	A#2
	123.471,		//	47	B2
	130.813,		//	48	C3
	138.591,		//	49	C#3
	146.832,		//	50	D3
	155.563,		//	51	D#3
	164.814,		//	52	E3
	174.614,		//	53	F3
	184.997,		//	54	F#3
	195.998,		//	55	G3
	207.652,		//	56	G#3
	220.000,		//	57	A3
	233.082,		//	58	A#3
	246.942,		//	59	B3
	261.626,		//	60	C4 (Middle C)
	277.183,		//	61	C#4
	293.665,		//	62	D4
	311.127,		//	63	D#4
	329.628,		//	64	E4
	349.228,		//	65	F4
	369.994,		//	66	F#4
	391.995,		//	67	G4
	415.305,		//	68	G#4
	440.000,		//	69	A4
	466.164,		//	70	A#4
	493.883,		//	71	B4
	523.251,		//	72	C5
	554.365,		//	73	C#5
	587.330,		//	74	D5
	622.254,		//	75	D#5
	659.255,		//	76	E5
	698.456,		//	77	F5
	739.989,		//	78	F#5
	783.991,		//	79	G5
	830.609,		//	80	G#5
	880.000,		//	81	A5
	932.328,		//	82	A#5
	987.767,		//	83	B5
	1046.502,		//	84	C6
	1108.731,		//	85	C#6
	1174.659,		//	86	D6
	1244.508,		//	87	D#6
	1318.510,		//	88	E6
	1396.913,		//	89	F6
	1479.978,		//	90	F#6
	1567.982,		//	91	G6
	1661.219,		//	92	G#6
	1760.000,		//	93	A6
	1864.655,		//	94	A#6
	1975.533,		//	95	B6
	2093.005,		//	96	C7
	2217.461,		//	97	C#7
	2349.318,		//	98	D7
	2489.016,		//	99	D#7
	2637.020,		//	100	E7
	2793.826,		//	101	F7
	2959.955,		//	102	F#7
	3135.963,		//	103	G7
	3322.438,		//	104	G#7
	3520.000,		//	105	A7
	3729.310,		//	106	A#7
	3951.066,		//	107	B7
	4186.009,		//	108	C8
	4434.922,		//	109	C#8
	4698.636,		//	110	D8
	4978.032,		//	111	D#8
	5274.041,		//	112	E8
	5587.652,		//	113	F8
	5919.911,		//	114	F#8
	6271.927,		//	115	G8
	6644.875,		//	116	G#8
	7040.000,		//	117	A8
	7458.620,		//	118	A#8
	7902.133,		//	119	B8
	8372.018,		//	120	C9
	8869.844,		//	121	C#9
	9397.273,		//	122	D9
	9956.063,		//	123	D#9
	10548.080,		//	124	E9
	11175.300,		//	125	F9
	11839.820,		//	126	F#9
	12543.850		//	127	G9
};

double get_note_frequency(uint8_t note)
{
	double f;

	memcpy_P(&f, &standard_freq[note], sizeof(f));

	return f;
}
