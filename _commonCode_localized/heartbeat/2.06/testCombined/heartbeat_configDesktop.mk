#/* mehPL:
# *    This is Open Source-ish, but NOT GPL. I call it mehPL.
# *    I'm not too fond of long licenses at the top of the file.
# *    Please see the bottom.
# *    Enjoy!
# */
#
#
#
#

######## HEARTBEAT CONFIGURATION #########

############ DESKTOP EDITION #############


# Heartbeat can be tested on a desktop PC by choosing this file
# And commenting-out 'MCU=' in the main 'makefile'
#
# There are some limitations
#  as the desktop-test was developed long-after heartbeat (for the AVR)
#  Some of the options available to the AVR version are not available for
#  desktop-testing.
#  (Which kinda goes-against the point of having a desktop test-
#   application in the first place, no?)

# See the file heartbeat_config.mk for more-detailed explanations of the
#  options used here.






####   VER_HEARTBEAT   ####
#
#  ******************************************
#  *** DO THIS NOW, while you're in here: ***
#  ******************************************
#
# VER_HEARTBEAT SHOULD BE SET *HERE* to the specific version used by this 
#  project. (see notes, below)
#
#  E.G. UNCOMMENT AND *UPDATE* VER_HEARTBEAT *here* to match the folder:
#           _commonCode_localized/heartbeat/<version>
#       
#VER_HEARTBEAT = 2.02




# Note Re: above:
# VER_HEARTBEAT *should* be uncommented and set-correctly, above.
#
#  The *only* case it *can* be commented-out is if this is the 
#   heartbeat test-program, in which case commonTestEasyVersion.mk is
#   handling it via THIS_commonThing definitions in the project's 
#   'makefile'.  This is due to laziness, on my part... I don't want to
#   have to modify the test-program code/configuration every time there's
#   minor version-change while I'm in the middle of a bigger project.
#  SO it's QUITE LIKELY that if VER_HEARTBEAT, above, is commented-out,
#   then its value is *outdated*.
#  Please update it and uncomment it NOW, even if you're using the 
#   heartbeat test-program. Somewhere down the line you might modify this
#   project for your own needs, and then forget to do it, and wonder why it
#   broke.




####   HEART_NOPIN   ####
#
# This tells heartbeat not to use the AVR pin-writing macros
# INSTEAD: it causes heartbeat to look for the file 'heartNoPinStuff.h'
#  which must contain a few functions...
# In a desktop-environment, they needn't really do anything
# BUT: this could also be used, e.g., to use heartbeat code on a completely
#  different MCU architecture. 
CFLAGS += -D'HEART_NOPIN=TRUE'



####   WDT_DISABLE   ####
#
# This MUST be TRUE for desktop-testing
WDT_DISABLE = TRUE



####   HEART_DMS / HEART_TCNTER   ####
#
# DMS is (currently) specific to AVRs, 
#   So cannot be used in desktop-testing
# TCNTER is capable of being desktop-tested, 
#   BUT requires a bunch of configuration options and additional
#   support-code that isn't implemented here.
#
# NEITHER of these should be selected.
#HEART_TCNTER = TRUE
#HEART_DMS = TRUE



####   HEART PIN STUFF   (AVRs only)   ####
# Obviously, in a desktop-application, we have no pins to work with...
# But some configuration-options are still necessary.

# HEARTPIN_HARDCODED
#
# Not sure this is necessary...
# Just leave this TRUE
CFLAGS += -D'HEARTPIN_HARDCODED=TRUE'

# HEART_PINNUM / HEART_PINPORT
#
# These are irrelevent in a desktop-testing situation.
#CFLAGS += -D'HEART_PINNUM=PB2'
#CFLAGS += -D'HEART_PINPORT=PORTB'


# HEART_LEDCONNECTION
#
# Not sure this is necessary, either...
# Just leave it like this
CFLAGS += -D'HEART_LEDCONNECTION=LED_TIED_HIGH'


# HEART_PULLUP_DELAY
#
# This *does* affect the desktop-application
# But it can be safely-ignored
CFLAGS += -D'HEART_PULLUP_DELAY=255'



# HEARTBEAT_LIB
# This is rather ugly, but it has gotten the job done for quite some time.
#
# The project's 'makefile' will do the actual inclusion
#  via 'include $(HEARTBEAT_LIB).mk'
#
# Just leave this alone...
HEARTBEAT_LIB = $(COMDIR)/heartbeat/$(VER_HEARTBEAT)/heartbeat


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
# * /home/meh/_avrProjects/sdramThingZero/21-gittifying/_commonCode_localized/heartbeat/2.06/testCombined/heartbeat_configDesktop.mk
# *
# *    (Wow, that's a lot longer than I'd hoped).
# *
# *    Enjoy!
# *    --Esot.Eric (aka EricWazhung)
# */
