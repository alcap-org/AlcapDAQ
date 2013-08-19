/*********************************************************************

  Name:         lrs1821.c
  Created by:   Stefan Ritt

  Cotents:      Routines for accessing Fastbus over LeCroy 1821 SMI
                under MS-DOS via the 1691A IBM-PC interface.
                
  $Log: lrs1821.c,v $
  Revision 1.1.1.1  2005/06/20 23:37:07  mucap
  Importing release 1.9.5 of the MIDAS source code as distributed by PSI.
  (Next, I'll commit our local customizations.)

  Revision 1.2  2004/01/08 08:40:08  midas
  Implemented standard indentation

  Revision 1.1  1999/12/20 10:18:11  midas
  Reorganized driver directory structure

  Revision 1.2  1998/10/12 12:18:57  midas
  Added Log tag in header


*********************************************************************/

#include <stdio.h>

#include "lrs1821.h"

/*---- Function declarations ---------------------------------------*/

BOOL fb_exec(WORD location);

/*------------------------------------------------------------------*/

static struct {
   WORD addr;
   char data[17];
} patch_table[] = {

   0x34, "c00400000708",        // ECL OUT1=1
       0x35, "c00400000308",    // ECL OUT1=0
       0x36, "800401020000",    // load pdreg=01
       0x37, "830400000000",    // test IN1
       0x38, "a3043a000000",    // return if IN1 on
       0x39, "800400020000",    // load pdreg=00
       0x3a, "c00400", 0x40, "800000080000",    // Hardware block transfer
       0x41, "80040000023f", 0x42, "85b400020200", 0x43, "a5b443040301", 0x44, "807700000e20", 0x45, "a5874a000f01", 0x46, "a07745000a00", 0x47, "850400000301", 0x48, "d0047a00021f", 0x49, "c00400000000", 0x4a, "80041e020000", 0x4b, "c58440000301", 0x7a, "819400028609", 0, "",   // end marker
};

/*------------------------------------------------------------------*/

#ifdef _MSC_VER
static HANDLE _hdio = 0;
#endif

int fb_init(void)
/*********************************************************************
* Initialize Fastbus including 1821 and 1691                         *
*********************************************************************/
{
   WORD status, version, update, i, data, addr;
   char *pc, str[4];

#ifdef _MSC_VER
   OSVERSIONINFO vi;
   DWORD buffer[] = { 6, BASE_1691, BASE_1691 + 18, 0 };
   DWORD size;

   vi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
   GetVersionEx(&vi);

   /* use DirectIO driver under NT to gain port access */
   if (vi.dwPlatformId == VER_PLATFORM_WIN32_NT) {
      _hdio = CreateFile("\\\\.\\directio", GENERIC_READ, FILE_SHARE_READ, NULL,
                         OPEN_EXISTING, 0, NULL);
      if (_hdio == INVALID_HANDLE_VALUE)
         return -1;
   }

   if (!DeviceIoControl(_hdio, (DWORD) 0x9c406000, &buffer, sizeof(buffer),
                        NULL, 0, &size, NULL))
      return -1;
#endif                          /* _MSC_VER */

   // set 1821 front panel address
   OUTP(RCL, BYPASS + 0);

   // clear registers
   OUTWR(REG0, 0);
   OUTWR(REG1, 0);
   OUTWR(REG2, 0);
   OUTWR(REG3, 0);
   OUTWR(REG4, 0);
   OUTWR(REG5, 0);
   OUTWR(REG6, 0);
   OUTWR(REG7, 0);

   // Check communication
   OUTWR(REG3, _RESET);
   status = INPW(REG3);
   if (status != _RESET) {
      cm_msg(MERROR, "fb_init", "Cannot access 1821. Check power and cabling.");
      return FALSE;
   }
   // Check Fastbus power
   status = INPW(REG7);
   if (!(status & PWR_1) || !(status & PWR_2)) {
      cm_msg(MERROR, "fb_init", "Fastbus power test failed. Check power and cabling.");
      return FALSE;
   }
   // Load ROM to sequencer
   OUTWR(REG0, PD_PMAR_TO_SEQ | PA_MENU_TO_SEQ | PLOAD + ROM_NO);
   OUTWR(REG7, PMADZ | PMADL);

   Sleep(10);
   while (INPW(REG7) & ACTIVE);

   // check version of PROM
   OUTWR(REG0, PA_HOST_TO_SEQ | PD_MENU_TO_HOST + ROM_NO);

   OUTWR(REG1, ROM_VER);        // Address of version #
   version = INPW(REG2) & 0xFF;

   OUTWR(REG1, ROM_UPD);        // Address of update #
   update = INPW(REG2) & 0xFF;

   if (version != 6 && version != 7)
      fprintf(stderr, "fb_init: Unkown PROM version %d.%d.\n", version, update);

   // load patch table
   OUTWR(REG0, PA_HOST_TO_SEQ | PD_HOST_TO_SEQ | PLOAD);
   for (i = 0; patch_table[i].addr > 0; i++) {
      pc = patch_table[i].data;
      addr = patch_table[i].addr * 8;
      do {
         if (isxdigit(*pc) && isxdigit(*(pc + 1))) {
            str[0] = *pc;
            str[1] = *(pc + 1);
            str[2] = 0;
            sscanf(str, "%X", &data);

            OUTWR(REG1, addr);
            OUTWR(REG2, data);
            OUTWR(REG7, SQWRT);
         }
         pc += 2;
         addr++;
      } while (*pc);
   }

   // ignite the sequencer
   OUTWR(REG0, PA_HOST_TO_SEQ | PD_HOST_TO_SEQ | PLOAD);
   OUTWR(REG1, IDLE);
   OUTWR(REG0, PA_HOST_TO_SEQ | PD_HOST_TO_SEQ);

   // clear all fastbus lines
   fb_exec(FPCLBS);

   // set arbitration level
   OUTWR(REG2, ARB_LEVEL);
   fb_exec(FBARB);

   // reset bus
   fb_exec(FPRBUS);

   // request mastership
   fb_exec(FPREQ);

   return SUCCESS;
}

/*------------------------------------------------------------------*/

void fb_exit(void)
{
   // release mastership
   fb_exec(FPREL);

#ifdef _MSC_VER
   {
      DWORD buffer[] = { 6, BASE_1691, BASE_1691 + 18, 0 };
      DWORD size;

      if (_hdio <= 0)
         return;

      DeviceIoControl(_hdio, (DWORD) 0x9c406000, &buffer, sizeof(buffer),
                      NULL, 0, &size, NULL);
   }
#endif
}

/*------------------------------------------------------------------*/

int fb_reset(void)
{
   // reset bus
   return fb_exec(FPRBUS);
}

/*------------------------------------------------------------------*/

void fb_write_data_memory(WORD * buffer, WORD count)
/*********************************************************************
* Write 'count' words to 1821 data memory from 'buffer'              *
*********************************************************************/
{
   int i, j;

   OUTWR(REG3, _RESET | DMB_HOST_TO_DMEM);
   OUTWR(REG6, EAUTO | UP + 0);
   OUTWR(REG7, DMARL);

   for (i = 0, j = 0; i < count / 2; i++) {
      OUTWR(REG4, buffer[j]);
      j++;
      OUTWR(REG5, buffer[j]);
      j++;

      OUTWR(REG7, DMWRT);
   }
}

/*------------------------------------------------------------------*/

void fb_read_data_memory(DWORD * data, int count)
/*********************************************************************
* Read 'count' words to 1821 data memory from 'buffer'               *
*********************************************************************/
{
   WORD *p, i;

   if (count == 0)
      return;

   OUTWR(REG3, _RESET | MUXR0 | DMB_DMEM_TO_HOST);
   OUTWR(REG6, EAUTO | UP | EGIP5 + 0);
   OUTWR(REG7, DMARL);

   p = (WORD *) data;

   for (i = 0; i < count / 2; i++)
      *p++ = INPW(REG4);
}

/*------------------------------------------------------------------*/

BOOL fb_exec(WORD location)
/*********************************************************************
* Execute sequencer function at "location" in 1821                   *
*********************************************************************/
{
   int i;

   OUTWR(REG1, location * 8);
   OUTWR(REG7, GO);

   // check if sequencer executed start, ~100 us timeout
   for (i = 100; (INPW(REG7) & IWARDY) && i > 0; i--);
   if (i == 0) {
      fprintf(stderr, "fb_exec: sequencer timeout.\n");
      return (FALSE);
   }
   return (TRUE);
}

/*------------------------------------------------------------------*/

BOOL fb_load32(DWORD data)
/*********************************************************************
* Load a 32-bit data word to the sequencer. Used in read/write fast- *
* bus cycles.                                                        *
*********************************************************************/
{
   OUTWR(REG2, (WORD) data);
   if (!fb_exec(LD_LOW))
      return (FALSE);

   OUTWR(REG2, (WORD) (data >> 16));
   if (!fb_exec(LD_HIGH))
      return (FALSE);

   return (TRUE);

}

/*------------------------------------------------------------------*/

int fb_frd(int paddr, int saddr, DWORD * data)
/* read data space */
{
   // primary address cycle to data space
   OUTWR(REG2, paddr);
   OUTWR(REG1, FPAD * 8);
   OUTWR(REG7, GO);

   // Seconadry address write cycle
   fb_load32(saddr);
   fb_exec(FPWNTA);

   // no pedestal subtraction, reg. 4 & 5 dest of ped.subr.subsyst. output
   OUTWR(REG3, _RESET | MWENB | DMB_PED_TO_HOST);

   // no auto advance due to read
   OUTWR(REG6, 0);

   // data memory addr load from reg. 6
   OUTWR(REG7, DMARL);

   if (!fb_exec(FPR))
      return FALSE;

   ((WORD *) data)[0] = INPW(REG4);
   ((WORD *) data)[1] = INPW(REG5);

   return SUCCESS;
}

/*------------------------------------------------------------------*/

int fb_frc(int paddr, int saddr, DWORD * data)
/* read control space */
{
   // primary address cycle to control space
   OUTWR(REG2, paddr);
   OUTWR(REG1, FPAC * 8);
   OUTWR(REG7, GO);

   // Seconadry address write cycle
   fb_load32(saddr);
   fb_exec(FPWNTA);

   // no pedestal subtraction, reg. 4 & 5 dest of ped.subr.subsyst. output
   OUTWR(REG3, _RESET | MWENB | DMB_PED_TO_HOST);

   // no auto advance due to read
   OUTWR(REG6, 0);

   // data memory addr load from reg. 6
   OUTWR(REG7, DMARL);

   if (!fb_exec(FPR))
      return FALSE;

   ((WORD *) data)[0] = INPW(REG4);
   ((WORD *) data)[1] = INPW(REG5);

   return TRUE;
}

/*------------------------------------------------------------------*/

int fb_fwd(int paddr, int saddr, DWORD data)
{
   // primary address cycle to data space
   OUTWR(REG2, paddr);
   OUTWR(REG1, FPAC * 8);
   OUTWR(REG7, GO);

   // Seconadry address write cycle
   fb_load32(saddr);
   fb_exec(FPWNTA);

   fb_load32(data);
   return (fb_exec(FPW));
}

/*------------------------------------------------------------------*/

int fb_fwc(int paddr, int saddr, DWORD data)
{
   // primary address cycle to control space
   OUTWR(REG2, paddr);
   OUTWR(REG1, FPAC * 8);
   OUTWR(REG7, GO);

   // Seconadry address write cycle
   fb_load32(saddr);
   fb_exec(FPWNTA);

   fb_load32(data);
   return (fb_exec(FPW));
}

/*------------------------------------------------------------------*/

int fb_fwdm(int paddr, int saddr, DWORD data)
{
   return SUCCESS;
}

/*------------------------------------------------------------------*/

int fb_fwcm(int paddr, int saddr, DWORD data)
{
   return SUCCESS;
}

/*------------------------------------------------------------------*/

BOOL fb_frdb(int paddr, int saddr, DWORD * data, int *count)
/*********************************************************************
* Execute a FASTBUS block read into 1821 internal memory.            *
*********************************************************************/
{
   int i;

   // primary address cycle to data space
   OUTWR(REG2, paddr);
   OUTWR(REG1, FPAD * 8);
   OUTWR(REG7, GO);

   // Seconadry address write cycle
   fb_load32(saddr);
   fb_exec(FPWNTA);

   // no pedestal subtraction, reg. 4 & 5 dest of ped.subr.subsyst. output
   OUTWR(REG3, _RESET | MWENB | DMB_PED_TO_HOST);

   // Configure 1821
   OUTWR(REG3, _RESET | MWENB | DMB_PED_TO_DMEM);       // no pedestal subtraction
//  OUTWR(REG3, _RESET | MWENB | DMB_PED_TO_DMEM | PDMSUB | SUPRNEG | SUPRZERO);
   OUTWR(REG6, EAUTO | UP + 0); // start at zero
   OUTWR(REG7, DMARL);

//  OUTWR(REG1, USER);
//  OUTWR(REG1, HDRBL);
   OUTWR(REG1, FPRBL * 8);      // FPRBL for non-LeCroy modules (slower)
   OUTWR(REG7, GO);

   // wait for 1821 to complete, ~200 us timeout
   for (i = 200; (INPW(REG7) & (IWARDY | ACTIVE)) && i > 0; i--);

   if (i == 0) {
      fprintf(stderr, "fb_fprbl: block transfer timeout.\n");
      return (FALSE);
   }
   // read status
   OUTWR(REG0, PA_HOST_TO_SEQ | PD_SEQ_TO_HOST);
   if (INPW(REG2) != 0x200) {
      fprintf(stderr, "fb_fprbl: bad block transfer status.\n");
      return (FALSE);
   }
   OUTWR(REG0, PA_HOST_TO_SEQ | PD_HOST_TO_SEQ);

   // read dword count
   i = (INPW(REG6) & 0xFFF) * sizeof(DWORD);
   if (i > *count)
      i = *count;
   *count = i;

   fb_read_data_memory(data, *count);

   return (TRUE);
}

/*------------------------------------------------------------------*/

BOOL fb_ped_load(WORD * data, WORD count, WORD slot, BOOL high)
/*********************************************************************
* Download 'count' pedestal values in 'data' on 'slot'               *
* If 'high'==TRUE, put to the high range pedestal memory.            *
*********************************************************************/
{
   int i;
   WORD addr;

   OUTWR(REG3, _RESET | PDMSEL | DMB_HOST_TO_PED);

   for (i = 0; i < count; i++) {
      addr = (slot << 11) | (high << 7) | i;
      OUTWR(REG5, addr);
      OUTWR(REG4, data[i]);
      OUTWR(REG7, PDMADR);
      OUTWR(REG7, PDMWRT);
   }

   // verify data
   for (i = 0; i < count; i++) {
      OUTWR(REG3, _RESET | PDMSEL | DMB_HOST_TO_PED);
      addr = (slot << 11) | (high << 7) | i;
      OUTWR(REG5, addr);
      OUTWR(REG7, PDMADR);

      OUTWR(REG3, _RESET | PDMSEL | DMB_PED_TO_HOST);
      if (INPW(REG4) != data[i]) {
         printf("fb_ped_load: pedestal verify error.\n");
         return (FALSE);
      }
   }

   return (TRUE);
}

/*------------------------------------------------------------------*/

BOOL fb_thr_load(WORD data, BOOL high)
/*********************************************************************
* Download threshold value                                           *
*********************************************************************/
{
   OUTWR(REG3, _RESET | DMB_HOST_TO_PED);
   OUTWR(REG5, (high << 7));
   OUTWR(REG4, data);
   OUTWR(REG7, PDMADR);
   OUTWR(REG7, PDCWRT);
   return (TRUE);
}

/*------------------------------------------------------------------*/

DWORD fb_in()
/*********************************************************************
* Read ECL IN1                                                       *
*********************************************************************/
{
   DWORD s;

   fb_exec(IN1TST);

   // read status
   OUTWR(REG0, PA_HOST_TO_SEQ | PD_SEQ_TO_HOST);
   s = INPW(REG2);
   OUTWR(REG0, PA_HOST_TO_SEQ | PD_HOST_TO_SEQ);

   return s;
}

/*------------------------------------------------------------------*/

int fb_out(DWORD data)
/*********************************************************************
* Write ECL OUT1                                                     *
*********************************************************************/
{
   // TDB
   return SUCCESS;
}
