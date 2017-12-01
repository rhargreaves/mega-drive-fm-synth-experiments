EntryPoint:          ; Entry point address set in ROM header

; 1. CHECKING THE RESET BUTTON
CheckReset:
	tst.w 0x00A10008  ; Test mystery reset (expansion port reset?)
	bne Main          ; Branch if Not Equal (to zero) - to Main
	tst.w 0x00A1000C  ; Test reset button
	bne Main          ; Branch if Not Equal (to zero) - to Main

; 2. CLEARING THE RAM (lol)
ClearMem:
	move.l #0x00000000, d0     ; Place a 0 into d0, ready to copy to each longword of RAM
	move.l #0x00000000, a0     ; Starting from address 0x0, clearing backwards
	move.l #0x00003FFF, d1     ; Clearing 64k's worth of longwords (minus 1, for the loop to be correct)
	@Clear:
	move.l d0, -(a0)           ; Decrement the address by 1 longword, before moving the zero from d0 to it
	dbra d1, @Clear            ; Decrement d0, repeat until depleted

; 3. WRITING THE TMSS
WriteTmss:
	move.b 0x00A10001, d0      ; Move Megadrive hardware version to d0
	andi.b #0x0F, d0           ; The version is stored in last four bits, so mask it with 0F
	beq @Skip                  ; If version is equal to 0, skip TMSS signature
	move.l #'SEGA', 0x00A14000 ; Move the string "SEGA" to 0xA14000
	@Skip:

; 4. INITIALISING THE Z80
InitZ80:
	move.w #0x0100, 0x00A11100 ; Request access to the Z80 bus, by writing 0x0100 into the BUSREQ port
	move.w #0x0100, 0x00A11200 ; Hold the Z80 in a reset state, by writing 0x0100 into the RESET port
	@Wait:
	btst #0x0, 0x00A11100   ; Test bit 0 of A11100 to see if the 68k has access to the Z80 bus yet
	bne @Wait               ; If we don't yet have control, branch back up to Wait

	move.l #Z80Data, a0      ; Load address of data into a0
	move.l #0x00A00000, a1   ; Copy Z80 RAM address to a1
	move.l #0x29, d0         ; 42 bytes of init data (minus 1 for counter)
	@Copy:
	move.b (a0)+, (a1)+      ; Copy data, and increment the source/dest addresses
	dbra d0, @Copy
	
	move.w #0x0000, 0x00A11200 ; Release reset state
	move.w #0x0000, 0x00A11100 ; Release control of bus

; 5. INITIALISING THE PSG
InitPSG:
	move.l #PSGData, a0      ; Load address of PSG data into a0
	move.l #0x03, d0         ; 4 bytes of data
	@Copy:
	move.b (a0)+, 0x00C00011 ; Copy data to PSG RAM
	dbra d0, @Copy

; 6. INITIALISING THE VDP
InitVDP:
	move.l #VDPRegisters, a0 ; Load address of register table into a0
	move.l #0x18, d0         ; 24 registers to write
	move.l #0x00008000, d1   ; 'Set register 0' command (and clear the rest of d1 ready)

	@Copy:
	move.b (a0)+, d1         ; Move register value to lower byte of d1
	move.w d1, 0x00C00004    ; Write command and value to VDP control port
	add.w #0x0100, d1        ; Increment register #
	dbra d0, @Copy

; 7. INITIALISING THE CONTROLLER PORTS
InitCtrl:			 
	; Set IN I/O direction, interrupts off, on all ports
	move.b #0x00, 0x000A10009 ; Controller port 1 CTRL
	move.b #0x00, 0x000A1000B ; Controller port 2 CTRL
	move.b #0x00, 0x000A1000D ; EXP port CTRL

; 8. CLEARING THE REGISTERS AND TIDYING UP
ClearReg:
	move.l #0x00000000, a0    	; Move 0x0 to a0
	movem.l (a0), d0-d7/a1-a7 	; Multiple move 0 to all registers
	move #0x2700, sr	  	; Init status register (no trace, A7 is 
					; Interrupt Stack Pointer, no interrupts, clear condition code bits)
	jmp __init_done

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

__init_done