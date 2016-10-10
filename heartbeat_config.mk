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
VER_HEARTBEAT = 2.06




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








####   WDT_DISABLE   ####
# The heartbeat can be responsible for resetting the AVR's Watch-Dog-Timer
# In fact, it's default.
# 
# But, it's probably best not to enable the WDT while testing/debugging, as
# it gets confusing when the system resets mysteriously; there're plenty of
# potential reasons for a reset (e.g. bad power, stack-overflow...)
# Or even more confusing when a simple program (blinking LED?) seems to be
# running fine for days on end, only to find out that it's been resetting
# every few seconds with no indication.
#
# The heartbeat's WDT code *used to* indicate if the reset was due to a WDT
# timeout, but it was a bit confusing, and didn't seem entirely reliable.
# Really, it's been unused for quite some time.
#
# Leave this TRUE unless you intend to do some debugging of my code.
WDT_DISABLE = TRUE

####
# ignore this block...
#
# This is not necessary, it just stores a viewable note in the FLASH
# to indicate that the WDT is disabled...
# PROJ_OPT_HEADER stuff hasn't really been used in a while
# and takes up memory-space.
# Further, this only works if WDT_DISABLE is defined, which it doesn't
# necessarily need to be, if the default case (enabled) is used...
#
# ignore this...
#
#PROJ_OPT_HDR += WDT_DIS=$(WDT_DISABLE)


####
# ignore this block, as well...
#
# It's possible to override the heartbeat's default 4sec choice for the WDT
# Timeout... (If, of course, the WDT is not disabled...)
# 
# Use 1sec instead.
#
# ignore this...
#
#sdramThing3.0 had this set... but also WDT_DISABLE, so this probably has
#  no function.
#CFLAGS += -D'_WDTO_USER_=WDTO_1S'



####   HEART_DMS / HEART_TCNTER   (AVRs only)   ####
# The heartbeat *can* use either tcnter or dmsTimer for precision-timing.
# With either of these set TRUE an entire fade-in/out cycle should take 8
# seconds, and each blink should take 1 second (when heartBlink is set).
#
# While these allow for precision-timing for the heartbeat, (it is helpful
# to know the cycle-rate to determine e.g. if the CLKDIV8 fuse is set when
# you weren't expecting it to be) it's mostly here for the convenience of 
# having them available, without having to explicitly include them, for 
# your own code. 
#
# DMS
# The dmsTimer is a deci-millisecond timer (.0001 second resolution) which
# uses a timer-interrupt at this rate. 
# At 32-bits this lasts for 4 days before overflowing. 
# At 16-bits, it lasts for ~6 seconds.
# The dmsTimer was developed prior to tcnter, and in most cases tcnter is a
# better choice.
#
# TCNTER
# The tcnter also uses a timer, but doesn't use interrupts. It requires a
# call to "tcnter_update()" at least once per timer cycle (before it
# overflows/resets).
# Benefits include:
#  * Higher resolution
#  * Doesn't require an interrupt
#  * Is used by several other timing-specific commonThings
#    (e.g. polled/bitbanged uart)
# As it stands, I'm not sure exactly what happens when it overflows, or
# when... but haven't run into any issues with it, yet.
#
# NEITHER:
# Without HEART_DMS or HEART_TCNTER set TRUE, the heartbeat runs at a rate
# that's *entirely dependent* on the rate at which heart_update() is called
# and is therefore unpredictable. Though, it does reduce code-size
# dramatically if neither the tcnter nor the dmsTimer are used by other
# code.
#
# CHOOSE ONE OR NEITHER:

#a/o sdramThing3.0-11, heartDMS has been removed
# There were issues with interrupts delaying sample-starting, and whatnot
HEART_TCNTER = TRUE
#HEART_DMS = TRUE







####   HEART PIN STUFF   (AVRs only)   ####
# In the vast-majority of cases, you'll want the heartbeat to drive an LED
# connected to a specific pin...

# HEARTPIN_HARDCODED
#
# Just leave this... There was a time I experimented with using variables
# to store pin-assignments... that's long-since fallen to the way-side, and
# probably doesn't even work anymore.
# (Using variables to store pin-assignments was slow and took a lot of
#  program-space. Besides all the accesses to the variables themselves, it
#  also required simple things like "setPin" to result in function-calls,
#  which ran tests (setPin->writePin(high)->if(low).../if(high)...)
#  But since the pin shouldn't change, and the wiring of the LED shouldn't
#  change, the optimizer can remove all that if the pin is hardcoded)
# Just leave this TRUE
CFLAGS += -D'HEARTPIN_HARDCODED=TRUE'

# HEART_PINNUM / HEART_PINPORT
#
# These define the pin
#   HEART_PINNUM refers to the pin name, e.g. PB4
#   HEART_PINPORT refers to the port name, e.g. PORTB
#
# GENERALLY: I connect the heartbeat to the same pin as the programming-
#  header's MISO. AVRs can drive 20mA or more, so choosing this pin has
#  several benefits.
#  * It means a prototype can be whipped-up *real quick* without even
#    having to wire up the heartbeat LED or pushbutton (they are already
#    wired-up on my programmer's dongle)
#  * MISO was chosen (over SCK or MOSI):
#    All other programming-header-pins are outputs from the programmer
#    whose buffers might not have the drive-strength to handle an LED...
#
# CAVEAT: As I begin to work more with SPI *peripherals* we run into the
#  exact-opposite problem (and several more). 
#  E.G. connecting the heartbeat to MISO, when the AVR is Master, and the 
#  Slave is, e.g., an SD-Card... The SD-Card likely hasn't the 
#  drive-strength for an LED (well, maybe a high-brightness LED with a
#  large resistor...? LEDs just weren't bright enough at ~1mA when I 
#  started this code). 
#  Regardless, sharing the heartbeat with an SPI device is possible, but
#  would require some modification.
# SO FAR: I've been lucky, the cases where I needed SPI, I was able to
#  use a different port (e.g. the attiny861 has a bit to specifically remap
#  SPI to another port, the atmega328p has the dedicated SPI port AND a
#  USART which can be put into SPI-mode.)
#
# IF LEFT UNSET (e.g. commented-out) heartbeat's code will attempt to use
# MISO by default. SO FAR THIS IS ONLY IMPLEMENTED ON THE atmega328p, so
# these MUST BE SET for other MCUs. (view atmega328p.mk)
# (This is now set in atmega8515 as well as attiny861)
#
# There is *no requirement* that heartbeat be on the MISO pin, you can
# choose any pin you like. 
#
#sdramThing3.0 uses the default (MISO) pin, so these can be left unset
#CFLAGS += -D'HEART_PINNUM=PB6'
#CFLAGS += -D'HEART_PINPORT=PORTB'


# HEART_LEDCONNECTION
#
# a/o v2.00, the definitions for the heartbeat's connection have changed.
# HEART_LEDCONNECTION=LED_TIED_HIGH is the only option that should be used.
#
# The heartbeat LED and pushbutton should be connected as follows: 
#
#   HEART_PINNUM (e.g. PB5) >-----+-----|<|---/\/\/\---> V+
#                                 |
#                                 O |
#                                   |-
#                                 O |
#                                 |
#                                 v
#                                GND
#
# This should be the case 99% of the time, as it also allows the heart-pin
# to be shared with the momentary-pushbutton input.
#
# (This *can* be overridden by changing *other* variables, see the code).
#
# (for the atmega328p, this is now default unless overridden)
# (a/o 2.02: I can't recall, is it default for *all* devices? Must be...)
CFLAGS += -D'HEART_LEDCONNECTION=LED_TIED_HIGH'


# HEART_PULLUP_DELAY
#
# (You can just ignore this)
#
# For the heartbeat pin being shared with a momentary pushbutton:
#
# heart_update() generally spends most of its time actually driving the
# LED. But, periodically it switches the pin to an input and reads the
# pushbutton. 
#
# There's one case where this can act weirdly: When the LED is On (the
# AVR's output is low), then it switches to an input with the pull-up
# resistor to test the button-value... Various capacitances can actually
# cause the pull-up resistor not to pull-up the value on the pin 
# fast-enough to read the actual button value (it will read the last value
# output to the LED). 
# This was a result of early-early code which literally switched the pin to
# an input, pulled-up, then sampled *immediately* (three CPU cycles). 
# So HEART_PULLUP_DELAY was a simple spin-loop that waited for the pull-up
# to overcome that capacitance, and a value of 255 seemed to be way more
# than enough in all cases.
#
# NOW: heart_update() has been turned into a state-machine which only
# advances one state each time it's called. 
#  E.G. call 0: heart_update() turns the pin into an input, pulled-up
#       calls 1 to HEART_PULLUP_DELAY: heart_update() doesn't do anything
#       call HEART_PULLUP_DELAY+1: heart_update() reads the pin-value
#
# So, originally in a spinloop, 255 was plenty... that'd be probably 512
#  cycles
# Now there's almost certainly 512 cycles between individual calls to 
# heart_update() (depending on what else you've got going on in your main()
#  loop) 
# So, realistically, a value of 1 or 2 should be plenty (maybe even 0). 
#
# Its maximum value is 255, (which is the default, if not entered here)
#
# 255 should be *way* higher than necessary, and should pretty much be
# guaranteed to work. Though, if heart_update() isn't called often-enough,
# it may result in noticeable flicker.
#
# If you feel so inclined to optimize things, just keep in mind that
# decreasing this value too low may cause heart_getButton() to return TRUE
# even if the pushbutton isn't activated.
# Further, keep in mind that the value is sensitive to CPU speed, the rate
# of calls to heart_update(), as well as external circuitry...
# (e.g. can an LED have a capacitance?)
CFLAGS += -D'HEART_PULLUP_DELAY=255'



#### A few other optimization options...
#
# These can be ignored.
#
# If not using the heartbeat as a button, this should save some code-space
#CFLAGS += -D'HEART_INPUTPOLLING_UNUSED=TRUE'
#
# If not using heart_getRate(), this should save some code-space
#CFLAGS += -D'HEART_GETRATE_UNUSED=TRUE'
#
#





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
# * /home/meh/_avrProjects/sdramThingZero/21-gittifying/heartbeat_config.mk
# *
# *    (Wow, that's a lot longer than I'd hoped).
# *
# *    Enjoy!
# *    --Esot.Eric (aka EricWazhung)
# */
