/* mehPL:
 *    This is Open Source-ish, but NOT GPL. I call it mehPL.
 *    I'm not too fond of long licenses at the top of the file.
 *    Please see the bottom.
 *    Enjoy!
 */


//polled_uar 0.40-19
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





//0.40-24 adding support for DEFAULT pins, now available from atmega324p.mk
//        and likely, later, to be supported by all devices' .mk files
//        TODO: This is currently implemented *in the test*'s pinout.h
//              Consider making this a polled_uar.h/c thing, instead
//        LIKEWISE: Is there really any reason for puarStuff.h? Why not
//                  include that in polled_uar.c when compiling for an AVR?
//                  TODO: Consider *number* of puars...
//0.40-[19-23] testDesktop has been dramatically improved
//             Various backups made during repair
//             the BIT_TCNT=OVERFLOW_VAL bug in -18 was due to 
//             reallyCommon2.mk which didn't remove polled_uar.o during 
//             'make clean'
//0.40-18 setting up test for ATmega328p (a/o audioThing50)
//        RUNNING testDesktop:
//           Mid-Bit delay was not functioning, 6/8=0
//           ALSO: Oddly was running at BIT_TCNT=OVERFLOW_VAL
//           DESPITE: having changed it in makefile
//           It fixed itself *after* changing BOTH BIT_TCNT AND /8->/2
//           
//0.40-17 for Debian (toward Tiny84 resolutionDetector)
//        'test' needed to be revised to use stdin_nonBlock
//        TODO: testTiny84 doesn't work at 19200, which is default for
//        polled_uat's Tiny84 test, and serin/out_nonBlock... Using 9600
//0.40-16 a/o sdramThing3.0-0.9, delayCyc0.10ncf, heartbeat 1.30-somn...
//        started making 8515 test programs
//        So, adding one here as well...
//0.40-15 
//        a/o sdramThing3.0-1, trying to figure out this big WTF 
//        (as in, I don't remember what it was)
//        See notes made in polled_uar.c
//        Yeah, there's a bug, but it wouldn't compile with those options,
//        so I'd have my chance to fix it then... TODO
//        So... weird.
//        The apparent bug is *not* fixed, and should fail on compilation
//        with those options (BIT_TCNT not defined)
//        Disabling the error.
// WTF::::
//0.40-14  Override for error re: tcnterIsItTime8()
//				Discovered "HOLY SHIT WTF"
//          Disabling compilation until I have a chance to figure it out...
//         See notes added to 0.40-2_pre...
//-------------------------------
//0.40-2_preQuarterStartBit (Revisited a/o 0.40-14... this is a new branch)
//     Discovered what looks to be a pretty big bug in the latest version
//     (0.40-14)
//     a/o tabletBluetoother-8, which has been revisited *long* after last
//     used. This version, 0.40-2, appears to have been developed alongside
//     tabletBluetoother-7 (judging by the date/time)
//     The latest version has been disabled until I have a chance to figure
//     out what's wrong (and right).
//-------------------------------
//0.40-14: see above... "WTF"
//0.40-13: _BITHANDLING_HEADER_
//0.40-12: WHOOPS, yet another bug like in 0.40-7
//0.40-11: adding to COM_HEADERS when INLINED
//0.40-10: switching test over to reallyCommon2.mk
//         a/o avr/audioThing18 for removing of test/polled_uar.d/o
//           in "make clean"
//         ran into some other issues... apparently I'd been referencing
//         tcnter0.20 directly in my projects and now this doesn't work
//         with tcnter0.10...
//0.40-9: -8 Not Backed Up
//        REPLACING GLOBALS' NAMES with puar_*
//        (e.g. rxData was used by both puar and ps2_host)
//         And they actually interfered with each other...
//         Really makes you wonder how much this happens without even
//         noticing...
//         SEE NOTES in ps2_touchPad3
//0.40-8: when using inlining...
// polled_uar.c:197: warning: ‘thisByte’ is static but declared in inline 
//                             function ‘puar_update’ which is not static
// polled_uar.c:202: warning: ‘lastTime’ is static but declared in inline 
//                             function ‘puar_update’ which is not static
//0.40-7 -6 introduced a bug, which hasn't shown itself to be a problem
//       YET... but it's a big one...
//       polled_uar.c:363 (and 402): warning: passing argument 1 of 
//          ‘tcnter_isItTime8’ from incompatible pointer type
//0.40-6 looking into smaller variable for myTcnter_t
//       NOTE: This has been a recurring problem, I think it's solvable
//       Places Encountered: dmsTimer, Tcnter, probably others...
//       TODO: If it works here, move it over to them!
//             I think I solved this elsewhere, already
//             But I can't recall where...
//0.40-5 replacing update if-statements with switch... for this purpose:
//       Intentional fall-through after start-detected
//          such that sampling starts immediately (might be useful for
//          high-speed bauds relative to TCNT updates...)
//       CFLAGS += -D'SKIP_WAIT_TO_SAMPLE_START=TRUE'
//   NOTE: Code-Size has Increased (likely due to multiple locations
//       testing and setting nextTcnt).... TODO: This could be a separate
//       thinggy outside the switch statement...
//
//0.40-4 if BIT_TCNT is defined, then don't use bitTcnt AT ALL
//       should optimize a bit... There's a /8 at some point, which *does*
//       result in ror's, but 3 of them...
//       and a few other things... loading a byte to/from SRAM, etc.
//       TODO: isn't there some way to use a smaller variable than
//         myTcnter_t (uint32_t) for bitTcnt?
//0.40-3 using 1/4 bit for start-detection...
//0.40-2 inlining...
//		 bitTcnt -> puar_bitTcnt (so it won't conflict with puat's bitTcnt
//      when both are inlined)
//     forgot to mention in a prior version BIT_TCNT now puar_bitTcnt[]
//0.40-1	puar_readInput() in puarStuff.h
//0.40 
//			TODO: NOGO: Trying to figure out how to auto-calibrate...
//     Ideas auto-calibrate either:
//        OSSCAL
//				Doesn't affect code, BIT_TCNT remains constant 
//					(rather than a variable) so slightly faster updates
//        BIT_TCNT
//				Small BIT_TCNT values could lead to tremendous error...
//          Easier to implement... 
//					send a character (so bit7 = 0, then there's guaranteed
//						0->1 transition between bit7 and stop
//					check TCNT at start edge
//					check TCNT at bit7->stop edge
//             divide by... appropriate time (9 bits)
//              ___     ___ ___ ___ ___ ___ ___ ___ ___ ___
//                 \___/___X___X___X___X___X___X___X___/
//             TODO: Higher precision if this isn't in the main loop...
//               Ideally, use timer with interrupts...
//                  Difficult without knowing exactly what byte is sent
//               SAME PROBLEM with any...
//          BAUD-Rate independent... (auto-baud rather than auto-OSCCAL)
//             U = 0x55 =
//              ___     ___     ___     ___     ___     ___ ___
//                 \___/   \___/   \___/   \___/   \___/
//            Start--^   0   1   2   3   4   5   6   7   ^--Stop
//
//0.35 TONS of changes, catching up with polled_uat:
//				Multiple Receivers
//				renaming POLLED_UAR_ functions to puar_
//       	add puar_readInput() to main instead of makefile CFLAG
//0.10-8 renaming tcnt_ and TCNT_ stuff to tcnter_ and TCNTER_
//0.10-7 removing manual tcnt stuff from here, and using tcnter instead
//       thus it won't run redundantly when running both uar and uat
//0.10-6 slight modification for better debugging
//       polled_uat now has a combined test of uar and uat
//0.10-5 addressing 0.10-4's todone
//        Minor fix in .mk re: UART -> UAR
//0.10-4 Buncha Notes regarding xyt at bottom of .c file
//       Looking into running-tcnt
//				myTcnter and nextTcnter now implemented
//          Fixes potential issues with multi-TCNTs per update
//            Aiding cumulative-error fixing
//            (Next time was dependent on the time of the current update)
//       See todo: in .c
//0.10-3 test app using makefile...
//			cleanup
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

#ifndef __POLLED_UAR_H__
#define __POLLED_UAR_H__

//This shouldn't be necessary... (or its necessity removed soon)
//#include <avr/io.h>
#include <stdint.h>
#include _TCNTER_HEADER_

// This'd be nice to change...
#if (!defined(PU_PC_DEBUG) || !PU_PC_DEBUG)
 #ifndef _BITHANDLING_HEADER_
  #error "WTF"
  #include "../../bithandling/0.94/bithandling.h"
 #else
  #include _BITHANDLING_HEADER_
 #endif
#else
 #include <stdio.h>
#endif


#if(!defined(NUMPUARS))
	#define NUMPUARS	1
#endif

//********** POLLED_UAR Prototypes **********//

//When called, it waits in its own loop to receive 'U' to calibrate
// the BIT_TCNT
#ifndef BIT_TCNT
myTcnter_t puar_calibrateBitTcnt(uint8_t puarNum);

//This only needs to be called when setting a specific bitTcnt
// (e.g. calibrated on puar0, and mimicking the rate on puar1)
void puar_setBitTcnt(uint8_t puarNum, myTcnter_t tcnt);
#endif


//These are the functions that are used in main code...
// General initialization...
void puar_init(uint8_t puarNum);

// To be called in the main loop...
// (Handles reading the pins and shifting bits, etc)
void puar_update(uint8_t puarNum);

// Check if there's data waiting
uint8_t puar_dataWaiting(uint8_t puarNum);

// Read the data that's waiting
//Must be called AFTER testing if data's waiting
// otherwise it will return garbage
uint8_t puar_getByte(uint8_t puarNum);

#if (defined(_PUAR_INLINE_) && _PUAR_INLINE_)
   #define PUAR_INLINEABLE extern __inline__
   #include "polled_uar.c"
#else
   #define PUAR_INLINEABLE //Nothing Here
#endif




// This must be defined in main.c or somewhere...
// See example below.
// how can we inline this? Is it possible?
//extern uint8_t puar_readInput(uint8_t puarNum);

//Basic functionality:
// int main(void)
// {
//		init();
// 	while(1)
// 	{
//			update();
//			if(dataWaiting())
//				data = getByte();
// 	}
// }
//
// This must be defined in puarStuff.h somewhere in the include paths...
// See example below.
//extern __inline__ uint8_t puar_readInput(uint8_t puarNum)
// __attribute__((__always_inline__));
//
// uint8_t puar_readInput(uint8_t puarNum)
//	{
//		if(puarNum==0)
//			return getpinPORT(Rx0Pin, PORTB);
//		else
//			return getpinPORT(Rx1Pin, PORTA);
//	}
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
 * /home/meh/_avrProjects/sdramThingZero/21-gittifying/_commonCode_localized/polled_uar/0.40/polled_uar.h
 *
 *    (Wow, that's a lot longer than I'd hoped).
 *
 *    Enjoy!
 *    --Esot.Eric (aka EricWazhung)
 */
