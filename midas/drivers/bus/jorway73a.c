/********************************************************************

Name:         jorway73a.c
Created by:   Greg Hackman

(based on kcs2927.c by P.-A. A)

Contents:     LINUX Device driver for Jorway 73A
following the MIDAS CAMAC Standard for DirectIO

NOTE:  In this driver code, the number that you pass as "crate"
should be interpreted as the SCSI host adapter channel number 
times 8 plus the thumbwheel setting.  Most systems will only 
have one SCSI adapter in which case the "crate" becomes just the 
thumbwheel setting.  To confrim this, "cat /proc/scsi/scsi" will
return something that looks like:
Host: scsi0 Channel: 00 Id: 01 Lun: 00
Vendor: JORWAY   Model: 73A              Rev: 300  
Type:   Processor                        ANSI SCSI revision: 02
In this case there is only one SCSI host adapter and the thumbwheel
on the 73A had been set to 1, so this 73A controls "crate" 1.
Note that -- depending on the order in which things are set up
during the boot process -- the channel number may not be 00; for example
if the "iSCSI" module gets installed before the "real" SCSI devices
are installed, then the channel number may be 1.

Note that the definitions of "branch" and "crate" that are used in
the Jorway documentation, and in the Fermitools software on which this
driver is based, are different than what is used in the MIDAS CAMAC
standard software.

THE NODMA FLAG:  If you define NODMA (e.g with -DNODMA when you
compile), then repeated CAMAC accesses are implemented as a bunch
of single-datum transfers, rather than as a block read or write.
The NODMA transfers are *much* slower than when this option is
not set.  All SCSI transfers involve about a 1 ms overhead before
any data is transfered, but if you do block transfers the data
is transferred at pretty much the CAMAC maximum speed of
one datum per 1.3 us.  So for example reading a full 4302 memory
takes almost 12 seconds in NODMA mode but occurs in a blistering
15 ms if block transfers are enabled.

$Log: jorway73a.c,v $
Revision 1.1.1.1  2005/06/20 23:37:07  mucap
Importing release 1.9.5 of the MIDAS source code as distributed by PSI.
(Next, I'll commit our local customizations.)

Revision 1.3  2004/01/08 08:40:08  midas
Implemented standard indentation

Revision 1.2  2004/01/08 06:49:58  pierre
fix small bugs

Revision 1.1  2002/05/08 22:21:30  pierre
initial version


\********************************************************************/

#include <stdio.h>
#include "midas.h"
#include "mcstd.h"
#define SJY73A
#define XDEFLG
#include "jorway73a.h"
#undef XDEFLG
/*------------------------------------------------------------------*/

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
#elif defined(OS_LINUX)
#include <asm/io.h>
#include <unistd.h>
#define OUTP(_p, _d) outb(_d, _p)
#define OUTPW(_p, _d) outw(_d, _p)
#define INP(_p) inb(_p)
#define INPW(_p) inw(_p)
#endif

#define NAFCMD(n,a,f) ( (SJY_CAMAC_EXT((f),(n),(a),1)) )        /* Crate must = 1 */

/* #define NODMA */
/* Don't Define the above if you want to use the 73A's DMA modes (Qstop)
or don't define it if you want to just mimic q-stop with a bunch
of single reads in a loop. */

int xdummy, qdummy;             /* for calls where x, q aren't important */
int ieee_last_Q = 0;            /* for compatibility with get_qx */
int ieee_last_X = 0;
WORD d16dummy;                  /* for calls where data isn't important */
DWORD d24dummy;                 /* ditto */

int jorway_crate_opened[8] = { 0 };     /* Set to 1 if crate has been opened */
DWORD jorway_crate_LAM[8] = { 0 };      /* There doesn't seem to ba a cmd    */
/* to read LAM mask off 73A.  Need   */
/* to keep track of it manually.     */

unsigned int cdchn(int branch, int channel, int route);

/* Taking advantage of the various modes the SCSI controller actually */
/* supports ... */

/*------------------------------------------------------------------*/
INLINE void cam8i(const int c, const int n, const int a, const int f, unsigned char *d)
{
}

/*------------------------------------------------------------------*/
INLINE void cami(const int c, const int n, const int a, const int f, WORD * d)
{
   cam16i(c, n, a, f, d);
}

/*------------------------------------------------------------------*/
INLINE void cam16i(const int c, const int n, const int a, const int f, WORD * d)
{
   int cmd;
   unsigned short locdat[2] = { 0, 0 };
   cmd = NAFCMD(n, a, f);       /* get cmd to pass to sjy_read */
   /* According to Jorway docx, stop-on-word mode will xfer data */
   /* EVEN IF q=0.  I think this is consistent with what I was   */
   /* seeing with the 4434 module.                               */
   cmd = cmd | (SJY_STOP_ON_WORD << 16);
   sjy_read(c, cmd, &locdat[0], sizeof(WORD));
   *d = locdat[0];
}

/*------------------------------------------------------------------*/
INLINE void cam24i(const int c, const int n, const int a, const int f, DWORD * d)
{
   int cmd;                     /* Take a page from sjy_cfsa.c */
   cmd = NAFCMD(n, a, f);       /* get cmd to pass to sjy_read */
   /* See above notes regarding stop_on_word mode */
   cmd = cmd | (SJY_STOP_ON_WORD << 16) | SJY_D24_MODE;
   sjy_read(c, cmd, d, sizeof(DWORD));
}

/*------------------------------------------------------------------*/
INLINE void cam8i_q(const int c, const int n, const int a, const int f,
                    unsigned char *d, int *x, int *q)
{
   fprintf(stderr, "cam8i_q not implemented\n");
}

/*------------------------------------------------------------------*/
INLINE void cam16i_q(const int c, const int n, const int a, const int f,
                     WORD * d, int *x, int *q)
{
   int cmd;                     /* Take a page from sjy_cfsa.c */
   unsigned short locdat[2] = { 0, 0 };
   cmd = NAFCMD(n, a, f);       /* get cmd to pass to sjy_read */
   /* According to Jorway docx, I should probably use Q_STOP mode even */
   /* if it's only for one word, otherwise the controller responds as if */
   /* q=1, even if it wasn't.   This is confirmed by Radfords code. */
   cmd = cmd | (SJY_Q_STOP << 16);
   sjy_read(c, cmd, &locdat[0], sizeof(WORD));
   *d = locdat[0];
   *q = sjy_get_qx(c);
   *x = ieee_last_X;            /* Need to get this from global variables I guess ... */
}

/*------------------------------------------------------------------*/
INLINE void cam24i_q(const int c, const int n, const int a, const int f,
                     DWORD * d, int *x, int *q)
{
   int cmd;                     /* Take a page from sjy_cfsa.c */
   cmd = NAFCMD(n, a, f);       /* get cmd to pass to sjy_read */
   /* See above discussions regarding modes */
   cmd = cmd | (SJY_Q_STOP << 16) | SJY_D24_MODE;
   sjy_read(c, cmd, d, sizeof(DWORD));
   *q = sjy_get_qx(c);
   *x = ieee_last_X;            /* Need to get this from global variables I guess ... */
}

/*------------------------------------------------------------------*/
INLINE void cam16i_r(const int c, const int n, const int a, const int f,
                     WORD ** d, const int r)
{
#ifndef NODMA
   int cmd;
   int xferlen, status;
   cmd = NAFCMD(n, a, f);       /* get cmd to pass to sjy_write */
   cmd |= (SJY_Q_REPEAT << 16); /* set q-repeat mode */
   xferlen = r * sizeof(WORD);  /* size, in bytes, of stuff to write */
   status = sjy_read(c, cmd, *d, xferlen);
   *d += r;                     /* assume everything got xfered */
#else
   int i;
   for (i = 0; i < r; i++) {
      cam16i(c, n, a, f, *d);
      (*d)++;
   }
#endif
}

/*------------------------------------------------------------------*/
INLINE void cam24i_r(const int c, const int n, const int a, const int f,
                     DWORD ** d, const int r)
{
#ifndef NODMA
   int cmd;
   int xferlen, status;
   cmd = NAFCMD(n, a, f) | SJY_D24_MODE;        /* get cmd to pass to sjy_write */
   cmd |= (SJY_Q_REPEAT << 16); /* set q-repeat mode */
   xferlen = r * sizeof(DWORD); /* size, in bytes, of stuff to write */
   status = sjy_read(c, cmd, *d, xferlen);
   *d += r;                     /* assume everything got xfered... */
#else
   int i;
   for (i = 0; i < r; i++) {
      cam24i(c, n, a, f, *d);
      (*d)++;
   }
#endif
}

/*------------------------------------------------------------------*/
INLINE void cam16i_rq(const int c, const int n, const int a, const int f,
                      WORD ** d, const int r)
{
#ifndef NODMA
   int cmd;
   int xferlen, status;
   cmd = NAFCMD(n, a, f);       /* get cmd to pass to sjy_write */
   cmd |= (SJY_Q_STOP << 16);   /* set q-repeat mode */
   xferlen = r * sizeof(WORD);  /* size, in bytes, of stuff to write */
   status = sjy_read(c, cmd, *d, xferlen);
   *d += (status / sizeof(WORD));
#else
   int i, x, q;
   for (i = 0; i < r; i++) {
      cam16i_q(c, n, a, f, (*d)++, &x, &q);
      if (!q) {
         (*d)--;
         break;
      }
   }
#endif
}

/*------------------------------------------------------------------*/
INLINE void cam24i_rq(const int c, const int n, const int a, const int f,
                      DWORD ** d, const int r)
{
#ifndef NODMA
   int cmd;
   int xferlen, status;
   cmd = NAFCMD(n, a, f) | SJY_D24_MODE;        /* get cmd to pass to sjy_write */
   cmd |= (SJY_Q_STOP << 16);   /* set q-repeat mode */
   xferlen = r * sizeof(DWORD); /* size, in bytes, of stuff to write */
   fprintf(stderr, "cam24i_rq: xferlen=%d,", xferlen);
   status = sjy_read(c, cmd, *d, xferlen);
   fprintf(stderr, "status=%d\n", status);
   *d += (status / sizeof(DWORD));
#else
   int i, x, q;
   for (i = 0; i < r; i++) {
      cam24i_q(c, n, a, f, (*d)++, &x, &q);
      if (!q) {
         (*d)--;
         break;
      }
   }
#endif
}

/*------------------------------------------------------------------*/
INLINE void cam16i_sa(const int c, const int n, const int a, const int f,
                      WORD ** d, const int r)
{
   WORD i, aa;
   aa = a;
   for (i = 0; i < r; i++) {
      cam16i(c, n, a, f, *d);
      aa++;
      (*d)++;
   }
}

/*------------------------------------------------------------------*/
INLINE void cam24i_sa(const int c, const int n, const int a, const int f,
                      DWORD ** d, const int r)
{
   WORD i, aa;
   aa = a;
   for (i = 0; i < r; i++) {
      cam24i(c, n, a, f, *d);
      aa++;
      (*d)++;
   }
}

/*------------------------------------------------------------------*/
INLINE void cam16i_sn(const int c, const int n, const int a, const int f,
                      WORD ** d, const int r)
{
   int i;

   for (i = 0; i < r; i++)
      cam16i(c, n + i, a, f, (*d)++);
}

/*------------------------------------------------------------------*/
INLINE void cam24i_sn(const int c, const int n, const int a, const int f,
                      DWORD ** d, const int r)
{
   int i;
   for (i = 0; i < r; i++)
      cam24i(c, n + i, a, f, (*d)++);
}

/*------------------------------------------------------------------*/
INLINE void cam8o(const int c, const int n, const int a, const int f, unsigned char d)
{
   fprintf(stderr, "cam8o not implemented\n");
}

/*------------------------------------------------------------------*/
INLINE void camo(const int c, const int n, const int a, const int f, WORD d)
{
   cam16o(c, n, a, f, d);
}

/*------------------------------------------------------------------*/
INLINE void cam16o(const int c, const int n, const int a, const int f, WORD d)
{
   cam16o_q(c, n, a, f, d, &xdummy, &qdummy);
}

/*------------------------------------------------------------------*/
INLINE void cam24o(const int c, const int n, const int a, const int f, DWORD d)
{
   cam24o_q(c, n, a, f, d, &xdummy, &qdummy);
}

/*------------------------------------------------------------------*/
INLINE void cam16o_q(const int c, const int n, const int a, const int f,
                     WORD d, int *x, int *q)
{
   int cmd;                     /* Take a page from sjy_cfsa.c */
   unsigned short locdat[2] = { 0, 0 };
   cmd = NAFCMD(n, a, f);       /* get cmd to pass to sjy_read */
   /* Since this is a read we don't need to check function .. I hope .. */
   cmd = cmd | (SJY_STOP_ON_WORD << 16);
   locdat[0] = d;
   sjy_write(c, cmd, &locdat[0], sizeof(WORD));
   *q = sjy_get_qx(c);
   *x = ieee_last_X;            /* Need to get this from global variables I guess ... */

}

/*------------------------------------------------------------------*/
INLINE void cam24o_q(const int c, const int n, const int a, const int f,
                     DWORD d, int *x, int *q)
{
   int cmd;                     /* Take a page from sjy_cfsa.c */
   cmd = NAFCMD(n, a, f);       /* get cmd to pass to sjy_read */
   /* Since this is a read we don't need to check function .. I hope .. */
   cmd = cmd | (SJY_STOP_ON_WORD << 16) | SJY_D24_MODE;
   sjy_write(c, cmd, &d, sizeof(DWORD));
   *q = sjy_get_qx(c);
   *x = ieee_last_X;            /* Need to get this from global variables I guess ... */
}

/*------------------------------------------------------------------*/
INLINE void cam8o_r(const int c, const int n, const int a, const int f,
                    BYTE * d, const int r)
{
}

/*------------------------------------------------------------------*/
INLINE void cam16o_r(const int c, const int n, const int a, const int f,
                     WORD * d, const int r)
{
#ifndef NODMA
   int cmd;
   int xferlen, status;
   cmd = NAFCMD(n, a, f);       /* get cmd to pass to sjy_write */
   cmd |= (SJY_Q_REPEAT << 16); /* set q-repeat mode */
   xferlen = r * sizeof(WORD);  /* size, in bytes, of stuff to write */
   status = sjy_write(c, cmd, d, xferlen);
#else
   int i;
   for (i = 0; i < r; i++) {
      cam16o(c, n, a, f, *d);
      d++;
   }
#endif
}


/*------------------------------------------------------------------*/
INLINE void cam24o_r(const int c, const int n, const int a, const int f,
                     DWORD * d, const int r)
{
#ifndef NODMA
   int cmd;
   int xferlen, status;
   cmd = NAFCMD(n, a, f) | SJY_D24_MODE;        /* get cmd to pass to sjy_write */
   cmd |= (SJY_Q_REPEAT << 16); /* set q-repeat mode */
   xferlen = r * sizeof(DWORD); /* size, in bytes, of stuff to write */
   status = sjy_write(c, cmd, d, xferlen);
#else
   int i;
   for (i = 0; i < r; i++) {
      cam24o(c, n, a, f, *d);
      d++;
   }
#endif
}

/*------------------------------------------------------------------*/
INLINE int camc_chk(const int c)
{
   /* GH:  Interpret this as being a "test unit ready" as far as SCSI goes */
   int status, ext_add;
   status = sjy_tur(c);
   if (status)
      return (-1);

   return (0);
}

/*------------------------------------------------------------------*/
INLINE void camc(const int c, const int n, const int a, const int f)
{
   /* Non-data command */
   camc_q(c, n, a, f, &qdummy);
}

/*------------------------------------------------------------------*/
INLINE void camc_q(const int c, const int n, const int a, const int f, int *q)
{
   int cmd;
   cmd = NAFCMD(n, a, f);
   sjy_nondata(c, cmd);
   *q = sjy_get_qx(c);
}

/*------------------------------------------------------------------*/
INLINE void camc_sa(const int c, const int n, const int a, const int f, const int r)
{
   int i;
   for (i = 0; i < r; i++)
      camc(c, n, a + i, f);
}

/*------------------------------------------------------------------*/
INLINE void camc_sn(const int c, const int n, const int a, const int f, const int r)
{
   int i;
   for (i = 0; i < r; i++)
      camc(c, n + i, a, f);
}

/*------------------------------------------------------------------*/

INLINE int cam_init(void)
{
   /* Not the best way to do this ... but ... */
   /* I'll handle this by opening EVERY available one. */
   int i, c, status;
   char filename[72];
   for (c = 0; c <= SJY_BRANCH_MAX; c++) {
      jorway_crate_opened[c] = 0;
      status = sjy_getdev(filename, c);
      if (status == CAM_S_SUCCESS) {    /* Found the device ... */
         fprintf(stderr, "cam_init: found crate %d on device %s, open it\n", c, filename);
         if ((status = cdchn(c, 1, 0)) == CAM_S_SUCCESS);       /* Opened the device */
         {
            ccctype(c, 0, 1);   /* set as parallel */
            jorway_crate_opened[c] = 1;
         }
      }
   }                            /* proceed to next crate ... */
   return SUCCESS;
}

/*------------------------------------------------------------------*/
INLINE void cam_exit(void)
{
   /* Not the best way to do this ... but ... */
   /* I'll handle this by opening EVERY available one. */
   int i, c, status;
   char filename[72];
   for (c = 0; c <= SJY_BRANCH_MAX; c++) {
      if (jorway_crate_opened[c] == 1) {
         fprintf(stderr, "cam_exit: closing crate %d\n", c);
         status = cdchn(c, 0, 0);       /* Close the device */
      }
   }                            /* proceed to next crate ... */
}

/*------------------------------------------------------------------*/
INLINE void cam_inhibit_set(const int c)
{
   camc(c, 30, 9, 26);
}

/*------------------------------------------------------------------*/
INLINE void cam_inhibit_clear(const int c)
{
   camc(c, 30, 9, 24);
}

/*------------------------------------------------------------------*/
INLINE void cam_crate_clear(const int c)
{
   camc(c, 28, 9, 26);
}

/*------------------------------------------------------------------*/
INLINE void cam_crate_zinit(const int c)
{
   camc(c, 28, 8, 26);
}

/*------------------------------------------------------------------*/
INLINE void cam_lam_enable(const int c, const int n)
{
   /* enable LAM mask for slot n in controller */
   jorway_crate_LAM[c] |= (1 << (n - 1));
   cam24o(c, 30, 0, 16, jorway_crate_LAM[c]);
}

/*------------------------------------------------------------------*/
INLINE void cam_lam_disable(const int c, const int n)
{
   /* disable LAM mask for slot n in controller */
   jorway_crate_LAM[c] &= ~(1 << (n - 1));
   cam24o(c, 30, 0, 16, jorway_crate_LAM[c]);
}

/*------------------------------------------------------------------*/
INLINE void cam_lam_read(const int c, DWORD * lam)
{
   cam24i(c, 30, 0, 26, lam);
}

/*------------------------------------------------------------------*/
INLINE void cam_lam_clear(const int c, const int n)
{
}

/*------------------------------------------------------------------*/
INLINE void cam_interrupt_enable(const int c)
{
   fprintf(stderr, "cam_interrupt_enable not implemented\n");
}

/*------------------------------------------------------------------*/
INLINE void cam_interrupt_disable(const int c)
{
   fprintf(stderr, "cam_interrupt_disable not implemented\n");
}

/*------------------------------------------------------------------*/
INLINE int cam_init_rpc(char *host_name, char *exp_name, char *fe_name,
                        char *client_name, char *rpc_server)
{
   return 1;
}

/*------------------------------------------------------------------*/



/* Specific stuff I need to add in order to make things work 
somewhat self-containedly */


int sjy_get_qx(int branch)
{
   unsigned char *buf;
   int *perrno = &sjy_controller[IDX(branch)].errn;

   if (*perrno == 0) {          /* all OK */
      ieee_last_Q = 1;
      ieee_last_X = 1;
   } else if (*perrno == ENODEV) {      /* non data command, no Q */
      ieee_last_Q = 0;
      ieee_last_X = 1;
   } else if (*perrno == EPIPE) {       /* bad word count transfer on pdt */
      ieee_last_Q = 0;
      ieee_last_X = 0;
   } else if (*perrno == EIO) { /* SCSI error */
      ieee_last_Q = 0;
      ieee_last_X = 0;
   } else if (*perrno == EFAULT) {      /* CAMAC error */
      ieee_last_Q = 0;
      ieee_last_X = 0;
   } else {
      buf = sjy_reqSense(sjy_controller[IDX(branch)].fd);
      ieee_last_Q = 1 - (buf[8] & 128) / 128;
      ieee_last_X = 1 - (buf[8] & 64) / 64;
   }

   return (ieee_last_Q);
}

/* ------ Included from sjy_interface ------- */

/* #define DEBUG */
/* #define GDEBUG */

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>
/*#include <scsi/sg.h>*/
#include "/usr/src/linux/include/scsi/sg.h"
#define SCSI_OFF sizeof(struct sg_header)
#define USER_BUF_SIZE (SG_BIG_BUFF - (SCSI_OFF + 18 + 511))
#define CMD_SIZE (SCSI_OFF + 18 + USER_BUF_SIZE)
static unsigned char cmd[CMD_SIZE];     /* SCSI command buffer */


/*
** handle_scsi_cmd - process a scsi command using the generic scsi interface
**
** returns
**  success  int status - number of bytes read minus the sizeof(sg_header)
**  fail     CAM_S_INVLD_ARG -  bad function argument
**           CAM_S_GETSEM_FAIL - failed to get semaphore
**           CAM_S_DEV_IO_ERROR - scsi write or read returned bad status 
**           CAM_S_PUTSEM_FAIL - failed to release semaphore
*/

/* static */
int handle_scsi_cmd(int branch, /* CAMAC branch */
                    unsigned cmd_len,   /* command length */
                    unsigned in_size,   /* input data size */
                    unsigned char *i_buff,      /* input buffer */
                    int out_size,       /* output data size */
                    unsigned char *o_buff       /* output buffer */
    )
{
   int status = 0;
   struct sg_header *sg_hd;
   sigset_t newmask, oldmask;
   int i;
   int idx = IDX(branch);
   int fd = sjy_controller[idx].fd;
   int defecit = 0;
#ifdef GDEBUG
   fflush(stderr);
   fflush(stdin);
   fprintf(stderr,
           "handle_scsi_cmd: branch=%d, idx=%d, fd=%d, cmd_len=%d, in_size=%d, ibuff=0x%x, out_size=%d, o_buff=0x%x\n",
           branch, idx, fd, cmd_len, in_size, i_buff, out_size, o_buff);
   fprintf(stderr, "            cmd: 0x");
   for (i = 0; i < cmd_len; i++) {
      fprintf(stderr, " %02x", i_buff[SCSI_OFF + i]);
   }
   fprintf(stderr, "\n");
   fflush(stderr);
   fflush(stdin);
#endif
   /* safety checks */
   if (!cmd_len)
      return (CAM_S_INVLD_ARG); /* need a cmd_len != 0 */
   if (!i_buff)
      return (CAM_S_INVLD_ARG); /* need an input buffer != NULL */
#ifdef SG_BIG_BUFF
   if (SCSI_OFF + cmd_len + in_size > SG_BIG_BUFF)
      return -1;
   if (SCSI_OFF + out_size > SG_BIG_BUFF)
      return -1;
#else
   if (SCSI_OFF + cmd_len + in_size > 4096)
      return -1;
   if (SCSI_OFF + out_size > 4096)
      return -1;
#endif

   if (!o_buff)
      out_size = 0;

   /* generic scsi device header construction */
   sg_hd = (struct sg_header *) i_buff;
   sg_hd->reply_len = SCSI_OFF + out_size;
   sg_hd->twelve_byte = cmd_len == 12;
   sg_hd->result = 0;
#if     0
   sg_hd->pack_len = SCSI_OFF + cmd_len + in_size;      /* not necessary */
   sg_hd->pack_id;              /* not used */
   sg_hd->other_flags;          /* not used */
#endif

   /* protect the write/read from being interrupted by ^C */
   sigemptyset(&newmask);
   sigaddset(&newmask, SIGINT);
   /*  block SIGINT and save current signal mask */
   if (sigprocmask(SIG_BLOCK, &newmask, &oldmask) < 0)
      fprintf(stderr, "sjy_interface: SIG_BLOCK error");
   /* protect the write/read section for multi-user */
   if (sjy_getsem() != 0) {
      fprintf(stderr, "sjy_interface: getsem failed");
      sjy_putsem();
      return (CAM_S_GETSEM_FAIL);
   }

   /* send command */
   status = write(fd, i_buff, SCSI_OFF + cmd_len + in_size);
#ifndef DEBUG
   if (status < 0) {
      sjy_putsem();
      sigprocmask(SIG_SETMASK, &oldmask, NULL);
      return (CAM_S_DEV_IO_ERROR);
   }
#endif

#ifdef DEBUG
   if (status < 0 || status != SCSI_OFF + cmd_len + in_size || sg_hd->result) {
      /* some error happened */
      fprintf(stderr, "write(generic) result = 0x%x cmd = 0x%x\n",
              sg_hd->result, i_buff[SCSI_OFF]);
      if (status < 0)
         perror("sjy_interface");
      sjy_putsem();
      return (CAM_S_DEV_IO_ERROR);
   }
#endif


   /* Since the sg.c driver _read_ always returns the sg_header, we must
    ** provide a buffer +SCSI_OFF in size. Since i_buff was already
    ** allocated as the largest size data buffer we could use (+SCSI_OFF),
    ** we use it. We have to copy the data read back to the user's 
    ** buffer, but this ultimatly saves an extra malloc call.
    */

   /* retrieve result */
   status = read(fd, i_buff, SCSI_OFF + out_size);

   /* return the semaphore */
   if (sjy_putsem() != 0) {
      fprintf(stderr, "sjy_interface: getsem failed");
      return (CAM_S_PUTSEM_FAIL);
   }

   /* reset signal mask which unblocks SIGINT */
   if (sigprocmask(SIG_SETMASK, &oldmask, NULL) < 0)
      fprintf(stderr, "sjy_interface: SIG_SETMASK error");


   /* copy data read back to the user buffer */
   /* The generic scsi driver keeps an internal buffer with the
    ** last data read, and does not clear it when a subsequent SCSI
    ** command completes with a non-zero SCSI response. sg_hd->result
    ** does not contain the SCSI response as observed from a SCSI
    ** analyzer, so it is not used. The only alternative is to test
    ** for a non-zero sense buffer sense key field returned from the
    ** Jorway. If the sense key is non-zero, the user buffer is
    ** zeroed instead of returning stale data from the internal SCSI
    ** buffer.
    */

   /* by rex hubbard (jorway): before doing the above, check if the
    ** sensekey is 9, indicating a transfer stopped by q-response. If
    ** so, get the defecit from the sense data and adjust the return
    ** value. The data in the internal buffer is valid, so copy it.
    */

   if (!(sg_hd->sense_buffer[SENSEKEY])) {
      if (o_buff) {
         memcpy(o_buff, i_buff + SCSI_OFF, out_size);
      }
   } else {
      if (sg_hd->sense_buffer[SENSEKEY] == 9) {
         defecit = sg_hd->sense_buffer[4] << 8 | sg_hd->sense_buffer[5];
         defecit = defecit << 8 | sg_hd->sense_buffer[6];
         if (o_buff) {
            memcpy(o_buff, i_buff + SCSI_OFF, out_size);
         }
      } else
         bzero(o_buff, out_size);
   }

   /* save scsi status and sense buffer */
   sjy_controller[idx].result = sg_hd->result;
   memcpy(sjy_controller[idx].sense_buffer, sg_hd->sense_buffer, SJY_SENSE_LENGTH);
   sjy_controller[idx].target_status = (sg_hd->target_status) * 2;
#ifdef GDEBUG
   fflush(stderr);
   fflush(stdin);
   fprintf(stderr, "handle_scsi_command:  target_status=0x%x\n",
           sjy_controller[idx].target_status);
#if 0
   if ((sjy_controller[idx].target_status != 0) &&
       (sjy_controller[idx].target_status != 0))
#endif
   {
      fprintf(stderr, "read(generic) sense "
              "%x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x\n",
              sg_hd->sense_buffer[0], sg_hd->sense_buffer[1],
              sg_hd->sense_buffer[2], sg_hd->sense_buffer[3],
              sg_hd->sense_buffer[4], sg_hd->sense_buffer[5],
              sg_hd->sense_buffer[6], sg_hd->sense_buffer[7],
              sg_hd->sense_buffer[8], sg_hd->sense_buffer[9],
              sg_hd->sense_buffer[10], sg_hd->sense_buffer[11],
              sg_hd->sense_buffer[12], sg_hd->sense_buffer[13],
              sg_hd->sense_buffer[14], sg_hd->sense_buffer[15]);
   }
   fflush(stderr);
   fflush(stdin);
#endif



   /* Unlike the Jorway 73A, the 411S returns SCSI status CONDITION_MET
    ** (0x04) for Q=1 on a data transfer. This is converted by the 
    ** generic SCSI driver sg.c to EIO (0x05). The CAMAC completion routines
    ** ignore this status, but the following code segment does not.
    */
#ifdef DEBUG
   if (status < 0 || status != SCSI_OFF + out_size || sg_hd->result) {
      /* some error happened */
      fprintf(stderr, "read(generic) result = 0x%x cmd = 0x%x\n",
              sg_hd->result, i_buff[SCSI_OFF]);
      fprintf(stderr, "read(generic) sense "
              "%x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x\n",
              sg_hd->sense_buffer[0], sg_hd->sense_buffer[1],
              sg_hd->sense_buffer[2], sg_hd->sense_buffer[3],
              sg_hd->sense_buffer[4], sg_hd->sense_buffer[5],
              sg_hd->sense_buffer[6], sg_hd->sense_buffer[7],
              sg_hd->sense_buffer[8], sg_hd->sense_buffer[9],
              sg_hd->sense_buffer[10], sg_hd->sense_buffer[11],
              sg_hd->sense_buffer[12], sg_hd->sense_buffer[13],
              sg_hd->sense_buffer[14], sg_hd->sense_buffer[15]);
      if (status < 0)
         perror("sjy_interface");
      return (CAM_S_DEV_IO_ERROR);
   }
#endif
   return (status - SCSI_OFF - defecit);
}

/*
** sjy_nondata - called by CAMAC IEEE routines. Sets up scsi command block non-data commands.
**
** returns
**  int status - return status from sjy_interface
**
*/

int sjy_nondata(int branch, int camCmd)
{
   static unsigned char scmdblk[6];
   static unsigned char lcmdblk[12];    /* required for commands 0xc0-0xff */
   static unsigned char *cmdblk;
   int cmdsize;
   int status;
   bzero(cmd, CMD_SIZE);
   /* cmdblk = scmdblk; */
   cmdblk = cmd + SCSI_OFF;
   cmdsize = 6;
   cmdblk[0] = SJY_NONDATA_CMD; /* Op code */
   cmdblk[1] = (camCmd >> 24) & 0x1F;   /* lun/function code */
   cmdblk[2] = (camCmd >> 16) & 0xFF;   /* mode/station */
   cmdblk[3] = (camCmd >> 8) & 0x0F;    /* address */
   cmdblk[4] = 0;               /* allocation length */
   cmdblk[5] = 0;               /* control byte, always 0 */
   status = handle_scsi_cmd(branch, cmdsize, 0, cmd, 0, NULL);
   /*     sjy_notify_camac_nondata(branch, 0, status); */
   notify_camac_nondata(branch, 0, status);
   return (status);
}

/*
** sjy_write - called by CAMAC IEEE routines. Sets up scsi command block for writes
**             and non-data commands.
**
** returns
**  int status - return status from sjy_interface
**
*/

int sjy_write(int branch, int camCmd, void *dataBuffer, int xferlen)
{
   static unsigned char scmdblk[6];
   static unsigned char lcmdblk[10];
   static unsigned char *cmdblk;
   int cmdsize;
   int status;

   bzero(cmd, CMD_SIZE);
   if (xferlen < 256) {         /* This is also the nondata command block */
      /* cmdblk = scmdblk; */
      cmdblk = cmd + SCSI_OFF;
      cmdblk[0] = SJY_SHORTXFER_DATA_CMD;       /* Op code */
      cmdblk[1] = (camCmd >> 24) & 0x1F;        /* lun/function code */
      cmdblk[2] = (camCmd >> 16) & 0xFF;        /* mode/station */
      cmdblk[3] = (camCmd >> 8) & 0x0F; /* address */
      cmdblk[4] = xferlen;      /* allocation length */
      cmdblk[5] = 0;            /* control byte, always 0 */
   } else {
      /* cmdblk = lcmdblk; */
      cmdblk = cmd + SCSI_OFF;
      cmdblk[0] = SJY_LONGXFER_DATA_CMD;        /* Op code */
      cmdblk[1] = 0;            /* lun/reserved */
      cmdblk[2] = (camCmd >> 24) & 0x1F;        /* reserved/function code */
      cmdblk[3] = (camCmd >> 16) & 0xFF;        /* mode/S/station */
      cmdblk[4] = (camCmd >> 8) & 0x0F; /* address */
      cmdblk[5] = 0;            /* reserved */
      cmdblk[6] = (xferlen >> 16) & 0xFF;       /* transfer length (MSB) */
      cmdblk[7] = (xferlen >> 8) & 0xFF;
      cmdblk[8] = xferlen & 0xFF;       /* transfer length (LSB) */
      cmdblk[9] = 0;            /* control byte, always 0 */
   }
   if (cmdblk[0] == SJY_SHORTXFER_DATA_CMD)
      cmdsize = 6;
   else
      cmdsize = 10;

   /* memcpy( cmd + SCSI_OFF, cmdblk, cmdsize ); */

   /* copy user buffer behind cmdblk */
   if (dataBuffer != NULL)
      memcpy(cmd + SCSI_OFF + cmdsize, dataBuffer, xferlen);

   status = handle_scsi_cmd(branch, cmdsize, xferlen, cmd, 0, NULL);
   /*  sjy_notify_camac_data(branch, xferlen, status); */
   notify_camac_data(branch, xferlen, status);
   return (status);
}


/*
** sjy_read - called by CAMAC IEEE routines. Sets up scsi command block for reads.
**
** returns
**  int status - return status from sjy_interface
**
*/
int sjy_read(int branch, int camCmd, void *dataBuffer, int xferlen)
{
   static unsigned char scmdblk[6];
   static unsigned char lcmdblk[10];
   static unsigned char *cmdblk;
   int cmdsize;
   int status;
   bzero(cmd, CMD_SIZE);        /* clear cmd for case of short transfers */
   if (xferlen < 256) {
      /* cmdblk = scmdblk; */
      cmdblk = cmd + SCSI_OFF;
      cmdblk[0] = SJY_SHORTXFER_DATA_CMD;       /* Op code */
      cmdblk[1] = (camCmd >> 24) & 0x1F;        /* lun/function code */
      cmdblk[2] = (camCmd >> 16) & 0xFF;        /* mode/station */
      cmdblk[3] = (camCmd >> 8) & 0x0F; /* address */
      cmdblk[4] = xferlen;      /* allocation length */
      cmdblk[5] = 0;            /* control byte, always 0 */
   } else {
      /* cmdblk = lcmdblk; */
      cmdblk = cmd + SCSI_OFF;
      cmdblk[0] = SJY_LONGXFER_DATA_CMD;        /* Op code */
      cmdblk[1] = 0;            /* lun/reserved */
      cmdblk[2] = (camCmd >> 24) & 0x1F;        /* reserved/function code */
      cmdblk[3] = (camCmd >> 16) & 0xFF;        /* mode/S/station */
      cmdblk[4] = (camCmd >> 8) & 0x0F; /* address */
      cmdblk[5] = 0;            /* reserved */
      cmdblk[6] = (xferlen >> 16) & 0xFF;       /* transfer length (MSB) */
      cmdblk[7] = (xferlen >> 8) & 0xFF;
      cmdblk[8] = xferlen & 0xFF;       /* transfer length (LSB) */
      cmdblk[9] = 0;            /* control byte, always 0 */
   }
   if (cmdblk[0] == SJY_SHORTXFER_DATA_CMD)
      cmdsize = 6;
   else
      cmdsize = 10;
   /* memcpy( cmd + SCSI_OFF, cmdblk, cmdsize ); */
   status = handle_scsi_cmd(branch, cmdsize, 0, cmd, xferlen, dataBuffer);
   notify_camac_data(branch, xferlen, status);
   return (status);
}


/*
** sjy_inquiry - request information about the JORWAY
**
** returns
**  success - pointer to a static char buffer containing inquiry data
**  fail - pointer to an empty static char buffer
**
*/

/* request vendor brand and model */
unsigned char *sjy_inquiry(int branch)
{
   static unsigned char Inqbuffer[SCSI_OFF + INQUIRY_REPLY_LEN];
   int i;
   char *cptr;
   unsigned char cmdblk[INQUIRY_CMDLEN] = { INQUIRY_CMD,        /* command */
      0,                        /* lun/reserved */
      0,                        /* page code */
      0,                        /* reserved */
      INQUIRY_REPLY_LEN,        /* allocation length */
      0
   };                           /* reserved/flag/link */

   memcpy(cmd + SCSI_OFF, cmdblk, sizeof(cmdblk));
   /* clear the Inquiry buffer */
   /*memset( Inqbuffer, '\0', sizeof(Inqbuffer) ); */

   handle_scsi_cmd(branch, sizeof(cmdblk), 0, cmd,
                   sizeof(Inqbuffer) - SCSI_OFF, Inqbuffer);

   /* handle_scsi_cmd removes sg_header in the copy from i_buff to o_buff,
    ** so don't add SCSI_OFF offset to Inqbuffer
    */
   /*
      fprintf(stderr,"Inquiry buffer\n");
      fprintf(stderr,"%s\n",Inqbuffer+INQUIRY_VENDOR);
    */
   return (Inqbuffer);
}


/*
** sjy_tur - sends Test Unit Ready command
**
** returns
**  int status - return status from sjy_interface 
*/

int sjy_tur(int branch)
{
   int i;
   int status;
   /* request READY status */
   static unsigned char cmdblk[TESTUNITREADY_CMDLEN] = {
      TESTUNITREADY_CMD,        /* command */
      0,                        /* lun/reserved */
      0,                        /* reserved */
      0,                        /* reserved */
      0,                        /* reserved */
      0
   };                           /* reserved */

   memcpy(cmd + SCSI_OFF, cmdblk, sizeof(cmdblk));
   /* This must be done twice to get good status from power cycled Jorway */
   for (i = 0; i < 2; i++) {
      status = handle_scsi_cmd(branch, sizeof(cmdblk), 0, cmd, 0, NULL);
   }
   if (status)
      perror("sjy_tur");

   return (status);
}



/*
** sjy_reqSense - REQUEST_SENSE from the JORWAY. Normally issued after a CHECK
**                CONDITION status is returned by any command.
**
** returns
**  success - pointer to a static char buffer containing sense data
**  fail - pointer to an empty static char buffer
**
*/

unsigned char *sjy_reqSense(int branch)
{
   int idx = IDX(branch);
   static unsigned char sensebuffer[SCSI_OFF + SJY_SENSE_LENGTH];

   unsigned char cmdblk[SENSE_CMDLEN] = { SENSE_CMD,    /* command */
      0,                        /* lun/reserved */
      0,                        /* reserved */
      0,                        /* reserved */
      SJY_SENSE_LENGTH,         /* allocation length */
      0
   };                           /* reserved/flag/link */

   memcpy(cmd + SCSI_OFF, cmdblk, sizeof(cmdblk));

   memset(sensebuffer, '\0', sizeof(sensebuffer));

   handle_scsi_cmd(sjy_controller[idx].fd, sizeof(cmdblk), 0, cmd,
                   sizeof(sensebuffer) - SCSI_OFF, sensebuffer);

   /* handle_scsi_cmd removes sg_header in the copy from i_buff to o_buff,
    ** so don't add SCSI_OFF offset to sensebuffer
    */
   return (sensebuffer);
}


/*
** sjy_notify_camac_data - SCSI command completion notification routine
**                         for CAMAC data transfers
**
** returns
**  int result - SCSI command completion result
*/

int sjy_notify_camac_data(int branch,   /* CAMAC branch */
                          int xferlen,  /* expected transfer length */
                          int status)
{                               /* actual transfer length */
   int idx = IDX(branch);
   int sense_key = sjy_controller[idx].sense_buffer[SENSEKEY];
   int add_sense = sjy_controller[idx].sense_buffer[ADD_SENSECODE];


   sjy_controller[idx].errn = 0;

   if (sense_key) {
      if (add_sense == SJY_NO_Q) {
         /* SCSI ok, CAMAC ok, no Q on transfer */
         sjy_controller[idx].errn = ENODEV;
      } else if (add_sense == SJY_NO_X) {
         /* SCSI ok, no X on transfer */
         sjy_controller[idx].errn = EFAULT;
      } else {
         /* SCSI problem */
         sjy_controller[idx].errn = EIO;
      }
   } else {
      if (status == xferlen) {
         /* SCSI ok, CAMAC ok, assuming Q on transfer */
         sjy_controller[idx].errn = 0;
      }
   }

   return sjy_controller[idx].result;
}


/*
** sjy_notify_camac_nondata - SCSI command completion notification routine
**                            for CAMAC nondata transfers
**
** returns
**  int result - SCSI command completion result
*/

int sjy_notify_camac_nondata(int branch,        /* CAMAC branch */
                             int xferlen,       /* expected transfer length */
                             int status)
{                               /* actual transfer length */
   int idx = IDX(branch);
   int sense_key = sjy_controller[idx].sense_buffer[SENSEKEY];
   int add_sense = sjy_controller[idx].sense_buffer[ADD_SENSECODE];

   sjy_controller[idx].errn = 0;

   if ((sjy_controller[idx].result == SJY_GOOD) && !(sense_key)) {
      /* SCSI ok. CAMAC ok, no Q on transfer */
      sjy_controller[idx].errn = ENODEV;
   } else if ((sjy_controller[idx].result == SJY_CHECK_CONDITION)
              || (sense_key)) {
      if (add_sense == SJY_NO_Q) {
         /* SCSI ok, CAMAC ok, no Q on transfer */
         sjy_controller[idx].errn = ENODEV;
      } else if (add_sense == SJY_NO_X) {       /* SCSI ok, no X on transfer */
         sjy_controller[idx].errn = EFAULT;
      } else {                  /* SCSI problem */
         sjy_controller[idx].errn = EIO;
      }
   } else {                     /* SCSI ok, CAMAC ok, assuming Q on transfer */
      sjy_controller[idx].errn = 0;
   }
   return sjy_controller[idx].result;
}


/* ------ End of Included from sjy_interface ------------------- --- */

/* sjy_cdchn.c */

#include <sys/types.h>
#include <sys/stat.h>

int sjy_getdev(char *dev, int branch);


unsigned int cdchn(int branch, int channel, int route)
{
   /* -----------------------------------------------------------------------   */
   char filename[72];
   char version[6];
   int fd;
   int status;
   struct stat buf;
   char dev[32];
   int idx = IDX(branch);


   if (ID(branch) < 0 || ID(branch) > SJY_BRANCH_MAX)
      return (CAM_S_INVLD_BRANCH);

   /* close the SCSI device file */
   if (channel == 0) {
      if (fstat(sjy_controller[idx].fd, &buf) != -1) {
         close(sjy_controller[idx].fd);
         return (CAM_S_SUCCESS);
      }
      return (CAM_S_SCSI_CLOSE_FAIL);
   }

   /* find the scsi device name based on our SCSI id and SCSI bus */
   status = sjy_getdev(filename, branch);
   if (status != CAM_S_SUCCESS) {
      fprintf(stderr, "CDCHN Error finding generic scsi device\n");
      return (status);
   }

   /* create the semaphore */
   if (sjy_inisem() == -1) {
      perror("cdchn: inisem");
      return (CAM_S_SEM_FAIL);
   }

   /* reserve the semaphore */
   if (sjy_getsem() != 0) {
      perror("cdchn: getsem");
      sjy_putsem();
      return (CAM_S_GETSEM_FAIL);
   }

   /* open the scsi device file */
   if ((fd = open(filename, O_RDWR)) == -1) {
      fprintf(stderr, "CDCHN Error opening %s\n", filename);
      perror("CDCHN");
      sjy_putsem();
      return (CAM_S_DEV_OPEN_FAIL);
   }

   /* release the semaphore */
   if (sjy_putsem() != 0) {
      perror("cdchn: putsem");
      return (CAM_S_PUTSEM_FAIL);
   }
#ifdef DEBUG
   fprintf(stderr, "cdchn: filename = %s  fd = %d\n", filename, fd);
#endif

   sjy_controller[idx].scsi_id = ID(branch);
   sjy_controller[idx].scsi_bus = BUS(branch);
   sjy_controller[idx].serial = 1;      /* assume serial */
   sjy_controller[idx].fd = fd;

   /* Send two Test Unit Ready Commands to Jorway */
   status = sjy_tur(branch);
   if (status) {                /* 0=success */
      fprintf(stderr, "CDCHN Error executing Test Unit Ready\n");
      perror("cdchn");
      return (CAM_S_DEV_OPEN_FAIL);
   }

   return (CAM_S_SUCCESS);
}

static char *devtble[] = { "/dev/sga",  /* scsi generic devices */
   "/dev/sgb",
   "/dev/sgc",
   "/dev/sgd",
   "/dev/sge",
   "/dev/sgf",
   "/dev/sgh",
   "/dev/sgg",
   "/dev/sgh"
};

int sjy_getdev(char *dev, int branch)
{
   int status = 0;
   char charBuff[120];          /* file line buffer */

   FILE *g_rfp;                 /* /proc/scsi/scsi file descriptor */
   char filename[32] = { "/proc/scsi/scsi" };   /* file describing SCSI devices attached to system */
   int bus;                     /* SCSI bus */
   int id;                      /* SCSI id */
   char vendor[10];             /* SCSI device vendor */
   char type[10];               /* SCSI device type */
   int found = 0;               /* Jorway found flag */
   int i = 0;                   /* devtble index */
   int devtblesz = 9;           /* size of devtble */


   /* open /proc/scsi/scsi */
   g_rfp = fopen(filename, "r");
   if (g_rfp == NULL) {
      fprintf(stderr, "ScanSCSIDevices:  Cannot open %s", filename);
      return (CAM_S_DEV_OPEN_FAIL);
   }

   /* read each line of filename */
   while ((status == 0) && (i < devtblesz)) {

      status = readFile(g_rfp, filename, charBuff, sizeof(charBuff));
      if (status == EOF) {      /* end of file */

         if (!found)
            status = CAM_S_DEV_OPEN_FAIL;
         break;

      } else {                  /* parse the file line */

         /* parse the Host line */
         if (strncmp(charBuff, "Host", 4) == 0) {
            sscanf(&charBuff[10], "%d", &bus);
            sscanf(&charBuff[29], "%d", &id);
         }

         /* parse the Vendor line */
         if (strncmp(&charBuff[2], "Vendor", 6) == 0) {
            sscanf(&charBuff[10], "%s", vendor);

            if (strcmp(vendor, "JORWAY") == 0) {
               if ((id == ID(branch)) && (bus == BUS(branch))) {
                  /* found our device by matching scsi id */
                  strcpy(dev, devtble[i]);
                  found = 1;
                  break;
               }
            } else {            /* SCSI device found, but not a Jorway */
               i++;
            }
         }

         /* parse the Type line */
         if (strncmp(&charBuff[2], "Type", 4) == 0) {
            sscanf(&charBuff[10], "%s", type);
#ifdef DEBUG
            if (strcmp(type, "Unknown") || (strcmp(type, "Processor"))) {
               fprintf(stderr, "Found type %s", type);
            }
#endif
         }
      }                         /* if status == 0 */
   }                            /* while */

   fclose(g_rfp);
   if (found)
      status = CAM_S_SUCCESS;
   return (status);
}


/* 
** Read file "filename" on stream g_rfp and return
** results in buffer charBuff.
*/

int readFile(FILE * g_rfp, char *filename, char charBuff[], int charBuffsize)
{
   int status;
   if (fgets(charBuff, charBuffsize, g_rfp) != NULL) {
      strtok(charBuff, "\n");
      status = 0;
   } else {                     /* error or end of file found and no characters read */
      status = EOF;
   }

   return (status);
}

/*----------------------- cctype ----------------------------*/
int ccctype(int branch, int lserial, int lparall)
{
   if (ID(branch) < 0 || ID(branch) > SJY_BRANCH_MAX)
      return (CAM_S_INVLD_BRANCH);
   if (lserial & lparall)
      return (CAM_S_SER_OR_PAR_ONLY);
   if (lserial)
      sjy_controller[IDX(branch)].serial = 1;
   if (lparall)
      sjy_controller[IDX(branch)].serial = 0;
   return (CAM_S_SUCCESS);
}


/*----------------------- SEMAPHORE stuff ------------------ */

/* #define USESEMAPHORES */

#ifdef USESEMAPHORES
/* sjy_semops.c				jms 6sep94 */
/* 21aug97 das Modified vsd_semops for Linux CAMAC */

/* operations on the semaphores - init, get and put. 
* sjy_inisem()		called in inicam
* sjy_getsem,putsem	called to reserve the semaphore
*
*/

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <fcntl.h>
#include <errno.h>

#ifdef VXWORKS
#define SJY_SEM_TYPE SEM_ID
#else
#define SJY_SEM_TYPE int
#endif

#define SJY_FILE "/tmp/sjykeyfile"
SJY_SEM_TYPE sjy_sem = (SJY_SEM_TYPE) - 1;
/* Note for flags:
*	IPC_WAIT is default (dont sepecify IPC_NOWAIT)
*	SEM_UNDO aids clean up on exit(2), by changing semadj
*/
int sjy_inisem()
{
#ifndef VXWORKS
   key_t sjy_key;
   union semun arg;
#endif
   int fd;
   /* only initialize it once */
   if (sjy_sem > (SJY_SEM_TYPE) - 1)
      return ((int) sjy_sem);
#ifdef VXWORKS
   sjy_sem = semBCreate(SEM_Q_PRIORITY, SEM_FULL);
   return ((int) sjy_sem);
#else
   /* create the key file */
   if (fd = open(SJY_FILE, O_CREAT | O_EXCL, 664) < 0) {
      if (errno != EEXIST) {
         fprintf(stderr, "sjy_semops: Severe Error, could not create key file %s, 
        error=%d\n", SJY_FILE, errno);
         return (-1);
      }
   }

   /* reserve the Semaphore */
   if ((sjy_key = ftok(SJY_FILE, 1)) == -1) {
      printf
          ("sjy_semops: Severe Error, could not get key for %s error=%d\n",
           SJY_FILE, errno);
      return (-1);
   }

   /* check to see if its already in system */
   sjy_sem = semget(sjy_key, 1, IPC_EXCL | IPC_CREAT | 0777);
   if (errno != EEXIST) {
      /* it didnt already exist, open it back up as non EXCL and set it to 1 */
      if (semctl(sjy_sem, 0, IPC_RMID, arg) != 0) {
         printf("sjy_semops: Severe Error, couldnt close semaphore\n");
         return (-1);
      }

      if ((sjy_sem = semget(sjy_key, 1, IPC_CREAT | 0777)) == -1) {
         printf("sjy_semops: Severe Error, couldnt reopen semaphore\n");
         return (-1);
      }

      /* set the semval to 1 to let only 1 process use it at a time */
      arg.val = 1;
      if (semctl(sjy_sem, 0, SETVAL, arg) != 0) {
         printf("sjy_semops: Severe Error, couldnt set semaphore\n");
         return (-1);
      }
   }

   else {
      /* it already existed, so just open it up */
      sjy_sem = semget(sjy_key, 1, IPC_CREAT | 0777);
   }

   return (sjy_sem);
#endif
}

int sjy_getsem()
{
   /* get the sjy semaphore */

#ifdef VXWORKS
   return (semTake(sjy_sem, WAIT_FOREVER));
#else
   struct sembuf sops[1];
   /* load the sops structure */
   sops[0].sem_num = 0;
   sops[0].sem_op = -1;
   sops[0].sem_flg = SEM_UNDO;  /* WAIT for it */
   return (semop(sjy_sem, sops, 1));
#endif
}

int sjy_putsem()
{
   /* release the sjy semaphore */

#ifdef VXWORKS
   return (semGive(sjy_sem));
#else
   struct sembuf sops[1];
   /* load the sops structure */
   sops[0].sem_num = 0;
   sops[0].sem_op = 1;
   sops[0].sem_flg = SEM_UNDO;
   return (semop(sjy_sem, sops, 1));
#endif
}
#else                           /* else for USESEMPOS */
int sjy_inisem()
{
   return (1);
}

int sjy_getsem()
{
   return (0);
}                               /* get the sjy semaphore */

int sjy_putsem()
{
   return (0);
}                               /* release the sjy semaphore */
#endif


/* ---------------------------------------------------------------- */
/* Replacement subroutines for deriving x, q response               */
/* ---------------------------------------------------------------- */


int notify_camac_nondata(int branch, int xferlen, int status)
/* Greg's version of determining final status */
/* Essentially copied from before */
{
   int idx = IDX(branch);
   int addsense = sjy_controller[idx].sense_buffer[ADD_SENSECODE];
   int target_status = sjy_controller[idx].target_status;
   switch (status) {
   case 0:
      sjy_controller[idx].errn = ENODEV;
      break;                    /* X=1,Q=0; see get_qx */
   case 4:
      sjy_controller[idx].errn = 0;
      break;                    /* X=1,Q=1; see get_qx */
   case 2:
      switch (addsense) {
      case SJY_NO_Q:
         sjy_controller[idx].errn = ENODEV;
         break;                 /* X=1,Q=0 */
      case SJY_NO_X:
         sjy_controller[idx].errn = EFAULT;
         break;                 /* X=0 but SCSI ok */
      default:
         sjy_controller[idx].errn = EIO;
         break;                 /* SCSI failed, too. */
      } /* matches switch addsense */ ;
      break;                    /* case 2 */
   default:                    /* Other status means soemthing's screwed up */
      fprintf(stderr, "Invalid SCSI Status byte: 0x%02x\n", target_status);
   }
}

/* Greg's slightly modified notify_camac_data */
/* Takes advantage of the fact that I do have access to the */
/* target status byte. */
/* So I don't have to derive "ok" from lack of other errrors. */

int notify_camac_data(int branch,       /* CAMAC branch */
                      int xferlen,      /* expected transfer length */
                      int status)
{                               /* actual transfer length */
   int idx = IDX(branch);
   int sense_key = sjy_controller[idx].sense_buffer[SENSEKEY];
   int add_sense = sjy_controller[idx].sense_buffer[ADD_SENSECODE];
   int target_status = sjy_controller[idx].target_status;
   if (target_status == 0) {    /* Good status -- no errors */
#if GDEBUG
      fprintf(stderr, "target_status=0x%02x\n", target_status);
#endif
      sjy_controller[idx].errn = 0;
      return;
   } else if (target_status != 2) {     /* Improper status */
      sjy_controller[idx].errn = EIO;
      fprintf(stderr, "Invalid SCSI status byte: 0x%02x\n", target_status);
   } else {                     /* Status = 2, check condition */
#if GDEBUG
      fprintf(stderr,
              "target_status=0x%02x,sense_key=0x%02x,add_sense=0x%02x\n",
              target_status, sense_key, add_sense);
#endif
      if (sense_key == 0) {     /* Should only have to "check condition" if !=0 */
      }
      if (add_sense == SJY_NO_Q) {      /* SCSI ok, CAMAC ok, no Q on transfer */
         sjy_controller[idx].errn = ENODEV;
      } else if (add_sense == SJY_NO_X) {       /* SCSI ok, no X on transfer */
         sjy_controller[idx].errn = EFAULT;
      } else {                  /* SCSI problem */
         sjy_controller[idx].errn = EIO;
      }
   }

}
