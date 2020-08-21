// Copyright 2020 (c) bbbbbr
//
// This software is licensed under:
//
// For the purposes of this project "Share Alike" shall also include
// distribution of the source code and any changes to it.
//
// CC-BY-NC-SA: Attribution Non-Commercial Share Alike Creative Commons
// Attribution-NonCommercial-ShareAlike 4.0 International License
// See: http://creativecommons.org/licenses/by-nc-sa/4.0/


// Both player consoles start and mostly wait
// in a ready-to-recieve state:
// * External Clock + Transfer Start set
//
// 1. Either console can initiate connection by sending
//    a INITIATE command.
//
// 2. The other console can respond by sending a READY,
//    after sending of that command is completed then
//    **BOTH** consoles immediately set link status as CONNECTED
//
// State A: RESET                   /--Rx:-> CONNECTED (follower)
// Link  A:  -> Tx:INITIATE        /
// Link  B:           \--Rx--> Tx:READY
// State B: RESET                   \------> CONNECTED (controller)


// If a serial transfer with internal clock is performed and no external GameBoy
// is present, a value of $FF will be received in the transfer.


#include <gb/gb.h>
#include <gb/cgb.h> // Include cgb functions
#include <stdlib.h>

#include "common.h"
#include "options.h"

#include "input.h"
#include "gameplay.h"
#include "gfx_print.h"

#include "status_win.h"

#include "serial_link.h"


UINT8 volatile link_status;
UINT8 link_rand_init;

// Initialize ISR
void init_link(void) {

    // IE flag doesn't get set here
    // it's done in link_enable() instead

    // Add interrupt handler for serial link rx and enable it
    disable_interrupts();
    add_SIO(link_isr);
    enable_interrupts();
}


void link_reset(void) {

    __critical {
        link_status = LINK_STATUS_RESET;
    }

    // Set to external clock as default
    // Load a placeholder byte into the transfer register
    SC_REG = LINK_CLOCK_EXT;
    SB_REG = LINK_CMD_IGNORE;
}


void link_enable(void) {

    // Enable link ISR
    disable_interrupts();
    set_interrupts(IE_REG |= SIO_IFLAG);
    enable_interrupts();
}


void link_disable(void) {

    // Disable link ISR
    disable_interrupts();
    set_interrupts(IE_REG &= ~SIO_IFLAG);
    enable_interrupts();
    link_reset();
}


// Gets called after a successful RX **or* TX
// This allows the sender to return to passive receiver waiting state
void link_isr(void) {

    UINT8 link_data;

    link_data = SB_REG;

    if (link_status == LINK_STATUS_RESET) {

        // === LINK-CONNECTION commands

        switch (link_data & LINK_CMD_MASK) {

            case LINK_CMD_INITIATE:
                // Send Ready to send random number seed
                // Then wait for transfer to complete
                link_rand_init = DIV_REG & LINK_DATA_MASK;
                LINK_SEND(LINK_CMD_RANDLO | (link_rand_init & LINK_DATA_MASK));
                break;

            case LINK_CMD_RANDLO:
                // Save incoming low nybble of shared random number seed
                // Then generate the high nybble and send it
                link_rand_init = (link_data & LINK_DATA_MASK) | (DIV_REG & 0xF0);
                LINK_SEND(LINK_CMD_RANDHI | (link_rand_init >> 4) & LINK_DATA_MASK);
                break;

            case LINK_CMD_RANDHI:
                // Save incoming hi nybble of shared random number seed
                link_rand_init |= (link_data & LINK_DATA_MASK) << 4;

                // Send ready to start & Game Type
                //
                // Wait until the transfer is complete then set link status to connected.
                // That should happen at a similar-ish time
                // as the other player processing LINK_CMD_READY
                LINK_SEND(LINK_CMD_READY | (option_game_type & LINK_DATA_MASK));
                while (SC_REG & LINK_XFER_START);

                link_status = LINK_STATUS_CONNECTED;
                break;

            case LINK_CMD_READY:
                // Save/apply incoming game type
                option_game_type = link_data & LINK_DATA_MASK;

                // This will complete the connection
                // should cause both players to start
                // at a similar-ish time
                link_status = LINK_STATUS_CONNECTED;
                break;
        }
    } else if (link_status == LINK_STATUS_CONNECTED) {

        // === IN-GAME commands

        switch (link_data & LINK_CMD_MASK) {

            case LINK_CMD_OPPONENT_LOST:
                    game_state = GAME_WON_LINK_VERSUS;
                break;

            case LINK_CMD_CRUNCHUP:
                    game_crunchups_enqueued++; // enqueue a(nother) crunch-up
                break;

            case LINK_CMD_PAUSE:
                    game_is_paused = TRUE;
                break;

            case LINK_CMD_UNPAUSE:
                    game_is_paused = FALSE;
                break;
        }
    }

// ***** POSSIBLE BUG? ****
// TODO: is there a problem where performing a TX
//       in the ISR and calling LINK_WAIT_RECEIVE
//       TOO SOON SUCH that it dumps or interferes with
//       the transfer in progress?

    // Return to waiting on an external clock (transfer bit not set) to wait for a sender
    LINK_WAIT_RECEIVE;
}



void link_try_connect(void) {

    UINT16 timeout = 0;

    // Show a popup window while waiting to connect
    status_win_popup_init();
    status_win_popup_show(WIN_Y_LINKPOPUP);
    SET_PRINT_TARGET(PRINT_WIN);
    PRINT(2,2, "WAITING FOR\nOTHER PLAYER..."
               "\n\nPRESS B TO CANCEL", 0);

    // Reset the link and sent a connection request
    link_reset();
    link_enable();
    LINK_SEND(LINK_CMD_INITIATE);

    // Wait a couple seconds to see if connection succeeds
    while (link_status != LINK_STATUS_CONNECTED) {

        // yield CPU while waiting
        wait_vbl_done();

        timeout++;
        if (timeout == LINK_CONNECT_TIMEOUT_LEN) {
            __critical {
                link_status = LINK_STATUS_FAILED;
            }
            break;
        } else if ((timeout & LINK_CONNECT_RESEND_MASK) == 0x00) {

            // Re-send the connection request every N frames
            LINK_SEND(LINK_CMD_INITIATE);
        }

        UPDATE_KEYS();
        if (KEY_TICKED(J_B)) {
            __critical {
                link_status = LINK_STATUS_ABORTED;
            }
            break;
        }
    }

    // Connection failed, notify user
    if (link_status == LINK_STATUS_FAILED) {
        PRINT(2,5, "CONNECT FAILED!  \n\nPRESS B TO RETURN", 0);
        waitpadticked_lowcpu(J_B, NULL);
    }

    // Close the popup window
    SET_PRINT_TARGET(PRINT_BKG);
    status_win_popup_hide();
}