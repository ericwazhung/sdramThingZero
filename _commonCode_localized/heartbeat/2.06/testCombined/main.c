/* mehPL:
 *    This is Open Source-ish, but NOT GPL. I call it mehPL.
 *    I'm not too fond of long licenses at the top of the file.
 *    Please see the bottom.
 *    Enjoy!
 */




#warning "### #error 'revisit tests from oneAxisHolder v34' ###"
//This error can be removed, it's just a reminder.



#include _HEARTBEAT_HEADER_

#if (defined(_TEST_DESKTOP_) && _TEST_DESKTOP_)
 #include "testDesktop.c"
#else
 #define tD_init()         //nada
 #define tD_update(a)    0 //nada
#endif



//This test fades the heartbeat in and out
// When the button is pressed, it causes the heart to blink
// Press it once, and it will blink once, then pause, then once again...
// Press it twice, and it will blink twice, pause, ...
// ...
// Press it four times, and it will return to fading.


//int quit = FALSE;


int main(void)
{

   //This is only relevent in testDesktop...
   uint8_t quit = FALSE;

   //heartBlink, when sent as an argument to heart_blink(), causes the
   //heartbeat to blink that number of times.
   //A value of 0 returns it to the default fade-in/out cycle.
   uint8_t heartBlink = 0;
   
   //buttonPressed is used to get an edge... we don't want to increment the
   //heart-blink every time heart_getButton() returns TRUE (which is every
   //time it's called, while the button is pressed), just increment once
   //for each separate press of the button.
   uint8_t buttonPressed = FALSE;

   
   //Initialize TestDesktop (if selected)
   tD_init();

   //Initialize the heartbeat
   // If using a timer method like HEART_DMS or HEART_TCNTER, this also
   // initializes those. (see heartbeat_config.mk)
   heart_init();

   while(!quit)
   {
      uint8_t heartOn;

      //Update the heartbeat
      // This handles fading, blinking, and reading the pushbutton
      // heartOn will be TRUE if the LED is on, FALSE if the LED is off.
      heartOn = heart_update();

      //Update TestDesktop (if selected)
      quit = tD_update(heartOn);

      //Read the heartbeat's button
      // It returns TRUE if the button is pressed, FALSE otherwise
      // (Of course, pressing the button causes the LED to light-up at
      //  full-brightness until it's released, since it's on the same pin)
      // The button-press is acknowledged upon release.
      if(heart_getButton())
      {
         buttonPressed = TRUE;
      }
      else
      {
         //Has the button *just been* released?
         if(buttonPressed)
         {
            // Pressing the button once will cause it to blink once,
            // twice after the second press
            // and so-on, until 4, when it should go back to fading.
            heartBlink++;
            heartBlink %= 4;
   
            heart_blink(heartBlink);
         }
            
         buttonPressed = FALSE;
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
 * /home/meh/_avrProjects/sdramThingZero/21-gittifying/_commonCode_localized/heartbeat/2.06/testCombined/main.c
 *
 *    (Wow, that's a lot longer than I'd hoped).
 *
 *    Enjoy!
 *    --Esot.Eric (aka EricWazhung)
 */
