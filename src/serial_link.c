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
// * External Clock + Transfer Start bit set
//
// 1. Either console can initiate connection by sending
//    a INITIATE command.
//
// 2. Receiver responds with RANDLO + random low nybble
//
// 3. Initiator responds with RANDHI + random high nybble
//
// 3. Receiver responds with READY + game type.
//    After sending of that command is completed then
//    **BOTH** consoles immediately set link status as CONNECTED
//

//
// State A   :      Command     : State B   : Data
// -----------------------------------------------
// RESET     :                  : RESET     :
// RESET     :  -> Initiate ->  : RESET     :
// RESET     :  <- Rand Lo  <-  : RESET     : Rand Lo
// RESET     :  -> Rand Hi  ->  : RESET     : Rand Hi
// CONNECTED :  <- Ready    <-  : CONNECTED : Game Type
//

// From the docs...
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
    SC_REG = LINK_CLOCK_EXT | LINK_SPEED_FAST;
    SB_REG = LINK_CMD_IGNORE;
}


void link_enable(void) {

    // Enable link ISR
    disable_interrupts();
    set_interrupts(IE_REG |= SIO_IFLAG);
    enable_interrupts();
    LINK_WAIT_RECEIVE;
}


void link_disable(void) {

    // Disable link ISR
    disable_interrupts();
    set_interrupts(IE_REG &= ~SIO_IFLAG);
    enable_interrupts();
    link_reset();
}


// Gets called after a successful RX *OR* TX
//
// Is used to both receive new data and allow the
// sender to return to passive receiver waiting state
void link_isr(void) {

    UINT8 link_data;

    link_data = SB_REG;

    // === Handle TX Completed ===
    // (Assume internal clock enabled
    //  means GB was in sending mode)
    if (SC_REG & LINK_CLOCK_INT) {

        // Now that transfer out is complete return to waiting state
        // (external clock + transfer bit set)
        LINK_WAIT_RECEIVE;

    } // === Handle RX New Data ===
    else {

        if (link_status == LINK_STATUS_RESET) {

            // === LINK-CONNECTION commands

            switch (link_data & LINK_CMD_MASK) {

                case LINK_CMD_INITIATE:
                    // Send Ready to send random number seed
                    // Then wait for transfer to complete
                    game_rand_init = DIV_REG & LINK_DATA_MASK;
                    LINK_SEND(LINK_CMD_RANDLO | (game_rand_init & LINK_DATA_MASK));
                    break;

                case LINK_CMD_RANDLO:
                    // Save incoming low nybble of shared random number seed
                    // Then generate the high nybble and send it
                    game_rand_init = (link_data & LINK_DATA_MASK) | (DIV_REG & 0xF0);
                    LINK_SEND(LINK_CMD_RANDHI | (game_rand_init >> 4) & LINK_DATA_MASK);
                    break;

                case LINK_CMD_RANDHI:
                    // Save incoming hi nybble of shared random number seed
                    game_rand_init |= (link_data & LINK_DATA_MASK) << 4;

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
            } // if (link_status == LINK_STATUS_RESET)

        } else if (link_status == LINK_STATUS_CONNECTED) {

            // === IN-GAME commands

            switch (link_data & LINK_CMD_MASK) {

                case LINK_CMD_OPPONENT_LOST:
                        game_state = GAME_WON_LINK_VERSUS;
                    break;

                case LINK_CMD_CRUNCHUP:
                        // enqueue N crunch-ups triggered by opponent
                        game_crunchups_enqueued += link_data & LINK_DATA_MASK;
                    break;

                case LINK_CMD_PAUSE:
                        game_is_paused = TRUE;
                    break;

                case LINK_CMD_UNPAUSE:
                        game_is_paused = FALSE;
                    break;
            }
        } // else if (link_status == LINK_STATUS_CONNECTED)
    } // else for: if (SC_REG & LINK_CLOCK_INT)

    // Only return to waiting state (external clock + transfer bit set)
    // if a transfer is NOT in progress (likely initiated in this ISR)
    // which is indicated by TX bit set

    // TODO: cleanup if ok
    // FIXME Use instead? (!(SC_REG & LINK_XFER_START))
    // if (!(SC_REG & LINK_CLOCK_INT)) {
    if (! (SC_REG & LINK_XFER_START)) {
        LINK_WAIT_RECEIVE;
    }
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