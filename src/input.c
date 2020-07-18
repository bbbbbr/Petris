// input.c

#include <gb/gb.h>
#include <gb/cgb.h> // Include cgb functions

#include "input.h"

UINT8 keys = 0x00;
UINT8 previous_keys = 0x00;
UINT8 key_repeat_count = 0x00;

// Reduce CPU usage by only checking once per frame
void waitpad_lowcpu(UINT8 button_mask, UINT8 wait_type) {

    UINT8 test_result;

    // J_WAIT_ANY_PRESSED  waits until ANY masked button is pressed
    // J_WAIT_ALL_RELEASED waits until ALL masked buttons are released
    do {
        wait_vbl_done();

        test_result = (joypad() & button_mask);

        if (wait_type == J_WAIT_ANY_PRESSED)
            test_result = !test_result;

    }  while (test_result);
}
