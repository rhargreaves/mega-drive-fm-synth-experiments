all: test.bin

test.bin:
	docker run --rm -v $(PWD):/data -w /data nguillaumin/vasm vasmm68k_std -Fbin -o test.bin test.asm

.PHONY: all
