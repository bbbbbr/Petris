// input.c

#include <gb/gb.h>
#include <gb/cgb.h> // Include cgb functions

UINT8 keys = 0x00;
UINT8 previous_keys = 0x00;
UINT8 key_repeat_count = 0x00;

// Reduce CPU usage by only checking once per frame
void waitpad_lowcpu(UINT8 button_mask, UINT8 button_match) {
    do {
        wait_vbl_done();
    } while ((joypad() & button_mask) != button_match); // TODO: FIXME - this doesn't work for multiple buttons
}
