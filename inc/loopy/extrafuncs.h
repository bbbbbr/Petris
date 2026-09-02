#pragma once

#include <stdint.h>
#include <stdbool.h>

#include "sh7021.h"
#include "loopy/constants.h"
#include "loopy/bios.h"

// Inline functions should nicely optimize to minimal code, especially when
// given constant parameters, if optimization is turned on which it should be.

static inline void sys_setInterruptMask(int priorityMask) {
	uint32_t sr;
	priorityMask &= 0xF;
	priorityMask <<= 4;
	__asm__ __volatile__("STC SR,%0\n\t" : "=r" (sr) : : );
	sr &= 0xFFFFFF0F;
	sr |= priorityMask;
	__asm__ __volatile__("LDC %0,SR\n\t" : : "r" (sr) : );
}

static inline void sys_setInterruptPriority(int select, int priority) {
	// IPRx registers are contiguous so we can offset like this
	volatile uint16_t *reg = (&INTC_IPRA) + (select >> 4);
	int shift = select & 0xF;
	*reg = (*reg & ~(0xF << shift)) | ((priority & 0xF) << shift);
}

static inline void sys_setDmaEnabled(bool enable) {
	if (enable)
		DMAC_DMAOR |=  0x0001;
	else
		DMAC_DMAOR &= ~0x0001;
}

static inline void sys_stopBgm(struct soundstate_t *soundState) {
	bios_playBgm(soundState, 0x80, 0, &biosdata_stopBgmTrackList);
}

static inline bool sys_bgmRunning() {
	return (ITU_TSTR & 1) != 0;
}
