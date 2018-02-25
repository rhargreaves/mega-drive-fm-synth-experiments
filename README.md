# Sega Mega Drive Experiments

Various experiments mostly centred around the Yamaha YM2612 FM Synthesis & PSG chips.

## Table of Contents

1. Experiments in [68000 Assembly](assembly-only)
2. Experiments in C using SGDK (this directory).

## Getting Started

Compiling from source:

### Docker (macOS/Linux)

```sh
./docker-make.sh
```

### Linux

1. Clone and make [gendev](https://github.com/kubilus1/gendev).
2. Run `make`

## Run

### Emulated (Regen via Wine)

```sh
./run-regen.sh bin/out.bin
```

### Actual Mega Drive (via Everdrive USB link)

First install the `megaedx7-run` tool (requires Golang):

1. `git clone git@github.com:rhargreaves/devkit-mega-everdrive-x7.git`
2. `make install`

Upload and execute the ROM on the Mega Drive:

```sh
./run-everdrive.sh bin/out.bin
```
