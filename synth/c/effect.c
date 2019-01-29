/* effect.c - control for an effect generator
 *
 * (c) 2018 David Haworth
 *
 * This file is part of SynthEffect.
 *
 * SynthEffect is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * SynthEffect is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with SynthEffect  If not, see <http://www.gnu.org/licenses/>.
*/
#include <kernel/h/dv-kconfig.h>
#include <kernel/h/dv-types.h>
#include <project/h/effect.h>

/* This control structure controls a single, sequential processing sequence
 * However, it should be possible to extend to a multiprocessing environment
 * by having a "first" pointer per core and having some inter-core
 * communication stages
*/
struct effect_s effect_list;

/* effect_init() - initialises the list of effect stages
*/
void effect_init(void)
{
	effect_list.next = DV_NULL;
	effect_list.prev = DV_NULL;
	effect_list.func = DV_NULL;
	effect_list.control = DV_NULL;
}

/* effect_chain() - processes an effect chain from start to end
*/
dv_i64_t effect_chain(struct effect_s *e, dv_i64_t signal)
{
	dv_i64_t x = signal;

	while ( e != DV_NULL )
	{
		x = e->func(e, x);
		e = e->next;
	}

	return x;
}

/* effect_processor() - processes the list of effects
 *
 * Note: the stage input/output variable (x) is not initialized each time round the loop.
 * This permits some kind of feedback if needed later if needed.
 *
 * However, it is likely that the first stage discards its input and the last stage produces zero output.
*/
void effect_processor(void)
{
	dv_i64_t x = 0;

	for (;;)
	{
		struct effect_s *e = effect_list.next;
		x = effect_chain(e, x);
	}
}

/* effect_append() - appends a new effect to the end of the list
*/
void effect_append(struct effect_s *e)
{
	e->next = DV_NULL;
	if ( effect_list.next == DV_NULL )
	{
		effect_list.next = e;
		effect_list.prev = e;
		e->prev = DV_NULL;
	}
	else
	{
		effect_list.prev->next = e;
		e->prev = effect_list.prev;
		effect_list.prev = e;
	}
}
