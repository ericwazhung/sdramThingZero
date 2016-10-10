/* mehPL:
 *    This is Open Source-ish, but NOT GPL. I call it mehPL.
 *    I'm not too fond of long licenses at the top of the file.
 *    Please see the bottom.
 *    Enjoy!
 */


//Stolen from sdramThing4.0-15


#ifndef __PINOUT_H__
#define __PINOUT_H__

//This should be moved to the makefile...
//#define HEARTBEAT       PB6       //(MISO)
//#define HEARTBEATPIN    PINB
//#define HEARTCONNECTION LED_TIED_HIGH


//Using the programming-header for debugging uart...
//
//
// 1  GND
// 2  V+
// 3  SCK   PB7   Rx0   (puar)
// 4  MOSI  PB5   Tx0   (puat)
// 5  /RST
// 6  MISO  PB6   (Heart)

//Rx is disabled...
//#define Rx0pin    PB7      //SCK
//#define Rx0PORT   PORTB

//Instead of using the default (on MOSI), use one of the available pins
//which otherwise would be used as a Side-Kick DQ
#define Tx0pin    PD5   //PB5      //MOSI
#define Tx0PORT   PORTD //PORTB

//Why was this here?
//#include "initialNotes.c"


// SDRAM: (stolen from initialNotes.c a/o sdramThing2.0-v13newish)

// CLK is tied directly to the crystal oscillator
// PLEASE use a shielded cable for this... I'm using an old cable to a
// wifi-antenna from a laptop. Made a *huge* difference for reliability.

// Command/Address pins are wired back to SDRAM Data pins
// for Free-Running mode.
// Writing To/Reading From these data-pins occurs via 
// multi-step process, described elsewhere.

// These data-pins are on a different SDRAM chip (?) than those
// connected to the SDR_DATA_PORT, but these two chips share CS, etc.
// Thus, they appear as a single 16-bit wide chip...
// (Heh, there's some confusion here... 
//   ADDR/BA1:0 is 14 bits
//   CMD (RAS, CAS, WE) are three bits
//   So there are at least 17 bits used by fed-back command/address pins.
//   
// As I recall, DQML and DQMH (a/o sdramThing2.0) select between these
// bytes, so e.g. data can be written to the pins tied to SDR_DATA_PORT 
// without interfering with data already stored for Free-Running mode.
//
// MORE PIECING TOGETHER:
// #define SDR_ADDRCMD_DQM_PIN (SDR_DQM_L_PIN)
// #define SDR_DATA_DQM_PIN    (SDR_DQM_H_PIN)
//
// So... These DQMs must be selecting between 32 bits
// Starting to remember: Addr/Cmd pins share three bytes (24 bits, several
// unused)
// And Data pins are one byte... then the "DQM_PIN"s here are tied to three
// DQM's on the DIMM, and one DQM, respectively.
//
// (For sdramThing3.0, this should mean that there are still 4 Bytes
// unused, on a separate chip-select, for use as our read/write "bank")

// So, as it stands, there is no sharing of data I/O pins on the AVR with
// multiple bytes on the SDRAM (DQM is only used to prevent writing to all
// bytes, as opposed to choosing which byte to read-back onto the data 
// port).

//#define SDR_nCS_PIN     PB3 //This is NOT fed-back to a SDRAM data-pin
//#define SDR_nRAS_PIN    PB2
//#define SDR_nCAS_PIN    PB1
//#define SDR_nWE_PIN     PB0

//#define SDR_CKE_PIN     PE0

// ADDRL                         ADDRH
// PORTA                         PORTC
// 0    1    2  3  4  5  6  7    0  1  2  3  4   5   6     7
// BA0  BA1  A0 A1 A2 A3 A4 A5   A6 A7 A8 A9 A10 A11 DQMAC DQMData

//#define SDR_BA0_PIN     PA0
//#define SDR_BA1_PIN     PA1
//#define SDR_DQM_L_PIN   PC6 // ADDRCMD_DQM_PIN
//#define SDR_DQM_H_PIN   PC7 // DATA_DQM_PIN

//#define SDR_DATA_PORT   PORTD


// This all needs to be verified...


#warning "Is PUAR/T implemented?! If so, CS0/1_Disable WILL interfere!"
//                             ATmega8515
//                            ____________________
//                           |         |_|        |
//                SDR_nWE  --|  1 PB0      VCC 40 |--  +3.3V
//               SDR_nCAS  --|  2 PB1      PA0 39 |--  SDR_BA0
// (AIN0)        SDR_nRAS  --|  3 PB2      PA1 38 |--  SDR_BA1
// (AIN1)         SDR_CS0  --|  4 PB3      PA2 37 |--  SDR_A0
//               SDR_CS1R  --|  5 PB4      PA3 36 |--  SDR_A1
// (PGM_MOSI)    SDR_CS1W  --|  6 PB5      PA4 35 |--  SDR_A2
// (PGM_MISO) +-/\/\/\-|>|---|  7 PB6      PA5 34 |--  SDR_A3
//  (PGM_SCK)  (H)SDR_CKE  --|  8 PB7      PA6 33 |--  SDR_A4
//              (PGM_RST)  --|  9 /RST     PA7 32 |--  SDR_A5
//  LCD_R               /  --| 10 PD0      PE0 31 |--  SDR_CS0_En (L)
//  LCD_G              |   --| 11 PD1      PE1 30 |--  SDR_CS1R_En (L)
//  LCD_DVHB SDRAM     |   --| 12 PD2      PE2 29 |--  SDR_CS1W_En+Trig (L)
//  LCD_CLK  Data     <    --| 13 PD3      PC7 28 |--  SDR_DATA_DQM
//  v4-7 Not Connected |   --| 14 PD4      PC6 27 |--  SDR_ADDRCMD_DQM (H)
//   PUAT(v3.5-11)      \  --| 15 PD5      PC5 26 |--  SDR_A11
//oneShotEnable(L)(v3.0:Tx)--| 16 PD6      PC4 25 |--  SDR_A10
//  anaButtons In ----+------| 17 PD7      PC3 24 |--  SDR_A9
//                    |    --| 18 XTAL2    PC2 23 |--  SDR_A8
//                   ---   --| 19 XTAL1    PC1 22 |--  SDR_A7
//                   ---   --| 20 GND      PC0 21 |--  SDR_A6
//                    |      |____________________| 
//                    V
//
// (L) = Pulled Low (~10k)
//
// (H) = Pulled High (~10k)

// SDR_ADDRCMD_DQM is pulled high so that when free-running and a reset
// occurs, free-running won't interfere with the programming-pins (and will
// eventually stop on its own, as no commands are fed-back)

// TODO: a/o sT3.5-1: The one-shot enable pin should also have a default
// value (pull-up/down) 
//
// Scope Trigger Output is connected to PE2, multiplexed with CS1W_En 
// (risky? Isn't there a better choice?)
#warning "Scope Trigger Output could cause an overwrite in data?!"
// It relies on the assumption that no CS1W commands occur in the
// free-runner during this time-period
// I THINK the old notes about being able to repurpose pins were about
// driving them *from* the free-runner... That might be a better idea,
// anyhow... (e.g. when CS1/0 are inactive)
// So Far: Haven't (seemed to) lost sampled data due to this, but it seems
// plausible.
// a/o v14: See the note in initialNotes.c before sdr_enableCS_forMCU()
// Roughly: The CS1W_En pin is repurposed for trigger-output WHEN NOT YET
// FREE-RUNNING.
// (Only CS1R[_En] are used for uC->Side-Kick commands)
//
// LCD_R and LCD_G are connected to two of the "Side-Kick's" DQ pins
// LCD_DVHB and LCD_CLK are connected to two otherwise-unused "Free-Runner"
//   DQ pins so that the LCD timing information won't be overwritten when 
//   sampling

//  LCD_DVHB and LCD_CLK have been wired to the two remaining DQ's on the
//  SDR_CMD_DQ's such that when the SideKick samples, it only samples on
//  the Red and Green pins.
//
// TODO: CS0/1_Disables should be pulled-high
// This'll cause Free-Running to stop when the processor is reset
// (e.g. during programming, which fails if FR is running...)
// (Or something with Reset?)
// This should be fixed by pulling SDR_ADDRCMD_DQM high, now.




// TODO: In order to save pins, now that the device is using two separate
// chips for data/addrCmd, could probably remove DQM? NOOOO Need these to
// read/write (at least from the Free-Runner)


//a/o sdramThing3.0-0.5 the pinout has changed yet again...
// the below is probably not quite accurate.
// NOTE that the new glue-arrangement makes CS0/CS1R/CS1W ACTIVE HIGH


// Each Command/Address-pin is fed-back from a data-pin on the SDRAM 
//  (except nCS, DQMs, and CKE. 
//   nCS and CKE are only controlled by the uC and are shared amongst all 
//      SDRAM chips
//   DQM inputs are only driven by the uC, but are grouped according to the
//      purpose/configuration of the associated DQ pins)
// For "fed-back" Command/Address pins, the corresponding DQ doesn't
//  matter, as long as they're all associated with the same DQM pins.
// The order of the Address pins connected to the uC *does* matter, as the
//  address bits serve multiple purposes (e.g. "Load Mode Register", also
//  A10 has a special purpose in some cases).
//         ______________________________________
//        /                                      \                //
//        |      __________________________      |
//        |     |  SDRAM  |                |     |
//        |     |---------                 |     |
//        | x17 |                 ADDR_CMD | x17 |
//        +---->| Cmd/Addr/BA           DQ |><---/
//        |     |   in                 I/O | (24-17=7 DQs are tied to GND)
//        |     |                          | ( Probably not ideal... better
//        |     |                          |   to pull them down via
//        |     |      /CS     ADDRCMD_DQM |   resistors, eh?)
//        |     |       in          ins    |
//        |     |__________________________|
//        |              ^         ^ ^ ^
//        |              |         | | |
//        |              |         \-+-/
//        v              |           |
//    x17 ^              ^           ^
//    --------------------------------------------------------------
//   | Cmd/Addr/BA      /CS       ADDRCMD_DQM              |   uC   |
//   |   out            out          out                    --------|
//   |    +                                                         |
//   | Free-Running                                                 |
//   | Cmd/Addr/BA                                                  |
//   | R/W I/O
//
//
// SDRAM Data pins are available for any purpose. in this case four are
// used to drive an LVDS-LCD display, when the SDRAM is in Free-Running
// mode:
//
//               ______________________       
//              |  SDRAM  |            | x8           x8  (x4 for LCD)
//              |---------     Data DQ |><------+-------> To LCD, etc.    
//              |                  I/O |        |
//              |                      |        |
//              | /CS        DATA_DQM  |        |
//              |  in           in     |        |
//              |______________________|        |
//                 ^             ^              |
//                 |             |              |
//                 |             |              v
//                 ^             ^              ^ x8
//     --------------------------------------------------------------
//    |           /CS         DATA_DQM       Data R/W       |   uC   |
//    |           out           out            I/O           --------|
//    |                                                              |
//

// a/o sdramThing3.0-1:
// The idea is to move toward the ability for one "bank" of SDRAM chip(s) 
// to control (via free-running) another "bank" of chip(s) 
//  (a "bank of chips" on a DIMM PCB, as opposed to a bank *within* a chip,
//  selected by a bank-address)
//
// The Free-Running bank contains commands to control itself *and* a
// separate bank. Commands to the second bank could be, e.g. "Write", in
// order to create a logic-analyzer (16Million samples per available DQ, at
// the sampling-rate determined by the SDRAM's clock-speed)

// This information should be moved to another file:
//
// In order to run a simple output-only Free-Running mode, on a
// single-bank (sdramThing2.0), we have each memory page filled as:
//
//   "P     A R" -> "P    A R" -> ...
// (P = Precharge the last memory-bank)
// (A = Activate the next bank/row)
// (R = Read the next bank/col)
// (' ' = NOP)
//
// In sdramThing1.0/2.0, the (single) Chip-Select pin is controlled
// directly by the microcontroller.
//
// In a double-bank system we have two separate Chip-Select pins, which can
// be selected via the "Free-Runner"
// For the first implementation, the idea is basically to just move the
// data pins from the same bank to the second-bank, so the "P    A R" 
// commands could actually be sent to *both* chips (both CS pins active)
//
// But, for the case of "Write" commands being sent to the second "bank"
// we need to have direct fed-back control of the separate CS pins.
//
// The "Free-Runner" (bank) will then contain commands such as:
//
// "P0 A1 W1    A0 R0" -> "P0 A1 W1     A0 R0" -> ...
//
// (where 0 indicates commands sent to the "Free-Runner" and 1 indicates
// commands sent to its "side-kick")
//
// Up to this point, no glue-logic was necessary for fed-back signals, and
// I was a bit proud of having figured out how to do that. But I don't see
// any way around a little glue in this case:
// The Chip-Select pins are used to indicate when the SDRAM chip should pay
// attention to a command, so when the microcontroller outputs the "write"
// command to the "Free-Runner" in order to load it with data, the
// microcontroller will have to activate the Free-Runner's CS pin. But the
// Free-Runner has to be able to activate its own CS pin, as well, via
// feed-back, much like the other command/address pins. But, in order to
// write a command into the Free-Runner to be later output during
// free-running mode, we can only have the CS pin active during the "write
// command" and *not* during the data (a command) being written into the
// Free-Runner (wow this is convoluted). 

// So we need a way to isolate the SDRAM's Chip-Select pin(s) from the
// data-pin which will later control the Chip-Select pin(s).
// In sdramThing(1.0), this sort of isolation was achieved via a resistor
// in the feedback-path. Unfortunately, this was found to limit the 
// operating-speed due to input capacitance.
//
// So for sdramThing2.0 we have:
//    
//         ______________________________________
//        /                                      \                //
//        |      __________________________      |
//        |     |  SDRAM 0 |               |     |
//        |     |----------                |     |
//        | x17 |                 ADDR_CMD | x17 |
//        +---->| Cmd/Addr/BA           DQ |><---/
//        |     |   in                 I/O |
//        |     |                          |
//        |     |                    DQ_CS |><---\                //
//        |     |      /CS             I/O |     |
//        |     |       in                 |     |
//        |     |__________________________|   | | ^
//        |           . ^ . . . . . .          | | |
//        |           . |        /| .         /  | |
//        |           . |       / | .    <---    | |
//        |           . +------<  |--------------+ |
//        |           . |       \ | .    <---    | |
//        |           . |       o\| .         \  | |
//        |           . |       |   .          | | |
//        |           . \--|<|--+   .          | | v
//        |           . . . . . | . .            |
//        v                     |                v    
//    x17 ^                     ^                ^
//    --------------------------------------------------------------
//   | Cmd/Addr/BA         CS_Disable           /CS        |   uC   |
//   |   out                                    out         --------|
//   |    +                                      +                  |
//   | Free-Running                        Free-Running             |
//   | Cmd/Addr/BA                          CS R/W I/O              |
//   | R/W I/O
//
// CS_Disable serves several purposes, first for Read/Write commands from
// the uC to the SDRAM, as described earlier. Also, it allows the uC to
// interrupt Free-Running Mode by discarding the CS sent by DQ_CS, which
// causes the associated Fed-Back command to be received as "NOP"
// (So the Free-Running Mode will exit after the interrupted burst 
//  completes). 
//
// The Buffer/Diode block is, essentially, an OR gate, and might well be
// implemented as such. I have some hesitation, as the DQ_CS output and
// /CS_out may both be floating in some circumstances, the effect of which
// I don't know. Further, I happen to have some high-speed 74AHC125s and
// all my OR gates are low-speed and rated for 5V. Further-still, the use
// of an output-enabled buffer in a case like this is a bit more
// common-place (and is actually how I came to this design, an earlier
// stage required *three* uC outputs!). 
//
//
// For the two separate banks of chips:
//
//         _______________________________
//        /                               \                       //
//        |      "Free-Runner"            |        "Side-Kick"
//        |      ___________________      |        _____________
//        |     |  SDRAM 0 |        |     |       |  SDRAM 1 |  |
//        |     |----------         |     |       |----------   |
//        | x17 |                   | x17 |       |             |
//        +---->| C/A/BA  C/A/BA_DQ |><---+------>| C/A/BA   DQ |><-
//        |     |   in              |             |   in    I/O |
//        |     |                   |             |             |
//        |     |            DQ_CS1 |><-------\   |             |
//        |     |                   |         |   |             |
//        |     |            DQ_CS0 |><--\    |   |             |
//        |     |      /CS          |    |    |   |        /CS  |
//        |     |       in          |    |    |   |         in  |
//        |     |___________________|    |    |   |_____________|
//        |             ^                |    |             ^
//        |             |                |    |             |
//        |             |        /|      |    |   |\        |
//        |             |       / |      |    |   | \       |
//        |             +------<  |------+    +---|  >------+
//        |             |       \ |      |    |   | /       |
//        |             |       o\|      |    |   |/o       |
//        |             |       |        |    |     |       |
//        |             \--|<|--+        |    |     +--|>|--/
//        |                     |        |    |     |
//        v                     |        v    v     |
//    x17 ^                     ^        ^    ^     ^
//    --------------------------------------------------------------
//   | Cmd/Addr/BA      CS0_Disable    /CS0  /CS1  CS1_Disable | uC |
//   |   out                                                    ----|


// SO To Write to the CMD/ADDR_DQ's:
// enable CS
// * deselect chip (CS inactive)
// * mask data (DQM active)
// * take the command-port
// * load WRITE command to the device's inputs (excluding CS)
// * strobe CS
//   (Now it's burst-writing, but the data is masked)
// * disable CS (CS0/1_Disable)
// * load the command to be written (including CS-values)
// * strobe DQM-inactive briefly
// ...

#warning "TODO: cmdToDataPort() (does it still exist?) Or its equivalent needs to be able to handle commands with CS0/1..."


// To Read from the CMD/ADDR_DQ's:
// enable CS
// * deselect chip (CS inactive)
// * mask data
// * take the command-port
// * load READ command to the device's inputs (excluding CS)
// * strobe CS
//   (Soon it will burst-read, but the data is masked)
// * disable CS
// * release the command-port
// * disable DQM
// * read the data on the CMD/ADDR pins (at the uC)
// * enable DQM
// ...

// To start Free-Running:
// Same as READ, up to and including "strobe CS"
// enable CS
// * deselect chip (CS inactive)
// * mask data
// * take the command-port
// * load READ command to the device's inputs (excluding CS)
// * strobe CS
//   (Soon it will burst-read, but the data is masked)
// * disable CS (The SDR will receive NOPs)
// * release the command-port 
//   (The SDR's command inputs may float, but are inhibited by CS_Disable)

//#error "release the data port!!!"

// * disable DQM
//   (Now the data is being output and fed-back to command-inputs,
//   exclusive of CS)
// enable CS
// ...

// To STOP Free-Running:
//  We need to take-back the cmd-port... (How?)
//  DQM so data doesn't overwrite the commands we intend to send...
//  Was:
//    CS inactive (but was an input)
//    NOP written to the cmd port (but was inputs)
//    take command (now NOP written, but Inhibited)
//    !!! BUT nothing's stopped FR's output! (sdramThing1.0 remnant?)
//    FR DQM (was Data DQM)
//    Burst-Terminate
//  This must not have been thoroughly-looked-over a/o sdramThing2.0
//  Something needs to DQM those command-feedback outputs!
//
//


//
// This same technique can be used to enable uC control over the
// "side-kick"'s received commands...
//
// This should be doable a/o 3.0-0.5
//#ifndef __ERROR_OVERRIDE_FOR_BUILD_TEST__
//#error "Precharge is necessary for Activate+Precharge Refresh!"
//#endif
// By removing the necessity for an explicit "Precharge" command in the
// Free-Runner (by using read/write with auto-precharge), the only commands
// required for Free-Running Mode are Read, Write, and Activate.
// Read and Write are identical except for the /WE pin, which is High for
// Read and Low for Write. Activate has /WE = High. So, by loading the
// Free-Runner with *write* commands to the "side-kick", the
// microcontroller can choose whether the side-kick receives Write or Read
// commands by using WE_Disable. 
// (Note that WE_Disable would affect Fed-Back commands from/to the
// Free-Runner because WE is connected to all devices on a DIMM. 
// But the Free-Runner only Feeds-Back Read and Activate commands to itself
// and these both have /WE=High)
//         _______________________________   
//        /                               \                          //
//        |      "Free-Runner"            |         "Side-Kick"
//        |      ___________________      |         _____________
//        |     |  SDRAM 0 |        |     |        |  SDRAM 1 |  |
//        |     |----------|        |     |        |----------|  |
//        | x16 |                   | x16 |    x16 |             |
//        +---->| C/A/BA  C/A/BA_DQ |><---+------->| C/A/BA   DQ |><-
//        |     |   in              |              |   in    I/O |
//        |     |                   |              |             |
//        |  /->| /WE         WE_DQ |><--\    /--->| /WE         |
//        |  |  |                   |    |    |    |             |
//        |  |  |___________________|    |    |    |_____________|
//        |  |                           |    |
//        |  +--------------------------------/
//        |  |                           |    
//        |  |                   /|      | 
//        |  |                  / |      | 
//        |  \----------+------<  |------+ 
//        |             |       \ |      | 
//        |             |       o\|      | 
//        |             |       |        | 
//        |             \--|<|--+        | 
//        |                     |        | 
//        v                     |        v 
//    x16 ^                     ^        ^ 
//    -------------------------------------------------------
//   | Cmd/Addr/BA       WE_Disable     /WE             | uC |
//   |   out                                            |----|
//

// TODO:
// Wiser might be a multiplexer...
// Have two WE outputs, if they're the same then it goes through regardless
// (I do still have a spare output-enabled buffer, and a single spare uC
// pin, to avoid needing an inverter...)
//
// The goal, e.g. to use this as a 'scopeable memory, do a single capture
// then have it repeatedly output to a scope, possibly have scroll and
// zoom, etc. (by changing the last FR Read command in a window to point to
// the beginning of the window) But somehow we'd have to keep refreshing
// the unviewed portion, e.g. via precharge-activate commands.

// a/o 3.0-0.7
// These schematics have changed again!!! (actually, before 0.7)
// /WE is *not* manipulated, as described above. 
// CS#_Disables have been replaced with CS#_Enables
// CS values are now Active HIGH
// This is the full schematic for all necessary glue-logic:
//
//     
//     /---------------------------\                                    //
//     |      "Free-Runner"        |             "Side-Kick"
//     |      _______________      |             _____________
//     |     |  SDRAM 0 |    |     |            |  SDRAM 1 |  |
//     |     |----------     |     |            |----------   |
//     | x17 |        C/A/BA | x17 |         17 |             |     Sample/
//     +---->| C/A/BA     DQ |><---+----------->| C/A/BA   DQ |><-- Repeat
//     |     |   in          |                  |   in    I/O |      I/O
//     |     |       DQ_CS1R |><----------\     |             |   
//     |     |               |            |     |             |
//     |     |       DQ_CS1W |><------\   |     |             |
//     |     |               |        |   |     |             |
//     |     |        DQ_CS0 |><--\   |   |     |             |
//     |     | /CS           |    |   |   |     |         /CS |
//     |     | in            |    |   |   |     |          in |
//     |     |_______________|    |   |   |     |_____________|
//     |       ^                  |   |   |                 ^
//     |       |                  |   |   |                 |
//     |       |                  |   |   |                 \---------\  //
//     |       | . . . . . .      |   |   |       . . . . . . . . . . |
//     |       | .   _____ .      |   |   |       . ____            . |
//     |       | .  /     |-------+   +------------|    \  ____     . |
//     |       \--O| NAND |.      |   |   |       .| AND |-\    \   . |
//     |         .  \_____|--\    |   |   |  /-----|____/   |    \  . |
//     |         . . . . . . |    |   |   |  |    . ____    | NOR O---/
//     |         1/2 74S51   |    |   |   +--------|    \   |    /  .
//     |                     |    |   |   |  |    .| AND |-/____/   .
//     |                     |    |   |   |  |  /--|____/           .
//     |             /-------/    |   |   |  |  | . . . . . . . . . .
//     |             |            |   |   |  |  |           1/2 74S51
//     |             |            |   |   |  |  \----------\             //
//     |             |            |   |   |  |             |
//     |             |            |   |   |  \-----\       |
//     v             |            v   v   v        |       |
// x17 ^             ^            ^   ^   ^        ^       ^
//  ---------------------------------------------------------------------
// | C/A/BA        CS0          CS0 CS1W CS1R     CS1W    CS1R      | uC |
// |  out          Enable                         Enable  Enable     ----|
//
// Original thought was to use a multiplexer instead of the 74S51, but I
// ran out of multiplexers, and actually this may be better, as it allows
// greater control over the "Side-Kick" (especially for commands being
// written to the Free-Runner that are active in both Read and Write modes,
// e.g. Activate and Precharge)
//
// Instead of switching the /WE pin for Read vs. Write, this allows for
// separate Write/Read commands stored in the Free-Runner for "Sample" vs.
// "Repeat" modes.
// This method also allows for Read and Write to occur at the exact same
// time in the Free-Running process (as Read is delayed by the CAS-Latency,
// and Write is not).
// The NAND gate used for CS0 is created by using the other half of the
// 74S51, with one of its ANDs connected such that it's always disabled.
// (one AND has its inputs tied low). 
//
// Re: 74S51
//
//  First attempt was with the 74LS51, since the LS series was found to
//  work with LCDdirectLVDS. No-go. (Were the propagation delays too long?)
//  74S51 seems to work fine at these low voltages, as well. (3.6V)
//  (Yes, the S-series is rated for 4.5-5.5V)

#endif

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
 * /home/meh/_avrProjects/sdramThingZero/21-gittifying/SDRAM_DIMM_Pinout/sT45pinout.h
 *
 *    (Wow, that's a lot longer than I'd hoped).
 *
 *    Enjoy!
 *    --Esot.Eric (aka EricWazhung)
 */
