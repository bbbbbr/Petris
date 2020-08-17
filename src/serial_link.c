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


UINT8 link_status;
UINT8 link_role;
UINT8 link_rand_init;

// #define LINK_ROLE_NONE       0x00
// #define LINK_ROLE_CONTROLLER 0x01
// #define LINK_ROLE_FOLLOWER   0x02

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

    link_status = LINK_STATUS_RESET;

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
                    //
                    // ? Apparently it's not necessary to re-enable
                    // LINK_XFER_START for the second transfer
                    // to start?
// TODO: Does this need a timeout? possible infinite loop?
// Splitting into RAND_LO/HI commands could avoid this
// But maybe it's not realy an issue if the first byte comes through ok
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
                break;

            case LINK_COM_EXIT_GAME:
                break;

            case LINK_COM_CRUNCHUP:
                game_crunchups_enqueued++; // enqueue a(nother) crunch-up
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

    // UINT8 timeout = 0;
    UINT16 timeout = 0;

    link_reset();

    link_enable();

    PRINT(1,1, "WAITING TO CONNECT", 0);

    LINK_SEND(LINK_COM_CHK_XFER | LINK_COM_INITIATE);

    while (link_status != LINK_STATUS_CONNECTED) {

        timeout++;
        if (timeout == (60 * 15)) {
            link_status = LINK_STATUS_FAILED;
            break;
        //} else if ((timeout & 0x3F) == 0x20) { // every 32 frames
        } else if ((timeout & 0x7F) == 0x00) { // every 64 frames

            // Send a periodic request
            LINK_SEND(LINK_COM_CHK_XFER | LINK_COM_INITIATE);
        }

        // TODO: allow user to break out?
        // UPDATE_KEYS();
        // if (KEY_TICKED(J_START)) {

        // mostly yield CPU while waiting
        wait_vbl_done();
    }
}