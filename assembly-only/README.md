# Sega Mega Drive Assembly Programming

Flipping bits the hard way

## What's here?

Pure 68000 assembly examples of:

1. [Play a chord with the PSG](src/psg-test.asm)
2. [Playing a tune with the PSG](src/psg-sequencer.asm)
3. [Playing a note with the YM2612 FM Synth Chip](src/ym2612-note.asm)

## Dependencies

* macOS or Linux
* Docker

## Compiling Code

Use `make` to build all .asm files into .bin ROM files, ready to be ran on an emulator or Sega Mega Drive. The ASM68k assembler is used (via Docker) to build the ROMs.

## Running

### Via Emulation

Regen is an emulator that has support for 68k/Z80 debugging. Use the [run-regen](../run-regen) script to load a ROM with the emulator using Wine. The script presumes a particular location for Regen which you may need to adjust to suit your needs.

### Via Hardware

Use the [copy-to-everdrive](../copy-to-everdrive) script (macOS only) to copy the .bin files to an everdrive SD card in preparation to run on a real Sega Mega Drive.

## Credits

* A lot of code is based on examples posted to [BIG EVIL CORPORATION's blog](https://bigevilcorporation.co.uk/2012/09/03/sega-megadrive-10-sound-part-i-the-psg-chip/)

