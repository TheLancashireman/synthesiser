ARDUINO_BASE			= /data1/projects/arduino
ARDUINO_DIR				= $(ARDUINO_BASE)/arduino-1.6.8
TARGET					= analogue
MCU						= atmega328p
F_CPU					= 16000000
ARDUINO_LIBS			= LiquidCrystal
ARDUINO_PORT			= /dev/ttyUSB0
AVRDUDE_ARD_BAUDRATE	= 57600
AVRDUDE_ARD_PROGRAMMER	= arduino
BOARD_TAG				= nano
ARDUINO_TOOLS_PATH		= $(ARDUINO_DIR)/hardware/tools
ARDUINO_ETC_PATH		= $(ARDUINO_TOOLS_PATH)/avr/etc
AVR_TOOLS_PATH			= $(ARDUINO_TOOLS_PATH)/avr/bin

PRJ_INCLUDES			= -I../arduino-common

OTHER_OBJS				+= $(OBJDIR)/init.o
OTHER_OBJS				+= $(OBJDIR)/timestamp.o
OTHER_OBJS				+= $(OBJDIR)/midi.o

include $(ARDUINO_BASE)/Arduino.make

$(OBJDIR)/init.o:		../arduino-common/init.cpp
	$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) $< -o $@

$(OBJDIR)/timestamp.o:	../arduino-common/timestamp.cpp
	$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) $< -o $@

$(OBJDIR)/midi.o:		../arduino-common/midi.cpp
	$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) $< -o $@
