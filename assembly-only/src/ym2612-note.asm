	include inc/rom-header.asm
	include inc/md-init.asm

VDPCtrlPort	equ	0x00C00004
VDPDataPort	equ	0x00C00000

Fm1AddrPort	equ	0x00A04000
Fm1DataPort	equ	0x00A04001
Fm2AddrPort	equ	0x00A04002
Fm2DataPort	equ	0x00A04003

FmLFOReg	equ	0x22
FmChModeReg	equ	0x27
FmKeyReg	equ	0x28

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

WaitFmReady:
	move.b  Fm1AddrPort, d0	; Read FM status into d2
	btst	#7, d0		; Test for bit 8 (FM busy)
	bne     WaitFmReady 	; Loop if busy
	rts

WriteFm1	macro	Reg, Data
		jsr	WaitFmReady
		move.b	\Reg, Fm1AddrPort
		move.b	\Data, Fm1DataPort
		endm

PlayYm2612Note:
	WriteFm1	FmLFOReg, #%00001000	; LFO On
	WriteFm1	FmChModeReg, #%00000000	; Ch 3 Normal
	WriteFm1	FmKeyReg, #0xF0		; Key On
	WriteFm1	0x28, 0			; All channels off
	WriteFm1	0x28, 1
	WriteFm1	0x28, 2
	WriteFm1	0x28, 4
	WriteFm1	0x28, 5
	WriteFm1	0x28, 6
	WriteFm1	0x30, 0x71	; DT1/MUL
	WriteFm1	0x34, 0x0D
	WriteFm1	0x38, 0x33
	WriteFm1	0x3C, 0x01
	WriteFm1	0x40, 0x23	; Total Level
	WriteFm1	0x44, 0x2D
	WriteFm1	0x48, 0x26
	WriteFm1	0x4C, 0x00
	WriteFm1	0x50, 0x5F	; RS/AR
	WriteFm1	0x54, 0x99
	WriteFm1	0x58, 0x5F
	WriteFm1	0x5C, 0x99
	WriteFm1	0x60, 5		; AM/D1R
	WriteFm1	0x64, 5
	WriteFm1	0x68, 5
	WriteFm1	0x6C, 7
	WriteFm1	0x70, 2		; D2R
	WriteFm1	0x74, 2
	WriteFm1	0x78, 2
	WriteFm1	0x7C, 2
	WriteFm1	0x80, 0x11	; D1L/RR
	WriteFm1	0x84, 0x11
	WriteFm1	0x88, 0x11
	WriteFm1	0x8C, 0xA6
	WriteFm1	0x90, 0		; Proprietary
	WriteFm1	0x94, 0
	WriteFm1	0x98, 0
	WriteFm1	0x9C, 0
	WriteFm1	0xB0, 0x32	; Feedback/algorithm
	WriteFm1	0xB4, 0xC0	; Both speakers on
	WriteFm1	0x28, 0x00	; Key off
	WriteFm1	0xA4, 0x22	; Set Freq
	WriteFm1	0xA0, 0x69
	WriteFm1	0x28, 0xF0	; Key On
	rts

__main:
	move.w	#0x8F02, VDPCtrlPort     ; Set autoincrement to 2 bytes
	move.w	#0x8708, VDPCtrlPort     ; Set background colour light blue (palette 0, colour 8)
	jsr	PlayYm2612Note

GameLoop:
	jsr	WaitVBlankStart		; Wait for start of vblank
	jsr     WaitVBlankEnd		; Wait for end of vblank
	jmp     GameLoop		; Back to the top

__end
