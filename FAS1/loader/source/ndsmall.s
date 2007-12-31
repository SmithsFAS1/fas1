@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@
@ NDSmall
@   by Rafael Vuijk (aka DarkFader)
@
@ Small NDS loader for PassMe/WifiMe/FlashMe
@
@ Usage:
@   cat loader.bin game.nds > game.ds.gba 
@
@ Todo:
@	Add filesystem/storage device driver
@	Add firmware-like features and put in firmware
@
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

	.equ	save_REGS,				0x027FF000
	.equ	NewARM9Loop_dest,		0x027FF100
	.equ	RAM_HEADER,				0x027FFE00
	.equ	NDSROM_HEADER,			0x08000200

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

	.global	_start
_start:
	b		start0
	.ds.b	12
	.byte	0x0D,0x0A,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
	.ascii	" NDS loader for "
	.ascii	"GBA flashcards. "
	.ascii	"v1.1.           "
	.byte	0x0D,0x0A,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
	.ascii	"cat loader.bin  "
	.ascii	"game.nds >      "
	.ascii	"game.ds.gba     "
	.byte	0x0D,0x0A,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x1A
	.ascii	"NDSLoader1.1"	@ title (short)
	.ascii	"PASS"	@ gamecode (for FlashMe)
	.ascii	"DF"	@ maker code
	.byte	0x96	@ fixed
	.ds.b	13
start0:

	@ save registers
	ldr		r14, =save_REGS
	stmia	r14, {r0-r11,r13}
	
	@ commonly used addresses
	ldr		r11, =NDSROM_HEADER
	ldr		r12, =RAM_HEADER

	@ check if running on gba
	ldr		r0, =0x04000136		@ if X_KEYS == 0
	ldrb	r0, [r0]		@ Read the byte
	cmp		r0, #0x00
	beq		gba_mode

	@ clear RAM
	mov		r0, #0
	mov		r1, #0
	mov		r2, #0
	mov		r3, #0
	mov		r4, #0
	mov		r5, #0
	mov		r6, #0
	mov		r7, #0
	ldr		r8, =0x02000000
	ldr		r9, =0x023F0000
1:
	stmia	r8!, {r0-r7}
	cmp		r8, r9
	bne		1b

	@ copy new ARM9 loop
	ldr		r0, NewARM9Loop
	ldr		r4, =NewARM9Loop_dest
	str		r0, [r4, #0]				@ place ldr instruction
	str		r4, [r4, #4]				@ address of ldr instruction
	str		r4, [r12, #0x24]			@ go to new loop

	@ copy header
	mov		r0, r11
	mov		r1, r12
	mov		r2, #0x170
	bl		Copy

	@ disable encryption (for debugging device for example)
	ldr		r4, =0x040001B0
	ldr		r0, =0
	str		r0, [r4, #0]
	str		r0, [r4, #4]
	str		r0, [r4, #8]

	@ copy ARM9 binary
	ldr		r0, [r11, #0x20]				@ ROM offset
	add		r0, r0, r11
	ldr		r1, [r11, #0x28]				@ RAM address
	ldr		r2, [r11, #0x2C]				@ code size
	bl		CopyAlign

	@ copy ARM7 binary
	ldr		r0, [r11, #0x30]				@ ROM offset
	add		r0, r0, r11
	ldr		r1, [r11, #0x38]				@ RAM address
	ldr		r2, [r11, #0x3C]				@ code size
	bl		Copy

	@ get ARM9/7 entry
	ldr		r0, [r11, #0x24]
	ldr		r12, [r11, #0x34]

	@ start ARM9
	ldr		r4, =NewARM9Loop_dest
	str		r0, [r4, #0x4]

	@ restore registers
	ldr		r14, =save_REGS
	ldmia	r14, {r0-r11,r13}

	mov		pc, r12

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

gba_mode:
	ldr		r0, [r11, #0x80]	@ nds rom size
	add		r0, r0, r11			@ gba rom is appended after nds rom
	ldr		r1, =0x2000000
	ldr		r2, =0x40000
	bl		Copy

	ldr		r0, =0x2000000
	bx		r0					@ jump to the gba multiboot rom

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

NewARM9Loop:
	ldr		pc, . + 4

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

@ copy [r0+] to [r1+]; length r2

CopyAlign:
	mov		r3, #0x1FC
	add		r2, r2, r3
	bic		r2, r2, r3
Copy:
	ldr		r3, [r0], #4
	str		r3, [r1], #4
	subs	r2, r2, #4
	bgt		Copy
	mov		pc, lr

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

	.pool

	.space (_start + 0x200) - .

	.end

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
