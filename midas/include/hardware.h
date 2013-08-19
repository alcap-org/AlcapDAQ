/********************************************************************\

  Name:         HARDWARE.H
  Created by:   Stefan Ritt

  Contents:     Type ID definitions and structured for specific
                hardware.

  $Log: hardware.h,v $
  Revision 1.1.1.1  2005/06/20 23:37:14  mucap
  Importing release 1.9.5 of the MIDAS source code as distributed by PSI.
  (Next, I'll commit our local customizations.)

  Revision 1.6  2004/04/30 07:27:20  midas
  Cleanup

  Revision 1.5  2004/01/08 08:40:09  midas
  Implemented standard indentation

  Revision 1.4  2003/11/19 11:22:18  midas
  Added CAEN775 TDC

  Revision 1.3  2003/09/29 07:59:46  midas
  *** empty log message ***

  Revision 1.2  1998/10/12 12:18:59  midas
  Added Log tag in header


\********************************************************************/

/* Hardware data types. Can be used to create MIDAS banks. Should
   be or'ed with standard data types like TID_DWORD | TID_LRS1882 */

#define TID_LRS1882    (1<<8)   /* LeCroy 1882 fastbus ADC */
#define TID_LRS1877    (2<<8)   /* LeCroy 1877 fastbus TDC */
#define TID_PCOS3      (3<<8)   /* LeCroy PCOS 3 wire chambers */
#define TID_LRS1875    (4<<8)   /* LeCroy 1875 fastbus TDC */

/* Hardware data records */

typedef struct {
   WORD data:12;
    WORD:4;
   WORD channel:7;
   WORD range:1;
   WORD event:3;
   WORD geo_addr:5;
} LRS1882_DATA;

typedef struct {
   WORD data:16;
   WORD edge:1;
   WORD channel:7;
   WORD buffer:2;
   WORD parity:1;
   WORD geo_addr:5;
} LRS1877_DATA;

typedef struct {
   WORD count:11;
   WORD buffer:3;
    WORD:1;
   WORD parity:1;
    WORD:11;
   WORD geo_addr:5;
} LRS1877_HEADER;

typedef struct {
  WORD data     : 12;
  WORD          : 4;
  WORD channel  : 6;
  WORD          : 1;
  WORD range    : 1;
  WORD event    : 3;
  WORD geo_addr : 5;
} LRS1875_DATA;

typedef struct {
  WORD          : 8;
  WORD count    : 6; 
  WORD          : 2; 
  WORD crate    : 8; 
  WORD tag      : 3;
  WORD geo_addr : 5; 
} CAEN775_HEADER;

typedef struct {
  WORD data     : 12;
  WORD overflow : 1;
  WORD underflow: 1;
  WORD valid    : 1;
  WORD          : 1;
  WORD channel  : 6;
  WORD          : 2;
  WORD tag      : 3;
  WORD geo_addr : 5;
} CAEN775_DATA;
