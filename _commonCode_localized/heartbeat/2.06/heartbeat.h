/* mehPL:
 *    This is Open Source-ish, but NOT GPL. I call it mehPL.
 *    I'm not too fond of long licenses at the top of the file.
 *    Please see the bottom.
 *    Enjoy!
 */


#warning "########     HEART_DMS is faulty!   ########"
#warning "######   testCombined doesn't include other commonThings!   ######"
//heartbeat 2.06-1
//
// TODO: REVISIT Switch (see 1.10-1/1.10)
// TODO: Revisit whether dms6sec_t is safe everywhere (especially blink)
//       Change Blink so upper nibble is first?
//       (currently blinks lower first, so 0x23 blinks 3 times, then 2)
// TODO: heartPinInputPoll should return 1 if HEART_DISABLED
//       since usually it's active-low...
// TODO: HEART_REMOVED's initHeartbeat doesn't take into account
// 		DMS_EXTERNALUPDATE
//
//TODO: From 2.05++NOTES.txt
//
//COMREL didn't work properly ->
//_pic32Projects/heartbeatTest2.05/1-from2.05_notTesting_testCombined
//
//heartbeatConfig (Desktop) could be combined, a bit... at least VER...?

//CENTRAL_COMDIR could be set for a test-program automatically...?
//...somehow automatically removed when not actually located in a cT...?

//heartNoPinStuff is ugly... how 'bout another name... a default
//and overridable? SEE NOTE IN 2.06-1, below.

//2.06-1: adding this TODO: heartNoPinStuff.h is ridiculous
//                          reimplement it like puar/puat:
//                          a local heartStuff.h EVEN FOR AVR
//    SEE: _pic32Projects/puartTest/2-puar 3_fixed_bithandlingError.zip
//    Possibly even later.
//2.06 Since 2.05's are now merged, might as well move up to 2.06
//     Also adding 2.05++NOTES.txt to TODOs, above...
//     Also TODONING re: reallyCommon3_testing.mk
//       (reallyCommon3.mk was identical?!)
//
//  Old Branch Note:
//   2.05_testing-1:
//       This is the result of some work done on the "testUser" account...
//       This has been merged (except for this note) with 2.05-6
//       I'mma rename 2.05-6 to 2.06 so there's no question
//
//2.05-6 Removing testCombined backup files (from 2.05-5)
//       Removing TODONES (also from 2.05-5)
//        TODO: Verify that 2.00's TODOS are TODONE and mark as such.
//2.05-5 merging differences from 2.05_testing/
//       Few notable changes...
//         TODONE: See 2.05-1  Re: reallyCommon3_testing.mk
//                              which was used in 2.05_testing/testCombined
//       Re-adding testCombined backups...
//         These were probably removed in 2.05 3_cleanup.zip
//         But re-added here for certainty
//           (They will be removed in the next version)
//    testCombined 2_priorToPROJ_OPT_HEADER+=WDT_DIS.zip
//    testCombined 1_errnoPathFix.zip
//    testCombined 0_COMRELremoval+rC3changes.zip
//
//       Numbering was off; forgot to make a few notes...
//
//  2.05 4_priorToDiffWith2.05testing.zip
//  2.05 3_cleanup.zip
//  2.05 2_priorToCleanupForPic32_NotUsing_2.05-testing_forgotWhatItWas.zip
//  2.05 1_priorToTestCombinedUpload.zip
//  2.05 0_DMS=NOGO.zip
//
//
//2.05-1 a/o PIC32 (immediately after xc32-simplifying)
//       Attempting to port this to the PIC32...
//       Probably will use the desktop application
//        (a/o 2.05-6: Yep, used testDesktop...)
//       2.05_testing exists, now, but for some reason I'm not using that
//       right now... That includes reallyCommon3_testing.mk which is also
//       not yet being used...
//       TODONE: SEE above 
//
//   Somewhere previously:
//      testCombined 0_COMRELremoval+rC3changes.zip
//      testCombined 1_errnoPathFix.zip
//      testCombined 2_priorToPROJ_OPT_HEADER+=WDT_DIS.zip
//
//
//
//2.05 Cleaning that up a bit, there're too many files.
//
//2.02 Looking into making a single test program that's easily switchable
//     via a single config option
//2.01-2 created
//       adding HEART_NOPIN, and heartNoPinStuff.h
//2.01-1 attempting to create a desktop-based test-program
//2.01 adjusting to dmsTimer 1.16 (from 1.14!) and tcnter 0.31 for
//     timerCommon 1.23 for gitHubbing polled_uar
//2.00-5 GO!
//       forgot Return False
//2.00-4 Various major changes...
//       Mainly, heart_update now handles getting the button input value
//       And doesn't work.
//2.00-3 Continuing to revisit wiring notes...
//       It's gotten much too confusing, there are too many variables
//       Instead, replace the old "HEART_LEDCONNECTION" with:
//         LED_ON_PINSTATE  (H, L, Z, PU)
//         LED_OFF_PINSTATE (H, L, Z, PU)
//         BUTTON_ON_PINVAL (H, L)
//         (BUTTON_OFF_PINVAL == implied opposite, NOT USED)
//         BUTTON_INPUT_PINSTATE (Z, PU)
//       Pinstates are now defined in bithandling as:
//         PINSTATE_LOW/HIGH/Z/PU
//
//       HEART_LEDCONNECTION == LED_TIED_HIGH is still valid, 
//                                            and defines the following:
//         LED_ON_PINSTATE         PU
//         LED_OFF_PINSTATE        L
//         BUTTON_ON_PINVAL        L
//         BUTTON_INPUT_PINSTATE   PU
//
//       THIS IS FOR the standard connection:
//
//       Heartbeat Pin ><-----+---|<|---/\/\/\----> V+
//                            |    
//                            |   _|_
//                            '---o o--->GND
//      -----
//       HEART_LEDCONNECTION == LED_INVERTED is also still valid, 
//                                           and defines the following:
//         LED_ON_PINSTATE         H
//         LED_OFF_PINSTATE        L
//         BUTTON_ON_PINVAL        L
//         BUTTON_INPUT_PINSTATE   PU
//
//       AS I RECALL (unverified):
//        This is for a switch/jumper rather than a momentary-pushbutton:
//
//       Heartbeat Pin ><---+---|>o-----|<|---/\/\/\----> V+
//                          |    
//                          |    /     e.g. 1k
//                          '---o  o---/\/\/\--->GND

//      
//
//2.00-2 Toward the new heart_getButton() functionality ideas...
//       First, let's centralize these various connection-methods.
//       REMOVING schematics/tables from version-notes and moving/merging
//       them with heartI_setLED() notes in heartbeat.c
//          From 1.10 and 1.00-4
//
//2.00-1 Cleanup... heartTimer_t now used for all timer-cases
//       (DMS, TCNTER, or even when neither is used)
//       thus, now, heart_isItTime() can be used in all cases, as well.
//       also heartI_setLED()
//
//2.00   So far, all that's been done is heart2replacement.sh
//       renaming most functions to be more normal.

//2.00 TODO:
//          (a/o 2.05-6: aren't these all TODONES?)
//    a/o audioThing56:
//       heartPinInputPoll takes quite a bit of time, due to waiting for
//       pull-ups, etc.
//       Also, it can't be run after *every* heart_update (e.g. can't be
//       tested in every main()->while() loop) otherwise it would override
//       the heart's value.
//       Let's take a new approach:
//        Instead of two separate functions, maybe it can be made part of
//        heart_update, in which case, heart_getButton() would do nothing
//        more than read a variable that was already written in a previous
//        heart_update()
//
//       Other Considerations:
//        I hate the naming-"convention" for these functions
//          they should be heart_update(), heart_pinInputPoll(), etc.
//        TODone
//
//        heart_update() might be made more state-machine-ish, thus
//          requiring less processing time in each call...
//







//1.50-14 audioThing50 won't work with the programming-header's default
//        heartbeat pin, but I would like to use it for early testing
//        Decided on a new standard for _make/<device>.mk:
//
//        PGM_MISO_PIN_NAME/PORT_NAME will be used unless the normal-style
//        naming has been defined in the project's makefile
//        
//1.50-13 LCDdirectLVDS77 complains that getHeartRate "reaches end of
//			 non-void function" when using TCNTER... but not with DMS?
//        I don't see it, and test+...TCNTER doesn't either. Weird.
//        TCNTER_DMS undeclared -> HEARTSTEPTIME... almost there.
//1.50-12 testing TCNTER in PWM161...
//1.50-11 testing DMS/TCNTER/LoopCounter, and fixing
//1.50-10 Cleanup, TCNTER_MS/DMS in tcnter.h now, etc...
//1.50-9 Looking into setting some defaults for tcnter
//       e.g. to include timerCommon, and init.
//1.50-8 first go with tcnter, seems to work. Reduced codesize from 1.5k to
//			1.4k... no optimization of timerCommon's unused functions
//       And, more importantly, it doesn't require a timer interrupt.
//1.50-7 getting rid of the -0, this is NOT directly from 1.50dne.
//       adding HEART_USES_TIMER...
//       backup file-numbers are off... should be correct now.

//1.50-0-5 Continuing with the 1.50-0 branch, despite 1.50dne being fixed
//
//   1.50dne-1: Fixed... Very strange: if(1,0) actually compiles.
//              in blinkHeart() -> if(dmsIsItTime(a,b),c)
//
//         ALSO: Looking into other uses of dmsIsItTime
//         		  More cleanup of blinkHeart()
//               testing with and without DMS...
//               DMS: 1520Bytes
//               NO-DMS: 850Bytes

//1.50-0-4 blinkHeart(void) and inline, for DMS
//         decreased codesize dramatically, now smaller than before
//         dmsIsItTime... Might be even smaller without dmsIsItTime, or
//         maybe it optimized that in and that's why it's smaller.
//         Who knows.
//1.50-0-3 HEART_PRECISE
//1.50-0-2 stepping through 1.50dnw
//         First: dmsIsItTime6secV2 in blinkHeart...
//1.50-0-1 revisiting 1.50(-0), switching to dmsTimer1.14
//1.50-1 DID NOT WORK (renamed 1.50dnw)
//1.50   1.30-7 didn't really amount to anything, but if this ends up
//       working, then it deserves a major revision change.
//1.30-7 Looking into thoughts on maybe using TCNTER for heart, instead of
//       dms... (a/o audioThing40-11)
//1.30-6 HEART_REMOVED causes "statement with no effect: (0)"
//       easy fix, but haven't necessarily thought of *every* case...
//       see cTools/unusedMacroTest.c
//1.30-5 HEART_PULLUP_DELAY can be overridden
//1.30-4 sdramThing3.0-3 was still using heart1.21! Weird...

//1.30-3 in HEART_REMOVED, should still init_dmsTimer() on init... if
//			HEART_DMS...
//       TODO: heartPinInputPoll should still be functional...
//       TODO: should probably also setoutPORT() for toggling, etc...?
//             Or make a *really simple* blinking heart...?
//1.30-2 Also testDMS
//1.30-1 a/o sdramThing3.0-0.7 and delayCyc0.10ncf
//       creating a test-program for delayCyc, started with heartbeat, so
//       might as well put it here.
//1.30   Using HFM1.00
//       Heartbeat is HUGE!
//       a/o LCDRevisited2012-28: Overflows by 138 bytes after HFM1.00
//         HEART_REMOVED -> 7574 Bytes... that's 756 bytes for heartbeat!
//       Moving WDT code grouped-together in init...
//        from 138 bytes overflowed to 82... nice
//       Adding HEART_BLINK_UNUSED
//1.21-1 Enabling HEART_REMOVED as TRUE causes the code not to be added to
//       SRC, which is the idea. BUT, it causes it to not be included in
//       make localize... here's a hack to fix that... using COM_HEADERS
//1.21-7 _BITHANDLING_HEADER_ wasn't used!
//1.21-6 adding this TODO: create a halt-function for errors, with blinks?
//			see audioThing-v4
//       What about a timer-interrupt-function for heartUpdates until
//       the system's booted...?
//        (maybe just for debugging... a CFLAG?)

//1.21-5 Input Polling Delay for pull-up...
//1.21-4 HeartBlink only runs once (when No DMS?) (Fixed)
//1.21-3 Shouldn't HFModulation be included in the ifndef? Maybe not.
//#warning "UNTESTED change re: HFModulation versioning"
//       NOGO, reverted, with notes in .mk
//1.21-2 HFModulation version overridable
//1.21-1 remove TCOMDIR in favor of COMDIR (forgot to back up 1.21)
//1.21 updating to latest dmsTimer... 
//1.20-1 Adding __HEART_REMOVED__:
//     In Makefile:
//      HEART_REMOVED = TRUE
//        will not add heartbeat.c to SRC and will add:
//        CFLAGS += -D'__HEART_REMOVED__=TRUE'
//      removes all heart-code quickly for code-size-testing...
//1.20 Adding HEARTPIN and HEARTPINPIN options
//     so compilation doesn't make them variables...
//		 In Makefile:
//			CFLAGS += -D'HEARTPIN_HARDCODED=TRUE'
//			CFLAGS += -D'HEART_PINNUM=PA3'
//			CFLAGS += -D'HEART_PINPORT=PORTA'
//			CFLAGS += -D'HEART_LEDCONNECTION=LED_TIED_HIGH'
//     This is a significant improvement...
//       threePinIDer51i (with 1.12-5) was 81000cyc/5sec, 7896B
//       threePinIDer51ib (with 1.20) is 103000cyc/5sec, 7650B
//       and four bytes freed from .bss
//1.12-5 HEART_DMS=FALSE doesn't seem to clear all references...
//1.12-4 attemping to save some codespace for threePinIDer49i
//       adding _UNUSED conditional compilation...
//1.12-3 wow, dmsTimer.h was hardcoded to v1.00! switching to 1.10
//1.12-2 switching to dmsTimer 1.10 (from 1.00)
//1.12-1 adding wdt_disable at start of heart_init
//1.12 adding error-blinking... I thought I had this before!
//1.10-1 #include "_HFMODULATION_HEADER_" notes... 
//       LED/Input notes re: LED tied to GND added to 1.10:
//       NOT CERTAIN CODE HANDLES ALL CASES PROPERLY
//1.10 LED/Input switching revisited for CS71518/9 (with inverter) 
//           --24Scam16
//   0.99.5 worked with CS71518/9 for PS24-6
//     revision to generalize screwed it up (musta been 1.00-4)
//     *NOTE: These schematics/notes have been moved a/o 2.00-2
//            they've been merged with others from various places
//            and are now in heartbeat.c:heartI_setLED()
//          
//1.00-10 removing obnoxious warning... 
//1.00-9 revising dmsUpdate's functionPointer for clarity/portability...
//		(1.00-8 not archived, see krautPoxn 117 tarball...)
//1.00-8 using dms6sec_t...
//1.00-7 HFModulation .93 (for adjustable divisor)
//1.00-6 adding _HEARTBEAT_HEADER_ to makefile
//1.00-5 bithandling .94
//1.00-4 LED / INPUT switching...
//		Input is only usable when pulled LOW, do not pull it high... 
//			(maybe different if the LED is tied low, but code would probably 
//        have to change)
//		This is only useful if the input usage is temporary since it forces 
//    the LED state
//			i.e. money-trigger, start-button, configuration-button, etc...
//		This will also work with non-temporary input if an inverter 
//    AND resistors are used
//			i.e. configuration DIP switch
//		Without inverter, LED tied high *
//		With inverter, LED tied high, no resistors *
//    * NOTE: These tables have been moved to heartbeat.c:heartI_setLED()
//      a/o v2.00-2
//1.00-3 adding dmsWait with heartUpdate...
//1.00-2 adding getHeartRate
//1.00-1 adding Brown-Out-Reset indication...
//1.00 adding watchdog timer... dms cleanup/update
//.99.6 latest dmsTimer, with DMS_INCLUDED, etc...
//.99.5 heartSharing (switch to an input and poll)
//.99.4 heartRate added, more DMS define fixes...
//.99.3 HEARTDMS fixes... added __HEARTBEAT_H__ definition
//.99.2-1 added __HEART_DMSNOINIT__
//.99.2 makefile changes...?
//.99.1 makefile added, made compatible with latest dmsTimer, bithandling. 
//      Changed HEART_DMS define, and made it configurable via project's 
//      makefile
//.99cf moved to _common from 24scam5
#if (__TODO_WARN__)
#warning ".99cf may actually be older in some ways than 0.97zcf"
#endif
//.99 setClear
//.98 /actual/ variable-based pin..
//.97 heartStepTime variable
//.96 using hfModulation
//.95 Heart Smoothing via adjustable-frequency PWM, completely different 
//    algorithm... needs cleanup
//.91b	never so-titled, but updated for uint8_t output from heartUpdate
//.91 added HEART2X for heart rate doubling
//.90 added heart for heart clearing

#ifndef __HEARTBEAT_H__
#define __HEARTBEAT_H__

#ifdef __AVR_ARCH__
#include <avr/io.h>
#include <avr/interrupt.h>	//for sei and cli, making the heartcount update
                           //atomic (could do this other ways...?)
#include <avr/wdt.h>
#endif

#ifndef _BITHANDLING_HEADER_
 #error "Not gonna letcha get away with this... _BITHANDLING_HEADER_ is not defined"
 #include "../../bithandling/0.95/bithandling.h"
#else
 #include _BITHANDLING_HEADER_
#endif
//#include "../../hfModulation/0.92.2/hfModulation.h"

//Use of _HFMODULATION_HEADER_ seems dangerous...
//   HFMODULATION_LIB = $(TCOMDIR)/hfModulation/0.93/hfModulation
//   CFLAGS+= -D'_HFMODULATION_HEADER_ = "$(HFMODULATION_LIB).h"'
//       TCOMDIR = ../../_common WOULD screw it up, 
//               IF it were referenced from here...
//       because it's done in CFLAGS, it's referenced from the compiling 
//               directory (not from here)
//       (Frankly, it's just luck it worked out this way... 
//               I don't think I was paying /that/ close attention while 
//               writing this)
//     So then, why did I come to looking into this...?
//       trying to compile an application outside the Trimpin folder... 
//         'make clean' reported errors here, likely due to
//             TCOMDIR = ~/Trimpin/_common
//             In file included from /Users/meh/Trimpin/_common/heartbeat/
//                   1.00/heartbeat.c:2:
//             /Users/meh/Trimpin/_common/heartbeat/1.00/heartbeat.h:57:31:
//                   error: ~/Trimpin/_common/hfModulation/0.93/
//                   hfModulation.h: No such file or directory
//          due to the use of "~" it seems. 
//          (TCOMDIR=/Users/meh/Trimpin/_common works)
//          
#include _HFMODULATION_HEADER_

//#define _HEART_DMS_

#ifndef _HEART_DMS_
#error "_HEART_DMS_ not defined. Make sure your project makefile has 'CFLAGS +=' everywhere..."
#endif

#if (_HEART_DMS_)
//#include "../../dmsTimer/1.00/dmsTimer.h"
#include _DMSTIMER_HEADER_


//Note that this is NOT calling int heart_update() but 
//                                                 void heart_voidUpdate()
#if defined(dmsWait)
 #undef dmsWait
 #define dmsWait(time)	dmsWaitFn((time), &heart_voidUpdate)
#endif

#endif

#if (defined(_HEART_TCNTER_) && _HEART_TCNTER_)
 #include _TCNTER_HEADER_


 //If using dmsTimer, this is the time in dms between each step...
 // else this is the number of heart_updates between each step
 extern tcnter_t heartStepTime;
#else
 extern uint16_t heartStepTime;
#endif



//For init: See 1.10 notes...
//These two are the originals... for backwards compatibility 
//  (heh, all of a half a version?)
#define LED_DIRECT_HIGH		FALSE	//(Currently an output L or an input, 
//                                                               pulled up)
#define LED_INVERTED		TRUE	//(Currently always an output, H/L)

//These make a little more sense
#define LED_TIED_HIGH		LED_DIRECT_HIGH
#define LED_TIED_LOW		LED_INVERTED	//Tied Low works with Inverted's 
                                    //functionality (always an output, H/L)
#define LED_BUFFERED		LED_INVERTED

//Amount of time between each level of brightness in deci-milliseconds
//There are 512 steps per bright/dim cycle...(?)
//7.8125ms = 128 steps in 1 second, whole bright/dim sequence in 4
//No real reason to make this externed... initialized in declaration in 
// heartbeat.c
//78*2 = 128 steps in 2 seconds, whole sequence in 8
#if (_HEART_DMS_)
 #define HEARTSTEPTIME	(78*2)	
#elif (_HEART_TCNTER_)
 #define HEARTSTEPTIME	(78*2*TCNTER_DMS)
#else
 //Number of loops before stepping to the next brightness level... 
 //(only if NODMS)
// #define HEART_ITERATIONCOUNT	400
 #define HEARTSTEPTIME	400
#endif

//#define HEART_MAXVAL 0xff
//#define HEART_MAXBRIGHT	(HEART_MAXVAL*2/3)
//#define HEART_MAXBRIGHT	(0xff)
#define HEART_MAXBRIGHT	(0xff)

/* testing of hfm with divisor
//  heart getNextDesired doesn't work well with different divisors
//  if MAXBRIGHT != maxPower... since power is read from the hfm_t in 
//                                                                 getNext
#define HEARTSTEPTIME	(78*256)	
#define HEART_MAXBRIGHT	(0x4)
*/
//Is this the number of iterations per 
//#define HEARTCOUNTMAX	((uint32_t)0x20000l) 
//#define HEARTNUMSTEPS	0x80l		//Number of PWM value steps between 
//                                 full-off and full-on


//#define HEARTNUMCOUNTSPERSTEP	(HEARTCOUNTMAX/2/HEARTNUMSTEPS)
//#define HEARTNUMCOUNTSPERPWMDIVISION	\
                  (HEARTNUMCOUNTSPERSTEP/HEARTNUMSTEPS)	

/*
//LATER MAYBE
//heartSet can be set to any of these values from external functions and 
// will be updated in the next heart_update()
//Requires fewer instructions from external functions...
#define HEARTDONTSET	0xff
#define HEART2X			0xf0	//beat twice as fast
#define HEARTRESET		0
void heartMode(uint8_t mode);
*/

//#define HEARTFULLOFF	0
//#define HEARTHALFBRIGHT	1
//#define HEARTFULLON		2
//#define HEARTHALFDIM	3
//#define HEARTJERK		0xf0

//heartSet can be set to 0-4 to set the heart to either full
//extern volatile uint8_t	heartSet;


#if (!defined(HEARTPIN_HARDCODED) || !HEARTPIN_HARDCODED)
//e.g. PINA
extern volatile uint8_t* heartPIN;
//extern volatile uint8_t* heartPORT;
//e.g. PA7
extern uint8_t heartBeatPin;

//Takes arguments in the form... (&(DDRD)) (see HEART_INIT() below)
//MUST BE CALLED WITHIN THE FIRST 15ms of boot so WDT won't get stuck in a
//  reset-loop!!!
// HEART UPDATE MUST ALSO BE CALLED BEFORE WDT runs out!!!!
 #if (!defined(__HEART_REMOVED__) || !__HEART_REMOVED__)
void heart_init(volatile uint8_t* pin, uint8_t pinName, 
							uint8_t ledConnectionType);
 #else	//HEART_REMOVED
  #if(defined(_HEART_DMS_) && _HEART_DMS_)
	#define heart_init(a,b,c) init_dmsTimer()
  #else
	#define heart_init(a, b, c) //(0)
  #endif
 #endif

//HEART_INIT(HEARTPIN,HEARTBEAT);
//MUST BE CALLED WITHIN THE FIRST 15ms of boot 
// so WDT won't get stuck in a reset-loop!!!
#define HEART_INIT(pin,pinName,ledConnType) \
	heart_init((&(pin)),(pinName),(ledConnType))

#else //HEARTPIN_HARDCODED

//The normal naming-scheme for the heart Pin/Port is (from the project
//makefile):
//CFLAGS += -D'HEART_PINNUM=PB1'
//CFLAGS += -D'HEART_PINPORT=PORTB'
//CFLAGS += -D'HEART_LEDCONNECTION=LED_TIED_HIGH'
// If these are not defined, then use the programming-header
// THIS IS NEW a/o audioThing50 via ATmega328P
// Other devices do not have PGM_xxx_yyy_NAME defined
// So, I suppose, no handling's really needed to make this
// backwards-compatible with old-style project makefiles. 
// But, forward-compatibility may be an issue if the project makefile
// doesn't include these, AND PGM_... isn't defined in the chip's makefile
// But I think that'll just cause an "undefined" error, which is exactly
// what I'd want... and would've been the same, had I forgotten to define
// HEART_PINNUM in the project's makefile, prior to this update.
// The device makefile has PGM_MISO_PIN_NAME
// and avrCommon.mk converts these to _PGM_MISO_PIN_NAME_ for use in code
#ifndef HEART_PINNUM
 #define HEART_PINNUM _PGM_MISO_PIN_NAME_
#endif
#ifndef HEART_PINPORT
 #define HEART_PINPORT _PGM_MISO_PORT_NAME_
#endif
#ifndef HEART_LEDCONNECTION
 #define HEART_LEDCONNECTION LED_TIED_HIGH
#endif


#if (!defined(__HEART_REMOVED__) || !__HEART_REMOVED__)
void heart_init(void);
#else //HEART_REMOVED
 #if(defined(_HEART_DMS_) && _HEART_DMS_)
	#define heart_init() init_dmsTimer()
 #else
	#define heart_init() //(0)
 #endif
#endif
#endif

#if (!defined(__HEART_REMOVED__) || !__HEART_REMOVED__)
void heart_clear(void);
#else	//HEART_REMOVED
#define heart_clear() //(0)
#endif

//!!!
// IF this is called from anywhere besides internal to heatbeat.c/h
// WDT indication will NOT function
#if (!defined(__HEART_REMOVED__) || !__HEART_REMOVED__)
void heart_setRate(uint8_t rate);
#else //HEART_REMOVED
#define heart_setRate(a) //(0)
#endif

#if (!defined(HEART_GETRATE_UNUSED) || !HEART_GETRATE_UNUSED)
//Get the current heart rate (especially useful for keeping error 
//  indication...)
#if (!defined(__HEART_REMOVED__) || !__HEART_REMOVED__)
uint8_t heart_getRate(void);
#else	//HEART_REMOVED
#define heart_getRate() ZERO8_FN() //(0)
#endif
#endif


#if (!defined(HEART_BLINK_UNUSED) || !HEART_BLINK_UNUSED)
//If this is set to 0, the heart fades as usual
// if non-zero the heart blinks as follows:
//  say value=0xXY, it will blink Y times, then X times, then repeat
// (for error indication)
#if (!defined(__HEART_REMOVED__) || !__HEART_REMOVED__)
void heart_blink(uint8_t value);
#else	//HEART_REMOVED
#define heart_blink(a) //(0)
#endif
#endif

//OLD:Returns TRUE if the LED is active, FALSE if the LED is inactive...
//Returns TRUE when the pin is an input and pulled-up. 
//  LED active-level depends on wiring/inverter
//a/o v2.00: This is all hokey...
// The actual return-value is the state of the LED, or FALSE if sampling
// the input.
#if (!defined(__HEART_REMOVED__) || !__HEART_REMOVED__)
uint8_t heart_update(void);
#else	//HEART_REMOVED
#define heart_update() ZERO8_FN() //({0;})
#endif

#if (!defined(DMS_WAITFN_UNUSED) || !DMS_WAITFN_UNUSED)
//NoReturn for dmsWait...
#if (!defined(__HEART_REMOVED__) || !__HEART_REMOVED__)
void heart_voidUpdate(void);
#else	//HEART_REMOVED
#define heart_voidUpdate() //(0)
#endif
#endif

//Poll the heartbeat pin an input...
//Returns the actual value of the pin
//!!! This should NOT be called in a typical while... updateEverything 
//    loop otherwise it will constantly interfere with the heartbeat
//  Ideally, this would be called once every... 256? heart_updates to 
//    minimize visual distortion
//	 Maybe during getNextDesired?
//Meh, the actual input poll takes all of... 25ish instructions, the loop 
//  itself probably takes significantly longer
//This could be more sophisticated to be tied together with heart being an
//  input...
// Except that testing would be slowed due to FULLBRIGHT...
#if (!defined(HEART_INPUTPOLLING_UNUSED) || !HEART_INPUTPOLLING_UNUSED)
#if (!defined(__HEART_REMOVED__) || !__HEART_REMOVED__)
uint8_t heart_getButton(void);
#else	//HEART_REMOVED
#define heart_getButton() ZERO8_FN()
#endif
#endif

#if (!defined(__HEART_REMOVED__) || !__HEART_REMOVED__)
//This is internal-only...
//uint8_t getNextDesired(void);
//And this doesn't seem to exist...
//uint8_t updateOutput(void);
#else //HEART_REMOVED
//This shouldn't be necessary if HEART_REMOVED...
//#define getNextDesired() (0)

//#define updateOutput() (0)
#endif

#if defined(MCUSR)
 #define MCUStatReg MCUSR
#elif defined(MCUCSR)
//8515...
 #define MCUStatReg MCUCSR
#endif


#endif
/* mehPL:
 *    I would love to believe in a world where licensing shouldn't be
 *    necessary; where people would respect others' work and wishes, 
 *    and give credit where it's due. 
 *    A world where those who find people's work useful would at least 
 *    send positive vibes--if not an email.
 *    A world where we wouldn't have to think about the potential
 *    legal-loopholes that others may take advantage of.
 *
 *    Until that world exists:
 *
 *    This software and associated hardware design is free to use,
 *    modify, and even redistribute, etc. with only a few exceptions
 *    I've thought-up as-yet (this list may be appended-to, hopefully it
 *    doesn't have to be):
 * 
 *    1) Please do not change/remove this licensing info.
 *       (You can add notes *surrounding* it!)
 *    2) Please do not change/remove others' credit/licensing/copyright 
 *         info, where noted. 
 *    3) If you find yourself profiting from my work, or derivatives,
 *         thereof, please send me a beer, a trinket, or cash is always 
 *         handy as well.
 *         (Please be considerate. E.G. if you've reposted my work on a
 *          revenue-making (ad-based) website, please think of the
 *          years and years of hard work that went into this!)
 *    4) If you/your company *intend(s)* to profit from my work, 
 *         you must get my permission, first. 
 *    5) No permission is given for my work to be used in Military, NSA,
 *         or other creepy-ass purposes. No exceptions. And if there's 
 *         any question in your mind as to whether your project qualifies
 *         under this category, you must get my explicit permission.
 *
 *    The open-sourced project this originated from is ~98% the work of
 *    Me, "Esot.Eric," except where otherwise noted.
 *    That includes the "commonCode" and makefiles.
 *    Thanks, of course, should be given to those who worked on the tools
 *    I've used: avr-dude, avr-gcc, gnu-make, vim, usb-tiny, and 
 *    I'm certain many others. 
 *    And, as well, to the countless coders who've taken time to post
 *    solutions to issues I couldn't solve, all over the internets.
 *
 *
 *    I'd love to hear of how this is being used, suggestions for
 *    improvements, etc!
 *         
 *    The creator of the original code and original hardware can be
 *    contacted at:
 *
 *        EricWazHung At Gmail Dotcom ("Me")
 *
 *    This code's origin (and latest versions) can be found at:
 *          https://github.com/ericwazhung
 *      or: https://github.com/esot-eric-test
 *      or Plausibly under a project page at: 
 *          https://hackaday.io/hacker/40107-esoteric
 *
 *    Older versions may be locatable at: (no longer updated)
 *        https://code.google.com/u/ericwazhung/
 *
 *
 *    The site associated with the original open-sourced project is at:
 *
 *        https://sites.google.com/site/geekattempts/
 *
 *    If any of that ever changes, I will be sure to note it here, 
 *    and add a link at the pages above.
 *
 * This license added to the original file located at:
 * /home/meh/_avrProjects/sdramThingZero/21-gittifying/_commonCode_localized/heartbeat/2.06/heartbeat.h
 *
 *    (Wow, that's a lot longer than I'd hoped).
 *
 *    Enjoy!
 *    --Esot.Eric (aka EricWazhung)
 */
