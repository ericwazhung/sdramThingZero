/* mehPL:
 *    This is Open Source-ish, but NOT GPL. I call it mehPL.
 *    I'm not too fond of long licenses at the top of the file.
 *    Please see the bottom.
 *    Enjoy!
 */


//TODO: These should probably be moved to bithandling_avr.h...?
#include <avr/io.h>
#include <avr/interrupt.h> //sei()

#include <stdint.h>
#include <inttypes.h>
#include _BITHANDLING_HEADER_
#include "pinout.h"
#include "sdram_general.h"
#include <util/delay.h> //_delay_us()


//See the notes in sdram_general.h
// These are NOT YET used (nor assigned values!) in sdram_general!
uint16_t colsPerPage;
uint16_t numRows;
uint8_t numBanks;



//These are AVR-specific, for working with a synchronized SDRAM and AVR
//clock. Wherein, it's possible on an AVR (and not, on many other devices)
//to toggle a pin for *exactly* one clock-cycle.
//These are not relevant anymore, as the one-shot circuitry takes care of
//it. (Note the dramatic difference between these functions and the
//general-purpose functions now used with one-shots!)
#if 0
//Insert some NOPs between CS->L and CS->H
// This'll simulate an asynchronous "host"
//  to verify our one-shot circuits
#define DRAG_OUT_CS  1

//Similarly for CKE
#define DRAG_OUT_CKE 1

//The goal is to have the chip-select strobed for only one clock-cycle
//TODO: This is ONLY for Early-Testing of sTZ
// later it will be implemented with a one-shot, and the duration won't
// matter.
void sdram_strobeChipSelect(uint8_t devNum)
{
   //BECAUSE CS and CKE are the only two pins on the port we can get away
   //with simplification...
   // We assume that CKE is already active.
   uint8_t csEnabled = (1<<SDRAM_CKE_PIN);
   uint8_t csDisabled = (1<<SDRAM_CKE_PIN);

   if( (devNum == 0) || (devNum == SDRAM__ALL_DEVICES) )
   {
      csEnabled |= (0<<SDRAM_FR_CS_PIN);
      csDisabled |= (1<<SDRAM_FR_CS_PIN);
   }
   //NYI
   //if( (devNum == 1) || (devNum == SDRAM__ALL_DEVICES) )
   
   
   uint8_t CLI_SAFE(oldIstate);

   //Doing inline-asm should assure that it's not mangled:
   // (e.g. that the optimizer doesn't choose to out CS Active first, then
   //  do the math for CS Inactive, followed by the output CS Inactive)
   __asm__ __volatile__
   (
      "out  %0, %1; \n\t"  //Output CS Active port value
#if (defined(DRAG_OUT_CS) && DRAG_OUT_CS)
      "nop; \n\t"
      "nop; \n\t"
      "nop; \n\t"
      "nop; \n\t"
#endif
      "out  %0, %2; \n\t"  //Output CS Inactive port value
      //Output Register: (none, nothing to return)
      :
      //Input values/registers:
      //These are set-up by C and the preprocessor before entering the asm
      //function (right? That's the point...)
      : "I" (_SFR_IO_ADDR(SDRAM_FR_CS_PORT)),      //%0
        "d" (csEnabled),       //%1
        "d" (csDisabled)       //%2
      //Clobbered Registers (none here):
      //(See notes in oldStrobeChipSelect.txt)
      :
   );

   SEI_RESTORE(oldIstate);
}



void sdram_strobeChipSelect_andDisableClock(uint8_t devNum)
{
   //BECAUSE CS and CKE are the only two pins on the port we can get away
   //with simplification...
   // We assume that CKE is already active.
   uint8_t csEnabled = (1<<SDRAM_CKE_PIN);
   uint8_t csDisabled_ckeDisabled = (0<<SDRAM_CKE_PIN);


   if( (devNum == 0) || (devNum == SDRAM__ALL_DEVICES) )
   {
      csEnabled |= (0<<SDRAM_FR_CS_PIN);
      csDisabled_ckeDisabled |= (1<<SDRAM_FR_CS_PIN);
   }
   //NYI
   //if( (devNum == 1) || (devNum == SDRAM__ALL_DEVICES) )


   uint8_t CLI_SAFE(oldIstate);

   //Doing inline-asm should assure that it's not mangled:
   // (e.g. that the optimizer doesn't choose to out CS Active first, then
   //  do the math for CS Inactive, followed by the output CS Inactive)
   __asm__ __volatile__
   (
      "out  %0, %1; \n\t"  //Output CS Active port value
      "out  %0, %2; \n\t"  //Output CS Inactive port value
      //Output Register: (none, nothing to return)
      :
      //Input values/registers:
      //These are set-up by C and the preprocessor before entering the asm
      //function (right? That's the point...)
      : "I" (_SFR_IO_ADDR(SDRAM_FR_CS_PORT)),      //%0
        "d" (csEnabled),       //%1
        "d" (csDisabled_ckeDisabled)       //%2
      //Clobbered Registers (none here):
      //(See notes in oldStrobeChipSelect.txt)
      :
   );

   SEI_RESTORE(oldIstate);
}

void sdram_strobeClockEnable(void)
{
   //BECAUSE CS and CKE are the only two pins on the port we can get away
   //with simplification...
   // We assume that CKE is already active.
   uint8_t csDisabled_ckeEnabled = 
                        (1<<SDRAM_CKE_PIN) | (1<<SDRAM_FR_CS_PIN);
   uint8_t csDisabled_ckeDisabled = 
                        (0<<SDRAM_CKE_PIN) | (1<<SDRAM_FR_CS_PIN);

   uint8_t CLI_SAFE(oldIstate);

   //Doing inline-asm should assure that it's not mangled:
   // (e.g. that the optimizer doesn't choose to out CS Active first, then
   //  do the math for CS Inactive, followed by the output CS Inactive)
   __asm__ __volatile__
   (
      "out  %0, %1; \n\t"  //Output CS Active port value
#if (defined(DRAG_OUT_CKE) && DRAG_OUT_CKE)
      "nop; \n\t"
      "nop; \n\t"
      "nop; \n\t"
      "nop; \n\t"
#endif
      "out  %0, %2; \n\t"  //Output CS Inactive port value
      //Output Register: (none, nothing to return)
      :
      //Input values/registers:
      //These are set-up by C and the preprocessor before entering the asm
      //function (right? That's the point...)
      : "I" (_SFR_IO_ADDR(SDRAM_FR_CS_PORT)),      //%0
        "d" (csDisabled_ckeEnabled),       //%1
        "d" (csDisabled_ckeDisabled)       //%2
      //Clobbered Registers (none here):
      //(See notes in oldStrobeChipSelect.txt)
      :
   );

   SEI_RESTORE(oldIstate);
}
#else // TRUE:
//These are the "strobe" functions to work with one-shot circuitry
//Note these don't require any specific-timing.
// and can even be interrupted!
// (and, maybe, would be a bit faster if #defines or always inline...?)
void sdram_strobeChipSelect(uint8_t devNum)
{

   if( (devNum == 0) || (devNum == SDRAM__ALL_DEVICES) )
   {
      clrpinPORT(SDRAM_FR_CS_PIN, SDRAM_FR_CS_PORT);
      setpinPORT(SDRAM_FR_CS_PIN, SDRAM_FR_CS_PORT);
   }
   //NYI
   //if( (devNum == 1) || (devNum == SDRAM__ALL_DEVICES) )
}

//This is no longer used...
//void sdram_strobeChipSelect_andDisableClock(uint8_t devNum)
//{}


void sdram_strobeClockEnable(void)
{
   setpinPORT(SDRAM_CKE_PIN, SDRAM_CKE_PORT);
   clrpinPORT(SDRAM_CKE_PIN, SDRAM_CKE_PORT);
}
#endif


// Commands are defined in sdram_general.h
//This assumes the command/address busses are uC->SDRAM
//  see also sdram_setupCommand()
void sdram_executeCommand(uint8_t devNum, uint8_t command)
{
   sdram_setupCommand(command);

   sdram_strobeChipSelect(devNum);
}

//This assumes the command/address busses are uC->SDRAM
void sdram_setupAddress(uint16_t address, uint8_t bankAddress)
{
   //A0-7 on ADDRL_PORT:
   SDRAM_ADDRL_PORT = (address)&0xff;

   //A8-An (An being the largest address-bit)
   writeMasked((address)>>8, SDRAM_ADDRH_MASK, SDRAM_ADDRH_PORT);

   //BA0-1:
   writeMasked((bankAddress<<SDRAM_BANKADDR_SHIFT), SDRAM_BANKADDR_MASK,
                                                      SDRAM_BANKADDR_PORT);
}

//This intentionally does not do CKE, DQMs, nor /CS...
// In general, it'll only be used by initAddrCmdIO()
// But can also be used in free-running...
void sdram_AddrCmdIO_AsOutputs(void)
{
   // /WE, /CAS, /RAS:
   setPORToutMasked(SDRAM_CMD_PORT, SDRAM_CMD_MASK);
   //A0-7:
   setPORTout(SDRAM_ADDRL_PORT);
   //A8-An (An being the largest address-bit)
   setPORToutMasked(SDRAM_ADDRH_PORT, SDRAM_ADDRH_MASK);
   //BA0-1
   setPORToutMasked(SDRAM_BANKADDR_PORT, SDRAM_BANKADDR_MASK);
}

//Initialize the SDRAM-related Address/Command IOs with default-values
// These are, in many cases, the same as the resistor-pulled values
void sdram_initAddrCmdIO(void)
{
   //TODO: pull-resistors are NYI in this early sTZ "CKE Test"...

   //For each pin: 
   // Set the value first
   // Then set it as an output

   //CKE
   // one shared amongst all chips
   // should be enabled
   // pulled-active during reset/programming
   //setpinPORT(SDRAM_CKE_PIN, SDRAM_CKE_PORT);
   sdram_clockEnable();
   setoutPORT(SDRAM_CKE_PIN, SDRAM_CKE_PORT);

   //Chip-Select[s]
   // should be disabled
   // pulled-inactive during reset/programming
   setpinPORT(SDRAM_FR_CS_PIN, SDRAM_FR_CS_PORT);
   setoutPORT(SDRAM_FR_CS_PIN, SDRAM_FR_CS_PORT);

   //Data-Masks
   // should disable all bytes
   // pulled-disabled during reset/programming
   //writeMasked(SDRAM_DQM_MASK, SDRAM_DQM_MASK, SDRAM_DQM_PORT);
   sdram_disableAllBytes();
   setPORToutMasked(SDRAM_DQM_PORT, SDRAM_DQM_MASK);

   //Let's make *certain* that DQM gets registered
   // could take a bit, if the device is bursting during uC-reset
   _delay_us(100);

   //For a Free-Runner/Side-Kick setup:
   //The remainder of the Address/Command pins are directly-wired to DQs
   // Thus, their directions and values will be changed as-needed.
   // (upon reset, unconfigured pins are inputs)
   // after Data-Masks disable the bytes, the SDRAM DQs will float
   //For a normal SDRAM-setup, it makes sense to set the associated ports
   // as outputs, and load NOP to the command-pins
   // (NOP will be inhibited via Chip-Selects, above)

   // /WE, /CAS, /RAS:
   // Load NOP: (SDR_nRAS_H | SDR_nCAS_H | SDR_nWE_H) == SDRAM_CMD_MASK
   //writeMasked(SDRAM_CMD_MASK, SDRAM_CMD_MASK, SDRAM_CMD_PORT);
   sdram_setupCommand(SDRAM_CMD__NOP); 
   //setPORToutMasked(SDRAM_CMD_PORT, SDRAM_CMD_MASK);

   sdram_setupAddress(0,0);
   //A0-7:
   //setPORTout(SDRAM_ADDRL_PORT);
   //A8-An (An being the largest address-bit)
   //setPORToutMasked(SDRAM_ADDRH_PORT, SDRAM_ADDRH_MASK);
   //BA0-1
   //setPORToutMasked(SDRAM_BANKADDR_PORT, SDRAM_BANKADDR_MASK);
   sdram_AddrCmdIO_AsOutputs();
}



//Stolen and simpl/mod-ified from sdrfr/4.00ncf4/sdrfr.c:sdrfr_init()
void sdram_init(uint8_t devNum)
{
   //why?
   uint8_t CLI_SAFE(oldI);

   //TODO: Do we need to consider pull-resistors for this early test?

   //Start the ports/pins to match their reset/pulled-states:
   sdram_initAddrCmdIO();

   //Initialization/Power-Up sequence per Micron 64mb data-sheet:
   //1) Simultaneously apply power to VDD and VDDQ --OK
   
   //2) Assert and hold CKE at logic LOW
   //    This is probably unnecessary, 
   //    the specs say it's OK to tie CKE high...
   sdram_clockDisable();

   //3) Provide a stable CLOCK signal... --OK
   
   //4) Wait at least 100us prior to issuing any command other than 
   //   Inhibit or NOP...
   //AND (now less-confusingly-worded):
   //5) within there, bring CKE High, issuing a few CMD_INHIBITS (or NOPs)
   _delay_us(50);
   sdram_clockEnable();
   //since nCS is high, CMD_INHIBITS are issued hereafter
   _delay_us(50);
   
   //6-13) These are all prepping-for/related-to loadModeRegister:
   //       (In fact, sdramThing4.0 and earlier called these loadModeReg()

   //6&7 make up precharge()...
   //6) Perform a PRECHARGE ALL command:   
   //7) Wait at least tRP (20ns!) ( included in precharge() )
   sdram_precharge(devNum, PRECHARGE_ALL); 

   //8&9 make up autoRefresh()...
   //8) Issue an AUTO REFRESH command:
   //9) Wait at least tRFC (66ns)
   sdram_autoRefresh(devNum);

   //10&11) Repeat 8&9:
   sdram_autoRefresh(devNum);

   //12) Now for the ACTUAL LMR command:
   //    FYI: (0)0 0000 0011 0111 (BA 00)
   //    a) Set BA1,0=0, Set Address=Mode
          sdram_setupAddress(   SDRAM_MODE__WRITEBURST
                              | SDRAM_MODE__PAGEBURST
                              | SDRAM_MODE__CASLATENCY(3), //Don't change
                              0);  //BankAddress == 0
   //    b) Send the LMR command
          sdram_executeCommand(devNum, SDRAM_CMD__LOAD_MODE_REGISTER);
   
   //13) Wait at least tMRD (2 SDRAM clocks)
       asm("nop;"); asm("nop;"); asm("nop;");

   //"At this point the DRAM is ready for any valid command."

   SEI_RESTORE(oldI);
}


void sdram_precharge(uint8_t devNum, uint8_t bankAddress)
{
   if(bankAddress == PRECHARGE_ALL)
      sdram_setupAddress((1<<10), 0);
   else
      sdram_setupAddress(0,bankAddress);

   sdram_executeCommand(devNum, SDRAM_CMD__PRECHARGE);

   //Per the initialization sequence (also valid here)
   //7) Wait at least tRP (~20ns)
   asm("nop;"); asm("nop;"); asm("nop;");
}

void sdram_autoRefresh(uint8_t devNum)
{
   sdram_executeCommand(devNum, SDRAM_CMD__AUTOREFRESH);
   //Per the initialization sequence (also valid here)
   //9) Wait at least tRFC (~66ns)
   asm("nop;"); asm("nop;"); asm("nop;");
}


void sdram_refreshAll(uint8_t devNum)
{
   uint16_t i;

   sdram_precharge(devNum, PRECHARGE_ALL);

   for(i=0; i<numRows; i++)
   {
      sdram_autoRefresh(devNum);
   }
}




void sdram_activateRow(uint8_t devNum, uint16_t rowNum, uint8_t bankNum)
{
   sdram_setupAddress(rowNum, bankNum);
   sdram_executeCommand(devNum, SDRAM_CMD__ACTIVE);
}

#if 0
//This doesn't make sense... the burst needs to be sync'd
// E.G. by stopping the clock immediately after the burst starts...
// We'll leave this to main.c for now...
void sdram_beginReadBurst(uint8_t devNum, uint8_t colNum, uint8_t bankNum)
{
   sdram_setupAddress(colNum, bankNum);
   sdram_executeCommand(devNum, SDRAM_CMD__READ);
}
#endif //0


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
 * /home/meh/_avrProjects/sdramThingZero/21-gittifying/sdram_general.c
 *
 *    (Wow, that's a lot longer than I'd hoped).
 *
 *    Enjoy!
 *    --Esot.Eric (aka EricWazhung)
 */
