/********************************************************************\

  Name:         lrs1821.h
  Created by:   Stefan Ritt

  Cotents:      Header file for LeCroy 1821 segment manager interface.
                Includes logical names for 1821 registers and macros.

  $Log: lrs1821.h,v $
  Revision 1.1.1.1  2005/06/20 23:37:07  mucap
  Importing release 1.9.5 of the MIDAS source code as distributed by PSI.
  (Next, I'll commit our local customizations.)

  Revision 1.2  2004/01/08 08:40:08  midas
  Implemented standard indentation

  Revision 1.1  1999/12/20 10:18:11  midas
  Reorganized driver directory structure

  Revision 1.2  1998/10/12 12:18:57  midas
  Added Log tag in header


\********************************************************************/

#ifndef _LRS1821_H_
#define _LRS1821_H_

#include "mfbstd.h"

/*---- Base IO-address for 1691 module. Must match with jumpers ----*/
#define BASE_1691 0x200

/*---- IO registers in 1691 ----------------------------------------*/
#define REG0      BASE_1691
#define REG1      BASE_1691 + 2
#define REG2      BASE_1691 + 4
#define REG3      BASE_1691 + 6
#define REG4      BASE_1691 + 8
#define REG5      BASE_1691 + 10
#define REG6      BASE_1691 + 12
#define REG7      BASE_1691 + 14
#define RCL       BASE_1691 + 16
#define DMA       BASE_1691 + 17

/*--- port commands ------------------------------------------------*/
#if defined( __MSDOS__ )
#include <dos.h>
#define OUTP(_p, _d) outportb(_p, _d)
#define OUTPW(_p, _d) outport(_p, _d)
#define INP(_p) inportb(_p)
#define INPW(_p) inport(_p)
#elif defined( _MSC_VER )
#include <windows.h>
#include <conio.h>
#define OUTP(_p, _d) _outp((unsigned short) (_p), (int) (_d))
#define OUTPW(_p, _d) _outpw((unsigned short) (_p), (unsigned short) (_d))
#define INP(_p) _inp((unsigned short) (_p))
#define INPW(_p) _inpw((unsigned short) (_p))
#endif

/*--- out port command, first hi-byte, then lo-byte ----------------*/
#define OUTWR(port, data)   OUTP(port+1, ((data)>>8)); OUTP(port, data)

/*---- logicals for 1691 -------------------------------------------*/
#define TAKE_CONTROL 0x10       // communication to 1821 over rear-panel
#define BYPASS       0x20       // bypass 1821 front-panel address

/*---- 1821 ROM number & arbitration level -------------------------*/
#ifndef ROM_NO
# define ROM_NO      0
#endif

#ifndef ARB_LEVEL
# define ARB_LEVEL   1
#endif

/*---- REG0 bitmap -------------------------------------------------*/
#define MMS0         0x0001     // Menu Memory Select bits
#define MMS1         0x0002
#define MMS2         0x0004

#define CLKSPD       0x0008     // 0: Clock with 128 nsec period, 1: 32 nsec
#define LPROC        0x0010
#define HOST         0x0020
#define ATTN         0x0040     // Request Sequencer Attention
#define PLOAD        0x0080     // Sequencer Program Load bit

#define PDD0         0x0100     // PDATA Bus Destination Select
#define PDD1         0x0200
#define PDS0         0x0400     // PDATA Bus Source Select
#define PDS1         0x0800

#define PAD0         0x1000     // PADDR Bus Destination Select
#define PAD1         0x2000
#define PAS0         0x4000     // PADDR Bus Source Select
#define PAS1         0x8000

#define PD_HOST_TO_SEQ   (PDS1 | PDD0)
#define PD_PMAR_TO_SEQ   (PDD0)
#define PD_MENU_TO_HOST  (PDD1)
#define PD_SEQ_TO_HOST   (PDS0 | PDD1)

#define PA_HOST_TO_SEQ   (PAS1 | PAD0)
#define PA_MENU_TO_SEQ   (PAD0)

/*---- REG1 bitmap -------------------------------------------------*/
#define ADLQ         0x2000     // Auto Download flip/flop

/*---- REG3 bitmap -------------------------------------------------*/
#define DMBD0        0x0001     // DMB Destination Select
#define DMBD1        0x0002
#define DMBS0        0x0004     // DMB Source Select
#define DMBS1        0x0008

#define DMB_HOST_TO_DMEM    (DMBS1)
#define DMB_DMEM_TO_HOST    (DMBD1)
#define DMB_PED_TO_HOST     (DMBS0 | DMBS1 | DMBD1)
#define DMB_PED_TO_DMEM     (DMBS0 | DMBS1)
#define DMB_HOST_TO_PED     (DMBS1 | DMBD0 | DMBD1)

#define MUXR0        0x0010     // Multiplex REG 4/5
#define PCMUX16      0x0020     // AUX Connector multiplex
#define PCFLG1       0x0040     // General purpose
#define PDMSUB       0x0100     // Subtract Data from Pedestral Memory
#define PDMSEL       0x0200     // Select Pedestral Memory as DMB Source/Dest.
#define MWENB        0x0400     // Enable Memory Write Strobes from Seq. Pip.
#define SUPRNEG      0x0800     // Supress negative Data to zero
#define SUPRZERO     0x1000     // Supress Memory Write of zeor Data
#define SIGNED       0x2000     // Signed Pedestral Data
#define _RESET       0x8000     // Master Reset Signal

/*---- REG6 bitmap -------------------------------------------------*/
#define DMIA_MASK    0x0FFF     // Data Memory Initial Address
#define EAUTO        0x1000     // Enable Auto Incr/Decr
#define UP           0x2000     // Incr/Decr mode
#define EGIP5        0x4000     // Auto increment on HOST I/O Reg. 5
#define LINMEM       0x8000     // Data memory addressing mode 0: circ 1:lin

/*---- REG7 bitmap -------------------------------------------------*/
#define GO           0x0001     // Sequencer GO command
#define PMADZ        0x0002     // Program Memory Address Register Zero
#define PMADL        0x0004     // Program Memory Auto Download Cycle Initiate
#define ROWCL        0x0008     // Readout Word Counter Load
#define DMARL        0x0010     // Data Memory Addresss Register Load
#define DMARC        0x0020     // Data Memory Addresss Register Count
#define DMWRT        0x0040     // Data Memory Write
#define SQWRT        0x0100     // Sequencer Program memory Write
#define PMARC        0x0200     // Program Memory Address Register Clock
#define PMWRT        0x0400     // Program Memory Write
#define ROWCC        0x0800     // Readout Word Counter Clock
#define PDCWRT       0x1000     // Pedestral Data Comparator Register Write
#define PDMADR       0x4000     // Pedestral Data Memory Address Latch
#define PDMWRT       0x8000     // Pedestral Data Memory Write

// Read masks

#define PWR_1        0x0001     // Sequencer power intact
#define PWR_2        0x0002     // Host interface power intact
#define RDOC         0x0004     // Sequencer Read Out Complete
#define PDFULL       0x0008     // Sequencer Parameter Data for Host
#define IWARDY       0x0010     // Sequencer ready for new Initial Word Addr.
#define ACTIVE       0x0020     // Sequencer active
#define DMEMPTY      0x0040     // Data memory empty status
#define DMFULL       0x0080     // Data memory full / cycled
#define PCPWR        0x0100     // Personality Card power status
#define PCENB        0x0200     // Personality Card memory enabled for output
#define PCFULL       0x0400     // Personality Card memory full status

#define SQATTN2      0x2000     // Set by SEQ
#define SQATTN1      0x4000     // Set by SEQ
#define SEQ_ATTNPEND 0x8000     // Set by SEQ

/*---- FASTBUS protocol standard routines --------------------------*/
#define FPAD         0x08       // geo addr to data space
#define FPAC         0x0C       // geo addr to control space
#define FPR          0x10       // read
#define FPRNTA       0x13       // sec addr read
#define FPW          0x16       // write
#define FPWNTA       0x19       // sec addr write
#define FPCLBS       0x1C       // clear fastbus
#define FPADM        0x1E       // broadcast to DSR space
#define FPACM        0x21       // broadcast to CSR space
#define FPRBUS       0x24       // assert rb line
#define FPREQ        0x2A       // request bus mastership
#define FPREL        0x2B       // release bus mastership
#define FPRBL        0x2E       // block transfer read

/*---- 1821 special routines ---------------------------------------*/
#define ROM_VER      (0x00*8+7) // ROM version
#define ROM_UPD      (0x01*8+7) // ROM update

#define IDLE         0x00       // idle loop
#define FPAD_T       0x09       // geo addr to data space, slot # in tcnt
#define FPAC_T       0x0D       // geo addr to control space, slot # in tcnt
#define TP_SCAN      0x25       // T-pin scan
#define CR_SCAN      0x26       // crate scan
#define EN_SS        0x27       // enable SS response to host
#define LD_NREG      0x28       // load nreg from host
#define FBARB        0x29       // set arbitration level
#define HDRBL        0x2C       // special hardware block transfer
#define LD_LOW       0x30       // load low 16 bits of 32 bit register
#define LD_HIGH      0x31       // load high 16 bits of 32 bit register
#define IND_TDC      0x32       // individual channel of TDC
#define TP_TDC       0x33       // T-pin readout of whole segment TDCs

/*---- diagnostic and test routines --------------------------------*/
#define OUT1UP       0x34       // ECL OUT1 up
#define OUT1DN       0x35       // ECL OUT1 down
#define IN1TST       0x36       // Test IN1

#define USER         0x40       // User downlodable area

/*---- replacements if not running runder MIDAS --------------------*/

#ifndef _MIDAS_H_

#define cm_msg(file, routine, msg) printf("[%s:%s] %s", file, routine, msg)

#define MERROR    __FILE__
#define MINFO     __FILE__

#endif                          /* _MIDAS_H_ */

#endif                          /* _LRS1821_H_ */
