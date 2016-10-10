#/* mehPL:
# *    This is Open Source-ish, but NOT GPL. I call it mehPL.
# *    I'm not too fond of long licenses at the top of the file.
# *    Please see the bottom.
# *    Enjoy!
# */
#
#
# stolen from oneAxisHolder-36
# stolen from heartbeat 2.05
#
# myMCU.mk is a makefile-snippet that will be included in the project's
# main makefile.
# The idea, then, is that it can be copied from project-to-project, rather
# than modifying these lines in every project-makefile
# THIS IS NEW, and likely not-implemented in most projects...
# So, these lines will have to be found and modified in the
# project-makefile
# (or replace them to include this)


#################################
###                           ###
###      IMPORTANT            ###
###
###  When Changing "MCU", below:
###                           
###  In my code, some devices' defaults make use of a crystal oscillator!
###  If you choose one of these devices as your MCU then run 'make fuse':
###
###  Your Device MAY NO LONGER RESPOND 
###   (until you attach the selected clock source)
###
###  Make Sure the Fuse Settings are Acceptable for your needs!
###    (before running 'make fuse')
###
###  Look at _commonCode/_make/<MCU>.mk
###  DON'T TRUST the comments in that file, I've a tendancy to copy-paste
###   and forget to update the comments (bit-explanations, etc.)
###  USE THE datasheet, instead!
###
###  And consider defining your own fuses here, instead.
###
##################################





####   MCU   ####
#
# Select your MCU, here.
# This is used quite a bit throughout most projects:
#  * It tells gcc what device to compile-for
#  * It tells avr-dude what device to program
# It also selects a makefile-snippet in _commonCode/_make/<MCU>.mk, which:
#  * Sets up my commonly-used fuse-settings 
#    (often different than factory-default)
#  * Can auto-configure certain pins (e.g. the heartbeat/button on MISO)
# ### NOTE ###
#  It's important you check <MCU>.mk to make sure it doesn't select a
#  crystal-oscillator when you don't have one, otherwise you might get an
#  unresponsive chip after running 'make fuse'
# ### NOTE 2 ###
#  DON'T TRUST the comments in <MCU>.mk, see above.
# MCU should be all lower-case.
MCU = atmega8515



####   FCPU   ####
#
# Enter your CPU frequency, here.
# This is also used quite a bit throught most projects to configure
#  timing-related stuff, e.g.:
#  * Heart time (a full fade-in/out cycle should be 8 seconds)
#  * Baud Rates
# FCPU is the makefile definition. If needed in C/Preprocessor-code
#  use F_CPU, which is automatically-defined to the value of FCPU, here.
# FCPU depends on your oscillator-selection and fuse-settings, which may
#  be different than the device defaults. 
# (e.g. 'make fuse' usually disables CLKDIV8 to run at 8MHz instead of the
#  default 1MHz)
# See _commonCode/_make/<MCU>.mk, or override with your own FUSE values,
#  below.
# FCPU should end with 'UL' 
#  (an unsigned-long integer, which is apparently 32-bits on 8-bit AVRs)
# a/o v33: Using the PLL to clock at 16MHz, see fuses below...
FCPU = 16000000UL



####   FUSES   ####
#
# This is where you can override my default fuse-settings 
# (My defaults can be found in _commonCode/_make/<MCU>.mk)
# (which may not match the factory-defaults, and for some devices assume
#  there's an external oscillator. Especially atmega644 and atmega8515.)
# Fuses can be programmed with 'make fuse'
# FUSEn values are in hex, with leading 0x
#FUSEX = 0x..
#FUSEH = 0x..
# This taken from sdramThing4.0-14/makefile (EXTERNAL_CLOCK)
FUSEL = 0xf0
FUSEH = 0xd9



#####  AVRDUDE_MY_FTDI  #####
#
# This is new, attempting to use the FT2232H as a chip-programmer
# This is ifdef'ed, so its value doesn't really matter...
# Either leave this, or comment it out.
# (Default, when commented-out, is the usb-tiny-ISP)
#NOTE: a/o sdramThingZero-2: usb-tiny-isp is having difficulties...
AVRDUDE_MY_FTDI = TRUE


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
# * /home/meh/_avrProjects/sdramThingZero/21-gittifying/myMCU.mk
# *
# *    (Wow, that's a lot longer than I'd hoped).
# *
# *    Enjoy!
# *    --Esot.Eric (aka EricWazhung)
# */
