/* mehPL:
 *    This is Open Source-ish, but NOT GPL. I call it mehPL.
 *    I'm not too fond of long licenses at the top of the file.
 *    Please see the bottom.
 *    Enjoy!
 */


THIS FILE copied from sdrfr/4.00ncf/NOTES/SDRAM_Pinout.txt
 And modified to show the current wiring... (a/o sT3.5-16)
THIS also pieced-together from the file [sdrfr.../]NOTES/PC133-128MB.txt
And again, stolen from sdramThing4.0-15 for sdramThingZero


Vss = GND
Vdd = 3V3

. = Data I/O (DQn)
^ = Address/Bank Address Input (An/BAn)
: = Clock Input (CKn)
* = Command/Control Input
# = Data Mask Input (DQMBn)
(n) = Not Connected and/or typically unused
<n> = Power/GND



NOTE: As-Implemented:
DQMB0, DQMB1, and DQMB4 are soldered-together
Likewise: CK0 and CK1 are also soldered-together

'51 indicates the associated output from the 74x51
OS indicates the associated output from a One-Shot...
Pins ending in, e.g. /---- or ----\
 indicate fed-back command/address DQs
Pins ending in, e.g. \---- or ----/
 indicate fed-back command/address pins
  NOTE: These are *also* connected to the microcontroller
   (A0-11, BA0-1, /RAS, /CAS, /WE = 17 directly-fed-back command/address
    pins)
The matching of a particular DQ (/----) with command/address pins (\----)
 doesn't really matter.
DQ_CSn 
 These are fed-back Chip-Select DQs, which go through the '51
 before being fed-back to the actual Chip-Select pins
 Again, which one goes where doesn't really matter (CS0, CS1R, CS1W)
 These are tied directly to the microcontroller, as well.

FREE-RUNNER:
 This consists of the lower-pinned half of the SDRAM DIMM
   (The left-side, when viewed upright from the front)
   (The top half of this drawing)
SIDE-KICK:
 This consists of the higher-pinned half of the SDRAM DIMM
   (The lower-half of this drawing)
There are 32 bits available to each half
 So far, the Side-Kick really only implements two
   LCD Red
   LCD Green
 Other DQ's on the side-kick HAVE been soldered-up to the uC
 But serve no purpose.
 They *could* be connected to the Terminal Block and used for sample/repeat
 (But not viewable on the LCD)
 Thus, the "Logic Analyzer" could easily sample/repeat up to 32 bits,
 simultaneously... when the DQMB pins are tied together)



v4:
This introduces the ability to modify the LCD's DVH/Blue signal *without*
modifying the LCD's clock or other Free-Running bits
By moving the DVH/Blue DQ to another chip, with a separate DQMB on DQM_DATA
For backwards-compatibility, DQMB5 can be connected via jumper to DQM_AC
v4.0 has it jumpered to DQM_DATA.

/*

                                         Front                Back
                                        ------------------------------
                                 /      <1 GND>              <85 GND>
                                 |
                                 |    **.2 DQ0----.     .----.86 DQ32------+----------->< [DQ]/WE
                                 |                |     |                  |
                                 |    **.3 DQ1    |     |    .87 DQ33------|-+--------->< [DQ]/CAS
                                 |                |     |                  | |
                                 |    **.4 DQ2    |     |    .88 DQ34------|-|-+------->< [DQ]/RAS
                                 |                |     |                  | | | 
                                 |    **.5 DQ3    |     |    .89 DQ35------|-|-|------->< [DQ]/CS_FR+SK_READ
                                 |                |     |                  | | | 
                                 |      <6 3V3>  B0~~+~~B4   <90 3V3>      | | |
                                 |                |  '  |                  | | |
                                 |    **.7 DQ4    |  '  |    .91 DQ36------|-|-|------->< [DQ]/CS_SK_WRITE (Sample)
                                 |                |  '  |                  | | |
                                 |    **.8 DQ5    |  '  |    .92 DQ37      | | |
                                 |                |  '  |                  | | |
                                 |    **.9 DQ6    |  '  |    .93 DQ38      | | |
                                 |                |  '  |                  | | |
                                 |   **.10 DQ7----*  '  *----.94 DQ39      | | |
                  Address DQ's  <                 .  '  .                  | | |
                                 |                .  '  .                  | | |
                                 |                .  '  .                  | | |
                                 |   **.11 DQ8---..  '  ..---.95 DQ40      | | |
                                 |               |.  '  .|                 | | |
                                 |     <12 GND>  |.  '  .|   <96 GND>      | | |
                                 |               |.  '  .|                 | | |
                                 |   **.13 DQ9   |.  '  .|   .97 DQ41      | | |
                                 |               |.  '  .|                 | | |
                                 |   **.14 DQ10  |.  '  .|   .98 DQ42      | | |
                                 |               |.  '  .|                 | | |
                                 |   **.15 DQ11  |.  '  .|   .99 DQ43      | | |
                                 |               |.  '  .|                 | | |
                                 |   **.16 DQ12 B1.~~+~~.B5 .100 DQ44      | | |
                                 |               |.  '  .|                 | | |
                                 |   **.17 DQ13  |.  '  .|  .101 DQ45      | | |
                                 |               |.  '  .|                 | | |
                                 |     <18 3V3>  |.  '  .|  <102 3V3>      | | |
                                 |               |.  '  .|                 | | |
                                 |   **.19 DQ14  |.  '  .|  .103 DQ46      | | |
                                 |               |.  '  .|                 | | |
                                 \   **.20 DQ15--*.  '  .*--.104 DQ47      | | |
                                                 ..  '  ..                 | | |
                                       (21 CB0)  ..  '  ..  (105 CB4)      | | |
                                                 ..  '  ..                 | | |
                                       (22 CB1)  ..  '  ..  (106 CB5)      | | |
                                                 ..  '  ..                 | | |
                                       <23 GND>  ..  '  ..  <107 GND>      | | |
                                                 ..  '  ..                 | | |
                                       (24 NC)   ..  '  ..  (108 NC)       | | |
                                                 ..  '  ..                 | | |
                                       (25 NC)   ..  '  ..  (109 NC)       | | |
                                                 ..  '  ..                 | | |
                                       <26 3V3>  ..  '  ..  <110 Vdd/3V3>  | | |
                                    .--------------------------------------' | |
                                    '--*27 /WE   ..  '  ..  *111 /CAS--------' |
                                                 ..  '  ..                     |
                               !!!     #28 DQMB0--'  '  '---#112 DQMB4  !!!    |
                                                 '   '   '                     |
                               !!!     #29 DQMB1-'   '   '--#113 DQMB5  !!!    |
                                    .-----------------------------------------------------< /CS_FreeRunner
                                    '--*30 /S0-------'      *114 /S1 (Unused)  |
                                                                               |
                                       (31 NC)              *115 /RAS----------'
                                 
                                       <32 GND>             <116 GND>                    
         
                                     **^33 A0               ^117 A1**                   
         
                                     **^34 A2               ^118 A3**                   
          
                                     **^35 A4               ^119 A5**                   

                                     **^36 A6               ^120 A7**         

                                     **^37 A8               ^121 A9**                   
      
                                     **^38 A10 (AP)         ^122 BA0**                 
                         
                                     **^39 BA1              ^123 A11**                 
      
                                       <40 3V3>             <124 Vdd/3V3>                
                                       
                                
                                
                                       <41 3V3>       /=====:125 CK1                  
                              _____                  /              
                      XTAL >--)----)---:42 CK0 =====/       ^126 A12**
                              ¯¯¯¯¯|
                                   '---<43 GND>             <127 Vss/GND>                

                                       (44 NC)              *128 CKE0   !!!
                                    .-------------------------------------------------------< /CS_SideKick
                                    '--*45 /S2-------.      *129 /S3 (Unused)
                                                     '
                           !!!         #46 DQMB2-.   '   .--#130 DQMB6  !!!
                                                 .   '   .
                           !!!         #47 DQMB3--.  '  .---#131 DQMB7  !!!
                                                 ..  '  ..
                                       (48 NC)   ..  '  ..  ^132 A13**
                                                 ..  '  ..
                                       <49 3V3>  ..  '  ..  <133 3V3>
                                                 ..  '  ..
                                       (50 NC)   ..  '  ..  (134 NC)
                                                 ..  '  ..
                                       (51 NC)   ..  '  ..  (135 NC) 
                                                 ..  '  ..
                                       (52 CB2)  ..  '  ..  (136 CB6)
                                                 ..  '  ..
                                       (53 CB3)  ..  '  ..  (137 CB7)
                                                 ..  '  ..
                                       <54 GND>  ..  '  ..  <138 Vss/GND>
                                                 ..  '  ..
                                       .55 DQ16--*.  '  .*--.139 DQ48
                                                 |.  '  .|
                                       .56 DQ17  |.  '  .|  .140 DQ49
                                                 |.  '  .|
                                       .57 DQ18  |.  '  .|  .141 DQ50
                                                 |.  '  .|
                                       .58 DQ19  |.  '  .|  .142 DQ51 
                                                 |.  '  .|
                                       <59 3V3>  |.  '  .|  <143 Vdd/3V3>
                                                 |.  '  .|
                                       .60 DQ20  |.  '  .|  .144 DQ52 
                                                 |.  '  .|
                                       (61 NC)  B2.~~+~~.B6 (145 NC)
                                                 |.  '  .|
                                       (62 NC)   |.  '  .|  (146 NC)
                                                 |.  '  .|
                           !!!         *63 CKE1  |.  '  .|  (147 NC)
                                                 |.  '  .|
                                       <64 GND>  |.  '  .|  <148 Vss/GND>
                                                 |.  '  .|
                                       .65 DQ21  |.  '  .|  .149 DQ53 
                                                 |.  '  .|
                                       .66 DQ22  |.  '  .|  .150 DQ54
                                                 |.  '  .|
                                       .67 DQ23--'.  '  .'--.151 DQ55
                                                  .  '  .
                                       <68 GND>   .  '  .   <152 GND>
                                                  .  '  .
                                       .69 DQ24---*  '  *---.153 DQ56 
                                                  |  '  |
                                       .70 DQ25   |  '  |   .154 DQ57 
                                                  |  '  |
                                       .71 DQ26   |  '  |   .155 DQ58 
                                                  |  '  |
                                       .72 DQ27   |  '  |   .156 DQ59 
                                                  |  '  |
                                       <73 3V3>  B3~~+~~B7  <157 Vdd/3V3>
                                                  |  '  |
                                       .74 DQ28   |     |   .158 DQ60 
                                                  |  '  |
                                       .75 DQ29   |     |   .159 DQ61 
                                                  |  '  |
                                       .76 DQ30   |     |   .160 DQ62 
                                                  |  '  |
                                       .77 DQ31---'     '---.161 DQ63 

                                       <78 GND>             <162 Vss/GND>

                           !!!         :79 CK2              :163 CK3    !!!

                                       (80 NC)              (164 NC) 

                                    /   81 WP                165 SA0        \

                                    |   82 SDA               166 SA1        | Both Sides:

                                    |   83 SCL               167 SA2        | For Serial Presence

                                    \  <84 Vdd/3V3>         <168 Vdd/3V3>   / Detect EEPROM


*/
/WE0: Dunno why they put 0 at the end, like there are other /WE pins... no.
CB7:0 = ECC Check Bits (usually unused)

ODDITIES I've run across:
* CS0 and CS2 as well as CS1 and CS3 *tied together* on the DIMM
   (Bad news, no? Tying driver outputs together?)
* Groups of chips connected with different methods depending on the signal
   (E.G. 8 chips, four connected to CS0, four to CS2
	               four connected to CK0, four to CK1
	 				   NOT THE SAME FOUR... i.e. Two of those connected to CS0
	 				   are connected to CK0, while the other two are on CK1)
* Four chips not consecutive
	(E.G. 8 chips on the same side: bytes 0, 4, 1, 5, 2, 6, 3, 7)
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
 * /home/meh/_avrProjects/sdramThingZero/21-gittifying/SDRAM_DIMM_Pinout/SDRAM_3_CommandPinout-stZ.c
 *
 *    (Wow, that's a lot longer than I'd hoped).
 *
 *    Enjoy!
 *    --Esot.Eric (aka EricWazhung)
 */
