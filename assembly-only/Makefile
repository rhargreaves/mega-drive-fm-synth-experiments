ASMS = $(wildcard *.asm)
BINS = $(patsubst %.asm,%.bin,$(wildcard *.asm))

all: clean $(BINS)

clean: 
	-rm *.bin

$(BINS): $(ASMS)
	docker run --rm -v $(PWD):/src rhargreaves/asm68k /p $<,$@

.PHONY: all clean
