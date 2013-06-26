/*
  $Id: vme_io.h,v 1.1 2004/09/22 10:36:51 mucap Exp $
 */
#ifndef __LINUX_VME_IO_H
#define __LINUX_VME_IO_H

#include <linux/ioctl.h>

/* ioctl()'s for the VME-Access */
/* first start with the control device (/dev/vme_ctl) which is used
   for operations affecting the entire bus, such as resetting, setting
   arbitration and so on */

/* execute a VMEBus reset */
#define VMECTL_RESET		_IO('x', 0)

/* get/set VMEBus time out, in usec */
#define VMECTL_GETVMETO		_IOR('x', 1, int)
#define VMECTL_SETVMETO		_IOW('x', 1, int)

#define VMEMAP_DWIDTH_8 (0)
#define VMEMAP_DWIDTH_16 (1)
#define VMEMAP_DWIDTH_32 (2)
#define VMEMAP_DWIDTH_64 (3)

#define VMEMAP_ASPACE_A16 (0)
#define VMEMAP_ASPACE_A24 (1)
#define VMEMAP_ASPACE_A32 (2)
#define VMEMAP_ASPACE_CRCSR (5)
#define VMEMAP_ASPACE_USR1 (6)
#define VMEMAP_ASPACE_USR2 (7)

#define VMEMAP_SUPUSERAM_USER (0)
#define VMEMAP_SUPUSERAM_SUPER (1)

#define VMEMAP_PRGDATAAM_DATA (0)
#define VMEMAP_PRGDATAAM_PRG (1)

#define VMEMAP_VMESLAVE_USER (0x01)
#define VMEMAP_VMESLAVE_SUPER (0x02)

#define VMEMAP_VMESLAVE_DATA (0x01)
#define VMEMAP_VMESLAVE_PRG (0x02)

struct vme_mapping_ctrl {
  char max_datawidth;
  char address_space;
  char supuseram;
  char prgdataam;
};

#define VMEIMG_GETMAPPING _IOR('x',2,struct vme_mapping_ctrl)
#define VMEIMG_SETMAPPING _IOW('x',2,struct vme_mapping_ctrl)

#define VMEIMG_GETVMEADDR _IOR('x',3,unsigned long)
#define VMEIMG_SETVMEADDR _IOW('x',3,unsigned long)

#define VMEIMG_GETGRANULARITY _IOR('x',4,unsigned long)

struct vme_irq_data_type {
  int nof_missed_interrupts;
  int vector_number;
};

struct vme_irq_mapping_type {
  unsigned char level;
  unsigned char vector;
};

#define VMEIRQ_GETLINK _IOR('x',6,struct vme_irq_mapping_type)
#define VMEIRQ_SETLINK _IOW('x',6,struct vme_irq_mapping_type)

struct DMA_chainblock_type {
  unsigned int vme_address;		/* the VME-Address of the block */
  size_t n_bytes;			/* the size of the block */
  int          offset;			/* The offset in bytes from the 
					   start of the buffer */
  struct vme_mapping_ctrl mapping;	/* the access mode for the VME bus */
  int          write;			/* != 0 for writing to VME,
					   0 for reading from VME */
};


#define VMEDMA_SETBUFFSIZE _IOW('x',7,size_t)
#define VMEDMA_GETBUFFSIZE _IOR('x',7,size_t)
#define VMEDMA_DODMA      _IO('x',8)
#define VMEDMA_ADDBLOCK	 _IOW('x',9,struct DMA_chainblock_type)
#define VMEDMA_CLEARBLOCKS _IO('x',10)


#define VMESLAVE_SETSIZE _IOW('x',11,size_t)
#define VMESLAVE_GETSIZE _IOR('x',11,size_t)
#endif

struct vme_dma_req
{
  unsigned int vme_addr;
  unsigned char *buf;
  unsigned int count;
  int fifo_block_size;
};

#define VMEIMG_DMA_READ _IOW('x', 12, struct vme_dma_req)
#define VMEIMG_DMA_WRITE _IOW('x', 13, struct vme_dma_req)

