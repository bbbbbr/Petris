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

#ifndef SERIAL_LINK_H
#define SERIAL_LINK_H

#define LINK_CLOCK_EXT  0x00U
#define LINK_CLOCK_INT  0x01U
#define LINK_CLOCK_MASK 0x01U

#define LINK_XFER_START 0x80U // This gets cleared at the end of transfer
#define LINK_XFER_MASK  0x80U

// Game serial link command structure
#define LINK_COM_NOCONNECT   0xFFU
#define LINK_COM_CHK_MASK    0xF0U // Mask for command check bits
#define LINK_COM_CHK_XFER    0xA0U // Command transfer bits
#define LINK_COM_CHK_IGNORE  0x50U // Ignore transfer bits
#define LINK_COM_CTRL_MASK   0x0FU // Mask for incoming command bits


// Game serial link commands
#define LINK_COM_INITIATE       0x01U  // Signal that player is ready to start
#define LINK_COM_SYNCRAND       0x02U
#define LINK_COM_READY          0x03U  // Signal that player is ready to start
#define LINK_COM_OPPONENT_LOST  0x04U  // Signal that player lost, so opponent won
#define LINK_COM_CRUNCHUP       0x05U  // Send a crunch-up to the othe player
#define LINK_COM_PAUSE          0x06U
#define LINK_COM_UNPAUSE        0x07U

// Game serial link status
typedef enum {
    LINK_STATUS_RESET = 0x00U,
    LINK_STATUS_ABORTED,
    LINK_STATUS_CONNECTED,
    LINK_STATUS_FAILED
} LINK_STATUSES;


#define LINK_CONNECT_TIMEOUT_LEN (60 * 5) // 5 seconds
//#define LINK_CONNECT_TIMEOUT_RESEND_MASK 0x1F // every 32 frames
#define LINK_CONNECT_RESEND_MASK 0x3FU //every 64 frames

#define WIN_Y_LINKPOPUP  (144 - (8*9)) // 9 tile/font rows tall


extern UINT8 volatile link_status;
extern UINT8 link_rand_init;


// For transmiting, the sequence is important:
// 1. SC_REG -> Set Clock to internal (transfer bit cleared) to be the sender
// 2. SB_REG -> Load data
// 3. SC_REG -> Enable transfer bit (leave clock set to internal)
#define LINK_SEND(command) \
    SC_REG = LINK_CLOCK_INT; \
    SB_REG = command; \
    SC_REG = (LINK_XFER_START | LINK_CLOCK_INT);

// For receiving, the sequence is important:
// 1. SC_REG -> Set Clock to external (transfer bit cleared) to be waiting for external sender
// 2. SB_REG -> Load placeholder data
// 3. SC_REG -> Enable transfer bit (leave clock set to external)
#define LINK_WAIT_RECEIVE \
    SC_REG = LINK_CLOCK_EXT; \
    SB_REG = LINK_COM_CHK_IGNORE; \
    SC_REG = (LINK_XFER_START | LINK_CLOCK_EXT);

void init_link(void);
void link_reset(void);
void link_enable(void);
void link_disable(void);

//void link_send(UINT8 command);
void link_isr(void);
void link_try_connect(void);

#endif // SERIAL_LINK_H