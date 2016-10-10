#/* mehPL:
# *    This is Open Source-ish, but NOT GPL. I call it mehPL.
# *    I'm not too fond of long licenses at the top of the file.
# *    Please see the bottom.
# *    Enjoy!
# */
#
#
# Stolen from sdramThing3.0-0.7
# Stolen from audioThing17


# Having trouble working with inline assembly experiments;
# the assembler is failing with pretty vague errors, and since I'm using
# macros both in cpp-form and asm-form it's hard to determine what exactly
# is responsible for the syntax errors.
# This is supposed to stop after compilation, prior to assembly
#CFLAGS += -S


# This gets modified for compilation to _BUILD/
# if needed after common.mk is included Reference ORIGINALTARGET...
TARGET = puarTest

MCU = attiny84

#Default FUSE settings (in _commonCode/_make/atmega8515.mk) for
# external clock at 16MHz
FCPU = 8000000UL

MY_SRC = main.c


#Location of the _common directory, relative to here...
# this should NOT be an absolute path...
# COMREL is used for compiling common-code locally...
COMREL = ../../../..
COMDIR = $(COMREL)/_commonCode

################# SHOULD NOT CHANGE THIS BLOCK... FROM HERE ############## 
#                                                                        #
# This stuff has to be done early-on (e.g. before other makefiles are    #
#   included..                                                           #
#                                                                        #
#                                                                        #
# If this is defined, we can use 'make copyCommon'                       #
#   to copy all used commonCode to this subdirectory                     #
#   We can also use 'make LOCAL=TRUE ...' to build from that code,       #
#     rather than that in _commonCode                                    #
LOCAL_COM_DIR = _commonCode_localized
#                                                                        #
#                                                                        #
# If use_LocalCommonCode.mk exists and contains "LOCAL=1"                #
# then code will be compiled from the LOCAL_COM_DIR                      #
# This could be slightly more sophisticated, but I want it to be         #
#  recognizeable in the main directory...                                #
# ONLY ONE of these two files (or neither) will exist, unless fiddled with 
SHARED_MK = __use_Shared_CommonCode.mk
LOCAL_MK = __use_Local_CommonCode.mk
#                                                                        #
-include $(SHARED_MK)
-include $(LOCAL_MK)
#                                                                        #
#                                                                        #
#                                                                        #
#Location of the _common directory, relative to here...                  #
# this should NOT be an absolute path...                                 #
# COMREL is used for compiling common-code into _BUILD...                #
# These are overriden if we're using the local copy                      #
# OVERRIDE the main one...                                               #
ifeq ($(LOCAL), 1)
COMREL = ./
COMDIR = $(LOCAL_COM_DIR)
endif
#                                                                        #
################# TO HERE ################################################


# Common "Libraries" to be included
#  haven't yet figured out how to make this less-ugly...
#DON'T FORGET to change #includes...
# (should no longer be necessary... e.g. "#include _HEARTBEAT_HEADER_")


# There're lots of options like this for code-size
# Also INLINE_ONLY's see the specific commonCode...
#CFLAGS += -D'TIMER_INIT_UNUSED=TRUE'



# Probably best not to enable the WDT while testing/debugging!
WDT_DISABLE = TRUE
VER_HEARTBEAT = 1.30
HEARTBEAT_LIB = $(COMDIR)/heartbeat/$(VER_HEARTBEAT)/heartbeat
HEART_DMS = TRUE
CFLAGS += -D'_DMS_CLKDIV_=CLKDIV64'
CFLAGS += -D'HEARTPIN_HARDCODED=TRUE'
# Heartbeat is on MISO 
#  since the ATtiny can drive up to 40mA it shouldn't affect programming
CFLAGS += -D'HEART_PINNUM=PB2'
CFLAGS += -D'HEART_PINPORT=PORTB'
CFLAGS += -D'HEART_LEDCONNECTION=LED_TIED_HIGH'
include $(HEARTBEAT_LIB).mk





###### FOR POLLED_UAR #########

# Polled UAR uses the TCNTER to determine its bit-timing...
# IF the DMS timer is in this project (e.g. via HeartBeat) we can use its
# timer for TCNTER... 
CFLAGS += -D'TCNTER_SOURCE_VAR=(TCNT0)'
CFLAGS += -D'TCNTER_SOURCE_OVERFLOW_VAL=(_DMS_OCR_VAL_)'
# Indicate to TCNTER to #include _DMSTIMER_HEADER_ so it can see the
# definition for _DMS_OCR_VAL_
CFLAGS += -D'_SPECIALHEADER_FOR_TCNTER_=_DMSTIMER_HEADER_'

#Otherwise, you'll have to set these as appropriate...
# e.g. if timer1 is dedicated to the tcnter, you'll have to initialize it,
# and plug its values in here... say Timer1 counts to the value in OCR1A
# (say 250) then resets to 0, then TCNTER_SOURCE_OVERFLOW_VAL = 250
#CFLAGS += -D'TCNTER_SOURCE_VAR=(TCNT1)'
#CFLAGS += -D'TCNTER_SOURCE_OVERFLOW_VAL=(250)'

#BIT_TCNT, when defined, sets the same bit-rate for *all* puars/puats
# So if you need different bit-rates, or need it to be adjustable, then
# comment-out this line, and make sure to set the value in main()...
# Here, the baud-rate is 9600, and 64 is DMS_CLKDIV (which *might* be
# usable here, but I'm not certain that value is the actual division-factor
# vs. possibly register configuration-values)
CFLAGS += -D'BIT_TCNT=(((F_CPU)/64/9600)-1)'


#This is kinda hokily-implemented... e.g. for this project we're using an
#external oscillator, so OSCCAL has no purpose...
#But, PolledUar has some warnings about it, so until I clean up things a
#bit, we need this here:
OSCCAL_SET = FALSE
#This should be set to a specific value for your project. E.G. uncomment:
#VER_POLLED_UAR = 0.40

# It is only set differently here so we don't have to change this every
# time a new version is created... THIS WILL NOT WORK in a project.
# (Comment it out)
VER_POLLED_UAR = $(notdir $(patsubst %/,%,$(dir $(CURDIR))))

POLLED_UAR_LIB = $(COMDIR)/polled_uar/$(VER_POLLED_UAR)/polled_uar
include $(POLLED_UAR_LIB).mk

##### TO HERE ######




# HEADERS... these are LIBRARY HEADERS which do NOT HAVE SOURCE CODE
# These are added to COM_HEADERS after...
# These are necessary only for make tarball... 
#   would be nice to remove this...
# NOTE: These CAN BE MULTIPLY-DEFINED! Because newer headers almost always 
#    include older headers' definitions, as well as new ones
#   i.e. bithandling...
#  the only way to track all this, for sure, is to hunt 'em all down
#  (or try make tarball and see what's missing after the compilation)
VER_BITHANDLING = 0.95
BITHANDLING_HDR = $(COMDIR)/bithandling/$(VER_BITHANDLING)/
COM_HEADERS += $(BITHANDLING_HDR)
# This is so #include _BITHANDLING_HEADER_ can be used in .c and .h files.
CFLAGS += -D'_BITHANDLING_HEADER_="$(BITHANDLING_HDR)/bithandling.h"'




include $(COMDIR)/_make/reallyCommon2.mk


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
# * /home/meh/_avrProjects/sdramThingZero/21-gittifying/_commonCode_localized/polled_uar/0.40/testTiny84/makefile
# *
# *    (Wow, that's a lot longer than I'd hoped).
# *
# *    Enjoy!
# *    --Esot.Eric (aka EricWazhung)
# */
