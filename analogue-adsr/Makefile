# Makefile for analogue-adsr
#
# Copyright David Haworth
# 
# This file is part of analogue-adsr.
# 
# analogue-adsr is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
# 
# analogue-adsr is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with analogue-adsr  If not, see <http://www.gnu.org/licenses/>.


BUILD		?=	build
ISPPORT		?=	/dev/ttyUSB0
OBJ_DIR		?=	$(BUILD)/o
LIB_DIR		?=	$(BUILD)/a
TLIB_DIR	?=	../../tiny-bare-metal/tinylib

GCC			=	avr-gcc
GLD			=	avr-gcc
GAR			=	avr-ar
OBJCOPY		=	avr-objcopy

CC_OPT		+=	-mmcu=attiny85
CC_OPT		+=	-fno-builtin
CC_OPT		+=	-Os
CC_OPT		+=	-g
CC_OPT		+=	-Wall
CC_OPT		+=	-I $(TLIB_DIR)

LD_OPT		+=	-mmcu=attiny85
LD_OPT		+=	-Wl,--gc-sections
LD_OPT		+=	-Os
LD_OPT		+=	-L $(LIB_DIR)

LD_LIB		+=	-ltiny
LD_LIB		+=	-lm

OBJS		+=	$(OBJ_DIR)/analogue-adsr.o
OBJS		+=	$(OBJ_DIR)/adsr.o

VPATH		+=	$(TLIB_DIR)
VPATH		+=	$(TIO_DIR)

.PHONY:		default clean hex upload

default:	$(BUILD)/analogue-adsr.elf

clean:
	if [ -d $(BUILD) ] ; then rm -r $(BUILD); fi

hex:		$(BUILD)/analogue-adsr.ihex

$(BUILD)/o:
	mkdir -p $(BUILD)/o

$(BUILD)/analogue-adsr.elf:	$(OBJ_DIR) $(OBJS) $(LIB_DIR)/libtiny.a
	$(GLD) $(LD_OPT) -o $@ $(OBJS) $(LD_LIB)
	

$(OBJ_DIR)/%.o:	%.c
	$(GCC) $(CC_OPT) -o $@ -c $<

$(BUILD)/analogue-adsr.ihex:	$(BUILD)/analogue-adsr.elf
	$(OBJCOPY) -O ihex -R .eeprom $< $@

upload:		$(BUILD)/analogue-adsr.ihex
	avrdude -P $(ISPPORT) -b 19200 -c avrisp -p t85 -U flash:w:build/analogue-adsr.ihex:i

include $(TLIB_DIR)/tinylib.make
