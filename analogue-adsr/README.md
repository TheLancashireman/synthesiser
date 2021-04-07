# ADSR envelope generator using ATTiny microcontroller

## Pinout

* Pin 1 = ADC0 = Attack rate
* Pin 2 = ADC3 = Release rate
* Pin 3 = ADC2 = Sustain level
* Pin 4 = GND
* Pin 5 = OC0A - control voltage output (CV)
* Pin 6 = PB1 - gate input
* Pin 7 = ADC1 = Decay rate
* Pin 8 = VCC

## Reset pin

Pin 1 of the ATtiny is the reset pin.

You have to disable the reset pin in order to get the full range of the release control.
If you don't disable the reset pin, turning the release control towards minimum resets the MPU. The data
sheet only guarantees that Vcc * 0.9 will hold reset high, but in practice the level at which the
device resets is much lower, so it's possible to test the circuit and software without disabling reset.

After you have disabled the reset pin, you can no longer program the flash with a normal programmer -
you have to use a "high voltage programmer" which applies 12V to the reset pin and uses special commands
method to reset the fuses.

## Operation

When Gate input changes from OFF to ON

* Attack phase  : ramp up CV until maximum reached
* Decay phase   : ramp down CV until sustain level reached
* Sustain phase : keep CV at sustain level until gate changes to OFF
* Release phase : ramp down CV until 0

If Gate changes from ON to OFF during attack or decay phase, switch immediately to release phase.
Ramp down from level reached.

If Gate changes from OFF to ON during release phase, force CV to 0 and start attack phase.

## Circuit diagram

https://wiki.thelancashireman.org/index.php?title=Digital_envelope_generator
