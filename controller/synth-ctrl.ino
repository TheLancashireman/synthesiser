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
#define data_out_0	7		// PD4		}
#define data_out_1	8		// PD5		} Data selectors for 74hc4067
#define data_out_2	9		// PD6		}
#define data_out_3	10		// PD7		}

#define n_analogue	2

#define midi		1		/* Set to 1 for midi, 0 for finding mapping */

#define n_ctrl		1		/* No. of controllers */

#define max_age		100		/* 10 seconds - when nothing happening */
#define chg_age		20		/* 20 seconds - after a significant change */

#define threshold	2

const struct ctrldef_s
{
	uint8_t ctrl_no;		/* MIDI controller number */
	uint8_t select;			/* Selector value (0..15) */
	uint8_t pin;			/* Analogue pin number (0..7) */
} ctrldef[n_ctrl] =
{
	{	0x00,	0x0,	A7	},
#if 0
	{	0x01,	0x1,	A7	},
	{	0x02,	0x2,	A7	},
	{	0x03,	0x3,	A7	}
#endif
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
