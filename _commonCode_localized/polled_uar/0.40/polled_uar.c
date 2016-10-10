/* mehPL:
 *    This is Open Source-ish, but NOT GPL. I call it mehPL.
 *    I'm not too fond of long licenses at the top of the file.
 *    Please see the bottom.
 *    Enjoy!
 */


//polled_uar 0.30
//#ifdef _POLLED_UAR_HEADER_
#include _POLLED_UAR_HEADER_
//#include _TCNTER_HEADER_
//#else
// #include "polled_uar.h"
//#endif

//This file needs to be located in the project's include path...
// e.g. in the main folder
// It's project-specific for readInput()
#include "puarStuff.h"



#if (defined(PU_PC_DEBUG) && PU_PC_DEBUG)
	#define DPRINT	printf
	//This is a hack... not sure what to do about it yet
//	extern int	TCNT0;
//	extern int	getbit(a);
#else
	#define DPRINT(...) {}
#endif

//********** POLLED UART Defines **********//

#define DATA_BITS                 8
#define START_BIT                 1
#define STOP_BIT                  1

//#define TIMER0_SEED (256 - ( (SYSTEM_CLOCK / BAUDRATE) / TIMER_PRESCALER ))


//#define BIT_TCNT	 //THIS NEEDS TO BE CALCULATED!!!
//#define HALF_TCNT (BIT_TCNT/2)

//SEE ps2_touchPad3 notes regarding globals and linking and shit.

uint8_t puar_rxData[NUMPUARS];
uint8_t puar_rxDataWaiting[NUMPUARS];//=FALSE;

//Inlining saved 12B (one call?)
PUAR_INLINEABLE
uint8_t puar_dataWaiting(uint8_t puarNum)
{
	return puar_rxDataWaiting[puarNum];
}

//Must be called AFTER testing if data's waiting
// otherwise it will return garbage
//Inlining saved 20B (one call?)
PUAR_INLINEABLE
uint8_t puar_getByte(uint8_t puarNum)
{
	puar_rxDataWaiting[puarNum] = FALSE;
	return puar_rxData[puarNum];
}

uint8_t puar_rxState[NUMPUARS];
#define RXSTATE_AWAITING_START	0	//Waiting for the edge
#define RXSTATE_EXPECTING_START  1	//Waiting to sample the start bit
#define RXSTATE_FIRST_BIT			2  //Waiting to sample the first bit
// Bits are received here
#define RXSTATE_EXPECTING_STOP	(RXSTATE_FIRST_BIT + DATA_BITS)



//#warning "Changing TCNT for this purpose makes the timer unusable for more than one UAR/T or other device at a time!"
// What about a tcnt DIFFERENCE, instead?
// How difficult would this be with wraparound and/or resets at arbitary
// values?
// Can we use the other devices' (e.g. DMS) OCR values?
// What about an xyTracker for more precision and less cumulative error?

// Wait for a 'U' to be sent, to determine BIT_TCNT
//myTcnter_t puar_bitTcnt[NUMPUARS];

#ifndef BIT_TCNT
myTcnter_t puar_bitTcnt[NUMPUARS];

#if 0 //auto calibrating doesn't work...
//    ?G?G?V
//    vvvvvv
// ___             ___             ___
//    \/\/\___/\/\/   \/\/\___/\/\/   \/\/\___
//    ^    ^  ^
//    \____|__/<-- Bit Duration
//    .    |  .
//    .    Verified
//    .    ^  .
// ..______/\______/\______/\____
//   0.      1.      2       3		detectFilteredEdge call #
//    ^       ^
//   ST0     ST1

#define BIT_TCNT_MIN	2//5//12
//Internal for calibrateBitTcnt()
PUAR_INLINEABLE
myTcnter_t detectFilteredEdge(uint8_t puarNum)
{
	uint8_t checkingEdge = FALSE;
	uint8_t startLevel = puar_readInput(puarNum);
	myTcnter_t thisEdgeStartTime = 0;
	myTcnter_t firstEdgeStartTime = 0;

	//tcnter_update();
	//startTime = tcnter_get();


	while(1)	//continue until break
	{
		tcnter_update();

		if(puar_readInput(puarNum) != startLevel)
		{
			if(checkingEdge)
			{
				//the level has been stable for enough time...
				if(tcnter_get() - thisEdgeStartTime > BIT_TCNT_MIN)
					break;
			}
			else	//Not Yet Checking Edge, start doing-so
			{
				checkingEdge = TRUE;
				thisEdgeStartTime = tcnter_get();

				if(firstEdgeStartTime == 0)
					firstEdgeStartTime = tcnter_get();
			}

		}
		else
		{
			//[Back] at the start-level... reset the edge-timer
			checkingEdge = FALSE;
			thisEdgeStartTime = 0;
		}

	}

	return firstEdgeStartTime;

}

//On the plus side, by inlining both this and the above,
// if this isn't called, it doesn't take *any* code-space (?!)
// finally this inlining thing seems to work as expected...
PUAR_INLINEABLE
myTcnter_t puar_calibrateBitTcnt(uint8_t puarNum)
{
	myTcnter_t bitStartTcnt[9];
	uint8_t bitNum = 0;


	for(bitNum = 0; bitNum < 4; bitNum++)
	{
		bitStartTcnt[bitNum] = detectFilteredEdge(puarNum);
	}


	//myTcnter_t totalTcnt = bitStartTcnt[8] - bitStartTcnt[0];

	//puar_bitTcnt[puarNum] = totalTcnt / 9;

	puar_bitTcnt[puarNum] = bitStartTcnt[2] - bitStartTcnt[1];
	//So the same bit-rate can be used for Tx, etc.
	return puar_bitTcnt[puarNum];
}
#endif //FALSE (calibrated stuff doesn't work)

//Inlining saved 26 bytes (one call?!)
PUAR_INLINEABLE
void puar_setBitTcnt(uint8_t puarNum, myTcnter_t tcnt)
{
	puar_bitTcnt[puarNum] = tcnt;
}
#endif

//Inlining saved 2 Bytes !!!
PUAR_INLINEABLE
void puar_init(uint8_t puarNum)
{
#if(!defined(TCNTER_STUFF_IN_MAIN) || !TCNTER_STUFF_IN_MAIN)
	tcnter_init();
#endif

//#ifdef BIT_TCNT
//	puar_bitTcnt[puarNum] = BIT_TCNT;
//#endif

	DPRINT("puar_init(%d)\n", puarNum);
}

// Inline Functions apparently aren't supposed to have static variables...
// Hopefully it can deal with globals!
#if (defined(_PUAR_INLINE_) && _PUAR_INLINE_)
 uint8_t puar_thisByte[NUMPUARS]; // = 0x00;
// #ifdef BIT_TCNT
  tcnter8_t puar_lastTime[NUMPUARS];
// #else
//  myTcnter_t puar_nextTcnter[NUMPUARS]; //When the next action should
// #endif                                    // occur wrt: myTcnter
#endif



//Inlining saved 172 Bytes !
PUAR_INLINEABLE
void puar_update(uint8_t puarNum)
{

#if (!defined(_PUAR_INLINE_) || !_PUAR_INLINE_)
	static uint8_t puar_thisByte[NUMPUARS]; // = 0x00;
// #ifdef BIT_TCNT
	static tcnter8_t puar_lastTime[NUMPUARS];
// #else
//	static myTcnter_t puar_nextTcnter[NUMPUARS]; //When the next action should
// #endif												// occur wrt: myTcnter
#endif




#if(!defined(TCNTER_STUFF_IN_MAIN) || !TCNTER_STUFF_IN_MAIN)
	tcnter_update();
#endif

//	int16_t deltaTcnt = (int16_t)thisTcnt - (int16_t)lastTcnt;

	// Handle wrap-around...
//	if (thisTcnt < lastTcnt)
//		deltaTcnt += (int16_t)(OCR0A);

//	lastTcnt = thisTcnt;

//	myTcnter += deltaTcnt;


//When a START /Edge/ is detected, wait for BIT_TCNT/SAMPLE_DIVISOR
// before sampling bits
// (This can be overridden by e.g. CFLAGS in the makefile
#ifndef SAMPLE_DIVISOR
#define SAMPLE_DIVISOR	8
#endif

//            start                                stop
// bits:        v   0   1   2   3   4   5   6   7   v 
//    .....___     ___ ___ ___ ___ ___ ___ ___ ___ ___ .......
//            \___/___X___X___X___X___X___X___X___/
//            ^ ^   ^   ^   ^   ^   ^   ^   ^   ^   ^
//            | |   |   |   |   |   |   |   |   |   |
//            | |   \---+-Data Bits Sampled-+---/   |
//            | |                                   |
//            | \--Start Tested             Stop Bit Sampled
//            |
//            \--Start Detected
//            
//    .....___     ___ ___ ___ ___ ___ ___ ___ ___ ___ .......
//            \___/___X___X___X___X___X___X___X___/
//	           ^ ^   ^   ^   ^   ^   ^   ^   ^   ^   ^
//    ...____/ V \_/ \___ 7 States Unnamed ____/ \_/ \___AWAITING_START (2)
//      |      |  |                               |
//      |      |  \--FIRST_BIT                    \--EXPECTING_STOP
//      |      |
//      |      \--EXPECTING_START
//      |
//      \--AWAITING_START
//
// Bit order might be reversed, (looks right, pseudo-verified)
// I am, however, pretty sure that start-bits are 0 and stop-bits are 1
//    so then the data-bits are directly-read from the pin (not inverted)


// Typically, the start-bit is sampled one half-bit after detection
// but doing-so causes some trouble with polling:
//
//	Just imagining that polling is *exact* timing-wise (which it's not)
//
// I can't seem to draw it in ASCII
//  Paper/Pen case: 4 1/3 TCNTS per poll
//                  20 TCNTS per bit
//                  -> some polls may occur *after* the bit, if starting
//                     with 1/2 bit 
//                     (which can be shifted ~3/4bit due to polling)
//                  -> A TCNT match might occur *immediately* after a poll
//                     but not be recognized until the next poll
//                     (which is at/near the next bit)
// 1Bit = Exactly 4 TCNTs
//    v       v               v               v               v
// |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |
//   |    |    |    |    |    |    |    |    |    |    |    |    |    | 
//  __                ______________                 _____________
//    \______________/              \_______________/
//       ^         ^               ^              ^     


	switch(puar_rxState[puarNum])
	{
		case RXSTATE_AWAITING_START:
	   	if(!puar_readInput(puarNum)) //getbit(rxPin))
			{
				puar_rxState[puarNum] = RXSTATE_EXPECTING_START;
				//Set it up to sample bits halfway...
				//TCNT0 = HALF_TCNT;
				//lastTcnt = thisTcnt;
				//myTcnter = 0;
//#error "HOLY SHIT WTF"
				// a/o 0.40-15
				// Trying to piece together what I saw when I wrote this and
				// disabled compiling with .40-14...
				// The best I can see is:
				// #ifdef BIT_TCNT
				// #else
				//  ... + (BIT_TCNT/8);
				// #endif
				// ***Hardly*** worth disabling entirely!
				// (it just wouldn't compile!)
				// Don't think I'd've disabled compilation based on the warning
				// about tcnter8_t... the warning already existed, no?
				// (at least in 0.40-11)
				// adding an error (kinda redundant anyhow) in that case...
				// Actually, this does make sense as to the reason behind the
				// "WTF" message, as it corresponds with the case, later, where
				// BIT_TCNT not being defined results in another "error"
				// regarding switching over to isItTime.
				// (Adding an override for that, being in an earlier version
				// while working on tabletBluetoother, which requires two
				// different bit_tcnts; one for the tablet at 38400 and one for
				// the bluetoothBee at 115200... so basically trying to compile
				// that project revealed the other error, here, that is now 
				// being ignored until later. )
//			#ifdef BIT_TCNT
#warning "it's assumed that tcnter8_t is good 'nough..."
				puar_lastTime[puarNum] = (tcnter8_t)tcnter_get();
				//puar_nextTcnter[puarNum] = tcnter_get() + puar_bitTcnt[puarNum]/8;
			  	//HALF_TCNT + tcnter_get();
//			#else
//#warning "still using puar_nextTcnter instead of tcnter_isItTime()..."
//				puar_nextTcnter[puarNum] = tcnter_get() + (BIT_TCNT/8);
//#error "This shouldn't compile, BIT_TCNT isn't defined, RIGHT?"
//			#endif
				//Depending on the timer resolution, it might be worthwhile
				// to restart the timer...
				// hopefully the res is high enough not to have to

				puar_thisByte[puarNum] = 0x00;
				DPRINT("@ tcnter=%d: Start Detected [%d]\n",
							(int)puar_lastTime[puarNum],
				 			(int)puarNum);
			#if(!defined(SKIP_WAIT_TO_SAMPLE_START) \
						|| !SKIP_WAIT_TO_SAMPLE_START)
				break;
			#else 
				//Intentional Fall-through
			#endif
			}
			else //Still waiting for that start indication
			{
				break;
			}
			//Potentially Intentional Fallthrough (see above)
		case RXSTATE_EXPECTING_START:
	#if(!defined(SKIP_WAIT_TO_SAMPLE_START) || !SKIP_WAIT_TO_SAMPLE_START)
	 #ifndef BIT_TCNT
			if(tcnter_isItTime8(&(puar_lastTime[puarNum]),
						puar_bitTcnt[puarNum]/SAMPLE_DIVISOR))
//			if(tcnter_get() >= puar_nextTcnter[puarNum])
//	  #if(!defined(PUAR_IGNORE_ISITTIME_ERROR) || \
//			  !PUAR_IGNORE_ISITTIME_ERROR)
//#error "It'd be *really* easy to switch this over to tcnter_isItTime8()"
//	  #endif
	 #else
			if(tcnter_isItTime8(&(puar_lastTime[puarNum]), 
										BIT_TCNT/SAMPLE_DIVISOR))
	 #endif
	#else	//SKIP_WAIT_TO_SAMPLE_START....
#warning "Skipping mid-bit sample delay"
			// No reason to update puar_lastTime... 
	#endif
			{
				// Assuming that this function is called often enough that
				// TCNT can never be >= 2* BIT_TCNT
				// This test should be safe, since it only gets here when 
				//   not awaiting start
				//	else if(myTcnter >= puar_nextTcnter)
				//else if(tcnter_get() >= puar_nextTcnter[puarNum])
				//{
				//This should be safe here for all cases below
				//TCNT0 = thisTcnt - BIT_TCNT;
				//lastTcnt = thisTcnt;
//			#ifndef BIT_TCNT
//				puar_nextTcnter[puarNum] += puar_bitTcnt[puarNum]; //BIT_TCNT;
//			#else
				//This is handled in if(), or unnecessary if SKIP_WAIT...
				//puar_nextTcnter[puarNum] += BIT_TCNT;
//			#endif

				//if(puar_rxState[puarNum] == RXSTATE_EXPECTING_START)
				//{
   			//Make sure it wasn't just a glitch...
   			if(!puar_readInput(puarNum)) //getbit(rxPin))
   			{
         		//TCNT0 = thisTcnt - BIT_TCNT;
         		puar_rxState[puarNum] = RXSTATE_FIRST_BIT;
         		//lastTcnt = thisTcnt;
         		//puar_nextTcnter += BIT_TCNT;
         		DPRINT("@ tcnter=%d: Sampled[%d]: Start\n", 
							(int)puar_lastTime[puarNum],
							(int)puarNum);
   			}
   			else  //GLITCH...
   			{
         		puar_rxState[puarNum] = RXSTATE_AWAITING_START;
         		DPRINT("Error[%d]: Start Glitch\n", (int)puarNum);
   			}
			}
			//else {}	//still waiting to sample...
			break;
		// Data Bits are Default Case... Later...
		case RXSTATE_EXPECTING_STOP:
#ifndef BIT_TCNT
//			if(tcnter_get() >= puar_nextTcnter[puarNum])
			if(tcnter_isItTime8(&(puar_lastTime[puarNum]),
						puar_bitTcnt[puarNum]))
			{	
//				   puar_nextTcnter[puarNum] += puar_bitTcnt[puarNum]; //BIT_TCNT;
#else
			if(tcnter_isItTime8(&(puar_lastTime[puarNum]), BIT_TCNT))
			{
				//puar_lastTime = (uint8_t)tcnter_get();
				//  puar_nextTcnter[puarNum] += BIT_TCNT;
#endif
						
				//Make sure we got our stop-bit
				if(puar_readInput(puarNum)) //getbit(rxPin))
				{
					//!!!LOAD DATA TO RX BUFFER
					puar_rxData[puarNum] = puar_thisByte[puarNum];
					puar_rxDataWaiting[puarNum] = TRUE;
				 DPRINT("@ tcnter=%d: Sampled[%d]: Stop. Received '%c'=0x%x\n",
		             (int)puar_lastTime[puarNum],  				
						 (int)puarNum, (char)(puar_rxData[puarNum]),
						               (int)(puar_rxData[puarNum]));
				}
				else  //framing error...
				{
					//Not sure what to do... could have an error
					// and load data anyhow...
					// or just discard it
					DPRINT("Error[%d]: Stop Not Received\n", (int)puarNum);
				}
				puar_rxState[puarNum] = RXSTATE_AWAITING_START;
			}
			//else {}  //Wait for puar_nextTcnter...
			break;
		// Receiving Data Bits...
		// We *might* be able to get away without these tests
		//  just making it an else case...
		// (Though, intuitively, it makes more visual-sense this way)
		default:
		//else if( (puar_rxState[puarNum] >= RXSTATE_FIRST_BIT) 
		//	 && (puar_rxState[puarNum] < RXSTATE_EXPECTING_STOP) )
#ifndef BIT_TCNT
			if(tcnter_isItTime8(&(puar_lastTime[puarNum]),
						puar_bitTcnt[puarNum]))
//			if(tcnter_get() >= puar_nextTcnter[puarNum])
			{
//				puar_nextTcnter[puarNum] += puar_bitTcnt[puarNum]; //BIT_TCNT;
			#else
			if(tcnter_isItTime8(&(puar_lastTime[puarNum]), BIT_TCNT))
			{
				//	puar_nextTcnter[puarNum] += BIT_TCNT;
			#endif

				uint8_t pinVal = puar_readInput(puarNum); //getbit(rxPin);

				(puar_thisByte[puarNum]) >>= 1;
				//This is specific to 8-bit data-streams!
				// (There may be a way to optimize this a bit)
				// (Especially since getbit uses shifts already)
				(puar_thisByte[puarNum]) |= (pinVal<<7);
				(puar_rxState[puarNum])++;
				DPRINT("@ tcnter=%d: Sampled[%d]: Bit %d = %d\n",
					  		(int)puar_lastTime[puarNum],
										(int)puarNum,
										(int)(puar_rxState[puarNum]-RXSTATE_FIRST_BIT-1),
										(int)pinVal);
			}
			//else {} //Wait for the puar_nextTcnter
			break;
	}
}


/* Timing Considerations:
		TCNT increments:
			near bit-rate (e.g. 1.5 TCNTS = 1 bit)
				high bit-rate
					Risky, updates might not be often enough regardless of xyt
				slow counter
					NEEDs xyt
			good (e.g. 52.9 TCNTS = 1 bit)
				since .9 is dropped from calculations we have 52 TCNTS/bit
				after 10 bits (one frame) this is only 9 TCNTS away from center
					or 9/52 = ~.2 bits away from center
			   	(and next Start-edge should realign again)
				Fast Counter
					multiple tcnts per update (possibly)
						HEREIN LIES THE PROBLEM:
							How to track sample times...?
								tcnt likely to overflow several times per byte
									running tcnt?
				
				xyt would probably be aceptable
				  though it's increasingly likely that each update will have
				  multiple tcnts (and xyt will need to update multiple times)
			far/risky (e.g. 250 TCNTS = 1 bit)
				math gets difficult due to constant wraparound
				Fast Counter
					xyt would be extremely difficult to keep-up
						then again, much less necessary; high-precision

*/



//For compatibility with Tiny24?'s code on 861...
//#define TCNT0 TCNT0L
//#define TCCR0 TCCR0B


//1MHz/9600 = 104.1667

#if (!defined(PU_PC_DEBUG) || (PU_PC_DEBUG == FALSE))
#if (defined(_OSCCAL_SET_))
 #if (!_OSCCAL_SET_)
   #warning "You should calibrate OSCCAL. (OSCCAL_SET is not TRUE in your makefile)"
 #endif
#else
 #error "Define OSCCAL_SET either TRUE or FALSE in your makefile"
 #error "I'm not going to let you get away without it"
 #error "and FALSE is USE-AT-YOUR-OWN-RISK"
 #error "You SHOULD calibrate your oscillator! Otherwise, at the very least,"
 #error "UART reception is gonna be shitty"
#endif
#endif

//THIS IS HIGHLY dependent on the processor frequency...
// which can be calibrated via OSCCAL
// 108 was used BEFORE setting OSCCAL
// 104 seems to work perfectly after
// As I see it (experimentally)
// Leaving this value at 103 (0->OCR0A->0...)
// Causes reception of 0xff after every byte received with 0 in its MSB
// (since the MSB is transmitted last, before the stop-bit)
// Which is basically every ASCII character...
// (somehow this zero is both shifted-in AND picked up as a new start-bit)
// INCREASING THIS VALUE decreases the likelyhood by sampling later in each
//  bit
// BUT it also makes transmission (and reception?) more likely to fail
// as the data rate increases


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
 * /home/meh/_avrProjects/sdramThingZero/21-gittifying/_commonCode_localized/polled_uar/0.40/polled_uar.c
 *
 *    (Wow, that's a lot longer than I'd hoped).
 *
 *    Enjoy!
 *    --Esot.Eric (aka EricWazhung)
 */
