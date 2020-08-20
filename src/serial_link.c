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
    SB_REG = LINK_COM_CHK_IGNORE;
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

    // Make sure it's a command packet (upper nybble)
    if ((link_data & LINK_COM_CHK_MASK) == LINK_COM_CHK_XFER) {

// THIS seems to work
// but it could probably be implemented more simply
// Rand Low/Rand Hi commands instead?
// #define LINK_COM_CHK_RANDLO  0xB0 // Ignore transfer bits
// #define LINK_COM_CHK_RANDHI  0xB0 // Ignore transfer bits
// TODO: reduce number of control mask bits???
// Create built-in payload bits?

        // Select command (lower nybble)
        switch (link_data & LINK_COM_CTRL_MASK) {

            case LINK_COM_INITIATE:
                // Lock out initiating a connection if already connected
                if (link_status == LINK_STATUS_RESET) {

                    // Send Ready to send random number seed
                    // Then wait for transfer to complete
                    LINK_SEND(LINK_COM_CHK_XFER | LINK_COM_SYNCRAND);
                    while (SC_REG & LINK_XFER_START);

                    // Generate a seed for the random number generator
                    // and send it to the other player
                    link_rand_init = DIV_REG >> 1;
                    LINK_SEND(link_rand_init);

                }
                break;


            case LINK_COM_SYNCRAND:

                if (link_status == LINK_STATUS_RESET) {
                    // Wait to receive the second byte which
                    // is the seed for the random number generator
                    LINK_WAIT_RECEIVE;
                    while (SC_REG & LINK_XFER_START);

                    // Save seed from Serial Link Reg
                    link_rand_init = SB_REG;

                    // Send ready to start
                    // Wait until the transmit is complete
                    // then set link status to connected.
                    // That should happen at a similar-ish time
                    // as the other player processing LINK_COM_READY
                    LINK_SEND(LINK_COM_CHK_XFER | LINK_COM_READY);
                    while (SC_REG & LINK_XFER_START);
                    link_status = LINK_STATUS_CONNECTED;
                }
                break;

            case LINK_COM_READY:
                // Send immediate reply of
                link_status = LINK_STATUS_CONNECTED;
                break;

            case LINK_COM_OPPONENT_LOST:
                if (link_status == LINK_STATUS_CONNECTED)
                    game_state = GAME_WON_LINK_VERSUS;
                break;

            case LINK_COM_CRUNCHUP:
                if (link_status == LINK_STATUS_CONNECTED)
                    game_crunchups_enqueued++; // enqueue a(nother) crunch-up
                break;

            case LINK_COM_PAUSE:
                if (link_status == LINK_STATUS_CONNECTED)
                    game_is_paused = TRUE;
                break;

            case LINK_COM_UNPAUSE:
                if (link_status == LINK_STATUS_CONNECTED)
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
    LINK_SEND(LINK_COM_CHK_XFER | LINK_COM_INITIATE);

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
            LINK_SEND(LINK_COM_CHK_XFER | LINK_COM_INITIATE);
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