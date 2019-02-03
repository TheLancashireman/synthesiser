/*	synth-stdio.h - header file for the stdio ringbuffers
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
#ifndef SYNTH_STDIO_H
#define SYNTH_STDIO_H	1

#include <dv-config.h>
#include <davroska.h>
#include <dv-ringbuf.h>
#include <dv-xstdio.h>
#include <dv-stdio.h>

/* There's a character buffer per core for printf & co.
 * There's a character buffer for incoming non-midi data from the uart.
*/
#define CHARBUF_LEN		1024

#define N_CHARBUF		5
#define CHARBUF_STDIN	4

struct charbuf_s
{
	dv_rbm_t rbm;
	char buffer[CHARBUF_LEN];
};

extern struct charbuf_s charbuf[N_CHARBUF];

extern void charbuf_init(void);
extern int charbuf_putc_0(int c);
extern int charbuf_putc_1(int c);
extern int charbuf_putc_2(int c);
extern int charbuf_putc_3(int c);
extern int sy_printf(const char *fmt, ...);
extern void charbuf_scan(void);

/* charbuf_full() - return true if charbuf is full
*/
static inline dv_boolean_t charbuf_full(int cb)
{
	return dv_rb_full(&charbuf[cb].rbm);
}

/* charbuf_empty() - return true if charbuf is empty
*/
static inline dv_boolean_t charbuf_empty(int cb)
{
	return dv_rb_empty(&charbuf[cb].rbm);
}

/* charbuf_putc() - push a character into a charbuf
 *
 * If the buffer is full, drop the character
*/
static inline int charbuf_putc(int cb, int c)
{
	struct charbuf_s *chbuf = &charbuf[cb];
	dv_rbm_t *rbm = &chbuf->rbm;

	if ( dv_rb_full(rbm) )		/* Drop character if buffer is full */
		return 0;

	dv_i32_t tail = rbm->tail;
	chbuf->buffer[tail] = (char)c;
	tail = dv_rb_add1(rbm, tail);
	dv_barrier();
	rbm->tail = tail;
	dv_barrier();
	return 1;
}

/* charbuf_getc() - get a character from a charbuf and return it
 *
 * If the buffer is empty, return -1
*/
static inline int charbuf_getc(int cb)
{
	struct charbuf_s *chbuf = &charbuf[cb];
	dv_rbm_t *rbm = &chbuf->rbm;

	if ( dv_rb_empty(rbm) )			/* Return -1 if charbuf is empty */
		return -1;

	dv_i32_t head = rbm->head;
	int c = (int)chbuf->buffer[head];
	head = dv_rb_add1(rbm, head);
	dv_barrier();
	rbm->head = head;
	dv_barrier();
	return c;
}

#endif
