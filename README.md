# Boutique Control

Translates Roland Boutique MIDI SYSEX control messages into normal CC messages.

## Features

* Bidirectional translation to/from MIDI CC for control from DAW.
* Optional external device control - connect an external MIDI controller directly to your Boutique without using your DAW.

## Usage

 boutique-control --list

Lists all available MIDI inputs and outputs.

 ./boutique --input=1 --output=1 --device=JX03

Connect to a JX-03 on input 1, output 1. Creates "Boutique Control" virtual MIDI inputs and outputs for connecting to DAW.

 ./boutique --input=1 --output=1 --ext-control=0 --device=JX03

Connect to a JX-03 on input 1, output 1, with an external MIDI controller connected on input 0

## Notes

* The Boutique series will only send slider/knob control messages when Chain Mode is "on". However, when Chain Mode is "on", external MIDI control is weird. However, they will receive slider/knob control messages always.
* Don't enable monitoring in the MIDI channel of your DAW, as the Boutique devices copy MIDI inputs to output and you'll get an infinite loop.

## TODO

* Make it work better

## License

GPLv3