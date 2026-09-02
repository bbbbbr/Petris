#pragma once

// For VDP registers

#define TRIGGER_CAPTURE      1
#define TRIGGER_PRINT_TEMP   2
#define TRIGGER_PRINT_SENSOR 4

#define IRQ1_ENABLE    1
#define IRQ1_SRC_VSYNC 0
#define IRQ1_SRC_HSYNC 2

#define BM_MODE_8BPP_VSPLIT 0
#define BM_MODE_8BPP_SHARED 1
#define BM_MODE_4BPP_VSPLIT 2
#define BM_MODE_4BPP_HSPLIT 3
#define BM_MODE_4BPP_SHARED 4

#define BM_LATCH_ENABLE 0x100

#define BG_TILEMAP_CHRNUM_MASK 0x07FFu
#define BG_TILEMAP_FLIPY       0x8000u
#define BG_TILEMAP_FLIPX       0x4000u
#define BG_TILEMAP_SUBPAL_MASK 0x3000u
#define BG_TILEMAP_SCREEN_B    0x0800u

#define BG_TILESIZE_8X8   0
#define BG_TILESIZE_16X16 1
#define BG_TILESIZE_32X32 2
#define BG_TILESIZE_64X64 3

#define BG0_FORMAT_4BPP 0
#define BG0_FORMAT_8BPP 8

#define BG0_SCROLL_X 0
#define BG0_SCROLL_Y 1
#define BG1_SCROLL_X 2
#define BG1_SCROLL_Y 3

#define BG_LAYOUT_64X64_SPLIT  0
#define BG_LAYOUT_64X64_SHARED 1
#define BG_LAYOUT_64X32_SPLIT  2
#define BG_LAYOUT_64X32_SHARED 3
#define BG_LAYOUT_32X64_SPLIT  4
#define BG_LAYOUT_32X64_SHARED 5
#define BG_LAYOUT_32X32_SPLIT  6
#define BG_LAYOUT_32X32_SHARED 7

#define PAL_0   0
#define PAL_1   1
#define PAL_2   2
#define PAL_3   3
#define PAL_4   4
#define PAL_5   5
#define PAL_6   6
#define PAL_7   7
#define PAL_8   8
#define PAL_9   9
#define PAL_10  10
#define PAL_11  11
#define PAL_12  12
#define PAL_13  13
#define PAL_14  14
#define PAL_15  15

#define OBJ_FORMAT_4BPP 0x0000
#define OBJ_FORMAT_8BPP 0x4000

#define BLEND_MATH      0
#define BLEND_MATH_HALF 1
#define BLEND_A_ONLY    2
#define BLEND_HIGHRES   3
#define BLEND_B_OVER_A  4
#define BLEND_A_OVER_B  5

#define LAYER_ENABLE_BG0  0x01
#define LAYER_ENABLE_BG1  0x02
#define LAYER_ENABLE_BM0  0x04
#define LAYER_ENABLE_BM1  0x08
#define LAYER_ENABLE_BM2  0x10
#define LAYER_ENABLE_BM3  0x20
#define LAYER_ENABLE_OBJ0 0x40
#define LAYER_ENABLE_OBJ1 0x80

#define LAYER_SCREEN_A 2
#define LAYER_SCREEN_B 1

#define BLEND_MATH_ADD  0x00
#define BLEND_MATH_SUB  0x80
#define SCREEN_A_ENABLE 0x40
#define SCREEN_B_ENABLE 0x20
#define SCREEN_B_SOLID  0x10

#define PRIORITY_BM_A   0x0
#define PRIORITY_BM_B   0x1
#define PRIORITY_BG0_A  0x0
#define PRIORITY_BG0_B  0x2
#define PRIORITY_OBJ0_A 0x0
#define PRIORITY_OBJ0_B 0x4
#define PRIORITY_OBJ0_C 0x8
#define PRIORITY_OBJ0_D 0xC

#define CAPTURE_MODE_OUTPUT_15BPP  0x000
#define CAPTURE_MODE_SCREENA_15BPP 0x100
#define CAPTURE_MODE_SCREENA_8BPP  0x200

#define IRQ0_ENABLE      0x82 // why 2 bits is unknown
#define IRQ0_VCMP_ENABLE 0x20
#define NMI_ENABLE       0x04

#define SENSOR_INK         4
#define SENSOR_PAPER_FRONT 2
#define SENSOR_PAPER_REAR  1

#define REGION_JUMPER_NTSC 1
#define REGION_JUMPER_PAL  0

// For BIOS functions

#define CONTROL_MODE_NONE    0
#define CONTROL_MODE_MOUSE   1
#define CONTROL_MODE_GAMEPAD 2

#define VIDEO_HEIGHT_224P 0
#define VIDEO_HEIGHT_240P 1

#define SEAL_TYPE_NONE   0
#define SEAL_TYPE_NORMAL 1
#define SEAL_TYPE_VHS    3

#define SOUND_CHANS_4CH        0 //  6, 4, 2, 4
#define SOUND_CHANS_3CH_RHYTHM 1 //  6, 4, 2, r
#define SOUND_CHANS_3CH        2 //  6, 4, 2, x
#define SOUND_CHANS_1CH        3 // 12, x, x, x

#define SOUND_VOL_CH2_3 0
#define SOUND_VOL_CH4   1
#define SOUND_VOL_60  0
#define SOUND_VOL_80  1
#define SOUND_VOL_100 2

// For macros and extra functions

#define GAMEPAD_BTN_DETECT 0x001
#define GAMEPAD_BTN_START  0x002
#define GAMEPAD_BTN_LTRIG  0x004
#define GAMEPAD_BTN_RTRIG  0x008
#define GAMEPAD_BTN_A      0x010
#define GAMEPAD_BTN_D      0x020
#define GAMEPAD_BTN_C      0x040
#define GAMEPAD_BTN_B      0x080
#define GAMEPAD_BTN_UP     0x100
#define GAMEPAD_BTN_DOWN   0x200
#define GAMEPAD_BTN_LEFT   0x400
#define GAMEPAD_BTN_RIGHT  0x800

#define MOUSE_BTN_RIGHT  0x4000
#define MOUSE_BTN_LEFT   0x1000

#define INT_PRIO_IRQ0   0x0C
#define INT_PRIO_IRQ1   0x08
#define INT_PRIO_IRQ2   0x04
#define INT_PRIO_IRQ3   0x00
#define INT_PRIO_IRQ4   0x1C
#define INT_PRIO_IRQ5   0x18
#define INT_PRIO_IRQ6   0x14
#define INT_PRIO_IRQ7   0x10
#define INT_PRIO_DMAC01 0x2C
#define INT_PRIO_DMAC23 0x28
#define INT_PRIO_ITU0   0x24
#define INT_PRIO_ITU1   0x20
#define INT_PRIO_ITU2   0x3C
#define INT_PRIO_ITU3   0x38
#define INT_PRIO_ITU4   0x34
#define INT_PRIO_SCI0   0x30
#define INT_PRIO_SCI1   0x4C
#define INT_PRIO_PRT    0x48
#define INT_PRIO_WDTREF 0x44
