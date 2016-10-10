#/* mehPL:
# *    This is Open Source-ish, but NOT GPL. I call it mehPL.
# *    I'm not too fond of long licenses at the top of the file.
# *    Please see the bottom.
# *    Enjoy!
# */
#
#
# Stolen from sdramThing4.5-14
#  and modified with stuff from oneAxisHolder-36
# Stolen from audioThing17
# a/o v10ish, updating to heartbeat2.05 style, rC3...



# Having trouble working with inline assembly experiments;
# the assembler is failing with pretty vague errors, and since I'm using
# macros both in cpp-form and asm-form it's hard to determine what exactly
# is responsible for the syntax errors.
# This is supposed to stop after compilation, prior to assembly
#CFLAGS += -S

# I think I handled this elsewhere, or at least documented it, better than
# this... -E causes the object files to contain preprocessed output... e.g.
# to 'grep -r stdio _BUILD'
#CFLAGS += -E

# ALSO HANDY:
# highlight.sh, not yet implemented as a make option...





# This gets modified for compilation to _BUILD/
# if needed after common.mk is included Reference ORIGINALTARGET...
TARGET = sdramThingZero_CKEtest


####   myMCU.mk   ####
#
# Modify the file myMCU.mk to suit your device
# (Then you can reuse it in other projects)
include myMCU.mk




###### Because sprintf_P is used, we need to start considering code-size:
#
# Again, if stdio.h is not included anywhere, having this enabled will
# *still* include it in the build...
#
# Having just removed all uses of stdio.h disabling this resulted in a
# code-size of 8192+40 (overflowed) dropping to 7258
# MIN_PRINTF takes 974 bytes just for its inclusion!
# Having this commented-out WHILE #including <stdio.h>
# Resulted in 8096 bytes
# Uncommenting this resulted in 7840
AVR_MIN_PRINTF = TRUE

# PROBABLY handled via above's means...
#Necessary for printing floats via sprintf... (gotoRamped, speed)
#LDFLAGS += -Wl,-u,vfprintf -lprintf_flt -lm
# Add lm if used... (also necessary for floats?)
#LDFLAGS += -lm



MY_SRC = main.c \
			sdram_general.c \
			sdram_freeRunner.c



# Use the short projInfo to save code-space... 
#CFLAGS += -D'PROJINFO_SHORT=TRUE'

# This completely removes the project header information from flash...
PROJINFO_OVERRIDE:=""





# Override the default optimization level:
#OPT = 3



# This makes it so "extern inline" functions are not compiled anywhere
# but inline... dunno if it'll change anything
CFLAGS += -fgnu89-inline

# This didn't seem to work
CFLAGS += -Wpacked-bitfield-compat

#Using `-Winline' will warn when a function marked `inline' could not be
#substituted, and will give the reason for the failure.
CFLAGS += -Winline




# I have configured OSCCAL for this chip...
# When using another chip I might have trouble
# This will warn whenever burning fuses...
#OSCCAL_SET = TRUE
OSCCAL_SET = FALSE





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
# (This should be unnecessary, as it's the default...)
CENTRAL_COMDIR = ../../../_commonCode

####   comdirEpoxyBlob.mk   ####
# This file needs to be included *here* 
# (imediately after CENTRAL_COMDIR... before any commonCode inclusions)
# Don't move it!
# If you're *really* curious, take a look inside...
include comdirEpoxyBlob.mk





# Common "Libraries" to be included
#  haven't yet figured out how to make this less-ugly...
#DON'T FORGET to change #includes...
# (should no longer be necessary... e.g. "#include _HEARTBEAT_HEADER_")


# There're lots of options like this for code-size
# Also INLINE_ONLY's see the specific commonCode...
#CFLAGS += -D'TIMER_INIT_UNUSED=TRUE'

#BECAUSE we've upgraded to the latest heartbeat, most-likely the
# PUAR/PUAT/TCNTER/anaButtons code includes older versions of the things 
# they use...
# Including heartbeat before them should assure the latest-versions are
# used... (e.g. TCNTER)

# Don't include HEART source-code, for size tests...
# a/o heartbeat2.05, and sdramThing3.0-11, this saves 734 bytes...
#HEART_REMOVED = TRUE


#####   heartbeat_config.mk   #####
# This contains the configuration-options for the heartbeat
# Including pinout, etc.
# Please view this file, and update VER_HEARTBEAT (as explained there)
include heartbeat_config.mk


include $(HEARTBEAT_LIB).mk



# Polled UAR/T stuff:
#### Because TCNTER is used in *both* puat and puar, we need to handle its
# updates in main, explicitly (otherwise, both puat and puar would call
# tcnter_update, etc... which actually should be OK, but slower, right?)
#
# This isn't exactly true, I don't think... I think it's being handled in
# heartbeat... (because I just switched-over from DMS->TCNTER  in the
# polled_uatTxRxTest, and it functions, and why was this there with DMS 
# anyhow?)
CFLAGS += -D'TCNTER_STUFF_IN_MAIN=TRUE'

#BIT_TCNT, when defined, sets the same bit-rate for *all* puars/puats
# So if you need different bit-rates, or need it to be adjustable, then
# comment-out this line, and make sure to set the value in main()...
# Here, the baud-rate is 9600, and 64 is DMS_CLKDIV (which *might* be
# usable here, but I'm not certain that value is the actual division-factor
# vs. possibly register configuration-values)
#CFLAGS += -D'BIT_TCNT=(((F_CPU)/(1<<(TCNTER_AVRTIMER_CLKDIV))/9600))'
# This +1 was found experimentally... rounding-up issue? Shouldn't be...
# CLKDIV defaults to 64, 16MHz -> 52.08 tcnts... Something to ponder.
CFLAGS += -D'BIT_TCNT=(((F_CPU)/(1<<(TCNTER_AVRTIMER_CLKDIV))/9600)+1)'

#This is kinda hokily-implemented... e.g. for this project we're using an
#external oscillator, so OSCCAL has no purpose...
#But, PolledUar has some warnings about it, so until I clean up things a
#bit, we need this here:
OSCCAL_SET = FALSE

CFLAGS += -D'PUAT_SENDSTRINGBLOCKING_ENABLED=TRUE'

VER_POLLED_UAT = 0.90
POLLED_UAT_LIB = $(COMDIR)/polled_uat/$(VER_POLLED_UAT)/polled_uat
include $(POLLED_UAT_LIB).mk

VER_POLLED_UAR = 0.40
POLLED_UAR_LIB = $(COMDIR)/polled_uar/$(VER_POLLED_UAR)/polled_uar
include $(POLLED_UAR_LIB).mk





# HEADERS... these are LIBRARY HEADERS which do NOT HAVE SOURCE CODE
# These are added to COM_HEADERS after...
# These are necessary only for make tarball... 
#   would be nice to remove this...
# NOTE: These CAN BE MULTIPLY-DEFINED! Because newer headers almost always 
#    include older headers' definitions, as well as new ones
#   i.e. bithandling...
#  the only way to track all this, for sure, is to hunt 'em all down
#  (or try make tarball and see what's missing after the compilation)


VER_BITHANDLING = 1.99

# gpio_port is now its own entity separate from bithandling
# BUT, for backwards-compatibility, we need it to be auto-included
#  wherever bithandling is included...
# THIS does-so via bithandling.mk
BITHANDLING_INCLUDES_GPIO_PORT = true

BITHANDLING_LIB = $(COMDIR)/bithandling/$(VER_BITHANDLING)/bithandling
include $(BITHANDLING_LIB).mk


ifeq (1,0)


# This implements read in the old-style sdramThing3.5... 
# (strobe CS, one-shots bypassed)
#VER_SDRFR = 3.50ncf

# This is an intermediate step toward one-shots
# (strobe CS_En, one-shots bypassed)
#VER_SDRFR = 3.98ncf

# This is an intermediate step *using* one-shots, but only for R/W
# And with the SDRAM Clock = AVR Clock
#VER_SDRFR = 3.99ncf

# So far, this attempts one-shots AND SDRAM Clock = 2*AVR Clock
# NOW: main.c overrides 2* clock...
VER_SDRFR = 4.00ncf

# The overlapping-columns/double-write issue seems to have cleared itself
# up... this dun seem right... nothing in that regard should've changed in
# software. Retesting the old versions...
# No problem in either of these
# BUT: Duh... Didn't the 16MHz crystal work OK, and 20 didn't?
# That seems to be the case, now we're getting the last column messed-up
# as expected.
#VER_SDRFR = 4.00ncf4
#VER_SDRFR = 4.00ncf3
	
SDRFR_HDR := $(COMDIR)/sdrfr/$(VER_SDRFR)/
COM_HEADERS += $(SDRFR_HDR)
CFLAGS += -D'_SDRFR_HEADER_="$(SDRFR_HDR)/sdrfr.h"'

endif



VER_ERRORHANDLING = 0.99
ERRORHANDLING_HDR = $(COMDIR)/errorHandling/$(VER_ERRORHANDLING)/
COM_HEADERS += $(ERRORHANDLING_HDR)

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
# * /home/meh/_avrProjects/sdramThingZero/21-gittifying/makefile
# *
# *    (Wow, that's a lot longer than I'd hoped).
# *
# *    Enjoy!
# *    --Esot.Eric (aka EricWazhung)
# */
