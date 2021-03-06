#!/usr/bin/python3
#
# Program to send midi commands to the analogue-synth project to set up the DAC
#
#
# n123 - send note-on command for note 123 (range 0..127)
# n    - send note-off command for last note
# c123 - set coarse DAC to 123 (range 0..255)
# f123 - set fine DAC to 123 (range 0..255)
# g1   - set gate on
# g0   - set gate off
# t	   - start tuning
# z    - start 1 hz gate on/off
#
#
# Midi:
#   0x8x     k   v          Note off, channel x, key k, velocity v
#   0x9x     k   v          Note on, channel x, key k, velocity v
#   0xBx     n   v          Continuous controller n, channel x, value v

import sys
import serial
import time

chan = 0
ser = serial.Serial('/dev/ttyUSB0', 115200, 8, 'N', 1)

def usage():
	print('Usage:')
	print('  n123 - send note-on command for note 123 (range 0..127)')
	print('  n    - send note-off command for last note')
	print('  c123 - set coarse DAC to 123 (range 0..255)')
	print('  f123 - set fine DAC to 123 (range 0..255)')
	print('  g1   - set gate on')
	print('  g0   - set gate off')
	print('  t    - start tuning')
	print('  s123 - play sequence 123')
	print('  z    - start gate test: 1 Hz square wave')
	print('  q    - quit')
	return

def note_on(n):
	global ser
	global chan
	print('Note', n, 'on')
	cmd = bytearray([0x90+chan, n, 127])
	ser.write(cmd)
	return

def note_off(n):
	global ser
	global chan
	print('Note', n, 'off')
	cmd = bytearray([0x80+chan, n, 127])
	ser.write(cmd)
	return

def set_dac(s, v):
	global ser
	global chan
	if s == 'c':
		print('Coarse:', v)
		ctrl = 0
	else:
		print('Fine:', v)
		ctrl = 2
	if v <= 127:
		v1 = v
	else:
		v1 = v - 128
		ctrl += 1
	cmd = bytearray([0xb0+chan, ctrl, v1])
	ser.write(cmd)
	return

def set_gate(v):
	global ser
	global chan
	print('Gate', v)
	cmd = bytearray([0xb0+chan, 4, v])
	ser.write(cmd)
	return

def gate_on_off():
	try:
		while True:
			set_gate(0)
			time.sleep(0.5)
			set_gate(1)
			time.sleep(0.5)
	except:
		print('\nInterrupted')

def start_tuning():
	global ser
	global chan
	print('Tuning')
	cmd = bytearray([0xb0+chan, 127, 127])
	ser.write(cmd)
	return

def seqnote(n):
	note_on(n)
	time.sleep(0.3)
	note_off(n)
	time.sleep(0.1)

def boogie(base):
	seqnote(base)
	seqnote(base+4)
	seqnote(base+4+3)
	seqnote(base+4+3+2)
	seqnote(base+4+3+2+1)
	seqnote(base+4+3+2)
	seqnote(base+4+3)
	seqnote(base+4)

def seq0(base):
	while True:
		boogie(base)

def seq1(base):
	while True:
		seqnote(base)
		seqnote(base+4)
		seqnote(base+7)
		seqnote(base+9)
		seqnote(base+12)
		seqnote(base+9)
		seqnote(base+7)
		seqnote(base+4)
		seqnote(base+7)
		seqnote(base-5)
		seqnote(base-1)
		seqnote(base+2)
		seqnote(base+7)
		seqnote(base+4)
		seqnote(base+2)
		seqnote(base+4)
		seqnote(base+9)
		seqnote(base+4)
		seqnote(base+9)
		seqnote(base+7)
		seqnote(base+5)
		seqnote(base)
		seqnote(base+5)
		seqnote(base+7)
		seqnote(base+9)
		seqnote(base+7)
		seqnote(base+16)
		seqnote(base+14)
		seqnote(base+12)
		seqnote(base+11)
		seqnote(base+9)
		seqnote(base+7)

def seq2(base):
	while True:
		boogie(base)
		boogie(base)
		boogie(base+5)
		boogie(base)
		boogie(base+7)
		boogie(base)


def play_sequence(n):
	try:
		if n == 0:
			seq0(48)
		elif n == 1:
			seq1(36)
		elif n == 2:
			seq2(36)
		else:
			print('No sequence #' + n)
	except:
			print('\nInterrupted')

lastnote = -1

print('>', end=' ', flush=True)

for l in sys.stdin:
	l = l.rstrip()

	if len(l) > 0:

		c = l[0]
		n = l[1:]

		if c == 'q':
			exit(0)

		if c == 'n':
			if len(n) > 0:
				try:
					v = int(n)
				except:
					v = -1
				if v >= 0 and v <= 127:
					lastnote = v
					note_on(v)
				else:
					usage()
			elif lastnote >= 0:
				note_off(lastnote)
				lastnote = -1
		elif c == 'c' or c == 'f':
			try:
				v = int(n)
			except:
				v = -1
			if v >= 0 and v <= 255:
				set_dac(c, v)
			else:
				usage()
		elif c == 'g':
			if len(n) > 0:
				try:
					v = int(n)
				except:
					v = -1
				if v >= 0 and v <= 1:
					set_gate(v)
				else:
					usage()
			else:
				usage()
		elif c == 's':
			if len(n) > 0:
				try:
					v = int(n)
				except:
					v = -1
			else:
				v = 0
			play_sequence(v)
		elif c == 't':
			start_tuning()
		elif c == 'z':
			gate_on_off()
		else:
			usage()
	print('>', end=' ', flush=True)

exit(0)
