#	Makefile - top level Makefile for effects/organ/synthesiser project
#
#	Copyright 2018 David Haworth
#
#	This file is part of $SYNTH.
#
#	$SYNTH is free software: you can redistribute it and/or modify
#	it under the terms of the GNU General Public License as published by
#	the Free Software Foundation, either version 3 of the License, or
#	(at your option) any later version.
#
#	$SYNTH is distributed in the hope that it will be useful,
#	but WITHOUT ANY WARRANTY; without even the implied warranty of
#	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#	GNU General Public License for more details.
#
#	You should have received a copy of the GNU General Public License
#	along with $SYNTH.  If not, see <http://www.gnu.org/licenses/>.

# Find out where we are :-)
PRJ_ROOT		?= $(shell pwd)
DV_ROOT			= $(PRJ_ROOT)/../davros/davros-3

# Select your board directory here
#DV_BOARD_D 		?=	$(DV_ROOT)/board/arm/raspberry-pi
DV_BOARD_D 		?=	$(DV_ROOT)/board/arm64/raspberry-pi-3

DV_COMPILER		?=	gnu

ifeq ($(DV_BOARD_D), $(DV_ROOT)/board/arm64/raspberry-pi-3)

# 64-bit

DV_GNU_D	?=	/data1/gnu/gcc-linaro-6.3.1-2017.02-x86_64_aarch64-elf
LD_OPT		+= -e dv_reset
#LD_OPT		+= -T $(DV_BOARD_D)/ldscript/pi3-64.ldscript
LD_OPT		+= -T $(DV_BOARD_D)/ldscript/rpi3-0based.ldscript

else

# 32-bit

DV_GNU_D	?=	/data1/gnu/gcc-linaro-6.3.1-2017.02-x86_64_arm-eabi
LD_OPT		+= -e dv_trap_reset
LD_OPT		+= -T $(DV_BOARD_D)/ldscript/pi-zero.ldscript

endif

DV_LD_OBJS	+= $(DV_OBJ_D)/dv-vectors.$(DV_O)
DV_LD_OBJS	+= $(DV_OBJ_D)/dv-c0.$(DV_O)
DV_LD_OBJS	+= $(DV_OBJ_D)/prj-vectors.$(DV_O)
DV_LD_OBJS	+= $(DV_OBJ_D)/synth-effect-main.$(DV_O)
DV_LD_OBJS	+= $(DV_OBJ_D)/effect.$(DV_O)
DV_LD_OBJS	+= $(DV_OBJ_D)/effect-adc.$(DV_O)
DV_LD_OBJS	+= $(DV_OBJ_D)/effect-dac.$(DV_O)
DV_LD_OBJS	+= $(DV_OBJ_D)/effect-synth.$(DV_O)
DV_LD_OBJS	+= $(DV_OBJ_D)/midi.$(DV_O)
DV_LD_OBJS	+= $(DV_OBJ_D)/wave.$(DV_O)
DV_LD_OBJS	+= $(DV_OBJ_D)/adsr.$(DV_O)

# Include the board's file lists
include $(DV_BOARD_D)/make/dv-board-list.make

#CC_OPT		+= -O2

VPATH       += project/c
VPATH       += project/s
VPATH       += $(DV_BOARD_D)/c
VPATH       += $(DV_BOARD_D)/s

DV_KLIB	= $(DV_LIB_D)/lib$(DV_KLIB_NAME).$(DV_A)
DV_ULIB = $(DV_LIB_D)/lib$(DV_ULIB_NAME).$(DV_A)

# Include the board's make selections
include $(DV_BOARD_D)/make/dv-board.make

include $(DV_ROOT)/make/dv-defs.make

.PHONY:		default all help klib ulib clean srec

default:	all

all:		klib ulib bin srec

klib:		$(DV_KLIB)

ulib:		$(DV_ULIB)

bin:		$(DV_BIN_D)/synth.bin

srec:		$(DV_BIN_D)/synth.srec

$(DV_BIN_D)/synth.srec:	 $(DV_BIN_D)/synth.elf
	$(dv_elf2srec)

$(DV_BIN_D)/synth.bin:	 $(DV_BIN_D)/synth.elf
	$(dv_elf2bin)

$(DV_BIN_D)/synth.elf:	$(DV_LD_OBJS) $(DV_KLIB) $(DV_ULIB)
	$(dv_ld)

install:	$(DV_BIN_D)/synth.bin
	cp $(DV_BIN_D)/synth.bin ../../raspberry-pi/usb-zero/msd/kernel.img


include $(DV_ROOT)/make/dv-commontargets.make
include $(DV_ROOT)/cpufamily/$(DV_FAMILY)/make/dv-$(DV_FAMILY)-$(DV_COMPILER).make
include $(DV_ROOT)/make/dv-rules.make
include $(DV_ROOT)/make/dv-klib.make
include $(DV_ROOT)/make/dv-ulib.make
