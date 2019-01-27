/* effect.h - interface for an effect generator
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
#ifndef EFFECT_H
#define EFFECT_H	1

#include <kernel/h/dv-kconfig.h>
#include <kernel/h/dv-types.h>

/* The effects generator consists of a sequence of 2 or more single digital
 * transformations of an input signal.
 *
 * The first transformation in the sequence provides the input signal as a sample.
 * The final transformation outputs the final sample to whatever output medium is used.
 * Thus the simplest case (2 stages) does no processing.
 *
 * Each transformation stage is a function that takes a configuration block and a sample as input,
 * and returns a transformed sample.
 * A generator stage ignores the input sample and generates an output sample based on control input
 * (e.g. key press/release events)
 * 
 * In the case of generation, it's possible that the process is recursive.
 *
 * The whole is managed using an effect_s structure that contains the head of the
 * list of transformations and some global configuration.
*/
struct effect_s;	/* Forward */
typedef dv_i64_t (*effectstage_t)(struct effect_s *, dv_i64_t);

struct effect_s
{
	struct effect_s *next;
	struct effect_s *prev;
	effectstage_t func;
	void *control;
	char *name;
};

extern struct effect_s effect_list;

extern void effect_init(void);
extern dv_i64_t effect_chain(struct effect_s *e, dv_i64_t signal);
extern void effect_processor(void);
extern void effect_append(struct effect_s *e);

#endif
