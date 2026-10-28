
#ifndef HIGHSCORES_H
#define HIGHSCORES_H

extern const settings_data * p_game_settings;

#define SCORES_PER_GAME_TYPE  (5)

typedef struct score_rec {
    uint16_t score;
    uint16_t max_level;
    int8_t difficulty;
} score_rec;

typedef struct highscores_rec {
    score_rec  long_pet[SCORES_PER_GAME_TYPE];
    score_rec  crunch_up[SCORES_PER_GAME_TYPE];
    score_rec  tail_cleanup[SCORES_PER_GAME_TYPE];
    score_rec  level_up[SCORES_PER_GAME_TYPE];
    score_rec  marathon[SCORES_PER_GAME_TYPE];
    uint16_t checksum;  // Only updated when reading or writing sram
} highscores_rec;


extern highscores_rec highscores;

void highscore_is_new_record_clear(void);
bool highscore_is_new_record(void);
bool highscore_new_record_is_this_entry(const int index, const int8_t game_type);
void highscores_reset_all(void);
void highscore_new_record_check_and_update(const score_rec * new_score, const int8_t game_type);

#endif // HIGHSCORES_H


