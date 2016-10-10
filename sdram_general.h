/* mehPL:
 *    This is Open Source-ish, but NOT GPL. I call it mehPL.
 *    I'm not too fond of long licenses at the top of the file.
 *    Please see the bottom.
 *    Enjoy!
 */


#ifndef __SDRAM_GENERAL_H__
#define __SDRAM_GENERAL_H__

#include _BITHANDLING_HEADER_
#include "pinout.h"

//a/o stZ-2 CKE-testing:
// Stolen from sdrfr-4.00ncf4
// Modified slightly
//  SDR_ -> SDRAM_
//  SDRCMD_ -> SDRAM_COMMAND__
//  OLD NOTES REMOVED: Revisit sdrfr!
//  addition of sdram_general.c (and associated function declarations here)

//This file consists of general SDRAM stuff...
// Not specific to Free-Runners/Implemention, etc...

//Note, now, there's the ability to access multiple SDRAMs separately
// via separate chip-selects
// E.G. Most single-sided DIMMs have 2 chip-selects, double-sided 4
// (And, there's no reason one couldn't have two DIMMs on the same bus with
//  additional chip-selects).
// Each Chip-Select corresponds to a "Device"
// Thus the argument devNum in each function
//
// Note that this does not necessarily directly correspond to the /CS# on 
// the DIMM
// And, handling of the device-number -> Chip-select execution code is
// project/pinout-specific...

//A normal single-sided DIMM has two "devices", each 32-bits wide
// for sdramThingZero, these are the Free-Runner and the Side-Kick.
#define SDRAM_NUM_DEVICES   2

//If you want to perform an action on All "devices" use this for the devNum
#define SDRAM__ALL_DEVICES 0xff

//These need to be detected...
// That, currently (a/o sdramThingZero-19) is in main.c
// What to do about that in a more general-purpose sense...?
// NOTE: There's no reason these *need* to be identical for all devices!
//  E.G. Two DIMMs may exist
//  E.G.2. Some DIMMs have different densities on opposite sides!
//  So, then, does it make sense to have these be arrays...?
//  (and all the extra processing-overhead required, when such cases are
//   rare?)
//  (and, also, I'm about 99% certain a single side of a DIMM, despite
//  having two "devices", will have the same dimensions... so, then, does
//  it really make sense to have two entries for each DIMM side?)
//AS IT STANDS:
// These are not actually *used* within sdram_general
//   NOW USED BY sdram_refreshAll() hmm... hmm...
// But ARE general-purpose SDRAM concepts...
// So, who knows...
extern uint16_t colsPerPage;
extern uint16_t numRows;
extern uint8_t numBanks;




//This initializes the SDRAM to a known-state 
// and configures the mode register
void sdram_init(uint8_t devNum);

//This is somewhat highly architecture-specific... TODO
// (so should maybe be moved to sdramStuff.c?)
// NOT to be called outside of sdram_init();
void sdram_initAddrCmdIO(void);




#define sdram_clockEnable()   setpinPORT(SDRAM_CKE_PIN, SDRAM_CKE_PORT)
#define sdram_clockDisable()  clrpinPORT(SDRAM_CKE_PIN, SDRAM_CKE_PORT)


void sdram_strobeChipSelect(uint8_t devNum);
void sdram_strobeChipSelect_andDisableClock(uint8_t devNum);
void sdram_strobeClockEnable(void);

//NOTE: Re: setupCommand/executeCommand:
// These are basically "internal"
//  These only affect the Command-Pins
//  MOST commands require you also set up the address/data pins
//  And: Many commands require delays or other sequences of events
// THUS: many commands have their own function-calls
//  (that call these, when appropriate)

//Setup a command to the command-pins, but do NOT strobe chip-select
// Thus, the command is INHIBITED until strobeChipSelect()
// Commands are defined below, e.g. SDRAM_CMD__NOP
// This assumes the command/address busses are uC->SDRAM
#define sdram_setupCommand(command) \
      writeMasked((command), SDRAM_CMD_MASK, SDRAM_CMD_PORT)

void sdram_executeCommand(uint8_t devNum, uint8_t command);

void sdram_setupAddress(uint16_t address, uint8_t bankAddress);

//Throw this in sdram_precharge(..., bankAddress=PRECHARGE_ALL)
#define PRECHARGE_ALL   0xff
void sdram_precharge(uint8_t devNum, uint8_t bankAddress);

void sdram_autoRefresh(uint8_t devNum);

//This performs numRows autoRefresh commands
// TODO: numRows is not detected/assigned in sdram_general!
//       Maybe something like "sdram_higherLevel"?
// This is not a low-level command, it's a combination of precharge and
// autoRefresh, so maybe doesn't quite belong here...?
void sdram_refreshAll(uint8_t devNum);


void sdram_activateRow(uint8_t devNum, uint16_t rowNum, uint8_t bankNum);


#if 0
//This doesn't make sense... the burst needs to be sync'd...
void sdram_beginReadBurst(uint8_t devNum, uint8_t colNum, uint8_t bankNum);
#endif

//DQM's...
// Kinda confusing because many programmers use "1" in a "mask" to indicate
// which items are "in the mask" or *enabled* by the mask...
// In SDRAM, a byte is "masked" (disabled) when the DQM is 1
// So, instead, I'm using DisableBytes/EnableBytes
#define sdram_disableAllBytes() \
               writeMasked(SDRAM_DQM_MASK, SDRAM_DQM_MASK, SDRAM_DQM_PORT)

//Less-logical (on sdram_general) to enableAllBytes...
// we'll leave it to main.c for now...

//For general sdram-usage this is only called during initAddrCmdIO()
// Though it'll also be useful for free-running
void sdram_AddrCmdIO_AsOutputs(void);

//Less-logical (on sdram_general) to set them as Inputs...
// we'll leave that to main.c for now...



//I believe this is the case for all SDRAM chips, but not certain
// This is the number of banks within a chip, as opposed to the "banks" of
// chips on a DIMM (e.g. "sides")
//TURNS OUT: Some devices have only 2 banks 
// (and is it possible there are 1-bankers on DIMMs intended for PCs?)
//Probably best to get rid of this in favor of a variable...
// and use SDRAM_BANKADDER_BITS as necessary.
//#define SDRAM_NUM_BANKS 4


//These are internal... align with Micron 64Mb manual, Table14
// These do *not* necessarily align with e.g. SDRAM_nWE_PIN
// (Though, it's certainly easier that way)
#define SDRAM_nCS_BIT  3
#define SDRAM_nRAS_BIT 2
#define SDRAM_nCAS_BIT 1
#define SDRAM_nWE_BIT  0


//This just makes SDRAM_CMD__'s easier to view...
#define SDRAM_nCS      (1<<SDRAM_nCS_BIT)
#define SDRAM_nCS_H       (SDRAM_nCS)
#define SDRAM_nCS_L       (0)
#define SDRAM_nCS_X       (0)
#define SDRAM_nRAS     (1<<SDRAM_nRAS_BIT)
#define SDRAM_nRAS_H      (SDRAM_nRAS)
#define SDRAM_nRAS_L      (0)
#define SDRAM_nRAS_X      (0)
#define SDRAM_nCAS     (1<<SDRAM_nCAS_BIT)
#define SDRAM_nCAS_H      (SDRAM_nCAS)
#define SDRAM_nCAS_L      (0)
#define SDRAM_nCAS_X      (0)
#define SDRAM_nWE      (1<<SDRAM_nWE_BIT)
#define SDRAM_nWE_H       (SDRAM_nWE)
#define SDRAM_nWE_L       (0)
#define SDRAM_nWE_X       (0)


// This *is* Table 14, excluding DQM, Addr, DQ, and notes
//  (NOTE: chip-select is generally masked-out and handled exclusively)

// Inhibit: "Operations already in progress are not affected"
//  CS is inactive. All other pins/ports are Don't-Care.
//  (NOTE: ALL commands are INHIBITED as long as CS is inactive)
#define SDRAM_CMD__INHIBIT \
                  (SDRAM_nCS_H | SDRAM_nRAS_X | SDRAM_nCAS_X | SDRAM_nWE_X)

// NOP: "Operations already in progress are not affected"
//  Same function as INHIBIT, but with CS active
//  Only the command-port is necessary
#define SDRAM_CMD__NOP \
                  (SDRAM_nCS_L | SDRAM_nRAS_H | SDRAM_nCAS_H | SDRAM_nWE_H)
//SEE NOTE FOR PRECHARGE

//This only works because _X = 0, AS DEFINED HERE (above)...
// This is useful in certain places where a stray /CS might come 'round...
#define SDRAM_CMD__NOP_INHIBITED \
                  (SDRAM_CMD__NOP | SDRAM_CMD__INHIBIT)

//Activate (open) a row/bank
// Address indicates the Row, BankAddress indicates the Bank
// sdr_activateRow() should be used, in most cases
//  as the active rows are (somewhat poorly) tracked in code.
#define SDRAM_CMD__ACTIVE \
                  (SDRAM_nCS_L | SDRAM_nRAS_L | SDRAM_nCAS_H | SDRAM_nWE_H)

//Seriously "Active"? Is this not a command? Is that not an action?
#define SDRAM_CMD__ACTIVATE   SDRAM_CMD__ACTIVE


//Begin a read (usually burst) after the CAS-Latency
// Address indicates the start-Column, BankAddress indicates the Bank
// DQM inhibits output on the data-bus (following the DQM-Latency)
#define SDRAM_CMD__READ \
                  (SDRAM_nCS_L | SDRAM_nRAS_H | SDRAM_nCAS_L | SDRAM_nWE_H)
//Begin a write (usually burst) immediately
// Address indicates the start-Column, BankAddress indicates the Bank
// DQM inhibits input on the data-bus from being written to that column
#define SDRAM_CMD__WRITE \
                  (SDRAM_nCS_L | SDRAM_nRAS_H | SDRAM_nCAS_L | SDRAM_nWE_L)
//Stop an active read/write burst
// (Data *could* be on the bus, e.g. during a read-burst, when this is
//  called)
#define SDRAM_CMD__BURST_TERMINATE  \
                  (SDRAM_nCS_L | SDRAM_nRAS_H | SDRAM_nCAS_H | SDRAM_nWE_L)
//Precharge (close/deactivate) an active row/bank
// Address A10 and BankAddress need to be driven as appropriate
// sdr_precharge() should be used, in most cases
//  as the active rows are (somewhat poorly) tracked in code.
#define SDRAM_CMD__PRECHARGE \
                  (SDRAM_nCS_L | SDRAM_nRAS_L | SDRAM_nCAS_H | SDRAM_nWE_L)

//Self-Refresh is usually used for low-power mode(?)
//Self-Refresh has the same command/Address/DQ/DQM-values as Auto-Refresh,
// but CKE must be low...
//  CKE is generally high, so Self-Refresh isn't generally available to 
//  this project. There are other methods of refresh that are more useful
//  for free-running. E.G. Activate->Precharge refreshes that row/bank.
#define SDRAM_CMD__SELFREFRESH    SDRAM_CMD__AUTOREFRESH

//Auto-Refresh, when called regularly, automatically cycles through
// banks/rows refreshing one for each call.
// Address/DQ/DQM are don't-care.
// As I Recall, this can only be called when all banks are inactive.
// see sdr_autoRefresh().
#define SDRAM_CMD__AUTOREFRESH \
                  (SDRAM_nCS_L | SDRAM_nRAS_L | SDRAM_nCAS_L | SDRAM_nWE_H)

//Load Mode Register configures the SDRAM
// Only the Command/Address pins are used
//  (DQ, DQM are don't care)
//  The value to be written to the register is to be loaded by the
//  microcontroller on the Address Bus
#define SDRAM_CMD__LOAD_MODE_REGISTER \
                  (SDRAM_nCS_L | SDRAM_nRAS_L | SDRAM_nCAS_L | SDRAM_nWE_L)
















/* Mode Register */
//These register settings stolen from the Micron 64Mb (?) manual
// Am thinkink they must be the same, roughly, for most SDRAMs
// otherwise, how would a typical (PC) system know what to do with 'em?
// MAYBE in the SPD, but I doubt it.

//A12?!
// Values possibly used here...
// (Best to assume others are Reserved, though I may have excluded a few
//  that didn't seem useful here)
//A:  12  11  10  9  8  7  6  5  4  3  2  1  0
//    \--------/  ^  \--/  \-----/  ^  \-----/
//    Reserved=0  |   0  CASLatency 0   Burst Length
//                |         010=2       0 0 0 = 1
//  0=WriteBurst  |         011=3       0 0 1 = 2
//  1=WriteSingle-/                     0 1 0 = 4
//                                      0 1 1 = 8
//                                      1 1 1 = Full Page
// * Can't use Write-Burst as it's intended, due to uC speed,
//    But it is used for single-writes via DQM.
// * Not sure about the CAS Latency, seems that larger would be handy
//    "The latency can be set to two or three clocks."
//    It's also compatible with most devices.
// * Burst-Length of full-page *wraps repeatedly* and is most-useful for
//    free-running

//THIS IS DEVICE-SPECIFIC -ish...
// I *believe* most SDRAMs can support CASL=3
// So that's what I'm going with.
// It's PLAUSIBLE that changing this, here, won't change *all* the
// necessary code in e.g. sdramThing3.5 and sdrfr.h/c
// So, best to leave this...
/*
#define SDRAM_CAS_LATENCY 3



#if((SDRAM_CAS_LATENCY < 2) || (SDRAM_CAS_LATENCY > 3))
   #error "SDRAM_CAS_LATENCY should be either 2 or 3"
#endif
*/





//These are to be |'d with each other 
// for sdr_init()/sdr_loadModeRegister()
// e.g.  
//  sdr_init(   SDRAM_MODE__PAGEBURST 
//            | SDRAM_MODE__WRITEBURST 
//            | SDRAM_MODE__CASLATENCY);



//To be |'d with other moderegs...
#define SDRAM_MODE__WRITEBURST    (0<<9)
#define SDRAM_MODE__WRITESINGLE   (1<<9)

//Best to use 3... LOTS of reasons
// See notes above for the now defunct SDRAM_CAS_LATENCY
#define SDRAM_MODE__CASLATENCY(cas) ((cas)<<4)


#define SDRAM_MODE__BURST1        (0)
#define SDRAM_MODE__BURST2        (1)
#define SDRAM_MODE__BURST4        (2)
#define SDRAM_MODE__BURST8        (3)
#define SDRAM_MODE__PAGEBURST     (7)





















#ifdef TODO_ERRORS
   // This note is *duplicated* here... as it's a general SDRAM concept
#error "Wasn't there mention somewhere that precharge on an unopen bank is problematic? The table says Precharge on an IDLE row acts as a NOP..."
   //This has been running this way for quite some time... but it's
   //something to think about.
   // Realistically, since the table says-so, then it's probably time to
   // remove this error *and* the "mention somewhere"
#endif






// Buncha really old notes moved to oldUnused.c
#endif //__SDRAM_GENERAL_H__
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
 * /home/meh/_avrProjects/sdramThingZero/21-gittifying/sdram_general.h
 *
 *    (Wow, that's a lot longer than I'd hoped).
 *
 *    Enjoy!
 *    --Esot.Eric (aka EricWazhung)
 */
