# Makefile for noise-generator
#
# Copyright David Haworth
# 
# This file is part of noise-generator.
# 
# noise-generator is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
# 
# noise-generator is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with noise-generator  If not, see <http://www.gnu.org/licenses/>.


BUILD		?=	build
ISPPORT		?=	/dev/ttyUSB0
OBJ_DIR		?=	$(BUILD)/o
LIB_DIR		?=	$(BUILD)/a
TLIB_DIR	?=	../../tiny-bare-metal/tinylib

GCC			=	avr-gcc
GLD			=	avr-gcc
GAR			=	avr-ar
OBJCOPY		=	avr-objcopy

CC_OPT		+=	-mmcu=attiny13a
CC_OPT		+=	-fno-builtin
CC_OPT		+=	-O3
CC_OPT		+=	-g
CC_OPT		+=	-Wall
CC_OPT		+=	-I $(TLIB_DIR)

LD_OPT		+=	-mmcu=attiny13a
LD_OPT		+=	-Wl,--gc-sections
LD_OPT		+=	-Os
LD_OPT		+=	-L $(LIB_DIR)

LD_LIB		+=	-ltiny
LD_LIB		+=	-lm

OBJS		+=	$(OBJ_DIR)/noise-generator.o

VPATH		+=	$(TLIB_DIR)
VPATH		+=	$(TIO_DIR)

.PHONY:		default clean hex upload

default:	$(BUILD)/noise-generator.elf

clean:
	if [ -d $(BUILD) ] ; then rm -r $(BUILD); fi

hex:		$(BUILD)/noise-generator.ihex

$(BUILD)/o:
	mkdir -p $(BUILD)/o

$(BUILD)/noise-generator.elf:	$(OBJ_DIR) $(OBJS) $(LIB_DIR)/libtiny.a
	$(GLD) $(LD_OPT) -o $@ $(OBJS) $(LD_LIB)
	

$(OBJ_DIR)/%.o:	%.c
	$(GCC) $(CC_OPT) -o $@ -c $<

$(BUILD)/noise-generator.ihex:	$(BUILD)/noise-generator.elf
	$(OBJCOPY) -O ihex -R .eeprom $< $@

upload:		$(BUILD)/noise-generator.ihex
	avrdude -P $(ISPPORT) -b 19200 -c avrisp -p t13 -U flash:w:build/noise-generator.ihex:i

include $(TLIB_DIR)/tinylib.make
