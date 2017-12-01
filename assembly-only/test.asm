	include rom-header.asm
	include md-init.asm

Main:
	jmp __main ; Jump to the game code!
 
HBlankInterrupt:
VBlankInterrupt:
	rte   ; Return from Exception
 
Exception:
	rte   ; Return from Exception

Palette:
   dc.w 0x0000 ; Colour 0 - Transparent
   dc.w 0x000E ; Colour 1 - Red
   dc.w 0x00E0 ; Colour 2 - Green
   dc.w 0x0E00 ; Colour 3 - Blue
   dc.w 0x0000 ; Colour 4 - Black
   dc.w 0x0EEE ; Colour 5 - White
   dc.w 0x00EE ; Colour 6 - Yellow
   dc.w 0x008E ; Colour 7 - Orange
   dc.w 0x0E0E ; Colour 8 - Pink
   dc.w 0x0808 ; Colour 9 - Purple
   dc.w 0x0444 ; Colour A - Dark grey
   dc.w 0x0888 ; Colour B - Light grey
   dc.w 0x0EE0 ; Colour C - Turquoise
   dc.w 0x000A ; Colour D - Maroon
   dc.w 0x0600 ; Colour E - Navy blue
   dc.w 0x0060 ; Colour F - Dark green

__main:

VDPCtrlPort	equ	0x00C00004
VDPDataPort	equ	0x00C00000
PSGCtrlPort	equ	0x00C00011

SetPurpleBackground:
	move.l #0x40000003, VDPCtrlPort	; we’re about to write data to VRAM address 0xC000
	move.w #0x8F02, VDPCtrlPort  	; Set autoincrement to 2 bytes
	move.l #0xC0000003, VDPCtrlPort 	; Set up VDP to write to CRAM address 0x0000
	lea Palette, a0          	; Load address of Palette into a0
	move.l #0x07, d0         	; 32 bytes of data (8 longwords, minus 1 for counter) in palette
	@Loop:
	move.l (a0)+, 0x00C00000 	; Move data to VDP data port, and increment source address
	dbra d0, @Loop
	move.w #0x8707, VDPCtrlPort  	; Set background colour to palette 0, colour 8

MakeSound:
	move.b #%10001110, PSGCtrlPort ; Latch ON, channel 0, counter data type, lower 4 bits of data
	move.b #%00001111, PSGCtrlPort ; Latch OFF, upper 6 bits of data
	move.b #%10010000, PSGCtrlPort ; Latch OFF, channel 0, attenuation data type, 4 bits of data

	Loop:
	move.l #0xF, d0 ; Move 15 into register d0
	move.l d0, d1   ; Move contents of register d0 into d1
	jmp Loop        ; Jump back up to 'Loop'

__end    ; Very last line, end of ROM address
