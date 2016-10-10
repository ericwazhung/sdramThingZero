/* mehPL:
 *    This is Open Source-ish, but NOT GPL. I call it mehPL.
 *    I'm not too fond of long licenses at the top of the file.
 *    Please see the bottom.
 *    Enjoy!
 */


//bithandling 1.99
//
// bithandling is *generally* intended for bit-manipulation MACROS
//  (Thus, none-a-dis will get compiled UNLESS USED)
//
// HOWEVER: Since I use bithandling in danged-near *everything* I code...
//  I also have it set-up to do a lot of common things I do... commonly.
//  E.G. it auto-includes gpio_port
//        (when 'BITHANDLING_INCLUDES_GPIO_PORT = true' in yer makefile)
//         which is basically bithandling for GPIO pins
//       it also is responsible for determining/defining __MCU_ARCH__
//         which is used in many (other) places for selecting
//          architecture-specific code (e.g. gpio_port_avr.h)
//       it also auto-includes 'stringify' to that same end
//         for e.g. CONCAT_HEADER()


#ifndef __BITHANDLING_H__
#define __BITHANDLING_H__

//TODO: MOST (if not all?) of grbl-abstracting-30's port/bit-handling
//       code has been moved here (or to gpio_port, as appropriate)
//      BUT:
//       MOST OF bithandling0.99 has NOT YET BEEN MOVED HERE TODO

//1.99-1 THIS HAS BEEN TESTED COMPATIBLE WITH sTZ-16
//       (Prior to SEI_RESTORE() change, this is build-identical with 0.99)
//1.99 - This is a gigantic step from: 
//        bithandling 0.99-7 combined with 
//        port_handling from grbl-abstacting-30
//
//       This is not complete, thus 1.99, rather than 2.00
//
//       Major Differences from the 0.xx series:
//        NAMES
//         e.g. setpinPORT() -> gpioPORT_setpin()
//        ARGUMENTS
//         e.g. setpinPORT(PIN, PORT) -> gpioPORT_setpin(PORT, PIN)
//         WHEREIN: I attempt to keep the *output* variable in the *first*
//                  argument
//         THUS
//            e.g. writeMasked(val, mask, var) -> writeMasked2(var, ...)
//      CONVERSION:
//       These notes regard how to convert from bithandling0.xx to 1.99
//       (Toward scripting?)
//       SEE ALSO----vvv
#include "bithandling_099compat.h"

//I likes me some [u]int[8/16/32/64]_t's
#include <stdint.h>

//In order to use CONCAT_HEADER() later, for Architecture-specific code...
#include _STRINGIFY_HEADER_

//Determine which architecture we're compiling for...
#ifdef __AVR_ARCH__
 #define __MCU_ARCH__ avr
#elif (defined(__PIC32MX__))
 #define __MCU_ARCH__ pic32
#elif (defined(__PIC32MZ__))
 #error "I dunno whether PIC32MZ is directly compatible with PIC32MX, but you're welcome to try!"
 #define __MCU_ARCH__ pic32
#endif


//Prefix a function-name with its architecture
// e.g. ARCHIFY(function(arg)) -> avr__function(arg)
// This is used, mostly, for e.g. gpio_port.h
#define ARCHIFY(name)   \
   GLUE_THESE_VALUES( \
         GLUE_THESE_VALUES(__MCU_ARCH__,__), \
         name )



#define TRUE   1
#define FALSE  0




//Set a bit in a byte, leaving all other bits unchanged
//Wow... apparently avr-gcc is smart enough to optimize this to a cbi
// if possible!
#define setbit2(variable, bitNum) \
         (variable = ((variable) | (1 << (bitNum))))

//Clear a bit in a byte, leaving all other bits unchanged
//Wow... apparently avr-gcc is smart enough to optimize this to a cbi
// if possible!
#define clrbit2(variable, bitNum) \
         (variable = ((variable) & (~(1 << (bitNum)))))

//Write a single bit 0 or 1
// variable = the variable to be modified
// bitNum = the bit number to modify
// bitVal = the value to set the bit to (0 or 1)
//TODO: There ARE LIKELY faster implementations!
//      (But, if all args are constant, maybe not)
#define writebit2(variable, bitNum, bitVal) \
      ((bitVal) \
       ? setbit((bitNum),(variable)) \
       : clrbit((bitNum),(variable)) )
//(bitwiseByte = ((bitwiseByte) | ((value & 0x01) << (bitNum))))


//??? Would this be more efficient if ((bitwiseByte >> bitNum) & 0x01)?
//#define getbit(bitNum, bitwiseByte) 
//    (((1 << (bitNum)) & (bitwiseByte)) >> (bitNum))
//!!! HOLY FU$! This was 70 bytes on an AVR?! (WRT PE0,PINE...)
//a/o 0.99: I cant remember the details of that test, nor whether this
//code has changed, since...
#define getbit2(variable, bitNum) \
         (((variable) >> (bitNum)) & 0x01)



//This writes a value to a variable,
// but only to those 'mask'-bits which are 1
//This handles writing both 1's and 0's to those masked-bits.
//  (variable & ~mask) clears the bits to be written
//  (value & mask)     assures value doesn't overwrite masked bits
//NOTE: This is SAFE to use with values that have 1's or 0's *outside* the
//      masked-bits. 
//      This introduces some overhead, but allows for e.g.
//       writeMasked2(variable, mask=0x03, value=0xff);
//NOTE2: This is UNSAFE to use with interrupts which might modify variable!
//CONVERSION:
//  writeMasked(value, mask, variable) 
//    -> writeMasked2(variable, mask, value)
#define writeMasked2(variable, mask, value)   \
      (variable) = (((variable) & ~(mask)) | ((value) & (mask)))


//Is the bit high? 
// Returns: 0        = bit is low
//          NON-ZERO = bit is high
// (generally: returns the binary value of the bit... e.g. bit6 would
//  return 0 if the bit is low and 0100 0000 = 0x40 if the bit is high
//  This is much more efficient than returning 0 or 1, as that would
//  require bit-shifts... but if you need that, see getbitVal() )
#define isBitHigh_ZNZ(variable, bitNum) \
      (((variable) & (1<<(bitNum))))


// Some of this code is MCU-architecture-specific, so include the
// associated architecture-specific file.
//  Essentially: #include "bithandling_avr.h" or "bithandling_pic32.h", etc
//  as appropriate
#include CONCAT_HEADER(bithandling_,__MCU_ARCH__)


//When the makefile says: 'BITHANDLING_INCLUDES_GPIO_PORT = true'
// THEN bithandling1.99 kinda acts like bithandling 0.xx
//  which had the gpio-functionality intermixed
// (So, then, no need to #include _GPIO_PORT_HEADER_ everywhere...
//  #include _BITHANDLING_HEADER_ will take care of adding it.)
#ifdef _GPIO_PORT_HEADER_
 #include _GPIO_PORT_HEADER_
#endif



//DISABLE AND REENABLE INTERRUPTS *IF* They were already enabled.
//
//This isn't really *bit-handling* per-se... but is commonly-used enough,
//by me, that I want it easily-accessed
//This is Architecture-Specific, so see the definition in the appropriate
//file.
//TODO: Make note regarding architecture-specific files, and
//       macro-definitions ala gpio_port.h
//
//GENERAL USAGE:
//  uint8_t CLI_SAFE(uint8_name);
//   ...do something that needs to be handled without interrupt...
//  SEI_RESTORE(uint8_name);
//
// Clear/Disable interrupts if enabled (and save their previous state)
#define CLI_SAFE(uint8_name) \
         ARCHIFY( \
          CLI_SAFE(uint8_name) \
         )
// Reenable interrupts (if they were previously enabled)
#define SEI_RESTORE(uint8_name) \
         ARCHIFY( \
          SEI_RESTORE(uint8_name) \
         )




#endif //__BITHANDLING_H__

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
 * /home/meh/_avrProjects/sdramThingZero/21-gittifying/_commonCode_localized/bithandling/1.99/bithandling.h
 *
 *    (Wow, that's a lot longer than I'd hoped).
 *
 *    Enjoy!
 *    --Esot.Eric (aka EricWazhung)
 */
