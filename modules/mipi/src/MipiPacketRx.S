
#include <xs1.h>
#include <xs3a_registers.h>

// XOR      DISABLED
// BYTEREV  DISABLED

/****************************************************
 ****************************************************

Then to receive a packet:

- Load context data from global memory (RxA, RxD)
- If RxA is high, wait for it to go low. (so we don't start mid-packet)
  - Initial version can just do this with polling so we don't need to do any 
    weird jiggery pokery with the RxA low event vector (which we're already
    using for a different purpose).
- Once RxA is low, use IN to wait for the first word of the packet
- Put the first word in the buffer
- If it's a short packet, return
  - Maybe it's a good idea to return the data type?
- Otherwse, do the data loop
- Return
  - Maybe it's a good idea to return the data type?

****************************************************
****************************************************/

/*
In case we need byte reversal or xor for some reason, here's how to do it:
  // { byterev S0, S0              ;                             }
  // { xor S0, S0, r8              ;                             }
After .L_RX_LONG_LOOP:
*/

/*
  See MIPI CSI section 9.1.2

  typedef struct {
    uint8_t data_id;
    uint16_t word_count; // In MIPI a word is a byte
    uint8_t vcx_ecc;
  } mipi_packet_header_t;

  Note: When that 32-bit header is read in on the buffered port, the data_id
  will be the *least* significant 8 bits because of the direction of this port's
  shift register.

  Bits:
    data_id[7:6] - least significant two bits of 4-bit Virtual Channel ID
    data_id[5:0] - data_type
    vcx_ecc[7:6] - most significant two bits of 4-bit Virtual Channel ID
    vcx_ecc[5:0] - error correction code
*/

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


/****************************************************
 ****************************************************

  extern struct {
      uint32_t RxD;
      uint32_t RxA;
  } mipi_context;

  Non-zero return signals error.  

  unsigned MipiPacketRx(uint8_t packet_buffer[]);

  void MipiPacketRx2(
      const unsigned RxD,
      const unsigned RxA,
      streaming chanend c_pkt,
      streaming chanend c_ctrl)

 ****************************************************
 ****************************************************
*/
.align 4
.skip 0
FUNCTION_NAME:
  dualentsp NSTACKWORDS
  std r4, r5, sp[1]
  std r6, r7, sp[2]
  std r8, r9, sp[3]


// Enable MIPI shim hardware
{ ldc r4, 0x01                ; ldc r9, 8                   }
{ shl r4, r4, r9              ;                             }
  ldc r9, XS1_PS_XCORE_CTRL0
  get r11, ps[r9]
{ or r4, r4, r11              ;                             }
  set ps[r9], r4


// Enable RxA event
  setc res[P_RXA], XS1_SETC_COND_EQ // <-- event on edge of RxA
  setc res[P_RXA], XS1_SETC_IE_MODE_EVENT  // <-- do event, not interrupt
  ldc r11, 0  
  setd res[P_RXA], r11   // <-- falling edge
  ldap r11, .L_MIPI_RXA_LOW
  setv res[P_RXA], r11

// Enable events for the thread
  setsr 1  // Sets the EEBLE bit (I couldn't find a #define!)

// Set up interrupt for c_ctrl
// Note: has to be interrupt because we can't have events always enabled.
// Wait, no, events can be enabled/disabled per resource. I should be able to
// use an event...
/*
//////// TODO ///////
*/


#if 0 // astew: I'm not sure if we need this, since we just turned on the shim
      //        but I don't see why we couldn't start receiving a packet while
      //        setting up the RxA event or whatever.


// Check whether the RxA signal is currently high.
//    If so, we need to wait until it is low or we will start reading mid-packet

.L_WAIT_LOOP:
{ peek S0, res[P_RXA]         ;                             }
{ ecallt S0                   ;                             }
{                             ; bt S0, .L_WAIT_LOOP         }
  
#endif

ldc r8, 0x80
shl r9, r8, 8
or r8, r8, r9
shl r9, r8, 16
or r8, r8, r9


.L_GET_NEXT_BUFFER:

{ in BUFF, res[C_PKT]         ;                             }
{ mov BUFF_HEAD, BUFF         ; bf BUFF, .L_EXIT_THREAD     } // <-- exit if null buffer

.L_WAIT_FOR_NEXT_PACKET:

// peek S0, res[P_RXA]
// ecallt S0

// Receive packet header from data port
{ in HEADER, res[P_RXD]       ; ldc S1, 0x00000030          }
{ and S1, S1, HEADER          ; stw HEADER, BUFF[0]         }

// Check whether the header indicates it is a long or short packet.
//   If long, enable the falling-edge event on RxA.
//   If short, we already have the whole packet
{ eet S1, res[P_RXA]          ; bt S1, .L_RX_LONG           }


.L_RX_SHORT:
// A short packet was received.
{ out res[C_PKT], BUFF_HEAD   ; bu .L_GET_NEXT_BUFFER       }


.L_RX_LONG:
// Long packet incoming. The header has been placed into the buffer, but hold on
//   to it until we get the rest of the packet.

{ in S0, res[P_RXD]           ; add BUFF, BUFF, 4           }
.L_RX_LONG_LOOP:
  { add BUFF, BUFF, 4           ; stw S0, BUFF[0]             }
  { in S0, res[P_RXD]           ; bu .L_RX_LONG_LOOP          }

 /****
 *      RxA event vector target.
 * 
 * RxA is a signal coming from the xcore.ai hardware's MIPI shim layer, 
 * signaling high while a packet is being received. On the falling edge of the
 * signal, the packet has been fully received, and this event fires, breaking
 * the thread out of the receive loop.
 * 
 * If the received packet was not a multiple of 4 bytes, there will still be
 * data left in the shift register. This event grabs any remaining data and puts
 * it at the end of the received frame.
 * 
 * When the data has been grabbed, it jumps down to .L_RxLONG_DataEnd
 * 
 * This is only used for LONG frames. Short frames are always exactly 32 bits.
 *****
 */
.align 32
.skip 20
.L_MIPI_RXA_LOW:

// Clear event on RxA port
{ in r11, res[P_RXA]          ;                             } // <-- aligned 20 mod 32
// End input on P_RXD, so we can pull out any remaining bytes without blocking.
{ endin r11, res[P_RXD]       ;                             } // <-- aligned 24 mod 32

// NOTE: r11 will contain a number of BITS remaining, instead of bytes. The 
// .align 32's below just avoid the need to change it to dual-issue instructions
// for the BRU instruction. r11 will be one of {0,8,16,24}
{ in S1, res[P_RXD]           ; bru r11                     } // <-- aligned 28 mod 32


// In each of the 4 cases, the very last byte of the transmission should end
// up in S2. The last byte contains error bits and whatnot

// In the 0 case, the byte with the error details is already in S2, but in the
// most significant bits, so pull it down.
//.... is it already in S2..? If r11 is zero, what did the IN instruction paired
//     with BRU do?  I suspect this is wrong..
//     
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
//   (any bits set in S2 other than LSb indicate an error)
{ edu res[P_RXA]              ; shr S0, S0, 1               }

// Here's how we'll deal with received packet errors for now: If there is a
//   packet error (if S0 is nonzero right here) in this long packet, change the
//   data length in the packet header to 0. The header is already in the buffer,
//   so if the next layer really needs to know what we saw for a packet length, 
//   it can just look in the buffer.
// 
// Actually, for now, just assert on any errors.
{ out res[C_PKT], BUFF_HEAD   ;                    }

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



