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
####   comdirEpoxyBlob.mk   ####

# YOU CAN IGNORE THIS FILE


# This file includes a block of code necessary for
# _commonCode vs. _commonCode_localized
# That particular block of code has to occur *before* any actual commonCode
# is included, and so therefore can't be in reallyCommon3.mk
# Nor can it be located inside _commonCode, because it determines *where*
# _commonCode is located.


# This is, roughly, what it used to look like in the project's 'makefile'
# which is just absurd to bombard a newbie with.



# THERE's REALLY NO REASON TO CHANGE THIS FILE...
# Though it may be informative.



#SKIP THIS BLOCK...
#   'till "### TO HERE ###"


#CENTRAL_COMDIR:
# THE FOLLOWING DEFINITIONS, THIS EXPLANATION, AND THE "###" BLOCK BELOW
# CAN BE IGNORED (But must be here)
#
# So here's an explanation:
#CENTRAL_COMDIR:
#Location of the centralized commonCode directory, relative to here...
#
# The contents of the _commonCode_localized directory, here, can be moved 
# to a centralized location 
# (e.g. CENTRAL_COMDIR="/home/you/_commonCode").
# Then each of your projects can use that same commonCode directory...
# E.G. Bug-fixes to _commonCode will then trickle-down to all your 
# projects.
#
# To do this, run 'make delocalize'
#
# Then, to later copy *from* the centralized _commonCode directory
#  back to _commonCode_localized:
#
# run 'make localize'
#
# CENTRAL_COMDIR should NOT be an absolute path... (nor '~')
#  Keep them it in this format.
# ALSO: CENTRAL_COMDIR should *not* end with '/'
#  OLDNOTE:
#  ...COMREL is used for compiling common-code locally (into _BUILD).
#  NOW (a/o reallyCommon3.mk++, heartbeat 2.05):
#  ...COMREL is extracted for that purpose FROM COMDIR
#     and no longer needs to be explicitly-defined
#
# UNTIL you plan to use a centralized _commonCode directory, you can
# just ignore this and the following "####" block.


# MY STRUCTURE:
# Projects are usually located in:
#   ~/_avrProjects/<projectName>/<projectVersion>
# And the central _commonCode directory is located at:
#   ~/_commonCode
# So, CENTRAL_COMDIR="../../../_commonCode" is where it's at.
#
# NOTABLE EXCEPTION:
# commonThings that have test-programs e.g.:
#  ~/_commonCode/heartbeat/2.05/testCombined/
# in which case, CENTRAL_COMDIR should be ../../../../_commonCode
# (note the additional '..')

# So we'll assume this is a normal project, and can override CENTRAL_COMDIR
# in the project's makefile as necessary...
ifndef CENTRAL_COMDIR
CENTRAL_COMDIR = ../../../_commonCode
endif

################# SHOULD NOT CHANGE THIS BLOCK... FROM HERE ##############
#                                                                        #
# This stuff has to be done early-on (e.g. before other makefiles are    #
#   included..                                                           #
#                                                                        #
#                                                                        #
# LOCAL_COMDIR is the subdirectory of this project-directory where       #
#   _commonCode can be located for local usage.                          #
# (Unless you're me, this is most-likely where this project's            #
#  _commonCode is located)                                               #
LOCAL_COMDIR = ./_commonCode_localized
#                                                                        #
#                                                                        #
# If the file __use_Local_CommonCode.mk exists,                          #
#  then code will be compiled from the LOCAL_COMDIR                      #
# This could be slightly more sophisticated, but I want it to be         #
#  recognizeable in the main directory...                                #
# ONLY ONE of these two files (or neither) will exist, unless fiddled with
# AND SHOULD contain 'LOCAL=0' and 'LOCAL=1', respectively.              #
SHARED_MK = __use_Shared_CommonCode.mk
LOCAL_MK = __use_Local_CommonCode.mk
#                                                                        #
-include $(SHARED_MK)
-include $(LOCAL_MK)
#                                                                        #
#                                                                        #
#                                                                        #
#COMDIR:                                                                 #
# The *actual* *used* _commonCode directory is in the variable "COMDIR"  #
# COMDIR is automatically assigned the values from either                #
#  CENTRAL_COMDIR or LOCAL_COMDIR as appropriate.                        #
# COMDIR will be used in all references to commonCode, throughout the    # 
#  project-makefile                                                      #
ifeq ($(LOCAL), 1)
COMDIR = $(LOCAL_COMDIR)
else
COMDIR = $(CENTRAL_COMDIR)
endif
#                                                                        #
################# TO HERE ################################################



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
# * /home/meh/_avrProjects/sdramThingZero/21-gittifying/_commonCode_localized/heartbeat/2.06/testCombined/comdirEpoxyBlob.mk
# *
# *    (Wow, that's a lot longer than I'd hoped).
# *
# *    Enjoy!
# *    --Esot.Eric (aka EricWazhung)
# */
