; Modified version os serial.s from GBDK 2020 4.0
;
; Does not include default serial_IO handler that is not
; possible to remove with stock GBDK 2020 4.0
;
; Standard GBDK 4.0 Serial Link / SIO handler will not be
; linked unless any of it's functions are called/used.


	.globl	.int

	.area	_HEADER_SIO (ABS)

	.org	0x58		; SIO (Install SIO handler at interrupt address)
.custom_int_SIO:
	PUSH	AF
	PUSH	HL
	LD	HL,#.custom_int_0x58   ; Load SIO chained interrupt table
	JP	.int            ; Call main interrupt handler


	.area   _GSINIT

	;; Clear SIO global variables
	XOR A
	LD  HL,#.start_custom_sio_globals
	LD  C,#(.end_custom_sio_globals - .start_custom_sio_globals)
	RST 0x28


	.area	_BASE

_custom_add_SIO::       ; Add an entry to the SIO chained interrupt table
	PUSH	BC
	LDA	HL,4(SP)	    ; Skip return address and registers
	LD	C,(HL)
	INC	HL
	LD	B,(HL)
	CALL	.custom_add_SIO
	POP	BC
	RET

.custom_add_SIO::
	LD	HL,#.custom_int_0x58 ; Load SIO chained interrupt table
	JP	.add_int

_custom_remove_SIO::  ; Remove an entry to the SIO chained interrupt table
	PUSH	BC
	LDA	HL,4(SP)	  ; Skip return address and registers
	LD	C,(HL)
	INC	HL
	LD	B,(HL)
	CALL	.custom_remove_SIO
	POP	BC
	RET

.custom_remove_SIO::
	LD	HL,#.custom_int_0x58 ; Load SIO chained interrupt table
	JP	.remove_int


	.area	_BSS

.start_custom_sio_globals:

.custom_int_0x58:: ; SIO chained interrupt table
	.blkw	0x08

.end_custom_sio_globals:



