ASMS = $(wildcard src/*.asm)
BINS = $(patsubst src/%.asm,bin/%.bin,$(wildcard src/*.asm))

all: clean $(BINS)

clean: 
	-rm bin/*.bin

bin/%.bin: src/%.asm
	docker run --rm -v $(PWD):/work -w /work rhargreaves/asm68k /j src /p $<,$@

.PHONY: all clean
