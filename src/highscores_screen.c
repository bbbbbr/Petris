
// Copyright 2026 (c) bbbbbr
//
// This software is licensed under:
//
// For the purposes of this project "Share Alike" shall also include
// distribution of the source code and any changes to it.
//
// CC-BY-NC-SA: Attribution Non-Commercial Share Alike Creative Commons
// Attribution-NonCommercial-ShareAlike 4.0 International License
// See: http://creativecommons.org/licenses/by-nc-sa/4.0/

#include <gbdk/platform.h>
#include <gbdk/metasprites.h>

#include <stdint.h>
#include <stdbool.h>

#include "gfx.h"
#include "gfx_print.h"
#include "input.h"

#include "common.h"
#include "fade.h"

#include "options.h"
#include "options_screen.h"
#include "highscores.h"
#include "cartsave.h"

#include "game_board_gfx.h"

#include "intro_screen_out.h"  // For determining where to start loading font tiles
#include "pet_and_special_tiles_out.h"

// The tile load start offset here is to skip past main background + transparent tile + the two tiles
// used during gameplay for filling/resetting under the board, so that when the font loads it won't
// overwrite the filled board (from end of gameplay) and turn that area transparent
#define TILE_LOAD_OFFSET_FONT_VRAM_ABS     (OBJ_TILEGROUP_BASE_512 + 0u)
#define TILE_LOAD_OFFSET_FONT_VRAM_OAM_REL (0u)



//  32 x 28
//
// |01234567890123456789012345678912|
// |                                |
// |  HIGH SCORES: < TAIL CLEANUP > | 4 (Row Color: YELLOW)
// |                                |
// |   SCORE  DIFF.  NAME  MAX LEV. | 7 (Row Color: YELLOW)
// |  655350  EXPERT  XXX  999      | 9
// |  123450  BEAST   A--   99 NEW! | 11 <-- New Highscore: Row color: YELLOW, PINK "new" bouncing sprite, otherwise GREY
// |    1000  NORMAL  ---    0      | 13
// |     100  NORMAL  ---    0      | 15
// |       0  NORMAL  ---    0      | 17
// |       0  NORMAL  ---    0      | 19


#define PAL_TITLE      (PRINT_PAL_TILE_YELLOW)
#define PAL_TITLE_TYPE (PRINT_PAL_TILE_GREY)
#define PAL_HEADERS    (PRINT_PAL_TILE_YELLOW)
#define PAL_SCORE      (PRINT_PAL_TILE_GREY)
#define PAL_NON_SCORE  (PRINT_PAL_TILE_GREY)
#define PAL_HIGHLIGHT  (PRINT_PAL_TILE_YELLOW)  // TODO: PRINT_PAL_TILE_PINK was removed, but might have enough free to add back?

#define TITLE_X_START         (2u)
#define TITLE_Y_START         (3u)
#define TITLE_TYPE_X_START    (TITLE_X_START + 15u)
#define TITLE_TEXT            "HIGH SCORES: < ____________ >"  // TODO CHANGE PARENS TO < > to indicate left/right

#define HEADERS_X_START       (3u)
#define HEADERS_Y_START       (TITLE_Y_START + FONT_8x16_TILE_HEIGHT + 1u)
#define HEADERS_TEXT          "SCORE  DIFF.  NAME  MAX LEV."

#define SCORES_Y_START        (HEADERS_Y_START + FONT_8x16_TILE_HEIGHT)
#define SCORES_X_START        (2u)
#define SCORES_X_SCORE_LEN    (5u)
#define SCORES_X_FAKE_0_LEN   (1u)
#define SCORES_X_GAP          (2u)
#define SCORES_X_DIFF_LEN     (6u)
#define SCORES_X_INITIALS_LEN (SCORES_INITIALS_LEN)
#define SCORES_X_SPRITE_GAP   (1u)
#define SCORES_X_INITIALS     (SCORES_X_START + SCORES_X_SCORE_LEN + SCORES_X_FAKE_0_LEN + SCORES_X_GAP + SCORES_X_DIFF_LEN + SCORES_X_GAP)

#define SPR_ENTER_INITIALS_Y_OFFSET (TILE_WIDTH_8x8 + 2u)

#define SCORES_ZERO_BLANK_TEXT         "     " // Print blank text when zero since there is a fake trailing zero, match length of max score (5)
#define SCORES_SPRITE_TEXT_NEW_RECORD  "NEW!"
#define SCORES_SPRITE_TEXT_ENTRY_INITS "-"


#define IDLE_GAMETYPE_COUNT_RESET      (0u)
#define IDLE_GAMETYPE_COUNT_THRESHOLD  (7u * 60u) // Rotate through score types every N seconds if no input
#define IDLE_EXIT_COUNT_RESET          (0u)
#define IDLE_EXIT_COUNT_THRESHOLD      (OPTION_GAME_TYPE_MAX) // Exit screen after rotating through all game types with no player input

#define SCORE_PRINT_DELAY_FRAMES       (250 / 16.6)           // 1/4 of second (in frames) between printing consecutive score records

#define SPR_ID_NEW_RECORD_START   0u // New record starts at sprite ID 0
#define SPR_ID_NEW_RECORD_RESET   (SPR_ID_NEW_RECORD_START)


#define SPR_ANIMATE_UPDATE_MASK (0x03) // Only update cursor once every 8 frames
#define NEW_RECORD_BITSHIFT     (3)
#define NEW_RECORD_LUT_MASK     (0x07 << NEW_RECORD_BITSHIFT) // 8 offset entries in the LUT
const uint8_t spr_new_record_offset[] = {0,1,2,3,3,2,1,0};

static uint8_t spr_id_new_record_end;
static uint8_t spr_new_record_y;
static uint8_t spr_new_record_x;
static bool show_new_record_indicator_sprite;

static bool    enter_initials_mode_is_active;
static uint8_t enter_initials_index;
static char *  enter_initials_str;
static uint8_t spr_id_enter_initials_highlight;

static int8_t showing_game_type;
static uint16_t idle_gametype_count;
static uint16_t idle_exit_count;
static uint16_t skip_delay_keys;


static void highscores_draw(void);
static void highscores_screen_switch_game_type(int8_t dir);
static void highscores_screen_exit_cleanup(void);
static void highscore_update_idle_actions(void);
static void initials_entry_mode_init(void);
static void initials_entry_mode_handle(void);


static void highscores_draw(void) {

    // Clear everything on BG0 (game pieces, text printing, etc), and hide possible new record indicator sprites
    set_bkg_tilemap_base_address(BG0_MAP_START());
    set_bkg_tiles_target_screen_a_or_b(LAYER_SCREEN_A);
    fill_bkg_rect(0, 0, DEVICE_SCREEN_BUFFER_WIDTH, DEVICE_SCREEN_BUFFER_HEIGHT, SHARED_4BPP_TRANSP_TILE_ID_START);
    hide_sprites_range(0, MAX_HARDWARE_SPRITES);
    show_new_record_indicator_sprite = false;

    score_rec * p_scores = NULL;

    // Pick scores for the matching game type
    switch (showing_game_type) {
        case OPTION_GAME_TYPE_LONG_PET:    p_scores = highscores.long_pet;     break;
        case OPTION_GAME_TYPE_CRUNCH_UP:   p_scores = highscores.crunch_up;    break;
        case OPTION_GAME_TYPE_PET_CLEANUP: p_scores = highscores.tail_cleanup; break;
        case OPTION_GAME_TYPE_LEVEL_UP:    p_scores = highscores.level_up;     break;
        case OPTION_GAME_TYPE_MARATHON:    p_scores = highscores.marathon;     break;
    }

    // Check to see if the score made it into the top scores table
    if (p_scores != NULL) {

        skip_delay_keys = J_NONE;        

        // TODO: check and display (or hide) new highscore sprite indicator "NEW!"

        // Print Title + type
        PRINT_PAL(PAL_TITLE);
        PRINTXY(TITLE_X_START, TITLE_Y_START, TITLE_TEXT, 0);
        PRINT_PAL(PAL_TITLE_TYPE);
        PRINTXY(TITLE_TYPE_X_START, TITLE_Y_START, options_type[showing_game_type], 0);

        // Print column headers
        PRINT_PAL(PAL_HEADERS);
        PRINTXY(HEADERS_X_START, HEADERS_Y_START, HEADERS_TEXT, 0);

        if (!skip_delay_keys) skip_delay_keys = delay_frames_with_input_exit(SCORE_PRINT_DELAY_FRAMES, J_ANY);

        uint16_t y = SCORES_Y_START;
        for (int c = 0; c < SCORES_PER_GAME_TYPE; c++) {
            uint16_t x = SCORES_X_START;

            // Score + fake trailing zero
            bool is_new_highscore_entry = highscore_new_record_is_this_entry(c, showing_game_type);
            uint16_t pal = (is_new_highscore_entry) ? PAL_HIGHLIGHT : PAL_SCORE;
            PRINT_PAL(pal);

            // Print blank text when zero since there is a fake trailing zero (to avoid double zeros)
            if (p_scores[c].score > 0) print_num_u16(x, y, p_scores[c].score, STR_DIGIT_LEN_5);
            else                       PRINTXY(x, y, SCORES_ZERO_BLANK_TEXT, 0);
            x += STR_DIGIT_LEN_5;
            PRINTXY(x, y, "0", 0);
            x += SCORES_X_GAP + SCORES_X_FAKE_0_LEN;

            // Difficulty mode
            PRINTXY(x, y, options_difficulty[p_scores[c].difficulty], 0);
            x += SCORES_X_GAP + SCORES_X_DIFF_LEN;

            // Player Initials
            PRINTXY(x, y, p_scores[c].initials, 0);
            x += SCORES_X_GAP + SCORES_X_INITIALS_LEN;

            // Max level reached
            print_num_u16(x, y, p_scores[c].max_level, STR_DIGIT_LEN_3);
            x += STR_DIGIT_LEN_3 + SCORES_X_SPRITE_GAP;

            if (is_new_highscore_entry) {
                show_new_record_indicator_sprite = true;
                spr_new_record_y = y * TILE_WIDTH_8x8;
                spr_new_record_x = x * TILE_WIDTH_8x8;
            }

            y += FONT_8x16_TILE_HEIGHT;
            if (!skip_delay_keys) skip_delay_keys = delay_frames_with_input_exit(SCORE_PRINT_DELAY_FRAMES, J_ANY);
        }

        // Show new record sprite *after* all records print so it doesn't
        // look weirdly fixed in place and only animating after record printing finishes
        if (show_new_record_indicator_sprite) {
            PRINT_POS(spr_new_record_x, spr_new_record_y);
            spr_id_new_record_end = print_to_sprites(SPR_ID_NEW_RECORD_START, REL_PAL_FONT_8x16_PINK_OAM_1, SCORES_SPRITE_TEXT_NEW_RECORD);
        }
    }
}


static void highscores_screen_switch_game_type(int8_t dir) {

    // Increment which type of game mode scores are being shown for and handle wraparound
    showing_game_type += dir;
    if (showing_game_type < OPTION_GAME_TYPE_MIN)      showing_game_type = OPTION_GAME_TYPE_MAX;
    else if (showing_game_type > OPTION_GAME_TYPE_MAX) showing_game_type = OPTION_GAME_TYPE_MIN;

    highscores_draw();
}


// Rotates through game type high scores if no buttons are pressed
// If it rotates through them all then it exits the screen
static void highscore_update_idle_actions(void) {

    if (++idle_gametype_count > IDLE_GAMETYPE_COUNT_THRESHOLD) {

        if (++idle_exit_count > IDLE_EXIT_COUNT_THRESHOLD) {
            highscores_screen_exit_cleanup();
            game_state = GAME_INTRO_INIT;
        }

        highscores_screen_switch_game_type(1);
        idle_gametype_count = IDLE_GAMETYPE_COUNT_RESET;
    }
    if (KEY_TICKED(J_ANY)) {
        idle_gametype_count = IDLE_GAMETYPE_COUNT_RESET;
        idle_exit_count     = IDLE_EXIT_COUNT_RESET;
    }
}


static void highscores_screen_exit_cleanup(void) {

    // Clear new highscore flag so it doesn't show
    // if just returning to the screen
    highscore_is_new_record_clear();

    palette_fade_start(FADE_TO_BLACK);
    palette_fade_wait_completed();

    hide_sprites_range(0, MAX_HARDWARE_SPRITES);

    // Clear BG0 (where the text is printed)
    fill_bkg_rect(0, 0, DEVICE_SCREEN_BUFFER_WIDTH, DEVICE_SCREEN_BUFFER_HEIGHT, SHARED_4BPP_TRANSP_TILE_ID_START);
}


static void new_record_animate(void) {

    // If visible, animate the new record indicator with a mild bounce every N frames
    if (show_new_record_indicator_sprite) {
        if ((sys_time & SPR_ANIMATE_UPDATE_MASK) == SPR_ANIMATE_UPDATE_MASK) {

            for (uint16_t c = SPR_ID_NEW_RECORD_START; c < spr_id_new_record_end; c+=2) {
                // 8x16 font is 2 tiles/sprites high
                uint8_t sine_offset = spr_new_record_offset[(sys_time & NEW_RECORD_LUT_MASK) >> NEW_RECORD_BITSHIFT] - 2;
                move_sprite_y(c,        spr_new_record_y + sine_offset);
                move_sprite_y((c + 1u), spr_new_record_y + sine_offset + TILE_HEIGHT_8x8);
            }
        }
    }
}


static void initials_entry_mode_init(void) {

    enter_initials_index                     = SCORES_INITIALS_MIN;
    enter_initials_str                       = highscore_new_record_get_initials_str_ptr();

    // Put intials entry highlight under active initial character
    PRINT_POS((SCORES_X_INITIALS + enter_initials_index) * TILE_WIDTH_8x8, spr_new_record_y + SPR_ENTER_INITIALS_Y_OFFSET);
    spr_id_enter_initials_highlight = spr_id_new_record_end;
    print_to_sprites(spr_id_enter_initials_highlight , REL_PAL_FONT_8x16_PINK_OAM_1, SCORES_SPRITE_TEXT_ENTRY_INITS);
}


static void initials_entry_mode_handle(void) {

    bool redraw         = false;
    bool entry_complete = false;

    // Redraw for any input
    if (KEYS_TICKED(J_ANY)) redraw = true;

    switch (KEYS_TICKED(J_ANY)) {
                      // Cursor Left
        case J_B:
        case J_LEFT:  if (enter_initials_index > SCORES_INITIALS_MIN) enter_initials_index--;
                      break;

                      // Cursor Right
                      // If pressing "A" after last character is entered then commit the entry and exit edit mode
        case J_A:     if (enter_initials_index == SCORES_INITIALS_MAX) { entry_complete = true; break; }
                      // Otherwise fall through to changing active letter
        case J_RIGHT: if (enter_initials_index < SCORES_INITIALS_MAX) enter_initials_index++;
                      break;

                      // Rotate character toward A
        case J_DOWN:  if (enter_initials_str[enter_initials_index] > 'A') enter_initials_str[enter_initials_index]--;
                      break;

                      // Rotate character toward Z
        case J_UP:    if (enter_initials_str[enter_initials_index] < 'Z') enter_initials_str[enter_initials_index]++;
                      break;

                      // Commit the entry and exit edit mode
        case J_START: entry_complete = true;
                     break;

    }

    if (entry_complete) {
        // Exit initials entry mode
        enter_initials_mode_is_active = false;
        // Write updated score records to SRAM
        cartsave_save_highscores();
        // Hide the highlight sprite
        hide_sprites_range(spr_id_enter_initials_highlight, spr_id_enter_initials_highlight + 1u);
    }

    if (redraw) {
        // Move highlight sprite
        move_sprite_x(spr_id_enter_initials_highlight,      (SCORES_X_INITIALS + enter_initials_index) * TILE_WIDTH_8x8);
        move_sprite_x(spr_id_enter_initials_highlight + 1u, (SCORES_X_INITIALS + enter_initials_index) * TILE_WIDTH_8x8);

        // And print updated initials
        PRINT_PAL(PAL_HIGHLIGHT);
        PRINTXY(SCORES_X_INITIALS, spr_new_record_y / TILE_HEIGHT_8x8, enter_initials_str, 0);
    }

    // Animate the enter initials cursor with a mild bounce every N frames
    if ((sys_time & SPR_ANIMATE_UPDATE_MASK) == SPR_ANIMATE_UPDATE_MASK) {
        uint8_t sine_offset = spr_new_record_offset[(sys_time & NEW_RECORD_LUT_MASK) >> NEW_RECORD_BITSHIFT] - 2;
        move_sprite_y(spr_id_enter_initials_highlight,      spr_new_record_y + sine_offset + SPR_ENTER_INITIALS_Y_OFFSET);
        move_sprite_y(spr_id_enter_initials_highlight + 1u, spr_new_record_y + sine_offset + SPR_ENTER_INITIALS_Y_OFFSET + TILE_HEIGHT_8x8);
    }
}

// Initializes high score screen
void highscores_screen_init(void) {

    // Rely on Title Screen or Game Over Screen graphics loading for Background image and Font tiles / drawing setup
    load_8x16_font_tiles(TILE_LOAD_OFFSET_FONT_VRAM_ABS, TILE_LOAD_OFFSET_FONT_VRAM_OAM_REL);
    load_8x16_font_tilemap_palettes();
    load_8x16_font_sprite_palettes();

    showing_game_type   = game_options.type;

    idle_gametype_count = IDLE_GAMETYPE_COUNT_RESET;
    idle_exit_count     = IDLE_EXIT_COUNT_RESET;

    skip_delay_keys = J_NONE;

    show_new_record_indicator_sprite = false;
    spr_id_new_record_end = SPR_ID_NEW_RECORD_RESET;


    palette_fade_start(FADE_TO_TARGET);

    SHOW_SPRITES;

    highscores_draw();

    // Switch to initials entry mode if there is a new highscore
    enter_initials_mode_is_active = highscore_is_new_record();
    if (enter_initials_mode_is_active) initials_entry_mode_init();

}


void highscores_screen_handle(void) {


    if (enter_initials_mode_is_active) {
        initials_entry_mode_handle();
    }
    else {
        
        uint16_t keys_ticked = KEYS_TICKED(J_ANY);
        // Allows pressing any key to skip score line printing delays
        if (skip_delay_keys != J_NONE) {
            keys_ticked = skip_delay_keys;
            skip_delay_keys = J_NONE;
        }

        // If there isn't a new high-score record active then
        // cycle through the screens when there isn't any input
        if (!highscore_is_new_record()) {
            highscore_update_idle_actions();
        }

        if (keys_ticked & (J_LEFT)) {

            highscores_screen_switch_game_type(-1);
        }
        else if (keys_ticked & (J_RIGHT)) {

            highscores_screen_switch_game_type(1);
        }
        // Go back to Intro Screen
        else if (keys_ticked & (J_Y | J_START | J_A)) {

            highscores_screen_exit_cleanup();
            game_state = GAME_INTRO_INIT;
        }
    }

    new_record_animate();
}

