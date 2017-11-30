all: clean test.bin

clean:
	-rm *.bin

test.bin: test.asm
	docker run --rm -v $(PWD):/src rhargreaves/asm68k /p $<,$@

.PHONY: all clean
