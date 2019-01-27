/* synth-ctrl - interface for a synthesiser control panel
 *
 * (c) 2018 David Haworth
 *
 * synth-ctrl is free software: you can redistribute it and/or modify
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
 * synth-ctrl is an Arduino sketch, written for an Arduino Nano
*/

// Pins for data lines
#define data_out_0	10		// PD7		}
#define data_out_1	9		// PD6		} Data selectors for 74hc4067
#define data_out_2	8		// PD5		}
#define data_out_3	7		// PD4		}

#define n_analogue	2

#define midi		1		/* Set to 1 for midi, 0 for finding mapping */

#define n_ctrl		1		/* No. of controllers */
#define max_ctrl	32		/* Max. no of controllers (2 mux boards) */

#define max_age		100		/* 10 seconds - when nothing happening */
#define chg_age		20		/* 20 seconds - after a significant change */

#define threshold	2

/* ctrldef[] - an array of all possible controllers (given the hardware setup)
 *
 * Only the first n_ctrl are used.
 *
 * The order is not important, except that it is slightly more efficient to order the
 * controllers so that those with the same select pins and different analogue pins
 * come together.
*/

const struct ctrldef_s
{
	uint8_t ctrl_no;		/* MIDI controller number */
	uint8_t select;			/* Selector value (0..15) */
	uint8_t pin;			/* Analogue pin number (0..7) */
} ctrldef[max_ctrl] =
{
	{	0x00,	0x0,	A7	},
	{	0x01,	0x1,	A7	},
	{	0x02,	0x2,	A7	},
	{	0x03,	0x3,	A7	},
	{	0x04,	0x4,	A7	},
	{	0x05,	0x5,	A7	},
	{	0x06,	0x6,	A7	},
	{	0x07,	0x7,	A7	},
	{	0x08,	0x8,	A7	},
	{	0x09,	0x9,	A7	},
	{	0x0a,	0xa,	A7	},
	{	0x0b,	0xb,	A7	},
	{	0x0c,	0xc,	A7	},
	{	0x0d,	0xd,	A7	},
	{	0x0e,	0xe,	A7	},
	{	0x0f,	0xf,	A7	},
	{	0x10,	0x0,	A6	},
	{	0x11,	0x1,	A6	},
	{	0x12,	0x2,	A6	},
	{	0x13,	0x3,	A6	},
	{	0x14,	0x4,	A6	},
	{	0x15,	0x5,	A6	},
	{	0x16,	0x6,	A6	},
	{	0x17,	0x7,	A6	},
	{	0x18,	0x8,	A6	},
	{	0x19,	0x9,	A6	},
	{	0x1a,	0xa,	A6	},
	{	0x1b,	0xb,	A6	},
	{	0x1c,	0xc,	A6	},
	{	0x1d,	0xd,	A6	},
	{	0x1e,	0xe,	A6	},
	{	0x1f,	0xf,	A6	},
};

struct ctrlstate_s
{
	uint16_t value;			/* Last known value */
	int16_t age;			/* Counts down */
} ctrlstate[n_ctrl];

char midi_chan = 0;

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

	analogReference(DEFAULT);
	pinMode(A7, INPUT);
	pinMode(A6, INPUT);

	pinMode(data_out_0, OUTPUT);
	pinMode(data_out_1, OUTPUT);
	pinMode(data_out_2, OUTPUT);
	pinMode(data_out_3, OUTPUT);

	for ( i = 0; i < n_ctrl; i++ )
	{
		ctrlstate[i].value = 65535;
		ctrlstate[i].age = max_age;
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
	uint8_t sel = 255;		/* Impossible value forces a change the first time */
	unsigned long then = millis();

	for (;;)
	{
		do {
		} while ( (millis() - then) < 100 );

		then += 100;

		for ( int i = 0; i < n_ctrl; i++ )
		{
			if ( ctrldef[i].select != sel )
			{
				/* Change the selectors
				*/
				sel = ctrldef[i].select;
				digitalWrite(data_out_0, (sel & 0x01) == 0 ? LOW : HIGH);
				digitalWrite(data_out_1, (sel & 0x02) == 0 ? LOW : HIGH);
				digitalWrite(data_out_2, (sel & 0x04) == 0 ? LOW : HIGH);
				digitalWrite(data_out_3, (sel & 0x08) == 0 ? LOW : HIGH);

				/* Allow time for the selected ananlogue signals to settle
				*/
				delayMicroseconds(500);
			}

			uint16_t val = analogRead(ctrldef[i].pin)/8;
			ctrlstate[i].age--;

			uint8_t send = 0;

			if ( ctrlstate[i].age < 0 )
			{
				send = 1;
				ctrlstate[i].age = max_age;
			}
			else
			if ( ( val == 0 || val == 127 ) &&
				 ( val !=  ctrlstate[i].value ) )
			{
				send = 1;
				ctrlstate[i].age = max_age;
			}
			else
			{
				uint16_t diff = ( val < ctrlstate[i].value ) ?
									( ctrlstate[i].value - val ) :
									( val - ctrlstate[i].value );
				if ( diff >= threshold )
				{
					send = 1;
					ctrlstate[i].age = chg_age;
				}
			}

			if ( send )
			{
				ctrlstate[i].value = val;
#if midi
				midi_send(0xB0+midi_chan, ctrldef[i].ctrl_no, val);
#else
				Serial.print(i);
				Serial.print(' ');
				Serial.print(val);
				Serial.print('\n');
#endif
			}
		}
	}
}
