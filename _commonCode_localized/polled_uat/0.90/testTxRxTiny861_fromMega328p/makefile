#/* mehPL:
# *    This is Open Source-ish, but NOT GPL. I call it mehPL.
# *    I'm not too fond of long licenses at the top of the file.
# *    Please see the bottom.
# *    Enjoy!
# */
#
#
# Stolen from anaButtons/0.50/testTiny861
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
TARGET = polled_uart_Test

MCU = attiny861

#Using the default fuses found in _commonCode/_make/atmega324p.mk
FCPU = 8000000UL

MY_SRC = main.c



###### Because sprintf_P is used, we need to start considering code-size:
# Unless you need special functionality, use this.
# (If you're not using any stdio stuff, then use AVR_NO_STDIO = TRUE
#  to save even more code-space)
AVR_MIN_PRINTF = TRUE
# If you need to use floating-point in printf, etc. then use
# AVR_FLOAT_PRINTF = TRUE
# And regardless, it's recomended you look into:
# _commonCode/_make/avrCommon.mk for an explanation of the options



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

### This was necessary for PWM161, but apparently isn't necessary (and
###	causes problems) with the 324p. TODO: I need to look into that.
# Using the default in e.g. polled_uar/0.40/testMega328p/makefile
# seemed to work, so let's try that here.
#Usually DMSTIMER needn't be exclusively included here, but:
#Heartbeat 1.50 references an earlier version of dmsTimer, we need 1.15:
#VER_DMSTIMER = 1.15
#DMSTIMER_LIB = $(COMDIR)/dmsTimer/$(VER_DMSTIMER)/dmsTimer
#include $(DMSTIMER_LIB).mk


# Probably best not to enable the WDT while testing/debugging!
WDT_DISABLE = TRUE
VER_HEARTBEAT = 1.50
HEARTBEAT_LIB = $(COMDIR)/heartbeat/$(VER_HEARTBEAT)/heartbeat
#HEART_DMS = TRUE
HEART_TCNTER = TRUE
CFLAGS += -D'HEARTPIN_HARDCODED=TRUE'
# Heartbeat is on MISO 
#  since most AVRs can drive up to 40mA it shouldn't affect programming
# These are now configured as defaults in atmega328p, it may not be the
# case for other devices. (These pin/ports are from an old device)
#CFLAGS += -D'HEART_PINNUM=PB2'
#CFLAGS += -D'HEART_PINPORT=PORTB'
#CFLAGS += -D'HEART_LEDCONNECTION=LED_TIED_HIGH'
include $(HEARTBEAT_LIB).mk




###### FOR POLLED_UAR AND POLLED_UAT #########

# Polled UAR/T use the TCNTER to determine their bit-timing...
# IF the DMS timer is in this project (e.g. via HeartBeat) we can use its
# timer for TCNTER... 
#CFLAGS += -D'TCNTER_SOURCE_VAR=(DMS_AVRTIMER_TCNT)'
#CFLAGS += -D'TCNTER_SOURCE_OVERFLOW_VAL=(_DMS_OCR_VAL_)'
# Indicate to TCNTER to #include _DMSTIMER_HEADER_ so it can see the
# definition for _DMS_OCR_VAL_
#CFLAGS += -D'_SPECIALHEADER_FOR_TCNTER_=_DMSTIMER_HEADER_'

# On the PWM161, the DMS timer runs on Timer1 (the only timer)
# Which can only run at CLKDIV1
# Which means it *has to* run 16-bit...
# Math-wise, the overflow value is in the thousands (16MHz/DMS_SEC = 1600)
# So we need to set the tcnter_source_t to uint16...
#CFLAGS += -D'PUAT_TCNTER_BITS=16'
#CFLAGS += -D'tcnter_source_t=uint16_t'
#CFLAGS += -D'tcnter_compare_t=int32_t'

###

#Otherwise, you'll have to set these as appropriate...
# e.g. if timer1 is dedicated to the tcnter, you'll have to initialize it,
# and plug its values in here... say Timer1 counts to the value in OCR1A
# (say 250) then resets to 0, then TCNTER_SOURCE_OVERFLOW_VAL = 250
#CFLAGS += -D'TCNTER_SOURCE_VAR=(TCNT1)'
#CFLAGS += -D'TCNTER_SOURCE_OVERFLOW_VAL=(250)'

#### Because TCNTER is used in *both* puat and puar, we need to handle its
# updates in main, explicitly (otherwise, both puat and puar would call
# tcnter_update, etc... which actually should be OK, but slower, right?)
#
# This isn't exactly true, I don't think... I think it's being handled in
# heartbeat... (because I just switched-over from DMS->TCNTER and it
# functions, and why was this here with DMS anyhow?)
CFLAGS += -D'TCNTER_STUFF_IN_MAIN=TRUE'




#BIT_TCNT, when defined, sets the same bit-rate for *all* puars/puats
# So if you need different bit-rates, or need it to be adjustable, then
# comment-out this line, and make sure to set the value in main()...
# Here, the baud-rate is 9600, and 64 is DMS_CLKDIV (which *might* be
# usable here, but I'm not certain that value is the actual division-factor
# vs. possibly register configuration-values)
CFLAGS += -D'BIT_TCNT=(((F_CPU)/(1<<(TCNTER_AVRTIMER_CLKDIV))/9600))'
#CFLAGS += -D'BIT_TCNT=(((F_CPU)/(1<<_DMS_CLKDIV_)/19200)-1)'
#CFLAGS += -D'BIT_TCNT=1667'
#CFLAGS += -D'BIT_TCNT=6000'

#This is kinda hokily-implemented... e.g. for this project we're using an
#external oscillator, so OSCCAL has no purpose...
#But, PolledUar has some warnings about it, so until I clean up things a
#bit, we need this here:
OSCCAL_SET = FALSE

####### TO HERE ##########



####### FOR POLLED_UAT ##########
#Enable the use of puat_sendStringBlocking(char* string)
# This is risky-business, because it blocks *everything* during the entire
# transmission (except maybe interrupts)
# Only use it when you know it's safe...
# It also takes over the tcnter_updates, etc.
CFLAGS += -D'PUAT_SENDSTRINGBLOCKING_ENABLED=TRUE'

#This should be set to a specific value for your project. E.G. uncomment:
#VER_POLLED_UAT = 0.75

# It is only set differently here so we don't have to change this every
# time a new version is created... THIS WILL NOT WORK in a project.
# (Comment it out)
VER_POLLED_UAT = $(notdir $(patsubst %/,%,$(dir $(CURDIR))))

POLLED_UAT_LIB = $(COMDIR)/polled_uat/$(VER_POLLED_UAT)/polled_uat
include $(POLLED_UAT_LIB).mk

####### TO HERE @@@@@@@@



####### FOR POLLED_UAR ##########

VER_POLLED_UAR = 0.40
POLLED_UAR_LIB = $(COMDIR)/polled_uar/$(VER_POLLED_UAR)/polled_uar
include $(POLLED_UAR_LIB).mk

####### TO HERE @@@@@@@@



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

# This is used by timerCommon (which is included by DMS, which is included
# by heartBeat... I need to make this system more sophisticated.

COM_HEADERS += $(COMDIR)/errorHandling/0.99/errorHandling.h



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
# * /home/meh/_avrProjects/sdramThingZero/21-gittifying/_commonCode_localized/polled_uat/0.90/testTxRxTiny861_fromMega328p/makefile
# *
# *    (Wow, that's a lot longer than I'd hoped).
# *
# *    Enjoy!
# *    --Esot.Eric (aka EricWazhung)
# */
