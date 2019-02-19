/*	synth-stdio.c - printf using ringbuffers
 *
 *	Copyright 2019 David Haworth
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
#include <dv-ringbuf.h>
#include <dv-xstdio.h>
#include <dv-stdio.h>
#include <synth-stdio.h>
#include <effect.h>

struct charbuf_s charbuf[N_CHARBUF];

/* charbuf_init() - initialise the character buffers
*/
void charbuf_init(void)
{
	for ( int i = 0; i < N_CHARBUF; i++ )
	{
		charbuf[i].rbm.head = 0;
		charbuf[i].rbm.tail = 0;
		charbuf[i].rbm.length = CHARBUF_LEN;
	}
}

/* charbuf_putc_X() - write a character to charbuf[X]
*/
int charbuf_putc_0(int c)
{
	return charbuf_putc(0, c);
}

int charbuf_putc_1(int c)
{
	return charbuf_putc(1, c);
}

int charbuf_putc_2(int c)
{
	return charbuf_putc(2, c);
}

int charbuf_putc_3(int c)
{
	return charbuf_putc(3, c);
}

/* sy_printf() - a printf function that should work from anywhere in syntheffect
 *
 * If the cores haven't reached synchronisation yet, the function is identical to dv_printf()
 * If the cores are synchronised, the characters are sent to the core's charbuf
*/
int sy_printf(const char *fmt, ...)
{
	int nprinted;
	va_list ap;

	dv_intstatus_t is = dv_disable();

	va_start(ap, fmt);
	if ( effect_sync )
	{
		switch ( dv_get_coreidx() )
		{
		case 0:
			nprinted = dv_xprintf(&charbuf_putc_0, fmt, ap);
			break;
		case 1:
			nprinted = dv_xprintf(&charbuf_putc_1, fmt, ap);
			break;
		case 2:
			nprinted = dv_xprintf(&charbuf_putc_2, fmt, ap);
			break;
		case 3:
			nprinted = dv_xprintf(&charbuf_putc_3, fmt, ap);
			break;
		default:
			nprinted = 0;
			break;
		}
	}
	else
	{
		nprinted = dv_xprintf(&dv_putc, fmt, ap);
	}
	va_end(ap);

	dv_restore(is);

	return nprinted;
}

/* charbuf_scan() - scan all the "printf" charbufs for characters to sent to the console
 *
 * If already accepting characters from a charbuf, stay with it until a control character is seen.
 * Otherwise, scan for a non-empty charbuf and switch to it unless the character is a control character.
*/
static int scan_cb = -1;
static int current_cb = -1;

void charbuf_scan(void)
{
	if ( dv_consoledriver.istx() )
	{
		if ( current_cb < 0 )
		{
			for ( int i = 0; i < 4; i++ )
			{
				scan_cb++;
				if ( scan_cb > 3 )	scan_cb = 0;

				int c = charbuf_getc(scan_cb);

				if ( c >= 0 )
				{
					if ( c != 0 )			/* Don't send NUL */
						dv_putc(c);
					if ( c >= 0x20 )		/* Stay on this charbuf if not a control character */
						current_cb = scan_cb;
					break;					/* Break out of the 0..3 loop */
				}
			}
		}
		else
		{
			int c = charbuf_getc(current_cb);

			if ( c >= 0 )
			{
				if ( c != 0 )				/* Don't send NUL */
					dv_putc(c);
				if ( c < 0x20 )				/* Switch to next charbuf on a control character */
					current_cb = -1;
			}
		}
	}
}
