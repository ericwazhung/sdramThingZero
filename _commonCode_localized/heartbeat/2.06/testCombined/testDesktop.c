/* mehPL:
 *    This is Open Source-ish, but NOT GPL. I call it mehPL.
 *    I'm not too fond of long licenses at the top of the file.
 *    Please see the bottom.
 *    Enjoy!
 */




//#include _HEARTBEAT_HEADER_
#include <stdio.h>
#include _STDIN_NONBLOCK_HEADER_
#include <unistd.h> //usleep


//testDesktop tests the heartbeat code on a desktop PC...

//Note that the heartbeat code is designed for an LED, subtle variances in
//intensity are generally unnoticeable.
//
//Also, this attempts to convert the "HFM" (which is sort-of like PWM) to a
//screen-viewable form, which introduces various issues which might also
//account for the non-smooth nature of the output, here.
//Finally, this doesn't remove the heartbeat's button-handling code,
//(including waiting for pull-up, etc.), which also helps contribute to the
//non-smooth nature, here.
//
// More importantly, maybe... it seems to be *beating* in the
// physics-sense... Plausibly due to a slight difference in the loopCount,
// vs the heart update rate...? ToPonder.
//
//With some understanding of heartbeat, noticeable variances in intensity 
//and other effects can be useful for visualizing things like
//processor-loading... but here it's just confusing :)

#define HEART_SCREENWIDTH 80

int buttonOn = FALSE;


int heartIntensity = 0;

int loopCount = 0;



void tD_init(void)
{
   stdinNB_init();


   printf("\n\nThis simulates the heartbeat on a desktop PC\n");
   printf(" The heartbeat LED fades smoothly in and out\n");
   printf(" Until the button is pressed (and released)\n");
   printf(" The heart LED will blink the number of times the button has been pressed.\n");
   printf(" Press it four times to return to fading\n");
   printf(" (Note: The lack of smoothness, here, is largely a factor of\n"
          "  trying to convert a quickly-toggling bit into a measurable\n"
          "  value. The effect is unnoticeable on an actual LED)\n");
   printf("\n");
   printf(" Type 'q<Enter>' to quit\n");
   printf(" Type '1<Enter>' to simulate pushing the button\n");
   printf(" Type '0<Enter>' to simulate releasing the button\n");
   printf("(Press <Enter> to begin)\n");

   while(stdinNB_getChar() < 0)
   {}

}


uint8_t tD_update(uint8_t heartOn)
{
   int kbChar = stdinNB_getChar();

   switch(kbChar)
   {
      case '1':
         buttonOn=TRUE;
         break;
      case '0':
         buttonOn=FALSE;
         break;
      case 'q':
         return TRUE;
         break;
      default:
         break;
   }

   heartIntensity += heartOn;


      loopCount++;

      //HFM...
      //Pull-Up switching...
      // so with loopCount ==1024 here, we're getting around 400 as a max
      // heartIntensity...
      if(loopCount == 1024) //256) //HEART_SCREENWIDTH)
      {
         loopCount = 0;

         int i;
         for (i=0; i<HEART_SCREENWIDTH; i++)
         {
               //printf("\r");
               //Print a bar
               if ( i <= (heartIntensity*80/1024) )
                  printf("#");
               //Clear out a previous bar
               //else
               //   printf(" ");
         }
         printf("\n");

         heartIntensity = 0;
      }

      usleep(100);

      return FALSE;
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
 * /home/meh/_avrProjects/sdramThingZero/21-gittifying/_commonCode_localized/heartbeat/2.06/testCombined/testDesktop.c
 *
 *    (Wow, that's a lot longer than I'd hoped).
 *
 *    Enjoy!
 *    --Esot.Eric (aka EricWazhung)
 */
