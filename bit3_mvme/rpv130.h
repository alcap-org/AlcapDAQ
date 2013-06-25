// ****************************************************************************
// *     Filename:  rpv130.h
// *  Description:  
// *   Created by:  Nam Tran (nam at kuno-g.phys.sci.osaka-u.ac.jp)
// *     Time tag:  2013-04-06 11:11:18
// ****************************************************************************

#ifndef RPV130_VT29172O
#define RPV130_VT29172O

#define RPV130_LATCH_1    0x0
#define RPV130_LATCH_2    0x2
#define RPV130_RSFF       0x4
#define RPV130_THROUGH    0x6
#define RPV130_PULSE      0x8
#define RPV130_LEVEL      0xa
#define RPV130_CSR_1      0xc
#define RPV130_CSR_2      0xe

#define RPV130_DATA_SIZE   MVME_DMODE_D16       //16bit
#define RPV130_AM          MVME_AM_A16_ND //AM: 2D
#define RPV130_AMODE VMEDRV_A16D16
#include "mvmestd.h"

int rpv130_Clear(MVME_INTERFACE *vme, mvme_addr_t base_addr);
int rpv130_IsBusy1(MVME_INTERFACE *vme, mvme_addr_t base_addr);
int rpv130_IsBusy2(MVME_INTERFACE *vme, mvme_addr_t base_addr);
int rpv130_IsBusy3(MVME_INTERFACE *vme, mvme_addr_t base_addr);
int rpv130_ClearBusy1(MVME_INTERFACE *vme, mvme_addr_t base_addr);
int rpv130_ClearBusy2(MVME_INTERFACE *vme, mvme_addr_t base_addr);
int rpv130_ClearBusy3(MVME_INTERFACE *vme, mvme_addr_t base_addr);
int rpv130_Pulse(MVME_INTERFACE *vme, mvme_addr_t base_addr, int chn); 
int rpv130_Level(int outp); // set outp at level 1 
void rpv130_PrintBusyStatus(MVME_INTERFACE *vme, mvme_addr_t base_addr);

#endif /* end of include guard: RPV130_VT29172O */
