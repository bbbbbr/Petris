// input.c

#include <gbdk/platform.h>

#include <stdint.h>
#include <stdbool.h>

#include "input.h"

uint16_t keys = 0x0000;
uint16_t previous_keys = 0x0000;
uint16_t key_repeat_count = 0x0000;

// Reduce CPU usage by only checking once per frame
// Allows a loop control to be passed in
void waitpadticked_lowcpu(uint16_t button_mask) {

    while (true) {

        vsync(); // yield CPU
        UPDATE_KEYS();
        if (KEY_TICKED(button_mask))
            break;
    };

    // Prevent passing through any key ticked
    // event that may have just happened
    UPDATE_KEYS();
}
