/* mehPL:
 *    This is Open Source-ish, but NOT GPL. I call it mehPL.
 *    I'm not too fond of long licenses at the top of the file.
 *    Please see the bottom.
 *    Enjoy!
 */


//gpio_port_avr.h
//
// This file handles the ARCHITECTURE-SPECIFIC aspects of the macros
// defined in gpio_port.h
//
// This file handles port-stuff, such as reading/writing bits to port-pins
//   or setting such pins as inputs/outputs...
// One of its key features is the use of ONLY the PORTx register-name
//   such that, e.g. setting the direction-register (DDRx, on an AVR)
//   can be accomplished by calling one of these macros with the PORTx
//   register-name.
//   With AVR's optimization-scheme, this generally results in *zero* extra
//   instructions, the math/remapping is handled during compilation-time.
#ifndef __GPIO_PORT_AVR_H__
#define __GPIO_PORT_AVR_H__

#include <avr/io.h>

//BEWARE when implementing for your own architecture!!!!
//  Many of these macros require read-modify-write operation
//  If they're called when interrupts are running, they may result in
//  damaged-data!
//  R-M-W operation *could* be problematic, but in many cases avr-gcc's 
//  optimizer is smart-enough to recognize e.g. 'PORTx |= (1<<BITNUM);' 
//  as an 'sbi' instruction, which is inherently atomic. 
//  HOWEVER, if this same operation was written and compiled for another 
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


//GPIO ports on AVRs are 8-bits wide
#define avr__gpioPORT_width_t    uint8_t



//By accessing, essentially, PORT+<offset> we can refer to ONLY the PORTx
//definition everywhere in our code, regardless of whether we're trying to
//write a pin-value or set a pull-up resistor...
//With -Os, this code will generally reduce to a single register-write 
// instruction;
//  The math of calculating the offsets, etc. will be handled during
//  compile-time.

//I've never run into an AVR that doesn't follow this scheme, though I
//suppose it's plausible they exist.
#define PORTPORTOFFSET  0
#define DDRPORTOFFSET   1
#define PINPORTOFFSET   2

#define DDR_FROM_PORT(PORTx) \
      ((_MMIO_BYTE(&(PORTx) - DDRPORTOFFSET)))

#define PIN_FROM_PORT(PORTx) \
      ((_MMIO_BYTE(&(PORTx) - PINPORTOFFSET)))


//THIS MACRO DOES NOT READ THE "PORT REGISTER"
//This macro reads the values at the pins connected to PORTx
// (Regardless of whether those pins are input or output).
// For AVR: This is essentially "{ return PINx; }"
// For PIC32:                   "{ return PORTx; }"
//THE ARGUMENT, however, is PORTx, regardless of the architecture.
//  NOTE: That because these are all constants, most optimizers will reduce
//  this entire operation to a single instruction
//  e.g. "move to a general-purpose register the value at PINx"
#define avr__gpioPORT_readIO(PORTx)  (PIN_FROM_PORT(PORTx))
//(_MMIO_BYTE(&(PORTx) - PINPORTOFFSET))

#define avr__gpioPORT_getPinVal(PORT, pin) \
   getbit2(PIN_FROM_PORT(PORT), pin)


//This will give *all* the bits on that register, regardless of whether
//those bits are outputs or inputs or anything else.
//(E.G. on an AVR, this will read-back the latched output-bits AND
//read-back whether the pull-up resistors are active on the input-bits!)
//So, only work with the bits you intend to!
// This is *ONLY* intended to enable you to work with the OUTPUT bits
// Any other use would be HIGHLY architecture-specific.
#define avr__gpioPORT_readOutputs(PORTx)   (PORTx)


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
#define avr__gpioPORT_writeAllOutputs(PORTx, value)    ((PORTx) = (value))

//Set a single output-pin to 1
#define avr__gpioPORT_outputOneToPin(PORTx, pin)   \
   setbit2(PORTx, (pin))
//   setbit2((_MMIO_BYTE(&(PORTx) - PORTPORTOFFSET)), (pin))

//Clear a single output-pin to 0
#define avr__gpioPORT_outputZeroToPin(PORTx, pin)   \
   clrbit2(PORTx, (pin))
//   clrbit2((_MMIO_BYTE(&(PORTx) - PORTPORTOFFSET)), (pin))


//This sets several bits/pins on a port as outputs
#define avr__gpioPORT_configureMaskedPinsAsOutputs(PORT, mask) \
         writeMasked2(DDR_FROM_PORT(PORTx), mask, mask)


//Again, for PIC32 (as opposed to, say, AVR) using the Masked macros is
//just as efficient (?) as writing indivudual bit-handling macros...
//So, here, I'll just call them as appropriate.
// (In, say, an AVR, it makes more sense to have separate macros for
//  masked-writes vs. individual bit-writes, as it has SBI and CBI)

//This sets a single bit/pin on a port as an output...
#define avr__gpioPORT_configurePinAsOutput(PORTx, pin)   \
      setbit2(DDR_FROM_PORT(PORTx), pin)
//      setbit2(_MMIO_BYTE(&(PORTx) - DDRPORTOFFSET), pin)


//This writes a value to a port, but only to those bits that are Masked (1)
// This handles writing both 1's and 0's to those masked-bits.
#define avr__gpioPORT_writeMaskedOutputPins(PORTx, Mask, Value) \
   writeMasked2((PORTx), (Mask), (Value))


//Configure PORTx's pins as inputs, per the Mask
// (Mask Bits: 1 = input, 0 = no change)
#define avr__gpioPORT_configureMaskedPinsAsInputs(PORTx, Mask) \
   writeMasked2(DDR_FROM_PORT(PORTx), (Mask), 0x00)

#define avr__gpioPORT_configureMaskedPinsAsOutputs(PORTx, Mask) \
   writeMasked2(DDR_FROM_PORT(PORTx), (Mask), 0xff)

#define avr__gpioPORT_configurePinAsInput(PORTx, pin) \
   clrbit2(DDR_FROM_PORT(PORTx), pin)

#define avr__gpioPORT_configureAllPinsAsInputs(PORT) \
   (DDR_FROM_PORT(PORT) = 0x00)

#define avr__gpioPORT_configureAllPinsAsOutputs(PORT) \
   (DDR_FROM_PORT(PORT) = 0xff)

//Enable the pull-ups on the masked bits.
#define avr__gpioPORT_enablePullupsOnMaskedPins(PORTx, Mask) \
      writeMasked2(PORTx, Mask, Mask)

//Disable the pull-ups on the masked bits.
#define avr__gpioPORT_disablePullupsOnMaskedPins(PORTx, Mask) \
      writeMasked2(PORTx, Mask, 0x00)


//Disable the pull-up resistor on a pin
#define avr__gpioPORT_disablePullupOnPin(PORT, pin) \
   clrbit2(PORT, pin)

//Enable the pull-up resistor on a pin
#define avr__gpioPORT_enablePullupOnPin(PORT, pin) \
   setbit2(PORT, pin)


//Is the pin on the port high...? 0 = no, non-zero = yes
//NOTE that reading the PORTx register on a PIC32 reads the value on
//the pin... Whereas reading the PORTx register on an AVR reads the value
//*written to* that pin (by internal registers), which, if an input, would
//therefore read-back not the value on the pin, but the value of whether
//the internal pull-up resistor is enabled.
// This macro reads the value on the pin (especially if an input)
// It will also read the value written to an output, but should NOT be used
// that way, as its results may be architecture-dependent.
#define avr__gpioPORT_isInputPinHigh_ZNZ(PORTx, pin) \
      isBitHigh_ZNZ(PIN_FROM_PORT(PORTx), pin)
//      isBitHigh_ZNZ(_MMIO_BYTE(&(PORTx) - PINPORTOFFSET), pin)



#endif //__PORTHANDLING_AVR_H__

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
 * /home/meh/_avrProjects/sdramThingZero/21-gittifying/_commonCode_localized/gpio_port/1.99/gpio_port_avr.h
 *
 *    (Wow, that's a lot longer than I'd hoped).
 *
 *    Enjoy!
 *    --Esot.Eric (aka EricWazhung)
 */
