/* mehPL:
 *    This is Open Source-ish, but NOT GPL. I call it mehPL.
 *    I'm not too fond of long licenses at the top of the file.
 *    Please see the bottom.
 *    Enjoy!
 */


//These macros are merely for renaming/reargumenting original
//0.99-compatible gpio_port macros to 1.99-compatible
//THIS SHOULD BE REMOVED at some point (2.00?)

#ifndef __GPIO_PORT_099COMPAT_H__
#define __GPIO_PORT_099COMPAT_H__

#define clrpinPORT(pin, PORT) \
         gpioPORT_outputZeroToPin(PORT, pin)

#define setpinPORT(pin, PORT) \
         gpioPORT_outputOneToPin(PORT, pin)

#define writepinPORT(pin, PORT, value) \
         gpioPORT_outputValueToPin(PORT, pin, value)


#define setPORToutMasked(PORT, mask) \
         gpioPORT_configureMaskedPinsAsOutputs(PORT, mask)

#define setoutPORT(pin, PORT) \
         gpioPORT_configurePinAsOutput(PORT, pin)

#define setPORTout(PORT) \
         gpioPORT_configureAllPinsAsOutputs(PORT)

#define setPORTinMasked(PORT, mask) \
         gpioPORT_configureMaskedPinsAsInputs(PORT, mask)

#define setinPORT(pin, PORT) \
         gpioPORT_configurePinAsInput(PORT, pin)

#define setPORTin(PORT) \
         gpioPORT_configureAllPinsAsInputs(PORT)

#define getpinPORT(pin, port) \
         gpioPORT_getPinVal(port, pin)

#define setPinStatePORT(pin, port, state) \
         gpioPORT_setPinState(port, pin, state)

#endif //__GPIO_PORT_099COMPAT_H__
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
 * /home/meh/_avrProjects/sdramThingZero/21-gittifying/_commonCode_localized/gpio_port/1.99/gpio_port_099compat.h
 *
 *    (Wow, that's a lot longer than I'd hoped).
 *
 *    Enjoy!
 *    --Esot.Eric (aka EricWazhung)
 */
