// Copyright 2023-2024 XMOS LIMITED.
// This Software is subject to the terms of the XMOS Public Licence: Version 1.

#include <xs1.h>
#include <xs3a_registers.h>

.issue_mode dual

#define FUNCTION_NAME   MipiPacketRx
#define NSTACKWORDS     8

.globl FUNCTION_NAME.nstackwords
.globl FUNCTION_NAME.maxthreads
.globl FUNCTION_NAME.maxtimers
.globl FUNCTION_NAME.maxchanends

.linkset FUNCTION_NAME.nstackwords, NSTACKWORDS
.linkset FUNCTION_NAME.maxchanends, 0
.linkset FUNCTION_NAME.maxtimers,   0
.linkset FUNCTION_NAME.maxthreads,  0

.globl FUNCTION_NAME
.type FUNCTION_NAME, @function
.text
.cc_top FUNCTION_NAME.func, FUNCTION_NAME

#define P_RXD     r0
#define P_RXA     r1
#define C_PKT     r2
#define BUFF      r3

#define HEADER    r4
#define S0        r5
#define S1        r6
#define BUFF_HEAD r7

#define WAIT_FOR_CLEAN_PACKET 0

/* -----------------------------------------------------------------------

MipiPacketRx
============

Mipi driver for receiving packets from the MIPI shim hardware.

This function reads a 32-bit packet, it determines either if it a shor packet or a long packet.
If it is a short packet, it sends the packet to the c_pkt channel and waits for the next packet.
If it is a long packet, it reads the rest of the packets until a falling event on Rxa is detected,
where then the whole packet and buffer is sent to the c_pkt channel.

Finally there is some logic to handle the remaining bits. 

If a Null buffer is received, the function exits.

void MipiPacketRx(
    const unsigned RxD,
    const unsigned RxA,
    streaming chanend c_pkt,
    streaming chanend c_ctrl);
----------------------------------------------------------------------- */


.align 4
.skip 0
FUNCTION_NAME:
  dualentsp NSTACKWORDS
  std r4, r5, sp[1]
  std r6, r7, sp[2]
  std r8, r9, sp[3]


// Enable RxA event
  setc res[P_RXA], XS1_SETC_COND_EQ         // <-- event on edge of RxA
  setc res[P_RXA], XS1_SETC_IE_MODE_EVENT   // <-- do event, not interrupt
  ldc r11, 0  
  setd res[P_RXA], r11                      // <-- falling edge
  ldap r11, .L_MIPI_RXA_LOW                 // <-- adress to go on event
  setv res[P_RXA], r11                      // <-- set the event  

// Enable events for the thread
  setsr 1  // Sets the EEBLE bit (I couldn't find a #define!)
           // TODO: get more information on this becasue not sure is the EEBLE bit.  

// Note: if needed, other events could be activated here.
// For instance, c_ctrl could be used to skip one row so the bayer order changes
// or drop some packets if we want a shorter data buffer.

#if WAIT_FOR_CLEAN_PACKET 
// Check whether the RxA signal is currently high.
//    If so, we need to wait until it is low or we will start reading mid-packet
// astew: I'm not sure if we need this, since we just turned on the shim
//        but I don't see why we couldn't start receiving a packet while
//        setting up the RxA event or whatever.

.L_WAIT_LOOP:
{ peek S0, res[P_RXA]         ;                             }
{ ecallt S0                   ;                             }
{                             ; bt S0, .L_WAIT_LOOP         }
#endif


.L_GET_NEXT_BUFFER:
// Get the pointer to the packet structure, and read data
//    If null buffer, exit. Otherwise, continue
{ in BUFF, res[C_PKT]         ;                             }
{ mov BUFF_HEAD, BUFF         ; bf BUFF, .L_EXIT_THREAD     } // exit if null buffer


.L_WAIT_FOR_NEXT_PACKET:
// Receive packet header from data port
// S1 : 0x30 if long, 0x00 if short
// We store the header in the buffer
//   If long, enable the falling-edge event on RxA (eet S1)
//   If short, we already have the whole packet 
{ in HEADER, res[P_RXD]       ; ldc S1, 0x00000030          }
{ and S1, S1, HEADER          ; stw HEADER, BUFF[0]         } 
{ eet S1, res[P_RXA]          ; bt S1, .L_RX_LONG           }


.L_RX_SHORT:
// A short packet was received, S1=0.
{ out res[C_PKT], BUFF_HEAD   ; bu .L_GET_NEXT_BUFFER       }


.L_RX_LONG:
// Long packet incoming, S1=0x30
// We store the header in the buffer
// Then we store image data in the buffer
// This loop is unconditional until RxA events keeps high.
{ in S0, res[P_RXD]           ; add BUFF, BUFF, 4           }
.L_RX_LONG_LOOP:
  { add BUFF, BUFF, 4           ; stw S0, BUFF[0]             }
  { in S0, res[P_RXD]           ; bu .L_RX_LONG_LOOP          }


.align 32
.skip 20
.L_MIPI_RXA_LOW:
// RXA low event has occurred. 
// We need to pull out any remaining bytes from the RXD port.
// Clear event on RxA port
// Get the remaining bits from the RXD port. 
// r11 will be one of {0,8,16,24}
{ in r11, res[P_RXA]          ;                             } // <-- aligned 20 mod 32
{ endin r11, res[P_RXD]       ;                             } // <-- aligned 24 mod 32 
{ in S1, res[P_RXD]           ; bru r11                     } // <-- aligned 28 mod 32

// NOTE 1: r11 will contain a number of BITS remaining, instead of bytes. The 
// .align 32's below just avoid the need to change it to dual-issue instructions
// for the BRU instruction.

// NOTE 2: The last byte contains error bits. 
// Debug operations can be performed analysing if this last byte is 0 or not.
// In this case we assume mipi shim has already corrected any potential issues. 

.L_RXA_OUT_TAIL0: // (0 byte tail)
{ shr S0, S0, 24              ;                             } // <-- aligned 0 mod 32
  bu .L_RXA_DATA_END  

.align 32
.L_RXA_OUT_TAIL1: // (1 Tail Byte; the status byte) 
{ shr S0, S1, 24              ;                             }
{                             ; stw S0, BUFF[0]             }
{                             ; bu .L_RXA_DATA_END          }

.align 32
.L_RXA_OUT_TAIL2: // (2 Tail Bytes)
{ shr S1, S1, 16              ;                             }
{ shr S0, S1, 8               ; stw S1, BUFF[0]             }
{                             ; bu .L_RXA_DATA_END          }

.align 32
.L_RXA_OUT_TAIL3: // (3 Tail Bytes)
{ shr S1, S1, 8               ;                             }
{ shr S0, S1, 16              ; stw S1, BUFF[0]             }
{                             ; bu  .L_RXA_DATA_END         }

.align 8
.L_RXA_DATA_END:

// We've received the full packet and S0 contains the status byte
// Disable the RxA event
{ edu res[P_RXA]              ; shr S0, S0, 1               }
{ out res[C_PKT], BUFF_HEAD   ;                             }

// And now we need a new buffer
// Also reenable events.
{ setsr 1                     ; bu .L_GET_NEXT_BUFFER       }


.L_EXIT_THREAD:

  ldd r8, r9, sp[3]
  ldd r6, r7, sp[2]
  ldd r4, r5, sp[1]
  retsp NSTACKWORDS

.size FUNCTION_NAME, .-FUNCTION_NAME
.cc_bottom FUNCTION_NAME.func
