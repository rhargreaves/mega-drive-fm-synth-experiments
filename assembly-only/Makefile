all: clean test.bin

clean:
	rm *.bin | true

test.bin:
	docker run --rm -v $(PWD):/data -w /data nguillaumin/vasm vasmm68k_mot -Fbin -o test.bin test.asm

.PHONY: all clean
