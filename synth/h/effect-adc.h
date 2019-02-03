/*	effect-adc.h - header file for ADC input
 *
 *	Copyright 2018 David Haworth
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
#ifndef EFFECT_ADC_H
#define EFFECT_ADC_H	1

#include <dv-config.h>
#include <davroska.h>

#include <effect.h>

struct effect_adc_s
{
	dv_i32_t select;	/* non-zero ==> right, zero ==> left */
	dv_i32_t pace;
};

extern struct effect_adc_s effect_adc;

extern dv_i64_t effect_adc_input(struct effect_s *e, dv_i64_t signal);
extern void effect_adc_init(struct effect_s *e);

#endif
