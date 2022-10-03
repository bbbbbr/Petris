// input.c

#include <gb/gb.h>
#include <gb/cgb.h> // Include cgb functions

#include "input.h"

UINT8 keys = 0x00;
UINT8 previous_keys = 0x00;
UINT8 key_repeat_count = 0x00;

// Reduce CPU usage by only checking once per frame
// Allows a loop control to be passed in
void waitpadticked_lowcpu(UINT8 button_mask, volatile UINT8 * loop_control) {

    UINT8 always_loop = 1;

    // If no loop control var specified, use a placeholder
    if (loop_control == NULL)
        loop_control = &always_loop;

    while (*loop_control) {

        wait_vbl_done(); // yield CPU
        UPDATE_KEYS();
        if (KEY_TICKED(button_mask))
            break;
    };

    // Prevent passing through any key ticked
    // event that may have just happened
    UPDATE_KEYS();
}
