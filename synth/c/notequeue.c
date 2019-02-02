/*	notequeue.c - note queues for all synth channels
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

#include <notequeue.h>

struct notechannels_s notechannels;

/* notechannels_init() - intialise the note channels
*/
void notechannels_init(void)
{
	for ( int i = 0; i < N_NQ; i++ )
	{
		notechannels.nq[i].channel = i;
		notechannels.nq[i].rbm.head = 0;
		notechannels.nq[i].rbm.tail = 0;
		notechannels.nq[i].rbm.length = NQ_LEN;
	}
}
