#pragma once

#include <stdint.h>
#include <stddef.h>

#ifndef __cplusplus
#ifndef static_assert
#define static_assert _Static_assert
#endif
#endif

typedef uint8_t PADBYTES;

// All memory is natively 16bit access; cast to 8bit or 32bit if you need to
struct vdp {
	// Video memory
	union {
		uint16_t BITMAP_VRAM[0x10000];
		uint8_t BITMAP_VRAM_8BIT[0x20000];
	};
	PADBYTES _pad_20000[0x20000];
	uint16_t TILE_VRAM[0x8000];
	uint32_t OAM[0x80];
	PADBYTES _pad_50200[0xE00];
	uint16_t PALETTE[0x100];
	PADBYTES _pad_51200[0xE00];
	union {
		uint16_t CAPTURE_DATA[0x100];
		uint8_t CAPTURE_DATA_8BIT[0x200];
	};
	PADBYTES _pad_52200[0x5E00];
	// General registers
	uint16_t MODE;
	uint16_t HCOUNT;
	uint16_t VCOUNT;
	uint16_t TRIGGER;
	uint16_t SYNC_IRQ_CTRL;
	PADBYTES _pad_5800A[0xFF6];
	// Bitmap registers
	uint16_t BM_SCROLLX[4];
	uint16_t BM_SCROLLY[4];
	uint16_t BM_SCREENX[4];
	uint16_t BM_SCREENY[4];
	uint16_t BM_WIDTH[4];
	uint16_t BM_HEIGHT[4];
	uint16_t BM_CTRL;
	PADBYTES _pad_59032[0xE];
	uint16_t BM_SUBPAL;
	PADBYTES _pad_59042[0xE];
	uint16_t BM_COL_LATCH[4];
	PADBYTES _pad_59058[0xFA8];
	// Background and object registers
	uint16_t BG_CTRL;
	uint16_t BG_SCROLL[4];
	uint16_t BG_SUBPAL[2];
	PADBYTES _pad_5A00E[0x2];
	uint16_t OBJ_CTRL;
	uint16_t OBJ_SUBPAL[0x2];
	PADBYTES _pad_5A016[0xA];
	uint16_t CHARBASE;
	PADBYTES _pad_5A022[0xFDE];
	// Display registers
	uint16_t BLEND;
	uint16_t LAYER_CTRL;
	uint16_t SCREENPRIO;
	uint16_t BACKDROP_B;
	uint16_t BACKDROP_A;
	uint16_t CAPTURE_CTRL;
	PADBYTES _pad_5B00C[0xFF4];
	// IRQ0 registers
	uint16_t IRQ0_NMI_CTRL;
	uint16_t IRQ0_HCMP;
	uint16_t IRQ0_VCMP;
	PADBYTES _pad_5C006[0xFFA];
	// Printer and controller MMIO registers (should these be separated?)
	uint16_t IO_PRINT_TEMP;
	PADBYTES _pad_5D002[0xE];
	uint16_t IO_GAMEPAD[3];
	PADBYTES _pad_5D016[0xA];
	uint16_t UNK_5D020;
	PADBYTES _pad_5D022[0xE];
	uint16_t IO_SENSOR;
	PADBYTES _pad_5D032[0xE];
	uint16_t UNK_5D040;
	uint16_t UNK_5D042;
	uint16_t UNK_5D044;
	PADBYTES _pad_5D046[0xA];
	uint16_t IO_MOUSEX;
	uint16_t IO_MOUSEY;
	uint16_t UNK_5D054;
	PADBYTES _pad_5D056[0xFAA];
	// Bitmap fill registers
	uint16_t UNK_5E000;
	uint16_t BM_FILL_MASK;
	uint16_t BM_FILL_VALUE;
	PADBYTES _pad_5E006[0xFFA];
	uint16_t BM_FILL_TRIGGER[0x200];
	PADBYTES _pad_5F400[0xC00];
	// Misc registers (should these be separated?)
	uint16_t UNK_60000;
	PADBYTES _pad_60002[0x1FFFE];
	uint16_t SOUND_CTRL;
	PADBYTES _pad_80002[0x1FFFE];
	uint16_t SOUND_EXP_DATA;
};

// Sanity check some key addresses
static_assert((offsetof (struct vdp, BITMAP_VRAM))     == 0x00000);
static_assert((offsetof (struct vdp, TILE_VRAM))       == 0x40000);
static_assert((offsetof (struct vdp, OAM))             == 0x50000);
static_assert((offsetof (struct vdp, PALETTE))         == 0x51000);
static_assert((offsetof (struct vdp, CAPTURE_DATA))    == 0x52000);
static_assert((offsetof (struct vdp, MODE))            == 0x58000);
static_assert((offsetof (struct vdp, BM_SCROLLX))      == 0x59000);
static_assert((offsetof (struct vdp, BG_CTRL))         == 0x5A000);
static_assert((offsetof (struct vdp, BLEND))           == 0x5B000);
static_assert((offsetof (struct vdp, IRQ0_NMI_CTRL))   == 0x5C000);
static_assert((offsetof (struct vdp, IO_PRINT_TEMP))   == 0x5D000);
static_assert((offsetof (struct vdp, UNK_5E000))       == 0x5E000);
static_assert((offsetof (struct vdp, BM_FILL_TRIGGER)) == 0x5F000);
static_assert((offsetof (struct vdp, UNK_60000))       == 0x60000);
static_assert((offsetof (struct vdp, SOUND_CTRL))      == 0x80000);
static_assert((offsetof (struct vdp, SOUND_EXP_DATA))  == 0xA0000);

extern struct vdp VDP;
