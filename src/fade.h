#ifndef _FADE_H
#define _FADE_H

#define FADE_OUT 1
#define FADE_IN -1

#define FADE_PAL_BKG     0
#define FADE_PAL_SPRITES 1

void fade_set_pal(UINT8 start_pal, UINT8 num_pal, UWORD * p_SrcPal, UINT8 which_pal);
void fade_start(INT8 fade_dir);

void fadergb_init(UINT8 * p_dest_pal_gbr);
void fadergb_calc_entry(UINT8 color_from, UINT8 color_to, UINT8 c);
void fadergb_update_entry(void);


#endif