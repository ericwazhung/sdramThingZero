/* mehPL:
 *    This is Open Source-ish, but NOT GPL. I call it mehPL.
 *    I'm not too fond of long licenses at the top of the file.
 *    Please see the bottom.
 *    Enjoy!
 */


//TODO:
// IOCTL: 0x0000060C   LPRESET       void
//        LPSETTIMEOUT? /drivers/char/lp.c
// (parport.h? in lp.c, not lp.h?
// epp_read/write_addr parport.h...?
// ppdev)


//Pondering using a USB-to-Parallel adaptor 
//They seem to have three modes:
// One-directional
// EPP/ECP
// IEEE-1284

// One-directional likely won't work
//  (Would with an *internal* and bit-banging)
// IEEE-1284 looks a bit too complicated for my needs
//
// EPP/ECP looks to be the way to go...
//  Bidirectional, using the same 8-bit data lines
//  Strobe, etc...

//First we need to make sure it's *possible* to set it in EPP/ECP mode!
#include <stdio.h>
#include <sys/ioctl.h>
//#include <linux/drivers/usb/class/usblp.h>

//ALL THESE for open()?!
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <errno.h>
#include <string.h> //strerror

//#include <linux/lp.h> //LPRESET
#if 0
#ifndef IOCNR_GET_PROTOCOLS
 #error "YEP"
#endif
#endif

int onError(char *location)
{
   if(errno)
   {
      printf("%s: Error %d, '%s'\n", location, errno, strerror(errno));
      return 1;
   }
   else
      return 0;
}

int printerPort_FD;

//These are defined in <kernel-source>/drivers/usb/class/usblp.c
// But there doesn't appear to be an associated header-file!
//This similar manual-entry (rather'n #including) is done in hpoj-0.91
#define IOCNR_GET_DEVICE_ID      1
#define IOCNR_GET_PROTOCOLS      2
#define IOCNR_SET_PROTOCOL    3
#define IOCNR_HP_SET_CHANNEL     4
#define IOCNR_GET_BUS_ADDRESS    5
#define IOCNR_GET_VID_PID     6
#define IOCNR_SOFT_RESET      7
   /* Get device_id string: */
#define LPIOC_GET_DEVICE_ID(len) _IOC(_IOC_READ, 'P', IOCNR_GET_DEVICE_ID, len)

/* The following ioctls were added for http://hpoj.sourceforge.net: */
/* Get two-int array:
  * [0]=current protocol (1=7/1/1, 2=7/1/2, 3=7/1/3),
  * [1]=supported protocol mask (mask&(1<<n)!=0 means 7/1/n supported): */

#define LPIOC_GET_PROTOCOLS(len) _IOC(_IOC_READ, 'P', IOCNR_GET_PROTOCOLS, len)

/* Set protocol (arg: 1=7/1/1, 2=7/1/2, 3=7/1/3): */
#define LPIOC_SET_PROTOCOL _IOC(_IOC_WRITE, 'P', IOCNR_SET_PROTOCOL, 0)


/* Perform class specific soft reset */
#define LPIOC_SOFT_RESET _IOC(_IOC_NONE, 'P', IOCNR_SOFT_RESET, 0)

//see-also hpoj-0.91/mlcd/ExMgr.cpp  (?): llioSetProtocol()


void getProtos(void)
{
   //Huh, isn't this architecture-dependent...?
   int twoInts[2];

   //WHOA DIGITY:
   //Bug in hpoj...?
   //int twoints[2];
   //ioctl(llioInterface[EX_INTERFACE_MLC].fdDevice,LPIOC_GET_PROTOCOLS,
   //      &twoints) <---- twoints is already an address... &(twoints[0])!
   ioctl(printerPort_FD, LPIOC_GET_PROTOCOLS(sizeof(int[2])), 
         (void*)twoInts);

   onError("GET_PROTOCOLS");

   printf("Current Protocol: %d\n"
          "Supported Protocols (Mask): 0x%x\n", twoInts[0], twoInts[1]);
}

int main(int argc, char* argv[])
{
   //ioctl requires a file-descriptor not a FILE*...
   // And we might want some of those other O_ options, as well.
   //FILE *printerPort;
   //printerPort = fopen("/dev/usb/lp0", "r+");

   //int 

   char port[80];
   sprintf(port, "/dev/usb/lp%s", argv[1]);
   
   printf("Opening %s\n", port);

   printerPort_FD =
      open(port, O_RDWR); //Maybe O_NONBLOCK?
   
   if (printerPort_FD == -1)
   {
      if( onError("Open") )
         return 1;
      //printf("Open: Error %d, '%s'\n", errno, strerror(errno));
      //return 1;
   }




#define STRING_LEN  1000

   char ioctl_return[STRING_LEN] = { [0 ... (STRING_LEN-1)] = '\0'};
   
   ioctl(printerPort_FD, LPIOC_GET_DEVICE_ID(STRING_LEN), 
         (void*)ioctl_return);

   onError("DEVICE_ID");

   printf("DEVICE_ID: '%s'\n", ioctl_return);



   getProtos();

   if(argc > 2)
   {
      int newProto = atoi(argv[2]);
    
      printf("Per Request: Setting Protocol to %d\n", newProto); 

      //man ioctl is a bit confusing... or this is implemented weird
      // don't send a pointer to newProto, send newProto itself.
      ioctl(printerPort_FD, LPIOC_SET_PROTOCOL, newProto);

      onError("SET_PROTOCOL");

      getProtos();
   }

   printf("Performing a soft-reset\n");
   printf("This doesn't toggle nINIT/nRESET?!\n");

   ioctl(printerPort_FD, LPIOC_SOFT_RESET, NULL);

   getProtos();

   /*
   //Error 25, 'Inappropriate ioctl for device'
   printf("Performing an LPRESET\n");
   ioctl(printerPort_FD, LPRESET, NULL);
   getProtos();
   */

//Currently: This seems to do *something* to the data-lines, but thereafter
//it seems to stall. Likely due to lack of handshaking per ECP
// Should be testable with little more than a resistor (and capacitor?) to
// add a delay between the write-strobe and the acknowledge...?
#if 1
   int i;
   for(i=1; i<10; i++)
   {
      write(printerPort_FD, &i, 1);
   }
#endif

   close(printerPort_FD);
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
 * /home/meh/_avrProjects/sdramThingZero/21-gittifying/usb-to-parallel/usblpTest.c
 *
 *    (Wow, that's a lot longer than I'd hoped).
 *
 *    Enjoy!
 *    --Esot.Eric (aka EricWazhung)
 */
