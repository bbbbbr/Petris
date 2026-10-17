#ifndef _FADE_H
#define _FADE_H

#define FADE_TO_BLACK    (true)
#define FADE_TO_TARGET   (false)

void palettes_init(void);
void palette_fade_wait_completed(void);
bool palette_fade_is_running(void);
void palette_fade_start(bool is_FADE_TO_BLACK);
void palette_fade_vlb_ended_handler(void);
void palette_fade_vlb_handler(void);

void set_fade_target_4bpp_palette(unsigned int first_palette, unsigned int nb_palettes, const palette_color_t *rgb_data);
void set_fade_target_4bpp_palette_entry(unsigned int palette, unsigned int entry, palette_color_t rgb_data);

#endif
