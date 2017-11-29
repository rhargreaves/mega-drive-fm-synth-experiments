all: clean test.bin

clean:
	-rm *.bin

test.bin:
	docker run --rm -v $(PWD):/src rhargreaves/asm68k /p test.asm,test.bin

.PHONY: all clean
