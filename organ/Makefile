# Makefile - for organ components
#
# (c) David Haworth

CFLAGS += -I.
CFLAGS += -Wall

Objects += organ.o
Objects += waveform.o
Objects += tonegen.o

organ:	$(Objects)
	cc -o $@ $^ -lm

clean:
	-rm organ
	-rm $(Objects)
	-rm *.dat
	-rm *.wav

%.wav:	%.dat
	sox $^ $@

%.dat:	organ
	./organ > $@
