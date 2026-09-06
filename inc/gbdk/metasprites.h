#pragma #once

#ifndef _METASPRITES_H
#define _METASPRITES_H

#include "stdint.h"
#include "loopy.h"
#include "loopy_helpers.h"

#undef ALWAYS_INLINE
#define ALWAYS_INLINE __attribute__((always_inline))


typedef struct metasprite_t {
    uint8_t dtile;
    int8_t  dy;
    uint8_t props;
    int8_t  dx;
} metasprite_t;

#define metasprite_end -128
#define METASPR_ITEM(dy,dx,dt,a) {(dy),(dx),(dt),(a)}
#define METASPR_TERM {metasprite_end}

extern const void * __current_metasprite;
extern uint8_t __current_base_tile;
extern uint8_t __current_base_prop;
extern uint8_t __render_shadow_OAM;


/**
    Hides all hardware sprites in range from <= X < to
    @param from start OAM index
    @param to finish OAM index (must be <= MAX_HARDWARE_SPRITES)

    @see hide_sprite, MAX_HARDWARE_SPRITES
 */
void hide_sprites_range(uint8_t from, uint8_t to);


#endif // _METASPRITES_H
