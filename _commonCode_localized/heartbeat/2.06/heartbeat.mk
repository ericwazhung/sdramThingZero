#/* mehPL:
# *    This is Open Source-ish, but NOT GPL. I call it mehPL.
# *    I'm not too fond of long licenses at the top of the file.
# *    Please see the bottom.
# *    Enjoy!
# */
#
#
# heartbeat depends on the hfModulation library, as well as dmsTimer when __HEART_NODMS__ is not defined elsewhere...

# The following variables are expected to be defined in the calling makefile(s)
# HEARTBEAT_LIB
# COMDIR

# These two are mutually-exclusive
# Both can be false...
# HEART_DMS (TRUE or FALSE)
# HEART_TCNTER (TRUE or FALSE)

# DMS_EXTERNALUPDATE (Only required if HEART_DMS is TRUE)	(TRUE or FALSE)
# WDT_DISABLE (TRUE or FALSE optional?)

# optional definitions (in gcc -D listing) TRUE or FALSE
# __TODO_WARN__ - warn about TODOs
# __UNKN_WARN__ - warn about unknowns/untesteds
# __OPTIM_WARN__ - warn of potential optimizations
# __ERR_WARN__   - warn of potential errors (divide-by-zero, etc...)
# HEART_REMOVED = TRUE/FALSE -- Remove all heart code for size-testing

#only include heartbeat once...
ifneq ($(HEARTBEAT_INCLUDED),true)

HEARTBEAT_INCLUDED = true




#The default naming-scheme for the heart Pin/Port is (from the project
# makefile):
#CFLAGS += -D'HEART_PINNUM=PB1'
#CFLAGS += -D'HEART_PINPORT=PORTB'
#CFLAGS += -D'HEART_LEDCONNECTION=LED_TIED_HIGH'
# If these are not defined, then use the programming-header
# THIS IS NEW a/o audioThing50 via ATmega328P
# Other devices do not have PGM_xxx_yyy_NAME defined
# So, I suppose, no handling's really needed to make this
# backwards-compatible with old-style project makefiles. 
# But, forward-compatibility may be an issue if the project makefile
# doesn't include these, AND PGM_... isn't defined in the chip's makefile
# But I think that'll just cause an "undefined" error, which is exactly
# what I'd want... and would've been the same, had I forgotten to define
# HEART_PINNUM in the project's makefile, prior to this update.
######## NOOOOOOOOOOOO!!!! ############
# This info is CFLAG'd, NOT makefile variables!
#ifndef HEART_PINNUM
#HEART_PINNUM = $(PGM_MISO_PIN_NAME)
#endif
#ifndef HEART_PINPORT
#HEART_PINPORT = $(PGM_MISO_PORT_NAME)
#endif
#ifndef HEART_LEDCONNECTION
#HEART_LEDCONNECTION = $(LED_TIED_HIGH)
#endif
#The same can be handled in the .c file... see there.

ifndef HEART_DMS
HEART_DMS=FALSE
endif
CFLAGS += -D'_HEART_DMS_=$(HEART_DMS)'

#HEART_TCNTER is new, and so hopefully can not have to be explicitly
#defined in a makefile... unless TRUE is desired.
# (Realistically, this should be the case for HEART_DMS, too... no?)
ifndef HEART_TCNTER
HEART_TCNTER = FALSE
endif
CFLAGS += -D'_HEART_TCNTER_=$(HEART_TCNTER)'


CFLAGS += -D'_HEARTBEAT_HEADER_="$(HEARTBEAT_LIB).h"'

ifeq ($(HEART_REMOVED),TRUE)
CFLAGS += -D'__HEART_REMOVED__=TRUE'
endif

#Don't define it otherwise, because ifdef is used rather than if true...
ifeq ($(WDT_DISABLE), TRUE)
CFLAGS += -D'_WDT_DISABLE_=$(WDT_DISABLE)'
endif

# The following variables are defined here for called makefile(s)
ifndef HFMODULATION_LIB
VER_HFMODULATION = 1.00
HFMODULATION_LIB = $(COMDIR)/hfModulation/$(VER_HFMODULATION)/hfModulation
#include $(HFMODULATION_LIB).mk
endif

#Add this library's source code to SRC
ifneq ($(HEART_REMOVED),TRUE)
SRC += $(HEARTBEAT_LIB).c
else
# This is a TOTAL hack...
# Apparently I wasn't thinking when creating HEART_REMOVED, which removes
# heartbeat.c from SRC...
# And since SRC is responsible for localizing... we get an issue.
COM_HEADERS += $(dir $(HEARTBEAT_LIB))
endif


#include the libraries this one depends on
# WTF. Why shouldn't this be in the ifndef above? It's not functioning
# with LCDreIDer47 that way...
include $(HFMODULATION_LIB).mk

#only include dmsTimer if HEART_DMS is TRUE
ifeq ($(HEART_DMS),TRUE)

#UGH: All this because I had 'HEART_DMS = TRUE ' with a trailing space
#CFLAGS += "!!!OK!!!"
ifndef DMSTIMER_LIB
VER_DMSTIMER = 1.16
DMSTIMER_LIB = $(COMDIR)/dmsTimer/$(VER_DMSTIMER)/dmsTimer
include $(DMSTIMER_LIB).mk
endif

#else
#CFLAGS += "!!!WTF!!! HEART_DMS=$(HEART_DMS)"
endif

ifeq ($(HEART_TCNTER),TRUE)
ifndef TCNTER_LIB
VER_TCNTER = 0.31
TCNTER_LIB = $(COMDIR)/tcnter/$(VER_TCNTER)/tcnter
include $(TCNTER_LIB).mk
endif
endif

endif
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
# * /home/meh/_avrProjects/sdramThingZero/21-gittifying/_commonCode_localized/heartbeat/2.06/heartbeat.mk
# *
# *    (Wow, that's a lot longer than I'd hoped).
# *
# *    Enjoy!
# *    --Esot.Eric (aka EricWazhung)
# */
