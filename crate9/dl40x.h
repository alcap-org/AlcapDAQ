/*----------------------------------------------------------------------------
  $Id: dl40x.h,v 1.1 2004/09/22 10:36:51 mucap Exp $
  STRUCK DL40x modules interface 
  T.Gorringe/P.Zolnierczuk  (University of Kentucky)

  $Log: dl40x.h,v $
  Revision 1.1  2004/09/22 10:36:51  mucap
  First commit of crate program for fall 2004 run.  It currently supports
  all modules in the two 6U VME crates (CAEN V767 TDC, SIS FIFO, DL40x flash
  ADCs) and the CAMAC modules that will be in the fast readout.

  Revision 1.2  2003/05/29 17:30:08  mucap
  Changed FADC readout to start at the "oldest" sample rather than the
  first byte of the buffer.

  Revision 1.1  2003/05/01 08:28:50  mucap

  FADC integration is now complete.  Piotr's old FADC code is now employed by
  crate3.cpp in order to operate the FADCs.

  Revision 1.4  2001/12/18 21:04:07  zolnie
  polishing the code

  Revision 1.3  2001/12/03 17:59:17  zolnie
  dma working now

  Revision 1.2  2001/11/26 02:48:47  zolnie
  more primitive daq SHISHSTEM

  Revision 1.1.1.1  2001/11/25 23:36:57  zolnie
  Imported sources

  Revision 1.7  2001/11/24 23:28:04  zolnie
  *** empty log message ***

  Revision 1.4  2001/10/19 17:31:48  zolnie
  working version (dl401/dl403)

  Revision 1.3  2001/10/04 23:50:06  zolnie
  further improvements

  Revision 1.2  2001/10/04 19:52:04  zolnie
  some progress

  ---------------------------------------------------------------------------*/
#ifndef STRUCK_DL40X_H
#define STRUCK_DL40X_H

/* ====================   STRUCK DL401 VME FLASH ADC ========================*/
#define DL401_MAXMODULES  16

#define DL401_NCHAN        4 /* number of inputs per module   */
#define DL401_PEDCHANBITS  6
#define DL401_PEDCHANMASK 0x3F /* pedestal mask (one chan)    */
#define DL401_PEDBITMASK  0x00800000 /* pedestal bit mask     */ 
#define DL401_PEDBITS     24 
#define DL401_PEDMASK     0x00FFFFFF /* pedestal mask         */
#define DL401_CNTMASK     0x03FF
#define DL401_CNTMAX      0x0400

/* ---------------  16 bit word access ---------------------  */
#define DL401_ICNV     0x02 /* Internal CoNVert (D0)      (R) */
#define DL401_CLIF     0x02 /* Clear Interrupt Flag       (W) */
#define DL401_WDAC     0x12 /* Write serial DAC data      (W) */
#define DL401_MRST     0x22 /* Module ReSeT               (W) */
#define DL401_BASE     0x52 /* load BASE address (0x00)   (W) */
#define DL401_MASK     0x62 /* MASK word (see below)      (W) */
#define DL401_MODE     0x72 /* MODE word (see below)    (R/W) */
#define DL401_RMAD     0x90 /* Read Memory ADdres counter (R) */


/* ---------------  DL401 MASK (0x50/0x52) BITS ------------- */
#define DL401_SMODSIZ  0x000f  /* load SubMODule SIZe (==0)   */
#define DL401_ADDRMOD  0x3f00  /* load ADDress MODifier       */
#define DL401_ADDRMSH  8       /* ADDress Modifier SHift      */
#define VME_AM_EXT_USR_DATA    0x09
#define DL401_AM_EXT_MODE (VME_AM_EXT_USR_DATA << DL401_ADDRMSH)
#define DL401_AM_STD_MODE (VME_AM_STD_USR_DATA << DL401_ADDRMSH)
#define DL401_LWIOENA  0x8000  /* Long Word I/O ENAble        */

/* ---------------  DL401 MODE (0x70/0x72) BITS ------------- */
#define DL401_IRQVEC   0x00ff  /* Interrupt VECtor      (R/W) */
#define DL401_IRQPRI   0x0700  /* Interrupt PRIority    (R/W) */
#define DL401_IRQMOD   0x0800  /* Interrupt MODe        (R/W) */

#define DL401_DEFMODE  0x0000  /* DEFault  MODe          (W)  */
#define DL401_INTMODE  0x1000  /* INTernal MODe          (W)  */
#define DL401_EXTMODE  0xefff  /* EXTernal MODe          (W)  */
#define DL401_INTGATE  0x2000  /* internal gate          (W)  */
#define DL401_EXTGATE  0xdfff  /* external gate          (W)  */
#define DL401_DACWDIS  0x4000  /* DAC Write DISable      (W)  */
#define DL401_DACWENA  0xbfff  /* DAC Write ENAble       (W)  */
#define DL401_AUTOMODE 0x8000  /* AUTOstop MODE enable   (W)  */
#define DL401_CONTMODE 0x7fff  /* CONTinuous MODE enable (W)  */

#define DL401_ASTOPACT 0x1000  /* :0 auto stop active          */
#define DL401_INTREQ   0x2000  /* :1 internal interrupt request*/
#define DL401_RDAC     0x4000  /* Read serial DAQ output       */



/* ====================   STRUCK DL403 VME 100MHz CLOCK =====================*/
#define DL403_MAXMODULES  4
#define DL403_CYCLESNUM   3

/* DL403 register offsets */
#define DL403_CYCLE1  0x02
#define DL403_CYCLE2  0x04
#define DL403_CYCLE3  0x06
#define DL403_RATE    0x08
#define DL403_START   0x0a
#define DL403_STOP    0x0c
#define DL403_MODE    0x72
#define DL403_RESET   0x22

/* DL403 clock defines */
#define DL403_EXTERN   0x00
#define DL403_SELECTED 0x01
#define DL403_50MHZ    0x02
#define DL403_100MHZ   0x03

/* DL403 modes */
#define DL403_ASYNC   0x1000
#define DL403_CONT    0x2000
#define DL403_STEADY  0x4000

#define DL403_RUN     0x1000
#define DL403_RUNSHFT 12

#define DL403_GO      0xffff /* used for start/stop commands */

INT   dl401_init        (struct dl401 *d);
void           dl401_teardown     ();
INLINE void    dl401_setup       (struct dl401 *  d, u_short bs, u_short am, u_short md);
INLINE int     dl401_setPedestals(struct dl401 * d, u_short p[], int np);
INLINE u_long  dl401_getPedestals(struct dl401 * d);
INLINE u_short dl401_getCounts   (struct dl401 * d);
INLINE u_short dl401_getMode     (struct dl401 * d);
INLINE u_short dl401_getMask     (struct dl401 * d);
INLINE void    dl401_clear       (struct dl401 * d);
INLINE void    dl401_reset       (struct dl401 * d);
INLINE void    dl401_convert     (struct dl401 * d);
INLINE int     dl401_finished    (struct dl401 * d);
INLINE int     dl401_read        (struct dl401 * d, u_int *data);
INLINE int     dl401_dmaread     (struct dl401 * d, u_int *data);

INT dl403_init         (struct dl403 *d);

void         dl403_setClockRate (struct dl403 * d, u_short clk1, u_short clk2, u_short clk3);
void         dl403_setClockCycle(struct dl403 * d, u_short cyc1, u_short cyc2, u_short cyc3);
INLINE void  dl403_asyncMode    (struct dl403 * d);
INLINE void  dl403_contMode     (struct dl403 * d);
INLINE void  dl403_steadyMode   (struct dl403 * d); 
INLINE short dl403_runBit       (struct dl403 * d);
INLINE void  dl403_start        (struct dl403 * d);
INLINE void  dl403_stop         (struct dl403 * d); 
INLINE void  dl403_reset        (struct dl403 * d); 
INLINE short dl403_running      (struct dl403 * d);


#endif
