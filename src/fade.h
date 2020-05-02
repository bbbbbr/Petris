#ifndef _FADE_H
#define _FADE_H

#define FADE_OUT 1
#define FADE_IN -1

#define FADE_PAL_BKG     0
#define FADE_PAL_SPRITES 1

void fade_set_pal(UINT8 start_pal, UINT8 num_pal, UWORD * p_SrcPal, UINT8 which_pal);
void fade_start(INT8 fade_dir);

#endif