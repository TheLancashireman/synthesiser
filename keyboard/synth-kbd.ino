/* synth-kbd - interface for a synthesiser or organ keyboard
 *
 * (c) 2016 David Haworth
 *
 * synth-kbd is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * synth-kbd is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this software.  If not, see <http://www.gnu.org/licenses/>.
 *
 * $Id: synth-kbd.ino 3 2016-11-24 04:25:43Z dave $
 *
 * synth-kbd is an Arduino sketch, written for an Arduino Nano
 *
 * Pins d2 - d9 = 8-way input
 * Pins d10-d13, a0-a3 = 8-way output - each pin (driven low) scans a set of 8 keys
 *     ==> up to 64 keys (60 key keyboard plus 4 switches)
 * Alternatively, use pins d10-d12 to drive a 3-to-8-way demux chip (74138?)
 *     ==> could use d13 to select 1 of 2 demux chips to increase range
 * 
 * Can also use d2-d9 as data bus to a latch for driving 8 LEDs. Use another pin (or a demux line) to latch.
 * 
 * Use any remaining A pins for potentiometers (pitch bend etc.)
 * 
*/

// Pins for data lines
#define data_out_0	14		// PC0		}
#define data_out_1	15		// PC1		} Column selectors for scan --> A0..A2 of 74HCT138
#define data_out_2	16		// PC2		}

#define data_out_3	17		// PC3		}
#define data_out_4	18		// PC4		} Row selectors for scan --> S0..S2 of 74HCT354
#define data_out_5	19		// PC5		}

#define kscan_en	13		// PB5 - enables the Y output of the '354  (OE3)
#define data_in		12		// PB4 - data in (from Y output of the '354)

#define kbd_nscan	8		/* No. of scan lines */
#define kbd_nrtn	8		/* No. of return lines */
#define kbd_ctrl	7		/* No. of control lines */
#define kbd_nkeys	(kbd_nscan*kbd_nrtn)	/* Total number of keys */

#define midi		1		/* Set to 1 for midi, 0 for finding mapping */

const unsigned char dataline[kbd_ctrl] =
{	data_out_0, data_out_1, data_out_2, data_out_3, data_out_4, data_out_5, kscan_en };

const unsigned char scanline[3] =
{	data_out_0, data_out_1, data_out_2	};

const unsigned char rtnline[3] =
{	data_out_3, data_out_4, data_out_5	};

struct keystate_s
{
	unsigned char state;
	unsigned char count;
};

keystate_s keystate[kbd_nkeys];

#include "../arduino-common/midi-thru.h"

/* Keymap.
 * Numbers in the range 0..127 are midi notes.
 * Numbers in the range 0x80-0xfe are switches.
 * 0xff means do nothing.
 * The columns (x0..x7) and rows (Ax..Hx) of this table are the raw alphanumeric scan codes in the test version.
*/
const unsigned char keymap[kbd_nkeys] =
{//	x0		x1		x2		x3		x4		x5		x6		x7
	21,		22,		23,		24,		17,		18,		19,		20,		// Ax
	13,		14,		15,		16,		9,		10,		11,		12,		// Bx
	5,		6,		7,		8,		0xff,	0xff,	0xff,	0xff,	// Cx
	29,		30,		31,		32,		25,		26,		27,		28,		// Dx
	37,		38,		39,		40,		33,		34,		35,		36,		// Ex
	45,		46,		47,		48,		41,		42,		43,		44,		// Fx
	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	// Gx
	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	// Hx
};

char midi_chan = 0;
char midi_offset = 36;

void midi_send(char p0, char p1, char p2)
{
	Serial.print(p0);
	Serial.print(p1);
	Serial.print(p2);
}

/* setup() - standard Arduino "Init Task"
*/
void setup(void)
{
	int i;

	pinMode(data_in, INPUT_PULLUP);		// Data input pin

	for ( i = 0; i < kbd_ctrl; i++ )	// All output pins
	{
		pinMode(dataline[i], OUTPUT);
		digitalWrite(dataline[i], LOW);
	}

	for ( i = 0; i < 64; i++ )
	{
		keystate[i].state = 0;
		keystate[i].count = 0;
	}

	Serial.begin(115200);					// Start the serial port.
#if !midi
	Serial.println("Hello world");
#endif
}

/* loop() - standard Arduino "Background Task"
*/
void loop(void)
{
	unsigned long then;

	then = millis();

	for (;;)
	{
		do {
			midi_thru();
		} while ( (millis() - then) < 10 );

		then += 10;

#if 1
		{
			unsigned scan, rtn, npress = 0;

			digitalWrite(kscan_en, HIGH);		// Enable selected scan data onto input pin

			for ( scan = 0; scan < kbd_nscan; scan++ )
			{
				digitalWrite(scanline[0], (scan & 0x01) == 0 ? LOW : HIGH);
				digitalWrite(scanline[1], (scan & 0x02) == 0 ? LOW : HIGH);
				digitalWrite(scanline[2], (scan & 0x04) == 0 ? LOW : HIGH);

				for ( rtn = 0; rtn < kbd_nrtn; rtn++ )
				{
					digitalWrite(rtnline[0], (rtn & 0x01) == 0 ? LOW : HIGH);
					digitalWrite(rtnline[1], (rtn & 0x02) == 0 ? LOW : HIGH);
					digitalWrite(rtnline[2], (rtn & 0x04) == 0 ? LOW : HIGH);

					unsigned idx = scan*8 + rtn;
					int k;

					if ( keystate[idx].count > 0 )
						keystate[idx].count--;			// Ignore keys whose count is non-zero
					else
					if ( digitalRead(data_in) == LOW )
					{
						// Key down
						if ( keystate[idx].state == 0 )
						{
							// Pressed
							keystate[idx].state = 1;
							keystate[idx].count = 2;		// 20 ms

#if midi
							if ( (k = keymap[idx]) <= 127 )
							{
								k += midi_offset;
								if ( k >= 0 && k <= 127 )
								{
									midi_send(0x90+midi_chan, k, 127);
								}
							}
#else
							npress++;
							char c = 'A'+scan;
							Serial.print(c);
							c = '0'+rtn;
							Serial.print(c);
							Serial.print(' ');
#endif
						}
					}
					else
					{
						// Key up
						if ( keystate[idx].state == 1 )
						{
							// Released
							keystate[idx].state = 0;
							keystate[idx].count = 2;		// 20 ms
#if midi
							if ( (k = keymap[idx]) <= 127 )
							{
								k += midi_offset;
								if ( k >= 0 && k <= 127 )
								{
									midi_send(0x80+midi_chan, k, 127);
								}
							}
#else
							npress++;
							char c = 'a'+scan;
							Serial.print(c);
							c = '0'+rtn;
							Serial.print(c);
							Serial.print(' ');
#endif
						}
					}
				}
			}

			digitalWrite(kscan_en, LOW);		// Disable selected scan data (high impedence)

			if ( npress > 0 )
				Serial.println('!');
		}
#endif
	}
}
