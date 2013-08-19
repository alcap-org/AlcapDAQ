/********************************************************************\

  Name:         khyt1331.c
  Created by:   Stefan Ritt

  Contents:     Kernel mode driver for Hytec 1331 CAMAC interface

  $Log: khyt1331.c,v $
  Revision 1.1.1.1  2005/06/20 23:37:10  mucap
  Importing release 1.9.5 of the MIDAS source code as distributed by PSI.
  (Next, I'll commit our local customizations.)

  Revision 1.6  2004/01/08 08:40:08  midas
  Implemented standard indentation

  Revision 1.5  2003/06/23 08:00:57  midas
  Replaced malloc.h by slab.h

  Revision 1.4  2001/10/03 14:57:22  midas
  Check IO address region before allocation

  Revision 1.3  2001/10/03 14:51:30  midas
  Return -ENODEV if interface not found

  Revision 1.2  2001/10/03 14:45:11  midas
  Separated IOCTL functions code into header file

  Revision 1.1  2001/08/21 14:28:34  midas
  Initial revision

\********************************************************************/

#define __KERNEL__
#define MODULE

#include <linux/config.h>

#if defined(CONFIG_MODVERSIONS) && !defined(MODVERSIONS)
#  define MODVERSIONS
#endif

#ifdef MODVERSIONS
#  include <linux/modversions.h>
#endif

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/sched.h>
#include <linux/pci.h>
#include <linux/slab.h>
#include <linux/delay.h>
#include <linux/proc_fs.h>
#include <linux/poll.h>

#include <asm/uaccess.h>
#include <asm/io.h>

MODULE_AUTHOR("Stefan Ritt <stefan.ritt@psi.ch>");
MODULE_DESCRIPTION("Hytec 1331 CAMAC driver");
EXPORT_NO_SYMBOLS;

/*---- definitions -------------------------------------------------*/

/* LOCAL/EXPERIMENTAL use, see devices.txt */
#define HYT1331_MAJOR         60

#include "khyt1331.h"

#define OUTP(_p, _d) outb(_d, _p)
#define OUTPW(_p, _d) outw(_d, _p)
#define OUTP_P(_p, _d) outb_p(_d, _p)
#define INP(_p) inb(_p)
#define INPW(_p) inw(_p)

/*---- globals -----------------------------------------------------*/

int io_base = 0, irq = 0;

spinlock_t khyt1331_lock;       /* spin lock for busy check */
int khyt1331_in_use = 0;        /* flag if device in use */
int in_use_pid;                 /* PID of process which uses device */
int interrupt_info;             /* crate/station which caused interrupt */
int interrupt_count;

DECLARE_WAIT_QUEUE_HEAD(wq);
struct fasync_struct *async_queue;

/********************************************************************/

/*---- open --------------------------------------------------------*/

static int khyt1331_open(struct inode *inode, struct file *file)
{
   unsigned int minor;

   minor = MINOR(inode->i_rdev);
   printk(KERN_INFO "khyt1331: open called by PID %u, on minor %d\n", current->pid,
          minor);

   spin_lock_irq(&khyt1331_lock);

   if (khyt1331_in_use) {
      /* return if device already open */
      spin_unlock_irq(&khyt1331_lock);
      return -EBUSY;
   }

   /* mark usage */
   khyt1331_in_use = 1;
   in_use_pid = current->pid;
   interrupt_info = 0;
   interrupt_count = 0;
   spin_unlock_irq(&khyt1331_lock);

   MOD_INC_USE_COUNT;
   return 0;
}

/*---- close -------------------------------------------------------*/

static int khyt1331_release(struct inode *inode, struct file *filep)
{
   unsigned int minor;

   minor = MINOR(inode->i_rdev);
   printk(KERN_INFO "khyt1331: release called by PID %d, on minor %d\n", current->pid,
          minor);

   /* mark device unused */
   spin_lock_irq(&khyt1331_lock);
   khyt1331_in_use = 0;
   spin_unlock_irq(&khyt1331_lock);

   /* remove file from asynchronous readers */
   khyt1331_fasync(-1, filep, 0);

   MOD_DEC_USE_COUNT;
   return 0;
}

/*---- interrupt handler -------------------------------------------*/

void khyt1331_interrupt(int irq, void *dev_id, struct pt_regs *regs)
{
   unsigned short adr, i;
   unsigned char status, enclam;

   /* figure out which crate caused interrupt */
   for (i = 0; i < 4; i++) {
      adr = io_base + (i << 4);
      status = INP(adr + 6);
      if (status != 0xFF && (status & (1 << 3))) {
         enclam = (INP(adr + 8) & 0x1F);        // mask upper 3 bits
         interrupt_info = (i << 16) | enclam;

         printk(KERN_INFO "khyt1331: interrupt crate %d, station %d\n", i, enclam);

         /* disable interrupts */
         OUTP(adr + 10, 40);

         /* increment interrupt counter */
         interrupt_count++;

         /* wake user process */
         wake_up_interruptible(&wq);

         /* send signal to user process */
         kill_fasync(&async_queue, SIGIO, POLL_IN);
      }
   }

   /* signal raw interrupt response time
      adr = io_base;
      OUTP(adr, 0xFF);
      OUTP(adr+8, 1);
      OUTP(adr+6, 1);
      OUTP(adr+10, 16);
      OUTP(adr, 0);
      OUTP(adr+10, 16);
    */

}

/*---- ioctl -------------------------------------------------------*/

static int khyt1331_ioctl(struct inode *inode, struct file *file,
                          unsigned int opcode, unsigned long param)
{
   int i, j;
   int c, n, a, f, data_size, write_flag, read_flag;
   unsigned char status;
   unsigned short adr = 0;
   unsigned int lam;
   CNAF_BUF cnaf_buf;
   void *buf;

   // printk(KERN_INFO "khyt1331: ioctl %d, param  %lX\n", opcode, param);

   switch (opcode) {
    /*---- initialization functions --------------------------------*/

   case OP_CRATE_ZINIT:
      if (param > 3 || param < 0)
         return -EINVAL;

      adr = io_base + (param << 4);
      OUTP(adr + 10, 34);
      break;

   case OP_CRATE_CLEAR:
      if (param > 3 || param < 0)
         return -EINVAL;

      adr = io_base + (param << 4);
      OUTP(adr + 10, 36);
      break;

   case OP_CRATE_CHECK:
      if (param > 3 || param < 0)
         return -EINVAL;

      adr = io_base + (param << 4);

      /* dummy clear inhibit operation */
      OUTP(adr + 8, 1);
      OUTP(adr + 6, 2);
      OUTP(adr + 10, 32);

      /* readback n, a, f */
      a = INP(adr + 10);
      n = INP(adr + 10);
      f = INP(adr + 10);

      if (n != 1 || a != 2 || f != 32)
         return 0;
      return 1;

    /*---- normal CAMAC functions ----------------------------------*/

   case OP_CNAF8:
   case OP_CNAF16:
   case OP_CNAF24:
      /*
         printk(KERN_INFO "khyt1331: CNAF: %d %d %d %d %d\n",
         cnaf_buf.c, cnaf_buf.n, cnaf_buf.a, cnaf_buf.f, cnaf_buf.d);
       */

      if (copy_from_user(&cnaf_buf, (void *) param, sizeof(cnaf_buf)) > 0)
         return -EFAULT;

      if (cnaf_buf.c > 3 || cnaf_buf.c < 0)
         return -EINVAL;

      adr = io_base + (cnaf_buf.c << 4);

      OUTP(adr + 8, cnaf_buf.n);

      if (cnaf_buf.f >= 8) {
         /* write operation */
         OUTP(adr, (unsigned char) cnaf_buf.d);
         if (opcode != OP_CNAF8) {
            OUTP(adr + 2, *(((unsigned char *) &cnaf_buf.d) + 1));
            if (opcode == OP_CNAF24)
               OUTP(adr + 4, *(((unsigned char *) &cnaf_buf.d) + 2));
         }
      }
      OUTP(adr + 6, cnaf_buf.a);
      OUTP(adr + 10, cnaf_buf.f);

      status = (unsigned char) INP(adr + 6);
      cnaf_buf.q = status & 1;
      cnaf_buf.x = status >> 7;

      if (cnaf_buf.f < 8) {
         cnaf_buf.d = 0;
         /* read operation */
         *((unsigned char *) &cnaf_buf.d) = INP(adr);
         if (opcode != OP_CNAF8) {
            *(((unsigned char *) &cnaf_buf.d) + 1) = INP(adr + 2);
            if (opcode == OP_CNAF24)
               *(((unsigned char *) &cnaf_buf.d) + 2) = INP(adr + 4);
         }
      }

      if (copy_to_user((void *) param, &cnaf_buf, sizeof(cnaf_buf)) > 0)
         return -EFAULT;
      break;

    /*---- repeat CAMAC functions ----------------------------------*/

   case OP_CNAF8R:
   case OP_CNAF16R:
   case OP_CNAF24R:

      data_size = opcode == OP_CNAF8R ? 1 : opcode == OP_CNAF16R ? 2 : 4;

      if (copy_from_user(&cnaf_buf, (void *) param, sizeof(cnaf_buf)) > 0)
         return -EFAULT;

      printk(KERN_INFO "khyt1331: CNAFR: %d %d %d %d %lX %d %d\n",
             cnaf_buf.c, cnaf_buf.n, cnaf_buf.a, cnaf_buf.f, cnaf_buf.d,
             cnaf_buf.r, cnaf_buf.m);

      /* check for valid crate parameter */
      if (cnaf_buf.c > 3 || cnaf_buf.c < 0)
         return -EINVAL;

      /* check for valid repeat count, allow 1M max. size */
      if (cnaf_buf.r < 0 || cnaf_buf.r > 1024 * 1024)
         return -EINVAL;

      /* check for valid repeat mode */
      if (cnaf_buf.m < RM_COUNT || cnaf_buf.m > RM_NSCAN)
         return -EINVAL;

      /* allocate local data buffer */
      buf = kmalloc(cnaf_buf.r * data_size, GFP_KERNEL);
      if (buf == NULL)
         return -ENOMEM;

      read_flag = (cnaf_buf.f < 8);
      write_flag = (cnaf_buf.f >= 16 && cnaf_buf.f <= 23);

      if (write_flag) {
         /* copy data buffer */
         if (copy_from_user(buf, (void *) cnaf_buf.d, cnaf_buf.r * data_size) > 0) {
            kfree(buf);
            return -EFAULT;
         }
      }

      /* Turn on the Q-mode for repeat until Q=1 in the INPW(adr+12) */
      if (cnaf_buf.m == RM_QMODE)
         OUTP(adr + 1, 0x10);

      adr = io_base + (cnaf_buf.c << 4);

      OUTP(adr + 8, cnaf_buf.n);
      OUTP(adr + 6, cnaf_buf.a);

      if (write_flag) {
         if (data_size == 1) {
            OUTP(adr, *((unsigned char *) buf));
         } else if (data_size == 2) {
            //printk("OUT0: %d\n", *((unsigned short *) buf));
            OUTP(adr, *((unsigned char *) buf));
            OUTP(adr + 2, *((unsigned char *) buf + 1));
         } else if (data_size == 4) {
            OUTP(adr, *((unsigned char *) buf));
            OUTP(adr + 2, *((unsigned char *) buf + 1));
            OUTP(adr + 4, *((unsigned char *) buf + 2));
         }
      }

      /* trigger first cycle */
      OUTP(adr + 10, cnaf_buf.f);

      if (read_flag) {
         if (data_size == 1) {
            *((unsigned char *) buf) = INP(adr);
         } else if (data_size == 2) {
            *((unsigned char *) buf) = INP(adr);
            *((unsigned char *) buf + 1) = INP(adr + 2);
         } else if (data_size == 4) {
            *((unsigned char *) buf) = INP(adr);
            *((unsigned char *) buf + 1) = INP(adr + 2);
            *((unsigned char *) buf + 1) = INP(adr + 4);
         }

         /* trigger first cycle */
         INPW(adr + 12);
         INP(adr + 6);          /* some delay */
      }

      /* repeat cycles */
      for (i = 1; i < cnaf_buf.r; i++) {
         if (write_flag) {
            if (data_size == 1) {
               OUTPW(adr + 12, *((unsigned char *) buf + i));
            } else if (data_size == 2) {
               //printk("OUT%d: %d\n", i, *((unsigned short *) buf+i));
               OUTPW(adr + 12, *((unsigned short *) buf + i));
            } else if (data_size == 4) {
               /* write high byte */
               OUTP(adr + 4, *((unsigned char *) buf + 4 * i + 2));

               /* write low and middle bytes to repeat register */
               OUTPW(adr + 12, *((unsigned short *) buf + 2 * i));
            }
         }

         if (read_flag) {
            if (data_size == 1) {
               *((unsigned char *) buf + i) = (unsigned char) INPW(adr + 12);
            } else if (data_size == 2) {
               *((unsigned short *) buf + i) = INPW(adr + 12);
            } else if (data_size == 4) {
               /* read high byte */
               *((unsigned char *) buf + 4 * i + 2) = INP(adr + 4);

               /* read low and middle bytes from repeat register */
               *((unsigned short *) buf + i) = INPW(adr + 12);
            }
         }

         if (cnaf_buf.m == RM_QMODE) {
            /* in Q mode, test FAIL bit in status register */
            status = (unsigned char) INP(adr + 6);
            if ((status & (1 << 5)) > 0)
               break;
         }

         if (cnaf_buf.m == RM_ASCAN)
            OUTP(adr + 6, ++cnaf_buf.a);

         if (cnaf_buf.m == RM_NSCAN)
            OUTP(adr + 8, ++cnaf_buf.n);

         for (j = 0; j < 1000; j++)
            udelay(1000);
      }

      /* Turn off the Q-mode */
      if (cnaf_buf.m == RM_QMODE)
         OUTP(adr + 1, 0);

      if (read_flag)
         if (copy_to_user((void *) cnaf_buf.d, buf, i * data_size) > 0) {
            kfree(buf);
            return -EFAULT;
         }

      kfree(buf);
      break;

   case OP_INHIBIT_SET:
      if (param > 3 || param < 0)
         return -EINVAL;

      adr = io_base + (param << 4);
      OUTP(adr + 10, 33);
      break;

   case OP_INHIBIT_CLEAR:
      if (param > 3 || param < 0)
         return -EINVAL;

      adr = io_base + (param << 4);
      OUTP(adr + 10, 32);
      break;

   case OP_INHIBIT_TEST:
      if (param > 3 || param < 0)
         return -EINVAL;

      adr = io_base + (param << 4);
      status = INP(adr + 6);
      return (status & (1 << 1)) > 0;

   case OP_LAM_ENABLE:
      c = param >> 16;
      n = param & 0xFF;
      if (c > 3 || c < 0 || n > 24 || n < 0)
         return -EINVAL;

      adr = io_base + (c << 4);
      OUTP(adr + 10, 64 + n);
      break;

   case OP_LAM_DISABLE:
      c = param >> 16;
      n = param & 0xFF;
      if (c > 3 || c < 0 || n > 24 || n < 0)
         return -EINVAL;

      adr = io_base + (c << 4);
      OUTP(adr + 10, 128 + n);
      break;

   case OP_LAM_READ:
      /*
         return a BITWISE coded station NOT the station number
         i.e.: n = 5  ==> lam = 0x10
       */

      if (param > 3 || param < 0)
         return -EINVAL;

      adr = io_base + (param << 4);
      status = INP(adr + 6);
      if (status & (1 << 3)) {
         lam = (INP(adr + 8) & 0x1F);   // mask upper 3 bits
         lam = 1 << (lam - 1);
      } else
         lam = 0;
      return lam;

   case OP_LAM_CLEAR:
      c = param >> 16;
      n = param & 0xFF;
      if (c > 3 || c < 0 || n > 24 || n < 0)
         return -EINVAL;

      adr = io_base + (c << 4);

      /* restart LAM scanner in controller */
      INP(adr + 8);
      break;

   case OP_INTERRUPT_ENABLE:
      if (param > 3 || param < 0)
         return -EINVAL;

      adr = io_base + (param << 4);
      OUTP(adr + 10, 41);
      break;

   case OP_INTERRUPT_DISABLE:
      if (param > 3 || param < 0)
         return -EINVAL;

      adr = io_base + (param << 4);
      OUTP(adr + 10, 40);
      break;

   case OP_INTERRUPT_TEST:
      if (param > 3 || param < 0)
         return -EINVAL;

      adr = io_base + (param << 4);
      status = INP(adr + 6);
      return (status & (1 << 2)) > 0;
   }

   /* signal success */
   return 1;
}

/*---- read --------------------------------------------------------*/

int khyt1331_read(struct file *filep, char *buf, size_t count, loff_t * ppos)
{
   while (!interrupt_info) {
      if (filep->f_flags & O_NONBLOCK)
         return -EAGAIN;

      if (signal_pending(current))
         return -ERESTARTSYS;

      interruptible_sleep_on(&wq);
   }

   copy_to_user(buf, &interrupt_info, 4);
   interrupt_info = 0;
   return 4;
}

/*---- poll --------------------------------------------------------*/

unsigned int khyt1331_poll(struct file *filep, poll_table * wait)
{
   poll_wait(filep, &wq, wait);

   if (interrupt_info)
      return POLLIN | POLLRDNORM;

   return 0;
}

/*---- fasync ------------------------------------------------------*/

int khyt1331_fasync(int fd, struct file *filep, int mode)
{
   printk("khyt1331: fasync %d\n", mode);
   return fasync_helper(fd, filep, mode, &async_queue);
}

/*---- info rovided for /proc/khyt1331 interface -------------------*/

int khyt1331_read_procmem(char *buf, char **start, off_t offset, int count,
                          int *eof, void *data)
{
   int i, len;

   if (io_base) {
      len =
          sprintf(buf, "Hytec 5331 card found at address 0x%X, using interrupt %d\n",
                  io_base, irq);

      if (khyt1331_in_use)
         len += sprintf(buf + len, "Device in use by process with PID %d\n", in_use_pid);
      else
         len += sprintf(buf + len, "Device not in use\n");

      if (!khyt1331_in_use) {
         for (i = 0; i < 4; i++)
            len += sprintf(buf + len, "CAMAC crate %d: %s\n", i,
                           khyt1331_ioctl(NULL, NULL, OP_CRATE_CHECK, i) ?
                           "responding" : "not responding");
      } else
         len += sprintf(buf + len, "Received %d interrupts\n", interrupt_count);
   } else
      len = sprintf(buf, "No Hytec 5331 card found\n");

   *eof = 1;
   return len;
}

/*---- jump table for direcio operations ---------------------------*/

static struct file_operations khyt1331_fops = {
   ioctl:&khyt1331_ioctl,
   open:&khyt1331_open,
   read:&khyt1331_read,
   poll:&khyt1331_poll,
   release:&khyt1331_release,
   fasync:&khyt1331_fasync,
};

/*---- init_module -------------------------------------------------*/

int init_module(void)
{
   struct pci_dev *dev;
   int err;

   printk(KERN_INFO "khyt1331: start initialization\n");

   if (register_chrdev(HYT1331_MAJOR, "hyt1331", &khyt1331_fops)) {
      printk(KERN_ERR "khyt1331: unable to get major %d\n", HYT1331_MAJOR);
      return -EBUSY;
   }

   /* PCI scan */
   dev = NULL;
   dev = pci_find_device(0x1196, 0x5331, dev);
   if (dev != NULL) {
      io_base = pci_resource_start(dev, 3);
      irq = dev->irq;
      printk(KERN_INFO "khyt1331: Found 5331 card at %X, irq %d\n", io_base, irq);

      /* request IO ports */

      if ((err = check_region(io_base, 0x40)) < 0) {
         printk(KERN_INFO "khyt1331: Cannot request IO address %X\n", io_base);
         return err;
      }

      request_region(io_base, 0x40, "khyt1331");

      /* request interrupt */
      if (request_irq(irq, khyt1331_interrupt, SA_SHIRQ, "khyt1331", &khyt1331_fops)) {
         printk(KERN_INFO "khyt1331: Cannot request interrupt %d\n", irq);
         irq = -1;
      }
   } else {
      printk(KERN_INFO "khyt1331: No 5331 card found\n");
      return -ENODEV;
   }

   /* register /proc/khyt1331 interface */
   create_proc_read_entry("khyt1331", 0, NULL, khyt1331_read_procmem, NULL);

   printk(KERN_INFO "khyt1331: initialization finished\n");

   return 0;
}

/*---- cleanup_module ----------------------------------------------*/

void cleanup_module(void)
{
   release_region(io_base, 0x40);

   if (irq > 0)
      free_irq(irq, &khyt1331_fops);

   if (unregister_chrdev(HYT1331_MAJOR, "hyt1331"))
      printk(KERN_ERR "khyt1331: can't unregister device\n");

   remove_proc_entry("khyt1331", NULL);

   printk(KERN_INFO "khyt1331: driver removed\n");
}
