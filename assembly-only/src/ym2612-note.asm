	include inc/rom-header.asm
	include inc/md-init.asm

VDPCtrlPort	equ	0x00C00004
VDPDataPort	equ	0x00C00000

Fm1AddrPort	equ	0x00A04000
Fm1DataPort	equ	0x00A04001
Fm2AddrPort	equ	0x00A04002
Fm2DataPort	equ	0x00A04003

Main:
	jmp __main ; Jump to the game code!

SizeByte:       equ 0x01
SizeWord:       equ 0x02
SizeLong:       equ 0x04
SizeSpriteDesc: equ 0x08
SizeTile:       equ 0x20
SizePalette:    equ 0x40

hblank_counter	equ 0x00FF0000          ; Start of RAM
vblank_counter	equ (hblank_counter+SizeLong)
audio_clock	equ (vblank_counter+SizeLong)

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

HBlankInterrupt:
	addi.l	#0x1, hblank_counter	; Increment hinterrupt counter
	rte

VBlankInterrupt:
	addi.l	#0x1, vblank_counter	; Increment vinterrupt counter
	addi.l	#0x1, audio_clock	; Increment audio clock
	rte

Exception:
	stop	#$2700		; Halt CPU

WaitVBlankStart:
	move.w  VDPCtrlPort, d0	; Move VDP status word to d0
	andi.w  #0x0008, d0     ; AND with bit 4 (vblank), result in status register
	bne     WaitVBlankStart ; Branch if not equal (to zero)
	rts

WaitVBlankEnd:
	move.w  VDPCtrlPort, d0	; Move VDP status word to d0
	andi.w  #0x0008, d0     ; AND with bit 4 (vblank), result in status register
	beq     WaitVBlankEnd   ; Branch if equal (to zero)
	rts

WaitFrames:
	; d0 - Number of frames to wait
	move.l  vblank_counter, d1 ; Get start vblank count
	@Wait:
	move.l  vblank_counter, d2 ; Get end vblank count
	subx.l  d1, d2             ; Calc delta, result in d2
	cmp.l   d0, d2             ; Compare with num frames
	bge     @End               ; Branch to end if greater or equal to num frames
	jmp     @Wait              ; Try again
	
	@End:
	rts

SetBackground:
	move.l 	#0x40000003, VDPCtrlPort ; we’re about to write data to VRAM address 0xC000
	move.w 	#0x8F02, VDPCtrlPort  	; Set autoincrement to 2 bytes
	move.l 	#0xC0000003, VDPCtrlPort ; Set up VDP to write to CRAM address 0x0000
	lea 	Palette, a0          	; Load address of Palette into a0
	move.l 	#0x07, d0         	; 32 bytes of data (8 longwords, minus 1 for counter) in palette
	@Loop:
	move.l 	(a0)+, 0x00C00000 	; Move data to VDP data port, and increment source address
	dbra 	d0, @Loop
	move.w 	#0x8707, VDPCtrlPort  	; Set background colour to palette 0, colour 8
	rts

SuspendZ80:
	move.w 	#0x0100, 0x00A11100 ; Request access to the Z80 bus, by writing 0x0100 into the BUSREQ port
	move.w 	#0x0100, 0x00A11200 ; Hold the Z80 in a reset state, by writing 0x0100 into the RESET port
	rts

WaitFmReady:
	move.b  Fm1AddrPort, d0		; Read FM status into d2
	btst	#7, d0			; Test for bit 8 (FM busy)
	bne     WaitFmReady 		; Loop if busy
	rts

WriteFm1	macro	Reg, Data
		jsr	WaitFmReady
		move.b	\Reg, Fm1AddrPort
		move.b	\Data, Fm1DataPort
		endm

PlayYm2612Note:
	jsr	SuspendZ80
	WriteFm1	#0x22, #%00000000	; LFO Off
	WriteFm1	#0x27, #0		; Ch 3 Normal
	WriteFm1	#0x28, #0		; All channels off
	WriteFm1	#0x28, #1
	WriteFm1	#0x28, #2
	WriteFm1	#0x28, #4
	WriteFm1	#0x28, #5
	WriteFm1	#0x28, #6
	WriteFm1	#0x30, #0x71	; DT1/MUL
	WriteFm1	#0x34, #0x0D
	WriteFm1	#0x38, #0x33
	WriteFm1	#0x3C, #0x01
	WriteFm1	#0x40, #0x23	; Total Level
	WriteFm1	#0x44, #0x2D
	WriteFm1	#0x48, #0x26
	WriteFm1	#0x4C, #0x00
	WriteFm1	#0x50, #0x5F	; RS/AR
	WriteFm1	#0x54, #0x99
	WriteFm1	#0x58, #0x5F
	WriteFm1	#0x5C, #0x99
	WriteFm1	#0x60, #5		; AM/D1R
	WriteFm1	#0x64, #5
	WriteFm1	#0x68, #5
	WriteFm1	#0x6C, #7
	WriteFm1	#0x70, #2		; D2R
	WriteFm1	#0x74, #2
	WriteFm1	#0x78, #2
	WriteFm1	#0x7C, #2
	WriteFm1	#0x80, #0x11	; D1L/RR
	WriteFm1	#0x84, #0x11
	WriteFm1	#0x88, #0x11
	WriteFm1	#0x8C, #0xA6
	WriteFm1	#0x90, #0		; Proprietary
	WriteFm1	#0x94, #0
	WriteFm1	#0x98, #0
	WriteFm1	#0x9C, #0
	WriteFm1	#0xB0, #0x32	; Feedback/algorithm
	WriteFm1	#0xB4, #0xC0	; Both speakers on
	WriteFm1	#0x28, #0x00	; Key off
	WriteFm1	#0xA4, #0x22	; Set Freq
	WriteFm1	#0xA0, #0x69
	WriteFm1	#0x28, #0xF0	; Key On
	rts

__main:
	move.w	#0x8F02, VDPCtrlPort     ; Set autoincrement to 2 bytes
	move.w	#0x8708, VDPCtrlPort     ; Set background colour light blue (palette 0, colour 8)
	jsr	SetBackground
	jsr	PlayYm2612Note

GameLoop:
	jsr	WaitVBlankStart		; Wait for start of vblank
	jsr     WaitVBlankEnd		; Wait for end of vblank
	jmp     GameLoop		; Back to the top

__end
