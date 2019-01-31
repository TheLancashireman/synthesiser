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

DV_ROOT		?= /data1/projects/davros
DVSK_ROOT	= $(DV_ROOT)/davroska

# Board settings
DV_BOARD 	?=	pi3-arm64

# Compiler settings
DV_GNU_D	?=	/data1/gnu/gcc-linaro-6.3.1-2017.02-x86_64_aarch64-elf

DV_GCC		?=	$(DV_GNU_D)/bin/aarch64-elf-gcc
DV_LD		?=	$(DV_GNU_D)/bin/aarch64-elf-ld
DV_OBJCOPY	?=	$(DV_GNU_D)/bin/aarch64-elf-objcopy
DV_LDLIB_D	?=	$(DV_GNU_D)/aarch64-elf/libc/usr/lib/
DV_LDSCRIPT ?=	$(DVSK_ROOT)/hardware/arm64/ld/dv-pi3.ldscript

DV_ENTRY	?=	dv_reset

DV_BIN_D	=	bin
DV_OBJ_D	=	obj

CC_OPT		+=	-D DV_DAVROSKA=1
CC_OPT		+=	-I h
CC_OPT		+=	-I $(DVSK_ROOT)/h
CC_OPT		+=	-I $(DVSK_ROOT)/hardware
CC_OPT		+=	-I $(DV_ROOT)/devices/h
CC_OPT		+=	-I $(DV_ROOT)/lib/h
CC_OPT		+=	-Wall
CC_OPT		+=	-fno-common

LD_OPT		+=	-e $(DV_ENTRY)
LD_OPT		+=	-T $(DV_LDSCRIPT)
LD_OPT		+=	-L $(DV_LDLIB_D)
LD_OPT		+=	-lc

# project files
DV_LD_OBJS	+=	$(DV_OBJ_D)/davroska-hardware.o
DV_LD_OBJS	+=	$(DV_OBJ_D)/davroska-main.o
DV_LD_OBJS	+=	$(DV_OBJ_D)/synth-effect-main.o
DV_LD_OBJS	+=	$(DV_OBJ_D)/effect.o
DV_LD_OBJS	+=	$(DV_OBJ_D)/effect-adc.o
DV_LD_OBJS	+=	$(DV_OBJ_D)/effect-dac.o
DV_LD_OBJS	+=	$(DV_OBJ_D)/effect-synth.o
DV_LD_OBJS	+=	$(DV_OBJ_D)/midi.o
DV_LD_OBJS	+=	$(DV_OBJ_D)/wave.o
DV_LD_OBJS	+=	$(DV_OBJ_D)/adsr.o

# davroska and associated library files
DV_LD_OBJS	+=	$(DV_OBJ_D)/davroska.o
DV_LD_OBJS	+=	$(DV_OBJ_D)/davroska-time.o
DV_LD_OBJS	+=	$(DV_OBJ_D)/davroska-extended.o
DV_LD_OBJS	+=	$(DV_OBJ_D)/dv-printf.o
DV_LD_OBJS	+=	$(DV_OBJ_D)/dv-xprintf.o

# davroska pi3-arm64 files 
DV_LD_OBJS	+=	$(DV_OBJ_D)/davroska-arm64.o

DV_LD_OBJS	+=	$(DV_OBJ_D)/dv-arm-bcm2835-uart.o
DV_LD_OBJS	+=	$(DV_OBJ_D)/dv-arm-bcm2835-gpio.o
DV_LD_OBJS	+=	$(DV_OBJ_D)/dv-arm-bcm2835-pcm.o
DV_LD_OBJS	+=	$(DV_OBJ_D)/dv-arm-bcm2835-interruptcontroller.o
DV_LD_OBJS	+=	$(DV_OBJ_D)/dv-arm-bcm2836-interruptcontroller.o

DV_LD_OBJS	+=	$(DV_OBJ_D)/dv-armv8-mmu.o
DV_LD_OBJS	+=	$(DV_OBJ_D)/dv-arm64-setmmuregisters.o
DV_LD_OBJS	+=	$(DV_OBJ_D)/dv-arm64-reset.o
DV_LD_OBJS	+=	$(DV_OBJ_D)/dv-switchcall.o
DV_LD_OBJS	+=	$(DV_OBJ_D)/dv-switch-el3el2.o
DV_LD_OBJS	+=	$(DV_OBJ_D)/dv-switch-el2el1.o
DV_LD_OBJS	+=	$(DV_OBJ_D)/dv-vectors.o


VPATH		+=	c
VPATH		+=	s
VPATH		+=	$(DVSK_ROOT)/c
VPATH		+=	$(DVSK_ROOT)/hardware/arm64/c
VPATH		+=	$(DVSK_ROOT)/hardware/arm64/s
VPATH		+=	$(DVSK_ROOT)/hardware/common/c
VPATH		+=	$(DV_ROOT)/lib/c
VPATH		+=	$(DV_ROOT)/devices/c
VPATH		+=	$(DV_ROOT)/devices/s

.PHONY:		default all help clean srec

default:	all

all:		$(DV_OBJ_D) $(DV_BIN_D) elf

clean:
	-rm -rf $(DV_OBJ_D) $(DV_BIN_D)

elf:		$(DV_BIN_D)/synth.elf

$(DV_BIN_D)/synth.elf:	$(DV_LD_OBJS)
	$(DV_LD) -o $@ $(DV_LD_OBJS) $(LD_LIB) $(LD_OPT)

$(DV_OBJ_D)/%.o:	%.c
	$(DV_GCC) $(CC_OPT) -o $@ -c $<

$(DV_OBJ_D)/%.o:	%.S
	$(DV_GCC) $(CC_OPT) -o $@ -c $<

$(DV_BIN_D):
	mkdir -p bin

$(DV_OBJ_D):
	mkdir -p obj

srec:		all
	$(DV_OBJCOPY) bin/synth.elf -O srec --srec-forceS3 /dev/stdout | dos2unix | egrep -v '^S3..........00*..$$' > bin/synth.srec
