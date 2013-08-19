/*-----------------------------------------------------------------------------
 *  Copyright (c) 1998      TRIUMF Data Acquistion Group
 *  Please leave this header in any reproduction of that distribution
 * 
 *  TRIUMF Data Acquisition Group, 4004 Wesbrook Mall, Vancouver, B.C. V6T 2A3
 *  Email: online@triumf.ca           Tel: (604) 222-1047  Fax: (604) 222-1074
 *         amaudruz@triumf.ca
 * -----------------------------------------------------------------------------
 *  
 *  Description	: MCStd compliant CAMAC device driver for Linux.
 *  Requires 	: requires the driver specific include file linux-camac.h
 *  Application : Linux
 *  Author      : Pierre-Andre Amaudruz Data Acquisition Group
 *  Revision    : 1.0  1998        Pierre	 Initial revision
 *                include linux-camac.h from linux drivers
 *  $Log: camaclx.c,v $
 *  Revision 1.1.1.1  2005/06/20 23:37:06  mucap
 *  Importing release 1.9.5 of the MIDAS source code as distributed by PSI.
 *  (Next, I'll commit our local customizations.)
 *
 *  Revision 1.4  2004/01/08 08:40:08  midas
 *  Implemented standard indentation
 *
 *  Revision 1.3  2002/01/24 22:24:44  pierre
 *  adjust for new esone
 *
 *  Revision 1.2  2000/09/07 17:16:46  pierre
 *  -Add fe_name
 *
 *  Revision 1.1  1999/12/20 10:18:11  midas
 *  Reorganized driver directory structure
 *
 *  Revision 1.5  1999/07/22 19:19:26  pierre
 *  - implemented cam16o_r, cam24o_r
 *
 *  Revision 1.4  1999/05/06 18:40:35  pierre
 *  - remove came_xxx
 *
 *  Revision 1.3  1998/11/23 17:45:32  pierre
 *  cleanup variables, remove test func()
 *
 *  Revision 1.2  1998/10/09 23:17:41  midas
 *  -PAA- Made OS_LINUX dependent
 *
 *  Revision 1.1  1998/10/09 22:43:35  midas
 *  -PAA- MCStd compliant for Linux driver
 *
 *
 *---------------------------------------------------------------------------*/

#ifndef INLINE
#if defined( _MSC_VER )
#define INLINE __inline
#elif defined(__GNUC__)
#define INLINE __inline__
#else
#define INLINE
#endif
#endif

#include "midas.h"
#include <stdio.h>
#include "mcstd.h"

#ifdef OS_LINUX
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>

#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

#ifndef DEVNAME
#define DEVNAME		"/dev/camac"
#endif
#include <unistd.h>

/************** start of linux-camac.h ***********************/
#ifndef _LINUX_CAMAC_H
#define _LINUX_CAMAC_H

#define Q_STOP		0
#define Q_IGNORE	1
#define Q_REPEAT	2
#define Q_SCAN		3

typedef unsigned long cdata_t;

/* Structure for single camac command ioctl()s */
typedef struct camac_t {
   cdata_t d;                   /* Data for read or write       */
   unsigned char n;             /* Device number                */
   unsigned char a;             /* Subaddress                   */
   unsigned char f;             /* Camac function               */
   unsigned int q;              /* Status flags                 */
   unsigned int x;
} camac_t;

/* Structure for single camac command ioctl()s */
typedef struct camacblock_t {
   char *buffer;                /* Data for read or write       */
   size_t count;                /* Bytes to transfer            */
   unsigned int bits;           /* Bits per transfer word       */
   unsigned int type;           /* Block transfer type          */
   unsigned char n;             /* Device number                */
   unsigned char a;             /* Subaddress                   */
   unsigned char f;             /* Camac function               */
   int status;                  /* Return status of transfer    */
} camacblock_t;

/* Structures for camac command list processing */
typedef struct camac_cmd_t {
   unsigned char n;
   unsigned char a;
   unsigned char f;
} camac_cmd_t;

typedef struct camaclist_t {
   int n;                       /* Number of commands in list   */
   camac_cmd_t *cmd;            /* Array of camac commands      */
   cdata_t *dbuf;               /* Data buffer for read/write   */
} camaclist_t;

/* The magic number for KCS ioctl codes
   - see /usr/src/linux/MAGIC & /usr/src/linux/include/asm/iotcl.h */
#ifndef CAMAC_MAGIC
#define CAMAC_MAGIC		'c'
#endif

/* Ioctl values - I'm using the ioctl numbering scheme described in
 * /usr/src/linux/MAGIC and /usr/src/linux/include/asm/ioctl.h */

/* Generic ioctl codes to control data acquisition */
#define CAMAC_INIT		_IO  ( CAMAC_MAGIC, 1 )
#define CAMAC_CRATE_SELECT	_IO  ( CAMAC_MAGIC, 2 )
#define CAMAC_ZCYCLE		_IO  ( CAMAC_MAGIC, 3 )
#define CAMAC_CCYCLE		_IO  ( CAMAC_MAGIC, 4 )
#define CAMAC_SETINH		_IO  ( CAMAC_MAGIC, 5 )
#define CAMAC_CLRINH		_IO  ( CAMAC_MAGIC, 6 )
#define CAMAC_EXEC		_IOWR( CAMAC_MAGIC, 7, camac_t )
#define CAMAC_BLK_TRANSFER	_IOWR( CAMAC_MAGIC, 8, camacblock_t )
#define CAMAC_STATUS    	_IOWR( CAMAC_MAGIC, 9, camac_t )
#define CAMAC_ENABLELAM    	_IOW ( CAMAC_MAGIC, 10, camac_t )
#define CAMAC_READ_LIST		_IOW ( CAMAC_MAGIC, 20, camaclist_t )
#define CAMAC_WRITE_LIST	_IOW ( CAMAC_MAGIC, 21, camaclist_t )
/*-PAA-*/
#define CAMAC_LAM_READ    	_IOWR( CAMAC_MAGIC, 22, camac_t  )
#define CAMAC_LAM_ENABLE	_IOW ( CAMAC_MAGIC, 23, camac_t  )
#define CAMAC_LAM_DISABLE	_IOW ( CAMAC_MAGIC, 24, camac_t  )
#define CAMAC_LAM_CLEAR 	_IOW ( CAMAC_MAGIC, 25, camac_t  )

#ifdef __KERNEL__

/* The timeout value while waiting for acknowledge from camac (seconds ) */
#define	CAMAC_TIMEOUT	1

/* Static functions don't show up in ksyms - make external for debugging */
#ifndef STATIC
#define STATIC static
#endif

typedef struct CAMAC_t {
   int loaded;                  /* Is the device loaded         */
   int present;                 /* A card is present            */
   int open;                    /* The device is open           */
   int stat;                    /* Device status                */
   int dmaen;                   /* Enable DMA transfers         */
   int sharedma;                /* Share the DMA channel        */
   /* Data acquisition buffer */
   char *buffer;
   int bufsize;                 /* Size of buffers              */
   /* Device driver resources */
   int base;                    /* IO port base address         */
   int irq;                     /* Interrupt number             */
   int ownirq;                  /* Do we own the IRQ            */
   int dma;                     /* DMA channel                  */
   int owndma;                  /* Do we own the dma channel    */
   struct semaphore sem;        /* Semaphore to lock port access */
   struct wait_queue *wait_lam; /* Wait queue for LAM interrupts */
   struct wait_queue *wait_dma; /* Wait queue for blk transfers */
} CAMAC_t;

/* Functions to read and write to/from the card io ports */
static inline void write_port(CAMAC_t * camac, unsigned char data, int port)
{
   outb(data, camac->base + port);
}

static inline unsigned char read_port(CAMAC_t * camac, int port)
{
   return inb(camac->base + port);
}

/* Functions to read and write to/from the card io ports in 16bit wide fmt */
static inline void writew_port(CAMAC_t * camac, unsigned int data, int port)
{
   outw((unsigned int) data, camac->base + port);
}

static inline unsigned int readw_port(CAMAC_t * camac, int port)
{
   return inw(camac->base + port);
}

/* The functions which implement the camac functions */
/* Each of these functions return non-zero on error */
/*-PAA-*/
int creadlam(CAMAC_t * camac, cdata_t * d);
int cenalam(CAMAC_t * camac, int n);
int cdislam(CAMAC_t * camac, int n);
int cclearlam(CAMAC_t * camac, int n);

int cselect(CAMAC_t * camac, int c);
int cinit(CAMAC_t * camac);
int zcycle(CAMAC_t * camac);
int ccycle(CAMAC_t * camac);
int setinh(CAMAC_t * camac);
int clrinh(CAMAC_t * camac);
void chkxq(CAMAC_t * camac, int *x, int *q);
int cread(CAMAC_t * camac, unsigned char n, unsigned char a,
          unsigned char f, cdata_t * d);
int cwrite(CAMAC_t * camac, unsigned char n, unsigned char a, unsigned char f, cdata_t d);
int cexecute(CAMAC_t * camac, unsigned char n, unsigned char a, unsigned char f);
int cblkstart(CAMAC_t * camac, int type, int bits, unsigned long nb);
void cblkgo(CAMAC_t * camac, unsigned char n, unsigned char a, unsigned char f);
void cblkend(CAMAC_t * camac);
int cblkstatus(CAMAC_t * camac);
int clamstatus(CAMAC_t * camac);

#ifdef DEBUG_DRIVER
#define	dprintk( a )		printk a
#else                           /* DEBUG_DRIVER */
#define dprintk( a )
#endif                          /* DEBUG_DRIVER */

/* verify_ioctl() checks `arg' is a valid data pointer for access
 * to user provided data.
 * This uses the ioctl numbering convention provided by the _IOXX macros
 * See /usr/include/asm/ioctl.h */
extern inline int ioctl_verify(unsigned int cmd, unsigned long arg)
{
   int stat;

   if (cmd & IOC_IN) {
      stat = verify_area(VERIFY_READ, (void *) arg, _IOC_SIZE(cmd));
      if (stat)
         return stat;
   }
   if (cmd & IOC_OUT) {
      stat = verify_area(VERIFY_WRITE, (void *) arg, _IOC_SIZE(cmd));
      if (stat)
         return stat;
   }
   return 0;
}

#endif                          /* __KERNEL__ */

#endif                          /* _LINUX_CAMAC_H */

/************** end of linux-camac.h ***********************/
char *lx_filename = DEVNAME;

static int fd = -1;
static char *fname = NULL;

static void eprintf(const char *fmt, ...)
{
   va_list ap;                  /* variable argument lists      */

   va_start(ap, fmt);
   vfprintf(stderr, fmt, ap);
   va_end(ap);
}

/* Select camac crate number `c' */
INLINE int cselect(int c)
{
   int stat;

   if ((stat = ioctl(fd, CAMAC_CRATE_SELECT, c)) != 0)
      perror(fname);
   return stat;
}

/*-input---------------------------------------------------------*/
INLINE void cam16i(const int c, const int n, const int a, const int f, WORD * d)
{
   cam24i(c, n, a, f, (DWORD *) d);
}

/*--input--------------------------------------------------------*/
INLINE void cam24i(const int c, const int n, const int a, const int f, DWORD * d)
{
   int stat;
   camac_t c_t;

   cselect(c);
   c_t.n = n;
   c_t.a = a;
   c_t.f = f;

   if ((stat = ioctl(fd, CAMAC_EXEC, &c_t)) != 0) {
      perror(fname);
   }
   *d = c_t.d;
}

/*--input--------------------------------------------------------*/
INLINE void cam16i_q(const int c, const int n, const int a, const int f, WORD * d, int *x,
                     int *q)
{
   cam24i_q(c, n, a, f, (DWORD *) d, x, q);
}

/*--input--------------------------------------------------------*/
INLINE void cam24i_q(const int c, const int n, const int a, const int f, DWORD * d,
                     int *x, int *q)
{
   int stat;
   camac_t c_t;

   cselect(c);
   c_t.n = n;
   c_t.a = a;
   c_t.f = f;
   if ((stat = ioctl(fd, CAMAC_EXEC, &c_t)) != 0) {
      perror(fname);
   }
   *d = c_t.d;
   *x = c_t.x;
   *q = c_t.q;
}

/*--input--------------------------------------------------------*/
INLINE void cam16i_r(const int c, const int n, const int a, const int f, WORD ** d,
                     const int r)
{
   printf("cam16i_r not yet implemented\n");
}

/*--input--------------------------------------------------------*/
INLINE void cam24i_r(const int c, const int n, const int a, const int f, DWORD ** d,
                     const int r)
{
   printf("cam24i_r not yet implemented\n");
}

/*--input--------------------------------------------------------*/
INLINE void cam16i_rq(const int c, const int n, const int a, const int f, WORD ** d,
                      const int r)
{
   printf("cam16i_rq not yet implemented\n");
}

/*--input--------------------------------------------------------*/
INLINE void cam24i_rq(const int c, const int n, const int a, const int f, DWORD ** d,
                      const int r)
{
   printf("cam24i_rq not yet implemented\n");
}

/*--input--------------------------------------------------------*/
INLINE void cam16i_sa(const int c, const int n, const int a, const int f, WORD ** d,
                      const int r)
{
   printf("cam16i_sa not yet implemented\n");
}

/*--input--------------------------------------------------------*/
INLINE void cam24i_sa(const int c, const int n, const int a, const int f,
                      unsigned long **d, const int r)
{
   printf("cam24i_sa not yet implemented\n");
}

/*--input--------------------------------------------------------*/
INLINE void cam16i_sn(const int c, const int n, const int a, const int f, WORD ** d,
                      const int r)
{
   printf("cam16i_sn not yet implemented\n");
}

/*--input--------------------------------------------------------*/
INLINE void cam24i_sn(const int c, const int n, const int a, const int f, DWORD ** d,
                      const int r)
{
   printf("cam24i_sn not yet implemented\n");
}

/*--input--------------------------------------------------------*/
INLINE void cami(const int c, const int n, const int a, const int f, WORD * d)
{
   cam16i(c, n, a, f, d);
}

/*--output-------------------------------------------------------*/
INLINE void cam16o(const int c, const int n, const int a, const int f, WORD d)
{
   cam24o(c, n, a, f, (long) d);
}

/*--output-------------------------------------------------------*/
INLINE void cam24o(const int c, const int n, const int a, const int f, DWORD d)
{
   int stat;
   camac_t c_t;

   cselect(c);
   c_t.n = n;
   c_t.a = a;
   c_t.f = f;
   c_t.d = d;
   if ((stat = ioctl(fd, CAMAC_EXEC, &c_t)) != 0) {
      perror(fname);
   }
}

/*--output-------------------------------------------------------*/
INLINE void cam16o_q(const int c, const int n, const int a, const int f, WORD d, int *x,
                     int *q)
{
   cam24o_q(c, n, a, f, (long) d, x, q);
}

/*--output-------------------------------------------------------*/
INLINE void cam24o_q(const int c, const int n, const int a, const int f, DWORD d, int *x,
                     int *q)
{
   int stat;
   camac_t c_t;

   cselect(c);
   c_t.n = n;
   c_t.a = a;
   c_t.f = f;
   c_t.d = d;
   if ((stat = ioctl(fd, CAMAC_EXEC, &c_t)) != 0) {
      perror(fname);
   }
   *x = c_t.x;
   *q = c_t.q;
}

/*--output-------------------------------------------------------*/
INLINE void camo(const int c, const int n, const int a, const int f, WORD d)
{
   cam16o(c, n, a, f, d);
}

/*------------------------------------------------------------------*/

void cam16o_r(const int c, const int n, const int a, const int f, WORD * d, const int r)
{
   INT i;
   for (i = 0; i < r; i++)
      cam16o(c, n, a, f, *d++);
}

/*------------------------------------------------------------------*/
void cam24o_r(const int c, const int n, const int a, const int f, DWORD * d, const int r)
{
   INT i;
   for (i = 0; i < r; i++)
      cam24o(c, n, a, f, *d++);
}

/*--command------------------------------------------------------*/
INLINE int camc_chk(const int c)
{
   printf("camc_chk not yet implemented\n");
   return 0;
}

/*--command------------------------------------------------------*/
INLINE void camc(const int c, const int n, const int a, const int f)
{
   INT d;
   cam16o(c, n, a, f, d);
}

/*--command------------------------------------------------------*/
INLINE void camc_q(const int c, const int n, const int a, const int f, int *q)
{
   printf("camc_q not yet implemented\n");
}

/*--command------------------------------------------------------*/
INLINE void camc_sa(const int c, const int n, const int a, const int f, const int r)
{
   printf("camc_sa not yet implemented\n");
}

/*--command------------------------------------------------------*/
INLINE void camc_sn(const int c, const int n, const int a, const int f, const int r)
{
   printf("camc_sn not yet implemented\n");
}

/*--General functions--------------------------------------------*/
INLINE int cam_init(void)
{
   int stat;

   if (fd >= 0) {
      eprintf("copen(): `%s' already open: use cclose() to close\n", lx_filename);
      return -1;
   }
   fname = strdup(lx_filename);
   if ((fd = open(fname, O_RDWR)) < 0) {
      perror(fname);
      return fd;
   }
   if ((stat = ioctl(fd, CAMAC_INIT)) != 0)
      perror(fname);
   if (stat != 0)
      return -1;

   stat = cselect(0);
   printf("cselect for crate 0:%x\n", stat);
   return SUCCESS;
}

/*--General functions--------------------------------------------*/
INLINE void cam_exit(void)
{
   if (fd < 0) {
      eprintf("cclose(): No file open\n");
   }
   close(fd);
   fd = -1;
   if (fname)
      free(fname);
   fname = NULL;
}

/*--General functions--------------------------------------------*/
INLINE void cam_inhibit_set(const int c)
{
   int stat;

   cselect(c);
   if ((stat = ioctl(fd, CAMAC_SETINH)) != 0)
      perror(fname);
}

/*--General functions--------------------------------------------*/
INLINE int cam_inhibit_test(const int c)
{
}

/*--General functions--------------------------------------------*/
INLINE void cam_inhibit_clear(const int c)
{
   int stat;

   cselect(c);
   if ((stat = ioctl(fd, CAMAC_CLRINH)) != 0)
      perror(fname);
}

/*--General functions--------------------------------------------*/
INLINE void cam_crate_clear(const int c)
{
   int stat;

   cselect(c);
   if ((stat = ioctl(fd, CAMAC_CCYCLE)) != 0)
      perror(fname);
}

/*--General functions--------------------------------------------*/
INLINE void cam_crate_zinit(const int c)
{
   int stat;

   cselect(c);
   if ((stat = ioctl(fd, CAMAC_ZCYCLE)) != 0)
      perror(fname);
}

/*--General functions--------------------------------------------*/
INLINE void cam_lam_enable(const int c, const int n)
{
   int stat;
   camac_t c_t;

   cselect(c);
   c_t.n = n;
   if ((stat = ioctl(fd, CAMAC_LAM_ENABLE, &c_t)) != 0) {
      perror(fname);
   }
}

/*--General functions--------------------------------------------*/
INLINE void cam_lam_disable(const int c, const int n)
{
   int stat;
   camac_t c_t;

   /* see comment in cam_lam_enable */
   cselect(c);
   c_t.n = n;
   if ((stat = ioctl(fd, CAMAC_LAM_DISABLE, &c_t)) != 0) {
      perror(fname);
   }
}

/*--General functions--------------------------------------------*/
INLINE void cam_lam_read(const int c, DWORD * lam)
{
   int stat;
   camac_t c_t;

   cselect(c);
   if ((stat = ioctl(fd, CAMAC_LAM_ENABLE, &c_t)) != 0) {
      perror(fname);
   }
   *lam = c_t.d;
}

/*--General functions--------------------------------------------*/
INLINE void cam_lam_clear(const int c, const int n)
{
   int stat;
   camac_t c_t;

   cselect(c);
   c_t.n = n;
   if ((stat = ioctl(fd, CAMAC_LAM_CLEAR, &c_t)) != 0) {
      perror(fname);
   }
}


/*--Interrupt functions------------------------------------------*/
INLINE void cam_interrupt_enable(const int c)
{
   printf("cam_interrupt_enable not yet implemented\n");
}

/*--Interrupt functions------------------------------------------*/
INLINE void cam_interrupt_disable(const int c)
{
   printf("cam_interrupt_disable not yet implemented\n");
}

/*--Interrupt functions------------------------------------------*/
INLINE void cam_interrupt_attach(const int c, const int n, void (*isr) (void))
{
   printf("cam_interrupt_attach not yet implemented\n");
}

/*--Interrupt functions------------------------------------------*/
INLINE void cam_interrupt_detach(const int c, const int n)
{
   printf("cam_interrupt_detach not yet implemented\n");
}

/*--Interrupt functions------------------------------------------*/
INLINE int cam_interrupt_test(const int c)
{
}

/*---------------------------------------------------------------*/

/*--Interrupt functions------------------------------------------*/
INLINE void cam_glint_enable(void)
{
   printf("cam_glint_enable not yet implemented\n");
}

/*--Interrupt functions------------------------------------------*/
INLINE void cam_glint_disable(void)
{
   printf("cam_glint_disable not yet implemented\n");
}

INLINE void cam_glint_attach(int lam, void (*isr) (void))
{
   printf("cam_glint_attach not yet implemented\n");
}

/*--Extra functions------------------------------------------*/
int cam_init_rpc(char *host_name, char *exp_name, char *fe_name, char *client_name,
                 char *rpc_server)
{
   return SUCCESS;
}

int cwritelist(camaclist_t * clist)
{
   int stat;

   if ((stat = ioctl(fd, CAMAC_WRITE_LIST, clist)) != 0) {
      perror(fname);
      return stat;
   }
   return 0;
}

int creadlist(camaclist_t * clist)
{
   int stat;

   if ((stat = ioctl(fd, CAMAC_READ_LIST, clist)) != 0) {
      perror(fname);
      return stat;
   }
   return 0;
}

int cblock(int n, int a, int f, int mode, int bits, void *buffer, long *count)
{
   camacblock_t cblock;
   int stat;

   cblock.buffer = buffer;
   cblock.count = *count;
   cblock.n = n;
   cblock.a = a;
   cblock.f = f;
   cblock.bits = bits;
   cblock.type = mode;
   cblock.status = 0;
   if ((stat = ioctl(fd, CAMAC_BLK_TRANSFER, &cblock)) != 0) {
      perror(fname);
      return stat;
   }
   *count = cblock.count;
   return 0;
}
#else
INLINE void cam16i_q(const int c, const int n, const int a, const int f, WORD * d, int *x,
                     int *q)
{
   *d = 0;
   *q = *x = 1;
}
INLINE void cam24i_q(const int c, const int n, const int a, const int f, DWORD * d,
                     int *x, int *q)
{
   *d = 0;
   *q = *x = 1;
}
INLINE void cam16o_q(const int c, const int n, const int a, const int f, WORD d, int *x,
                     int *q)
{
   *q = *x = 1;
}
INLINE void cam24o_q(const int c, const int n, const int a, const int f, DWORD d, int *x,
                     int *q)
{
   *q = *x = 1;
}
INLINE int cam_init(void)
{
   printf(" NOT OS_LINUX ==> simulation \n");
   return 1;
}
INLINE void cam_exit(void)
{
   printf(" WAS NOT OS_LINUX ==> simulation \n");
}
INLINE void cam_inhibit_set(const int c)
{
}
INLINE void cam_inhibit_clear(const int c)
{
}
INLINE void cam_crate_clear(const int c)
{
}
INLINE void cam_crate_zinit(const int c)
{
}
INLINE void cam_lam_enable(const int c, const int n)
{
}
INLINE void cam_lam_disable(const int c, const int n)
{
}
INLINE void cam_lam_read(const int c, DWORD * lam)
{
}
INLINE void cam_lam_clear(const int c, const int n)
{
}
#endif
