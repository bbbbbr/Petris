#ifndef _FADE_H
#define _FADE_H


void palettes_init(void);
void palette_fade_wait_completed(void);
bool palette_fade_is_running(void);
void palette_fade_to_black_start(void);
void palette_fade_to_target_start(void);
void palette_fade_vlb_ended_handler(void);
void palette_fade_vlb_handler(void);
void set_fade_target_4bpp_palette(unsigned int first_palette, unsigned int nb_palettes, const palette_color_t *rgb_data);

#endif
