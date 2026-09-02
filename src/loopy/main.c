#include "loopy.h"
#include "loopy_helpers.h"

#include "serial.h"
#include "music_data.h"

#include "gbdk/platform.h"

#include "intro_cat_out.h"
#include "title_screen_out.h"

#define TILE_NUM_0  0u


bool gamepadActive = false;
bool mouseControlMode = false;
uint32_t pressedButtons = 0;
uint32_t heldButtons = 0;

struct soundstate_t soundState;

void setupSystem() {
	// Turn off all video output
	VDP.SCREENPRIO = 0;

    // Set Backdrop color to RGB 0,0,0
	VDP.BACKDROP_A = RGB888(0,0,0);
	VDP.BACKDROP_B = RGB888(0,0,0);

	// Turn off controller input
	bios_vdpMode(CONTROL_MODE_NONE, 0);

	// Setup sound hardware (takes a few frames)
	bios_soundChannels(SOUND_CHANS_4CH);
	bios_soundVolume(SOUND_VOL_CH2_3, SOUND_VOL_100);
	bios_soundVolume(SOUND_VOL_CH4,   SOUND_VOL_100);
	bios_initSoundTransmission();

	// Check mouse presence and set control mode accordingly
	// Requires that controller input was turned off for the last few frames
	mouseControlMode = (MOUSE_DET != 0);

	// Enable interrupts and DMA for music
	sys_setInterruptPriority(INT_PRIO_ITU0, 0xF);
	sys_setInterruptMask(0xE);
	sys_setDmaEnabled(true);
}

void init_gfx() {
	bios_vsync();

	VDP.BACKDROP_A      = RGB888(0,0,0);
	VDP.BACKDROP_B      = RGB888(0,0,0);

    // Blend style
	// VDP.BLEND           = BLEND_B_OVER_A;  // Only color 0 tiles show through
    // VDP.BLEND           = BLEND_MATH;
    VDP.BLEND           = BLEND_MATH_HALF;

    VDP.BG_SCROLL[BG0_SCROLL_X] = 0;
    VDP.BG_SCROLL[BG0_SCROLL_Y] = 0;
    VDP.BG_CTRL         = BG_TILESIZE(BG_TILESIZE_8X8, BG_TILESIZE_8X8) | BG0_FORMAT_4BPP | BG_LAYOUT_64X64_SPLIT;
    VDP.BG_SUBPAL[0]    = BG_PAL_SETUP(PAL_0, PAL_1, PAL_2, PAL_3);  // BG0
    VDP.BG_SUBPAL[1]    = BG_PAL_SETUP(PAL_4, PAL_5, PAL_6, PAL_7);  // BG1

	// VDP.SCREENPRIO      = BLEND_MATH_ADD | SCREEN_A_ENABLE | PRIORITY_BM_A | PRIORITY_BG0_A | PRIORITY_OBJ0_A;
    // VDP.SCREENPRIO      = BLEND_MATH_SUB | SCREEN_A_ENABLE | SCREEN_B_ENABLE | PRIORITY_BM_A | PRIORITY_BG0_A | PRIORITY_OBJ0_A;
    VDP.SCREENPRIO      = BLEND_MATH_ADD | SCREEN_A_ENABLE | SCREEN_B_ENABLE | PRIORITY_BM_A | PRIORITY_BG0_A | PRIORITY_OBJ0_A;
	VDP.LAYER_CTRL      = LAYER_SCREEN(LAYER_SCREEN_A, LAYER_SCREEN_A, LAYER_SCREEN_A, LAYER_SCREEN_A) | LAYER_ENABLE_BG0 | LAYER_ENABLE_BG1;

    #define  RESERVE_8BPP_TILE_ROWS   0u  // Number of tile pattern rows to reserve for 8bpp tiles (8 per row)
    // AKA CHAR_SPLIT, Tile Base, VDP.TILEBASE
    VDP.CHARBASE           = RESERVE_8BPP_TILE_ROWS;

    // g_BG0_MAP_START        = VDP.TILE_VRAM;  // BG0 Starts at base of tile vram
    // g_BG1_MAP_START        = g_BG0_MAP_START + BG1_OFFSET[BG_CTRL_LAYOUT_GET()];
    // g_CHAR_VRAM_4BPP_START = (uint8_t *)VDP.TILE_VRAM + CHARBASE_OFFSET[BG_CTRL_LAYOUT_GET()] + (VDP.CHARBASE * BYTES_PER_8BPP_TILE_ROW);
    set_4bpp_tile_patterns_base_address(CHAR_VRAM_4BPP_START());
}

// Test: Draw onto BG1 (and offset loaded palette, tiles and tilemap entries)
void load_intro_cat(void) {
    // Offset past title screen tiles
    const unsigned int tile_num_start_offset = TILE_NUM_0 + title_screen_out_TILE_COUNT;

    bios_vsync();
    set_bkg_4bpp_palette(PAL_4, intro_cat_out_PALETTE_COUNT, intro_cat_out_palettes);
    set_bkg_4bpp_data(tile_num_start_offset, intro_cat_out_TILE_COUNT, intro_cat_out_tiles);
    // Set BG1 tilemap tiles to render on Screen B, then it can blend with BG0 on Screen A
    // Otherwise it would be totally hidden by BG0 (it's current image has no transparent/backdrop tiles)
    set_bkg_tilemap_base_address(BG1_MAP_START());
    set_bkg_tiles_target_screen_a_or_b(LAYER_SCREEN_B);
    set_bkg_based_tiles((DEVICE_SCREEN_WIDTH - intro_cat_out_TILES_WIDTH)/2,  // Tile centered X
                  (DEVICE_SCREEN_HEIGHT - intro_cat_out_TILES_HEIGHT)/2,  // Tile centered Y
                  intro_cat_out_TILES_WIDTH, intro_cat_out_TILES_HEIGHT, intro_cat_out_map, tile_num_start_offset);
}

// Test: Draw onto BG0
void load_title_screen(void) {
    bios_vsync();
    set_bkg_4bpp_palette(PAL_0, title_screen_out_PALETTE_COUNT, title_screen_out_palettes);
    set_bkg_4bpp_data(TILE_NUM_0, title_screen_out_TILE_COUNT, title_screen_out_tiles);
    set_bkg_tilemap_base_address(BG0_MAP_START());
    set_bkg_tiles_target_screen_a_or_b(LAYER_SCREEN_A);
    set_bkg_tiles((DEVICE_SCREEN_WIDTH - title_screen_out_TILES_WIDTH)/2,  // Tile centered X
                  (DEVICE_SCREEN_HEIGHT - title_screen_out_TILES_HEIGHT)/2,  // Tile centered Y
                  title_screen_out_TILES_WIDTH, title_screen_out_TILES_HEIGHT, title_screen_out_map);
}


int main() {
	// Initialize the hardware and system state, and check controller mode
	setupSystem();

	// // Print a hellorld message on the serial port
	// serial_begin(9600);
	// serial_print("Casio Loopy says hello world\r\n");

	// Set the appropriate controller scanning mode and video height
	bios_vdpMode(mouseControlMode ? CONTROL_MODE_MOUSE : CONTROL_MODE_GAMEPAD, VIDEO_HEIGHT_224P);

	// Display some graphics on the BM0 layer and set a backdrop
	init_gfx();


    // Test:
    load_intro_cat();
    load_title_screen();

	// Every frame, move the BM0 layer according to gamepad/mouse
	while(1) {
		bios_vsync();
	}

	return 0;
}
