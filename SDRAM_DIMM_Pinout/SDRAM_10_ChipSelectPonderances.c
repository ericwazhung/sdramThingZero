/* mehPL:
 *    This is Open Source-ish, but NOT GPL. I call it mehPL.
 *    I'm not too fond of long licenses at the top of the file.
 *    Please see the bottom.
 *    Enjoy!
 */


//This is for testing the functionality of CKE

// We'll use an AVR
// With pretty much the same pinout as sT4.5
// The Free-Runner is wired directly to the AVR
// The AVR and SDRAM run at the same clock-speed
//  But, with CKE-testing, the *only* instruction that should be
//  timing-sensitive (and thus in ASM) is the CKE strobe



// MOST Address/Command pins are directly-wired to a DQ for Free-Running
// (see SDRAM_DIMM_PINOUT/SDRAM_[1-8]*)
//
// CHIP-SELECT differs:
//
//  There IS a DQ associated with Chip-Select
//  BUT:
//   In order for the host to write/read a bit on that *DQ*,
//    that DQ cannot be *directly* wired to the /CS input!
//   Otherwise, when that DQ is written/read Low,
//    it would activate a command.
//
//---------------
// One obvious solution is a multiplexer:
//
//                   .-----------------------------------------.
//                   |                                         |
//                   |  V+                                     |
//                   |   ^               ----------------      |
// --------          |   |  |\          |     SDRAM      |     |
// Host    |         |   '->|0\         |                |     |
//         |         |      |  |------->| /CS     DQ_/CS |><---'
//     /CS |><-------+----->|1/         |                |
//         |                |/|         |                |
//         |                  |
//   CS_En |>-----------------'
//         |
//
//
// Thus, when CS_En is high,
//  whatever's output by eitherthe SDRAM's DQ_/CS or by the Host's /CS
//  will be responsible for driving the SDRAM's /CS
//  This mode, then, is true when:
//   Sending commands from the Host to the SDRAM
//   In Free-Running (when the SDRAM sends commands back to itself)
// And, when CS_En is low,
//  The SDRAM's /CS will always be High (inactive)
//  Allowing the host to write/read the SDRAM's DQ_/CS 
//   (even when DQ_/CS=L)
//  Without causing a new instruction
//
//----------------
// sdramThing4.5 (and earlier) used this same idea,
//  but implemented it slightly differently
//  (due mostly to available ICs)
//
//
//                   .--------------------------------------------.
//                   |                                            |
// --------          |                      ----------------      |
// Host    |         |                     |   SDRAM        |     |
//         |         |     _____           |                |     |
//      CS |><-------+--->|     \          |                |     |
//         |              |      |O------->| /CS      DQ_CS |><---'
//   CS_En |>-------------|_____/          |                |
//         |
//
// Note that CS
//  as output by the Host, AND as stored in DQ_CS 
//  is now active-high
//   No-biggie, just a software-thing.
//
//------------------
// A plausibly More-Obvious solution is a buffer
//  with an output-enable
//  and a pull-up resistor
//
//                   .--------------------------------------------------.
//                   |             V+                                   |
//                   |              ^                                   |
//                   |              |                                   |
//                   |              /             ----------------      |
// --------          |              \            |     SDRAM      |     |
// Host    |         |      |\      /            |                |     |
//         |         |      | \     |            |                |     |
//     /CS |><-------+----->|  >----+----------->| /CS     DQ_/CS |><---'
//         |                | /                  |                |
//         |                |/o                  
//         |                  |
//   CS_En |>-----------------'
//         |
//  
//-----------------
// For testing sdramThingZero with the CKE test...
//  Was initially thought, maybe, CKE would allow for solving this problem
//  However: e.g. read: CKE-Strobe to advance the burst...
//                       DQ_/CS may be active
//                       Which would load that instruction
//                write; (worse?)
//    (MAYBE: could handle *single* reads/writes, but definitely only one)
//
// TODO: Ponder...?
//
//-------------------------------
// An easier alternative, especially for testing this particular phase:
//
// --------                        --------------------
// Host    |                      |    SDRAM
//         |                      |
//     /CS |>--------------+----->| /CS
//         |               |      |
//         |               /      |
//         |               \      |
//         |               /      |
//         |               |      |
//  /DQ_CS |>--------------+----><| /DQ_CS
//         |                      |
//
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
 * /home/meh/_avrProjects/sdramThingZero/21-gittifying/SDRAM_DIMM_Pinout/SDRAM_10_ChipSelectPonderances.c
 *
 *    (Wow, that's a lot longer than I'd hoped).
 *
 *    Enjoy!
 *    --Esot.Eric (aka EricWazhung)
 */
