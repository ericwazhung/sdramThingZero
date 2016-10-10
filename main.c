/* mehPL:
 *    This is Open Source-ish, but NOT GPL. I call it mehPL.
 *    I'm not too fond of long licenses at the top of the file.
 *    Please see the bottom.
 *    Enjoy!
 */



//#define DO_PRINT_UNEXPECTED   1

#include <avr/io.h>
#include <stdint.h>
#include <inttypes.h>
#include _BITHANDLING_HEADER_
#include _HEARTBEAT_HEADER_
#include "pinout.h"
#include "sdram_general.h"
#include <util/delay.h> //_delay_us()
#include _POLLED_UAT_HEADER_
#include <stdio.h>   //sprintf
#include <avr/pgmspace.h> //PSTR(), etc.
#include "RW_TimingLimitTesting.c"

#include "fr_config.h"
#include "sdram_oneShots.h"
#include "sdram_freeRunner.h"

//Large things like this I MUCH prefer to be global, so I can get a general
//idea of what the memory-usage is looking like...
char stringBuf[80];



uint8_t sdramFR_readAddrLByte(uint16_t row, uint16_t col)
{
   uint16_t addr; 
   sdramFR_readACD(0, row, col, NULL, &addr, NULL, 1); 
   return (uint8_t)(addr);
}




//What with unknown col-shifts, write to a column other than 0 to assure
//it's not wrapped-around in reverse, or something.
#define START  3

uint8_t generateNewByte(uint8_t byte, uint8_t phase)
{
   phase &= 0x07;

   //For now, rotation...
   byte = (byte<<phase) | (byte>>(8-phase));

   return byte;
}


void fillCols(uint16_t colStart, uint16_t colStop, uint8_t rev, 
               uint8_t dataShift, uint8_t phase)
{
   int16_t col;

   if(!rev)
   {
#define WRITES 32
      for(col=colStart; col<=(int16_t)colStop; )
      {
         //Despite the fact we're writing only uint8_t data, 
         // writeACD's dAddr expects a uint16_t array...
         uint16_t data[WRITES];
         uint8_t dataCount;
         uint16_t thisColStart = col;
         for(dataCount = 0; 
               (dataCount < WRITES) && (col<=(int16_t)colStop); 
               dataCount++)
         {
           data[dataCount] = generateNewByte(col+dataShift, phase+col/256);
           col++;
         }
         sdramFR_writeACD(0, 0, thisColStart, NULL, &data, NULL, WRITES);

         //sdramFR_writeAddrL(0, col, generateNewByte(col+dataShift,
         //         phase+col/256));
      }
   }
   else //write columns in reverse-order
   {
      for(col=colStop; col>=(int16_t)colStart; col--)
      {
         uint16_t data = generateNewByte(col+dataShift, phase+col/256);

         sdramFR_writeACD(0,0, col, NULL, &data, NULL, 1);
         //sdramFR_writeAddrL(0, col, generateNewByte(col+dataShift,
         //         phase+col/256));
      }
   }
}

uint8_t endLine(uint8_t function)
{
   uint8_t ret = function;
 
   if(ret)
   {
#if (defined(DO_PRINT_UNEXPECTED) && DO_PRINT_UNEXPECTED)
      puat_sendByteBlocking(0, '\n');
      puat_sendByteBlocking(0, '\r');
#endif
   }
   return ret;
}

uint32_t errPlus, errMinus, errCol, errOther;

void resetCounts(void)
{
   errPlus = 0;
   errMinus = 0;
   errCol = 0;
   errOther = 0;
}

//Uses colShift, so should only be called after detectColShift assigns it
uint8_t verifyCol(uint16_t col, uint8_t dataShift, uint8_t phase)
{
   uint8_t expect = generateNewByte(col+dataShift, phase+col/256);

   uint8_t read = sdramFR_readAddrLByte(0, col - colShift);

   if( read != expect )
   {
#if (defined(DO_PRINT_UNEXPECTED) && DO_PRINT_UNEXPECTED)
      sprintf_P(stringBuf, 
        PSTR("col %" PRIu16 " = %" PRIu8 ", expected %" PRIu8), 
         col, read, expect);
      
      puat_sendStringBlocking(0, stringBuf);
#endif

      if((read == col - colShift) || (read == col) || (read ==
               col-READ_COL_OFFSET))
         errCol++;
      else if(read == generateNewByte(col-1+dataShift, phase+col/256))
         errMinus++;
      else if(read == generateNewByte(col+1+dataShift, phase+col/256))
         errPlus++;
      else
         errOther++; 

      return 0;
   }
   else
      return 1;
}



void detectColShift(void)
{
   int16_t col;

   fillCols(100,200,FALSE,0, 3);

   for(col=125; col<175; col++)
   {
     if(sdramFR_readAddrLByte(0, col) == generateNewByte(150, 3))
        break;
   }

   if(col == 175)
   {
      sprintf_P(stringBuf, PSTR("Can't Detect colShift!!!\n\r"));
      puat_sendStringBlocking(0, stringBuf);
      while(1){}
   }



   colShift = (int8_t)((int16_t)150 - (int16_t)col);

   //Now that we've found our shift, let's verify it...
   sprintf_P(stringBuf, PSTR("# colShift = %" PRIi8 "\n\r"),
         colShift);
   puat_sendStringBlocking(0, stringBuf);

   fillCols(0,255,FALSE,127, 5);

   uint8_t error = FALSE;
   for(col=0; col<=255; col++)
   {
      if(endLine(!verifyCol(col, 127, 5)))
         error = TRUE;
   }

   if(error)
   {
      sprintf_P(stringBuf, PSTR("...Error\n\r"));
   }
   else
   {
      sprintf_P(stringBuf, PSTR("...Verified.\n\r"));
   }
   puat_sendStringBlocking(0, stringBuf);

}


#define DETECT_COLS  0
#define DETECT_ROWS  1
#define DETECT_BANKS 2
void detectDimension(uint8_t rowColBank)
{
   uint8_t highestBit; // = SDRAM_ADDR_BITS-1;
   uint16_t dataToWrite = 0;

   if(rowColBank == DETECT_BANKS)
      highestBit = SDRAM_BANKADDR_BITS-1;
   else
      highestBit = SDRAM_ADDR_BITS-1;

   //First, write something to the wraparound-point...
   //If, somehow *all* the bits are used, then we should read this value
   sdramFR_writeACD(0, 0, START, NULL, &dataToWrite, NULL,1);

   uint8_t safeHighBit = 0;
   int8_t highBit;

   //Start with the highest-possible address-bit set
   for(highBit = highestBit; highBit>=0; highBit--)
   {
      int16_t addr = (1<<highBit);
      
#if 1
      sprintf_P(stringBuf, PSTR("%s %"PRIu16" <-- %" PRIu8 "\n\r"), 
               (rowColBank) 
               ? (rowColBank == DETECT_ROWS)
                  ? "row" 
                  : "bank"
               : "col",
               addr, highBit);
#else
      //No decrease in .data/.bss, increased .text from 8152 to 8160!
      sprintf_P(stringBuf, PSTR("%S %"PRIu16" <-- %" PRIu8 "\n\r"), 
               (rowNCol) ? PSTR("row") : PSTR("col"),
               addr, highBit);
#endif
      puat_sendStringBlocking(0, stringBuf);
      
      uint16_t dataToWrite = highBit;

      uint8_t bank = 0;
      uint16_t row = 0;
      uint16_t col = START;

      switch(rowColBank)
      {
         case DETECT_ROWS: 
            row=addr;     
            break;
         case DETECT_COLS:
            col+=addr;
            break;
         case DETECT_BANKS:
         default:
            bank=addr;
            break;
      }

      sdramFR_writeACD(bank, row, col, NULL, &dataToWrite, NULL, 1);
      //If it's wrapped, we'll read-back from the same wrapped-address that
      //which we wrote... instead test the wraparound-point...
      uint8_t read;
      read = sdramFR_readAddrLByte(0, START-colShift);

      sprintf_P(stringBuf, PSTR("@0 read: %" PRIu8 "\n\r"), read);
      puat_sendStringBlocking(0, stringBuf);

      //Check if wraparound occurred this time 'round...
      if(read != highBit)
      {
         break;  
      }
   }
   
   safeHighBit = highBit;

   uint16_t things = (1<<(safeHighBit+1));

   switch(rowColBank)
   {
      case DETECT_COLS:
         colsPerPage = things; 
         break;
      case DETECT_ROWS:
         numRows = things; 
         break;
      case DETECT_BANKS:
      default:
         numBanks = things; 
         break;
   }

   sprintf_P(stringBuf, PSTR("# %" PRIu8 " %s addr bits\n\r"
                             "= %" PRIu16 " %s\n\r"),
            safeHighBit+1, 
            (rowColBank) 
            ? (rowColBank == DETECT_ROWS)
               ? "row" 
               : "bank"
            : "col",
            things,
            (rowColBank)
            ? (rowColBank == DETECT_ROWS)
              ? "rows"
              : "banks"
            : "col/page");
   puat_sendStringBlocking(0, stringBuf);
  /* 
   switch(rowColBank)
   {
      case DETECT_COLS:
         sprintf_P(stringBuf, PSTR("= %" PRIu16 " col/page\n\r"),
             colsPerPage);
         break;
      case DETECT_ROWS:
         sprintf_P(stringBuf, PSTR("= %" PRIu16 " rows\n\r"), numRows);
         break;
      case DETECT_BANKS:
      default:
         sprintf_P(stringBuf, PSTR("= %" PRIu8 " banks\n\r"), numBanks);
         break;
   }
   puat_sendStringBlocking(0, stringBuf);
*/
}



void sdramFR_loadFreeRunnerLoop(void)
{
   uint8_t bank;
   uint16_t row;

   //Wait a minute... If starting-free-running takes more than P_COL cycles
   //before it's started, then the first page won't precharge the next...
#define P_COL  (256-A_COL_OFFSET-10) //10
#define A_COL_OFFSET    127 //Relatively arbitrary...
#define R_COL_OFFSET    2//3//2//10  //CAS-Latency + a buffer for testing.
   //Interesting test, when not in the right mindset to look at
   //timing-diagrams...
   // Insert one DIMM with 2-banks, and another with 4-banks of the same
   // row-width (2-banks 512 cols, 4-banks 256 cols)
   // When they execute the same number of row-advancements in a second,
   // then your R_COL_OFFSET is correct, and every column is being burst.


   //Normally, these'd go unchanged
   //But, when attempting to keep the address as constant-as-possible
   // they might be...
   uint16_t pCol = P_COL;
   uint16_t aCol = colsPerPage-A_COL_OFFSET;
   uint16_t rCol = colsPerPage-R_COL_OFFSET;
   //Which column should the next read-burst begin from?
   uint16_t dAddr_R = 0;

   uint16_t lastRow;
  
#ifdef FR_ROWS
   lastRow = (FR_ROWS - 1);
#else
   lastRow = numRows - 1;
#endif

   for(row = 0; row <= lastRow; row++)
   {  
      if(row%64 == 0)
         sdram_refreshAll(SDRAM_FR_DEVNUM);

     for(bank = 0; bank < numBanks; bank++)
     {
        uint8_t dBank;
        uint16_t dAddr;
        uint8_t dCmd;

        //In the interest of loading *all* memory-locations with the next
        //bank-address (for 'scoping, and/or for monitoring free-running
        //via the AVR's inputs)
#if (FR_PA__BANKS_AWAY != FR_READ__BANKS_AWAY)
#error "For this to work, FR_PA__BANKS_AWAY and FR_READ__BANKS_AWAY must be equal"
#endif
        dBank = (bank + FR_PA__BANKS_AWAY) & (numBanks - 1);

        //Do the same for the address
        // So, let's load it up that way now, and that'll be the only bit
        // that ends up changing between the PAR commands....
        // WAIT.
#warning "HERE"
        // READ a *column*, and *usually* read from the FIRST column...

        //For PRECHARGE: dAddr is Don't-Care
        //               EXCEPT for A10, which must be LOW.
        //For ACTIVATE: dAddr == The row to be activated
        //For READ: dAddr == The Column to begin the read-burst
        //                   NORMALLY: This would be column 0
        //                   BUT, there's no reason it *has* to be
        //                   (Read-Bursts wrap within a page)
        //          FOR NOW: we'll leave it at col0
        //EVERYWHERE ELSE: The address-bits are irrelevent to free-running
        // So, nice to have them show something...
        // So, have it show the next row-address
        dAddr = (dBank == 0) ? (row + 1) : row;

        //FOR NOW: Cause Free-Running to loop repeatedly
        if((row == lastRow) && dBank == 0)
            dAddr = 0;
#define READ_COL__EQUALS_ROW  TRUE
#if (defined(READ_COL__EQUALS_ROW) && READ_COL__EQUALS_ROW)
        //NOTE:
        // This relies HEAVILY on wraparound-math
        // Both here in these variables, as well as in the SDRAM's
        // column-addressing mechanisms.
        pCol = P_COL + dAddr;
        aCol = colsPerPage - A_COL_OFFSET + dAddr;
        rCol = colsPerPage - R_COL_OFFSET + dAddr;
        dAddr_R = dAddr;
#endif

        //First, "erase" the whole thing, most-importantly:
        // load INHIBITED NOPs everywhere.
        // Inhibited NOPs are useful because if there's a case where /CS is
        // somehow activated (strayly), it'll still just be NOP.
#define WRITE_REPEATEDLY(numCols)   (-(numCols))
#define WRITE_ARRAY(numCols)        (numCols)
        dCmd = SDRAM_CMD__NOP_INHIBITED;
        
        sdramFR_writeACD(bank, row, 0, &dBank, &dAddr, &dCmd, 
                                       WRITE_REPEATEDLY(colsPerPage));



#if (defined(ROWADDR_VISUAL) && ROWADDR_VISUAL)
#error "This isn't really useful anymore, and may interfere"
        //For something visual, and since we're having some issues with /CS
        //being fed-back (thus being scoped)
        dCmd = SDRAM_CMD__NOP;
        sdramFR_writeACD(bank, row, (row<<2) | bank, NULL,NULL, &dCmd, 1);
#endif

   //PRECHARGE the next-to-be-used bank
        //Now, load a PRECHARGE command to the last-used bank in the first
        //column...
        //"The bank(s) will be available for a sub-sequent row access some 
        // specified time ( t RP) after the PRECHARGE command is issued"
        // Micron tRP > 20ns 
        // 20MHz is 50ns/cycle
        // 100MHz is 10ns/cycle
        //So we should be able to call Activate within 2-3 cycles after
        //precharge, on the same bank!
//moved above
//        dBank = (bank+FR_PA__BANKS_AWAY)&(SDRAM_NUM_BANKS-1);
        //Clear A10, but leave it for later (Activate)
        uint16_t dAddr_P = dAddr & ~(1<<10); 
        dCmd = SDRAM_CMD__PRECHARGE;

        //This command is NOT inhibited
        uint8_t cmdNull = SDRAM_CMD__NOP;

//#warning "NOTE: This ain't quite right..." 
//Or maybe it is...
        //As-Implemented, we'll get two back-to-back non-inhibited commands
        // The first will be at P_COL from P_COL-1's /CS
        // The second will be at P_COL+1 from P_COL's /CS
        // (at P_COL+1, we've "erased" it to NOP)
        // (And if the register's removed...? NOP at P_COL-1, CMD at P_COL)
        sdramFR_writeACD(bank, row, pCol, &dBank, &dAddr_P, &dCmd, 1);
        //Chip-select goes through a register, causing it to be delayed by
        //one clock-cycle.
        //Thus, we'll strobe /CS in one column *earlier* than where the
        //address/command is stored.
        //Set up *everything* except the command
        // (Really, all we need is the /CS)
        // (And, really, it could be the regular PRECHARGE command, since
        //  it would be inhibited anyhow... right?)
        sdramFR_writeACD(bank, row, pCol-1, &dBank, &dAddr_P, &cmdNull, 1);
   //REACTIVATE that bank at the next location
        //Now load the ACTIVATE command to the next-bank/row as appropriate
        //"After a row is opened with the ACTIVE command, a READ or WRITE
        // command can be issued to that row, subject to the t RCD 
        // specification."
        // Micron tRCD > 20ns
        //"The minimum time interval between successive ACTIVE commands to
        // the same bank is defined by t RC."
        // (And that must include a PRECHARGE first...)
        // Micron tRC > 66ns
        //"The minimum time interval between successive ACTIVE commands to
        // different banks is defined by t RRD."
        // Micron tRRD > 12ns
//moved above
//        dBank = (bank+FR_PA__BANKS_AWAY)&(SDRAM_NUM_BANKS-1);
        //Which row...?
        //Continue in this same row until the banks are finished...
//moved above
//        dAddr = (dBank == 0) ? row + 1 : row;

        

        dCmd = SDRAM_CMD__ACTIVATE;

        sdramFR_writeACD(bank, row, aCol, &dBank, &dAddr, &dCmd, 1);
        sdramFR_writeACD(bank, row, aCol-1, &dBank, &dAddr, &cmdNull, 1);

        //READ the next bank
        //Now load the READ(burst) command to the next bank, col 0

//moved above
//        dBank = (bank+1)&(SDRAM_NUM_BANKS-1);
//#else
//        dBank = (bank+1)&(1);
//#endif
        
        //Burst From Column 0
//moved above
//        dAddr = 0;
        
        dCmd = SDRAM_CMD__READ;

        sdramFR_writeACD(bank, row, rCol, &dBank, &dAddr_R, &dCmd, 1);
        sdramFR_writeACD(bank, row, rCol-1, &dBank, &dAddr_R, &cmdNull, 1);
        //dCmd = SDRAM_CMD__INHIBIT | SDRAM_CMD__READ;


        //Mwahahahaha
        // Technically, an address that's out of bounds, both row or col
        // will wrap... so we should get a FreeRunning Loop... (?)
        //Apparently not... and other oddities.

     }
   } 

}


int main(void)
{

   sdramFR_init();

   //Heh, despite TCNTER_IN_MAIN, heart_update() seems to be all that's
   //necessary.
   heart_init();

   puat_init(0);
   //puat_sendStringBlocking_P(0, stringBuf, PSTR("Boot\n\r"));
   puat_sendStringBlocking(0,"Boot\n\r");


   tcnter_t startTime; // = tcnter_get();

   int16_t col;
   //Allow old values to be tested once...
   //for(col=0; col<256; col++)
   //   sdramFR_writeAddrL(0, col, col+100);

   col = 0;

   detectColShift();
   detectDimension(DETECT_COLS);
   detectDimension(DETECT_ROWS);
   detectDimension(DETECT_BANKS);

   //4 banks...
   // We've only measured one byte... there are 8 bytes (64bits)
   //(This assumes a single-sided DIMM)
   uint32_t size = (uint32_t)numRows 
                  * (uint32_t)colsPerPage
                  * (uint32_t)numBanks
                  * (uint32_t)8; //*64 bits * 1byte/8bits

   sprintf_P(stringBuf, PSTR("%"PRIu32" bytes = %"PRIu16" MB\n\r"), size,
            (uint16_t)(size/1024/1024));
   puat_sendStringBlocking(0, stringBuf);
#define TRY_FR 1
#if (defined(TRY_FR) && TRY_FR)
   sprintf_P(stringBuf, PSTR("Loading FreeRunner Loop...\n\r"));
   puat_sendStringBlocking(0, stringBuf);

   //Can't do this, there's no tcnter_update() inside!
   //TODO: tcnter update in interrupt as an option...?
   //tcnter_t startTime = tcnter_get();
   sdramFR_loadFreeRunnerLoop();
   
   sprintf_P(stringBuf, PSTR("...done\n\r"));
   puat_sendStringBlocking(0, stringBuf);


   sprintf_P(stringBuf, PSTR("Starting FR: %" PRIu16" rows\n\r"),
#ifdef FR_ROWS
         FR_ROWS
#else
         numRows
#endif
            );

   puat_sendStringBlocking(0, stringBuf);
//#if (defined(SHORT_BURST) && SHORT_BURST)
//HACK, trying to just get two banks running...
   //sdram_activateRow(SDRAM_FR_DEVNUM, 10, 1);
   //sdram_activateRow(SDRAM_FR_DEVNUM, 20, 2);
   //sdram_activateRow(SDRAM_FR_DEVNUM, 30, 3);
//#endif
//   sdram_activateRow(SDRAM_FR_DEVNUM, 0, 1); 
   sdramFR_startFreeRunning();


   uint32_t frBankCount = 0;
   uint8_t lastRead = 0;
   tcnter_update();
   startTime = tcnter_get();
   while(1)
   {
      //BankAddress and AddrH on the same port.
      // Ridiculous
      // Too many transitions, maybe, to be caught.
      //uint8_t readAddrH = PIN_FROM_PORT(SDRAM_BANKADDR_PORT);
      uint8_t readAddr = PIN_FROM_PORT(SDRAM_ADDRL_PORT);

      //Attempt to filter-out false-transitions that might be detected when
      //sampling a PAR command
      //if(( (readAddrH - lastRead) & 0xC0) == 0x40)
      if(readAddr != lastRead)
      {
         //This is in no way accurate...
         // Could be sampling transitions from PAR commands
         // Could be too slow to catch every transition...
         frBankCount++;
         lastRead = readAddr;
      }

      heart_update();
      if(tcnter_isItTime(&startTime, TCNTER_SEC))
      { 
         sprintf_P(stringBuf, PSTR("%" PRIu32 "\n\r"), frBankCount);
         puat_sendStringBlocking(0, stringBuf);
         if(frBankCount == 0)
            break;
         frBankCount=0;
      }
   }
   
   sdramFR_stopFreeRunning();
#endif

   uint8_t dataShift = 0;

   uint16_t errCount = 0;
   uint16_t errMax = 0;

   uint32_t readCount = 0;

   uint8_t phase = 0;
   resetCounts();

   while(1)
   {
      //if(tcnter_isItTime(&startTime, TCNTER_SEC/(STOP-START)))
      {
         static uint8_t rev = 0;

         if(col == 0)
         {
            if(errCount > errMax)
               errMax = errCount;

#if (!defined(DO_PRINT_UNEXPECTED) || !DO_PRINT_UNEXPECTED )
            sprintf_P(stringBuf, PSTR("  %c:%3" PRIu8 " /%" PRIu32 
                     " p:%" PRIu8
                     " e:%3" PRIu16 " /%" PRIu16),
                  rev ? 'r' : 'f', dataShift, readCount,
                  phase, 
                  errCount, errMax);

            puat_sendStringBlocking(0, stringBuf);

            
            sprintf_P(stringBuf, PSTR(
                     " M:%" PRIu32 " P:%" PRIu32 
                     " C:%" PRIu32 " O:%" PRIu32 "    \r\r\r" ), 
                  errMinus, errPlus, 
                  errCol, errOther );

#else
            sprintf_P(stringBuf, PSTR("  %c: %" PRIu8 "  \r\r\r"), 
                  rev ? 'r' : 'f', dataShift );
#endif
            puat_sendStringBlocking(0, stringBuf);

            errCount=0;
            //Automatically wraps to 0 from 255...
            dataShift++;
            
            if(dataShift == 0)
            {
               rev=!rev;
               if(rev)
               {
                  phase++;
                  phase &= 0x07;
               }
            }
            
            //static uint8_t rev = 0;
            fillCols(0,colsPerPage-1, rev, dataShift, phase);
            
         }
         
         readCount++;

         if(!verifyCol(col, dataShift, phase))
         {
            errCount++;

#if (defined(DO_PRINT_UNEXPECTED) && DO_PRINT_UNEXPECTED)
            puat_sendByteBlocking(0, rev ? 'r' : 'f');
            endLine(1);
#endif
         }

         col++;
         if(col > colsPerPage-1)
            col = 0;
      }
      

      heart_update();
   }


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
 * /home/meh/_avrProjects/sdramThingZero/21-gittifying/main.c
 *
 *    (Wow, that's a lot longer than I'd hoped).
 *
 *    Enjoy!
 *    --Esot.Eric (aka EricWazhung)
 */
