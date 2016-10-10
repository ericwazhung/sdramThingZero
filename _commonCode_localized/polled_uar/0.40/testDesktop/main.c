/* mehPL:
 *    This is Open Source-ish, but NOT GPL. I call it mehPL.
 *    I'm not too fond of long licenses at the top of the file.
 *    Please see the bottom.
 *    Enjoy!
 */


//Taken from stdinNonBlock
//  Taken from serIOtest20, and modified slightly


#include <stdio.h>
#include <errno.h>
#include <string.h> //needed for strerror()
#include <fcntl.h> //file control for open/read
#include _POLLED_UAR_HEADER_



//These need to be global because they're used by polled_uar.c
// via #defines...
int lineVal[NUMPUARS]; // = 1;
tcnter_source_t timerCounter = 0;

//These have been moved to the makefile
//#define PU_PC_DEBUG	1
//#define getbit(a) (lineVal)
//#define TCNT0	(timerCounter)
//#define OCR0A	(9)
//#define BIT_TCNT 6
//#include "../polled_uar.c"


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


//uint8_t puar_readInput(uint8_t puarNum)
//{
//	CFLAGS += -D'getbit(...)=(lineVal)'
//	return lineVal[puarNum];
//}

#include "../../../__std_wrappers/stdin_nonBlock/0.10/stdin_nonBlock.h"
int main(void)
{

	int quit = 0;
	int timer = 0;
	int doRun = 0;

	/*
	// Set STDIN non-blocking (Still requires Return)
	int flags = fcntl(0, F_GETFL);
	flags |= O_NONBLOCK;
	fcntl(0, F_SETFL, flags);
	*/
	stdinNB_init();


//	handleError("Couldn't open stdin.", 1);

	printf("\n\nSimulate the Polled Universal Asynchronous Receiver\n\n");

	printf(
"            start                                stop               \n"
" bits:        v   0   1   2   3   4   5   6   7   v                 \n"
"  idle...___     ___ ___ ___ ___ ___ ___ ___ ___ ___ ....idle        \n"
"            \\___/___X___X___X___X___X___X___X___/                   \n"
"            ^ ^   ^   ^   ^   ^   ^   ^   ^   ^   ^                 \n"
"            | |   |   |   |   |   |   |   |   |   |                 \n"
"            | |   \\---+-Data Bits Sampled-+---/   |                 \n"
"            | |                                   |                 \n"
"            | \\--Start Sampled           Stop Bit Sampled          \n"
"            |                                                       \n"
"            \\--Start Detected                                    \n\n");

   printf("NOTES:\n");
	printf(" The \"baud-rate\" for this test is %d TCNTs per bit\n",
																					 BIT_TCNT);
	printf(" Sampling begins %d TCNTs *after* detection of a start condition\n",
																BIT_TCNT/SAMPLE_DIVISOR);
	printf(
" There are *two* polled-UAR's simulated, here. puar0 and puar1\n");
	printf(
" 'TCNT' represents an AVR timer that counts from 0 and resets when it\n"
"  reaches %d.\n", TCNTER_SOURCE_OVERFLOW_VAL);
	printf(
" 'tcnter' tracks the total number of TCNTs, regardless of when it resets\n");
	printf("Both PUARs begin in the idle condition (line-level = 1)\n");

	printf("\n (All keyboard entries must be followed by <Enter>)\n");
	printf(" 'q'  quit\n");
	printf(" '0'  sets the Rx line level of puar0 to 0 (Low)\n");
	printf(" '1'  sets the Rx line level of puar0 to 1 (High)\n");
	printf(" 'L'  sets the Rx line level of puar1 to 0 (Low)\n");
	printf(" 'H'  sets the Rx line level of puar1 to 1 (High)\n");


	//POLLED_UAR Specific Stuff
	//puar_init();

	uint8_t puar;
	for(puar=0; puar<NUMPUARS; puar++)
	{
		puar_init(puar);
		lineVal[puar] = 1;
	}
	tcnter_init();



	printf("Press <Enter> to begin\n");
	while(stdinNB_getChar() != '\n'){}
	


	while(!quit)
	{
		//This worked in 0.10-3! But now 0.10-4?!
//		handleError(" Unhandled Error.", 0);

//		char kbChar;
//		kbChar = getchar();

		int kbChar = stdinNB_getChar();

		//"Resource Temporarily Unavailable" isn't an error in this case
//		if(errno == 35)
//			errno = 0;
//		else
		{
			switch(kbChar)
			{
				case -1:
					break;
				case 'q':
					quit = 1;
					break;
				//POLLED_UAR TESTS:
				case '1':
					lineVal[0] = 1;
					printf("lineVal[0]: 1\n");
					break;
				case '0':
					lineVal[0] = 0;
					printf("lineVal[0]: 0\n");
					break;
				case 'H':
				case 'h':
					lineVal[1] = 1;
					printf("lineVal[1]: 1\n");
					break;
				case 'L':
				case 'l':
					lineVal[1] = 0;
					printf("lineVal[1]: 0\n");
					break;
				case '\n':
					break;
				//^^^ TO HERE
				default:
					printf("Not Handled: '%c'", kbChar);
			}
		}


		tcnter_update();
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

		//Update the tcnter once per second. 
		//But run the loop 10 times per second.
		usleep(100000);
		printf(".");
		timer++;

		if(timer == 10)


		{
			//A simulated timer/counter... timerCounter is the same as TCNT
			// it counts from 0 to TCNTER_SOURCE_OVERFLOW_VAL and resets
			timerCounter++;
			if(timerCounter == TCNTER_SOURCE_OVERFLOW_VAL) //OCR0A)
				timerCounter = 0;
			timer=0;
			printf("TCNT: %d, tcnter=%d\n",timerCounter, (int)tcnter_get());
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
 * /home/meh/_avrProjects/sdramThingZero/21-gittifying/_commonCode_localized/polled_uar/0.40/testDesktop/main.c
 *
 *    (Wow, that's a lot longer than I'd hoped).
 *
 *    Enjoy!
 *    --Esot.Eric (aka EricWazhung)
 */
