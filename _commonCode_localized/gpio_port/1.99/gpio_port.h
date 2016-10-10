/* mehPL:
 *    This is Open Source-ish, but NOT GPL. I call it mehPL.
 *    I'm not too fond of long licenses at the top of the file.
 *    Please see the bottom.
 *    Enjoy!
 */


//gpio_port 1.99-2
#ifndef __GPIO_PORT_H__
#define __GPIO_PORT_H__

//1.99-3ish  THIS HAS BEEN TESTED COMPATIBLE WITH sTZ-16
//           (Prior to SEI_RESTORE() change, 
//            this is build-identical with 0.99)

//1.99-2 Extracted everything from grbl-abstracted-30/porthandling_avr.h
//1.99-1 Extracted everything from grbl-abstracted-30/porthandling.h
//
//1.99 - This is NEW
//        Extracting the gpio-related stuff from:
//         bithandling 0.99-7 combined with
//         port_handling from grbl-abstracting-30
//
//        This is being created alongside bithandling 1.99
//
//        This is not complete, thus 1.99, rather than 2.00
//
//       Major Differences from the bithandling 0.xx series:
//        NAMES
//         e.g. setpinPORT() -> gpioPORT_setpin()
//        ARGUMENTS
//         e.g. setpinPORT(PIN, PORT) -> gpioPORT_setpin(PORT, PIN)
//         PORT will always be the FIRST argument

// DESCRIPTION:
//
// This file handles port-stuff, such as reading/writing bits to port-pins
//   or setting such pins as inputs/outputs...
// One of its key features is the use of ONLY the PORTx register-name
//   such that, e.g. setting the direction-register (DDRx, on an AVR)
//   can be accomplished by calling one of these macros with the PORTx
//   register-name as the argument.
//
//   With AVR's optimization-scheme, this generally results in *zero* extra
//   instructions, the math/remapping is handled during compilation-time.
//   This is in direct-opposition to e.g. "digital_write()" type functions
//   which execute in real-time, performing considerable amounts of math 
//   and jumping in order to relate a pin number to its actual pin/port 
//   registers.
//   Using these macros should be as fast as using direct 'PORT=' or 'DDR='
//   statements. Though a tiny bit of overhead may be added for some
//   risk-avoidance (e.g. Read-Modify-Write macros may be surrounded by
//   CLI/SEI where it may be unnecessary, And some processor-architectures
//   have requirements for, e.g. disabling the "analog" circuitry on
//   input-pins before they can be made digital-inputs. Rather'n having two
//   separate macros, these are combined into a single macro, which can
//   result in some redundant register-writes, resulting in a few
//   unnecessary clock-cycles, in *some* cases. Though, these are usually
//   handled upon init, and generally don't slow actual pin-toggling.

//NOTE: IMPORTANT:
// SOME pins on SOME devices need special handling to be configured as
// inputs/outputs.
// THESE macros will handle the vast-majority of normal pins.
// HOWEVER, some pins will default to a mode which can NOT be overridden
// merely by using these macros
// E.G. the JTAG pins on a PIC32 can only be used for I/O if the JTAG unit
// is explicitly disabled.


//gpio_port makes use of bithandling
#include _BITHANDLING_HEADER_

#include "gpio_port_099compat.h"

// Some of this code is MCU-architecture-specific, so include the
// associated architecture-specific file.
//  Essentially: #include "gpio_port_avr.h" or "gpio_port_pic32.h", etc.,
//  as appropriate
#include CONCAT_HEADER(gpio_port_,__MCU_ARCH__)

//Because much of this code is MCU-architecture-specific, 
// the actual definitions are in the associated architecture-specific file.
//But it's nice to have their names and details here, for easy-viewing...
// Check out bithandling.h for the definition of ARCHIFY()
//  But the jist is e.g. 
//   ARCHIFY( gpioPORT_width_t )
//     -> avr__gpioPORT_width_t
//    (Which is defined in gpio_port_avr.h)

//In some cases it's handy to e.g. read an entire port into a variable
// This defines a type for that purpose, with the appropriate width for
// your architecture (e.g. AVRs' ports are 8bits, PIC32s' are 16bits)
#define gpioPORT_width_t \
         ARCHIFY( \
          gpioPORT_width_t \
         )



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
// For AVR: This is essentially "{ return PINx; }"
// For PIC32:                   "{ return PORTx; }"
//THE ARGUMENT, however, is PORTx, regardless of the architecture.
// (Doing-so allows for fewer #defines... e.g. for AVR:
//       #define inputPinBit  3
//       #define inputPinPORT PORTA
//     //#define inputPinPIN  PINA  //This is unnecessary
//  Again, Doing-so also allows for more hardware-abstraction...
//   as most devices will have "Ports" with "pins" attached to its bits
//   but their register-definitions will vary.
//  NOTE: That because these are all constants, most optimizers will reduce
//  this entire operation to a single instruction
//  e.g. (AVR) "move to a general-purpose register the value at PINx"
#define gpioPORT_readIO(PORTx)  \
         ARCHIFY( \
          gpioPORT_readIO(PORTx) \
         )


//A rare occurance, but say one wants to read the port-outputs into a
//variable... do something with that variable... then rewrite the port
//outputs. But can't do-so in a single write-masked operation...
//(Don't forget to surround it with CLI_SAFE() and SEI_RESTORE()!)
//Anyways, one might wish to *read* the values *written* to a
//port-output-register.
//This will give *all* the bits on that register, regardless of whether
//those bits are outputs or inputs or anything else.
//(E.G. on an AVR, this will read-back the latched output-bits AND
//read-back whether the pull-up resistors are active on the input-bits!)
//So, only work with the bits you intend to!
// This is *ONLY* intended to enable you to work with the OUTPUT bits
// Any other use would be HIGHLY architecture-specific.
#define gpioPORT_readOutputs(PORTx) \
         ARCHIFY( \
          gpioPORT_readOutputs(PORTx) \
         )

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
//
// THIS MACRO SHOULD ONLY BE USED if the *entire* port is
// being used for a single-purpose (e.g. an 8bit data-bus) where *all* pins
// are outputs.
// Use gpio_writeOutputPins_masked() instead!
//TODO: This name might be a bit confusing...
//      Might think it means write the same value (1, 0) to all outputs
//      rather'n write all bits in value to their associate bits on ALL the
//      outputs...
#define gpioPORT_writeAllOutputs(PORTx, value) \
         ARCHIFY( \
          gpioPORT_writeAllOutputs(PORTx, value) \
         )



//Configure the masked-bits as outputs on a PORT
// (Mask Bits: 1 = output, 0 = no change)
#define gpioPORT_configureMaskedPinsAsOutputs(PORT, mask) \
         ARCHIFY( \
          gpioPORT_configureMaskedPinsAsOutputs(PORT, mask) \
         )

//Configure one pin on a PORT as an output
// (This may seem unnecesary, with ..._masked, above... but some
// architectures can handle single-bit-writes more efficiently (e.g. AVR))
#define gpioPORT_configurePinAsOutput(PORTx, Bit) \
         ARCHIFY( \
          gpioPORT_configurePinAsOutput(PORTx, Bit) \
         )

//Configure one pin on a PORT as an input
#define gpioPORT_configurePinAsInput(PORTx, pin) \
         ARCHIFY( \
          gpioPORT_configurePinAsInput(PORTx, pin) \
         )

//Configure the masked-bits as inputs on a PORT
// (Mask Bits: 1 = input, 0 = no change)
#define gpioPORT_configureMaskedPinsAsInputs(PORT, mask) \
         ARCHIFY( \
          gpioPORT_configureMaskedPinsAsInputs(PORT, mask) \
         )

#define gpioPORT_configureAllPinsAsInputs(PORT) \
         ARCHIFY( \
          gpioPORT_configureAllPinsAsInputs(PORT) \
         )

#define gpioPORT_configureMaskedPinsAsOutputs(PORT, mask) \
         ARCHIFY( \
          gpioPORT_configureMaskedPinsAsOutputs(PORT, mask) \
         )

#define gpioPORT_configureAllPinsAsOutputs(PORT) \
         ARCHIFY( \
          gpioPORT_configureAllPinsAsOutputs(PORT) \
         )

//Disable the masked pull-up resistors on a PORT
// (Mask Bits: 1 = Disable the pull-up, 0 = no change)
#define gpioPORT_disablePullupsOnMaskedPins(PORT, mask) \
         ARCHIFY( \
          gpioPORT_disablePullupsOnMaskedPins(PORT, mask) \
         )


//Enable the masked pull-up resistors on a PORT
// (Mask Bits: 1 = Enable the pull-up, 0 = no change)
#define gpioPORT_enablePullupsOnMaskedPins(PORT, mask) \
         ARCHIFY( \
          gpioPORT_enablePullupsOnMaskedPins(PORT, mask) \
         )


//Disable the pull-up resistor on a pin
#define gpioPORT_disablePullupOnPin(PORT, pin) \
         ARCHIFY( \
          gpioPORT_disablePullupOnPin(PORT, pin) \
         )

//Enable the pull-up resistor on a pin
#define gpioPORT_enablePullupOnPin(PORT, pin) \
         ARCHIFY( \
          gpioPORT_enablePullupOnPin(PORT, pin) \
         )


//Write a value to a port, but only to those bits that are Masked (1)
// This handles writing both 1's and 0's to those masked-bits.
//THIS IS NOT SAFE when PORTx may be modified e.g. in an interrupt...
//Surround it with CLI_SAFE() and SEI_RESTORE()!
#define gpioPORT_writeMaskedOutputPins(PORT, mask, value) \
         ARCHIFY( \
          gpioPORT_writeMaskedOutputPins(PORT, mask, value) \
         )

//Clear an ouput-pin to 0
#define gpioPORT_outputZeroToPin(PORTx, Pxn) \
         ARCHIFY( \
          gpioPORT_outputZeroToPin(PORTx, Pxn) \
         )

//Set an output-pin to 1
#define gpioPORT_outputOneToPin(PORTx, Pxn) \
         ARCHIFY( \
          gpioPORT_outputOneToPin(PORTx, Pxn) \
         )

//TODO: This may be improved on a per-architecture-basis...
//      (and, maybe, that'd be a good idea for many of these macros...
//       #ifndef them, here, with default *functional*-but-not-optimals? )
#define gpioPORT_outputValueToPin(PORTx, pin, pinValue) \
         ( (pinValue) \
           ? gpioPORT_outputOneToPin(PORTx, pin)   \
           : gpioPORT_outputZeroToPin(PORTx, pin)  )

//Is the input-pin high? 
// Returns: 0        = pin is low
//          NON-ZERO = pin is high
// (generally: returns the binary value of the bit... e.g. bit6 would
//  return 0 if the pin is low and 0100 0000 = 0x40 if the pin is high
//  This is much more efficient than returning 0 or 1, as that would
//  require bit-shifts... but if you need that, see gpioPORT_getPinVal() )
//
// NOTE that this may return undefined (architecture-specific) values for
// pins that are *not* already configured as inputs.
#define gpioPORT_isInputPinHigh_ZNZ(PORTx, Pxn) \
         ARCHIFY( \
          gpioPORT_isInputPinHigh_ZNZ(PORTx, Pxn) \
         )

//TODO: There's probably a more efficient method...
//      ANOTHER prime example of something that could be implemented
//      as a default, and only overridden if the architecture has a
//      more-efficient method...
#define gpioPORT_getPinVal(PORT, pin) \
         ARCHIFY( \
          gpioPORT_getPinVal(PORT, pin) \
         )
//         (gpioPORT_isInputPinHigh_ZNZ(PORT, pin) >> pin)



//TODO: This is a bit AVR-Specific, no?
//There're four *typical* possibilities for a pin
// (Some architectures may have more)
// High, Low, Z, and Pulled-Up
//  (Generally, the input register can still be read in any of these 
//   states, so "input" isn't really a port-state)
// These are to be used in setPinStatePORT()
// (Plausibly: there may be a time when getPinStatePORT could be useful
// NOTE: PINSTATE_LOW=0 and PINSTATE_HIGH=1 can be used to compare to a 
//        pin input-value (e.g. if(getpinPORT(...) == PINSTATE_LOW))
//        It's kinda hokey, since PINSTATE and PINVAL are different
//        concepts, but PINVAL doesn't exist. IT IS used this way, in
//        heartbeat 2.00
// These values were originally defined for AVRs...
#define PINSTATE_LOW    0  //Don't change this without looking at NOTE3
#define PINSTATE_HIGH   1  //Ditto
#define PINSTATE_Z      2
#define PINSTATE_PU     3


//There're four typical possibilities for a pin
// (as determined by its PORT and DDR values)
// High, Low, Z, and Pulled-Up
//  (Generally, the input register can still be read in any of these
//   states, so "input" isn't really a port-state)
// (Plausibly: there may be a time when getPinStatePORT could be useful
//  TODO: Revise these states to match e.g. (DDRn | PORTn<<1) ? )
// NOTE: This can lead to a pretty large bit of inline-code UNLESS it gets
// optimized, which it should if it's given *constants* as arguments
// IOW: Don't use this for non-constants, turn it into a function first.
// (could just create a wrapper for this?)
// NOTE2: This does *not* (yet?) take into consideration TRANSITIONS
//        e.g. what was the previous port-state, and which would be the
//        best order to go about switching to the next)
// NOTE3: PINSTATE_LOW=0 and PINSTATE_HIGH=1 can be used to compare to a 
//        pin input-value (e.g. if(getpinPORT(...) == PINSTATE_LOW))
//        It's kinda hokey, since PINSTATE and PINVAL are different
//        concepts, but PINVAL doesn't exist. IT IS used this way, in
//        heartbeat 2.00
// NOTE4: This is *ONLY* implemented for these four states
//        Don't go tryin' ta use this with other architectures,
//        implementing new states, expecting it to still remain
//        architecture-unspecific!
//These are in bithandling.h...
//#define PINSTATE_LOW    0  //Don't change this without looking at NOTE3
//#define PINSTATE_HIGH   1  //Ditto
//#define PINSTATE_Z      2
//#define PINSTATE_PU     3

#define gpioPORT_setPinState(port, pin, state) \
   ({ \
      switch(state) \
      { \
         case PINSTATE_HIGH: \
            gpioPORT_configurePinAsOutput(port, pin); \
            gpioPORT_outputOneToPin(port, pin); \
            break; \
         case PINSTATE_LOW: \
            gpioPORT_configurePinAsOutput(port, pin); \
            gpioPORT_outputZeroToPin(port, pin); \
            break; \
         case PINSTATE_Z: \
            gpioPORT_configurePinAsInput(port, pin); \
            gpioPORT_disablePullupOnPin(port, pin); \
            break; \
         case PINSTATE_PU: \
         default: \
            gpioPORT_configurePinAsInput(port, pin); \
            gpioPORT_enablePullupOnPin(port,pin); \
            break; \
      } \
      {}; \
    })







#endif //__GPIO_PORT_H__

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
 * /home/meh/_avrProjects/sdramThingZero/21-gittifying/_commonCode_localized/gpio_port/1.99/gpio_port.h
 *
 *    (Wow, that's a lot longer than I'd hoped).
 *
 *    Enjoy!
 *    --Esot.Eric (aka EricWazhung)
 */
