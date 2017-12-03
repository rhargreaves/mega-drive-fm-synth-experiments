ASMS = $(wildcard *.asm)
BINS = $(patsubst %.asm,%.bin,$(wildcard *.asm))

all: clean $(BINS)

clean: 
	-rm *.bin

%.bin: %.asm
	docker run --rm -v $(PWD):/src rhargreaves/asm68k /p $<,$@

psgtest:
	docker run --rm -v $(PWD)/MD_PSGTest:/src rhargreaves/asm68k /p soundtest.asm,soundtest.bin

.PHONY: all clean
