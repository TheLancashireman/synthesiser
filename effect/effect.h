/* effect.h - interface for an effect generator
 *
 * (c) 2018 David Haworth
 *
 * $(NAME) is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * $(NAME) is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with $(NAME).  If not, see <http://www.gnu.org/licenses/>.
 *
 * $Id$
*/
#ifndef EFFECT_H
#define EFFECT_H	1

/* The effects generator consists of a sequence of 2 or more single digital
 * transformations. Each transformation takes a configuration block and a sample as input,
 * and returns a transformed sample.
 *
 * The first transformation in the sequence provides the input signal as a sample.
 * The final transformation outputs the final sample to whatever output medium is used.
 *
 * A single effect module provides a configuration structure that contains an effect_t structure.
 * The effect_cfg pointer in the effect_t gives the address of the containing function.
 *
 * The whole is managed using an effect_gen_t structure that contains the head of the
 * list of transformations and some global configuration.
*/

typedef struct effect_s effect_t;
typedef int (*effect_function_t)(effect_t *, int);

struct effect_s
{
	effect_t *next;
	effect_function_t func;
};

extern effect_t *first;
extern int samples_per_sec;

#endif
