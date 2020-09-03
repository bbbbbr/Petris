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
//
// * External Clock + Transfer Start bit set
//
// 1. Consoles go into a DETECTING state and send
//    INITIATE commands that may get a CONFIRM
//
// 2. Either console can try to intitiate GAME_START
//    by sending RANDLO + random low nybble
//
// 3. Receiver responds with sending RANDHI + random high nybble
//
// 4. Receiver responds with READY + game type.
//    After sending of that command is completed then
//    **BOTH** consoles set link status as CONNECTED
//    and try to start the game


// == Link Detecting sequence
//
// 0. start_detect()
//      sets:  LINK_STATUS_RESET -> STATUS_DETECTING
//
// 1. Leader
//    try_connect()
//      sets:  timeout = MAX
//      sends: CMD_INITIATE (passes: rand-lo)
//
// 2. Follower
//    rx: CMD_INITIATE
//      sets:  timeout = MAX
//      sends: CMD_CONFIRM (passes: rand-hi)
//
// 2. Follower
//    rx: CMD_CONFIRM
//      sets:  timeout = MAX
//      sends: <none>



// == Game Starting sequence
//
// 1. Leader
//    try_gamestart()
//      sets:  STATUS_DETECTING -> STATUS_GAME_START
//      sends: CMD_RANDLO (passes: rand-lo)
//
// 2. Follower
//    rx: CMD_RANDLO
//      sets:  STATUS_DETECTING -> STATUS_GAME_START
//      sends: CMD_RANDHI (passes: rand-hi)
//
// 3. Leader
//    rx: CMD_RANDHI
//      sets:  STATUS_GAME_START -> STATUS_CONNECTED (triggers game start)
//      sends: LINK_CMD_START (passes game-type)
//
// 4. Follower
//    rx: LINK_CMD_START
//      sets:  STATUS_GAME_START -> STATUS_CONNECTED (triggers game start)
//      forces: option_game_link2p == OPTION_LINK2P_ON;
//      sends: <none>


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
#include "gfx.h"
#include "gfx_print.h"

#include "status_win.h"

#include "serial_link.h"

#define LINK_STATUS_X 1
#define LINK_STATUS_Y 0


const UINT8 LINK_ICONS_ON[3] = {TILE_ID_2P_LINK_START,     // GB Icon
                                TILE_ID_2P_LINK_START + 1, // Text
                                TILE_ID_2P_LINK_START};    // GB Icon

const UINT8 LINK_ICONS_OFF[3] = {TILE_ID_2P_LINK_OFF,      // All blank tiles
                                 TILE_ID_2P_LINK_OFF,
                                 TILE_ID_2P_LINK_OFF};



UINT8 volatile link_status;
UINT8 volatile link_timeout;

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
        link_timeout = LINK_TIMEOUT_RESET;
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


void link_start_detect(void) {
    link_reset();
    link_enable();
    link_status = LINK_STATUS_DETECTING;
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

        if (link_status == LINK_STATUS_DETECTING) {

            // === LINK-DETECTING commands

            switch (link_data & LINK_CMD_MASK) {

                case LINK_CMD_INITIATE:
                    // (Follower) - DETECT pt 2

                    // Send reply to connection ping
                    // and reset connection timeout
                    LINK_SEND(LINK_CMD_CONFIRM);
                    link_timeout = LINK_TIMEOUT_CONNECT_MAX;
                    link_update_status_icon();
                    break;

                case LINK_CMD_CONFIRM:
                    // (Leader) - DETECT pt. 3

                    // Complete connection ping round trip
                    // and reset connection timeout
                    link_timeout = LINK_TIMEOUT_CONNECT_MAX;
                    link_update_status_icon();
                    break;

                case LINK_CMD_RANDLO:
                    // (Follower) - GAME START pt 2

                    // Save incoming low nybble of shared random number seed
                    // Then generate the high nybble and send it
                    game_rand_init = (link_data & LINK_DATA_MASK) | (DIV_REG & 0xF0);
                    LINK_SEND(LINK_CMD_RANDHI | (game_rand_init >> 4) & LINK_DATA_MASK);
                    //
                    link_status = LINK_STATUS_GAME_START;
                    break;
             }

        } // end: if (link_status == LINK_STATUS_DETECTING)
        else if (link_status == LINK_STATUS_GAME_START) {


            // === LINK-ESTABLISHED -> GAME STARTING commands

            switch (link_data & LINK_CMD_MASK) {

                case LINK_CMD_RANDHI:
                    // (Leader) - GAME START pt 3

                    // Save incoming hi nybble of shared random number seed
                    game_rand_init |= (link_data & LINK_DATA_MASK) << 4;

                    // Send ready to start & Game Type
                    //
                    // Wait until the transfer is complete then set link status to connected.
                    // That should happen at a similar-ish time
                    // as the other player processing LINK_CMD_READY
                    LINK_SEND(LINK_CMD_START | (option_game_type & LINK_DATA_MASK));
                    while (SC_REG & LINK_XFER_START);

                    link_status = LINK_STATUS_CONNECTED;
                    break;

                case LINK_CMD_START:
                    // (Follower) - GAME START pt 4

                    // Save/apply incoming game type
                    // and force 2-player mode option = ON
                    option_game_type = link_data & LINK_DATA_MASK;
                    option_game_link2p == OPTION_LINK2P_ON;

                    // This will complete the connection
                    // should cause both players to start
                    // at a similar-ish time
                    link_status = LINK_STATUS_CONNECTED;
                    break;
            } // if (link_status == LINK_STATUS_RESET)

        }  // end: if (link_status == LINK_STATUS_GAME_START)
        else if (link_status == LINK_STATUS_CONNECTED) {

            // === LINK-CONNECTED: IN-GAME commands

            switch (link_data & LINK_CMD_MASK) {

                case LINK_CMD_OPPONENT_LOST:
                        game_state = GAME_WON_LINK_VERSUS;
                    break;

                case LINK_CMD_CRUNCHUP:
                        // enqueue N crunch-ups triggered by opponent
                        // and start the board shake counter
                        game_shake_enqueued = GAME_CRUNCHUP_SHAKE_START;
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

    if (! (SC_REG & LINK_XFER_START)) {
        LINK_WAIT_RECEIVE;
    }
}



// Periodic connection detection and timeout
// Meant to be called regularly to
// establish a link for -> link_try_gamestart()
void link_try_connect(void) {

    // Send periodic pings if in the idle Link Checking state
    if (link_status == LINK_STATUS_DETECTING) {
        // Periodically ping over link for the presence of another player
        // (About 2x as often as the timeout window)
        if ((sys_time & LINK_CHECK_MASK) == LINK_CHECK_MATCH) {
            LINK_SEND(LINK_CMD_INITIATE);
        }
    }

    // Update the timeout window
    // If it goes to zero then reset the link status
    __critical {
        if (link_timeout) {
            link_timeout--;

            // If link is timed out reset icon
            if (link_timeout == LINK_TIMEOUT_RESET) {
                link_status = LINK_STATUS_DETECTING;
                link_update_status_icon();
            }
        }
    }
}



// Try to initiate the game start sequence if requested by user
void link_try_gamestart(void) {

    // Only try the link start game sequence if already connected
    if ((link_timeout != LINK_TIMEOUT_RESET) &&
        (link_status == LINK_STATUS_DETECTING)) {

        // Reset timeout counter
        __critical {
            link_timeout = LINK_TIMEOUT_CONNECT_MAX;
            link_status = LINK_STATUS_GAME_START;
        }

        // Send Ready to send random number seed
        // Then wait for transfer to complete
        game_rand_init = DIV_REG & LINK_DATA_MASK;
        LINK_SEND(LINK_CMD_RANDLO | (game_rand_init & LINK_DATA_MASK));
    }
}



// Draw the link status icon on the Options and Game Board screens
void link_update_status_icon(void) {

    VBK_REG = 0; // Select regular BG tile map

    // Show a status icon if link is detected or connected, otherwise blank
    if ((link_timeout != LINK_TIMEOUT_RESET) ||
        (link_status == LINK_STATUS_CONNECTED)) {

        set_bkg_tiles(LINK_STATUS_X, LINK_STATUS_Y,
                      ARRAY_LEN(LINK_ICONS_ON),1,
                      LINK_ICONS_ON);
    } else {

        set_bkg_tiles(LINK_STATUS_X, LINK_STATUS_Y,
                      ARRAY_LEN(LINK_ICONS_OFF),1,
                      LINK_ICONS_OFF);
    }
}