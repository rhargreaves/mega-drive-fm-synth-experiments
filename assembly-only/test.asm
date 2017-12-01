	include rom-header.asm
	include md-init.asm

Main:
	jmp __main ; Jump to the game code!
 
HBlankInterrupt:
VBlankInterrupt:
	rte   ; Return from Exception
 
Exception:
	rte   ; Return from Exception

PSGData:
	dc.w 0x9fbf, 0xdfff

Z80Data:
	dc.w 0xaf01, 0xd91f
	dc.w 0x1127, 0x0021
	dc.w 0x2600, 0xf977
	dc.w 0xedb0, 0xdde1
	dc.w 0xfde1, 0xed47
	dc.w 0xed4f, 0xd1e1
	dc.w 0xf108, 0xd9c1
	dc.w 0xd1e1, 0xf1f9
	dc.w 0xf3ed, 0x5636
	dc.w 0xe9e9, 0x8104
	dc.w 0x8f01

VDPRegisters:
	dc.b 0x20 ; 0: Horiz. interrupt on, plus bit 2 (unknown, but docs say it needs to be on)
	dc.b 0x74 ; 1: Vert. interrupt on, display on, DMA on, V28 mode (28 cells vertically), + bit 2
	dc.b 0x30 ; 2: Pattern table for Scroll Plane A at 0xC000 (bits 3-5)
	dc.b 0x40 ; 3: Pattern table for Window Plane at 0x10000 (bits 1-5)
	dc.b 0x05 ; 4: Pattern table for Scroll Plane B at 0xA000 (bits 0-2)
	dc.b 0x70 ; 5: Sprite table at 0xE000 (bits 0-6)
	dc.b 0x00 ; 6: Unused
	dc.b 0x00 ; 7: Background colour - bits 0-3 = colour, bits 4-5 = palette
	dc.b 0x00 ; 8: Unused
	dc.b 0x00 ; 9: Unused
	dc.b 0x00 ; 10: Frequency of Horiz. interrupt in Rasters (number of lines travelled by the beam)
	dc.b 0x08 ; 11: External interrupts on, V/H scrolling on
	dc.b 0x81 ; 12: Shadows and highlights off, interlace off, H40 mode (40 cells horizontally)
	dc.b 0x34 ; 13: Horiz. scroll table at 0xD000 (bits 0-5)
	dc.b 0x00 ; 14: Unused
	dc.b 0x00 ; 15: Autoincrement off
	dc.b 0x01 ; 16: Vert. scroll 32, Horiz. scroll 64
	dc.b 0x00 ; 17: Window Plane X pos 0 left (pos in bits 0-4, left/right in bit 7)
	dc.b 0x00 ; 18: Window Plane Y pos 0 up (pos in bits 0-4, up/down in bit 7)
	dc.b 0x00 ; 19: DMA length lo byte
	dc.b 0x00 ; 20: DMA length hi byte
	dc.b 0x00 ; 21: DMA source address lo byte
	dc.b 0x00 ; 22: DMA source address mid byte
	dc.b 0x00 ; 23: DMA source address hi byte, memory-to-VRAM mode (bits 6-7)

__main:
	Loop:
	move.l #0xF, d0 ; Move 15 into register d0
	move.l d0, d1   ; Move contents of register d0 into d1
	jmp Loop        ; Jump back up to 'Loop'

__end    ; Very last line, end of ROM address
