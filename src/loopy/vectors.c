#include "serial.h"
#include "loopy/bios.h"

/*
All vector functions MUST be declared with __attribute__((interrupt_handler)) to
return properly. Not strictly necessary for halting functions but done for best
practice. Small functions may also have __attribute__((section(".smallfunc")))
to place them in the small space between vector table and the main entry point.
*/

__attribute__((interrupt_handler))
__attribute__((section(".smallfunc")))
void doNothing(void) { /*asm volatile ("nop");*/ }

__attribute__((noreturn))
__attribute__((interrupt_handler))
__attribute__((section(".smallfunc")))
void halt(void) { while(1); }
#define RESERVED halt

// Vector table laid out according to SH7021 datasheet tables 4.2, 5.3.
__attribute__((section (".vectors"), used))
void (* const vector_table[])(void) = {
/* Reset vectors */
/* Never used, BIOS has its own at address 0 */
	// Power-on reset
	RESERVED, RESERVED,
	// Manual reset
	RESERVED, RESERVED,
/* CPU interrupts */
/* Always active, except User Break masked at level 15 */
	// General illegal instruction
	halt,
	// (Reserved for system use)
	RESERVED,
	// Illegal slot instruction
	halt,
	// (Reserved for system use)
	RESERVED, RESERVED,
	// CPU address error
	halt,
	// DMA address error
	halt,
	// NMI
	doNothing,
	// User Break
	doNothing,
	// (Reserved for system use)
	RESERVED, RESERVED, RESERVED, RESERVED,
	RESERVED, RESERVED, RESERVED, RESERVED,
	RESERVED, RESERVED, RESERVED, RESERVED,
	RESERVED, RESERVED, RESERVED, RESERVED,
	RESERVED, RESERVED, RESERVED,
	// Trap instruction (user vectors)
	doNothing, doNothing, doNothing, doNothing,
	doNothing, doNothing, doNothing, doNothing,
	doNothing, doNothing, doNothing, doNothing,
	doNothing, doNothing, doNothing, doNothing,
	doNothing, doNothing, doNothing, doNothing,
	doNothing, doNothing, doNothing, doNothing,
	doNothing, doNothing, doNothing, doNothing,
	doNothing, doNothing, doNothing, doNothing,
/* IRQ and on-chip module interrupts */
/* Set nonzero, nonmasked priority in IPRA-IPRB to enable */
	// IRQ0..IRQ7
	doNothing, doNothing, doNothing, doNothing,
	doNothing, doNothing, doNothing, doNothing,
	// DMAC0..DMAC3 (DEI, Reserved)
	doNothing, RESERVED,
	doNothing, RESERVED,
	doNothing, RESERVED,
	doNothing, RESERVED,
	// ITU0..ITU4 (IMIA, IMIB, OVI, Reserved)
	bios_isrAutoUpdateBgm, doNothing, doNothing, RESERVED,
	doNothing, doNothing, doNothing, RESERVED,
	doNothing, doNothing, doNothing, RESERVED,
	doNothing, doNothing, doNothing, RESERVED,
	doNothing, doNothing, doNothing, RESERVED,
	// SCI0..SCI1 (ERI, RxI, TxI, TEI)
	serial_isrERI0, serial_isrRxI0, doNothing, doNothing,
	doNothing, doNothing, doNothing, doNothing,
	// PRT PEI
	doNothing,
	// Reserved
	RESERVED, RESERVED, RESERVED,
	// WDT
	halt,
	// REF CMI
	halt,
	// Reserved
	RESERVED, RESERVED
};
