/*	effect-notecontrol.h - header file for note-control
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
#ifndef EFFECT_NOTECONTROL_H
#define EFFECT_NOTECONTROL_H	1

#include <dv-config.h>
#include <davroska.h>
#include <effect.h>
#include <dv-ringbuf.h>

#define NOTE_START	0x10000
#define NOTE_STOP	0x00000

#define NQ_LEN	32

#define	N_CHANNELS	1

struct notequeue_s
{
	dv_u32_t channel;
	dv_rbm_t rbm;
	dv_u32_t buffer[NQ_LEN];
};

struct effect_notecontrol_s
{
	struct notequeue_s nq[N_CHANNELS];
};

extern struct effect_notecontrol_s effect_notecontrol;

extern dv_i64_t effect_notecontrol_scan(struct effect_s *e, dv_i64_t input);
extern void effect_notecontrol_init(struct effect_s *e);

static inline void synth_send_note(dv_u32_t ch, dv_u32_t note)
{
	for ( int i = 0; i < N_CHANNELS; i++ )
	{
		struct notequeue_s *nq = &effect_notecontrol.nq[i];
		if ( nq->channel == ch )
		{
			dv_rbm_t *rbm = &nq->rbm;
			if ( !dv_rb_full(rbm) )
			{
				dv_i32_t tail = rbm->tail;
				nq->buffer[tail] = note;
				__asm volatile("dsb sy");
				rbm->tail = dv_rb_add1(rbm, tail);
			}
			break;
		}
	}
}

#endif
