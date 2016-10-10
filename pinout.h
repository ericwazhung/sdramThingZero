/* mehPL:
 *    This is Open Source-ish, but NOT GPL. I call it mehPL.
 *    I'm not too fond of long licenses at the top of the file.
 *    Please see the bottom.
 *    Enjoy!
 */


//This is for testing the functionality of CKE
// We'll use an AVR
// With pretty much the same pinout as sT4.5
// The Free-Runner is wired directly to the AVR
// The AVR and SDRAM run at the same clock-speed
//  But, with CKE-testing, the *only* instruction that should be
//  timing-sensitive (and thus in ASM) is the CKE strobe


//This pinout from sT4.5-15, and modified as necessary...
//MANY have been remapped.

// 1  GND
// 2  V+
// 3  SCK   PB7   Rx0 (puar)
// 4  MOSI  PB5   Tx0 (puat)
// 5  /RST
// 6  MISO  PB6   (heart)

// Most devices don't yet have it implemented, so must put actual pin/port
// names here (The commented pin/port names are from another device!)
//Likewise: If you decide to use different pins than those on the
// programming-header, type them here:

#define Rx0pin    _PGM_SCK_PIN_NAME_
#define Rx0PORT   _PGM_SCK_PORT_NAME_


#define Tx0pin    _PGM_MOSI_PIN_NAME_ //PA6     //MOSI
#define Tx0PORT   _PGM_MOSI_PORT_NAME_ //PORTA



#define SDRAM_CKE_PIN   0
#define SDRAM_CKE_PORT  PORTE

#define SDRAM_FR_CS_PIN 1
#define SDRAM_FR_CS_PORT   PORTE

//As it stands: 
// DQMs must be on the same port...
#define SDRAM_FR_DQM_PIN 0
#define SDRAM_DQM_PORT  PORTD

#define SDRAM_DQM_MASK  (1<<(SDRAM_FR_DQM_PIN))

//So far, we're only implementing A0-A12 (A11? Deprecated?)
// Technically, the SDRAM DIMM spec allows for up to A13
//  Combined with BA0-1, this makes 16 bits, total
// But, ADDR_BITS is only referring to A0-An, not including BA0-1
//  (Don't forget to count A0 in the number of bits!)
//Currently, A13 is grounded at the "motherboard"
// So, A13 on the microcontroller is not connected
// But we have A0-A12, which is *13* bits
#define SDRAM_ADDR_BITS 13

//As it stands:
// ADDRL_PORT must contain (in order, LSB-first):
//  Address bits 0-7
// Consuming *ALL* 8-bits on the port
#define SDRAM_ADDRL_PORT   PORTA

//As it stands:
// ADDRH_PORT contains the remainder of the address bits
//  Address bits 8-13 (12,13 N/C)
//  Port-Bits 14 and 15 can be used for other purposes.
#define SDRAM_ADDRH_BITS (SDRAM_ADDR_BITS-8)

#define SDRAM_ADDRH_PORT   PORTC

#define SDRAM_ADDRH_MASK   ((1<<(SDRAM_ADDRH_BITS))-1)

//Bank Address bits (2) should be sequential on a port
//CURRENTLY: BANKADDR SHOULD be the highest two-bits on the ADDRH_PORT
#define SDRAM_BANKADDR_PORT   PORTC
#define SDRAM_BANKADDR_SHIFT  6  //BA0 on bit 6

#define SDRAM_BANKADDR_BITS   2  //BA0, BA1 on the DIMM

#define SDRAM_BANKADDR_MASK   \
            ( ((1<<SDRAM_BANKADDR_BITS)-1) << SDRAM_BANKADDR_SHIFT )
            //(0x3<<SDRAM_BANKADDR_SHIFT)


//nWE, nCAS, and nRAS 
// must be on the same port
// must be in that order
// (To match SDRAM_nWE_BIT, etc.)
#define SDRAM_nWE_PIN   0
#define SDRAM_nCAS_PIN  1
#define SDRAM_nRAS_PIN  2

#define SDRAM_CMD_PORT  PORTB
#define SDRAM_CMD_MASK  ( (1<<SDRAM_nWE_PIN) \
                        | (1<<SDRAM_nCAS_PIN) \
                        | (1<<SDRAM_nRAS_PIN) )


//CURRENTLY: This must be aligned as bit3 on the same port as CMD...
// Thus, loading a command to be written to the DQs matches the SDRAM_CMD__
// values...
#define SDRAM_FR__nCS_DQ_PIN  3
#define SDRAM_FR__nCS_DQ_PORT PORTB

#define SDRAM_FR__CMD_DQ_PORT    SDRAM_CMD_PORT
#define SDRAM_FR__CMD_DQ_MASK    ( SDRAM_CMD_MASK \
                                 | (1<<SDRAM_FR__nCS_DQ_PIN) )

//TODO: Debatable...
//      CKE-Oneshot isn't exactly specific to Free-Running
//      So, *could* be part of sdram_general
//      (as an option?)
#define CKE_ONESHOT_BYPASS_PIN   7
#define CKE_ONESHOT_BYPASS_PORT  PORTD

#define CS_ONESHOT_OVERRIDE_PIN  4
#define CS_ONESHOT_OVERRIDE_PORT PORTD

// (L) = Pulled Low (~10k)
// (H) = Pulled High (~10k)
//  *  = found on IDE-header
//

//                             ATmega8515
//                            ____________________
//                           |         |_|        |
//                SDR_nWE  -*|  1 PB0      VCC 40 |--  +3.3V
//               SDR_nCAS  -*|  2 PB1      PA0 39 |*-  SDR_A0 
//               SDR_nRAS  -*|  3 PB2      PA1 38 |*-  SDR_A1
//              FR_nCS_DQ  -*|  4 PB3      PA2 37 |*-  SDR_A2
//                         --|  5 PB4      PA3 36 |*-  SDR_A3
// (PGM_MOSI)              --|  6 PB5      PA4 35 |*-  SDR_A4
// (PGM_MISO) +-/\/\/\-|>|---|  7 PB6      PA5 34 |*-  SDR_A5
//  (PGM_SCK)              --|  8 PB7      PA6 33 |*-  SDR_A6
//  (PGM_RST) ---------------|  9 /RST     PA7 32 |*-  SDR_A7
//        (H) SDRAM_FR_DQM -*| 10 PD0      PE0 31 |*-  SDR_CKE (H)
//                         --| 11 PD1      PE1 30 |*-  /CS_FreeRunner (H) 
//                         --| 12 PD2      PE2 29 |--  
//                         --| 13 PD3      PC7 28 |*-  SDR_BA1
//     CS_ONESHOT_OVERRIDE --| 14 PD4      PC6 27 |*-  SDR_BA0
//                         --| 15 PD5      PC5 26 |*-  SDR_A13 (N/C)
//                         --| 16 PD6      PC4 25 |*-  SDR_A12 (N/C)
//      CKE_ONESHOT_BYPASS --| 17 PD7      PC3 24 |*-  SDR_A11
//                         --| 18 XTAL2    PC2 23 |*-  SDR_A10
//              XTAL >-------| 19 XTAL1    PC1 22 |*-  SDR_A9
//                         --| 20 GND      PC0 21 |*-  SDR_A8
//                           |____________________| 
//                     
//

//This is from SDRAM_DIMM_Pinout/SDRAM_9_IDEctd.c
/*
                 ------          
      GND   40  | G  o |  39 /CS_FR_READ_DQ * (see below)
     +3V3   38  | o  o |  37 +3V3             
   *  CKE   36  | o  o |  35 /WE   *
      GND   34  | /  o |  33 /CAS  *
   * XTAL   32  | o  o |  31 /RAS  *
      GND   30  | G  o |  29 DQMB0 ----.
     +3V3   28  | /  o |  27 DQMB1 ----+-< SDR_ADDRCMD_DQM (L=data active)
      GND   26  | G  o |  25 DQMB4 ----'
      GND   24  | G  o |  23 DQMB5 ----> 3V3
      GND   22  | G  o  | 21 /CS_FreeRunner * (see below)
        x   20  | X  G  | 19 GND
     *  A0  18  | o  o |  17  A1   *      
     *  A2  16  | o  o |  15  A3   *      
     *  A4  14  | o  o |  13  A5   *       
     *  A6  12  | o  o |  11  A7   *      
     *  A8  10  | o  o |  9   A9   *      
     *  A10  8  | o  o |  7   A11  *     
 GND<---A12  6  | o  o |  5   A13  N/C   
     *  BA0  4  | o  o |  3   BA1  *
        GND  2  | G  ? |  1   +3V3
                 ------        
*/


// MOST Address/Command pins are directly-wired to a DQ for Free-Running
//  (see SDRAM_DIMM_Pinout/SDRAM_[1-8]*)
//
// CHIP-SELECT differs
//  (see SDRAM_DIMM_Pinout/SDRAM_10_ChipSelectPonderances.c)
//
//  There IS a DQ associated with Chip-Select
//  BUT:
//   In order for the host to write/read a bit on that *DQ*,
//    that DQ cannot be *directly* wired to the /CS input!
//   Otherwise, when that DQ is written/read Low,
//    it would activate a command.
//
//-----------------
// For testing sdramThingZero with the CKE test...
//  Was initially thought, maybe, CKE would allow for solving this problem
//  However: e.g. read: CKE-Strobe to advance the burst...
//                       DQ_/CS may be active
//                       Which would load that instruction
//                write: (worse?)
//    (MAYBE: could handle *single* reads/writes, but definitely only one)
//
// TODO: Ponder...?
//
//-------------------------------
// An easy solution, especially for testing this particular phase:
//
// --------                                      --------------------
// Host    |                                    |    SDRAM
//         | /CS_FreeRunner                     |
//     /CS |>----------------------------+----->| /CS
//         |                             |      |
//         |                             /      |
//         |                             \      |
//         |                             /      |
//         | /CS_FR_READ_DQ              |      |
//  /DQ_CS |>----------------------------+----><| /DQ_CS
//         |                                    |
//


/* mehPL:
 *    I would love to believe in a world where licensing shouldn't be
 *    necessary; where people would respect others' work and wishes, 
 *    and give credit where it's due. 
 *    A world where those who find people's work useful would at least 
 *    send positive vibes--if not an email.
 *    A world where we wouldn't have to think about the potential
 *    legal-loopholes that others may take advantage of.
 *
 *    Until that world exists:
 *
 *    This software and associated hardware design is free to use,
 *    modify, and even redistribute, etc. with only a few exceptions
 *    I've thought-up as-yet (this list may be appended-to, hopefully it
 *    doesn't have to be):
 * 
 *    1) Please do not change/remove this licensing info.
 *       (You can add notes *surrounding* it!)
 *    2) Please do not change/remove others' credit/licensing/copyright 
 *         info, where noted. 
 *    3) If you find yourself profiting from my work, or derivatives,
 *         thereof, please send me a beer, a trinket, or cash is always 
 *         handy as well.
 *         (Please be considerate. E.G. if you've reposted my work on a
 *          revenue-making (ad-based) website, please think of the
 *          years and years of hard work that went into this!)
 *    4) If you/your company *intend(s)* to profit from my work, 
 *         you must get my permission, first. 
 *    5) No permission is given for my work to be used in Military, NSA,
 *         or other creepy-ass purposes. No exceptions. And if there's 
 *         any question in your mind as to whether your project qualifies
 *         under this category, you must get my explicit permission.
 *
 *    The open-sourced project this originated from is ~98% the work of
 *    Me, "Esot.Eric," except where otherwise noted.
 *    That includes the "commonCode" and makefiles.
 *    Thanks, of course, should be given to those who worked on the tools
 *    I've used: avr-dude, avr-gcc, gnu-make, vim, usb-tiny, and 
 *    I'm certain many others. 
 *    And, as well, to the countless coders who've taken time to post
 *    solutions to issues I couldn't solve, all over the internets.
 *
 *
 *    I'd love to hear of how this is being used, suggestions for
 *    improvements, etc!
 *         
 *    The creator of the original code and original hardware can be
 *    contacted at:
 *
 *        EricWazHung At Gmail Dotcom ("Me")
 *
 *    This code's origin (and latest versions) can be found at:
 *          https://github.com/ericwazhung
 *      or: https://github.com/esot-eric-test
 *      or Plausibly under a project page at: 
 *          https://hackaday.io/hacker/40107-esoteric
 *
 *    Older versions may be locatable at: (no longer updated)
 *        https://code.google.com/u/ericwazhung/
 *
 *
 *    The site associated with the original open-sourced project is at:
 *
 *        https://sites.google.com/site/geekattempts/
 *
 *    If any of that ever changes, I will be sure to note it here, 
 *    and add a link at the pages above.
 *
 * This license added to the original file located at:
 * /home/meh/_avrProjects/sdramThingZero/21-gittifying/pinout.h
 *
 *    (Wow, that's a lot longer than I'd hoped).
 *
 *    Enjoy!
 *    --Esot.Eric (aka EricWazhung)
 */
