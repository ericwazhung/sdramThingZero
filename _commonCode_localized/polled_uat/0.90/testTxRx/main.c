/* mehPL:
 *    This is Open Source-ish, but NOT GPL. I call it mehPL.
 *    I'm not too fond of long licenses at the top of the file.
 *    Please see the bottom.
 *    Enjoy!
 */


//Taken from polled_uat/test 0.10-5
//  Taken from stdinNonBlock
//  Taken from serIOtest20, and modified slightly

#include <stdio.h>
#include <errno.h>
#include <string.h> //needed for strerror()
#include <fcntl.h> //file control for open/read
#include _POLLED_UAT_HEADER_
#include _POLLED_UAR_HEADER_

//These need to be global because they're used by polled_uat.c
// via #defines...

#if( NUMPUATS != NUMPUARS )
   #error "Num Tx !=Num Rx... this is OK, but not as-implemented here"
#endif

int lineVal[NUMPUATS];// = 1;
tcnter_source_t timerCounter = 0;

//These have been moved to the makefile
//#define PU_PC_DEBUG   1
//#define getbit(a) (lineVal)
//#define TCNT0   (timerCounter)
//#define OCR0A   (9)
//#define BIT_TCNT 6
//#include "../polled_uat.c"


#define handleError(string, exit) \
   if(errno)   \
{\
      printf( string " Error %d: '%s'\n", errno, strerror(errno)); \
      if(exit) \
         return 1; \
      else \
         errno = 0; \
}

//errno = 0 seems to work, whereas clearerr(stdin) doesn't

/*
void puat_writeOutput(uint8_t puatNum, uint8_t value)
{
   lineVal[puatNum]=value;

   printf("writeOutput[%d]: %d\n", puatNum, value);
}

uint8_t puar_readInput(uint8_t puarNum)
{
   return lineVal[puarNum];
}
*/

int main(void)
{

   int quit = 0;
   int timer = 0;

   // Set STDIN non-blocking (Still requires Return)
   int flags = fcntl(0, F_GETFL);
   flags |= O_NONBLOCK;
   fcntl(0, F_SETFL, flags);

   handleError("Couldn't open stdin.", 1);

   printf("---Press q and press return to quit---\n");
   printf("   Press another letter and return to start sending.\n");
   printf("   (upper-case letters sent on puat 1, lower-case on 0).\n");

   //POLLED_UAT Specific Stuff

   uint8_t puar;
   for(puar=0; puar<NUMPUARS; puar++)
   {
      puar_init(puar);
      //puat_init handles this... via writeOutput()
      // lineVal[puar] = 1;
   }

   int puat;
   for(puat=0; puat<NUMPUATS; puat++)
      puat_init(puat);
   tcnter_init();

   while(!quit)
   {
      //This worked in 0.10-3! But now 0.10-4?!
//    handleError(" Unhandled Error.", 0);

      char kbChar;
      kbChar = getchar();

      //"Resource Temporarily Unavailable" isn't an error in this case
      if(errno == 35)
         errno = 0;
      else
      {
         switch(kbChar)
         {
            case 'q':
               quit = 1;
               break;
            //POLLED_UAT TESTS:
            case '\n':
               break;
            //^^^ TO HERE
            default:
               //Send Upper-case chars to puat 1
               //     Lower-case to puat 0
               if((kbChar >= 'A') && (kbChar <= 'Z'))
                  puat = 1;
               else
                  puat = 0;
      
               if(puat_dataWaiting(puat))
                  printf("Buffer[%d] Full; Ignoring '%c'\n",
                                       (int)puat, kbChar);
               else
                  puat_sendByte(puat,kbChar);
         
         }
      }


      tcnter_update();

      for(puat=0; puat<NUMPUATS; puat++)
         puat_update(puat);
      //POLLED_UAR_update();

      for(puar=0; puar<NUMPUARS; puar++)
      {
         puar_update(puar);

         if(puar_dataWaiting(puar))
         {
            unsigned char byte = puar_getByte(puar);
            printf("POLLED_UAR[%d] Received: '%c'=0x%x\n",
                           (int)puar, byte, (int)byte);
         }
      }


//    if(puat_dataWaiting())
//    {
//       unsigned char byte = POLLED_UAT_getByte();
//       printf("POLLED_UAT Received: '%c'=0x%x\n", byte, (int)byte);
//    }

      timer++;

      if(timer == 100000)
      {
         timerCounter++;
         if(timerCounter == TCNTER_SOURCE_OVERFLOW_VAL) //OCR0A)
            timerCounter = 0;
         timer=0;
         printf("TCNT: %d\n",timerCounter);
      }

   }


   return 0;
}

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
 * /home/meh/_avrProjects/sdramThingZero/21-gittifying/_commonCode_localized/polled_uat/0.90/testTxRx/main.c
 *
 *    (Wow, that's a lot longer than I'd hoped).
 *
 *    Enjoy!
 *    --Esot.Eric (aka EricWazhung)
 */
