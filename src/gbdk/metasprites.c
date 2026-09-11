
#include "stdint.h"
#include "loopy.h"
#include "loopy_helpers.h"

#include "gbdk/platform.h"



/**
    Hides all hardware sprites in range from <= X < to
    @param from start OAM index
    @param to finish OAM index (must be <= MAX_HARDWARE_SPRITES)

    @see hide_sprite, MAX_HARDWARE_SPRITES
 */
void hide_sprites_range(uint8_t from, uint8_t to) {

    if (to > (MAX_HARDWARE_SPRITES - 1)) to = MAX_HARDWARE_SPRITES - 1;
    OAM_item_t * itm = &shadow_OAM[from];

    while (from++ <= to) {
        itm->y = 128;
        itm->prop |= S_Y_HIBIT_ON;  // Hide sprite by setting Y high (signed) bit and low y to give it a negative off-screen location
        itm++;
    }
}



