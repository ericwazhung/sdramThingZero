/* mehPL:
 *    This is Open Source-ish, but NOT GPL. I call it mehPL.
 *    I'm not too fond of long licenses at the top of the file.
 *    Please see the bottom.
 *    Enjoy!
 */


#ifndef __SDRAM_FREE_RUNNER_H__
#define __SDRAM_FREE_RUNNER_H__

#include "sdram_oneShots.h"

//TODO: Remove this shizzle
void delay(void);


//For functions/macros in sdram_general that are used with the Free-Runner,
//the Free-Runner's "Device-Number" is 0
//This basically just tells the function/macro which /CS line to strobe
// And, obviously, some sdram_general functions CAN NOT be used with the
// Free-Runner, such as Read/Write, etc. which need special handling.
#define SDRAM_FR_DEVNUM 0


//Load A Command To The Free-Runner's DQ's for *writing* *that command*
// INTO the Free-Runner
// (Rather'n for *executing* that command)
//Takes e.g. SDRAM_CMD__ACTIVE and routes /CS to the appropriate DQ
#define sdramFR_setupFRcmd(cmd)  \
({ \
   /* Setup the command (sans /CS) */ \
   sdram_setupCommand(cmd); \
   /* Now setup the /CS DATA */ \
   writepinPORT(SDRAM_FR__nCS_DQ_PIN, SDRAM_FR__nCS_DQ_PORT, \
                  ( (cmd)&SDRAM_nCS ) ); \
   {};   \
}) 


//Initialize the Free-Runner's SDRAM
// NOTE: THIS DOES NOT initialize the Side-Kick!
// (why not...? Isn't it just a matter of using SDRAM_ALL_DEVICES?)
void sdramFR_init(void);

//Write the address, command, AND bank-values to be stored in the 
//Free-Runner
// UNLIKE readACD(), we MUST supply *all* values
//   (There is no DQM for each of these)
// BUT: Writing NULL to an argument will write 0 in that location
//    EXCEPT dCmd, where loading NULL will store an INHIBIT (with /CS high)
// E.G.
//  uint8_t command = SDRAM_CMD__NOP;
//  sdramFR_writeACD( .. , dBank=NULL, dAddr=NULL, dCmd=&command, 1);
// Or:
//  uint8_t commands[5] = { [0 ... 4] = SDRAM_CMD__NOP };
//  sdramFR_writeACD( .. , dBank=NULL, dAddr=NULL, dCmd=&(commands[0]), 5);
// NOTE:
//  0 = SDRAM_CMD__LOAD_MODE_REGISTER... so no nops/inhibits that way :/
//BIG_NOTE: a/o v17-2ish...
// NEGATIVE counts result in treating pointers (dBank, dAddr, dCmd) as a
// pointer to a single variable, rather than to an array (with abs(count)
// elements). So, the values at dBank, dAddr, and dCmd will be repeated
// abs(count) times. 
void sdramFR_writeACD(uint8_t bank, uint16_t row, uint16_t column,
          uint8_t *dBank, uint16_t *dAddr, uint8_t *dCmd, int16_t count);


//Read back the address, command, and/or bank-values stored in the
//Free-Runner
// Can also read-back to arrays (make use of that burst!)
// dBank, dAddr, and dCmd can be NULL
// E.G.
//  uint8_t readBank;
//  sdramFR_readACD(... , dBank=&readBank, ... , 1)
// Or:
//  uint8_t readBanks[5];
//  sdramFR_readACD(... , dBank=&(readBanks[0]), ... , 5)
// dBank, dAddr, and/or dCmd can be NULL
//  Then read-back data will just be ignored
// count can be 1 to write to pointers to variables
// but when count > 1, pointers must be to the first element of an array
// large enough to handle count elements
void sdramFR_readACD(uint8_t bank, uint16_t row, uint16_t column,
          uint8_t *dBank, uint16_t *dAddr, uint8_t *dCmd, uint16_t count);


void sdramFR_startFreeRunning(void);

void sdramFR_stopFreeRunning(void);



//Turn the address/command I/Os into inputs (for read-back, etc.)
// (This, of course, is only necessary for addr/cmd pins which are directly
//  wired to DQ's... thus, here, instead of sdram_general)
void sdramFR_AddrCmdIO_AsInputs(void);

//Similarly for outputs...
// (sdram_general's init() takes care of this, in general, but free-runners
// need to swap 'em from time to time.)
void sdramFR_AddrCmdIO_AsOutputs(void);

extern int8_t colShift;


#endif //__SDRAM_FREE_RUNNER_H__
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
 * /home/meh/_avrProjects/sdramThingZero/21-gittifying/sdram_freeRunner.h
 *
 *    (Wow, that's a lot longer than I'd hoped).
 *
 *    Enjoy!
 *    --Esot.Eric (aka EricWazhung)
 */
