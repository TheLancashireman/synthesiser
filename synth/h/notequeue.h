/*	notequeue.h - header file for the note queues
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
#ifndef NOTEQUEUES_H
#define NOTEQUEUES_H	1

#include <dv-config.h>
#include <davroska.h>
#include <dv-ringbuf.h>

/* There's one queue for every synth channel that's supported (currently just one).
 *
 * The channel index is not necessarily the midi channel number.
 *
 * Each synthesiser reads the note messages from its own channel.
*/
#define NQ_LEN	32

#define NQ_SYNTH	0
#define	N_NQ		1

/* Each note message is 32 bits. These macros differentiate between start and stop.
*/
#define NOTE_START	0x10000
#define NOTE_STOP	0x00000

struct notequeue_s
{
	dv_u32_t channel;
	dv_rbm_t rbm;
	dv_u32_t buffer[NQ_LEN];
};

struct notechannels_s
{
	struct notequeue_s nq[N_NQ];
};

extern struct notechannels_s notechannels;

extern void notechannels_init(void);

/* Push a note message into a queue.
*/
static inline void send_note(dv_u32_t ch, dv_u32_t note)
{
	/* First find the note queue that's handling the note's MIDI channel.
	*/
	for ( int i = 0; i < N_NQ; i++ )
	{
		struct notequeue_s *nq = &notechannels.nq[i];
		if ( nq->channel == ch )
		{
			/* Found it! Push the note into the queue (unless the queue is full)
			*/
			dv_rbm_t *rbm = &nq->rbm;
			if ( !dv_rb_full(rbm) )
			{
				dv_i32_t tail = rbm->tail;
				nq->buffer[tail] = note;
				tail = dv_rb_add1(rbm, tail);
				__asm volatile("dsb sy");
				rbm->tail = tail;
			}
			break;
		}
	}
}

#endif
