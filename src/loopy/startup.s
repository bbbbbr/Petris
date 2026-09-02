	.section .text.startup
	.globl start
start:
	! Set up initial stack pointer
	mov.l stackaddr, r15
	mov #0, r0

	! Initialize bss, data, and ctors
	mov.l crtinitaddr, r0
	jsr @r0
	nop

	! Setup VBR
	mov.l vbraddr, r1
	ldc	r1, vbr

	! Execute main program
	mov.l mainaddr, r0
	jsr @r0
	nop

	! IF main program ever exits, halt
terminated:
	bra terminated
	nop

	.align 4
stackaddr:
	.long __stack_end
vbraddr:
	.long __vbr_link_start
crtinitaddr:
	.long _crt_init
mainaddr:
	.long _main

	.end
