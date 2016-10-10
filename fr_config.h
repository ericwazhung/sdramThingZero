/* mehPL:
 *    This is Open Source-ish, but NOT GPL. I call it mehPL.
 *    I'm not too fond of long licenses at the top of the file.
 *    Please see the bottom.
 *    Enjoy!
 */


#ifndef __FR_CONFIG_H__
#define __FR_CONFIG_H__


//IF this is uncommented, the free-runner will ONLY run from FR_ROWS rows,
//and loop back around.
//IF it's COMMENTED-OUT, the free-runner will run from *all* the rows
//available (and detected) on the device.
//4096//2048 //32 //(numRows) //2048 1022; //528; //1023; //1024; 
// numRows-1;
//#define FR_ROWS 32


//The "READ" command, in the free-runner, typically reads the next active
//bank... e.g. if we're *in* bank 1, then the next read will be at bank 2
// (3 -> 0)
//There's really no reason to change this, but it makes more sense when
//equating FR_PA__BANKS_AWAY to this
// (so we know that we're doing *everything* to the next bank)
//REALISTICALLY: This shouldn't be changed...
// Apparently (recently-discovered) there exist SDRAM chips that only have
// TWO banks (rather than four)... In such a case, we *have* to work with
// "the other" bank... so that one's always *one* bank away.
#define FR_READ__BANKS_AWAY   1

//Precharge and Activate are loaded into a page, prepping for a future read
// Should it precharge/activate the *next* bank, or one later down the line
// (Don't forget that when starting, the intermediate banks must be active)
//This Option was originally intended for the PLAUSIBILITY that
// execution of the Precharge/Activate commands might take longer than
// read-burst of a single page.
// A) That's not really the case, even at 133MHz, each of these take only a
//    few SDRAM-clocks (a few column-bursts within a page)
// B) Some SDRAM chips only have *two* banks... so we'd *always* be
//    performing the Precharge/Activate commands on *the other* bank
//So, Don't change this...
#define FR_PA__BANKS_AWAY  FR_READ__BANKS_AWAY//1

#endif //__CONFIG_H__
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
 * /home/meh/_avrProjects/sdramThingZero/21-gittifying/fr_config.h
 *
 *    (Wow, that's a lot longer than I'd hoped).
 *
 *    Enjoy!
 *    --Esot.Eric (aka EricWazhung)
 */
