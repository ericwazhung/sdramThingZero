/* mehPL:
 *    This is Open Source-ish, but NOT GPL. I call it mehPL.
 *    I'm not too fond of long licenses at the top of the file.
 *    Please see the bottom.
 *    Enjoy!
 */


//This file is the result of many iterations of testing...
// The CURRENT STATUS is that read and write are both setup such that the
// /CS signal is active both BEFORE AND AFTER the transition of CKE to low
// This results in *exactly* ONE read/write burst executed immediately as
// CKE is detected-low. THUS, the *last* read/write burst is paused,
// immediately, until CKE is later strobed.
// (/CS can go high any time before the first CKE strobe)
//
// CODE for the OLDER tests exists prior to sdramThingZero-19 and has been
// removed, since.


#define READ_COL_OFFSET 0
//           _1_______________________________6                                                             B____________________________
//      DQM: _/                               \_____________________________________________________________/    (DQM-Latency 2 clocks)
//           ___2___________________________5                                                                               vvvvvvvv
// CMD/ADDR:    X "READ BURST bank 0,col 0" >-----------------------------< col 0        >X< col 1                         ><c2><c3>----  
//           ¯¯¯ ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯ 
//                3>:v                4a
// /CS_OS_OVRD:¯¯¯\___________________/////¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
//                  :v                  : __:______________________________
//    (/CS):    ¯¯¯¯¯|___________________|___|
//                  :   : 4>:   :   :   :   :  7:   :  8:  9:   :  A:   :9'>: A':                                  D:
// uC_CKE_Out: ¯¯¯¯¯¯¯¯¯¯¯\____________________/¯\\\\\\\___/¯\\\\\\\_____/¯¯¯¯\____________________________________/¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
//                      :   :     4i            :           :               :                                   C   :
//CKE_OS_/BYPS:___________________//////////¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯\_________________
//                      :   :>>>:v              :>>>:v  :   :>>>:v  :   :   :>>>:v  :   :                           :>>>:v  :
//     (CKE): ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯|___________________|¯¯¯|_______|¯¯¯|___________|¯¯¯|___________________________________|¯¯¯¯¯¯¯¯¯¯¯¯
//                  :   :   :   :   :\  :\  :\  :\  :\  :\  :\  :\  :\  :\  :\  :\  :\  :\                              :\  :\
//      CLK:  |_|¯|_|¯|_|¯|_|¯|_|¯|_|¯|_|¯|_|¯|_|¯|_|¯|_|¯|_|¯|_|¯|_|¯|_|¯|_|¯|_|¯|_|¯|_|¯|_|¯|_|¯|_|¯|_|¯|_|¯|_|¯|_|¯|_|¯|_|¯|_|¯|_|¯|_|¯
//                      :   :   :   :  \:  \:  \:  \:  \:  \:  \:  \:  \:  \:  \:  \:  \:  \:                              \:  \:
// internal:  |_|¯|_|¯|_|¯|_|¯|_|¯|_|¯|_.___.___.___.___.___|¯|_.___.___|¯|_.___.___.___|¯|_.___.___.___.___.___.___.___.___.___|¯|_|¯|_|¯|_
//                      :   :   :   :\                      :\          :\              :\
//  Burst 1: ----------c1--c2--c3-<0m>|-(truncated)           |           |               |
//  Burst 2: --------------c1--c2--c3-|< 0m                  >|(truncated)|               |
//  Burst 3: ------------------c1--c2----------------------c3-|< 0m      >|(truncated)    |
//  Burst 4: ----------------------c1----------------------c2----------c3-|< 0d          >|< 1d ...
//    c# = CAS-Latency count, 
//    #m = data/column output *masked*
//    #d = data/column output available at DQs (attached to CMD/ADDR pins)
//
// NOTE: The *last* read-burst executed is that which coincides with the first clock-cycle when (CKE) is sampled low
//       This is the burst which results in data-output.

// NOTE: Need two CKE strobes before the first data is available, because DQM-Latency is two.
//       
// NOTE: Any number of SDRAM clock-cycles can occur between most steps (e.g. 1->2, 2->3)
//       Most must occur in this order, but some can occur simultaneously (e.g. 2->3)
//       (Plausibly, with some nitty-gritty knowledge, some of these could be *reordered*, 
//        when taking-into-account things like the delays introduced by the one-shot circuits)
//       Only *some* of these step-transitions have a *minimum* number of clock-cycles between them...
//       (none have a maximum, except in consideration of the fact that DRAM must be refreshed periodically)
// NOTE: 9' and A' show an unlimited number of additional CKE strobes, allowing for reading back multiple columns

// 1: Blocks data at DQ_ADDR/CMD from a previous burst
//     Takes two cycles
//     REALISTICALLY:
//      Should just never call read() without making sure the previous burst is already masked...
//   ALSO (more importantly):
//    Assures that e.g. Burst1's output doesn't interfere with the "READ BURST" command still on the ADDR/CMD pins
// 2: Set up the READ BURST command/address
//     This can happen simultaneously with 3
//     (Actually, 3 could occur one SDRAM clock prior, due to latch, correct?)
// 3: Activate the command (bring /CS low)
//     This is *extended* for MULTIPLE clock-cycles *on purpose*
//      Because it's easiest to do in a GPIO-setup
//     ALTERNATIVELY:
//      There could be some major reduction here...
//      if 3->/CS and 4->CKE are synchronized (use /CS one-shot)
//      BUT: BEWARE: Because SDRAM is *asynchronous* with host, it's plausible e.g. one signal-transition might arrive slightly later than another, 
//       right around a clock-edge.


















//OPTIONS:
//  R_TWO_CKE  - Strobe CKE twice (and read after the second) during read-cycles
//  R/W_CS_PREFIX_TEST  - enable /CS for Read/Write *before* the strobe...
//  R/W_CS_SUFFIX_TEST  - keep /CS active for Read/Write until *after* CKE is disabled

//  R_2CKE  W_PRE  R_PRE  W_SUF  R_SUF   Results
//  0       0      0      0      0       FAIL: Reads-Back column-address rather than DQ
//  1       0      0      0      0       GOOD: Mistaken timing-charts, but fixed, below.
//                                             READs col+1, WRITEs col+2
//  1       1      0      0      0       GOOD: READs col+1, WRITEs col+2
//  1       1      1      0      0       GOOD: READs col+1, WRITEs col+2
//  0       1      1      0      0       FAIL: Reads-back column-address rather than DQ
//  1       1      1      1      0       GOOD: READs col+1, WRITES col+1
//  1       1      1      1      1       GOOD: READs col+0, WRITES col+1


#if (  ( !defined( W_CS_PREFIX_TEST ) || !W_CS_PREFIX_TEST ) \
    && ( !defined( W_CS_SUFFIX_TEST ) || !W_CS_SUFFIX_TEST ) )

#define WRITE_COL_OFFSET 2
//TODO: Figure51 shows CKE registering low at the same edge as the WRITE
//      (Not required, but possible)
//
// THIS: Is how it's CURRENTLY implemented:
//              
//           ___1______________6             9_________
//      DQM: ___/              \_____________/
//           _____2__________5______________________________
// CMD/ADDR:      X   WRITE  X      DQ Data           
//           ¯¯¯¯¯ ¯¯¯¯¯¯¯¯¯¯ ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
//                    3 4
//      /CS: ¯¯¯¯¯¯¯¯¯\_/¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
//                     :4  :   :   :  7:8  :   : A :   :
//      CKE: ¯¯¯¯¯¯¯¯¯¯¯\_____________/¯\________/¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
//                     :   :\  :\  :\  :\  :\  :\  :\  :
//      CLK:  _|¯|_|¯|_|¯|_|¯|_|¯|_|¯|_|¯|_|¯|_|¯|_|¯|_|¯|_|¯|_|¯
//                         :  \:  \:  \:  \:  \:  \   \
// internal:  _|¯|_|¯|_|¯|_|¯|_.___.___.___|¯|_.___.___|¯|_|¯|_|¯
//
// Write Col: ---------0m  1m              2d          3m  4m  5m
//
// #m = column-write MASKED
// #d = column is written with DATA
//TODO: Above is WRONG... see RW_TimingLimitTesting.c
#endif


#if (  ( !defined( R_CS_PREFIX_TEST ) || !R_CS_PREFIX_TEST ) \
    && ( !defined( R_CS_SUFFIX_TEST ) || !R_CS_SUFFIX_TEST ) \
    && (  defined( R_TWO_CKE )        &&  R_TWO_CKE        ) )

#define READ_COL_OFFSET 1
//           ___1______________6                        B_________
//      DQM: ___/              \________________________/
//           _____2__________5
// CMD/ADDR:      X   READ   >-----------------------------       
//           ¯¯¯¯¯ ¯¯¯¯¯¯¯¯¯¯ 
//                    3 4
//      /CS: ¯¯¯¯¯¯¯¯¯\_/¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
//                     :4  :   :   :  7:8  :   :  9:A  :
//      CKE: ¯¯¯¯¯¯¯¯¯¯¯\_____________/¯\_________/¯\_______
//                     :   :\  :\  :\  :\  :\  :\  :\  :\
//      CLK:  _|¯|_|¯|_|¯|_|¯|_|¯|_|¯|_|¯|_|¯|_|¯|_|¯|_|¯|_|¯|_|¯
//                     :   :  \:  \:  \:  \:  \:  \   \   \
// internal:  _|¯|_|¯|_|¯|_|¯|_.___.___.___|¯|_.___.___|¯|_.____
//                     :   :               :\          :\
// Read Col: ---------c1--c2--------------c3-< 0m       >< 1d
//
// c# = CAS Latency count (CAS=3)
// 0m = DQM Latency is two clock-cycles...
// See Figure 52. It appears the CKE=L affects, data-wise, the *second* clock-edge
//   But, really, it's affecting the first, but the first causes the data-setup that's ready BY the second.
#endif




#if (  (  defined( W_CS_PREFIX_TEST ) &&  W_CS_PREFIX_TEST ) \
    && ( !defined( W_CS_SUFFIX_TEST ) || !W_CS_SUFFIX_TEST ) )

#define WRITE_COL_OFFSET 2
//W_CS_PREFIX_TEST
//           ___1___________________6             9_________
//      DQM: ___/                   \_____________/
//           _____2_______________5______________________________
// CMD/ADDR:      X        WRITE  X      DQ Data           
//           ¯¯¯¯¯ ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯ ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
//                    3      4
//      /CS: ¯¯¯¯¯¯¯¯¯\______/¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
//                      :   :4  :   :   :  7:8  :   : A :   :
//      CKE: ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯\_____________/¯\________/¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
//                      :   :   :\  :\  :\  :\  :\  :\  :\  :
//      CLK:  |_|¯|_|¯|_|¯|_|¯|_|¯|_|¯|_|¯|_|¯|_|¯|_|¯|_|¯|_|¯|_|¯|_|¯
//                      :   :   :  \:  \:  \:  \:  \:  \   \
// internal:  |_|¯|_|¯|_|¯|_|¯|_|¯|_.___.___.___|¯|_.___.___|¯|_|¯|_|¯
//                      :   :   :               :           :   :   :
// Write Col: ---------0m  0m  1m              2d          3m  4m  5m
//
// #m = column-write MASKED
#endif


#if (  (  defined( W_CS_PREFIX_TEST ) &&  W_CS_PREFIX_TEST ) \
    && (  defined( W_CS_SUFFIX_TEST ) &&  W_CS_SUFFIX_TEST ) )

#define WRITE_COL_OFFSET 1
//W_CS_PREFIX_TEST
// + W_CS_SUFFIX_TEST
//           ___1________________________6             9_________
//      DQM: ___/                        \_____________/
//           _____2____________________5______________________________
// CMD/ADDR:      X        WRITE       X      DQ Data           
//           ¯¯¯¯¯ ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯ ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
//                    3           4a
//      /CS: ¯¯¯¯¯¯¯\\\___________/////¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
//                      :   :4  :   :   :  7:8  :   : A :   :
//      CKE: ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯\_____________/¯\________/¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
//                      :   :   :\  :\  :\  :\  :\  :\  :\  :
//      CLK:  |_|¯|_|¯|_|¯|_|¯|_|¯|_|¯|_|¯|_|¯|_|¯|_|¯|_|¯|_|¯|_|¯|_|¯
//                      :   :   :  \:  \:  \:  \:  \:  \   \
// internal:  |_|¯|_|¯|_|¯|_|¯|_|¯|_.___.___.___|¯|_.___.___|¯|_|¯|_|¯
//                      :   :   :               :           :   :   :
// Write Col: ---------0m  0m  0m              1d          3m  4m  5m
//
// #m = column-write MASKED
// NOTE: 4->4a:
//          The instruction during the first clock *when* CKE is registered low is registered
//          Thus, Write is registered once *after* CKE->Low
#endif


//MISTAKE:::::::::::::
//RW_CS_PREFIX_TEST
// + W_CS_SUFFIX_TEST
//                               V
//           ___1________________._______6             9_________
//      DQM: ___/                .       \_____________/
//           _____2______________._____5______________________________
// CMD/ADDR:      X      WRITE   .       X      DQ Data           
//           ¯¯¯¯¯ ¯¯¯¯¯¯¯¯¯¯¯¯¯¯.¯¯¯¯¯¯¯ ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
//                    3       4  .
//      /CS: ¯¯¯¯¯¯¯\\\_______/¯¯.¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
//                      :   :   :.4a:   :   :  7:8  :   : A :   :
//      CKE: ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯.\_____________/¯\________/¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
//                      :   :  .:.  :\  :\  :\  :\  :\  :\  :\  :
//      CLK:  |_|¯|_|¯|_|¯|_|¯|_|.|_|¯|_|¯|_|¯|_|¯|_|¯|_|¯|_|¯|_|¯|_|¯
//                      :   :   :.  :  \:  \:  \:  \:  \:  \   \
// internal:  |_|¯|_|¯|_|¯|_|¯|_|.|_|¯|_.___.___.___|¯|_.___.___|¯|_|¯|_|¯
//                      :   :   :.  :               :           :   :   :
// Write Col: ---------0m  0m  0m. nm              n+1d          2m  3m  4m
//                               .
//                              arbitrary but equal for READ MISTAKE  (looks to be about 13 cols)
// #m = column-write MASKED
// NOTE: 4->4a:
//          The instruction during the first clock *when* CKE is registered low is registered
//          Thus, Write is registered once *after* CKE->Low


#if (  (  defined( R_CS_PREFIX_TEST ) &&  R_CS_PREFIX_TEST ) \
    && ( !defined( R_CS_SUFFIX_TEST ) || !R_CS_SUFFIX_TEST ) )
//    && (  defined( R_TWO_CKE )        &&  R_TWO_CKE        ) )

#define READ_COL_OFFSET 1
//RW_CS_PREFIX_TEST
//           ___1___________________6                        B________
//      DQM: ___/                   \________________________/
//           _____2_______________5
// CMD/ADDR:      X      READ     >-----------------------------       
//           ¯¯¯¯¯ ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯ 
//                   3           4a
//      /CS: ¯¯¯¯¯¯¯¯\___________/¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
//                  :   :   :   :4  :   :   :  7:8  :   :  9:A  :
//      CKE: ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯\_____________/¯\_________/¯\_______
//                  :   :   :   :\  :\  :\  :\  :\  :\  :\  :\  :\
//      CLK:  |_|¯|_|¯|_|¯|_|¯|_|¯|_|¯|_|¯|_|¯|_|¯|_|¯|_|¯|_|¯|_|¯|_|¯
//                      :   :   :  \:  \:  \:  \:  \:  \:  \:  \
// internal:  |_|¯|_|¯|_|¯|_|¯|_|¯|_|¯|_.___.___.___|¯|_.___.___|¯|_.____
//                      :   :   :   :\              :\          :\
//  Burst 1: ----------c1--c2--c3-<0m>-(truncated)
//  Burst 2: --------------c1--c2--c3-< 0m           >-(truncated)
//  Burst 3: ------------------c1--c2--------------c3-< 0m       >< 1d ...
//
// Note that column 0 is available for quite some time, but needs to be masked while switching pin-directions
// So, we still read column 1
#endif



#if (  (  defined( R_CS_PREFIX_TEST ) &&  R_CS_PREFIX_TEST ) \
    && (  defined( R_CS_SUFFIX_TEST ) &&  R_CS_SUFFIX_TEST ) )
//    && (  defined( R_TWO_CKE )        &&  R_TWO_CKE        ) )

#define READ_COL_OFFSET 0
//RW_CS_PREFIX_TEST
// +R_CS_SUFFIX_TEST
//           ___1_____________________________6                        B________
//      DQM: ___/                             \________________________/
//           _____2_________________________5
// CMD/ADDR:      X            READ         >-----------------------------       
//           ¯¯¯¯¯ ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯ 
//                   3                4a
//      /CS: ¯¯¯¯¯¯¯¯\________________/////¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
//                  :   :   :   :4  :   :   :  7:8  :   :  9:A  :
//      CKE: ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯\_____________/¯\_________/¯\_______
//                  :   :   :   :   :\  :\  :\  :\  :\  :\  :\  :\
//      CLK:  |_|¯|_|¯|_|¯|_|¯|_|¯|_|¯|_|¯|_|¯|_|¯|_|¯|_|¯|_|¯|_|¯|_|¯
//                      :   :   :   :  \:  \:  \:  \:  \:  \:  \
// internal:  |_|¯|_|¯|_|¯|_|¯|_|¯|_|¯|_.___.___.___|¯|_.___.___|¯|_.____
//                      :   :   :   :\              :\          :\
//  Burst 1: ----------c1--c2--c3-<0m>-(truncated)
//  Burst 2: --------------c1--c2--c3-< 0m           >-(truncated)
//  Burst 3: ------------------c1--c2--------------c3-< 0m       >-(truncated)
//  Burst 4: ----------------------c1--------------c2----------c3-< 0d  ...
// NOTE: still need two strobes, because DQM-Latency is two.
#endif


//OLD / WRONG:
// THIS: Is how it was originally designed:
//  and my misunderstanding of how it functioned.
//              
//           ___1______________6                        B_________
//      DQM: ___/              \________________________/
//           _____2__________5
// CMD/ADDR:      X   READ   >-----------------------------       
//           ¯¯¯¯¯ ¯¯¯¯¯¯¯¯¯¯ 
//                    3 4
//      /CS: ¯¯¯¯¯¯¯¯¯\_/¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
//                     :4  :   :   :  7:8  :   :  9:A  :
//      CKE: ¯¯¯¯¯¯¯¯¯¯¯\_____________/¯\_________/¯\_______
//                     :   :\  :\  :\  :\  :\  :\  :\  :\
//      CLK:  _|¯|_|¯|_|¯|_|¯|_|¯|_|¯|_|¯|_|¯|_|¯|_|¯|_|¯|_|¯|_|¯
//                         :  \:  \:  \:  \:  \:  \   \   \
// internal:  _|¯|_|¯|_|¯|_|¯|_.___.___.___|¯|_.___.___|¯|_.____
//                         :               :           :
// Read Col: --------------c1              c2          0
//
// c# = CAS Latency count (CAS=3)
// WRONG: c1 starts at 3->4, c2 occurs slightly before the masked-clock
//        Thus, read-back actually occurs at column 1
//        See drawing at the top.
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
 * /home/meh/_avrProjects/sdramThingZero/21-gittifying/RW_TimingLimitTesting.c
 *
 *    (Wow, that's a lot longer than I'd hoped).
 *
 *    Enjoy!
 *    --Esot.Eric (aka EricWazhung)
 */
