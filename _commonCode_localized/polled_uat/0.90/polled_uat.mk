#/* mehPL:
# *    This is Open Source-ish, but NOT GPL. I call it mehPL.
# *    I'm not too fond of long licenses at the top of the file.
# *    Please see the bottom.
# *    Enjoy!
# */
#
#
# polled_uat depends on tcnter

# The following variables are expected to be defined 
#  in the calling makefile(s)
# POLLED_UAT_LIB - Path to polled_uat.mk/c including filename, 
#  excluding extension
# COMDIR
#
# PUAT_INLINE (optional, NOT a CFLAG) to inline functions...
# PUAT_HFM_PERCENT (optional, NOT a CFLAG) to use the HFM to add precision
#                  to the BIT_TCNT
#                  (e.g. if tcntsPerBit = 16.4, PUAT_HFM_PERCENT = 40
#
# CFLAGS += -D'...
# NUMPUATS	-	Number of polled_uats (Required if >1)
#
# BIT_TCNT	- Should probably no longer be used...
#             see polled_uar0.40 for better notes
#
# CONSIDERATIONS:
#
# *** These could have defaults... ***
# TCNTER_SOURCE_VAR - e.g. TCNT0
#   might take some work if 
#   TCNTER_SOURCE_VAR is a variable which needs to be externed
#   TCNTER_SOURCE_EXTERNED = TRUE to add externing...
# TCNTER_SOURCE_MAX   - e.g. OCR0A
#   also, could be a constant value...
#
# OPTIONAL (change BOTH if either)
# tcnter_source_t - e.g. uint8_t, uint16_t
# tcnter_compare_t - e.g. int16_t, int32_t
#
# TCNTER_STUFF_IN_MAIN  - probably should be default...
#                        prevents running tcnter_update from uat_update()
#								 also tcnter_init from uat_init()
#                        don't forget to call them in the main while loop!
# CFLAGS += -D'PU_PC_DEBUG=TRUE' -> PC Debugging printf's...

#only include polled_uat once...
ifneq ($(POLLED_UAT_INCLUDED),true)
POLLED_UAT_INCLUDED = true

CFLAGS += -D'_POLLED_UAT_INCLUDED_=$(POLLED_UAT_INCLUDED)' 
CFLAGS += -D'_POLLED_UAT_HEADER_="$(POLLED_UAT_LIB).h"' 

ifndef TCNTER_LIB
VER_TCNTER = 0.30
TCNTER_LIB = $(COMDIR)/tcnter/$(VER_TCNTER)/tcnter
include $(TCNTER_LIB).mk
endif

ifeq ($(PUAT_INLINE), TRUE)
CFLAGS += -D'_PUAT_INLINE_=TRUE'
# Extract the directory we're in and add this to COM_HEADERS...
COM_HEADERS += $(dir $(POLLED_UAT_LIB))
else
CFLAGS += -D'_PUAT_INLINE_=FALSE'
#Add this library's source code to SRC
SRC += $(POLLED_UAT_LIB).c
endif

ifdef PUAT_HFM_PERCENT
CFLAGS += -D'_PUAT_HFM_PERCENT_=$(PUAT_HFM_PERCENT)'
ifndef HFMODULATION_LIB
VER_HFMODULATION = 1.00
HFMODULATION_LIB = $(COMDIR)/hfModulation/$(VER_HFMODULATION)/hfModulation
include $(HFMODULATION_LIB).mk
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
# * /home/meh/_avrProjects/sdramThingZero/21-gittifying/_commonCode_localized/polled_uat/0.90/polled_uat.mk
# *
# *    (Wow, that's a lot longer than I'd hoped).
# *
# *    Enjoy!
# *    --Esot.Eric (aka EricWazhung)
# */
