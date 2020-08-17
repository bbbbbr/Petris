// input.c

#include <gb/gb.h>
#include <gb/cgb.h> // Include cgb functions

#include "input.h"

UINT8 keys = 0x00;
UINT8 previous_keys = 0x00;
UINT8 key_repeat_count = 0x00;

// Reduce CPU usage by only checking once per frame
void waitpad_lowcpu(UINT8 button_mask, UINT8 wait_type) {

    UINT8 any_buttons_pressed;

    // J_WAIT_ANY_PRESSED  waits until ANY masked button is pressed
    // J_WAIT_ALL_RELEASED waits until ALL masked buttons are released
    do {
        wait_vbl_done();

        any_buttons_pressed = (joypad() & button_mask);

        // If waiting for ANY PRESSED button then
        // invert the test result so the loop exits
        // if a single one is pressed
        //
        // Otherwise the loop will exit when there are
        // no more buttons pressed (ALL RELEASED)
        if (wait_type == J_WAIT_ANY_PRESSED)
            any_buttons_pressed = !any_buttons_pressed;

    }  while (any_buttons_pressed);
}
