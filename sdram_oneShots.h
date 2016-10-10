/* mehPL:
 *    This is Open Source-ish, but NOT GPL. I call it mehPL.
 *    I'm not too fond of long licenses at the top of the file.
 *    Please see the bottom.
 *    Enjoy!
 */


#ifndef __SDRAM_ONE_SHOTS_H__
#define __SDRAM_ONE_SHOTS_H__

#include "sdram_general.h"

//One-Shots are necessary for interfacing an SDRAM chip with a "host" whose
// outputs toggle slower than a single SDRAM-clock-period.
//
//The effect of e.g. bringing /CS low for longer than one SDRAM
// clock-period is that of executing the same command multiple times
// back-to-back.
//SOME operations cannot be repeatedly executed back-to-back, such as
// Load-Mode-Register, Activate, and Precharge. 
//
//THUS: Insert a "one-shot" circuit between the "host's" slow /CS
// *output* and the SDRAM's much-faster /CS input. Thus, even when the host
// is slower than the SDRAM-clock, the /CS signal--as received by the SDRAM
// chip(s)--will only last for *one* SDRAM-clock period. Thus, the
// instruction will only be executed *once*.
//
//Similarly:
// CKE, the "Clock-Enable" input, can be one-shotted, as well. Doing-so
// allows for precise control over read/write burst-timing.
// For GENERAL-PURPOSE SDRAM-interfacing, this is unnecessary, but useful.
// When using a Free-Runner setup--wherein the Command/Address pins are
// *directly* wired to Data I/Os on the same SDRAM DIMM--it's absolutely 
// necessary to know (or control) *exactly* how many SDRAM-clock cycles 
// occur between the execution of the read/write instruction, and the time
// when the data is available for reading/writing. Most "Hosts" cannot
// control their GPIOs with the necessary level of precision. However, they
// *can* control CKE, and using a one-shot allows for controlling *exactly*
// how many SDRAM clock-cycles are acknowledged, and when.
//
//Again, CKE one-shotting isn't *required* for a general-purpose interface
//with an SDRAM, but could be useful for burst writes/reads. But it *is*
//pretty much *required* for a Free-Runner setup. Thus... Since it's useful
//to both cases, I'll include it here, rather than in sdram_freeRunner

// When bypassing the CKE-One-Shot, the value from sdram_clockEn/Disable()
//  is output directly (with one clock-delay, which shouldn't matter)
//  And stays high or low as long as it's so-set.
#warning "Is this appropriately-named? Seems CKE_ONESHOT_BYPASS is active-LOW...?"
#define sdramFR_bypassCKE_OneShot() \
         clrpinPORT(CKE_ONESHOT_BYPASS_PIN, \
                                CKE_ONESHOT_BYPASS_PORT)

#define sdramFR_enableCKE_OneShot() \
         setpinPORT(CKE_ONESHOT_BYPASS_PIN, \
                                CKE_ONESHOT_BYPASS_PORT)



#define sdramFR_overrideCS_OneShot()  \
         clrpinPORT(CS_ONESHOT_OVERRIDE_PIN, \
                                CS_ONESHOT_OVERRIDE_PORT)

#define sdramFR_unoverrideCS_OneShot() \
         setpinPORT(CS_ONESHOT_OVERRIDE_PIN, \
                                CS_ONESHOT_OVERRIDE_PORT)


#endif //__SDRAM_ONE_SHOTS_H__
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
 * /home/meh/_avrProjects/sdramThingZero/21-gittifying/sdram_oneShots.h
 *
 *    (Wow, that's a lot longer than I'd hoped).
 *
 *    Enjoy!
 *    --Esot.Eric (aka EricWazhung)
 */
