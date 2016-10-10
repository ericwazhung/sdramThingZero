#/* mehPL:
# *    This is Open Source-ish, but NOT GPL. I call it mehPL.
# *    I'm not too fond of long licenses at the top of the file.
# *    Please see the bottom.
# *    Enjoy!
# */
#
#
# Sample makefile 
# original by Eric B. Weddington, et al. for WinAVR
# modified for _common "libraries" and other goodies by Eric Hungerford
#
# Maybe someday to be released to the Public Domain
# Please read the make user manual!
#
#
# On command line:
# make = Make software.
# make clean = Clean out built project files.
#
# To rebuild project do "make clean" then "make".
#

#Note: Stolen from atmega162.mk and modified to match 24Scam9's makefile

# APPARENTLY ORDER-WRITTEN MATTERS
ifndef FUSEH
FUSEH = 0xd9
endif
# Internal Clock
#FUSEL = 0xe1
# External Clock
ifndef FUSEL
FUSEL = 0xe0
endif

# 8515 Fuses:
# 1 = not-programmed (disabled)

# Fuse high byte:
#0xd9  = 1 1 0 1   1 0 0 1
#        ^ ^ ^ ^   ^ \ / ^---- BOOTRST
#        | | | |   |  +------- BOOTSZ 1..0
#        | | | |   +---------- EESAVE
#        | | | +-------------- CKOPT
#        | | +---------------- SPIEN
#        | +------------------ WDTON
#        +-------------------- S8515C

# Fuse low byte:
# 0xe1 = 1 1 1 0   0 0 0 x
#        ^ ^ \ /   \--+--/
#        | |  |       +-----+- CKSEL 3..0 
#        | |  |             |   0001 internal clock, 1MHz
#        | |  |             |_  0000 external crystal
#        | |  +--------------- SUT 1..0 (crystal osc)
#        | +------------------ BODEN
#        +-------------------- BODLEVEL


# Could this be IFed for different MCUs?
#.PHONY: fuse
#fuse:
#	$(AVRDUDE) -U lfuse:w:$(FUSEL):m -U efuse:w:$(FUSEX):m -U hfuse:w:$(FUSEH):m


# This is copied and modified from attiny861.mk from atmega328p.mk...

# These are makefile variables, NOT preprocessor variables
# The file _make/avrCommon.mk will convert them to e.g.:
# _PGM_MISO_PIN_NAME_ preprocessor definitions via #defines...

# (As I have a tendancy to copy/paste files and forget to change some
# things... these PIN/PORT_NAME variables are set from the atmega8515 
# datasheet)

# usually used for Heartbeat
PGM_MISO_PIN_NAME = PB6
PGM_MISO_PORT_NAME = PORTB

# usually used for Tx0
PGM_MOSI_PIN_NAME = PB5
PGM_MOSI_PORT_NAME = PORTB

# usually used for Rx0
PGM_SCK_PIN_NAME = PB7
PGM_SCK_PORT_NAME = PORTB



#/* mehPL:
# *    I would love to believe in a world where licensing shouldn't be
# *    necessary; where people would respect others' work and wishes, 
# *    and give credit where it's due. 
# *    A world where those who find people's work useful would at least 
# *    send positive vibes--if not an email.
# *    A world where we wouldn't have to think about the potential
# *    legal-loopholes that others may take advantage of.
# *
# *    Until that world exists:
# *
# *    This software and associated hardware design is free to use,
# *    modify, and even redistribute, etc. with only a few exceptions
# *    I've thought-up as-yet (this list may be appended-to, hopefully it
# *    doesn't have to be):
# * 
# *    1) Please do not change/remove this licensing info.
# *       (You can add notes *surrounding* it!)
# *    2) Please do not change/remove others' credit/licensing/copyright 
# *         info, where noted. 
# *    3) If you find yourself profiting from my work, or derivatives,
# *         thereof, please send me a beer, a trinket, or cash is always 
# *         handy as well.
# *         (Please be considerate. E.G. if you've reposted my work on a
# *          revenue-making (ad-based) website, please think of the
# *          years and years of hard work that went into this!)
# *    4) If you/your company *intend(s)* to profit from my work, 
# *         you must get my permission, first. 
# *    5) No permission is given for my work to be used in Military, NSA,
# *         or other creepy-ass purposes. No exceptions. And if there's 
# *         any question in your mind as to whether your project qualifies
# *         under this category, you must get my explicit permission.
# *
# *    The open-sourced project this originated from is ~98% the work of
# *    Me, "Esot.Eric," except where otherwise noted.
# *    That includes the "commonCode" and makefiles.
# *    Thanks, of course, should be given to those who worked on the tools
# *    I've used: avr-dude, avr-gcc, gnu-make, vim, usb-tiny, and 
# *    I'm certain many others. 
# *    And, as well, to the countless coders who've taken time to post
# *    solutions to issues I couldn't solve, all over the internets.
# *
# *
# *    I'd love to hear of how this is being used, suggestions for
# *    improvements, etc!
# *         
# *    The creator of the original code and original hardware can be
# *    contacted at:
# *
# *        EricWazHung At Gmail Dotcom ("Me")
# *
# *    This code's origin (and latest versions) can be found at:
# *          https://github.com/ericwazhung
# *      or: https://github.com/esot-eric-test
# *      or Plausibly under a project page at: 
# *          https://hackaday.io/hacker/40107-esoteric
# *
# *    Older versions may be locatable at: (no longer updated)
# *        https://code.google.com/u/ericwazhung/
# *
# *
# *    The site associated with the original open-sourced project is at:
# *
# *        https://sites.google.com/site/geekattempts/
# *
# *    If any of that ever changes, I will be sure to note it here, 
# *    and add a link at the pages above.
# *
# * This license added to the original file located at:
# * /home/meh/_avrProjects/sdramThingZero/21-gittifying/_commonCode_localized/_make/atmega8515.mk
# *
# *    (Wow, that's a lot longer than I'd hoped).
# *
# *    Enjoy!
# *    --Esot.Eric (aka EricWazhung)
# */
