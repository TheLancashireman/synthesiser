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

## Operation

When Gate input changes from OFF to ON

* Attack phase  : ramp up CV until maximum reached
* Decay phase   : ramp down CV until sustain level reached
* Sustain phase : keep CV at sustain level until gate changes to OFF
* Release phase : ramp down CV until 0

If Gate changes from ON to OFF during attack or decay phase, switch immediately to release phase.
Ramp down from level reached.

If Gate changes from OFF to ON during release phase, force CV to 0 and start attack phase.
