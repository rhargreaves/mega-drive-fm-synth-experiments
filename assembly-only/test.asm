	include rom-header.asm
	include md-init.asm

Main:
	jmp __main ; Jump to the game code!
 
HBlankInterrupt:
VBlankInterrupt:
	rte   ; Return from Exception
 
Exception:
	rte   ; Return from Exception

__main:
	Loop:
	move.l #0xF, d0 ; Move 15 into register d0
	move.l d0, d1   ; Move contents of register d0 into d1
	jmp Loop        ; Jump back up to 'Loop'

__end    ; Very last line, end of ROM address
