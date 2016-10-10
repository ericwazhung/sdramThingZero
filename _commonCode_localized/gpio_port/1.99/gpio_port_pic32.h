/* mehPL:
 *    This is Open Source-ish, but NOT GPL. I call it mehPL.
 *    I'm not too fond of long licenses at the top of the file.
 *    Please see the bottom.
 *    Enjoy!
 */


#error "This needs to be modified similarly to gpio_port_avr.h"
// meh: TODO: This needs to be updated with license-info
//
// This file handles port-stuff, such as reading/writing bits to port-pins
//   or setting such pins as inputs/outputs...
// One of its key features is the use of ONLY the PORTx register-name
//   such that, e.g. setting the direction-register (DDRx, on an AVR)
//   can be accomplished by calling one of these macros with the PORTx
//   register-name.
//   With AVR's optimization-scheme, this generally results in *zero* extra
//   instructions, the math/remapping is handled during compilation-time.
// This file consists of code from esot.eric's "bithandling" which is part
// of his "_commonCode." which generally falls under the "mehPL" but is 
// presented here, by him, under the overall grbl licensing-scheme with 
// the exception that this message must be included in these files and/or 
// their derivatives.

#ifndef __PORTHANDLING_PIC32_H__
#define __PORTHANDLING_PIC32_H__

//This assumes you're working with Microchip's xc32-gcc compiler
//and have its supplied headers...
#include <xc.h>

//WARNING!!!!
// gcc version 4.8.3 MPLAB XC32 Compiler v1.40 (Microchip Technology)
//  running on Linux
// HAS A KNOWN BUG wherein math with int8_t/uint8_t may not be handled properly
// The workaround, as it stands, is to disable all optimization.
//  (This means, even though quite a few of these macros could reduce to 
//   little more than a single register-write, they may instead perform the
//   address-calculations regarding which registers to write in realtime,
//   rather than compile-time. Weeeeee!) 
#ifndef __IGNORE_MEH_ERRORS__
 #error "Don't forget optimization-level and int8_t..."
#endif
//BEWARE when implementing for your own architecture!!!!
//  Many of these macros require read-modify-write operation
//  If they're called when interrupts are running, they may result in
//  damaged-data!
//  Note that this message is the result of seeing *numerous* cases where
//  R-M-W operation *could* be problematic in the original code, EXCEPT
//  that the original code was implemented on an AVR. avr-gcc's optimizer
//  is smart-enough to recognize e.g. 'PORTx |= (1<<BITNUM);' as an 'sbi'
//  instruction, which is inherently atomic. This is the case when using
//  -Os, but when using lower or no optimization it may not be the case!
//  ALSO, if this same operation was written and compiled for another 
//  architecture, it would most-likely NOT be atomic.
//  If this R-M-W operation was interrupted, and PORTx was modified during
//  that interrupt, then when the interrupt completed, PORTx's other bits
//  would be rewritten to their original values, negating the effect of the
//  interrupt.
//  This effect/bug, most-likely, could go unnoticed a million times before
//  randomly showing its effects, and then could go unrepeated a million
//  more times... Debugging these things is difficult without knowing the
//  code inside-and-out. 
//  DO NOT CODE PACEMAKERS nor auto-driving cars LIKE THIS!
//  (even if you *know* the optimizer in version x of compiler y will
//   optimize it correctly!)
//
//  I have done my best to verify that wherever such operations occur,
//  they're *inside* an interrupt, (where other interrupts are disabled),
//  or otherwise will not cause trouble. 
//  BUT I have only done *my* best, not knowing the grbl code 
//   inside-and-out.


//GPIO ports on PIC32's appear to be 16-bits wide
// (all datasheets I've browsed seem to suggest, but not expressly-state,
//  a limit at 16bits
//  e.g. "ANSELx defaults to a value of 0xffff")
// in some cases it's handy to e.g. read an entire port to a variable
// then work on that variable... other architectures may have different
// port-widths (e.g. AVR has 8-bit ports)
// There's probably nothing wrong with changing this to uint32_t... if all
// the code's implemented right :)
#define gpioPortWidth_t    uint16_t


//AVR uses "sei()" to enable interrupts and "cli()" to disable them,
//globally...
//I happen to like this naming-scheme due to having started with AVRs
#define sei()  __builtin_enable_interrupts()
#define cli()  __builtin_disable_interrupts()



//Generally:
// uint8_t oldState;
// CLI_SAFE(oldState);
//  ...
// SEI_RESTORE(oldState);
//
//Can also call this as:
// uint8_t CLI_SAFE(uint8_name);
// ...
// SEI_RESTORE(uint8_name);
// ...
// //And can also be reused (but not inside!):
// CLI_SAFE(uint8_name);
// ...
// SEI_RESTORE(uint8_name);
//
//Newly-discovered:
//NOTE: This is similar in functionality to <util/atomic.h> which has a
//much better description of the potential necessity for this sorta thing
//It also appears to be more fully-featured, but also seems a bit more
//complicated to use... Also, it require C99, which, I suppose, these days
//is kinda stupid to try to avoid...
//
//TODO: was planning to have something like:
//      GET_ATOMIC(variable) (SET_ATOMIC?)
//      or maybe
//      DO_ATOMIC(...)?
//      
//      then could do, e.g. while(GET_ATOMIC(variable) != 0) {...}
//
//      This could probably be implemented with <util/atomic.h> if I
//      thought about it.

//PIC32: the IE bit is bit 0...
#define CLI_SAFE(uint8_name) (uint8_name) = \
                   (__builtin_disable_interrupts() & 0x01)

#define SEI_RESTORE(uint8_name) if(uint8_name) sei()



//PIC32: Peripheral-Pin-Selection is only doable via unlock-process...
// You must explicitly state the following somewhere:
//   #pragma config IOL1WAY = OFF // Allow multiple reconfigurations
// Without it, the ppsUNLOCK/LOCK procedure can only be performed *once*
// Which would mean that ALL your peripherals must be initialized in the
// same function!
//Example:
// ppsUNLOCK();
//  RPB15R = 0x01; //RPB15R<3:0> = 0001 = U1TX on RB15
// ppsLOCK();

#define ppsUNLOCK() \
({ \
   SYSKEY = 0x0; \
   SYSKEY = 0xAA996655; \
   SYSKEY = 0x556699AA; \
   CFGCON = CFGCON & ~(1<<13); \
   {}; \
 })

#define ppsLOCK() \
({ \
   SYSKEY = 0x0; \
   {}; \
 })





//By accessing, essentially, PORT+<offset> we can refer to ONLY the PORTx
//definition everywhere in our code, regardless of whether we're trying to
//write a pin-value or set a pull-up resistor...
//With -Os, this code will generally reduce to a single register-write 
// instruction;
//  The math of calculating the offsets, etc. will be handled during
//  compile-time.
#define LAT_PORT_OFFSET    (0x10)
#define CNPU_PORT_OFFSET   (0x30)
#define ANSEL_PORT_OFFSET  (-0x20)
#define TRIS_PORT_OFFSET   (-0x10)


//PIC32: "All registers in this table have corresponding CLR, SET and INV
//registers at their virtual addresses, plus offsets of 0x4, 0x8 and 0xC,
//respectively."
#define CLR_PORT_OFFSET    (0x4)
#define SET_PORT_OFFSET    (0x8)


//Read the bits at the port's inputs (and/or outputs, as-configured)
//Note this could seem a bit confusing when considering different
//Microcontroller architectures and their differing I/O registers
// (Especially: AVR, where reading the register called "PORT" reads the
// values written by the AVR to that register, and NOT the values on the
// pins. Whereas, e.g. reading a PIC32's "PORT" register returns the values
// on the pins.)
//THIS MACRO DOES NOT READ THE "PORT REGISTER"
//This macro reads the values at the pins connected to PORTx
// (Regardless of whether those pins are input or output).
// For AVR: This is essentially "uint8_t  readioPORT(...){ return PINx; }"
// For PIC32:                   "uint32_t readioPORT(...){ return PORTx; }"
//THE ARGUMENT, however, is PORTx, regardless of the architecture.
// (Doing-so allows for fewer #defines...
//  e.g. #define inputPinBit  3
//       #define inputPinPORT PORTA
//     //#define inputPinPIN  PINA  //This is unnecessary
//  Again, Doing-so also allows for more hardware-abstraction...
//   as most devices will have "Ports" with "pins" attached to its bits
//   but their register-definitions will vary.
//  NOTE: That because these are all constants, most optimizers will reduce
//  this entire operation to a single instruction
//  e.g. "move to a general-purpose register the value at PINx"
#define readioPORT(PORTx)  (PORTx)

//A rare occurance, but say one wants to read the port-outputs into a
//variable... do something with that variable... then rewrite the port
//outputs. But can't do-so in a single write-masked operation...
//(Frankly, this seems like bad-practice, to me... what if those
//port-outputs are changed in an interrupt during the math...?)
//Anyways, one might wish to *read* the values *written* to a
//port-output-register.
//This will give *all* the bits on that register, regardless of whether
//those bits are outputs or inputs or anything else.
//(E.G. on an AVR, this will read-back the latched output-bits AND
//read-back whether the pull-up resistors are active on the input-bits!)
//So, only work with the bits you intend to!
// This is *ONLY* intended to enable you to work with the OUTPUT bits
// Any other use would be HIGHLY architecture-specific.
#define readoutputsPORT(PORTx)   \
   (*(volatile int *) \
      ((int)(&(PORTx)) + LAT_PORT_OFFSET)


//Likewise, this is *equally* if not MORE risky...
// This will write *all* bits in the output-register
// BUT, again, the output-register is usually shared with other purposes
// when the associated bits are *inputs*
// E.G. again, on AVR, writing 1 to *input* bits, via this macro will turn
// on the pull-up resistors on those bits. Whereas, on PIC32, writing 1 to
// input-bits via this macro will have no effect.
// THUS: This macro is *highly* architecture-specific unless used *only* to
// manipulate output-bits. In other words. Unless you know *exactly* what
// you are doing, and all the possible consequences of your doing-so, that
// you might not foresee (e.g. what happens if those input-bits were
// modified via an interrupt before you rewrite their old values via
// this?)
// (e.g.2. If you have inputs on the same port, are you using this in an
// architecture-specific way?)
// Basically, this macro only exists for the purpose of
// backwards-compatibility-testing of the original grbl code.
//
// THIS MACRO SHOULD NOT BE USED except, maybe, if the *entire* port is
// being used for a single-purpose (e.g. an 8bit data-bus)
// Use gpio_writeOutputPins_masked() instead!
#define writeAllPORToutputs_unsafe(PORTx, value)    \
   ( (*(volatile int *) \
        ((int)(&(PORTx)) + LAT_PORT_OFFSET)) = (value) )




//Set a single output-pin to 1
#define setpinPORT(Pxn, PORTx)   \
   (*(volatile int *) \
       ((int)(&(PORTx)) + LAT_PORT_OFFSET + SET_PORT_OFFSET) \
       = (1<<(Pxn)))

//Clear a single output-pin to 0
#define clrpinPORT(Pxn, PORTx)   \
   (*(volatile int *) \
       ((int)(&(PORTx)) + LAT_PORT_OFFSET + CLR_PORT_OFFSET) \
       = (1<<(Pxn)))





//PIC32 lends itself well to using xxxMasked() first-and-foremost
// or at least calls to them from xxx()...

//HEH! For PIC32, would make sense to just set these once, and use
//...Masked() macros for all others....
// This is *not* the case for AVR...

//This sets several bits/pins on a port as outputs
//PIC32: Pull-ups (and downs) should be disabled when an output...
// Doing-so HERE assures that will always be the case, but also means more
// instructions (and redundant) whenever called
#define setPORToutMasked(PORTx, Mask)  \
 ({ \
   (*(volatile int *) \
       ((int)(&(PORTx)) + CNPU_PORT_OFFSET + CLR_PORT_OFFSET) \
       = (Mask)); \
   (*(volatile int *) \
       ((int)(&(PORTx)) + TRIS_PORT_OFFSET + CLR_PORT_OFFSET) \
       = (Mask)); \
  {}; \
 })


//Again, for PIC32 (as opposed to, say, AVR) using the Masked macros is
//just as efficient (?) as writing indivudual bit-handling macros...
//So, here, I'll just call them as appropriate.
// (In, say, an AVR, it makes more sense to have separate macros for
//  masked-writes vs. individual bit-writes, as it has SBI and CBI)

//This sets a single bit/pin on a port as an output...
#define setoutPORT(Pxn, PORTx) \
   setPORToutMasked((PORTx), (1<<(Pxn)))


//This writes a value to a port, but only to those bits that are Masked (1)
// This handles writing both 1's and 0's to those masked-bits.
//THIS IS ONLY ON PIC32 SO FAR!
// Alternatively: Separate SET and CLRs could be used...
#ifndef __IGNORE_MEH_ERRORS__
#error "This is a long operation requiring read-modify-write! It should probably be SEI/CLI'd unless in an interrupt"
#endif

#define writePORToutputsMasked(PORTx, Mask, Value) \
({ \
   volatile int *theLAT = \
               (volatile int *) \
                  ((int)(&(PORTx)) + LAT_PORT_OFFSET); \
   writeMasked(Value, Mask, *theLAT); \
   {}; \
})


//Configure PORTx's pins as inputs, per the Mask
// (Mask Bits: 1 = input, 0 = no change)
//PIC32: NOTE: ANSEL needs to be set appropriately!!!
// Doing-so HERE assures it will always be the case, but also means more
// instructions (and redundant) whenever called
#define setPORTinMasked(PORTx, Mask) \
 ({ \
   (*(volatile int *) \
       ((int)(&(PORTx)) + ANSEL_PORT_OFFSET + CLR_PORT_OFFSET) \
       = (Mask)); \
   (*(volatile int *) \
       ((int)(&(PORTx)) + TRIS_PORT_OFFSET + SET_PORT_OFFSET) \
       = (Mask)); \
  {}; \
 })


//Enable the pull-ups on the masked bits
//TODO: PIC32: CNPU has some weird requirements I can't recall...?
//      "should be disabled when the port is a digital output"?
//      Haven't *noticed* any trouble with this, yet...
#define setPORTpuMasked(PORTx, Mask) \
   (*(volatile int *) \
       ((int)(&(PORTx)) + CNPU_PORT_OFFSET + SET_PORT_OFFSET) \
       = (Mask))

//Disable the pull-ups on the masked bits
#define clrPORTpuMasked(PORTx, Mask) \
   (*(volatile int *) \
       ((int)(&(PORTx)) + CNPU_PORT_OFFSET + CLR_PORT_OFFSET) \
       = (Mask))

//Is the pin on the port high...? 0 = no, non-zero = yes
//PIC32: NOTE that reading the PORTx register on a PIC32 reads the value on
//the pin... Whereas reading the PORTx register on an AVR reads the value
//*written to* that pin (by internal registers), which, if an input, would
//therefore read-back not the value on the pin, but the value of whether
//the internal pull-up resistor is enabled.
// This macro reads the value on the pin (especially if an input)
// It will also read the value written to an output, but should NOT be used
// that way, as its results may be architecture-dependent.
#define isPinPORT(Pxn, PORTx) \
   isBitHigh_ZNZ((Pxn), (PORTx))



#endif //__PORTHANDLING_PIC32_H__

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
 * /home/meh/_avrProjects/sdramThingZero/21-gittifying/_commonCode_localized/gpio_port/1.99/gpio_port_pic32.h
 *
 *    (Wow, that's a lot longer than I'd hoped).
 *
 *    Enjoy!
 *    --Esot.Eric (aka EricWazhung)
 */
