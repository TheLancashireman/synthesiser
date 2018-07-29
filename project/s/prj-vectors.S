/*	prj-vectors.S - EL2/EL3 vector tables
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
#include <kernel/h/dv-asm.h>
#include <kernel/h/dv-kernel.h>
#include <cpufamily/arm64/h/dv-arm64-registers.h>

	.text
	.balign	2048
	.globl	prj_vectors
	.extern	prj_exc_handler

	.balign	128
prj_vectors:

	msr		TPIDR_EL0, x30
	mrs		x30, TPIDRRO_EL0
	ldr		x30, [x30, #DV_OFFSET_kvars_current_thread]
	ldr		x30, [x30, #DV_OFFSET_thr_regs]
	stp		x0, x1, [x30, #DV_OFFSET_regs_gpr0+0x00]
	stp		x2, x3, [x30, #DV_OFFSET_regs_gpr0+0x10]
	stp		x4, x5, [x30, #DV_OFFSET_regs_gpr0+0x20]
	stp		x6, x7, [x30, #DV_OFFSET_regs_gpr0+0x30]
	stp		x8, x9, [x30, #DV_OFFSET_regs_gpr0+0x40]
	stp		x10, x11, [x30, #DV_OFFSET_regs_gpr0+0x50]
	stp		x12, x13, [x30, #DV_OFFSET_regs_gpr0+0x60]
	stp		x14, x15, [x30, #DV_OFFSET_regs_gpr0+0x70]
	stp		x16, x17, [x30, #DV_OFFSET_regs_gpr0+0x80]
	stp		x18, x19, [x30, #DV_OFFSET_regs_gpr0+0x90]
	stp		x20, x21, [x30, #DV_OFFSET_regs_gpr0+0xa0]
	stp		x22, x23, [x30, #DV_OFFSET_regs_gpr0+0xb0]
	stp		x24, x25, [x30, #DV_OFFSET_regs_gpr0+0xc0]
	stp		x26, x27, [x30, #DV_OFFSET_regs_gpr0+0xd0]
	stp		x28, x29, [x30, #DV_OFFSET_regs_gpr0+0xe0]
	ldr		x1, _prj_exc_handler0
	b	prj_vector_common

	.balign	8
_prj_exc_handler0:
	.quad	prj_exc_handler

	.balign	128
	msr		TPIDR_EL0, x30
	mrs		x30, TPIDRRO_EL0
	ldr		x30, [x30, #DV_OFFSET_kvars_current_thread]
	ldr		x30, [x30, #DV_OFFSET_thr_regs]
	stp		x0, x1, [x30, #DV_OFFSET_regs_gpr0+0x00]
	stp		x2, x3, [x30, #DV_OFFSET_regs_gpr0+0x10]
	stp		x4, x5, [x30, #DV_OFFSET_regs_gpr0+0x20]
	stp		x6, x7, [x30, #DV_OFFSET_regs_gpr0+0x30]
	stp		x8, x9, [x30, #DV_OFFSET_regs_gpr0+0x40]
	stp		x10, x11, [x30, #DV_OFFSET_regs_gpr0+0x50]
	stp		x12, x13, [x30, #DV_OFFSET_regs_gpr0+0x60]
	stp		x14, x15, [x30, #DV_OFFSET_regs_gpr0+0x70]
	stp		x16, x17, [x30, #DV_OFFSET_regs_gpr0+0x80]
	stp		x18, x19, [x30, #DV_OFFSET_regs_gpr0+0x90]
	stp		x20, x21, [x30, #DV_OFFSET_regs_gpr0+0xa0]
	stp		x22, x23, [x30, #DV_OFFSET_regs_gpr0+0xb0]
	stp		x24, x25, [x30, #DV_OFFSET_regs_gpr0+0xc0]
	stp		x26, x27, [x30, #DV_OFFSET_regs_gpr0+0xd0]
	stp		x28, x29, [x30, #DV_OFFSET_regs_gpr0+0xe0]
	mov		x1, #0x01
	b	prj_vector_common

_prj_exc_handler1:
	.quad	prj_exc_handler

	.balign	128
	msr		TPIDR_EL0, x30
	mrs		x30, TPIDRRO_EL0
	ldr		x30, [x30, #DV_OFFSET_kvars_current_thread]
	ldr		x30, [x30, #DV_OFFSET_thr_regs]
	stp		x0, x1, [x30, #DV_OFFSET_regs_gpr0+0x00]
	stp		x2, x3, [x30, #DV_OFFSET_regs_gpr0+0x10]
	stp		x4, x5, [x30, #DV_OFFSET_regs_gpr0+0x20]
	stp		x6, x7, [x30, #DV_OFFSET_regs_gpr0+0x30]
	stp		x8, x9, [x30, #DV_OFFSET_regs_gpr0+0x40]
	stp		x10, x11, [x30, #DV_OFFSET_regs_gpr0+0x50]
	stp		x12, x13, [x30, #DV_OFFSET_regs_gpr0+0x60]
	stp		x14, x15, [x30, #DV_OFFSET_regs_gpr0+0x70]
	stp		x16, x17, [x30, #DV_OFFSET_regs_gpr0+0x80]
	stp		x18, x19, [x30, #DV_OFFSET_regs_gpr0+0x90]
	stp		x20, x21, [x30, #DV_OFFSET_regs_gpr0+0xa0]
	stp		x22, x23, [x30, #DV_OFFSET_regs_gpr0+0xb0]
	stp		x24, x25, [x30, #DV_OFFSET_regs_gpr0+0xc0]
	stp		x26, x27, [x30, #DV_OFFSET_regs_gpr0+0xd0]
	stp		x28, x29, [x30, #DV_OFFSET_regs_gpr0+0xe0]
	mov		x1, #0x02
	b	prj_vector_common

_prj_exc_handler2:
	.quad	prj_exc_handler

	.balign	128
	msr		TPIDR_EL0, x30
	mrs		x30, TPIDRRO_EL0
	ldr		x30, [x30, #DV_OFFSET_kvars_current_thread]
	ldr		x30, [x30, #DV_OFFSET_thr_regs]
	stp		x0, x1, [x30, #DV_OFFSET_regs_gpr0+0x00]
	stp		x2, x3, [x30, #DV_OFFSET_regs_gpr0+0x10]
	stp		x4, x5, [x30, #DV_OFFSET_regs_gpr0+0x20]
	stp		x6, x7, [x30, #DV_OFFSET_regs_gpr0+0x30]
	stp		x8, x9, [x30, #DV_OFFSET_regs_gpr0+0x40]
	stp		x10, x11, [x30, #DV_OFFSET_regs_gpr0+0x50]
	stp		x12, x13, [x30, #DV_OFFSET_regs_gpr0+0x60]
	stp		x14, x15, [x30, #DV_OFFSET_regs_gpr0+0x70]
	stp		x16, x17, [x30, #DV_OFFSET_regs_gpr0+0x80]
	stp		x18, x19, [x30, #DV_OFFSET_regs_gpr0+0x90]
	stp		x20, x21, [x30, #DV_OFFSET_regs_gpr0+0xa0]
	stp		x22, x23, [x30, #DV_OFFSET_regs_gpr0+0xb0]
	stp		x24, x25, [x30, #DV_OFFSET_regs_gpr0+0xc0]
	stp		x26, x27, [x30, #DV_OFFSET_regs_gpr0+0xd0]
	stp		x28, x29, [x30, #DV_OFFSET_regs_gpr0+0xe0]
	mov		x1, #0x03
	b	prj_vector_common

_prj_exc_handler3:
	.quad	prj_exc_handler

	.balign	128
	msr		TPIDR_EL0, x30
	mrs		x30, TPIDRRO_EL0
	ldr		x30, [x30, #DV_OFFSET_kvars_current_thread]
	ldr		x30, [x30, #DV_OFFSET_thr_regs]
	stp		x0, x1, [x30, #DV_OFFSET_regs_gpr0+0x00]
	stp		x2, x3, [x30, #DV_OFFSET_regs_gpr0+0x10]
	stp		x4, x5, [x30, #DV_OFFSET_regs_gpr0+0x20]
	stp		x6, x7, [x30, #DV_OFFSET_regs_gpr0+0x30]
	stp		x8, x9, [x30, #DV_OFFSET_regs_gpr0+0x40]
	stp		x10, x11, [x30, #DV_OFFSET_regs_gpr0+0x50]
	stp		x12, x13, [x30, #DV_OFFSET_regs_gpr0+0x60]
	stp		x14, x15, [x30, #DV_OFFSET_regs_gpr0+0x70]
	stp		x16, x17, [x30, #DV_OFFSET_regs_gpr0+0x80]
	stp		x18, x19, [x30, #DV_OFFSET_regs_gpr0+0x90]
	stp		x20, x21, [x30, #DV_OFFSET_regs_gpr0+0xa0]
	stp		x22, x23, [x30, #DV_OFFSET_regs_gpr0+0xb0]
	stp		x24, x25, [x30, #DV_OFFSET_regs_gpr0+0xc0]
	stp		x26, x27, [x30, #DV_OFFSET_regs_gpr0+0xd0]
	stp		x28, x29, [x30, #DV_OFFSET_regs_gpr0+0xe0]
	mov		x1, #0x10
	b	prj_vector_common

_prj_exc_handler4:
	.quad	prj_exc_handler

	.balign	128
	msr		TPIDR_EL0, x30
	mrs		x30, TPIDRRO_EL0
	ldr		x30, [x30, #DV_OFFSET_kvars_current_thread]
	ldr		x30, [x30, #DV_OFFSET_thr_regs]
	stp		x0, x1, [x30, #DV_OFFSET_regs_gpr0+0x00]
	stp		x2, x3, [x30, #DV_OFFSET_regs_gpr0+0x10]
	stp		x4, x5, [x30, #DV_OFFSET_regs_gpr0+0x20]
	stp		x6, x7, [x30, #DV_OFFSET_regs_gpr0+0x30]
	stp		x8, x9, [x30, #DV_OFFSET_regs_gpr0+0x40]
	stp		x10, x11, [x30, #DV_OFFSET_regs_gpr0+0x50]
	stp		x12, x13, [x30, #DV_OFFSET_regs_gpr0+0x60]
	stp		x14, x15, [x30, #DV_OFFSET_regs_gpr0+0x70]
	stp		x16, x17, [x30, #DV_OFFSET_regs_gpr0+0x80]
	stp		x18, x19, [x30, #DV_OFFSET_regs_gpr0+0x90]
	stp		x20, x21, [x30, #DV_OFFSET_regs_gpr0+0xa0]
	stp		x22, x23, [x30, #DV_OFFSET_regs_gpr0+0xb0]
	stp		x24, x25, [x30, #DV_OFFSET_regs_gpr0+0xc0]
	stp		x26, x27, [x30, #DV_OFFSET_regs_gpr0+0xd0]
	stp		x28, x29, [x30, #DV_OFFSET_regs_gpr0+0xe0]
	mov		x1, #0x11
	b	prj_vector_common

_prj_exc_handler5:
	.quad	prj_exc_handler

	.balign	128
	msr		TPIDR_EL0, x30
	mrs		x30, TPIDRRO_EL0
	ldr		x30, [x30, #DV_OFFSET_kvars_current_thread]
	ldr		x30, [x30, #DV_OFFSET_thr_regs]
	stp		x0, x1, [x30, #DV_OFFSET_regs_gpr0+0x00]
	stp		x2, x3, [x30, #DV_OFFSET_regs_gpr0+0x10]
	stp		x4, x5, [x30, #DV_OFFSET_regs_gpr0+0x20]
	stp		x6, x7, [x30, #DV_OFFSET_regs_gpr0+0x30]
	stp		x8, x9, [x30, #DV_OFFSET_regs_gpr0+0x40]
	stp		x10, x11, [x30, #DV_OFFSET_regs_gpr0+0x50]
	stp		x12, x13, [x30, #DV_OFFSET_regs_gpr0+0x60]
	stp		x14, x15, [x30, #DV_OFFSET_regs_gpr0+0x70]
	stp		x16, x17, [x30, #DV_OFFSET_regs_gpr0+0x80]
	stp		x18, x19, [x30, #DV_OFFSET_regs_gpr0+0x90]
	stp		x20, x21, [x30, #DV_OFFSET_regs_gpr0+0xa0]
	stp		x22, x23, [x30, #DV_OFFSET_regs_gpr0+0xb0]
	stp		x24, x25, [x30, #DV_OFFSET_regs_gpr0+0xc0]
	stp		x26, x27, [x30, #DV_OFFSET_regs_gpr0+0xd0]
	stp		x28, x29, [x30, #DV_OFFSET_regs_gpr0+0xe0]
	mov		x1, #0x12
	b	prj_vector_common

_prj_exc_handler6:
	.quad	prj_exc_handler

	.balign	128
	msr		TPIDR_EL0, x30
	mrs		x30, TPIDRRO_EL0
	ldr		x30, [x30, #DV_OFFSET_kvars_current_thread]
	ldr		x30, [x30, #DV_OFFSET_thr_regs]
	stp		x0, x1, [x30, #DV_OFFSET_regs_gpr0+0x00]
	stp		x2, x3, [x30, #DV_OFFSET_regs_gpr0+0x10]
	stp		x4, x5, [x30, #DV_OFFSET_regs_gpr0+0x20]
	stp		x6, x7, [x30, #DV_OFFSET_regs_gpr0+0x30]
	stp		x8, x9, [x30, #DV_OFFSET_regs_gpr0+0x40]
	stp		x10, x11, [x30, #DV_OFFSET_regs_gpr0+0x50]
	stp		x12, x13, [x30, #DV_OFFSET_regs_gpr0+0x60]
	stp		x14, x15, [x30, #DV_OFFSET_regs_gpr0+0x70]
	stp		x16, x17, [x30, #DV_OFFSET_regs_gpr0+0x80]
	stp		x18, x19, [x30, #DV_OFFSET_regs_gpr0+0x90]
	stp		x20, x21, [x30, #DV_OFFSET_regs_gpr0+0xa0]
	stp		x22, x23, [x30, #DV_OFFSET_regs_gpr0+0xb0]
	stp		x24, x25, [x30, #DV_OFFSET_regs_gpr0+0xc0]
	stp		x26, x27, [x30, #DV_OFFSET_regs_gpr0+0xd0]
	stp		x28, x29, [x30, #DV_OFFSET_regs_gpr0+0xe0]
	mov		x1, #0x13
	b	prj_vector_common

_prj_exc_handler7:
	.quad	prj_exc_handler

	.balign	128
	msr		TPIDR_EL0, x30
	mrs		x30, TPIDRRO_EL0
	ldr		x30, [x30, #DV_OFFSET_kvars_current_thread]
	ldr		x30, [x30, #DV_OFFSET_thr_regs]
	stp		x0, x1, [x30, #DV_OFFSET_regs_gpr0+0x00]
	stp		x2, x3, [x30, #DV_OFFSET_regs_gpr0+0x10]
	stp		x4, x5, [x30, #DV_OFFSET_regs_gpr0+0x20]
	stp		x6, x7, [x30, #DV_OFFSET_regs_gpr0+0x30]
	stp		x8, x9, [x30, #DV_OFFSET_regs_gpr0+0x40]
	stp		x10, x11, [x30, #DV_OFFSET_regs_gpr0+0x50]
	stp		x12, x13, [x30, #DV_OFFSET_regs_gpr0+0x60]
	stp		x14, x15, [x30, #DV_OFFSET_regs_gpr0+0x70]
	stp		x16, x17, [x30, #DV_OFFSET_regs_gpr0+0x80]
	stp		x18, x19, [x30, #DV_OFFSET_regs_gpr0+0x90]
	stp		x20, x21, [x30, #DV_OFFSET_regs_gpr0+0xa0]
	stp		x22, x23, [x30, #DV_OFFSET_regs_gpr0+0xb0]
	stp		x24, x25, [x30, #DV_OFFSET_regs_gpr0+0xc0]
	stp		x26, x27, [x30, #DV_OFFSET_regs_gpr0+0xd0]
	stp		x28, x29, [x30, #DV_OFFSET_regs_gpr0+0xe0]
	mov		x1, #0x20
	b	prj_vector_common

_prj_exc_handler8:
	.quad	prj_exc_handler

	.balign	128
	msr		TPIDR_EL0, x30
	mrs		x30, TPIDRRO_EL0
	ldr		x30, [x30, #DV_OFFSET_kvars_current_thread]
	ldr		x30, [x30, #DV_OFFSET_thr_regs]
	stp		x0, x1, [x30, #DV_OFFSET_regs_gpr0+0x00]
	stp		x2, x3, [x30, #DV_OFFSET_regs_gpr0+0x10]
	stp		x4, x5, [x30, #DV_OFFSET_regs_gpr0+0x20]
	stp		x6, x7, [x30, #DV_OFFSET_regs_gpr0+0x30]
	stp		x8, x9, [x30, #DV_OFFSET_regs_gpr0+0x40]
	stp		x10, x11, [x30, #DV_OFFSET_regs_gpr0+0x50]
	stp		x12, x13, [x30, #DV_OFFSET_regs_gpr0+0x60]
	stp		x14, x15, [x30, #DV_OFFSET_regs_gpr0+0x70]
	stp		x16, x17, [x30, #DV_OFFSET_regs_gpr0+0x80]
	stp		x18, x19, [x30, #DV_OFFSET_regs_gpr0+0x90]
	stp		x20, x21, [x30, #DV_OFFSET_regs_gpr0+0xa0]
	stp		x22, x23, [x30, #DV_OFFSET_regs_gpr0+0xb0]
	stp		x24, x25, [x30, #DV_OFFSET_regs_gpr0+0xc0]
	stp		x26, x27, [x30, #DV_OFFSET_regs_gpr0+0xd0]
	stp		x28, x29, [x30, #DV_OFFSET_regs_gpr0+0xe0]
	mov		x1, #0x21
	b	prj_vector_common

_prj_exc_handler9:
	.quad	prj_exc_handler

	.balign	128
	msr		TPIDR_EL0, x30
	mrs		x30, TPIDRRO_EL0
	ldr		x30, [x30, #DV_OFFSET_kvars_current_thread]
	ldr		x30, [x30, #DV_OFFSET_thr_regs]
	stp		x0, x1, [x30, #DV_OFFSET_regs_gpr0+0x00]
	stp		x2, x3, [x30, #DV_OFFSET_regs_gpr0+0x10]
	stp		x4, x5, [x30, #DV_OFFSET_regs_gpr0+0x20]
	stp		x6, x7, [x30, #DV_OFFSET_regs_gpr0+0x30]
	stp		x8, x9, [x30, #DV_OFFSET_regs_gpr0+0x40]
	stp		x10, x11, [x30, #DV_OFFSET_regs_gpr0+0x50]
	stp		x12, x13, [x30, #DV_OFFSET_regs_gpr0+0x60]
	stp		x14, x15, [x30, #DV_OFFSET_regs_gpr0+0x70]
	stp		x16, x17, [x30, #DV_OFFSET_regs_gpr0+0x80]
	stp		x18, x19, [x30, #DV_OFFSET_regs_gpr0+0x90]
	stp		x20, x21, [x30, #DV_OFFSET_regs_gpr0+0xa0]
	stp		x22, x23, [x30, #DV_OFFSET_regs_gpr0+0xb0]
	stp		x24, x25, [x30, #DV_OFFSET_regs_gpr0+0xc0]
	stp		x26, x27, [x30, #DV_OFFSET_regs_gpr0+0xd0]
	stp		x28, x29, [x30, #DV_OFFSET_regs_gpr0+0xe0]
	mov		x1, #0x22
	b	prj_vector_common

_prj_exc_handlera:
	.quad	prj_exc_handler

	.balign	128
	msr		TPIDR_EL0, x30
	mrs		x30, TPIDRRO_EL0
	ldr		x30, [x30, #DV_OFFSET_kvars_current_thread]
	ldr		x30, [x30, #DV_OFFSET_thr_regs]
	stp		x0, x1, [x30, #DV_OFFSET_regs_gpr0+0x00]
	stp		x2, x3, [x30, #DV_OFFSET_regs_gpr0+0x10]
	stp		x4, x5, [x30, #DV_OFFSET_regs_gpr0+0x20]
	stp		x6, x7, [x30, #DV_OFFSET_regs_gpr0+0x30]
	stp		x8, x9, [x30, #DV_OFFSET_regs_gpr0+0x40]
	stp		x10, x11, [x30, #DV_OFFSET_regs_gpr0+0x50]
	stp		x12, x13, [x30, #DV_OFFSET_regs_gpr0+0x60]
	stp		x14, x15, [x30, #DV_OFFSET_regs_gpr0+0x70]
	stp		x16, x17, [x30, #DV_OFFSET_regs_gpr0+0x80]
	stp		x18, x19, [x30, #DV_OFFSET_regs_gpr0+0x90]
	stp		x20, x21, [x30, #DV_OFFSET_regs_gpr0+0xa0]
	stp		x22, x23, [x30, #DV_OFFSET_regs_gpr0+0xb0]
	stp		x24, x25, [x30, #DV_OFFSET_regs_gpr0+0xc0]
	stp		x26, x27, [x30, #DV_OFFSET_regs_gpr0+0xd0]
	stp		x28, x29, [x30, #DV_OFFSET_regs_gpr0+0xe0]
	mov		x1, #0x23
	b	prj_vector_common

_prj_exc_handlerb:
	.quad	prj_exc_handler

	.balign	128
	msr		TPIDR_EL0, x30
	mrs		x30, TPIDRRO_EL0
	ldr		x30, [x30, #DV_OFFSET_kvars_current_thread]
	ldr		x30, [x30, #DV_OFFSET_thr_regs]
	stp		x0, x1, [x30, #DV_OFFSET_regs_gpr0+0x00]
	stp		x2, x3, [x30, #DV_OFFSET_regs_gpr0+0x10]
	stp		x4, x5, [x30, #DV_OFFSET_regs_gpr0+0x20]
	stp		x6, x7, [x30, #DV_OFFSET_regs_gpr0+0x30]
	stp		x8, x9, [x30, #DV_OFFSET_regs_gpr0+0x40]
	stp		x10, x11, [x30, #DV_OFFSET_regs_gpr0+0x50]
	stp		x12, x13, [x30, #DV_OFFSET_regs_gpr0+0x60]
	stp		x14, x15, [x30, #DV_OFFSET_regs_gpr0+0x70]
	stp		x16, x17, [x30, #DV_OFFSET_regs_gpr0+0x80]
	stp		x18, x19, [x30, #DV_OFFSET_regs_gpr0+0x90]
	stp		x20, x21, [x30, #DV_OFFSET_regs_gpr0+0xa0]
	stp		x22, x23, [x30, #DV_OFFSET_regs_gpr0+0xb0]
	stp		x24, x25, [x30, #DV_OFFSET_regs_gpr0+0xc0]
	stp		x26, x27, [x30, #DV_OFFSET_regs_gpr0+0xd0]
	stp		x28, x29, [x30, #DV_OFFSET_regs_gpr0+0xe0]
	mov		x1, #0x30
	b	prj_vector_common

_prj_exc_handlerc:
	.quad	prj_exc_handler

	.balign	128
	msr		TPIDR_EL0, x30
	mrs		x30, TPIDRRO_EL0
	ldr		x30, [x30, #DV_OFFSET_kvars_current_thread]
	ldr		x30, [x30, #DV_OFFSET_thr_regs]
	stp		x0, x1, [x30, #DV_OFFSET_regs_gpr0+0x00]
	stp		x2, x3, [x30, #DV_OFFSET_regs_gpr0+0x10]
	stp		x4, x5, [x30, #DV_OFFSET_regs_gpr0+0x20]
	stp		x6, x7, [x30, #DV_OFFSET_regs_gpr0+0x30]
	stp		x8, x9, [x30, #DV_OFFSET_regs_gpr0+0x40]
	stp		x10, x11, [x30, #DV_OFFSET_regs_gpr0+0x50]
	stp		x12, x13, [x30, #DV_OFFSET_regs_gpr0+0x60]
	stp		x14, x15, [x30, #DV_OFFSET_regs_gpr0+0x70]
	stp		x16, x17, [x30, #DV_OFFSET_regs_gpr0+0x80]
	stp		x18, x19, [x30, #DV_OFFSET_regs_gpr0+0x90]
	stp		x20, x21, [x30, #DV_OFFSET_regs_gpr0+0xa0]
	stp		x22, x23, [x30, #DV_OFFSET_regs_gpr0+0xb0]
	stp		x24, x25, [x30, #DV_OFFSET_regs_gpr0+0xc0]
	stp		x26, x27, [x30, #DV_OFFSET_regs_gpr0+0xd0]
	stp		x28, x29, [x30, #DV_OFFSET_regs_gpr0+0xe0]
	mov		x1, #0x31
	b	prj_vector_common

_prj_exc_handlerd:
	.quad	prj_exc_handler

	.balign	128
	msr		TPIDR_EL0, x30
	mrs		x30, TPIDRRO_EL0
	ldr		x30, [x30, #DV_OFFSET_kvars_current_thread]
	ldr		x30, [x30, #DV_OFFSET_thr_regs]
	stp		x0, x1, [x30, #DV_OFFSET_regs_gpr0+0x00]
	stp		x2, x3, [x30, #DV_OFFSET_regs_gpr0+0x10]
	stp		x4, x5, [x30, #DV_OFFSET_regs_gpr0+0x20]
	stp		x6, x7, [x30, #DV_OFFSET_regs_gpr0+0x30]
	stp		x8, x9, [x30, #DV_OFFSET_regs_gpr0+0x40]
	stp		x10, x11, [x30, #DV_OFFSET_regs_gpr0+0x50]
	stp		x12, x13, [x30, #DV_OFFSET_regs_gpr0+0x60]
	stp		x14, x15, [x30, #DV_OFFSET_regs_gpr0+0x70]
	stp		x16, x17, [x30, #DV_OFFSET_regs_gpr0+0x80]
	stp		x18, x19, [x30, #DV_OFFSET_regs_gpr0+0x90]
	stp		x20, x21, [x30, #DV_OFFSET_regs_gpr0+0xa0]
	stp		x22, x23, [x30, #DV_OFFSET_regs_gpr0+0xb0]
	stp		x24, x25, [x30, #DV_OFFSET_regs_gpr0+0xc0]
	stp		x26, x27, [x30, #DV_OFFSET_regs_gpr0+0xd0]
	stp		x28, x29, [x30, #DV_OFFSET_regs_gpr0+0xe0]
	mov		x1, #0x32
	b	prj_vector_common

_prj_exc_handlere:
	.quad	prj_exc_handler

	.balign	128
	msr		TPIDR_EL0, x30
	mrs		x30, TPIDRRO_EL0
	ldr		x30, [x30, #DV_OFFSET_kvars_current_thread]
	ldr		x30, [x30, #DV_OFFSET_thr_regs]
	stp		x0, x1, [x30, #DV_OFFSET_regs_gpr0+0x00]
	stp		x2, x3, [x30, #DV_OFFSET_regs_gpr0+0x10]
	stp		x4, x5, [x30, #DV_OFFSET_regs_gpr0+0x20]
	stp		x6, x7, [x30, #DV_OFFSET_regs_gpr0+0x30]
	stp		x8, x9, [x30, #DV_OFFSET_regs_gpr0+0x40]
	stp		x10, x11, [x30, #DV_OFFSET_regs_gpr0+0x50]
	stp		x12, x13, [x30, #DV_OFFSET_regs_gpr0+0x60]
	stp		x14, x15, [x30, #DV_OFFSET_regs_gpr0+0x70]
	stp		x16, x17, [x30, #DV_OFFSET_regs_gpr0+0x80]
	stp		x18, x19, [x30, #DV_OFFSET_regs_gpr0+0x90]
	stp		x20, x21, [x30, #DV_OFFSET_regs_gpr0+0xa0]
	stp		x22, x23, [x30, #DV_OFFSET_regs_gpr0+0xb0]
	stp		x24, x25, [x30, #DV_OFFSET_regs_gpr0+0xc0]
	stp		x26, x27, [x30, #DV_OFFSET_regs_gpr0+0xd0]
	stp		x28, x29, [x30, #DV_OFFSET_regs_gpr0+0xe0]
	mov		x1, #0x33
	b	prj_vector_common

_prj_exc_handlerf:
	.quad	prj_exc_handler

prj_vector_common:
	mrs		x2, TPIDR_EL0
	mrs		x3, ELR_EL1
	stp		x2, x3, [x30, #DV_OFFSET_regs_gpr0+0xf0]
	mrs		x4, SPSR_EL1
	mrs		x5, SP_EL0
	stp		x4, x5, [x30, #DV_OFFSET_regs_psr]
	mrs		x6, ESR_EL1
	str		w6, [x30, #DV_OFFSET_regs_esr]
	mrs		x0, TPIDRRO_EL0
	ldr		x3, [x0, #DV_OFFSET_kvars_kernelsp]
	mov		sp, x3
	b	prj_exc_handler		/* x0 = kvars, x1 = handler addr/id, x2 = lr, x3 = sp */

