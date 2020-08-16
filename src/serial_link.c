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


// TODO: what if link is lost?
// TODO: icon to indicate link status? "-vs-" or "]--["


// If a serial transfer with internal clock is performed and no external GameBoy
// is present, a value of $FF will be received in the transfer.


#include <gb/gb.h>
#include <gb/cgb.h> // Include cgb functions
#include <stdlib.h>

#include "serial_link.h"

#include "input.h"
#include "gameplay.h"
#include "gfx_print.h"


UINT8 printat_y;

UINT8 link_status;
UINT8 link_role;

// #define LINK_ROLE_NONE       0x00
// #define LINK_ROLE_CONTROLLER 0x01
// #define LINK_ROLE_FOLLOWER   0x02

// Initialize ISR
void init_link(void) {

    // NOTE: IE flag doesn't get set here, do that in link_enable instead

    // Add interrupt handler for serial link rx and enable it
    disable_interrupts();

    // remove default GBDK serial ISR
    // remove_SIO(serial_IO);
// TODO FIXME
// crt.s

//         ;; Install interrupt routines
//         LD      BC,#.vbl
//         CALL    .add_VBL
//         LD      BC,#.serial_IO
//         CALL    .add_SIO



    add_SIO(link_isr);

    enable_interrupts();
}


void link_reset(void) {

    link_status = LINK_STATUS_RESET;

    // Set to external clock as default
    // Load a placeholder byte into the transfer register
    SC_REG = LINK_CLOCK_EXT;
    SB_REG = LINK_COM_CHK_IGNORE;
}


void link_enable(void) {

    // Enable link ISR
    disable_interrupts();
    set_interrupts(VBL_IFLAG | SIO_IFLAG);
    enable_interrupts();
}


void link_disable(void) {

    disable_interrupts();

    set_interrupts(VBL_IFLAG); // remove SIO flag
    link_reset();

    enable_interrupts();
}


// TODO: Test for failed sending? Ref GBDK IO_FAILED

//#define LINK_SEND(command) SC_REG = LINK_CLOCK_INT; SB_REG = command; SC_REG = (LINK_XFER_START | LINK_CLOCK_INT);
// void link_send(UINT8 command) {

//     // Load command and send it
//     SB_REG = command;
//     SC_REG = (LINK_XFER_START | LINK_CLOCK_INT);
// }



// LINK does not "receive" a byte until TX is enabled?

void link_isr(void) {

    UINT8 link_data;

    link_data = SB_REG;

    // Make sure it's a command packet (upper nybble)
    if ((link_data & LINK_COM_CHK_MASK) == LINK_COM_CHK_XFER) {

        // Select command (lower nybble)
        switch (link_data & LINK_COM_CTRL_MASK) {

            case LINK_COM_INITIATE:

// Lock out initiating a connection if already connected
if (link_status == LINK_STATUS_RESET) {
                // Acknowledge ready to connect
                LINK_SEND(LINK_COM_CHK_XFER | LINK_COM_READY);

                // Wait until the transmit is complete
                // then set link status to connected.
                // That should roughly line up with the
                // other player processing LINK_COM_READY
                while (SC_REG & LINK_XFER_START);
                link_status = LINK_STATUS_CONNECTED;
                PRINT(2,printat_y++,"IN",0);
} else {
                PRINT(2,printat_y++,"NAK",0);
}
                break;

            case LINK_COM_READY:
                // Send immediate reply of
                link_status = LINK_STATUS_CONNECTED;
                PRINT(2,printat_y++,"RD",0);
                break;

            case LINK_COM_OPPONENT_LOST:
                break;

            case LINK_COM_EXIT_GAME:
                break;

            case LINK_COM_CRUNCHUP:
                game_crunchups_enqueued++; // enqueue a(nother) crunch-up
                break;
        }
    }
//    else {
        // enabling this seems to break the code
        // print_num_u16(2,printat_y++,(UINT16)link_status,3);
//    }

// TODO: should the controller end with clock control after Link?

    // Return to waiting on an external clock
    // Load a placeholder byte into the transfer register to avoid
// TODO Does this need a LINK_XFER_START?
//      to queue a read?
//      can reads even be queued, or do they have
//      to be polled?
    // Select external clock
    // Load placeholder data
    // Set ready for transfer
    SC_REG = LINK_CLOCK_EXT;
    SB_REG = LINK_COM_CHK_IGNORE;  // Should be set before XFER_START
    SC_REG = LINK_XFER_START | LINK_CLOCK_EXT;
//    SC_REG = LINK_CLOCK_EXT;
// }
}



void link_try_connect(void) {

    // UINT8 timeout = 0;
    UINT16 timeout = 0;

printat_y = 2;

    link_reset();

    link_enable();

    PRINT(1,1, "WAITING TO CONNECT\n--\n--\n--\n--", 0);

// TODO: why does it work perfectly the first time, but fail every time after, despite reset?
// Disabling below seems to fix the behavior
//  -- WHY
// (SENDER correctly shows RD, not just RECEIVER showing IN)
// ---->< ANSWER - it does work, it's just INSTANT :)
    LINK_SEND(LINK_COM_CHK_XFER | LINK_COM_INITIATE);

    while (link_status != LINK_STATUS_CONNECTED) {

        // This could be tightened up to improve sync of timing
        timeout++;
        // if (timeout == 255) {
         if (timeout == (60 * 15)) {
            link_status = LINK_STATUS_FAILED;
            break;
        //} else if ((timeout & 0x3F) == 0x20) { // every 32 frames
        } else if ((timeout & 0x7F) == 0x00) { // every 64 frames

        // UPDATE_KEYS();
        // if (KEY_TICKED(J_START)) {
            // Send a periodic request
            LINK_SEND(LINK_COM_CHK_XFER | LINK_COM_INITIATE);
        }

        // TODO: allow user to break out?

        // mostly yield CPU while waiting
        wait_vbl_done();
    }
}