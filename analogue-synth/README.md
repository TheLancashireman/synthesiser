# analogue-synth

An arduino sketch that controls a monophonic analogue synthesiser by means of a one-volt-per-octave
control voltage (CV) and a note on/off signal (GATE).

## Caveat

To allow the PWM outputs to generate analogue levels with very little ripple (and whose
ripple is above normal hearing range), the timers are clocked at full speed. This means
that the normal Arduino timing functions like millis(), micros() and delay() aren't
available. If you adapt the sketch and attempt to use Arduino timing, you should get a linker
error because of the duplicate init() function.

## Description

Timer0 runs at full speed (16 MHz on a Nano) to give the least ripple.

The DAC is constructed from two PWM outputs, OC0A and OC0B. OC0A provides the coarse control
and OC0B the fine control. To convert the PWM outputs into a true analogue output, an RC network
with a 100 nF capacitor is used. Both of the PWM outputs are fed to the same lead of the capacitor
(summing junction) via resistors: 4k7 for OC0A (coarse) and 100k for OC0B (fine). The other lead of
the capacitor is is connected to Arduino ground.
This arrangement gives OC0B about a 1:20 influence over the output signal and allows a range of over
4 V with a resolution of less than a millivolt.

The summing junction is fed to the non-inverting input of an op-amp configured as a non-inverting
amplifier. The output of the op-amp is the control voltage. To acheive a reasonable range that can
be adjusted in software, a gain of about 2.5 is required. A potential divider consisting of two 10k
resistors with a 10k potentiometer between them is connected between the op-amp's output and ground.
The wiper of the potentionmeter is connected to the inverting input of the op-amp.
This arrangement gives an adjustable gain of between 1.5 and 3. The op-amp used is LM324, but
just about any op-amp will work.

Important: the op-amp must be supplied from a symmetrical power supply - e.g. 15-0-15. The arduino ground
is connected to the 0 rail of the analogue supply.

## MIDI operation

The controller responds to note-on and note-off commands as usual. The DAC values are selected from a
table using the note number as index. Note-on commands set the DAC values, followed by the gate output
after a short delay to allow the DAC to settle.

Note-off commands turn off the gate but don't change the DAC output.

In addition to the note-on and -off messages, some other messages are accepted:
* Continuous controller 0 - set ADC coarse to given value
* Continuous controller 1 - set ADC coarse to given value + 128
* Continuous controller 2 - set ADC fine to given value
* Continuous controller 3 - set ADC fine to given value + 128
* Continuous controller 4 - set gate on or off depending on least significant bit of value

## MIDI channel selection

To be defined. Push-button and 7-segment display? Or 2x16 character LCD/VFD?

## Tuning

To be defined.

Manual: measure the VCO frequency, set ADC values using continuous controller 0..3. Push a button
to program the current settings into the current note (last note played)

External: build another arduino box that does the above automatically. Add another MIDI command to store
the setting.

Automatic: use this arduino to measure the frequency. This might mean using Timer2 for the PWM instead
of Timer0, because Timer0 has the option of an external clock. The procedure would be similar to the
external tuning (above) but done by the synth controller itself at the push of a button.



