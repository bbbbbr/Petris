
#ifndef HIGHSCORES_H
#define HIGHSCORES_H

extern const settings_data * p_game_settings;

#define SCORES_PER_GAME_TYPE  (5u)

#define SCORES_INITIALS_LEN (3u)
#define SCORES_INITIALS_MIN (0u)
#define SCORES_INITIALS_MAX (SCORES_INITIALS_LEN - 1u)
#define SCORES_INITIALS_ARRAY_SZ ((SCORES_INITIALS_LEN) + 1u)
#define SCORES_INITIALS_TERM     ((SCORES_INITIALS_ARRAY_SZ) - 1u)

typedef struct score_rec {
    uint16_t score;
    char     initials[SCORES_INITIALS_ARRAY_SZ];
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
char * highscore_new_record_get_initials_str_ptr(void);
bool highscore_new_record_is_this_entry(const int index, const int8_t game_type);
void highscores_reset_all(void);
void highscore_new_record_check_and_update(const score_rec * new_score, const int8_t game_type);

#endif // HIGHSCORES_H


