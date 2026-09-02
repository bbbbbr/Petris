#pragma once

// Compare against GAMEPAD_BTN_x masks, when using BiosVsync
#define DET_GAMEPAD1 ((biosvar_gamepad[0] & 0x0001)      )
#define DET_GAMEPAD2 ((biosvar_gamepad[0] & 0x0010) >>  4)
#define DET_GAMEPAD3 ((biosvar_gamepad[1] & 0x0100) >>  8)
#define DET_GAMEPAD4 ((biosvar_gamepad[1] & 0x1000) >> 12)
#define READ_GAMEPAD1 ( ((biosvar_gamepad[1] & 0x000F) << 8) | ((biosvar_gamepad[0] & 0x0F00) >> 4) | ((biosvar_gamepad[0] & 0x000F)      ) )
#define READ_GAMEPAD2 ( ((biosvar_gamepad[1] & 0x00F0) << 4) | ((biosvar_gamepad[0] & 0xF000) >> 8) | ((biosvar_gamepad[0] & 0x00F0) >>  4) )
#define READ_GAMEPAD3 ( ((biosvar_gamepad[2] & 0x0F00)     ) | ((biosvar_gamepad[2] & 0x000F) << 4) | ((biosvar_gamepad[1] & 0x0F00) >>  8) )
#define READ_GAMEPAD4 ( ((biosvar_gamepad[2] & 0xF000) >> 4) | ((biosvar_gamepad[2] & 0x00F0)     ) | ((biosvar_gamepad[1] & 0xF000) >> 12) )

// Compare against GAMEPAD_BTN_x masks, when not using BiosVsync
#define DET_GAMEPAD1_RAW ((VDP.IO_GAMEPAD[0] & 0x0001)      )
#define DET_GAMEPAD2_RAW ((VDP.IO_GAMEPAD[0] & 0x0010) >>  4)
#define DET_GAMEPAD3_RAW ((VDP.IO_GAMEPAD[1] & 0x0100) >>  8)
#define DET_GAMEPAD4_RAW ((VDP.IO_GAMEPAD[1] & 0x1000) >> 12)
#define READ_GAMEPAD1_RAW ( ((VDP.IO_GAMEPAD[1] & 0x000F) << 8) | ((VDP.IO_GAMEPAD[0] & 0x0F00) >> 4) | ((VDP.IO_GAMEPAD[0] & 0x000F)      ) )
#define READ_GAMEPAD2_RAW ( ((VDP.IO_GAMEPAD[1] & 0x00F0) << 4) | ((VDP.IO_GAMEPAD[0] & 0xF000) >> 8) | ((VDP.IO_GAMEPAD[0] & 0x00F0) >>  4) )
#define READ_GAMEPAD3_RAW ( ((VDP.IO_GAMEPAD[2] & 0x0F00)     ) | ((VDP.IO_GAMEPAD[2] & 0x000F) << 4) | ((VDP.IO_GAMEPAD[1] & 0x0F00) >>  8) )
#define READ_GAMEPAD4_RAW ( ((VDP.IO_GAMEPAD[2] & 0xF000) >> 4) | ((VDP.IO_GAMEPAD[2] & 0x00F0)     ) | ((VDP.IO_GAMEPAD[1] & 0xF000) >> 12) )

// Check with MOUSE_DETECT mask
#define MOUSE_DET (VDP.IO_GAMEPAD[0] & 0x8000)

// Check with MOUSE_BTN_LEFT/MOUSE_BTN_RIGHT masks
#define MOUSE_BUTTONS(x) ((~x) & 0x7000)

// Convert s12 to s16 for mouse deltas
#define MOUSE_DELTA(x) ((int16_t)((x)<<4) >> 4)

// Colors
#define RGB555(r,g,b) (((r) << 10) | ((g) << 5) | (b))

// VDP register helpers
#define BM_SUBPAL(s0,s1,s2,s3) ( ((s0) << 12) | ((s1) << 8) | ((s2) << 4) | (s3) )
#define BG_TILESIZE(ts0,ts1) ( ((ts0) << 6) | ((ts1) << 4) )
#define OBJ_TILEHIGH(th0,th1) ( ((th0) << 11) | ((th1) << 8) )
#define OBJ_SUBPAL(s0,s1,s2,s3) ( (s0) | ((s1) << 4) | ((s2) << 8) | ((s3) << 12) )
#define LAYER_SCREEN(bm01,bm23,ob0,ob1) ( ((bm01) << 8) | ((bm23) << 10) | ((ob0) << 12) | ((ob1) << 14) )

