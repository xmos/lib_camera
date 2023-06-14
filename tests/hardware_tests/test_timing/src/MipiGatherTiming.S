
#include <xs1.h>
#include <xs3a_registers.h>

/**
 * 
 * This function is a way to gather timing information about a device so that
 * its behavior can be reasoned about.
 * 
 * The output is a table  uint32_t[N][3], where each row corresponds to one 
 * received packet. The first column is the packet header (containing data type
 * and packet length). The second column is when the header was received. The
 * third column is when the end of the packet is received.
 * 
 * This function returns when the table has been filled.
 * 
 * This function throws away all packet data other than the headers.
 * 
*/

// Wait for EOF before populating the packet log. Useful to ensure a clean log.
#ifndef WAIT_FOR_EOF
#define WAIT_FOR_EOF (1)
#endif


.issue_mode dual

#define FUNCTION_NAME   MipiGatherTiming
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
#define TABLE     r2
#define LEN       r3

#define HEADER    r4
#define S0        r5
#define S1        r6
#define _12       r7


/****************************************************
 ****************************************************

  typedef struct {
    uint32_t header;
    uint32_t start_time;
    uint32_t end_time;
  } packet_timing_t;


  void MipiGatherTiming(
      const unsigned RxD,
      const unsigned RxA,
      packet_timing_t table[],
      const unsigned N);

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
{ shl r4, r4, r9              ; sub LEN, LEN, 1             }
  ldc r9, XS1_PS_XCORE_CTRL0
  get r11, ps[r9]
{ or r4, r4, r11              ; ldc _12, 12                 }
  set ps[r9], r4


// Enable RxA event
  setc res[P_RXA], XS1_SETC_COND_EQ // <-- event on logic value of RxA
  setc res[P_RXA], XS1_SETC_IE_MODE_EVENT  // <-- do event, not interrupt
  ldc r11, 0  
  setd res[P_RXA], r11   // <-- condition is low logic value
  ldap r11, .L_MIPI_RXA_LOW
  setv res[P_RXA], r11

// Enable events for the thread
  setsr 1  // Sets the EEBLE bit (I couldn't find a #define!)

#if (WAIT_FOR_EOF)

  ldap r11, .L_MULP
  setv res[P_RXA], r11

// Wait to receive an end-of-frame packet before collecting data
.L_BLORT:
{ in HEADER, res[P_RXD]       ; ldc S1, 0x00000030          }
{ zext HEADER, 6              ; and S1, S1, HEADER          }
{ eet S1, res[P_RXA]          ; bt S1, .L_SPOOT             }
{ eq S1, HEADER, 1            ;                             }
{                             ; bf S1, .L_BLORT             }
// It is an EOF packet
  ldap r11, .L_MIPI_RXA_LOW
{ setv res[P_RXA], r11        ; bu .L_GOT_EOF               }


.L_SPOOT: // long packet. Just burn the data
          // an RxA low event will break us out
  { in S0, res[P_RXD]           ;                             }
  {                             ; bu .L_SPOOT                 }


.align 32
.L_MULP:

{ in r11, res[P_RXA]          ;                             } 
{ endin r11, res[P_RXD]       ;                             } 
{ in S1, res[P_RXD]           ;                             } 
{ edu res[P_RXA]              ;                             }
{ setsr 1                     ; bu .L_BLORT                 }

.L_GOT_EOF:

#else 

// Check whether the RxA signal is currently high.
//    If so, we need to wait until it is low or we will start reading mid-packet
.L_WAIT_LOOP:
{ peek S0, res[P_RXA]         ;                             }
{                             ; bt S0, .L_WAIT_LOOP         }

#endif

.L_WAIT_FOR_NEXT_PACKET:

// Receive packet header from data port
{ in HEADER, res[P_RXD]       ; ldc S1, 0x00000030          }
{ gettime r11                 ; stw HEADER, TABLE[0]        }
{ and S1, S1, HEADER          ; stw r11, TABLE[1]           }

// If short packet, duplicate timestamp into column 3. 
// Otherwise, wait for end of packet.
{ eet S1, res[P_RXA]          ; bt S1, .L_RX_LONG           }

.L_RX_SHORT:

{ add TABLE, TABLE, _12       ; stw r11, TABLE[2]           }
{ sub LEN, LEN, 1             ; bf LEN, .L_EXIT_FUNC        }
{                             ; bu .L_WAIT_FOR_NEXT_PACKET  }


.L_RX_LONG:
// Burn through the rest of the packet. We don't care about the payload.
.L_RX_LONG_LOOP:
  { in S0, res[P_RXD]           ;                           }
  {                             ; bu .L_RX_LONG_LOOP        }


.align 32
.L_MIPI_RXA_LOW:

{ gettime r11                 ;                             } 
{ in r11, res[P_RXA]          ; stw r11, TABLE[2]           } 
{ endin r11, res[P_RXD]       ; add TABLE, TABLE, _12       } 
{ in S1, res[P_RXD]           ;                             } 
{ edu res[P_RXA]              ;                             }
{ sub LEN, LEN, 1             ; bf LEN, .L_EXIT_FUNC        }
{ setsr 1                     ; bu .L_WAIT_FOR_NEXT_PACKET  }


.L_EXIT_FUNC:

  // This doesn't actually undo the event enabling and whatnot, but that's fine.

  ldd r8, r9, sp[3]
  ldd r6, r7, sp[2]
  ldd r4, r5, sp[1]
  retsp NSTACKWORDS

.size FUNCTION_NAME, .-FUNCTION_NAME
.cc_bottom FUNCTION_NAME.func



