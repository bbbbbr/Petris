// game_stats.h

#ifndef GAME_STATS_H
#define GAME_STATS_H

void stats_maxpet_reset(void);
void stats_maxpet_copy_iflongest(void);
void stats_maxpet_draw(void);
void stats_display(void);
void stats_show_var(uint8_t, uint8_t, const char*, uint16_t);

#endif // GAME_STATS_H