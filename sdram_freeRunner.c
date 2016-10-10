/* mehPL:
 *    This is Open Source-ish, but NOT GPL. I call it mehPL.
 *    I'm not too fond of long licenses at the top of the file.
 *    Please see the bottom.
 *    Enjoy!
 */


#include "sdram_freeRunner.h"
//WITH stdio.h included elsewhere, this doesn't increase code-size...
// Without...?
#include "stdlib.h" //NULL ?! //not choosing stdio.h in case I decide to
                    //remove it for space...
                    // Other possibilities: stddef.h (not AVR)
                    // string.h....?
//#define __need_NULL
#include "RW_TimingLimitTesting.c"  //COL_OFFSETs


//Many of these functions are wrappers for the more general-purpose
//sdram_general functions... Except that, some of these functions must be
//handled specially for the Free-Runner, taking-into-account the fact that
//its DQs are directly-wired to the Command/Address inputs
// As Well As taking into account the necessity for one-shots.



//This should no longer be necessary, as the use of CKE (and extensive
//analysis/planning of the timing-diagrams) should assure that the
//col-shift is *always* zero. But, there may be potential... somewhere in
//there...
int8_t colShift = 0;



void sdramFR_init(void)
{
   //For sdram_ functions (especially _init(), at this point)
   // the CKE one-shot needs to be bypassed
   sdramFR_bypassCKE_OneShot();
   
   //For sdram_ functions the /CS one-shot needs to be active
   sdramFR_unoverrideCS_OneShot();
   
   setoutPORT(CKE_ONESHOT_BYPASS_PIN, CKE_ONESHOT_BYPASS_PORT);
   setoutPORT(CS_ONESHOT_OVERRIDE_PIN, CS_ONESHOT_OVERRIDE_PORT);
   
   sdram_init(SDRAM_FR_DEVNUM);
}  

void sdramFR_AddrCmdIO_AsInputs(void)
{        
   
   // /WE, /CAS, /RAS:
   setPORTinMasked(SDRAM_CMD_PORT, SDRAM_CMD_MASK);
   //A0-7:
   setPORTin(SDRAM_ADDRL_PORT);
   //A8-An (An being the largest address-bit)
   setPORTinMasked(SDRAM_ADDRH_PORT, SDRAM_ADDRH_MASK);
   //BA0-1
   setPORTinMasked(SDRAM_BANKADDR_PORT, SDRAM_BANKADDR_MASK);
 
   // Free-Runner's nCS_DQ
   setinPORT(SDRAM_FR__nCS_DQ_PIN, SDRAM_FR__nCS_DQ_PORT);
}

// The free-runner has a few more pins which need to be switched
void sdramFR_AddrCmdIO_AsOutputs(void)
{
   // /WE, /CAS, /RAS, A0-An, BA0-1
   sdram_AddrCmdIO_AsOutputs();
   
   // Free-Runner's nCS_DQ
   setoutPORT(SDRAM_FR__nCS_DQ_PIN, SDRAM_FR__nCS_DQ_PORT);
}  

//TODO: Would probably be better to remove all need for this...
void delay(void)
{
   //whelp, we've already got a jump and return...
   asm("nop");
}


//This procedure is internal to sdram_freeRunner...
// Used for sdramFR_readACD(), writeACD(), and sdramFR_startFreeRunning()
void sdramFR_prepRW(uint8_t bank, uint16_t row, uint16_t column, 
                                                      uint8_t command)
{
   //Assuming the Command/Addr IOs are currently outputs:

   //Mask the SDRAM's data-bits (DQs):
   // READ: 
   //  A) Don't allow a previous burst, etc. to interfere with 
   //     Command/Address output by the uC
   //  B) Hold-off the new burst-data output until we have a chance to 
   //     turn the uC's Command/Address pins into inputs
   //WRITE:
   //  A) (Same as above)
   //  B) Don't allow the Command/Address output by the uC to be stored
   //     into the memory, wait until the *data* has been set-up by the uC
   //
   //(Is this not part of the above assumption?)
   sdram_disableAllBytes();   //1

   //Activate the selected row on the selected bank
   sdram_activateRow(SDRAM_FR_DEVNUM, row, bank);


   //Prepare the read/write-burst (masked) starting at the selected column
   //NOTE: Read and Write may have different column-offsets!
   // Those need to be supplied in the column argument to prepRW()
   //(E.G. due to masking during the initial write-burst command, the
   // actual column written will be later than that requested)
   sdram_setupAddress(column, bank); //2
   sdram_setupCommand(command);   //2

   //delay(); //???

   //The read/write-burst command will execute immediately after this
   // BUT, it will be repeated/restarted with each clock-cycle
   // UNTIL the clock-edge AFTER CKE is disabled.
   sdramFR_overrideCS_OneShot(); //3

   sdram_clockDisable();      //4

   sdramFR_enableCKE_OneShot();  //4i

   // /CS is still active
   //  Only the first, AFTER clockDisable, will be registered
   sdramFR_unoverrideCS_OneShot();  //4a

   //Unmask the data-bits 
   // This does *NOT* occur immediately!
   // (And, note, CKE is currently disabled)
   // READ: the DQM-Latency during read is TWO clock-cycles. 
   //       Thus, data should be available exactly two CKE strobes later
   //       (So, after this function-call, gotta remember to switch those
   //        pins to inputs, before strobing CKE!)
   // WRITE: There is no DQM-Latency during write
   //        BUT, again, CKE is disabled and will only be strobed whence
   //        the data is properly set up
   //TODO: I've *reordered* this a bit! Do we need new timing-diagrams?
   clrpinPORT(SDRAM_FR_DQM_PIN, SDRAM_DQM_PORT);  //6
}

/*
//This procedure is internal to sdram_freeRunner...
// Used for both sdramFR_readACD() as well as sdramFR_startFreeRunning()
void sdramFR_prepReadBack(uint8_t bank, uint16_t row, uint16_t column)
{
   sdramFR_prepRW(bank, row, column-READ_COL_OFFSET, SDRAM_CMD__READ); 

   


   
   

   
   
   //Set the fed-back Address/Command pins as inputs
   // (Also takes care of Free-Runner's nCS_DQ
   sdramFR_AddrCmdIO_AsInputs();  //5


}
*/




void sdramFR_readACD(uint8_t bank, uint16_t row, uint16_t column,
          uint8_t *dBank, uint16_t *dAddr, uint8_t *dCmd, uint16_t count)
{
   //sdramFR_prepReadBack(bank, row, column);
   sdramFR_prepRW(bank, row, column-READ_COL_OFFSET, SDRAM_CMD__READ);

   //Set the fed-back Address/Command pins as inputs
   // (Also takes care of Free-Runner's nCS_DQ
   sdramFR_AddrCmdIO_AsInputs();  //5


   //Assuming a CAS-Latency of 3...
   //(Right, two are necessary, otherwise still in CASL
   //  read-back floating bus from last col-assignment TESTED TRUE)
   // OR: was that due to DQM-Latency?
   sdram_strobeClockEnable();    //7,8

   //Mightaswell take advantage of the ol' bursts!
 while(count>0)
 {
   sdram_strobeClockEnable();    //9,A
   //Note that the strobe is delayed a couple SDRAM clock-cycles 
   delay();

   //Now there should be data on the ports...
   //Read Back ADDRL
   uint8_t addrLval = PIN_FROM_PORT(SDRAM_ADDRL_PORT);
   //ASSUMING ADDRH and BA are on the same port... (BA = bits 6,7)
   uint8_t addrH_BA_val = PIN_FROM_PORT(SDRAM_ADDRH_PORT);
   if(dAddr != NULL)
   {
      *dAddr = (uint16_t)addrLval
              | (uint16_t)((addrH_BA_val & SDRAM_ADDRH_MASK) << 8);

      dAddr++;
   }

   if(dBank != NULL)
   {
      *dBank = (addrH_BA_val >> SDRAM_BANKADDR_SHIFT);

      dBank++;
   }
// uint8_t cmdVal = PIN_FROM_PORT(SDRAM_CMD_PORT) & SDRAM_CMD_MASK;
   //And tack-on the Free-Runner's fed-back Chip-Select
   //Actually, for now, this is all properly-aligned, so just change the
   //MASK, above...
   if(dCmd != NULL)
   {
      *dCmd = PIN_FROM_PORT(SDRAM_FR__CMD_DQ_PORT) & SDRAM_FR__CMD_DQ_MASK;

      dCmd++;
   }


   count--;
 }

   //Mask the data-bits so we can take back the C/A IOs as outputs
   //setpinPORT(SDRAM_FR_DQM_PIN, SDRAM_DQM_PORT);
   sdram_disableAllBytes();   //B

   //But that won't be registered until the clock's enabled...
   sdramFR_bypassCKE_OneShot();  //C

   sdram_clockEnable();  //D
   //And there's a slight DQM-latency, right...?
   delay();

   //Now take back the Address/Command pins as outputs
   // (Leaving it in a known state)
   sdramFR_AddrCmdIO_AsOutputs();


   //Let's do a precharge-all just so we know where we're at...
   sdram_precharge(SDRAM_FR_DEVNUM, PRECHARGE_ALL);
   //sdram_setupAddress((1<<10), 0);
   //sdram_executeCommand(SDRAM_FR_DEVNUM, SDRAM_CMD__PRECHARGE);
   //asm("nop;"); asm("nop;"); asm("nop;");

}


//a/o v17-2ish... count is now *signed*
void sdramFR_writeACD(uint8_t bank, uint16_t row, uint16_t column,
          uint8_t *dBank, uint16_t *dAddr, uint8_t *dCmd, int16_t count)
{         

   sdramFR_prepRW(bank, row, column-WRITE_COL_OFFSET, SDRAM_CMD__WRITE);

   



   uint8_t doAdvancePointers = 1;

   if(count < 0)
   {
      count = -count;
      doAdvancePointers = 0;
   }

 while(count>0)
 {
   //Now we can write our data to be written to the memory to the ports
   //SDRAM_ADDRL_PORT = data; //5
   uint8_t bankData = 0;

   if(dBank != NULL)
   {
      bankData = *dBank;
      dBank += doAdvancePointers;
   }

   uint16_t addrData = 0;

   if(dAddr != NULL)
   {
      addrData = *dAddr;
      dAddr += doAdvancePointers;
   }

   //NOTE: 0 = SDRAM_CMD__LOAD_MODE_REGISTER
   // Instead, let's repurpose a NULL-pointer to load SDRAM_CMD_INHIBIT 
   //Instead2.0, let's use NOP, inhibited.
   // Then if somehow a stray /CS occurs, we don't load another command by
   // mistake (INHIBIT only defines that /CS = H, the other bits may
   // resemble another command)
   uint8_t cmdData = SDRAM_CMD__NOP_INHIBITED;

   if(dCmd != NULL)
   {
      cmdData = *dCmd;
      dCmd += doAdvancePointers;
   }

   sdram_setupAddress(addrData, bankData);
   sdramFR_setupFRcmd(cmdData);

   count--;

/*
   //Now the data's ready to be written... 
   //Data-In is immediate with DQM not-disabling the byte...
   //So, unmask the data-bis (will occur alongside CKE, right?)
   //TODO: This coulda happened prior, since the clock's stopped, right?
   clrpinPORT(SDRAM_FR_DQM_PIN, SDRAM_DQM_PORT);  //6
*/

   //Strobe CKE to advance to the next column and write the data
   //So, it should write to column 1, right?
   sdram_strobeClockEnable();  //7,8

 }

   //Remask the data bits
   //setpinPORT(SDRAM_FR_DQM_PIN, SDRAM_DQM_PORT);
   sdram_disableAllBytes();   //9

   //That'll register with the next CKE...
   sdramFR_bypassCKE_OneShot();

   sdram_clockEnable();       //A

   //Let's do a precharge-all just so we know where we're at...
   sdram_precharge(SDRAM_FR_DEVNUM, PRECHARGE_ALL);
   //sdram_setupAddress((1<<10), 0);
   //sdram_executeCommand(SDRAM_FR_DEVNUM, SDRAM_CMD__PRECHARGE);
   //asm("nop;"); asm("nop;"); asm("nop;");
}




void sdramFR_startFreeRunning(void)
{
   //Assuming we always free-run from "the origin"
//   sdramFR_prepReadBack(0, 0, 0);
   sdramFR_prepRW(0, 0, 0-READ_COL_OFFSET, SDRAM_CMD__READ);

   //Set the fed-back Address/Command pins as inputs
   // (Also takes care of Free-Runner's nCS_DQ
   sdramFR_AddrCmdIO_AsInputs();  //5


   //The read-burst has begun, internally, and has been paused
   // It will resume when CKE is enabled.

   //Chip-Select is not yet fed-back...
   //Turn the chip-select override output into an input
   setinPORT(CS_ONESHOT_OVERRIDE_PIN, CS_ONESHOT_OVERRIDE_PORT);
   //Already pulled-up...
   //clrpinPORT(CS_ONESHOT_OVERRIDE_PIN, CS_ONESHOT_OVERRIDE_PORT);
   //Now,
   // Since /CS out from the uC is HIGH, the one-shot-override pin
   //  is routed directly to the SDRAM's /CS input through the one-shot's
   //  NAND-gates
   // And, now, we've turned the uC's one-shot-override pin into an input
   // SO...
   //  Drive that with the /CS_DQ via a resistor...(?)

   //I THINK that's where we start it...
   // THIS is where it begins to differ from readACD()
   //But that won't be registered until the clock's enabled...
   sdramFR_bypassCKE_OneShot();

   sdram_clockEnable();

   //Should be running now, right...?
}

void sdramFR_stopFreeRunning(void)
{
   sdram_disableAllBytes();

   delay();

   sdramFR_AddrCmdIO_AsOutputs();

   setoutPORT(CS_ONESHOT_OVERRIDE_PIN, CS_ONESHOT_OVERRIDE_PORT);

   sdram_precharge(SDRAM_FR_DEVNUM, PRECHARGE_ALL);
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
 * /home/meh/_avrProjects/sdramThingZero/21-gittifying/sdram_freeRunner.c
 *
 *    (Wow, that's a lot longer than I'd hoped).
 *
 *    Enjoy!
 *    --Esot.Eric (aka EricWazhung)
 */
