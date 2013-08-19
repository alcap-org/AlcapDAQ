/*********************************************************************

  Name:         str340.c
  Created by:   Stefan Ritt

  Cotents:      Routines for accessing Fastbus over Struck Fastbus
                Interface (SFI) STR340 in combination with a Bit3
                PCI-VME interface (Model 617) under Windows NT using
                the NT device driver Model 983.
                

  Revision history
  ------------------------------------------------------------------
  date        by    modification
  --------    ---   ------------------------------------------------
  28-APR-98   SR    created

*********************************************************************/

#include    <stdio.h>

#define BT983
#include    "btapi.h"

#include    "mfbstd.h"

/*---- macros for writing/reading SFI ------------------------------*/

extern char *_sfi;

#define SFI_OUT(adr, data) {*((DWORD*) (_sfi+(adr))) = (data);}
#define SFI_IN(adr) *((DWORD*) (_sfi+(adr)))
#define SFI_ADDRESS 0xE00000
#define SFI_SIZE    0x1FFFF

/*---- address offsets ---------------------------------------------*/

#define SFI_READ_INT_FB_AD    0x1000
#define SFI_LAST_PRIMADR_REG  0x1004

#define SFI_WRITE_VME_OUT_SIGNAL_REGISTER 0x1000
#define SFI_CLEAR_BOTH_LCA1_TEST_REGISTER 0x1004
#define SFI_WRITE_INT_FB_AD_REG			      0x1010
#define SFI_KEY_AUX_PULS_B40			        0x1014

#define SFI_READ_VME_TEST_REGISTER		    0x1004
#define SFI_READ_LOCAL_FB_AD_BUS          0x1008
#define SFI_READ_VME2SEQ_DATA_FIFO        0x100C
#define SFI_WRITE_VME_TEST_REGISTER       0x1008

#define SFI_FASTBUS_TIMEOUT_REGISTER                       0x2000
#define SFI_FASTBUS_ARBITRATION_LEVEL_REGISTER             0x2004
#define SFI_FASTBUS_PROTOCOL_INLINE_REGISTER               0x2008
#define SFI_SEQUENCER_FIFO_FLAG_AND_ECL_NIM_INPUT_REGISTER 0x200C

#define SFI_NEXT_SEQUENCER_RAM_ADDRESS_REGISTER  0x2018
#define SFI_LAST_SEQUENCER_PROTOCOL_REGISTER     0x201C
#define SFI_SEQUENCER_STATUS_REGISTER            0x2220
#define SFI_FASTBUS_STATUS_REGISTER1             0x2224
#define SFI_FASTBUS_STATUS_REGISTER2             0x2228

#define SFI_VME_IRQ_LEVEL_AND_VECTOR_REGISTER    0x2010
#define SFI_VME_IRQ_MASK_REGISTER                0x2014
#define SFI_SEQUENCER_RAM_ADDRESS_REGISTER       0x2018
#define SFI_RESET_REGISTER_GROUP_LCA2            0x201C

#define SFI_SEQUENCER_ENABLE              0x2020
#define SFI_SEQUENCER_DISABLE             0x2024
#define SFI_SEQUENCER_RAM_LOAD_ENABLE     0x2028
#define SFI_SEQUENCER_RAM_LOAD_DISABLE    0x202C
#define SFI_SEQUENCER_RESET               0x2030
#define SFI_SEQUENCER_TRY_AGAIN           0x2034

#define SFI_READ_SEQ2VME_FIFO_BASE        0x4000
#define SFI_WRITE_VME2SEQ_FIFO_BASE      0x10000

/*---- constants for sequencer key addresses -----------------------*/

#define PRIM_DSR     0x0004
#define PRIM_CSR     0x0104
#define PRIM_DSRM    0x0204
#define PRIM_CSRM    0x0304
#define PRIM_AMS4    0x0404
#define PRIM_AMS5    0x0504
#define PRIM_AMS6    0x0604
#define PRIM_AMS7    0x0704

#define PRIM_HM_DSR  0x0014
#define PRIM_HM_CSR  0x0114
#define PRIM_HM_DSRM 0x0214
#define PRIM_HM_CSRM 0x0314
#define PRIM_HM_AMS4 0x0414
#define PRIM_HM_AMS5 0x0514
#define PRIM_HM_AMS6 0x0614
#define PRIM_HM_AMS7 0x0714

#define PRIM_EG      0x1000

#define RNDM_R       0x0844
#define RNDM_W       0x0044
#define SECAD_R      0x0A44
#define SECAD_W      0x0244
#define SBLK_W       0x0144
#define SBLK_CLEAN   0x0064

#define RNDM_R_DIS   0x0854
#define RNDM_W_DIS   0x0054
#define SECAD_R_DIS  0x0A54
#define SECAD_W_DIS  0x0254
#define SBLK_CLEAN_DIS 0x0074

#define DISCON       0x0024
#define DISCON_RM    0x0034

#define START_FRDB_WITH_CLEAR_WORD_COUNTER 0x08A4

#define STORE_FRDB_WC 0x00E4
#define STORE_FRDB_AP 0x00D4

#define LOAD_DMA_ADDRESS_POINTER 0x0094

#define DISABLE_RAM_MODE 0x0038

#define ENABLE_RAM_SEQUENCER 0x0028

#define SEQ_WRITE_OUT_REG 0x0008

/*---- replacements if not running under MIDAS ---------------------*/

#ifndef _MIDAS_H_

#define cm_msg(file, routine, msg) printf("[%s:%s] %s", file, routine, msg)

#define MERROR    __FILE__
#define MINFO     __FILE__

#endif                          /* _MIDAS_H */

/*------------------------------------------------------------------*/

/* SFI base address when mapped to the local address space */
char *_sfi;

/* Unit descriptors for Bit 3 library */
bt_desc_t btd, btd_lm;

/* remote RAM buffer (writable by VME master) */
#define REMOTE_RAM_SIZE 0x10000
char *remote_ram_buffer;

/*------------------------------------------------------------------*/

int fb_init()
{
   bt_error_t status;           /* Bit 3 library error return type */
   char devname[BT_MAX_DEV_NAME];       /* Device to open */
   bt_devdata_t flag;

   /* Open the device in A24 space */
   bt_gen_name(0, BT_DEV_A24, devname, BT_MAX_DEV_NAME);
   status = bt_open(&btd, devname, BT_RDWR);
   if (status != BT_SUCCESS) {
      cm_msg(MERROR, "fb_init", "Cannot open Bit3 device driver");
      return FALSE;
   }
   bt_init(btd);
   bt_clrerr(btd);

   /* map SFI memory */
   status = bt_mmap(btd, &_sfi, SFI_ADDRESS, SFI_SIZE, BT_RDWR, BT_SWAP_DEFAULT);
   if (status != BT_SUCCESS) {
      cm_msg(MERROR, "fb_init", "Cannot map VME memory");
      bt_close(btd);
      return FALSE;
   }

   /* Open the device in local memory */
   bt_gen_name(0, BT_DEV_LM, devname, BT_MAX_DEV_NAME);
   status = bt_open(&btd_lm, devname, BT_RDWR);
   if (status != BT_SUCCESS) {
      cm_msg(MERROR, "fb_init", "Cannot open Bit3 device for local memory");
      return FALSE;
   }
   bt_clrerr(btd_lm);

   /* map local memory */
   status =
       bt_mmap(btd_lm, &remote_ram_buffer, 0, REMOTE_RAM_SIZE, BT_RDWR, BT_SWAP_DEFAULT);
   if (status != BT_SUCCESS) {
      cm_msg(MERROR, "fb_init", "Cannot map local memory");
      bt_close(btd);
      return FALSE;
   }

   /* clear local memory */
   memset(remote_ram_buffer, 0, REMOTE_RAM_SIZE);

   /* force D32 mode */
   bt_get_info(btd, BT_INFO_MMAP_AMOD, &flag);
   flag = BT_AMOD_A24_SD;
   bt_set_info(btd, BT_INFO_MMAP_AMOD, flag);

   /* sequencer reset */
   SFI_OUT(SFI_SEQUENCER_RESET, 0);

   /* arbitration level */
//  SFI_OUT(SFI_FASTBUS_ARBITRATION_LEVEL_REGISTER, 0x15);
   SFI_OUT(SFI_FASTBUS_ARBITRATION_LEVEL_REGISTER, 0xBF);

   /* timeout */
   SFI_OUT(SFI_FASTBUS_TIMEOUT_REGISTER, 0x73);

   /* sequencer enable */
   SFI_OUT(SFI_SEQUENCER_ENABLE, 0);

   /* switch off all output */
   SFI_OUT(SFI_CLEAR_BOTH_LCA1_TEST_REGISTER, 0);

   /* clear registers */
   SFI_OUT(SFI_RESET_REGISTER_GROUP_LCA2, 0);

   return SUCCESS;
}

/*------------------------------------------------------------------*/

void fb_exit()
{
   bt_unmmap(btd, _sfi, SFI_SIZE);
   bt_close(btd);
   bt_unmmap(btd_lm, remote_ram_buffer, REMOTE_RAM_SIZE);
   bt_close(btd_lm);
}

/*------------------------------------------------------------------*/

int fb_reset(void)
/* reset sequencer after FB failure (like blocktransfer with no data) */
{
   SFI_OUT(SFI_SEQUENCER_RESET, 0);
   SFI_OUT(SFI_SEQUENCER_ENABLE, 0);
   return SUCCESS;
}

/*------------------------------------------------------------------*/

static int fb_fifo_read(DWORD * data)
/* read SFI FIFO and check for error */
{
   DWORD status;

   /* wait for sequencer done */
   do {
      status = SFI_IN(SFI_SEQUENCER_STATUS_REGISTER);

      if ((status & 0x8001) == 0)
         /* not initialized */
         return 0x10000 | (status & 0xFFFF);

      if ((status & 0x8001) == 0x8000)
         /* bad status */
         return 0x10000 | (status & 0xFFFF);

      if ((status & 0x8001) == 0x8001) {
         /* OK, check if EMPTY */
         status = SFI_IN(SFI_SEQUENCER_FIFO_FLAG_AND_ECL_NIM_INPUT_REGISTER);
         if (status & 0x10) {
            /* dummy read */
            status = SFI_IN(SFI_READ_SEQ2VME_FIFO_BASE);
            /* check again for EMTPY */
            status = SFI_IN(SFI_SEQUENCER_FIFO_FLAG_AND_ECL_NIM_INPUT_REGISTER);
            if (status & 0x10)
               return 0x10000 | (SFI_IN(SFI_SEQUENCER_STATUS_REGISTER) & 0xFFFF);
         }
         /* read FIFO */
         *data = SFI_IN(SFI_READ_SEQ2VME_FIFO_BASE);
         return SUCCESS;
      }
   } while (TRUE);
}

/*------------------------------------------------------------------*/

int fb_frd(int paddr, int saddr, DWORD * data)
/* read data space */
{
   SFI_OUT(SFI_WRITE_VME2SEQ_FIFO_BASE + PRIM_DSR, paddr);
   SFI_OUT(SFI_WRITE_VME2SEQ_FIFO_BASE + SECAD_W, saddr);
   SFI_OUT(SFI_WRITE_VME2SEQ_FIFO_BASE + RNDM_R_DIS, 0);

   return fb_fifo_read(data);
}

/*------------------------------------------------------------------*/

int fb_frc(int paddr, int saddr, DWORD * data)
/* read control space */
{
   int status;

   SFI_OUT(SFI_WRITE_VME2SEQ_FIFO_BASE + PRIM_CSR, paddr);

   status = SFI_IN(SFI_SEQUENCER_STATUS_REGISTER);
   if (status & 0x20)
      cm_msg(MERROR, "fb_frc", "Arbitration error on Primary Address Cycle");

   SFI_OUT(SFI_WRITE_VME2SEQ_FIFO_BASE + SECAD_W, saddr);
   SFI_OUT(SFI_WRITE_VME2SEQ_FIFO_BASE + RNDM_R_DIS, 0);

   return fb_fifo_read(data);
}

/*------------------------------------------------------------------*/

int fb_fwd(int paddr, int saddr, DWORD data)
/* write data space */
{
   SFI_OUT(SFI_WRITE_VME2SEQ_FIFO_BASE + PRIM_DSR, paddr);
   SFI_OUT(SFI_WRITE_VME2SEQ_FIFO_BASE + SECAD_W, saddr);
   SFI_OUT(SFI_WRITE_VME2SEQ_FIFO_BASE + RNDM_W_DIS, data);

   return SUCCESS;
}

/*------------------------------------------------------------------*/

int fb_fwc(int paddr, int saddr, DWORD data)
/* write conrol space */
{
   SFI_OUT(SFI_WRITE_VME2SEQ_FIFO_BASE + PRIM_CSR, paddr);
   SFI_OUT(SFI_WRITE_VME2SEQ_FIFO_BASE + SECAD_W, saddr);
   SFI_OUT(SFI_WRITE_VME2SEQ_FIFO_BASE + RNDM_W_DIS, data);

   return SUCCESS;
}

/*------------------------------------------------------------------*/

int fb_fwdm(int paddr, int saddr, DWORD data)
/* write data space broadcast */
{
   SFI_OUT(SFI_WRITE_VME2SEQ_FIFO_BASE + PRIM_DSRM, paddr);
   SFI_OUT(SFI_WRITE_VME2SEQ_FIFO_BASE + SECAD_W, saddr);
   SFI_OUT(SFI_WRITE_VME2SEQ_FIFO_BASE + RNDM_W_DIS, data);

   return SUCCESS;
}

/*------------------------------------------------------------------*/

int fb_fwcm(int b_case, int paddr, int saddr, DWORD data)
/* Write control space broadcast.

  For a broadcast of case 2 (LRS1866), paddr must be the class N as
  defined in the CSR

  For a broadcast of case 3 (LRS1882), paddr must be a bit combination 
  of stations to be addressed: (1<<slot1) | (1<,slot2) | ... 

*/
{
   if (b_case == 2) {
      /* case 2 broadcast (class select) */
      SFI_OUT(SFI_WRITE_VME2SEQ_FIFO_BASE + PRIM_HM_CSRM, 0x05);
      SFI_OUT(SFI_WRITE_VME2SEQ_FIFO_BASE + RNDM_W, paddr);
   }
   if (b_case == 3) {
      /* case 3 broadcast (pattern select) */
      SFI_OUT(SFI_WRITE_VME2SEQ_FIFO_BASE + PRIM_HM_CSRM, 0x09);
      SFI_OUT(SFI_WRITE_VME2SEQ_FIFO_BASE + RNDM_W, paddr);
   }

   SFI_OUT(SFI_WRITE_VME2SEQ_FIFO_BASE + SECAD_W, saddr);
   SFI_OUT(SFI_WRITE_VME2SEQ_FIFO_BASE + RNDM_W_DIS, data);

   return SUCCESS;
}

/*------------------------------------------------------------------*/

int fb_frcm(int b_case, DWORD * data)
/* Read control space broadcast.

  For a broadcast of case 3 (sparse data scan), each module
  having data ready responds via its corresponding T-Pin bit.
*/
{
   if (b_case == 3)
      /* case 3 broadcast (sparse data scan) */
      SFI_OUT(SFI_WRITE_VME2SEQ_FIFO_BASE + PRIM_HM_CSRM, 0x09);

   SFI_OUT(SFI_WRITE_VME2SEQ_FIFO_BASE + RNDM_R_DIS, 0);

   return fb_fifo_read(data);
}

/*------------------------------------------------------------------*/

int fb_frdb(int paddr, int saddr, DWORD * data, int *count)
/* read data space block transfer */
{
   int status;

   SFI_OUT(SFI_SEQUENCER_DISABLE, 0);

   SFI_OUT(SFI_WRITE_VME2SEQ_FIFO_BASE + PRIM_DSR, paddr);
   SFI_OUT(SFI_WRITE_VME2SEQ_FIFO_BASE + SECAD_W, saddr);

   /* DMA address 0 (remote ram window) */
   SFI_OUT(SFI_WRITE_VME2SEQ_FIFO_BASE + LOAD_DMA_ADDRESS_POINTER, 0);

   /* VME block mode DMA */
   SFI_OUT(SFI_WRITE_VME2SEQ_FIFO_BASE + START_FRDB_WITH_CLEAR_WORD_COUNTER,
           (0x0A << 24) | (*count / sizeof(DWORD)));

   /* disonnect from module */
   SFI_OUT(SFI_WRITE_VME2SEQ_FIFO_BASE + DISCON, 0);

   /* store word count and adress pointer */
   SFI_OUT(SFI_WRITE_VME2SEQ_FIFO_BASE + STORE_FRDB_WC, 0);
//  SFI_OUT(SFI_WRITE_VME2SEQ_FIFO_BASE + STORE_FRDB_AP, 0);

   SFI_OUT(SFI_SEQUENCER_ENABLE, 0);

   status = fb_fifo_read(count);
   if (status & 0x10000)
      return status & 0xFFFF;

//  fb_fifo_read(&ap);

   *count &= 0xFFFFFF;
   *count *= sizeof(DWORD);

   /* move data from kernel memory to user buffer */
   memcpy(data, remote_ram_buffer, *count);

   return SUCCESS;
}

/*------------------------------------------------------------------*/

void fb_frdba(int paddr, int saddr, int count)
/* asynchronous read data space block transfer */
{
   /* address module */
   SFI_OUT(SFI_WRITE_VME2SEQ_FIFO_BASE + PRIM_DSR, paddr);
   SFI_OUT(SFI_WRITE_VME2SEQ_FIFO_BASE + SECAD_W, saddr);

   /* VME block mode DMA */
   SFI_OUT(SFI_WRITE_VME2SEQ_FIFO_BASE + START_FRDB_WITH_CLEAR_WORD_COUNTER,
           (0x0A << 24) | (count / sizeof(DWORD)));

   /* disonnect from module */
   SFI_OUT(SFI_WRITE_VME2SEQ_FIFO_BASE + DISCON, 0);
}

/*------------------------------------------------------------------*/

int fb_out(DWORD data)
/* write out-signal register 
   bit 0-3  : set LED
   bit 4-7  : set ECL
   bit 8-10 : set NIM
   bit 16-19: clear LED
   bit 20-23: clear ECL
   bit 24-26: clear NIM */
{
   /* directly access output register */
   /* SFI_OUT(SFI_WRITE_VME_OUT_SIGNAL_REGISTER, data); */

   /* ask the sequencer to write to the output */
   SFI_OUT(SFI_WRITE_VME2SEQ_FIFO_BASE + SEQ_WRITE_OUT_REG, data);
   return SUCCESS;
}

/*------------------------------------------------------------------*/

int fb_in(void)
/* read ECL/NIM input
   bit 8-11 : ECL input
   bit 12-15: NIM input */
{
   return (int) SFI_IN(SFI_SEQUENCER_FIFO_FLAG_AND_ECL_NIM_INPUT_REGISTER);
}

/*------------------------------------------------------------------*/

int fb_load_begin(int addr)
/* load sequencer in ram mode at given address */
{
   SFI_OUT(SFI_SEQUENCER_RESET, 0);
   SFI_OUT(SFI_NEXT_SEQUENCER_RAM_ADDRESS_REGISTER, addr);
   SFI_OUT(SFI_SEQUENCER_RAM_LOAD_ENABLE, 0);

   /* initialize DMA address */
   SFI_OUT(SFI_WRITE_VME2SEQ_FIFO_BASE + LOAD_DMA_ADDRESS_POINTER, 0);

   return SUCCESS;
}

/*------------------------------------------------------------------*/

int fb_load_end()
/* finish loading sequencer in ram mode */
{
   /* last command: store address pointer */
   SFI_OUT(SFI_WRITE_VME2SEQ_FIFO_BASE + STORE_FRDB_AP, 0);

   SFI_OUT(SFI_WRITE_VME2SEQ_FIFO_BASE + DISABLE_RAM_MODE, 0);

   /* 2us delay */
   SFI_OUT(SFI_WRITE_VME_OUT_SIGNAL_REGISTER, 0);

   SFI_OUT(SFI_SEQUENCER_RAM_LOAD_DISABLE, 0);
   SFI_OUT(SFI_SEQUENCER_ENABLE, 0);

   return SUCCESS;
}

/*------------------------------------------------------------------*/

int fb_execute(int addr, void *data, int *count)
/* execute commands form ram list at given address */
{
   int ap, status;

   SFI_OUT(SFI_WRITE_VME2SEQ_FIFO_BASE + ENABLE_RAM_SEQUENCER + (addr & 0xFF00), 0);

   if (data == NULL)
      return SUCCESS;

   status = fb_fifo_read(&ap);
   if (status & 0x10000)
      return status & 0xFFFF;

   ap &= 0xFFFFFF;
   if (ap < *count)
      *count = ap;

   /* move data from kernel memory to user buffer */
   memcpy(data, remote_ram_buffer, *count);

   return SUCCESS;
}

/*------------------------------------------------------------------*/

void fb_irq_enable(int source)
/* enable interrupt request for a given source
   bit 0 : NIM1
   bit 1 : NIM2
   bit 2 : NIM3
   bit 3 : ECL1 */
{
   SFI_OUT(SFI_VME_IRQ_MASK_REGISTER, source);
}

/*------------------------------------------------------------------*/

void fb_irq_disable(int source)
/* disable or clear interrupt request for a given source
   bit 0 : NIM1
   bit 1 : NIM2
   bit 2 : NIM3
   bit 3 : ECL1 */
{
   SFI_OUT(SFI_VME_IRQ_MASK_REGISTER, source << 8);
}

/*------------------------------------------------------------------*/

void fb_irq_read(int *source)
/* read intterupt source */
{
   *source = (SFI_IN(SFI_VME_IRQ_MASK_REGISTER) >> 8) & 0xFF;
}

/*------------------------------------------------------------------*/

void fb_irq_clear(int source)
/* disable or clear interrupt request for a given source
   bit 0 : NIM1
   bit 1 : NIM2
   bit 2 : NIM3
   bit 3 : ECL1 */
{
   SFI_OUT(SFI_VME_IRQ_MASK_REGISTER, source << 8);
   SFI_OUT(SFI_VME_IRQ_MASK_REGISTER, source);
}

/*------------------------------------------------------------------*/

void fb_interrupt_enable(void)
{
   /* VME IRQ Level 1 */
   SFI_OUT(SFI_VME_IRQ_LEVEL_AND_VECTOR_REGISTER, 0x800 | 0x100);
}

/*------------------------------------------------------------------*/

void fb_interrupt_disable(void)
{
   /* VME IRQ Level 1 */
   SFI_OUT(SFI_VME_IRQ_LEVEL_AND_VECTOR_REGISTER, 0x000);
}

/*------------------------------------------------------------------*/

void (*user_interrupt_routine) () = NULL;

void application_icbr(void *param_p, bt_irq_t irq_type, bt_data32_t vector)
{
   int status;

   status = bt_chkerr(btd);

   if (user_interrupt_routine != NULL)
      user_interrupt_routine();
}

/*------------------------------------------------------------------*/

void fb_interrupt_attach(void (*isr) ())
{
   int status;

   user_interrupt_routine = isr;
   status = bt_icbr_install(btd, BT_IRQ_IACK, application_icbr, NULL, BT_VECTOR_ALL);
   if (status != BT_SUCCESS)
      bt_perror(btd, status, "Could not install interrupt call back routine.");
}

/*------------------------------------------------------------------*/

void fb_interrupt_detach(void)
{
   bt_icbr_remove(btd, BT_IRQ_IACK, application_icbr);
   user_interrupt_routine = NULL;
}
