/* mehPL:
 *    This is Open Source-ish, but NOT GPL. I call it mehPL.
 *    I'm not too fond of long licenses at the top of the file.
 *    Please see the bottom.
 *    Enjoy!
 */


//polled_uat 0.77
//#ifdef _POLLED_UAT_HEADER_
 #include _POLLED_UAT_HEADER_
//#else
// #include "polled_uat.h"
//#endif

//This file needs to be located in the project's include path...
// e.g. in the main folder
// It's project-specific for writeOutput()
#include "puatStuff.h"


//This isn't implemented *everywhere*...
#if(!defined(PUAT_TCNTER_BITS) || (PUAT_TCNTER_BITS == 8))
 #define puat_tcnter_t  tcnter8_t
 #define puat_tcnterIsItTime(a,b) tcnter_isItTime8V2((a),(b),TRUE)
#elif (PUAT_TCNTER_BITS == 16)
 #define puat_tcnter_t  tcnter_t
 #define puat_tcnterIsItTime(a,b) tcnter_isItTimeV2((a),(b),TRUE)
#else
 #error "PUAT_TCNTER_BITS should either be 8 or 16"
#endif








#if (defined(PU_PC_DEBUG) && PU_PC_DEBUG)
   #define DPRINT printf
   //This is a hack... not sure what to do about it yet
// extern int  TCNT0;
// extern int  puat_writeOutput(a,b);

// This should no longer be used... since puatStuff.h... right?
// extern int lineVal[NUMPUATS]; //This is also quite hokey...
                        // it's exclusive to uat->uar test code
                        // it shouldn't matter otherwise, 'cause it's
                        // never referenced elsewhere (right?)
#else
   #define DPRINT(...) {}
#endif

//********** POLLED UART Defines **********//

#define DATA_BITS                 8
#define START_BIT                 1
#define PUAT_STOP_BITS              1//2

//#define TIMER0_SEED (256 - ( (SYSTEM_CLOCK / BAUDRATE) / TIMER_PRESCALER ))

//tcnter_t puat_bitTcnt[NUMPUATS];

#ifndef BIT_TCNT
 tcnter_t puat_bitTcnt[NUMPUATS];
 #define puat_bitTcntF(puatNum)  (puat_bitTcnt[puatNum])
 //Inlining this saved nearly 30Bytes! (in a for loop for two puats)
 PUAT_INLINEABLE
 void puat_setBitTcnt(uint8_t puatNum, tcnter_t tcnt)
 {
   puat_bitTcnt[puatNum] = tcnt;
   DPRINT("puat_setBitTcnt(%d, %d)\n", (int)puatNum, (int)tcnt);
 }
#else
 #define puat_bitTcntF(puatNum)  (BIT_TCNT)
#endif

#ifdef _PUAT_HFM_PERCENT_
 //uint8_t puat_hfmVal[NUMPUATS];
 tcnter_t puat_thisBitTcnt[NUMPUATS];
 #define puat_thisBitTcntF(puatNum) (puat_thisBitTcnt[puatNum])

 #define setNextBitTcnt(puatNum) (puat_thisBitTcnt[puatNum] \
       = ( puat_bitTcntF(puatNum) + hfm_nextOutput(&(puat_hfm[puatNum]))) )
 //#define bitHFM(puatNum) (puat_hfmVal[puatNum])
 hfm_t puat_hfm[NUMPUATS];

#else
 #define puat_thisBitTcntF(puatNum) puat_bitTcntF(puatNum)
 #define setNextBitTcnt(puatNum) {}
 //#define bitHFM(puatNum) (0)
#endif




//Inlining this saves 14Bytes (for loop, two puats)
PUAT_INLINEABLE 
void puat_init(uint8_t puatNum)
{
#if(!defined(TCNTER_STUFF_IN_MAIN) || !TCNTER_STUFF_IN_MAIN)
   tcnter_init();
#endif

#ifdef _PUAT_HFM_PERCENT_
    hfm_setup(&(puat_hfm[puatNum]), _PUAT_HFM_PERCENT_, 100);
#endif

   // e.g. set the pin as an output...
   puat_initOutput(puatNum);

   //Set the line to IDLE...
   puat_writeOutput(puatNum, 1);

#ifdef BIT_TCNT
   //Insert a delay on init to make sure the receiver recognizes the
 //This can't be used if the tcnter source is a variable that has to be
 //updated in the main-loop (duh)
 #if(!defined(TCNTER_SOURCE_EXTERNED) || !TCNTER_SOURCE_EXTERNED)
 
  #warning "This should work, but it's likely that BIT_TCNT*20 is greater than variable's max, then will wrap negative, in which case it won't run the loop anyhow."
   //Tx-Idle state... TODO FOR TCNTER: Add tcnter_delay(?)
   tcnter_update();
   tcnter_t startTime=tcnter_get();

   // Two bytes is BIT_TCNT * 10bits/byte * 2bytes
   while(!tcnter_isItTime(&startTime, (tcnter_t)(BIT_TCNT)*(tcnter_t)(20)))
   {
      tcnter_update();
   }
#endif

// puat_bitTcnt[puatNum] = BIT_TCNT;
#else
   #warning "Since BIT_TCNT isn't set, you're using your own bitTcnt... you'll probably have to insert some delays after puat_init (and after setting the bitTcnt) to make sure the receiver recognizes the Tx-idle state"
#endif


   DPRINT("puat_init(%d)\n", (int)puatNum);
}

//#define BIT_TCNT    //THIS NEEDS TO BE CALCULATED!!!
//#define HALF_TCNT (BIT_TCNT/2)

uint8_t puat_txData[NUMPUATS];
uint8_t puat_txDataWaiting[NUMPUATS]; //=FALSE;

//#warning "puat_dataWaiting/sendByte can't be inlined with btbcStuff?!"
// since btbcStuff is in a separate file-space... 
// that should be changed soon (by inlining it, as well)
PUAT_INLINEABLE
uint8_t puat_dataWaiting(uint8_t puatNum)
{
   return puat_txDataWaiting[puatNum];
}

//Must be called AFTER testing if data's waiting
// otherwise it will return garbage
PUAT_INLINEABLE
void puat_sendByte(uint8_t puatNum, uint8_t data)
{
   puat_txDataWaiting[puatNum] = TRUE;
   puat_txData[puatNum] = data;
}


uint8_t puat_txState[NUMPUATS];
#define TXSTATE_IDLE             0  //Waiting for data
#define TXSTATE_SENDING_START    1  //Waiting to complete the start bit
#define TXSTATE_SENDING_BIT8     (TXSTATE_SENDING_START + DATA_BITS) //Waiting to complete the last data bit
// Bits are received here
#define TXSTATE_SENDING_STOP  (TXSTATE_SENDING_BIT8+1)



#if(defined(PUAT_SENDSTRINGBLOCKING_ENABLED) \
    && PUAT_SENDSTRINGBLOCKING_ENABLED)

 #if(defined(TCNTER_SOURCE_EXTERNED) && TCNTER_SOURCE_EXTERNED)
  #error "Can't use Blocking functions with TCNTER_SOURCE_EXTERNED, because the variable needs to be updated, and while-loops would prevent that"
 #endif
void puat_sendByteBlocking(uint8_t puatNum, uint8_t byte)
{
   
   tcnter_update();
   puat_update(puatNum);
   puat_sendByte(puatNum, byte);

   extern uint8_t puat_txState[NUMPUATS];

   //17-3ish... why wasn't puat_dataWaiting() enough?!
   //  puat_txState, here, was originally txState, but couldn'ta been
   //  from another library... could it? Doubtful.
   while(puat_dataWaiting(puatNum) 
         || (puat_txState[puatNum] != TXSTATE_IDLE) )
   {
      tcnter_update();
      puat_update(puatNum);
   }
}


//To just send a string, without caring if it blocks...
void puat_sendStringBlocking(uint8_t puatNum, char string[])
{
   uint8_t i;

   for(i=0 ; string[i] != '\0' ; i++)
   {
      puat_sendByteBlocking(puatNum, string[i]);
   }

}

#ifdef __AVR_ARCH__
//#include <stdio.h>    //for sprintf_P...
//#include <string.h>
#include <avr/pgmspace.h> //for strcpy_P
void puat_sendStringBlocking_P(uint8_t puatNum, char stringBuffer[], 
                                    PGM_P P_string)
{
   //Wow, this change from 8106 -> 8036, stdio still included!
//   sprintf_P(stringBuffer, P_string);
   strcpy_P(stringBuffer, P_string);
   puat_sendStringBlocking(puatNum, stringBuffer);
   stringBuffer[0]='\0';
}
#endif
#endif







//#warning "Changing TCNT for this purpose makes the timer unusable for more than one UAR/T or other device at a time!"
// What about a tcnt DIFFERENCE, instead?
// How difficult would this be with wraparound and/or resets at arbitary
// values?
// Can we use the other devices' (e.g. DMS) OCR values?
// What about an xyTracker for more precision and less cumulative error?


//Apparently inline functions aren't allowed to have static variables...
// so let's make 'em global, YAY!
// HAH! Code-size has increased!
// whereas before it was using a register, now it's using SRAM
// (which is to be expected... but the bss/data spaces haven't changed...
//  so is it treating them as volatile, or something?)
// e.g.:
//shiftingByte[puatNum] = txData[puatNum];
//lds r13, 0x00D6
// is now
//puat_shiftingByte[puatNum] = puat_txData[puatNum];
//lds r24, 0x00CE
//sts 0x00D6, r24
// Or Worse:
//    nextTcnter[puatNum] = tcnter_get() + BIT_TCNT;
//e0 90 86 00  lds   r14, 0x0086
//f0 90 87 00  lds   r15, 0x0087
//00 91 88 00  lds   r16, 0x0088
//10 91 89 00  lds   r17, 0x0089
//8a e1        ldi   r24, 0x1A   ; 26
//90 e0        ldi   r25, 0x00   ; 0
//a0 e0        ldi   r26, 0x00   ; 0
//b0 e0        ldi   r27, 0x00   ; 0
//e8 0e        add   r14, r24
//f9 1e        adc   r15, r25
//0a 1f        adc   r16, r26
//1b 1f        adc   r17, r27
// is now (note 2-cycle instructions)
//    puat_nextTcnter[puatNum] = tcnter_get() + BIT_TCNT;
//80 91 86 00  lds   r24, 0x0086
//90 91 87 00  lds   r25, 0x0087
//a0 91 88 00  lds   r26, 0x0088
//b0 91 89 00  lds   r27, 0x0089
//4a 96        adiw  r24, 0x1a   ; 26
//a1 1d        adc   r26, r1
//b1 1d        adc   r27, r1
//80 93 cf 00  sts   0x00CF, r24
//90 93 d0 00  sts   0x00D0, r25
//a0 93 d1 00  sts   0x00D1, r26
//b0 93 d2 00  sts   0x00D2, r27
// using asm("r2") decreased code-size by 30 bytes!
// but it doesn't work too well with an array...
// feel free to revisit 0.40r but it ain't here.
#if(defined(_PUAT_INLINE_) && _PUAT_INLINE_)
 uint8_t puat_shiftingByte[NUMPUATS]; // asm("r2"); // = 0x00;


//These values are completely arbitrary!
#if(!defined(PUAT_OVERRIDE_BIT_TCNT_ERROR) || (BIT_TCNT > 220))
#if(defined(BIT_TCNT) && (BIT_TCNT > 128))
 #error "large BIT_TCNTs won't work with tcnter_isItTime()"
 #error "though this is a bit conservative"
#endif
#endif
 
 // tcnter8_t puat_lastTime[NUMPUATS];
   puat_tcnter_t puat_lastTime[NUMPUATS];
   //tcnter_t puat_nextTcnter[NUMPUATS]; // = 0; 
         //When the next action should occur
                                          // wrt: myTcnter
#endif



//Inlining this saved 18Bytes (one call, otherwise it'd go HUGE)
PUAT_INLINEABLE
void puat_update(uint8_t puatNum)
{
#if(!defined(_PUAT_INLINE_) || !_PUAT_INLINE_)
   static uint8_t puat_shiftingByte[NUMPUATS]; // = 0x00;

   static puat_tcnter_t puat_lastTime[NUMPUATS];
// static tcnter8_t puat_lastTime[NUMPUATS]; 
// static tcnter_t puat_nextTcnter[NUMPUATS]; // = 0; 
   //When the next action should occur
                                    // wrt: myTcnter
#endif

#if(!defined(TCNTER_STUFF_IN_MAIN) || !TCNTER_STUFF_IN_MAIN)
      tcnter_update();
#endif


// int16_t deltaTcnt = (int16_t)thisTcnt - (int16_t)lastTcnt;

   // Handle wrap-around...
// if (thisTcnt < lastTcnt)
//    deltaTcnt += (int16_t)(OCR0A);

// lastTcnt = thisTcnt;

// myTcnter += deltaTcnt;


//a/o v0.75:
//
// This is how it works and has for some time...
// so sending 0x55=0b01010101 gets reversed and equates to:
//        ....11111101010101011111111....
//              iiiis01234567Siiiii (i=idle, s=start, S=stop)
//
// Viewed on a 'scope, ¯¯¯¯¯|_|¯|_|¯|_|¯|_|¯|_|¯¯¯¯¯¯¯
//                           s 0 1 2 3 4 5 6 7 S
//
// So, at 9600bps, that's 9 bits visible (stop is eaten by the following
// idle) 937.5us
//
//                 LSB                         MSB
//            start                                stop
// bits:        v   0   1   2   3   4   5   6   7   v 
//  1 .....___     ___ ___ ___ ___ ___ ___ ___ ___ ___ .......
//  0         \___/___X___X___X___X___X___X___X___/
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
//
//    ..._____^_ _^____ 7 States Unnamed _____^_ _^_ _^_______IDLE (2)
//      |       |                               |   |
//      |       |                 SENDING_BIT8--/   \--SENDING_STOP
//      |       |
//      |       \--SENDING_START
//      |
//      \--IDLE
//
// Bit order might be reversed, (looks right, pseudo-verified)
// I am, however, pretty sure that start-bits are 0 and stop-bits are 1
//    so then the data-bits are directly-read from the pin (not inverted)

   if(puat_txState[puatNum] == TXSTATE_IDLE)
   {
      if(puat_txDataWaiting[puatNum])
      {
         puat_shiftingByte[puatNum] = puat_txData[puatNum];
         puat_txDataWaiting[puatNum] = FALSE;

         puat_txState[puatNum] = TXSTATE_SENDING_START;

         //myTcnter = 0;
         //puat_lastTime[puatNum] = (tcnter8_t)(tcnter_get());
         puat_lastTime[puatNum] = (puat_tcnter_t)(tcnter_get());
         //puat_nextTcnter[puatNum] = tcnter_get() + puat_bitTcnt[puatNum]; //BIT_TCNT + tcnter_get();
         
         //Depending on the timer resolution, it might be worthwhile
         // to restart the timer...
         // hopefully the res is high enough not to have to

         puat_writeOutput(puatNum, 0);
         DPRINT("Sending[%d]: StartBit (data='%c'=0x%x); lastTime=%d\n", 
               (int)(puatNum), puat_shiftingByte[puatNum],
               (int)(puat_shiftingByte[puatNum]), 
               (int)puat_lastTime[puatNum]);

         setNextBitTcnt(puatNum);
      }
      else
      {
         //Still waiting for data to transmit
      }
   }
   // Assuming that this function is called often enough that
   // TCNT can never be >= 2* BIT_TCNT
   // This test should be safe, since it only gets here when 
   //   not awaiting start
// else if(myTcnter >= puat_nextTcnter)

// else if(tcnter_isItTime8(&(puat_lastTime[puatNum]),
   else if(puat_tcnterIsItTime(&(puat_lastTime[puatNum]),
                                       puat_thisBitTcntF(puatNum)) )
      //tcnter_get() >= puat_nextTcnter[puatNum])
   {
      //This should be safe here for all cases below
      //TCNT0 = thisTcnt - BIT_TCNT;
      //lastTcnt = thisTcnt;

      // We *might* be able to get away without these tests
      //  just making it an else case...
      // (Though, intuitively, it makes more visual-sense this way)
      //Transmit Data Bits...
      if( (puat_txState[puatNum] >= TXSTATE_SENDING_START)
          && (puat_txState[puatNum] < TXSTATE_SENDING_BIT8) )
      {
         puat_txState[puatNum]++;
         puat_writeOutput(puatNum, (puat_shiftingByte[puatNum])&0x01);
         //puat_shiftingByte >>= 1;

         DPRINT("Sending[%d]: Bit %d = %d; lastTime=%d\n", (int)(puatNum),
                     (int)(puat_txState[puatNum]-TXSTATE_SENDING_START-1),
                     (int)((puat_shiftingByte[puatNum])&0x01),
                     (int)puat_lastTime[puatNum]);

         puat_shiftingByte[puatNum] >>= 1;
      }
      //Transmit Stop Bit...
      else if( puat_txState[puatNum] == TXSTATE_SENDING_BIT8 ) 
      {
         puat_txState[puatNum] = TXSTATE_SENDING_STOP;
         puat_writeOutput(puatNum, 1);

         DPRINT("Sending[%d]: StopBit; lastTime=%d\n", (int)(puatNum),
               (int)puat_lastTime[puatNum]);
      }
      else if(puat_txState[puatNum] == (TXSTATE_SENDING_STOP
                                          +PUAT_STOP_BITS - 1))
      {
         puat_txState[puatNum] = TXSTATE_IDLE;
         DPRINT("Transmission[%d] Complete; lastTime=%d\n", (int)(puatNum),
               (int)puat_lastTime[puatNum]);
      }
      else //Stop bits...
      {
         //Value has already been loaded we're just waiting...
         puat_txState[puatNum]++;
         //OLD://!!!We shouldn't get here!
      }

      setNextBitTcnt(puatNum);
   }
   else
   {
/*
#if (defined(PU_PC_DEBUG) && PU_PC_DEBUG)
//    static uint8_t lastPrintedTcnt = 0;
      //Between bits... nothing to do
//    if(thisTcnt != lastPrintedTcnt)
//    {
//       lastPrintedTcnt = thisTcnt;
      if(deltaTcnt)
      {
         DPRINT(" myTcnter: %d ", myTcnter);
      }  
#endif
*/
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
 * /home/meh/_avrProjects/sdramThingZero/21-gittifying/_commonCode_localized/polled_uat/0.90/polled_uat.c
 *
 *    (Wow, that's a lot longer than I'd hoped).
 *
 *    Enjoy!
 *    --Esot.Eric (aka EricWazhung)
 */
