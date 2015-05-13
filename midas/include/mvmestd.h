/*********************************************************************

  Name:         mvmestd.h
  Created by:   Stefan Ritt

  Cotents:      Midas VME standard routines (MVMESTD) supplying an
                abstract layer to all supported VME interfaces.
                
  $Id$

*********************************************************************/

#ifndef MVMESTD_H
#define MVMESTD_H

/**dox***************************************************************/
/** @file mvmestd.h
The Midas VME include file
*/

/** @defgroup mvmestdinclude Midas VME standard 
 */
/** @defgroup mvmestdfunctionh VME Functions (mvme_xxx) 
 */

/**dox***************************************************************/
/** @addtogroup mvmestdinclude
 *  
 *  @{  */

/**dox***************************************************************/
#ifndef DOXYGEN_SHOULD_SKIP_THIS

/*---- replacements if not running under MIDAS ---------------------*/

#ifndef MIDAS_TYPE_DEFINED
#define MIDAS_TYPE_DEFINED

typedef unsigned short int WORD;

#ifndef _MSC_VER
typedef unsigned int DWORD;
#endif

#define SUCCESS  1

#endif                          /* MIDAS_TYPE_DEFINED */

/* make functions under WinNT dll exportable */
#if defined(_MSC_VER) && defined(MIDAS_DLL)
#define EXPRT __declspec(dllexport)
#else
#define EXPRT
#endif

/**dox***************************************************************/
#endif                          /* DOXYGEN_SHOULD_SKIP_THIS */

/*---- status codes ------------------------------------------------*/

#define MVME_SUCCESS                  1
#define MVME_NO_INTERFACE             2
#define MVME_NO_CRATE                 3
#define MVME_UNSUPPORTED              4
#define MVME_INVALID_PARAM            5
#define MVME_NO_MEM                   6
#define MVME_ACCESS_ERROR             7

/*---- types -------------------------------------------------------*/

typedef unsigned int mvme_addr_t;
typedef unsigned int mvme_locaddr_t;
typedef unsigned int mvme_size_t;

/*---- constants ---------------------------------------------------*/

/**
data modes
*/
#define MVME_DMODE_D8                 1   /**< D8 */
#define MVME_DMODE_D16                2   /**< D16 */
#define MVME_DMODE_D32                3   /**< D32 */
#define MVME_DMODE_D64                4   /**< D64 */
#define MVME_DMODE_RAMD16             5   /**< RAM memory of VME adapter */
#define MVME_DMODE_RAMD32             6   /**< RAM memory of VME adapter */
#define MVME_DMODE_LM                 7   /**< local memory mapped to VME  */

#define MVME_DMODE_DEFAULT MVME_DMODE_D32

/* block transfer modes */

#define MVME_BLT_NONE                 1   /**< normal programmed IO */
#define MVME_BLT_BLT32                2   /**< 32-bit block transfer */
#define MVME_BLT_MBLT64               3   /**< multiplexed 64-bit block transfer */
#define MVME_BLT_2EVME                4   /**< two edge block transfer */
#define MVME_BLT_2ESST                5   /**< two edge source synchrnous transfer */
#define MVME_BLT_BLT32FIFO            6   /**< FIFO mode, don't increment address */
#define MVME_BLT_MBLT64FIFO           7   /**< FIFO mode, don't increment address */
#define MVME_BLT_2EVMEFIFO            8   /**< two edge block transfer with FIFO mode */

/* vme bus address modifiers */

#define MVME_AM_A32_SB     (0x0F)      /**< A32 Extended Supervisory Block */
#define MVME_AM_A32_SP     (0x0E)      /**< A32 Extended Supervisory Program */
#define MVME_AM_A32_SD     (0x0D)      /**< A32 Extended Supervisory Data */
#define MVME_AM_A32_NB     (0x0B)      /**< A32 Extended Non-Privileged Block */
#define MVME_AM_A32_NP     (0x0A)      /**< A32 Extended Non-Privileged Program */
#define MVME_AM_A32_ND     (0x09)      /**< A32 Extended Non-Privileged Data */
#define MVME_AM_A32_SMBLT  (0x0C)      /**< A32 Multiplexed Block Transfer (D64) */
#define MVME_AM_A32_NMBLT  (0x08)      /**< A32 Multiplexed Block Transfer (D64) */

#define MVME_AM_A32     MVME_AM_A32_SD
#define MVME_AM_A32_D64 MVME_AM_A32_SMBLT

#define MVME_AM_A24_SB     (0x3F)      /**< A24 Standard Supervisory Block Transfer      */
#define MVME_AM_A24_SP     (0x3E)      /**< A24 Standard Supervisory Program Access      */
#define MVME_AM_A24_SD     (0x3D)      /**< A24 Standard Supervisory Data Access         */
#define MVME_AM_A24_NB     (0x3B)      /**< A24 Standard Non-Privileged Block Transfer   */
#define MVME_AM_A24_NP     (0x3A)      /**< A24 Standard Non-Privileged Program Access   */
#define MVME_AM_A24_ND     (0x39)      /**< A24 Standard Non-Privileged Data Access      */
#define MVME_AM_A24_SMBLT  (0x3C)      /**< A24 Multiplexed Block Transfer (D64) */
#define MVME_AM_A24_NMBLT  (0x38)      /**< A24 Multiplexed Block Transfer (D64) */

#define MVME_AM_A24     MVME_AM_A24_SD
#define MVME_AM_A24_D64 MVME_AM_A24_SMBLT

#define MVME_AM_A16_SD  (0x2D) /**< A16 Short Supervisory Data Access            */
#define MVME_AM_A16_ND  (0x29) /**< A16 Short Non-Privileged Data Access         */

#define MVME_AM_A16     MVME_AM_A16_SD

#define MVME_AM_DEFAULT MVME_AM_A32

/*---- interface structure -----------------------------------------*/
typedef struct {
   int  initialized;         /**< 1 if VME interface initialized */
   int  handle;              /**< internal handle */
   int  index;               /**< index of interface 0..n */
   void *info;               /**< internal info structure */
   int  am;                  /**< Address modifier */
   int  dmode;               /**< Data mode (D8,D16,D32,D64) */
   int  blt_mode;            /**< Block transfer mode */
   void *table;              /**< Optional table for some drivers */
} MVME_INTERFACE;

/*---- function declarations ---------------------------------------*/

/* make functions callable from a C++ program */
#ifdef __cplusplus
extern "C" {
#endif

/**dox***************************************************************/
/** @addtogroup mvmestdfunctionh
 *  
 *  @{  */

/********************************************************************/
/** VME open
The code below summarize the use of most of the mvme calls included in this
interface.
\code
#include "vmicvme.h"  // or other VME interface driver.

int main () {
  int i, status, vmeio_status, data;
  MVME_INTERFACE *myvme;

  // Open VME channel 
  status = mvme_open(&myvme, 0);

  // Reset VME 
  // Under VMIC reboot CPU!!
  //  status = mvme_sysreset(myvme);

  // Setup AM 
  status = mvme_set_am(myvme, MVME_AM_A24_ND);

  // Setup Data size 
  status = mvme_set_dmode(myvme, MVME_DMODE_D32);

  // Read VMEIO status 
  status = mvme_read(myvme, &vmeio_status, 0x78001C, 4); 
  printf("VMEIO status : 0x%x\n", vmeio_status);

  // Write Single value 
  mvme_write_value(myvme, 0x780010, 0x3);

  // Read Single Value 
  printf("Value : 0x%x\n", mvme_read_value(myvme, 0x780018));

  // Write to the VMEIO in latch mode 
  for (i=0;i<10000;i++) {
    data = 0xF;
    status = mvme_write(myvme, 0x780010, &data, 4);
    data = 0x0;
    status = mvme_write(myvme, 0x780010, &data, 4);
  }

  // Close VME channel 
  status = mvme_close(myvme);
  return 1;
}
\endcode
@param **vme user VME pointer to the interface
@param index interface number should be used to distingush multiple VME 
interface access within the same program.
@return status MVME_SUCCESS, MVME_NO_INTERFACE, MVME_INVALID_PARAM, MVME_ACCESS_ERROR
*/
int EXPRT mvme_open(MVME_INTERFACE **vme, int idx);

/********************************************************************/
/**
Close and release ALL the opened VME channel.
See example in @ref mvme_open()
@param *vme     VME structure.
@return MVME_SUCCESS, MVME_ACCESS_ERROR              
*/
int EXPRT mvme_close(MVME_INTERFACE *vme);

/********************************************************************/
/**
VME bus reset.
Effect of the VME bus reset is dependent of the type of VME interface used.
See example in @ref mvme_open()
@param *vme     VME structure.
@return MVME_SUCCESS, MVME_ACCESS_ERROR             
*/
int EXPRT mvme_sysreset(MVME_INTERFACE *vme);

/********************************************************************/
/**
Read from VME bus.
Implementation of the read can include automatic DMA transfer based on the 
size of the data. See example in @ref mvme_open()
@param *vme VME structure
@param *dst  destination pointer
@param vme_addr source address (VME location).
@param n_bytes requested transfer size, returned actual transfer size
@return MVME_SUCCESS              
*/
int EXPRT mvme_read(MVME_INTERFACE *vme, void *dst, mvme_addr_t vme_addr, mvme_size_t* n_bytes);

/********************************************************************/
/**
Read single data from VME bus.
Useful for register access. See example in @ref mvme_open()
@param *vme VME structure
@param vme_addr source address (VME location).
@return MVME_SUCCESS              
*/
unsigned int EXPRT mvme_read_value(MVME_INTERFACE *vme, mvme_addr_t vme_addr);

/********************************************************************/
/**
Write data to VME bus.
Implementation of the write can include automatic DMA transfer based on the 
size of the data. See example in @ref mvme_open()
@param *vme VME structure
@param vme_addr source address (VME location).
@param *src source array
@param n_bytes  size of the array in bytes
@return MVME_SUCCESS               
*/
int EXPRT mvme_write(MVME_INTERFACE *vme, mvme_addr_t vme_addr, void *src, mvme_size_t n_bytes);

/********************************************************************/
/**
Write single data to VME bus.
Useful for register access. See example in @ref mvme_open()
@param *vme VME structure
@param vme_addr source address (VME location).
@param value Value to be written to the VME bus
@return MVME_SUCCESS
*/
int EXPRT mvme_write_value(MVME_INTERFACE *vme, mvme_addr_t vme_addr, unsigned int value);

/********************************************************************/
/**
Set Address Modifier.
@param *vme VME structure
@param am address modifier
@return MVME_SUCCESS
*/
int EXPRT mvme_set_am(MVME_INTERFACE *vme, int am);

/********************************************************************/
/**
Get Address Modifier.
@param *vme VME structure
@param *am returned address modifier
@return MVME_SUCCESS
*/
int EXPRT mvme_get_am(MVME_INTERFACE *vme, int *am);

/********************************************************************/
/**
Set Data mode.
@param *vme VME structure
@param dmode Data mode
@return MVME_SUCCESS
*/
int EXPRT mvme_set_dmode(MVME_INTERFACE *vme, int dmode);

/********************************************************************/
/**
Get current Data mode.
@param *vme VME structure
@param *dmode returned address modifier
@return MVME_SUCCESS
*/
int EXPRT mvme_get_dmode(MVME_INTERFACE *vme, int *dmode);

/********************************************************************/
/**
Set Block Transfer mode.
@param *vme VME structure
@param mode BLT mode
@return MVME_SUCCESS
*/
int EXPRT mvme_set_blt(MVME_INTERFACE *vme, int mode);

/********************************************************************/
/**
Get current Data mode.
@param *vme VME structure
@param *mode returned BLT mode
@return MVME_SUCCESS
*/
  int EXPRT mvme_get_blt(MVME_INTERFACE *vme, int *mode);
  int EXPRT mvme_interrupt_generate(MVME_INTERFACE *mvme, int level, int vector, void *info);
  int EXPRT mvme_interrupt_attach(MVME_INTERFACE *mvme, int level, int vector,
                                  void (*isr)(int, void*, void *), void *info);
  int EXPRT mvme_interrupt_detach(MVME_INTERFACE *mvme, int level, int vector, void *info);
  int EXPRT mvme_interrupt_enable(MVME_INTERFACE *mvme, int level, int vector, void *info);
  int EXPRT mvme_interrupt_disable(MVME_INTERFACE *mvme, int level, int vector, void *info);
  
#ifdef __cplusplus
}
#endif

/**dox***************************************************************/
/** @} */ /* end of mvmestdfunctionh */
/**dox***************************************************************/
/** @} */ /* end of mvmestdinclude */

#endif // MVMESTD_H
