ASMS = $(wildcard *.asm)
BINS = $(patsubst %.asm,%.bin,$(wildcard *.asm))

all: clean $(BINS)

clean: 
	-rm *.bin

%.bin: %.asm
	docker run --rm -v $(PWD):/src rhargreaves/asm68k /p $<,$@

.PHONY: all clean
