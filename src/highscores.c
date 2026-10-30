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

// options.c


#include <gbdk/platform.h>

#include <stdint.h>
#include <stdbool.h>

#include "common.h"
#include "cartsave.h"

#include "options.h"
#include "highscores.h"


highscores_rec highscores;

bool   is_new_highscore;
int    new_highscore_idx;
int8_t new_highscore_type;
char * new_highscore_initials_ptr;


// Called on startup and start of a new game
void highscore_is_new_record_clear(void) {
    is_new_highscore = false;
}

bool highscore_is_new_record(void) {
    return is_new_highscore;
}

char * highscore_new_record_get_initials_str_ptr(void) {
    return new_highscore_initials_ptr;
}


// Called on startup and start of a new game
bool highscore_new_record_is_this_entry(const int index, const int8_t game_type) {
    if (is_new_highscore) {
        return ((index == new_highscore_idx) && (game_type == new_highscore_type));
    } else {
        return false;
    }
}


void highscores_reset_all(void) {

    for (uint8_t c = 0u; c < SCORES_PER_GAME_TYPE; c++) {
        highscores.long_pet[c].score     = 0u;
        highscores.crunch_up[c].score    = 0u;
        highscores.tail_cleanup[c].score = 0u;
        highscores.level_up[c].score     = 0u;
        highscores.marathon[c].score     = 0u;

        // Add string terminator to the end of the initials
        highscores.long_pet[c].initials[SCORES_INITIALS_TERM]     = '\0';
        highscores.crunch_up[c].initials[SCORES_INITIALS_TERM]    = '\0';
        highscores.tail_cleanup[c].initials[SCORES_INITIALS_TERM] = '\0';
        highscores.level_up[c].initials[SCORES_INITIALS_TERM]     = '\0';
        highscores.marathon[c].initials[SCORES_INITIALS_TERM]     = '\0';

        highscores.long_pet[c].max_level     = 0u;
        highscores.crunch_up[c].max_level    = 0u;
        highscores.tail_cleanup[c].max_level = 0u;
        highscores.level_up[c].max_level     = 0u;
        highscores.marathon[c].max_level     = 0u;

        highscores.long_pet[c].difficulty     = OPTION_DIFFICULTY_DEFAULT;
        highscores.crunch_up[c].difficulty    = OPTION_DIFFICULTY_DEFAULT;
        highscores.tail_cleanup[c].difficulty = OPTION_DIFFICULTY_DEFAULT;
        highscores.level_up[c].difficulty     = OPTION_DIFFICULTY_DEFAULT;
        highscores.marathon[c].difficulty     = OPTION_DIFFICULTY_DEFAULT;
    }
}


// Check if a score + game type is a new record, if so update the table and save to SRAM
void highscore_new_record_check_and_update(const score_rec * p_new_score, const int8_t game_type) {

    score_rec * p_scores = NULL;
    is_new_highscore = false;

    // Pick scores for the matching game type
    switch (game_type) {
        case OPTION_GAME_TYPE_LONG_PET:    p_scores = highscores.long_pet;     break;
        case OPTION_GAME_TYPE_CRUNCH_UP:   p_scores = highscores.crunch_up;    break;
        case OPTION_GAME_TYPE_PET_CLEANUP: p_scores = highscores.tail_cleanup; break;
        case OPTION_GAME_TYPE_LEVEL_UP:    p_scores = highscores.level_up;     break;
        case OPTION_GAME_TYPE_MARATHON:    p_scores = highscores.marathon;     break;
        default: return;
    }

    // Check to see if the score made it into the top scores table
    if (p_scores != NULL) {

        for (int c = 0; c < SCORES_PER_GAME_TYPE; c++) {
            if (p_new_score->score > p_scores[c].score) {

                // It's a new high score!
                is_new_highscore   = true;
                new_highscore_idx  = c;
                new_highscore_type = game_type;

                // Shift the remaining scores below the new entry down (work from the bottom upward)
                // NOTE: FIRST COMPILER BUG? Runs forever and crashes without volatile attribute
                for (volatile int below_idx = (SCORES_PER_GAME_TYPE - 1); below_idx != c; below_idx--) {
                    p_scores[below_idx] = p_scores[below_idx - 1];
                }

                // Insert the new score and pre-fill the name with AAA
                p_scores[new_highscore_idx] = *p_new_score;
                new_highscore_initials_ptr  = p_scores[new_highscore_idx].initials;
                new_highscore_initials_ptr[SCORES_INITIALS_MIN     ] = 'A';
                new_highscore_initials_ptr[SCORES_INITIALS_MIN + 1u] = 'A';
                new_highscore_initials_ptr[SCORES_INITIALS_MIN + 2u] = 'A';

                // Write updated score records to SRAM
                cartsave_save_highscores();

                return;
            }
        }
    }
}
