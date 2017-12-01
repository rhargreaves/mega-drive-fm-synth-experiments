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
; PSG Control Port Bits:
; ----------------------
; First byte:
; 7 6 5 4 3 2 1 0
; L C C T D D D D
; Bit 7    : Latch. ON indicates this is the first (or only) byte being written
; Bits 6-5 : Channel ID (0-3)
; Bit 4    : Data type. ON if data bits contain attenuation value, OFF if they contain the square wave counter reset
; Bits 3-0 : The data. Either all 4 bits of the attenuation value, or the lower 4 bits of counter reset value

; Second byte:
; 7 6 5 4 3 2 1 0
; L X D D D D D D
; Bit 7    : Latch. OFF indicates this is the second byte, and will only contain the remainder of data
; Bit 6    : Unused
; Bits 5-0 : Upper 6 bits of data

; Counter reset value = effective frequency of sound:
; The counter reset registers are 10 bits in size, and store the 
; square wave’s time until the polarity of the output is flipped.
; 
; CRV = clock ticks / 16 (essentially the “wave width / 2”).
;
; NTSC clock frequency = 3579545 Hz:
; 440 Hz = 3579545 ÷ (2 x 16 x reg value))
; So:
; F = C / (32 x R)
; OR
; F = C / 32R
; F x R = C / 32
; R = (C / 32) / F
; where:
; F = frequency of the tone
; C = clock frequency (NTSC or PAL)
; R = register value
; So:
; For 660 Hz: R = 


Ch0:	; FE
	move.b #%10001110, PSGCtrlPort ; Latch ON, channel 0, counter data type, lower 4 bits of data
	move.b #%00001111, PSGCtrlPort ; Latch OFF, upper 6 bits of data
Ch1:	; A9
	move.b #%10101001, PSGCtrlPort ; Latch ON, channel 1, counter data type, lower 4 bits of data
	move.b #%00001010, PSGCtrlPort ; Latch OFF, upper 6 bits of data	
Ch0Vol:
	move.b #%10010000, PSGCtrlPort ; Latch OFF, channel 0, attenuation data type, 4 bits of data
Ch1Vol:
	move.b #%10110000, PSGCtrlPort ; Latch OFF, channel 0, attenuation data type, 4 bits of data	

	Loop:
	move.l #0xF, d0 ; Move 15 into register d0
	move.l d0, d1   ; Move contents of register d0 into d1
	jmp Loop        ; Jump back up to 'Loop'

__end    ; Very last line, end of ROM address
