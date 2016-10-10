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
# Having trouble working with inline assembly experiments;
# the assembler is failing with pretty vague errors, and since I'm using
# macros both in cpp-form and asm-form it's hard to determine what exactly
# is responsible for the syntax errors.
# This is supposed to stop after compilation, prior to assembly
#CFLAGS += -S

# This test was set-up while avr-GCC4.8 is unusable (due to libraries'
# inavailability). So use avr-GCC4.4
# a/o heartbeat2.02: as I recall that was a fault of the installation (?)
# And regardless I don't have avr-gcc4.8 on my new system.
# But if you have trouble with 4.8, try another. I had 4.4 and now I have
# 4.7... Kinda wish I'da made mention of which libraries were causing
# trouble, as I don't really remember what the issue was.
#CC = /usr/local/bin/avr-gcc

# TARGET gets modified for compilation to _BUILD/
# So don't reference it in your code, unless you know what you're doing.
# E.G. if needed after reallyCommon3.mk is included, use ORIGINALTARGET...
TARGET = heartbeatTest



####   TEST_DESKTOP   ####
#
# If this is defined, we'll compile a desktop application.
# Otherise, we'll compile for the AVR selected in myMCU.mk
# (Note: This is used with ifdef/ifndef... 
#  comment-it-out don't set it FALSE)
TEST_DESKTOP = TRUE


ifdef TEST_DESKTOP

# Make this info available to main.c
CFLAGS += -D'_TEST_DESKTOP_=TRUE'

endif



ifndef TEST_DESKTOP


####   myMCU.mk   ####
#
# Modify the file myMCU.mk to suit your device
# (Then you can reuse it in other projects)
include myMCU.mk



endif


### BECAUSE this is a commonThing test program, these should be defined:
THIS_COMMONTHING = HEARTBEAT
THIS_commonThing = heartbeat
#(They are used in commonThingEasyVersion.mk, which is included later)



MY_SRC = main.c

#Other source-code, e.g. that in _commonCode[_localized]/, will be included
#in SRC elsewhere.



####   CENTRAL_COMDIR   ####
# Just ignore this, for now.
#
# This configuration is for a commonThing test-program located within in 
#  my central _commonCode directory.
#   (e.g. ~/_commonCode/commonThing/version/test/)
#
# But, unless you're me, this project most-likely contains its own local
#  copy of the necessary commonCode at:
#    ./_commonCode_localized
#
# So the value of CENTRAL_COMDIR, here, is irrelevent for you, for now.
#
# LATER: If you're interested in centralizing the commonCode so that it can
# be used by other projects, look into 'comdirEpoxyBlob.mk' and change 
# or remove *this* as appropriate.
#
# Just ignore this, for now.
CENTRAL_COMDIR = ../../../../_commonCode


####   comdirEpoxyBlob.mk   ####
# This file needs to be included *here* 
# (imediately after CENTRAL_COMDIR... before any commonCode inclusions)
# Don't move it!
# If you're *really* curious, take a look inside...
include comdirEpoxyBlob.mk









ifdef TEST_DESKTOP

include heartbeat_configDesktop.mk


else

#####   heartbeat_config.mk   #####
# This contains the configuration-options for the heartbeat
# Including pinout, etc.
# Please view this file, and update VER_HEARTBEAT (as explained there)
include heartbeat_config.mk


endif








# FOR THIS PARTICULAR PROJECT (a commonThing test-program):
#
# For this particular case, we can extract the version-number of
# THIS_commonThing from the local directory structure, so that this 
# makefile (and indeed most--if not all--related test-code) needn't be 
# modified with each version of THIS_commonThing (as defined above)... 
#
# (When you set VER_<THIS_COMMONTHING>, commonTestEasyVersion.mk 
#  will be automatically disabled. BUT...)
#
# THIS LINE SHOULD BE REMOVED IN A CUSTOM PROJECT, 
#   And VER_<commonThings> should be explicitly-defined.
#   (VER_HEARTBEAT can AND SHOULD be set in heartbeat_config.mk)
include $(COMDIR)/_make/commonTestEasyVersion.mk



include $(HEARTBEAT_LIB).mk








# HEADERS... these are LIBRARY HEADERS which do NOT HAVE C SOURCE CODE
# These are added to COM_HEADERS in order to 'make localize'

# NOTE: These CAN BE MULTIPLY-DEFINED! Because newer headers almost always 
#    include older headers' definitions, as well as new ones
#   i.e. bithandling...
#  the only way to track all this, for sure, is to hunt 'em all down
#  (or try 'make localize' and see what's missing after the compilation)
#  (The need for multiple-versions has been greatly-reduced as-of
#  the ability to e.g. '#include _COMMONTHIHNG_HEADER_')

VER_BITHANDLING = 0.95
BITHANDLING_HDR = $(COMDIR)/bithandling/$(VER_BITHANDLING)/
COM_HEADERS += $(BITHANDLING_HDR)
# This is so #include _BITHANDLING_HEADER_ can be used in .c and .h files.
CFLAGS += -D'_BITHANDLING_HEADER_="$(BITHANDLING_HDR)/bithandling.h"'



# These are only used for TEST_DESKTOP...
# (And need to be better-handled here)
# But their unconditional inclusion, here, also assures that they will be
# available, should you decide to run TEST_DESKTOP.

VER_STDIN_NONBLOCK=0.15
STDIN_NONBLOCK_LIB=$(COMDIR)/__std_wrappers/stdin_nonBlock/$(VER_STDIN_NONBLOCK)/stdin_nonBlock
CFLAGS += -D'_STDIN_NONBLOCK_HEADER_="$(STDIN_NONBLOCK_LIB).h"'
COM_HEADERS += $(STDIN_NONBLOCK_LIB).*

VER_ERRNO_HANDLEERROR=0.15
ERRNO_HANDLEERROR_LIB=$(COMDIR)/__std_wrappers/errno_handleError/$(VER_ERRNO_HANDLEERROR)/errno_handleError
CFLAGS += -D'_ERRNO_HANDLEERROR_HEADER_="$(ERRNO_HANDLEERROR_LIB).h"'
COM_HEADERS += $(ERRNO_HANDLEERROR_LIB).*



include $(COMDIR)/_make/reallyCommon3.mk


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
# * /home/meh/_avrProjects/sdramThingZero/21-gittifying/_commonCode_localized/heartbeat/2.06/testCombined/makefile
# *
# *    (Wow, that's a lot longer than I'd hoped).
# *
# *    Enjoy!
# *    --Esot.Eric (aka EricWazhung)
# */
