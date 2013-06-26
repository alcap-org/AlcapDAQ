#include "vme_io.h"

struct vme_handle
{
  bool used;
  int fd;
  unsigned long vme_base;
  int size;
  volatile unsigned char *base;
  int reference_count;
  struct vme_mapping_ctrl mapping;
  int fifo_block_size;
};

struct vme_handle *vme_open(unsigned long vme_addr, 
			    struct vme_mapping_ctrl mapping,
			    int size,
			    int fifo_block_size = 0);

void vme_close(struct vme_handle *handle);

int vme_dma_read(struct vme_handle *handle,
		unsigned long vme_addr, 
		unsigned char *buffer,
 	        int size);

int vme_dma_write(struct vme_handle *handle, 
		  unsigned long vme_addr, 
		  unsigned char *buffer,
		  int size);

inline unsigned long vme_read_d32(struct vme_handle *handle, 
				  unsigned long vme_addr)
{
  volatile unsigned long *p = (volatile unsigned long *)
    (handle->base + (vme_addr - handle->vme_base));
  return *p;
}

inline unsigned short vme_read_d16(struct vme_handle *handle, 
				   unsigned long vme_addr)
{
  volatile unsigned short *p = (volatile unsigned short *)
    (handle->base + (vme_addr - handle->vme_base));
  return *p;
}

inline unsigned char vme_read_d8(struct vme_handle *handle, 
				 unsigned long vme_addr)
{
  volatile unsigned char *p = (volatile unsigned char *)
    (handle->base + (vme_addr - handle->vme_base));
  return *p;
}

inline void vme_write_d32(struct vme_handle *handle, 
			  unsigned long vme_addr,
			  unsigned long value)
{
  volatile unsigned long *p = (volatile unsigned long *)
    (handle->base + (vme_addr - handle->vme_base));
  *p = value;
}

inline void vme_write_d16(struct vme_handle *handle, 
			  unsigned long vme_addr,
			  unsigned short value)
{
  volatile unsigned short *p = (volatile unsigned short *)
    (handle->base + (vme_addr - handle->vme_base));
  *p = value;
}

inline void vme_write_d8(struct vme_handle *handle, 
			 unsigned long vme_addr,
			 unsigned char value)
{
  volatile unsigned char *p = (volatile unsigned char *)
    (handle->base + (vme_addr - handle->vme_base));
  *p = value;
}

inline unsigned long vme_read_d32_checked(struct vme_handle *handle, 
					  unsigned long vme_addr,
					  int *status = NULL)
{
  unsigned long result;

  int status1 = vme_dma_read(handle, vme_addr, (unsigned char *) &result, 
		             sizeof(unsigned long));

  if(status != NULL) {
    if(status1 == sizeof(unsigned long)) {
      *status = SUCCESS;
    } else {
      *status = -1;
    }
  }

  return result;
}

inline unsigned short vme_read_d16_checked(struct vme_handle *handle, 
					   unsigned long vme_addr,
					   int *status = NULL)
{
  unsigned short result;

  int status1 = vme_dma_read(handle, vme_addr, (unsigned char *) &result, 
		             sizeof(unsigned short));

  if(status != NULL) {
    if(status1 == sizeof(unsigned short)) {
      *status = SUCCESS;
    } else {
      *status = -1;
    }
  }

  return result;
}

inline unsigned char vme_read_d8_checked(struct vme_handle *handle, 
					 unsigned long vme_addr,
					 int *status = NULL)
{
  unsigned short result;

  int status1 = vme_dma_read(handle, vme_addr, (unsigned char *) &result, 
		             sizeof(unsigned char));

  if(status != NULL) {
    if(status1 == sizeof(unsigned char)) {
      *status = SUCCESS;
    } else {
      *status = -1;
    }
  }

  return result;
}

inline int vme_write_d32_checked(struct vme_handle *handle, 
				 unsigned long vme_addr,
				 unsigned long value)

{
  int status = vme_dma_write(handle, vme_addr, (unsigned char *) &value, 
		             sizeof(unsigned long));

  if(status == sizeof(unsigned long)) {
    return SUCCESS;
  } else {
    return -1;
  }
}

inline int vme_write_d16_checked(struct vme_handle *handle, 
				 unsigned long vme_addr,
				 unsigned short value)
{
  int status = vme_dma_write(handle, vme_addr, (unsigned char *) &value, 
		             sizeof(unsigned short));

  if(status == sizeof(unsigned short)) {
    return SUCCESS;
  } else {
    return -1;
  }

}

inline int vme_write_d8_checked(struct vme_handle *handle, 
				unsigned long vme_addr,
				unsigned char value)
{
  int status = vme_dma_write(handle, vme_addr, (unsigned char *) &value, 
		             sizeof(unsigned char));
  
  if(status == sizeof(unsigned char)) {
    return SUCCESS;
  } else {
    return -1;
  }
}

