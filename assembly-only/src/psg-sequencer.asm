	include inc/rom-header.asm
	include inc/md-init.asm

VDPCtrlPort	equ	0x00C00004
VDPDataPort	equ	0x00C00000
PSGCtrlPort	equ	0x00C00011

Main:
	jmp __main ; Jump to the game code!
 
; **********************************************
; Various size-ofs to make this easier/foolproof
; **********************************************
SizeByte:       equ 0x01
SizeWord:       equ 0x02
SizeLong:       equ 0x04
SizeSpriteDesc: equ 0x08
SizeTile:       equ 0x20
SizePalette:    equ 0x40

; ************************************
; System stuff
; ************************************
hblank_counter		equ 0x00FF0000                ; Start of RAM
vblank_counter		equ (hblank_counter+SizeLong)
audio_clock		equ (vblank_counter+SizeLong)

HBlankInterrupt:
   addi.l #0x1, hblank_counter    ; Increment hinterrupt counter
   rte ; Return from Exception

VBlankInterrupt:
   addi.l #0x1, vblank_counter    ; Increment vinterrupt counter
   addi.l #0x1, audio_clock       ; Increment audio clock
   rte ; Return from Exception

Exception:
   stop #$2700 ; Halt CPU

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

SoundTest:
	; Bit 7   : Latch (1 = indicates this is the first (or only) byte of data)
	; Bit 6-5 : Channel (0-3)
	; Bit 4   : Attenuation data (1) or tone/noise data (0)
	; Bit 3-0 : Low 4 bits of data to write to register
	
	; If data is more than 4 bits, send another byte...
	; Bit 7   : Latch (0 = indicates this is the second byte of data)
	; Bit 6   : Unused
	; Bit 5-0 : High 6 bits of data to write to register
	
	move.b #%10010000, PSGCtrlPort ; Channel 0 full volume
	
	lea chan0_notes, a0            ; Notes to a0
	move.l #chan0_notes_count, d1  ; Number of notes to d1
	subi.l #0x1, d1                ; -1 for counter
	
	@NextNote:
	
	move.w (a0)+, d0       ; Delay to d0 and inc. pointer
	move.w (a0)+, d2       ; Counter reset to d2 and inc. pointer
	move.b d2, d3          ; Lower byte of counter reset to d3
	and.b #%00001111, d3   ; Clear top nybble (leave lower 4 bits)
	or.b #%10000000, d3    ; Latch bit (7) on, chan 0 (6-5), tone data bit (4) off
	move.b d3, PSGCtrlPort ; Write to PSG port
	
	move.w d2, d3          ; Counter reset to d3 again
	ror.w #0x4, d3         ; Shift right 4 bits
	and.b #%00111111, d3   ; Only need bits 5-0 (upper 6 bits of the 10 bit value)
	move.b d3, PSGCtrlPort ; Write to PSG port
	
	move.l d1, -(sp)       ; Backup d1
	jsr WaitFrames         ; Delay frames is in d0
	move.l (sp)+, d1       ; Restore d1
	
	dbra d1, @NextNote
	
	move.b #%10011111, PSGCtrlPort ; Silence channel 0

	rts

chan0_notes:
	dc.w 0x0010, 0x02f8, 0x0010, 0x02cd, 0x0010, 0x02a5, 0x0010, 0x01aa, 0x0008, 0x0000                                                 ; D3 D#3 E3 C4 .
	dc.w 0x0010, 0x02a5, 0x0010, 0x01aa, 0x0008, 0x0000                                                                                 ; E3 C4 .
	dc.w 0x0010, 0x02a5, 0x0010, 0x01aa, 0x0010, 0x0000                                                                                 ; E3 C4 .
	dc.w 0x0010, 0x01aa, 0x0010, 0x0193, 0x0010, 0x017c, 0x0010, 0x0152, 0x0010, 0x01aa, 0x0010, 0x017c, 0x0010, 0x0152, 0x0008, 0x0000 ; C4 C#4 D4 E4 C4 D4 E4 .
	dc.w 0x0010, 0x01c4, 0x0010, 0x017c, 0x0008, 0x0000                                                                                 ; B3 D4 .
	dc.w 0x0010, 0x01aa                                                                                                                 ; C4
	
chan0_notes_end
chan0_notes_len equ chan0_notes_end-chan0_notes
chan0_notes_count equ chan0_notes_len/4
	
__main:
	move.w #0x8F02, VDPCtrlPort     ; Set autoincrement to 2 bytes
	move.w #0x8708, VDPCtrlPort     ; Set background colour light blue (palette 0, colour 8)
	
	jsr SoundTest

	; ************************************
	; Main game loop
	; ************************************
GameLoop:

	; ************************************
	; Update scrolling during vblank
	; ************************************

	jsr WaitVBlankStart   ; Wait for start of vblank

	jsr     WaitVBlankEnd ; Wait for end of vblank
	jmp     GameLoop      ; Back to the top


__end    ; Very last line, end of ROM address
