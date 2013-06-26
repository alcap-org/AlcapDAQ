#include <stdio.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <asm/page.h>

#include "midas.h" 

#include "crate.h"
#include "vme_universe.h"
#include "diag.h"

#if 0
extern "C" {
  ssize_t pread (int __fd, void *__buf, size_t __nbytes, off_t __offset);
  ssize_t pwrite (int __fd, __const void *__buf, size_t __n, off_t offset);
};
#endif

#define MAX_VME_MAPPED_SIZE (64*1024*1024)

#define MAX_VME_HANDLES 10
struct vme_handle vme_handles[MAX_VME_HANDLES];

inline unsigned long page_round_down(unsigned long addr)
{
  return addr & PAGE_MASK;
}

inline unsigned long page_round_up(unsigned long addr)
{
  if(addr & ~PAGE_MASK == 0) {
    return addr;
  } else {
    return (addr & PAGE_MASK) + PAGE_SIZE;
  }
}

void vme_init()
{
  // Mark all handles as unused
  for(int i = 0; i < MAX_VME_HANDLES; i++) {
    vme_handles[i].used = false;
  }
}

struct vme_handle *vme_open(unsigned long vme_addr, 
			    struct vme_mapping_ctrl mapping,
			    int size,
			    int fifo_block_size)
{
  struct vme_handle *handle = NULL;
  unsigned long new_vme_base = 0;
  unsigned long new_vme_size = 0;

  vme_addr = page_round_down(vme_addr);
  unsigned long vme_addr_end = page_round_up(vme_addr + size);

  // Search through the currently-allocated windows, looking for
  // one that would be compatible.
  for(int i = 0; i < MAX_VME_HANDLES; i++) {
    if(vme_handles[i].used &&
       vme_handles[i].mapping.address_space == mapping.address_space &&
       vme_handles[i].mapping.supuseram == mapping.supuseram &&
       vme_handles[i].mapping.prgdataam == mapping.prgdataam &&
       vme_handles[i].fifo_block_size == fifo_block_size) {

      new_vme_base = 
	MIN(vme_addr, vme_handles[i].vme_base);
      unsigned long new_vme_addr_end = 
	MAX(vme_addr_end, vme_handles[i].vme_base + vme_handles[i].size);
      new_vme_size = new_vme_addr_end - new_vme_base;

      if(new_vme_size <= MAX_VME_MAPPED_SIZE) {
	handle = &vme_handles[i];
	break;
      }
    }
  }

  // If we didn't find one, then look for a brand-new window
  if(handle == NULL) {
    new_vme_base = vme_addr;
    new_vme_size = vme_addr_end - vme_addr;

    for(int i = 0; i < MAX_VME_HANDLES; i++) {
      if(!(vme_handles[i].used)) {
	handle = &vme_handles[i];
      }
    }
  }

  // If the window is brand-new, then open a file descriptor for it.
  if(!handle->used) {

    // Try each of the device special files
    for(int i = 0; i < 8; i++) {
      char filename[20];
      sprintf(filename, "/dev/vme_a32_%d", i);
      handle->fd = open(filename, O_RDWR);
      if(handle->fd >= 0) {
	break;
      }
    }

    // Check that we found one
    if(handle->fd < 0) {
      diag_print(0, "Unable to open a VME device file\n");
      return NULL;
    }
  }
  // Otherwise, unmap the associated memory 
  else {
    munmap((void *) handle->base, handle->size);
  }

  // Set the address space mapping
  if(ioctl(handle->fd, VMEIMG_SETMAPPING, &mapping)) {
    perror("ioctl VMEIMG_SETMAPPING");
    return NULL;
  }

  // Set the VME base address appropriately
  if(ioctl(handle->fd, VMEIMG_SETVMEADDR, new_vme_base)) {
    perror("ioctl VMEIMG_SETVMEADDR");
    return NULL;
  }

  // Retrieve the base address that the driver actually accepted
  if(ioctl(handle->fd, VMEIMG_GETVMEADDR, &handle->vme_base)) {
    perror("ioctl VMEIMG_GETVMEADDR");
    return NULL;
  }

  // Map some memory for the region of interest
  handle->base = (volatile unsigned char *) 
    mmap(NULL, new_vme_size, PROT_WRITE | PROT_READ, MAP_SHARED, handle->fd, 0);
  if(handle->base == (volatile unsigned char *) -1) {
    perror("mmap VME address space");
    return NULL;
  }

  // Remember the basic properties of this handle
  handle->used = true;
  handle->mapping.max_datawidth = mapping.max_datawidth;
  handle->mapping.address_space = mapping.address_space;
  handle->mapping.supuseram = mapping.supuseram;
  handle->mapping.prgdataam = mapping.prgdataam;
  handle->fifo_block_size = fifo_block_size;
  handle->size = new_vme_size;  

  // Keep a reference count
  handle->reference_count++;

  return handle;
}

void vme_close(struct vme_handle *handle)
{
  handle->reference_count--;

  if(handle->reference_count == 0) {
    int status = munmap((void *) handle->base, handle->size);
    if(status < 0) {
        perror("Error unmapping VME:");
    }
    status = close(handle->fd);
    if(status < 0) {
        perror("Error closing VME:");
    }
    handle->used = false;
  }
}

int vme_dma_read(struct vme_handle *handle,
			unsigned long vme_addr, 
			unsigned char *buffer,
			int size)
{
  struct vme_dma_req dma_req;

  dma_req.vme_addr = vme_addr;
  dma_req.buf = buffer;
  dma_req.count = size;
  dma_req.fifo_block_size = handle->fifo_block_size;

  int bytes_read = ioctl(handle->fd, VMEIMG_DMA_READ, &dma_req);

  return bytes_read;

#if 0
  if(handle->fifo_block_size == 0) {
    return pread(handle->fd, buffer, size, vme_addr - handle->vme_base);
  } else {

    int bytes_read = 0;

    while(bytes_read < size) {
      
      int size_this_time = MIN(size - bytes_read, handle->fifo_block_size);
      
      int status = pread(handle->fd, buffer + bytes_read, size_this_time,
			 vme_addr - handle->vme_base);

      if(status > 0) {
          bytes_read += status;
      }

      if(status < size_this_time) {
	break;
      }
    }

    return bytes_read;
  }
#endif
}

int vme_dma_write(struct vme_handle *handle,
		  unsigned long vme_addr, 
		  unsigned char *buffer,
		  int size)
{
  struct vme_dma_req dma_req;

  dma_req.vme_addr = vme_addr;
  dma_req.buf = buffer;
  dma_req.count = size;
  dma_req.fifo_block_size = handle->fifo_block_size;

  int bytes_written = ioctl(handle->fd, VMEIMG_DMA_WRITE, &dma_req);

  return bytes_written;

#if 0
  if(handle->fifo_block_size == 0) {
    return pwrite(handle->fd, buffer, size, vme_addr - handle->vme_base);
  } else {
    
    int bytes_written = 0;
    
    while(bytes_written < size) {
      
      int size_this_time = MIN(size - bytes_written, handle->fifo_block_size);
      
      int status = pwrite(handle->fd, buffer + bytes_written, size_this_time,
			 vme_addr - handle->vme_base);

      if(status > 0) {
          bytes_written += status;
      }

      if(status < size_this_time) {
	break;
      }
    }

    return bytes_written;
  }
#endif
}
