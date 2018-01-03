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
 * along with DhG.  If not, see <http://www.gnu.org/licenses/>.
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

#define led1		13		// On-board LED connected to digital pin 13

// Pins for scan lines
#define scan_0		2		// PD2
#define scan_1		3		// PD3
#define scan_2		4		// PD4

// Pins for return lines
#define rtn_0		5		// PD5
#define rtn_1		6		// PD6
#define rtn_2		7		// PD7
#define rtn_3		8		// PB0
#define rtn_4		9		// PB1
#define rtn_5		10		// PB2
#define rtn_6		11		// PB3
#define rtn_7		12		// PB4

#define kbd_nscan	6		/* No. of scan lines */
#define kbd_nrtn	8		/* No. of return lines */

const unsigned char scanline[3] =
{	scan_0, scan_1, scan_2	};

const unsigned char rtnline[kbd_nrtn] =
{	rtn_0, rtn_1, rtn_2, rtn_3, rtn_4, rtn_5, rtn_6, rtn_7	};

struct keystate_s
{
	unsigned char state;
	unsigned char count;
};

keystate_s keystate[64];

/* setup() - standard Arduino "Init Task"
*/
void setup(void)
{
	int i;

	pinMode(led1, OUTPUT);				// Sets the digital pin as output
	digitalWrite(led1, LOW);			// Drive the pin low (LED off)

	for ( i = 0; i < 3; i++ )
	{
		pinMode(scanline[i], OUTPUT);
		digitalWrite(scanline[i], HIGH);
	}
	for ( i = 0; i < kbd_nrtn; i++ )
		pinMode(rtnline[i], INPUT_PULLUP);

	for ( i = 0; i < 64; i++ )
	{
		keystate[i].state = 0;
		keystate[i].count = 0;
	}

	Serial.begin(9600);					// Start the serial port.
	Serial.println("Hello world");
}

/* loop() - standard Arduino "Background Task"
*/
void loop(void)
{
	unsigned scan = 7;
	unsigned rtn = 7;
	int ledCount = 100;
	int ledState = 0;
	unsigned long then;

	then = millis();

	for (;;)
	{
		do {
		} while ( (millis() - then) < 10 );

		then += 10;

		ledCount--;
		if ( ledCount <= 0 )
		{
			if ( ledState )
			{
				digitalWrite(led1, LOW);
				ledState = 0;
				ledCount = 98;
			}
			else
			{
				digitalWrite(led1, HIGH);
				ledState = 1;
				ledCount = 2;
			}
		}

#if 0
		if ( Serial.read() >= 0 )
		{
			scan++;
			if ( scan > 7 )
				scan = 0;
			digitalWrite(scanline[0], (scan & 0x01) == 0 ? LOW : HIGH);
			digitalWrite(scanline[1], (scan & 0x02) == 0 ? LOW : HIGH);
			digitalWrite(scanline[2], (scan & 0x04) == 0 ? LOW : HIGH);
			Serial.print("Scan ");
			char c = '0'+scan;
			Serial.print(c);
			Serial.println("");
		}
#endif

#if 1
		{
			unsigned scan, rtn, npress = 0;

			for ( scan = 0; scan < kbd_nscan; scan++ )
			{
				digitalWrite(scanline[0], (scan & 0x01) == 0 ? LOW : HIGH);
				digitalWrite(scanline[1], (scan & 0x02) == 0 ? LOW : HIGH);
				digitalWrite(scanline[2], (scan & 0x04) == 0 ? LOW : HIGH);

				for ( rtn = 0; rtn < kbd_nrtn; rtn++ )
				{
					unsigned idx = scan*8 + rtn;

					if ( keystate[idx].count > 0 )
						keystate[idx].count--;			// Ignore keys whose count is non-zero
					else
					if ( digitalRead(rtnline[rtn]) == LOW )
					{
						// Key down
						if ( keystate[idx].state == 0 )
						{
							// Pressed
							keystate[idx].state = 1;
							keystate[idx].count = 2;		// 20 ms

							npress++;
							char c = 'A'+scan;
							Serial.print(c);
							c = '0'+rtn;
							Serial.print(c);
							Serial.print(' ');
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

							npress++;
							char c = 'a'+scan;
							Serial.print(c);
							c = '0'+rtn;
							Serial.print(c);
							Serial.print(' ');
						}
					}
				}
			}
			if ( npress > 0 )
				Serial.println('!');
		}
#endif
	}
}
