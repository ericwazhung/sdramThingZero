/* mehPL:
 *    This is Open Source-ish, but NOT GPL. I call it mehPL.
 *    I'm not too fond of long licenses at the top of the file.
 *    Please see the bottom.
 *    Enjoy!
 */


#warning "TODO: TCNTER_STUFF_IN_MAIN should be more like 'PUAT_UPDATE_DOESNT_TCNTER_UPDATE'"
//and therefore is nothing more than a speed/size optimization and
//unnecessary
#warning "TODO: BIT_TCNT could be replaced with PUAT_BAUD"
//except, sometimes it's handy to have direct-controll....


//polled_uat 0.90
//
// NOTE: BIT_TCNT <= 255 (not sure how it works near 255...)
//
// Stolen from polled_uar 0.10-5:
//   Screw the USI interface, let's bit-bang it dangit!
//   Benefits include: Should be usable on multiple pins (eventually)
//   Doesn't use interrupts (no lag-times, etc.)
//   Needn't be half-duplex, in fact could be quadraplex+ (intended)
// TODO:
//   Use structs to define "devices"? Similar to xytracker?
//   Use overall-distance rather than inter-bit distances for timing
//    (fractional bit-distances aren't feasible without something like xyt)
//    XYT only makes sense if the error is large...
//     e.g. if TCNT0 increments at 1MHz and the BAUD is 19200
//          that's 52.08 TCNTs per bit
//          the error is *tiny* (using 52)
//     but, if TCNT0 incremented slower... it could be handy
//          however, using xyt for fast TCNTs would a bit of a slow-down
//          (as it would have to do multiple increments on each update
//           to keep up with TCNT, likely)
//     (More notes on this in .c)

//0.90 is an intermediate-step toward reallyCommon3.mk and heartbeat 2.05's
//     structure... (which is not yet fully-defined)
//     Now using TCNTER instead of DMS (see tiny861 test)
//     Also adding name-related TODOs, above...
//     This a/o oneAxisHolder32
//0.77-1 issues with static inline appeared in 0.71 but never tested with
//       audioThing
//0.77 a/o audioThing62:
//     Attempting to set-up 12MHz F_CPU, but BIT_TCNT needs to be
//     fractional. Instead, using CLKDIV8, but now polled_uat complains
//     BIT_TCNT is too large.
//     ADDING: an option to override that error
//0.76 Experimenting with VIM's retab
//     I use 3 spaces for a tab
//     From: http://vim.wikia.com/wiki/Converting_tabs_to_spaces
//       :set expandtab // each 'tab' press will insert spaces 
//                      // (CTRL-V<Tab> inserts a real one)
//       :set tabstop=3 // has been for years, tabs wil be 3 spaces
//       :set shiftwidth=3 // auto-indentation
//       :retab         // replaces existing tab characters with spaces
//      (:set softtabstop //allegedly causes <Tab> and <Backspace> to
//                        //insert and delete tabstop spaces
//                        //I've never had any luck with this, and the
//                        //manual makes it sound like it's sorta mutually-
//                        //exclusive, as I recall)
//     THEN: using find ./ -name \*.[ch] -exec retab.sh {} \;
//0.75-7 There have been many backed-up but not-noted (here) changes
//       since 0.75-6 (backup files are not numbered to match the -7 here)
//       MAINLY:
//          revisited TESTS: testDesktop, testMega328p, and
//          testTxRxMega328p are now up-to-date.
//       (This a/o audioThing50 and Debian reinstall)
//
//0.75-6 'testTiny84' now functional
//       TODO: These tests need to be recommented!
//             testTiny84/PWM161 ONLY OUTPUT
//0.75-5 'test' is now functional; needed to use stdin_nonBlock
//0.75-3 This is in a questionable-state a/o Debian
//       'testLikePWM161' is uber-questionable
//       (What is it?)
//       Looking into whether 'test' still works
//       This a/o planning to set up puat on Tiny84 
//         (for resolutionDetector)
//       No notes re: whether PWM161/16-bit-timer has been fixed...?
//         (Must've, since PWM161 is working with other things, no?)
//       TODO: 'test' (and 'testPCLikePWM161') still use reallyCommon.mk
//0.75-2 can't use the delay in puat_init() when the tcnter's source
//       variable is updated in the main-loop (e.g. for the PC-based test
//       code... apparently it only worked, previously, because BIT_TCNT*20
//       resulted in a negative value, so it didn't even enter the loop)
//0.75-1 still trying to get the PWM161 going...
//0.75 Using the tcnter with a 16bit timer has not yet been implemented
//     PWM161 sorta requires it...
//0.71 use static inline, rather than extern inline
//0.70-3 puat_sendByteBlocking()
//0.70-2 If this is the only thing using stdio, try to replace it...
//       (sendStringBlocking_P uses sprintf to copy to the string buffer...
//        why not strcpy? dunno. But I'm pretty sure string.h also includes
//        stdio...).
//       ACTUALLY: strcpy_P is in <avr/pgmspace.h> which is included
//       everywhere anyhow... no more stdio here!
//       Also sendStringBlocking_P has been disabled for non-avr targets
//       (maybe not ideal for certain testing cases...)
//0.70-1 Sometimes prints garbage on boot, looking into adding a multi-byte
//       delay to puat_init so the PC will recognize idle after the pin is
//       pulled-up
//       TCNTER MUST BE INITIALIZED BEFORE puat_init()
//       (if TCNTER_IN_MAIN)
//0.70   wtf, sendStringBlocking() and sendStringBlocking_P() use weird
//       arguments... don't contain puatNum!
//       also, I think 0.60-2 didn't result in anything (no notes, even)
//       because the versions were the same.
//       Updating avrTest accordingly,
//       Changed boot-message, adding '.' printout every second.
//0.60-2 notes re: puat_sendStringBlocking() found in
//       sdramThing3.0/sendString.c
//0.60-1 avrTest now uses puat_sendStringBlocking()
//0.60   more toward avrTest... (now complete)
//       adding puat_initOutput() for external usage...
//       updating test and testTxRx
//0.51-1 attempting to create an avrTest, not yet working...
//       this a/o sdramThing3.0-0.9
//0.51   puat_sendStringBlocking has been used numerous times in various
//       projects, it's time to move it here...
//0.50-1 _BITHANDLING_HEADER_ instead...
//        How is it that this hasn't appeared (or at least caught my
//         attention) until several versions after switching over to
//         the new bithandling (a/o sdramThing2.0-6)
//0.50 First-go at using hfm for fractional BIT_TCNTs
//        xyT might make more sense logically, but hfm is 8bit...
//0.40-13 Adding multiple stop-bits (especially for sdramThing at 10MHz)
//        Most data seems OK, but some bytes are mangled
//        Adding stop-bits might allow for resyncing...
//        8MHz -> 13.02 TCNTs, but 10MHz is 16.4 or something.
//        Doesn't seem to help much, and in some cases makes it worse 
//        (different stop-bit values)
//0.40-12 trying out tcnterIsItTime...
//        whoa, 8190 -> 8026 bytes! Nice.
//        Also updating this file to indicate the purpose of 0.40r
//       
//0.40r (was 0.40-11) FOR TESTING ONLY
//
//0.40-11 seeing the effect of using "register" on globalized variables
//0.40-10 adding notes re: codesize increase since globalling...
//0.40-9 adding to COM_HEADERS when INLINED
//0.40-8 Changing Globals' names to puat_*
//       See Notes In puar... Nasty.
//0.40-7 Fixing These (per puar)
// polled_uat.c:111: warning: ‘shiftingByte’ is static but declared in 
//                      inline function ‘puat_update’ which is not static
// polled_uat.c:115: warning: ‘nextTcnter’ is static but declared in 
//                      inline function ‘puat_update’ which is not static
//0.40-6 NYI: TODO: using tcnter_isItTime8() if BIT_TCNT is defined...
//0.40-5 If BIT_TCNT is defined, use it instead of bitTcnt
//       Should optimize a bit... (see similar notes in puar_0.40-4
//       TODO: bitTcnt could be smaller than myTcnter_t (uint32_t)???
//0.40-4 inlining again...
//0.40-3 numbering's off... test apps fixed, puartStuffs fixed
//0.40-2 moved puatStuff.c -> puatStuff.h
//       todo: revisit test application
//             See 0.40b for inlining stuff...
//0.40-1 puat_writeOutput inlining?
//0.40   changing BIT_TCNT -> bitTcnt[puatNum]
//0.35-2 modified note about init in test code
//0.35-1 testTxRx on two puar's
//0.35 add puat_writeOutput() to main... instead of makefile CFLAG
//0.30 Don't like these function-names... using "puat_" for now on.
//0.20-2(?) revised testuar as well
//0.20-1 revising test for 2 transmitters
//0.20 Adding multiple transmitter ability
//0.10-2 removing manual tcnt stuff from here, and using tcnter instead
//       thus it won't run redundantly when running both uar and uat
//0.10-1 adding uat->uar test
//0.10 First version, stolen and modified from polled_uar 0.10-5
//    TODO: MOST notes here are old, from uar
//-----------
//polled_uar:
//0.10-5 addressing 0.10-4's todone
//0.10-4 Buncha Notes regarding xyt at bottom of .c file
//       Looking into running-tcnt
//          myTcnter and nextTcnter now implemented
//          Fixes potential issues with multi-TCNTs per update
//            Aiding cumulative-error fixing
//            (Next time was dependent on the time of the current update)
//       See todo: in .c
//0.10-3 test app using makefile...
//       cleanup
//0.10-2 Notes re: inter-bit distances...
//0.10-1 More mods, test app
//0.10 First Version stolen and modified heavily from usi_uart 0.22-3
//----------
//usi_uart 0.22-3:
//   Using the TinyAVRs' Universal Serial Interface as a UART
//   It's hokey, and seems way more complex than bit-banging
//   using lots of interrupts and various other things
//   CRAZY, but it works. Originally from Atmel... see 0.10's notes.
//0.22-3 TIMER_COMPARE can be overridden by USI_TIMER_COMPARE in makefile
//0.22-2 inlining everywhere!
//0.22-1 (22 not backed up)
//       Looking into speedup with USI_UART_Data_In_Receive_Buffer()
//       as a macro...
//0.22 Looking into using uart_in(1.10) for its indicator/buffer...
//     (Apparently no changes were necessary... forgot to update makefile
//      to use 0.22 anyhow)
//0.20-1 Looking into reception errors... seems OK-ish now.
//       (a/o threePinIDer9i3)
//0.20 Looking into Compare-Match (as opposed to Overflow) on Timer0
//     as the baud-rate clock source...
//     (odd that it worked as-is... maybe I don't understand it yet)
//     or it was that OCR0A was 0x00, so compare-match occurred immediately
//     after an overflow.
//0.10-1 
//0.10 - First common-filed version... 
//       from USI_UART_Proj (my converting the original Atmel AppNote AVR307
//         code to avr-gcc on the ATTiny861)
// AppNote       : AVR307 - Half duplex UART using the USI Interface
// Description   : Header file for USI_UART driver

#ifndef __POLLED_UAT_H__
#define __POLLED_UAT_H__

//This shouldn't be necessary... (or its necessity removed soon)
//#include <avr/io.h>
#include <stdint.h>
#include _TCNTER_HEADER_

// This'd be nice to change...
#if (!defined(PU_PC_DEBUG) || !PU_PC_DEBUG)
 #ifndef _BITHANDLING_HEADER_
  #error "Sorry, Not gonna letcha get away with this... _BITHANDLING_HEADER_ not defined"
  #include "../../bithandling/0.94/bithandling.h"
 #else
  #include _BITHANDLING_HEADER_
 #endif
#else
 #include <stdio.h>
#endif

#ifdef _PUAT_HFM_PERCENT_
 #include _HFMODULATION_HEADER_
#endif

#if (!defined(NUMPUATS))
   #define NUMPUATS  1
#endif



#if (defined(_PUAT_INLINE_) && _PUAT_INLINE_)
// #define PUAT_INLINEABLE extern __inline__
   #define PUAT_INLINEABLE static __inline__
#else
   #define PUAT_INLINEABLE //Nothing Here
#endif

//********** POLLED_UAT Prototypes **********//

#ifndef BIT_TCNT
PUAT_INLINEABLE
void puat_setBitTcnt(uint8_t puatNum, myTcnter_t bitTcnt);
#endif

//These are the functions that are used in main code...
// General initialization...
// This doesn't really do anything
// if update() is called, it will be updated, regardless of init...
PUAT_INLINEABLE
void puat_init(uint8_t puatNum);

// To be called in the main loop...
// (Handles writing the pins and shifting bits, etc)
PUAT_INLINEABLE
void puat_update(uint8_t puatNum);

// Check if there's data waiting
PUAT_INLINEABLE
uint8_t puat_dataWaiting(uint8_t puatNum);

// Load a byte to be sent
//  if data's waiting, this will overwrite it!
//  so best to test dataWaiting first...
PUAT_INLINEABLE
void puat_sendByte(uint8_t puatNum, uint8_t byte);

#if (defined(PUAT_SENDSTRINGBLOCKING_ENABLED) \
     && PUAT_SENDSTRINGBLOCKING_ENABLED)
// TO USE:
// Must CFLAGS+=-D'PUAT_SENDSTRINGBLOCKING_ENABLED=TRUE'
// 
#include <avr/pgmspace.h>  //for PGM_P

void puat_sendByteBlocking(uint8_t puatNum, uint8_t byte);

void puat_sendStringBlocking(uint8_t puatNum, char string[]);
//stringBuffer is where the string will be stored while sending
// ideally stringBuffer would be global/static, and therefore included in
// the build memory usage summary
// so we don't use huge chunks of memory without knowing it... and possibly
// overflow the stack...
void puat_sendStringBlocking_P(uint8_t, char stringBuffer[], PGM_P P_string);
#endif

#if (defined(_PUAT_INLINE_) && _PUAT_INLINE_)
   #include "polled_uat.c"
#endif
// This must be defined in puatStuff.h somewhere in the include paths...
// See example below.
//extern __inline__ void puat_writeOutput(uint8_t puatNum, uint8_t value)
// __attribute__((__always_inline__));

//Basic functionality (Basic Usage):
// Not particularly up-to-date...
//
// NOTE: If tcnter is in main, it must be initialized *before* puat_init.
// int main(void)
// {
//    init();
//    while(1)
//    {
//       update();
//       if(!dataWaiting())
//          sendByte(data);
//    }
// }
//
// //MUST BE DEFINED SOMEWHERE (e.g. main.c):
// void puat_writeOutput(uint8_t puatNum, uint8_t value)
// {
//    if(puatNum==0)
//       writepinPORT(Tx0Pin, PORTB, value)
//    else
//       writepinPORT(Tx1Pin, PORTA, value)
// }
// 
// void puat_initOutput(uint8_t puatNum)
// {
//     setoutPORT(Tx0Pin, Tx0PORT);
// }
//
//  This should probably be changed to be #defined application-specifically
//  Peripherals Used:
//     Timer0
//        --Required for timing of the serial bit-clock

#endif

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
 * /home/meh/_avrProjects/sdramThingZero/21-gittifying/_commonCode_localized/polled_uat/0.90/polled_uat.h
 *
 *    (Wow, that's a lot longer than I'd hoped).
 *
 *    Enjoy!
 *    --Esot.Eric (aka EricWazhung)
 */
