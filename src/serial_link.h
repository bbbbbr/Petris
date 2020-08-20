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

#define LINK_CLOCK_EXT  0x00
#define LINK_CLOCK_INT  0x01
#define LINK_CLOCK_MASK 0x01

#define LINK_XFER_START 0x80 // This gets cleared at the end of transfer
#define LINK_XFER_MASK  0x80

// Game serial link command structure
#define LINK_COM_NOCONNECT   0xFF
#define LINK_COM_CHK_MASK    0xF0 // Mask for command check bits
#define LINK_COM_CHK_XFER    0xA0 // Command transfer bits
#define LINK_COM_CHK_IGNORE  0x50 // Ignore transfer bits
#define LINK_COM_CTRL_MASK   0x0F // Mask for incoming command bits


// Game serial link commands
#define LINK_COM_INITIATE       0x01  // Signal that player is ready to start
#define LINK_COM_SYNCRAND       0x02
#define LINK_COM_READY          0x03  // Signal that player is ready to start
#define LINK_COM_OPPONENT_LOST  0x04  // Signal that player lost, so opponent won
#define LINK_COM_CRUNCHUP       0x05  // Send a crunch-up to the othe player

// Game serial link status
#define LINK_STATUS_RESET      0x00
#define LINK_STATUS_CONNECTED  0x04
#define LINK_STATUS_FAILED     0x05

#define LINK_ROLE_NONE       0x00
#define LINK_ROLE_CONTROLLER 0x01
#define LINK_ROLE_FOLLOWER   0x02


#define LINK_CONNECT_TIMEOUT_LEN (60 * 5) // 5 seconds
//#define LINK_CONNECT_TIMEOUT_RESEND_MASK 0x1F // every 32 frames
#define LINK_CONNECT_RESEND_MASK 0x3F //every 64 frames

#define WIN_Y_LINKPOPUP  (144 - 72)


extern UINT8 volatile link_status;
// extern UINT8 link_role;
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