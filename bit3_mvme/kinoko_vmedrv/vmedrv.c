/* vmedrv.c */
/* VME device driver for Bit3 Model 617/618/620 on Linux 2.6.x */
/* Created by Enomoto Sanshiro on 28 November 1999. */
/* Updated by Enomoto Sanshiro on 7 September 2008. */
/* Updated by Enomoto Sanshiro on 24 February 2012. */


#define TRACE_CONFIG 1
#ifdef TRACE_CONFIG
#define DEBUG_CONFIG(x) x
#else
#define DEBUG_CONFIG(x)
#endif

#ifdef TRACE_PIO
#define DEBUG_PIO(x) x
#else
#define DEBUG_PIO(x)
#endif

#ifdef TRACE_MAP
#define DEBUG_MAP(x) x
#else
#define DEBUG_MAP(x)
#endif

#ifdef TRACE_INTERRUPT
#define DEBUG_INT(x) x
#else
#define DEBUG_INT(x)
#endif

#ifdef TRACE_DMA
#define DEBUG_DMA(x) x
#else
#define DEBUG_DMA(x)
#endif


#if defined(USE_MODVERSIONS) && USE_MODVERSIONS
#  define MODVERSIONS
#  include <config/modversions.h>
#endif


#include <linux/version.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/ioctl.h>
#include <linux/mm.h>
#include <linux/poll.h>
#include <linux/sched.h>
#include <linux/interrupt.h>
#include <linux/pci.h>
#include <linux/ioport.h>
#include <linux/errno.h>
#include <linux/dma-mapping.h>
#include <asm/io.h>
#include <asm/uaccess.h>
#include <asm/segment.h>
#include "vmedrv.h"
#include "vmedrv_params.h"
#include "vmedrv_conf.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Enomoto Sanshiro");
MODULE_DESCRIPTION("VME Driver for SBS (Bit3) 617/618/620 Bus Bridge");


#ifdef SYSTEM_FC2
#include "vmedrv-FC2.h"
#endif
#ifdef SYSTEM_FC3
#include "vmedrv-FC3.h"
#endif


#if (LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 18))
#define USE_OLD_IRQ 1
#endif
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 36))
#define USE_OLD_IOCTL 1
#endif
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 32))
#define USE_OLD_DMABITMASK 1
#endif



struct interrupt_client_t {
    struct task_struct* task;
    int irq;
    int vector;
    int signal_id;
    int interrupt_count;
    int autodisable_flag;
    int vector_mask;
    struct interrupt_client_t* next;
};

struct local_interrupt_client_t {
    struct interrupt_client_t* interrupt_client;
    struct local_interrupt_client_t* next;
};

struct bit3_config_t {
    int dev_id;
    struct pci_dev* pci_dev;
    unsigned char irq;
    unsigned io_node_io_base;
    void* mapped_node_io_base;
    void* mapping_registers_base;
    void* dma_mapping_registers_base;
    void* window_region_base;
    unsigned long physical_window_region_base;
    char window_status_table[bit3_NUMBER_OF_WINDOWS];
    struct interrupt_client_t* interrupt_client_list[vmeNUMBER_OF_IRQ_LINES];
    unsigned interrupt_enabling_flags;
    unsigned saved_interrupt_flags;
    int is_dma_available;
    void* dma_buffer;
    unsigned long dma_buffer_size;
    unsigned long dma_buffer_bus_address;
    unsigned long dma_buffer_mapped_pci_address;
};

struct dev_prop_t {
    unsigned address_modifier;
    unsigned dma_address_modifier;
    unsigned function_code;
    unsigned byte_swapping;
    unsigned mapping_flags;
    unsigned dma_mapping_flags;
    unsigned data_width;
    unsigned transfer_method;
    void* pio_buffer;
    int pio_window_index;
    int number_of_pio_windows;
    int mmap_window_index;
    int number_of_mmap_windows;
    struct local_interrupt_client_t* local_interrupt_client;
};


static int vmedrv_open(struct inode* inode, struct file* filep);
static int vmedrv_release(struct inode* inode, struct file* filep);
static ssize_t vmedrv_read(struct file* filep, char* buf, size_t count, loff_t* f_pos);
static ssize_t vmedrv_write(struct file* filep, const char* buf, size_t count, loff_t* f_pos);
static loff_t vmedrv_lseek(struct file* filep, loff_t offset, int whence);
static long vmedrv_unlocked_ioctl(struct file *filep, unsigned int cmd, unsigned long arg);
static int vmedrv_mmap(struct file* filep, struct vm_area_struct* vma);
static unsigned int vmedrv_poll(struct file* filep, poll_table* table);

#ifdef USE_OLD_IRQ
static irqreturn_t vmedrv_interrupt(int irq, void* dev_id, struct pt_regs* regs);
#else
static irqreturn_t vmedrv_interrupt(int irq, void* dev_id);
#endif

static int vmedrv_pci_probe(struct pci_dev* dev, const struct pci_device_id* id);
static void vmedrv_pci_remove(struct pci_dev* dev);

static int initialize(void);
static int set_access_mode(struct dev_prop_t* dev_prop, int mode);
static int set_transfer_method(struct dev_prop_t* dev_prop, int method);
static int probe(struct dev_prop_t* dev_prop, __user vmedrv_word_access_t* word_access);

static int pio_read(struct dev_prop_t* dev_prop, char* buf, unsigned long vme_address, int count);
static int pio_write(struct dev_prop_t* dev_prop, const char* buf, unsigned long vme_address, int count);
static int prepare_pio(struct dev_prop_t* dev_prop);
static int allocate_windows(int number_of_windows);
static void free_windows(int window_index, int number_of_windows);
static void* map_windows(unsigned vme_address, unsigned window_index, unsigned number_of_windows, unsigned flags);

static int enable_normal_interrupt(void);
static int disable_normal_interrupt(void);
static int enable_error_interrupt(void);
static int disable_error_interrupt(void);
static void save_interrupt_flags(void);
static void restore_interrupt_flags(void);
static int acknowledge_error_interrupt(unsigned local_status);
static int acknowledge_pt_interrupt(unsigned local_status);
static int acknowledge_dma_interrupt(unsigned dma_status);
static int acknowledge_pr_interrupt(unsigned remote_status);
static int acknowledge_vme_interrupt(unsigned interrupt_status);
static int register_interrupt_notification(struct dev_prop_t* dev_prop, struct task_struct* task, int irq, int vector, int signal_id);
static int unregister_interrupt_notification(struct dev_prop_t* dev_prop, struct task_struct* task, int irq, int vector);
static int wait_for_interrupt_notification(struct task_struct* task, int irq, int vector, int timeout);
static int check_interrupt_notification(int irq, int vector);
static int clear_interrupt_notification(int irq, int vector);
static int set_interrupt_autodisable(int irq, int vector);
static int set_vector_mask(int irq, int vector, int vector_mask);
static int reset_adapter(void);
 
static int dma_read(struct dev_prop_t* dev_prop, char* buf, unsigned long vme_adress, int count);
static int dma_write(struct dev_prop_t* dev_prop, const char* buf, unsigned long vme_adress, int count);
static int prepare_dma(struct dev_prop_t* dev_prop);
static int start_dma(struct dev_prop_t* dev_prop, unsigned long pci_address, unsigned long vme_address, unsigned long size, int direction);
static void* allocate_dma_buffer(unsigned long size);
static void release_dma_buffer(void);
static unsigned map_dma_windows(unsigned pci_address, unsigned size, unsigned dma_flags);
static int initiate_dma(struct dev_prop_t* dev_prop, unsigned mapped_pci_address, unsigned vme_address, unsigned size, int direction);
static int release_dma(void);


#ifdef USE_OLD_IOCTL
static int vmedrv_ioctl(struct inode *inode, struct file *filep, unsigned int cmd, unsigned long arg);
#endif


static struct file_operations vmedrv_fops = {
    .owner = THIS_MODULE,
    .llseek = vmedrv_lseek,
    .read = vmedrv_read,
    .write = vmedrv_write,
#ifdef USE_OLD_IOCTL
    .ioctl = vmedrv_ioctl,
#else
    .unlocked_ioctl = vmedrv_unlocked_ioctl,
#endif
    .mmap = vmedrv_mmap,
    .poll = vmedrv_poll,
    .open = vmedrv_open,
    .release = vmedrv_release,
};

static struct pci_driver vmedrv_pci_driver = {
    .id_table = vmedrv_device_id_table,
    .probe = vmedrv_pci_probe,
    .remove = vmedrv_pci_remove,
};

static wait_queue_head_t vmedrv_dmadone_wait_queue;
static wait_queue_head_t vmedrv_vmebusirq_wait_queue;
static int is_dma_running;
static int module_open_count;
static int bit3_card_count = 0;
static int error_interrupt_count;
static int local_status_on_error;

static dev_t dev;
static struct cdev cdev;

static struct bit3_config_t bit3;


static int __init vmedrv_initialize_module(void)
{
    int result = -ENODEV;

    bit3.pci_dev = NULL;
    vmedrv_pci_driver.name = vmedrv_name;
    result = pci_register_driver(&vmedrv_pci_driver);
    if (result < 0) {
	printk(KERN_WARNING "%s: unable to register PCI driver\n", vmedrv_name);
	goto FAIL_PCI_DRIVER_REGISTRATION;
    }
    if (bit3.pci_dev == NULL) {
	printk(KERN_WARNING "%s: unable to find VME-PCI Bus Adapter.\n", vmedrv_name);
	result = -ENODEV;
	goto FAIL_DEVICE_DETECTION;
    }

    if (vmedrv_major == 0) {
	result = alloc_chrdev_region(
	    &dev, 0, vmedrv_NUMBER_OF_MINOR_IDS, vmedrv_name
	);
    }
    else {
	dev = MKDEV(vmedrv_major, 0);
	result = register_chrdev_region(
	    dev, vmedrv_NUMBER_OF_MINOR_IDS, vmedrv_name
	);
    }
    if (result < 0) {
        printk(KERN_WARNING "%s: unable to get major %d\n", vmedrv_name, vmedrv_major);
	goto FAIL_DRIVER_REGISTRATION;
    }
    if (vmedrv_major == 0) {
	vmedrv_major = MAJOR(dev);
    }

    bit3.interrupt_enabling_flags = 0;
    bit3.dma_buffer_size = 0;

    bit3.mapped_node_io_base = 0;
    bit3.mapping_registers_base = 0;
    bit3.dma_mapping_registers_base = 0;
    bit3.window_region_base = 0;
    module_open_count = 0;
    error_interrupt_count = 0;
    local_status_on_error = 0;

    bit3.io_node_io_base = (
	bit3.pci_dev->resource[bit3_IO_NODE_IO_BASE_INDEX].start
    );
    bit3.irq = bit3.pci_dev->irq;

    init_waitqueue_head(&vmedrv_dmadone_wait_queue);
    init_waitqueue_head(&vmedrv_vmebusirq_wait_queue);

    cdev_init(&cdev, &vmedrv_fops);
    cdev.owner = THIS_MODULE;
    cdev.ops = &vmedrv_fops;
    result = cdev_add(&cdev, dev, vmedrv_NUMBER_OF_MINOR_IDS);
    if (result < 0) {
        printk(KERN_WARNING "%s: unable to register driver.\n", vmedrv_name);
	goto FAIL_CDEV_REGISTRATION;
    }

    printk(KERN_INFO "%s: successfully installed at 0x%04x on irq %d (major = %d).\n", 
        vmedrv_name, bit3.io_node_io_base, bit3.irq, vmedrv_major
    );

    return 0;
    
  FAIL_CDEV_REGISTRATION:
    unregister_chrdev_region(dev, vmedrv_NUMBER_OF_MINOR_IDS);
  FAIL_DRIVER_REGISTRATION:
    ;
  FAIL_DEVICE_DETECTION:
    pci_unregister_driver(&vmedrv_pci_driver);
  FAIL_PCI_DRIVER_REGISTRATION:
    ;

    return result;  
}


static void __exit vmedrv_cleanup_module(void)
{
    release_dma_buffer();

    cdev_del(&cdev);
    unregister_chrdev_region(dev, vmedrv_NUMBER_OF_MINOR_IDS);

    pci_unregister_driver(&vmedrv_pci_driver);

    printk(KERN_INFO "%s: removed.\n", vmedrv_name);
}


module_init(vmedrv_initialize_module);
module_exit(vmedrv_cleanup_module);


static int vmedrv_open(struct inode* inode, struct file* filep)
{
    struct dev_prop_t* dev_prop;
    int minor_id, mode, method;

    minor_id = MINOR(inode->i_rdev);
    if (minor_id >= vmedrv_NUMBER_OF_MINOR_IDS) {
	printk(KERN_WARNING "%s: invalid minor ID.\n", vmedrv_name);
        return -ENODEV;
    }

    filep->private_data = kmalloc(sizeof(struct dev_prop_t), GFP_KERNEL);
    if (filep->private_data == 0) {
        printk(KERN_WARNING "%s: unable to allocate memory.", vmedrv_name);
        return -ENOMEM;
    }
    dev_prop = filep->private_data;

    while (module_open_count == 0) {
	struct resource* resource;
	int result;

	resource = request_region(
	    bit3.io_node_io_base, bit3_IO_NODE_IO_SIZE, vmedrv_name
	);
	if (resource == NULL) {
	    printk(KERN_WARNING "%s: I/O region request fault.\n", vmedrv_name);
	    result = -EIO;
	    goto FAIL_REGION_REQUEST;
	}

	result = initialize();
	if (result < 0) {
	    printk(KERN_WARNING "%s: initialization fault.\n", vmedrv_name);
	    goto FAIL_INITIALIZATION;
	}

#ifdef USE_OLD_IRQ
	result = request_irq(
	    bit3.irq, vmedrv_interrupt, 
	    SA_INTERRUPT | SA_SHIRQ, vmedrv_name, &bit3.dev_id
	);
#else
	result = request_irq(
	    bit3.irq, vmedrv_interrupt, 
	    IRQF_SHARED, vmedrv_name, &bit3.dev_id
	);
#endif
	if (result < 0) {
	    printk(KERN_WARNING "%s: IRQ request fault.\n", vmedrv_name);
	    goto FAIL_IRQ_REQUEST;
	}

	break;

      FAIL_IRQ_REQUEST:
	;
      FAIL_INITIALIZATION:
	if (bit3.window_region_base != 0) {
	    iounmap(bit3.mapped_node_io_base);
	    iounmap(bit3.mapping_registers_base);
	    iounmap(bit3.window_region_base);
	    bit3.window_region_base = 0;
	}
	release_region(bit3.io_node_io_base, bit3_IO_NODE_IO_SIZE);
      FAIL_REGION_REQUEST:
	;

	return result;
    }

    dev_prop->pio_buffer = 0;
    dev_prop->number_of_pio_windows = 0;
    dev_prop->number_of_mmap_windows = 0;
    dev_prop->local_interrupt_client = 0;

    mode = minor_to_access_mode[minor_id];
    method = minor_to_transfer_method[minor_id];
    set_access_mode(dev_prop, mode);
    set_transfer_method(dev_prop, method);
    
    try_module_get(THIS_MODULE);
    module_open_count++;

    return 0;
}


static int vmedrv_release(struct inode* inode, struct file* filep)
{
    struct dev_prop_t* dev_prop;
    dev_prop = filep->private_data;

    module_open_count--;
    if (module_open_count == 0) {
	disable_normal_interrupt();
	disable_error_interrupt();
    }

    /* unregister all registered interrupts */
    unregister_interrupt_notification(dev_prop, current, 0, 0);

    if (dev_prop->number_of_pio_windows > 0) {
        free_windows(dev_prop->pio_window_index, dev_prop->number_of_pio_windows);
    }
    if (dev_prop->number_of_mmap_windows > 0) {
        free_windows(dev_prop->mmap_window_index, dev_prop->number_of_mmap_windows);
    }
	
    if (dev_prop->pio_buffer > 0) {
	kfree(dev_prop->pio_buffer);
    }
    kfree(dev_prop);

    if (module_open_count == 0) {
	if (bit3.window_region_base != 0) {
	    iounmap(bit3.mapped_node_io_base);
	    iounmap(bit3.mapping_registers_base);
	    iounmap(bit3.window_region_base);
	    bit3.window_region_base = 0;
	}
	free_irq(bit3.irq, &bit3.dev_id);
	release_region(bit3.io_node_io_base, bit3_IO_NODE_IO_SIZE);
    }

    module_put(THIS_MODULE);

    return 0;
}


static ssize_t vmedrv_read(struct file* filep, char* buf, size_t count, loff_t* f_pos)
{
    struct dev_prop_t* dev_prop;
    unsigned long vme_address;
    int total_read_size, read_size, remainder_size;

    dev_prop = filep->private_data;
    vme_address = *f_pos;
    read_size = 0;
    total_read_size = 0;
    remainder_size = count;

    if ((count % dev_prop->data_width) != 0) {
	return -EINVAL;
    }

    while (remainder_size > 0) {
	if (
	    (dev_prop->transfer_method == tmDMA) ||
	    (dev_prop->transfer_method == tmNBDMA)
	){
	    read_size = dma_read(dev_prop, buf, vme_address, remainder_size);
	}
	else if (dev_prop->transfer_method == tmPIO) {
	    read_size = pio_read(dev_prop, buf, vme_address, remainder_size);
	}
	else {
	    return -EINVAL;
	}

	if (read_size < 0) {
	    return read_size;
	}
	else if (read_size == 0) {
	    break;
	}

	remainder_size -= read_size;
	total_read_size += read_size;
	vme_address += read_size;
	buf += read_size;
    }

    *f_pos += total_read_size;

    return total_read_size;
}


static ssize_t vmedrv_write(struct file* filep, const char* buf, size_t count, loff_t* f_pos)
{
    struct dev_prop_t* dev_prop;
    unsigned long vme_address;
    int total_written_size, written_size, remainder_size;

    dev_prop = filep->private_data;
    vme_address = *f_pos;
    written_size = 0;
    total_written_size = 0;
    remainder_size = count;

    if ((count % dev_prop->data_width) != 0) {
	return -EINVAL;
    }

    while (remainder_size > 0) {
	if (
	    (dev_prop->transfer_method == tmDMA) ||
	    (dev_prop->transfer_method == tmNBDMA) 
	){
	    written_size = dma_write(dev_prop, buf, vme_address, remainder_size);
	}
	else if (dev_prop->transfer_method == tmPIO) {
	    written_size = pio_write(dev_prop, buf, vme_address, remainder_size);
	}
	else {
	    return -EINVAL;
	}

	if (written_size < 0) {
	    return written_size;
	}
	else if (written_size == 0) {
	    break;
	}

	remainder_size -= written_size;
	total_written_size += written_size;
	vme_address += written_size;
	buf += written_size;
    }

    *f_pos += total_written_size;

    return total_written_size;
}


static loff_t vmedrv_lseek(struct file* filep, loff_t offset, int whence)
{
    switch (whence) {
      case 0: /* SEEK_SET */
	filep->f_pos = offset;
	break;
      case 1: /* SEEK_CUR */
	filep->f_pos += offset;
	break;
      case 2: /* SEEK_END */
	return -EINVAL;
      default:
	return -EINVAL;
    };

    return filep->f_pos;
}


#ifdef USE_OLD_IOCTL
static int vmedrv_ioctl(struct inode* inode, struct file *filep, unsigned int cmd, unsigned long arg)
{
    return vmedrv_unlocked_ioctl(filep, cmd, arg);
}
#endif

static long vmedrv_unlocked_ioctl(struct file *filep, unsigned int cmd, unsigned long arg)
{
    struct dev_prop_t* dev_prop;
    int argument_size;
    int result = -EINVAL;
    int value = 0;
    unsigned long uncopied_length;
    struct vmedrv_interrupt_property_t interrupt_property;
    int irq = 0, vector = 0, signal_id = 0, timeout = 0;
    int vector_mask = 0xffff;

    if (_IOC_TYPE(cmd) != VMEDRV_IOC_MAGIC) {
        return -EINVAL;
    }

    /* read arguments from user area */
    if (
	(cmd == VMEDRV_IOC_SET_ACCESS_MODE) || 
	(cmd == VMEDRV_IOC_SET_TRANSFER_METHOD)
    ){
        if (get_user(value, (int*) arg) < 0) {
	    return -EFAULT;
	}
    }
    else if (
	(cmd == VMEDRV_IOC_REGISTER_INTERRUPT) ||
	(cmd == VMEDRV_IOC_UNREGISTER_INTERRUPT) ||
	(cmd == VMEDRV_IOC_WAIT_FOR_INTERRUPT) ||
	(cmd == VMEDRV_IOC_CHECK_INTERRUPT) ||
	(cmd == VMEDRV_IOC_CLEAR_INTERRUPT) ||
	(cmd == VMEDRV_IOC_SET_INTERRUPT_AUTODISABLE) ||
	(cmd == VMEDRV_IOC_SET_VECTOR_MASK)
    ){
	argument_size = sizeof(struct vmedrv_interrupt_property_t);
	uncopied_length = copy_from_user(
	    &interrupt_property, (const __user void *) arg, argument_size
	);
	if (uncopied_length != 0) {
 	    return -EFAULT;
	}

	irq = interrupt_property.irq;
	vector = interrupt_property.vector;
	signal_id = interrupt_property.signal_id;
	timeout = interrupt_property.timeout;
	vector_mask = interrupt_property.vector_mask;
    }

    dev_prop = filep->private_data;

    switch (cmd) {
      case VMEDRV_IOC_SET_ACCESS_MODE:
	result = set_access_mode(dev_prop, value);
        break;
      case VMEDRV_IOC_SET_TRANSFER_METHOD:
	result = set_transfer_method(dev_prop, value);
        break;
      case VMEDRV_IOC_PROBE:
	result = probe(dev_prop, (__user vmedrv_word_access_t*) arg);
        break;
      case VMEDRV_IOC_REGISTER_INTERRUPT:
	result = register_interrupt_notification(dev_prop, current, irq, vector, signal_id);
        break;
      case VMEDRV_IOC_UNREGISTER_INTERRUPT:
	result = unregister_interrupt_notification(dev_prop, current, irq, vector);
        break;
      case VMEDRV_IOC_WAIT_FOR_INTERRUPT:
	result = wait_for_interrupt_notification(current, irq, vector, timeout);
        break;
      case VMEDRV_IOC_CHECK_INTERRUPT:
	result = check_interrupt_notification(irq, vector);
        break;
      case VMEDRV_IOC_CLEAR_INTERRUPT:
	result = clear_interrupt_notification(irq, vector);
        break;
      case VMEDRV_IOC_SET_INTERRUPT_AUTODISABLE:
	result = set_interrupt_autodisable(irq, vector);
        break;
      case VMEDRV_IOC_ENABLE_INTERRUPT:
	result = enable_normal_interrupt();
        break;
      case VMEDRV_IOC_DISABLE_INTERRUPT:
	result = disable_normal_interrupt();
        break;
      case VMEDRV_IOC_SET_VECTOR_MASK:
	result = set_vector_mask(irq, vector, vector_mask);
        break;
      case VMEDRV_IOC_ENABLE_ERROR_INTERRUPT:
	result = enable_error_interrupt();
        break;
      case VMEDRV_IOC_DISABLE_ERROR_INTERRUPT:
	result = disable_error_interrupt();
        break;
      case VMEDRV_IOC_RESET_ADAPTER:
	result = reset_adapter();
        break;
      default:
	return -EINVAL;
    }

    return result;
}



static int vmedrv_mmap(struct file* filep, struct vm_area_struct* vma)
{
    unsigned long vme_address, size;
    unsigned long physical_address, page_frame_number;
    struct dev_prop_t* dev_prop;

    dev_prop = filep->private_data;
    size = vma->vm_end - vma->vm_start;
    vme_address = vma->vm_pgoff * PAGE_SIZE;

    DEBUG_MAP(printk(KERN_DEBUG "mapping vme memory...\n"));
    DEBUG_MAP(printk(KERN_DEBUG "  vme address: 0x%lx\n", vme_address));
    DEBUG_MAP(printk(KERN_DEBUG "  mapping size: 0x%lx\n", size));

    if (vme_address & (PAGE_SIZE - 1)) {
        /* offset address must be aligned with the MMU page */
        return -ENXIO;
    }
    
    if (dev_prop->number_of_mmap_windows > 0) {
        /* FIXME: mmap() can be called only once every open() */
        return -ENXIO;
    }

    dev_prop->number_of_mmap_windows = ((unsigned long) size - 1) / (unsigned long) bit3_WINDOW_SIZE + 1;
    dev_prop->mmap_window_index = allocate_windows(dev_prop->number_of_mmap_windows);
    if (dev_prop->mmap_window_index < 0) {
        dev_prop->number_of_mmap_windows = 0;
        return dev_prop->mmap_window_index;
    }

    DEBUG_MAP(printk(KERN_DEBUG "  map pages: %d\n", dev_prop->number_of_mmap_windows));
    DEBUG_MAP(printk(KERN_DEBUG "  window index: %d\n", dev_prop->mmap_window_index));

    map_windows(vme_address, size, dev_prop->mmap_window_index, dev_prop->mapping_flags);
    physical_address = (
        bit3.physical_window_region_base + 
	dev_prop->mmap_window_index * bit3_WINDOW_SIZE +
	(vme_address & bit3_PAGE_OFFSET_MASK)
    );
    page_frame_number = physical_address >> PAGE_SHIFT;

    DEBUG_MAP(printk(KERN_DEBUG "  physical address: 0x%lx\n", physical_address));

    if (remap_pfn_range(vma, vma->vm_start, page_frame_number, size, vma->vm_page_prot) < 0) {
        return -EAGAIN;
    }

    DEBUG_MAP(printk(KERN_DEBUG "  mapped address: 0x%lx\n", vma->vm_start));

    return 0;
}


static unsigned int vmedrv_poll(struct file* filep, poll_table* table)
{
    struct dev_prop_t* dev_prop = filep->private_data;
    struct local_interrupt_client_t* local_interrupt_client;
    unsigned int mask = 0;

    poll_wait(filep, &vmedrv_vmebusirq_wait_queue, table);

    for (
	local_interrupt_client = dev_prop->local_interrupt_client;
	local_interrupt_client != 0;
	local_interrupt_client = local_interrupt_client->next
    ){
	if (local_interrupt_client->interrupt_client->interrupt_count > 0) {
	    mask |= POLLIN | POLLRDNORM;
	}
    }

    return mask;
}


#ifdef USE_OLD_IRQ
static irqreturn_t vmedrv_interrupt(int irq, void* dev_id, struct pt_regs* regs)
#else
static irqreturn_t vmedrv_interrupt(int irq, void* dev_id)
#endif
{
    unsigned status;

    DEBUG_INT(printk(KERN_DEBUG "interrupt handled.\n"));

    if (module_open_count <= 0) {
	/* note that access to the VME card unavailable */
	return IRQ_NONE;
    }

    /* check whether the PCI card is generating an interrupt */
    status = ioread8(bit3.mapped_node_io_base + regINTERRUPT_CONTROL);
    if (! (status & icINTERRUPT_ACTIVE)) {
        DEBUG_INT(printk(KERN_DEBUG "the card is not generating an interrupt.\n"));
	return IRQ_NONE;
    }

    /* checek for a error interrupt */
    status = ioread8(bit3.mapped_node_io_base + regLOCAL_STATUS);
    if (status & (lsERROR_BITS & ~lsRECEIVING_PR_INTERRUPT)) {
	acknowledge_error_interrupt(status);
	return IRQ_HANDLED;
    }
    
    /* check for a PR interrupt */
    if (status & lsRECEIVING_PR_INTERRUPT) {
	acknowledge_pr_interrupt(status);
	return IRQ_HANDLED;
    }

    /* check for a DMA DONE interrupt */
    if (bit3.is_dma_available) {
	status = ioread8(bit3.mapped_node_io_base + regDMA_COMMAND);
	if ((status & dcDMA_DONE_FLAG) && (status & dcENABLE_DMA_DONE_INTERRUPT)) {
	    acknowledge_dma_interrupt(status);
	    return IRQ_HANDLED;
	}
    }

    /* check for a PT interrupt */
    status = ioread8(bit3.mapped_node_io_base + regREMOTE_STATUS);
    if (status & rsRECEIVING_PT_INTERRUPT) {
	acknowledge_pt_interrupt(status);
	return IRQ_HANDLED;
    }

    /* check for a VME backplane interrupt */    
    status = ioread8(bit3.mapped_node_io_base + regINTERRUPT_STATUS);
    if (status) {
	acknowledge_vme_interrupt(status);
	return IRQ_HANDLED;
    }

    printk(KERN_WARNING "%s: Unknown interrupt handled...\n", vmedrv_name);
    return IRQ_HANDLED;
}


static int vmedrv_pci_probe(struct pci_dev* pci_dev, const struct pci_device_id* id)
{
    int result;

    printk(
	KERN_INFO "%s: %s is detected at ioport 0x%04lx on irq %d.\n", 
	vmedrv_name, 
	vmedrv_model_name_table[id->driver_data],
	(unsigned long) pci_dev->resource[bit3_IO_NODE_IO_BASE_INDEX].start,
	pci_dev->irq
    );
    printk(
	KERN_INFO "  I/O Mapped Node at 0x%04lx.\n", 
        (unsigned long) pci_dev->resource[bit3_IO_NODE_IO_BASE_INDEX].start
    );
    printk(
	KERN_INFO "  Memory Mapped Node at 0x%04lx.\n", 
        (unsigned long) pci_dev->resource[bit3_MAPPED_NODE_IO_BASE_INDEX].start
    );
    printk(
	KERN_INFO "  Mapping Register at 0x%04lx.\n", 
        (unsigned long) pci_dev->resource[bit3_MAPPING_REGISTERS_BASE_INDEX].start
    );
    printk(
	KERN_INFO "  Remote Memory at 0x%04lx.\n", 
        (unsigned long) pci_dev->resource[bit3_WINDOW_REGION_BASE_INDEX].start
    );

    if (bit3_card_count != target_card_number) {
	bit3_card_count++;
	printk(KERN_INFO "%s: this is not our target card...\n", vmedrv_name);
	return -ENODEV;
    }
    bit3_card_count++;

    bit3.pci_dev = pci_dev;
    bit3.is_dma_available = (id->device == di616) ? 0 : 1;

    result = pci_enable_device(pci_dev);
    if (result < 0) {
	printk(KERN_WARNING "%s: enabling device failed.\n", vmedrv_name);
    }

    /* force the DMA buffer be allocated in 32bit accessible range */
#ifdef USE_OLD_DMABITMASK
    pci_set_dma_mask(pci_dev, DMA_32BIT_MASK);
#else
    pci_set_dma_mask(pci_dev, DMA_BIT_MASK(32));
#endif


    return result;
}


static void vmedrv_pci_remove(struct pci_dev* pci_dev)
{
    if (pci_dev == bit3.pci_dev) {
	printk(KERN_WARNING "%s: device removed.\n", vmedrv_name);
    }
}


static int initialize(void)
{
    unsigned status;

    /* read PCI configurations */
    bit3.mapped_node_io_base = ioremap_nocache(
        bit3.pci_dev->resource[bit3_MAPPED_NODE_IO_BASE_INDEX].start,
        bit3_MAPPED_NODE_IO_SIZE
    );
    bit3.mapping_registers_base = ioremap_nocache(
        bit3.pci_dev->resource[bit3_MAPPING_REGISTERS_BASE_INDEX].start,
        bit3_MAPPING_REGISTERS_SIZE
    );
    bit3.window_region_base = ioremap_nocache(
        bit3.pci_dev->resource[bit3_WINDOW_REGION_BASE_INDEX].start,
        bit3_WINDOW_REGION_SIZE
    );

    bit3.physical_window_region_base = (
	bit3.pci_dev->resource[bit3_WINDOW_REGION_BASE_INDEX].start
    );
    bit3.dma_mapping_registers_base = (
	bit3.mapping_registers_base + bit3_DMA_MAPPING_REGISTERS_BASE_OFFSET
    );

    DEBUG_CONFIG(printk(KERN_DEBUG "remapping pci memories...\n"));
    DEBUG_CONFIG(printk(KERN_DEBUG "  memory mapped node: --> 0x%08lx\n", bit3.mapped_node_io_base));
    DEBUG_CONFIG(printk(KERN_DEBUG "  mapping regs: --> 0x%08lx\n", bit3.mapping_registers_base));
    DEBUG_CONFIG(printk(KERN_DEBUG "  window base: --> 0x%08lx\n", bit3.window_region_base));
    
    /* check whether remote power is on */
#if 0
    status = inb(bit3.io_node_io_base + regLOCAL_STATUS);
#else
    status = ioread8(bit3.mapped_node_io_base + regLOCAL_STATUS);
#endif
    if (status & lsREMOTE_BUS_POWER_OFF) {
        printk(KERN_WARNING "%s: ERROR: VME chassis power is off.\n", vmedrv_name);
        printk(KERN_WARNING "  (or I/O cable is not connected, or SYSRESET is active.)\n");
	printk(KERN_WARNING "  Local Status Register: 0x%02x\n", status);
        return -EIO;
    }

    /* clear error caused by the power on transition */
#if 0
    status = inb(bit3.io_node_io_base  + regREMOTE_STATUS);
    rmb();
    outb(bit3.io_node_io_base  + regLOCAL_COMMAND, lcCLEAR_STATUS);
    wmb();
    status = inb(bit3.io_node_io_base  + regLOCAL_STATUS);
#else
    status = ioread8(bit3.mapped_node_io_base + regREMOTE_STATUS);
    rmb();
    iowrite8(lcCLEAR_STATUS, bit3.mapped_node_io_base + regLOCAL_COMMAND);
    wmb();
    status = ioread8(bit3.mapped_node_io_base + regLOCAL_STATUS);
#endif
    
    /* make sure no error bits are set */
    if (status & lsERROR_BITS) {
        if (status & lsINTERFACE_PARITY_ERROR) {
	    printk(KERN_WARNING "%s: ERROR: interface parity error.\n", vmedrv_name);
	}
        if (status & lsREMOTE_BUS_ERROR) {
	    printk(KERN_WARNING "%s: ERROR: remote bus error.\n", vmedrv_name);
	}
        if (status & lsRECEIVING_PR_INTERRUPT) {
	    printk(KERN_WARNING "%s: ERROR: receiving PR interrupt.\n", vmedrv_name);
	}
        if (status & lsINTERFACE_TIMEOUT) {
	    printk(KERN_WARNING "%s: ERROR: interface timed out.\n", vmedrv_name);
	}
        if (status & lsLRC_ERROR) {
	    printk(KERN_WARNING "%s: ERROR: LRC(Longitudinal Redundancy Check) error.\n", vmedrv_name);
	}
        if (status & lsREMOTE_BUS_POWER_OFF) {
	    printk(KERN_WARNING "%s: ERROR: remote bus power off or I/O cable is off.\n", vmedrv_name);
	}

	printk(KERN_WARNING "  Local Status Register: 0x%02x\n", status);

	status = inb(bit3.io_node_io_base  + regADAPTER_ID);
	printk(KERN_WARNING "  Adapter ID (I/O): 0x%02x\n", status);
	status = ioread8(bit3.mapped_node_io_base + regADAPTER_ID);
	printk(KERN_WARNING "  Adapter ID (mem): 0x%02x\n", status);

        return -EIO;
    }
    
    DEBUG_CONFIG(printk("%s: remote VME card successfully initialized\n", vmedrv_name));

    return 0;
}


static int set_access_mode(struct dev_prop_t* dev_prop, int mode)
{
    if (mode >= vmedrv_NUMBER_OF_ACCESS_MODES) {
	return -EINVAL;
    }

    dev_prop->address_modifier = vmedrv_config_table[mode].address_modifier;
    dev_prop->dma_address_modifier = vmedrv_config_table[mode].dma_address_modifier;
    dev_prop->data_width = vmedrv_config_table[mode].data_width;
    dev_prop->function_code = vmedrv_config_table[mode].function_code;
    dev_prop->byte_swapping = vmedrv_config_table[mode].byte_swapping;

    dev_prop->mapping_flags = (
        (dev_prop->address_modifier << bit3_AM_SHIFT) | 
        (dev_prop->function_code << bit3_FUNCTION_SHIFT) | 
        (dev_prop->byte_swapping << bit3_BYTESWAP_SHIFT) 
    );

    dev_prop->dma_mapping_flags = (
	(dev_prop->byte_swapping & bit3_DMA_BYTESWAP_MASK) << bit3_DMA_BYTESWAP_SHIFT
    );

    /*DEBUG_CONFIG(printk(KERN_DEBUG "setting access modes...\n"));*/
    /*DEBUG_CONFIG(printk(KERN_DEBUG "  address modifier: 0x%02x\n", dev_prop->address_modifier));*/
    /*DEBUG_CONFIG(printk(KERN_DEBUG "  data width: %d\n", dev_prop->data_width));*/
    /*DEBUG_CONFIG(printk(KERN_DEBUG "  function code: 0x%02x\n", dev_prop->function_code));*/
    /*DEBUG_CONFIG(printk(KERN_DEBUG "  byte swapping: 0x%02x\n", dev_prop->byte_swapping));*/

    return 0;
}


static int set_transfer_method(struct dev_prop_t* dev_prop, int method)
{
    if (method == VMEDRV_PIO) {
	dev_prop->transfer_method = tmPIO;
        DEBUG_CONFIG(printk(KERN_DEBUG "transfer mode is set to PIO.\n"));
    }
    else if (method == VMEDRV_DMA) {
	if (dev_prop->dma_address_modifier == amINVALID) {
	    return -EINVAL;
	}
	if (bit3.is_dma_available) {
	    dev_prop->transfer_method = tmDMA;
	    DEBUG_CONFIG(printk(KERN_DEBUG "transfer mode is set to DMA.\n"));
	}
	else {
	    dev_prop->transfer_method = tmPIO;
	    DEBUG_CONFIG(printk(KERN_DEBUG "DMA transfer mode is requested, but not available.\n"));
	    DEBUG_CONFIG(printk(KERN_DEBUG "transfer mode is set to PIO.\n"));
	}
    }
    else if (method == VMEDRV_NBDMA) {
	if (dev_prop->address_modifier == amINVALID) {
	    return -EINVAL;
	}
	dev_prop->transfer_method = tmNBDMA;
	DEBUG_CONFIG(printk(KERN_DEBUG "transfer mode is set to Non-Blocking DMA.\n"));
    }
    else {
	return -EINVAL;
    }

    return 0;
}


static int probe(struct dev_prop_t* dev_prop, __user vmedrv_word_access_t* word_access)
{
    int result;
    unsigned long address = 0, data = 0;

    if (get_user(address, &word_access->address) < 0) {
        return -EFAULT;
    }
    if (put_user(data, &word_access->data) < 0) {
        return -EFAULT;
    }

    result = pio_read(dev_prop, (__user char*) &word_access->data, address, 1);
    if (result >= 0) {
	result = 0;
    }

    return result;
}


static int pio_read(struct dev_prop_t* dev_prop, char* buf, unsigned long vme_address, int count)
{
    unsigned long offset_address;
    unsigned long size, read_size;
    unsigned long uncopied_length;
    void* window_address;
    void* pio_buf;
    unsigned long pio_buf_data_size, pio_buf_index;
    unsigned status;
    int result;

    /* allocate PIO buffer and mapping windows, if it has not been yet */
    if ((result = prepare_pio(dev_prop)) < 0) {
	return result;
    }

    /* map windows */
    offset_address = vme_address & bit3_PAGE_OFFSET_MASK;
    if (offset_address + count <= bit3_WINDOW_SIZE * dev_prop->number_of_pio_windows) {
        size = count;
    }
    else {
        size = bit3_WINDOW_SIZE * dev_prop->number_of_pio_windows - offset_address;
    }
    window_address = map_windows(vme_address, size, dev_prop->pio_window_index, dev_prop->mapping_flags);
    
    /* read from mapped windows */
    pio_buf = dev_prop->pio_buffer;
    pio_buf_data_size = 0;
    pio_buf_index = 0;
    for (read_size = 0; read_size < size; read_size += dev_prop->data_width) {
        if (dev_prop->data_width == dwWORD) {
	    ((unsigned short *) pio_buf)[pio_buf_index] = ioread16(window_address);
	}
	else if (dev_prop->data_width == dwLONG) {
	    ((unsigned int *) pio_buf)[pio_buf_index] = ioread32(window_address);
	}

	window_address += dev_prop->data_width;
	pio_buf_data_size += dev_prop->data_width;
	pio_buf_index++;

	if (pio_buf_data_size + dwLONG >= PIO_BUFFER_SIZE) {
	    uncopied_length = copy_to_user(buf, pio_buf, pio_buf_data_size);
	    if (uncopied_length > 0) {
		return -EFAULT;
	    }
	    buf += pio_buf_data_size;
	    pio_buf_data_size = 0;
	    pio_buf_index = 0;
	}
    }

    uncopied_length = copy_to_user(buf, pio_buf, pio_buf_data_size);
    if (uncopied_length > 0) {
	return -EFAULT;
    }

    /* error check */
    status = ioread8(bit3.mapped_node_io_base + regLOCAL_STATUS);
    rmb();
    iowrite8(lcCLEAR_STATUS, bit3.mapped_node_io_base + regLOCAL_COMMAND);
    if (status & lsINTERFACE_TIMEOUT) {
        return -ETIMEDOUT;
    }
    if (status & lsERROR_BITS) {
	printk(KERN_WARNING "%s: VME I/O failed.\n", vmedrv_name);
	printk(KERN_WARNING "  Local Status Register: 0x%02x\n", status);
        return -EIO;
    }

    return read_size;
}


static int pio_write(struct dev_prop_t* dev_prop, const char* buf, unsigned long vme_address, int count)
{
    unsigned long offset_address;
    unsigned long size, remain_size;
    unsigned long uncopied_length;
    void* window_address;
    void* pio_buf;
    unsigned long pio_buf_data_size, pio_buf_index;
    unsigned status;
    int result;

    /* allocate PIO buffer and mapping windows, if it has not been yet */
    if ((result = prepare_pio(dev_prop)) < 0) {
	return result;
    }

    /* map windows */
    offset_address = vme_address & bit3_PAGE_OFFSET_MASK;
    if (offset_address + count <= bit3_WINDOW_SIZE * dev_prop->number_of_pio_windows) {
        size = count;
    }
    else {
        size = bit3_WINDOW_SIZE * dev_prop->number_of_pio_windows - offset_address;
    }
    window_address = map_windows(vme_address, size, dev_prop->pio_window_index, dev_prop->mapping_flags);
    
    /* write to mapped windows */
    pio_buf = dev_prop->pio_buffer;
    pio_buf_data_size = 0;
    pio_buf_index = 0;
    for (remain_size = size; remain_size > 0; remain_size -= dev_prop->data_width) {
	if (pio_buf_data_size <= 0) {
	    if (remain_size < PIO_BUFFER_SIZE) {
		pio_buf_data_size = remain_size;
	    }
	    else {
		pio_buf_data_size = PIO_BUFFER_SIZE;
	    }
	    uncopied_length = copy_from_user(pio_buf, buf, pio_buf_data_size);
	    if (uncopied_length > 0) {
		return -EFAULT;
	    }
	    buf += pio_buf_data_size;
	    pio_buf_index = 0;
	}

        if (dev_prop->data_width == dwWORD) {
	    iowrite16(
		((unsigned short *) pio_buf)[pio_buf_index], window_address
	    );
	}
	else if (dev_prop->data_width == dwLONG) {
	    iowrite32(
		((unsigned int *) pio_buf)[pio_buf_index], window_address
	    );
	}

	window_address += dev_prop->data_width;
	pio_buf_data_size -= dev_prop->data_width;
	pio_buf_index++;
    }

    /* error check */
    status = ioread8(bit3.mapped_node_io_base + regLOCAL_STATUS);
    rmb();
    iowrite8(lcCLEAR_STATUS, bit3.mapped_node_io_base + regLOCAL_COMMAND);
    if (status & lsINTERFACE_TIMEOUT) {
        return -ETIMEDOUT;
    }
    if (status & lsERROR_BITS) {
	printk(KERN_WARNING "%s: VME I/O failed.\n", vmedrv_name);
	printk(KERN_WARNING "  Local Status Register: 0x%02x\n", status);
        return -EIO;
    }

    return size;
}


static int prepare_pio(struct dev_prop_t* dev_prop)
{
    /* allocate PIO buffer, if it has not been allocated. */
    DEBUG_PIO(printk(KERN_DEBUG "allocating PIO buffer\n"));
    DEBUG_PIO(printk(KERN_DEBUG "  size=%d\n", PIO_BUFFER_SIZE));
    if (dev_prop->pio_buffer == 0) {
	dev_prop->pio_buffer = kmalloc(PIO_BUFFER_SIZE, GFP_KERNEL);
	if (dev_prop->pio_buffer == 0) {
	    printk(KERN_WARNING "%s: unable to allocate PIO buffer.", vmedrv_name);
	    printk(KERN_WARNING "  requested size: %d\n", PIO_BUFFER_SIZE);
	    return -ENOMEM;
	}
    }    
    DEBUG_PIO(printk(KERN_DEBUG "  ok, address=%0x\n", dev_prop->pio_buffer));

    /* allocate PIO windows, if it has not been allocated. */
    if (dev_prop->number_of_pio_windows == 0) {
        DEBUG_PIO(printk(KERN_DEBUG "allocating PIO windows\n"));
        DEBUG_PIO(printk(KERN_DEBUG "  size=%d\n", PIO_WINDOW_PAGES));
        dev_prop->pio_window_index = allocate_windows(PIO_WINDOW_PAGES);
	if (dev_prop->pio_window_index < 0) {
	    return dev_prop->pio_window_index;
	}
        DEBUG_PIO(printk(KERN_DEBUG "  ok, start_index=%d\n", dev_prop->pio_window_index));
	dev_prop->number_of_pio_windows = PIO_WINDOW_PAGES;
    }

    return 0;
}

static int allocate_windows(int number_of_windows)
{
    int number_of_free_windows;
    int window_index = -ENOMEM;
    int i;

    number_of_free_windows = 0;
    for (i = 0; i < bit3_NUMBER_OF_WINDOWS; i++) {
        if (bit3.window_status_table[i] != 0) {
	    number_of_free_windows = 0;
	    continue;
	}

	if (number_of_free_windows == 0) {
	    window_index = i;
	}

	number_of_free_windows++;
	if (number_of_free_windows == number_of_windows) {
	    break;
	}
    }

    if (i == bit3_NUMBER_OF_WINDOWS) {
        return -ENOMEM;
    }
    
    for (i = 0; i < number_of_windows; i++) {
        bit3.window_status_table[window_index + i] = 1;
    }

    return window_index;
}


static void free_windows(int window_index, int number_of_windows)
{
    int i;
    for (i = 0; i < number_of_windows; i++) {
        bit3.window_status_table[window_index + i] = 0;
    }
}


static void* map_windows(unsigned vme_address, unsigned size, unsigned window_index, unsigned flags) 
{
    unsigned base_address, offset_address;
    unsigned number_of_windows;
    void* window_address;
    int i;

    base_address = vme_address & bit3_PAGE_BASE_MASK;
    offset_address = vme_address & bit3_PAGE_OFFSET_MASK;
    number_of_windows = ((unsigned long) size - 1) / (unsigned long) bit3_WINDOW_SIZE + 1;
    window_address = bit3.window_region_base + bit3_WINDOW_SIZE * window_index + offset_address;

    for (i = 0; i < number_of_windows; i++) {
        iowrite32(
	    base_address | flags, 
            bit3.mapping_registers_base + bit3_MAPPING_REGISTER_WIDTH * window_index
	);
        base_address += bit3_WINDOW_SIZE;
	window_index++;
    }

    return window_address;
}


static int enable_normal_interrupt(void)
{
    bit3.interrupt_enabling_flags |= icNORMAL_INTERRUPT_ENABLE;
    iowrite8(
	bit3.interrupt_enabling_flags,
        bit3.mapped_node_io_base + regINTERRUPT_CONTROL
    );

    DEBUG_INT(printk(KERN_DEBUG "normal interrupt enabled.\n"));

    return 0;
}


static int disable_normal_interrupt(void)
{
    bit3.interrupt_enabling_flags &= ~icNORMAL_INTERRUPT_ENABLE;
    iowrite8(
	bit3.interrupt_enabling_flags,
        bit3.mapped_node_io_base + regINTERRUPT_CONTROL
    );

    DEBUG_INT(printk(KERN_DEBUG "normal interrupt disabled.\n"));

    return 0;
}


static int enable_error_interrupt(void)
{
    bit3.interrupt_enabling_flags |= icERROR_INTERRUPT_ENABLE;
    iowrite8(
	bit3.interrupt_enabling_flags,
        bit3.mapped_node_io_base + regINTERRUPT_CONTROL
    );

    return 0;
}


static int disable_error_interrupt(void)
{
    bit3.interrupt_enabling_flags &= ~icERROR_INTERRUPT_ENABLE;
    iowrite8(
	bit3.interrupt_enabling_flags,
        bit3.mapped_node_io_base + regINTERRUPT_CONTROL
    );

    return 0;
}


static void save_interrupt_flags(void)
{
    bit3.saved_interrupt_flags = bit3.interrupt_enabling_flags;
    DEBUG_INT(printk(
	KERN_DEBUG "interrupt flags are saved: 0x%02x\n", 
	bit3.interrupt_enabling_flags
    ));
}


static void restore_interrupt_flags(void)
{
    if (bit3.saved_interrupt_flags != bit3.interrupt_enabling_flags) {
	bit3.interrupt_enabling_flags = bit3.saved_interrupt_flags;
	iowrite8(
	    bit3.interrupt_enabling_flags,
	    bit3.mapped_node_io_base + regINTERRUPT_CONTROL
	);	
    }

    DEBUG_INT(printk(
	KERN_DEBUG "interrupt flags are restored: 0x%02x\n", 
	bit3.interrupt_enabling_flags
    ));
}


static int acknowledge_error_interrupt(unsigned local_status)
{
    error_interrupt_count++;
    local_status_on_error = local_status;

    printk(KERN_WARNING "%s: error interrupt handled.\n", vmedrv_name);
    printk(KERN_WARNING "  Local Status Register: 0x%02x\n", local_status);
    if (local_status & lsINTERFACE_PARITY_ERROR) {
        printk(KERN_WARNING "  Fiber-Optic Interface Data Error\n");
    }
    if (local_status & lsREMOTE_BUS_ERROR) {
        printk(KERN_WARNING "  Remote Bus Error\n");
    }
    if (local_status & lsINTERFACE_TIMEOUT) {
        printk(KERN_WARNING "  Interface Timeout\n");
    }
    if (local_status & lsREMOTE_BUS_POWER_OFF) {
        printk(KERN_WARNING "  Remote Bus Power Off or I/O Cable Is Off\n");
    }

    iowrite8(lcCLEAR_STATUS, bit3.mapped_node_io_base + regLOCAL_COMMAND);
    if (local_status & lsINTERFACE_TIMEOUT) {
	/* flush the interface error */
	ioread8(bit3.mapped_node_io_base + regREMOTE_STATUS);
    }

    return 0;
}


static int acknowledge_pt_interrupt(unsigned local_status)
{
    DEBUG_INT(printk(KERN_DEBUG "pt interrupt handled...\n"));

    iowrite8(lcCLEAR_PR_INTERRUPT, bit3.mapped_node_io_base + regLOCAL_COMMAND);

    return 0;
}


static int acknowledge_dma_interrupt(unsigned dma_status)
{
    DEBUG_INT(printk(KERN_DEBUG "DEBUG_INT: dma interrupt handled...\n"));
    DEBUG_DMA(printk(KERN_DEBUG "DEBUG_DMA: dma interrupt handled...\n"));

    /* clear the DMA Command Register */
    iowrite8(0, bit3.mapped_node_io_base + regDMA_COMMAND);  

    /* wake up the process */
    is_dma_running = 0;
    wake_up(&vmedrv_dmadone_wait_queue);

    return 0;
}


static int acknowledge_pr_interrupt(unsigned remote_status)
{
    DEBUG_INT(printk(KERN_DEBUG "pr interrupt handled...\n"));

    iowrite8(
        rcCLEAR_PT_INTERRUPT,
	bit3.mapped_node_io_base + regREMOTE_COMMAND_1
    );
    
    return 0;
}


static int acknowledge_vme_interrupt(unsigned interrupt_status)
{
    unsigned irq, vector;
    struct interrupt_client_t* interrupt_client;
    struct task_struct* task;
    int signal_id;
    int priv;

    DEBUG_INT(printk(KERN_DEBUG "vme interrupt handled...\n"));

    for (irq = 1; irq < vmeNUMBER_OF_IRQ_LINES; irq++) {
        /* check whether this IRQ is asserted */
        if (! (interrupt_status & (0x0001 << irq))) {
	    continue;
	}
	DEBUG_INT(printk(KERN_DEBUG "  irq = %d\n", irq));

	/* acknowledge IRQ request (send IACK) */
	iowrite8(irq, bit3.mapped_node_io_base + regREMOTE_COMMAND_1);
	wmb();
	vector = ioread16(bit3.mapped_node_io_base + regIACK_READ);
	DEBUG_INT(printk(KERN_DEBUG "  vector = 0x%04x\n", vector));

	/* send signal to registered processes */
	interrupt_client = bit3.interrupt_client_list[irq];
	while (interrupt_client != 0) {
	    if ((vector & interrupt_client->vector_mask) == (interrupt_client->vector & interrupt_client->vector_mask)) {
		if (interrupt_client->autodisable_flag != 0) {
		    disable_normal_interrupt();
                    DEBUG_INT(printk(KERN_DEBUG "  auto-disabled.\n"));
		}

	        task = interrupt_client->task;
		signal_id = interrupt_client->signal_id;
		if (signal_id > 0) {
		    send_sig(signal_id, task, priv = 1);
                    DEBUG_INT(printk(KERN_DEBUG "  send signal.\n"));
		}
		else {
		    interrupt_client->interrupt_count++;
		    wake_up_interruptible(&vmedrv_vmebusirq_wait_queue);
                    DEBUG_INT(printk(KERN_DEBUG "  wake up.\n"));
		}
	    }
	    interrupt_client = interrupt_client->next;
	}
    }
    
    DEBUG_INT(printk(KERN_DEBUG "now exit vme interrupt handling routine.\n"));

    return 0;
}


static int register_interrupt_notification(struct dev_prop_t* dev_prop, struct task_struct* task, int irq, int vector, int signal_id)
{
    struct interrupt_client_t* interrupt_client;
    struct local_interrupt_client_t* local_interrupt_client;

    if ((irq < 1) || (irq >= vmeNUMBER_OF_IRQ_LINES)) {
        return -EINVAL;
    }

    interrupt_client = kmalloc(sizeof(struct interrupt_client_t), GFP_KERNEL);
    local_interrupt_client = kmalloc(sizeof(struct local_interrupt_client_t), GFP_KERNEL);
    if ((interrupt_client == 0) || (local_interrupt_client == 0)) {
        printk(KERN_WARNING "%s: unable to allocate memory for interrupt client entry.\n", vmedrv_name);
        return -ENOMEM;
    }

    interrupt_client->task = task;
    interrupt_client->irq = irq;
    interrupt_client->vector = vector;
    interrupt_client->signal_id = signal_id;
    interrupt_client->interrupt_count = 0;
    interrupt_client->autodisable_flag = 0;
    interrupt_client->vector_mask = 0xffff;

    interrupt_client->next = bit3.interrupt_client_list[irq];
    bit3.interrupt_client_list[irq] = interrupt_client;

    local_interrupt_client->interrupt_client = interrupt_client;
    local_interrupt_client->next = dev_prop->local_interrupt_client;
    dev_prop->local_interrupt_client = local_interrupt_client;

    DEBUG_INT(printk(KERN_DEBUG 
        "vme interrupt is registered, "
	"irq: %d, vector: 0x%04x, pid: %d, signal: %d.\n",
	irq, vector, task->pid, signal_id
    ));

    return 0;
}


static int unregister_interrupt_notification(struct dev_prop_t* dev_prop, struct task_struct* task, int irq, int vector)
{
    struct interrupt_client_t *interrupt_client, *prev_interrupt_client;
    struct interrupt_client_t *unregistered_interrupt_client;
    struct local_interrupt_client_t *local_interrupt_client, *prev_local_interrupt_client;
    struct local_interrupt_client_t *unregistered_local_interrupt_client;
    int process_id;
    int irq_index;

    process_id = task->pid;
    for (irq_index = 1; irq_index < vmeNUMBER_OF_IRQ_LINES; irq_index++) {

        interrupt_client = bit3.interrupt_client_list[irq_index];
	prev_interrupt_client = 0;
	while (interrupt_client != 0) {
	    if (
		(process_id != interrupt_client->task->pid) ||
		((irq != 0) && (irq != interrupt_client->irq)) ||
		((vector != 0) && (vector != interrupt_client->vector))
	    ){
  	        prev_interrupt_client = interrupt_client;
		interrupt_client = interrupt_client->next;
		continue;
	    }

	    DEBUG_INT(printk(KERN_DEBUG 
	        "vme interrupt is unregistered, "
		"irq: %d, vector: 0x%04x, pid: %d.\n",
		interrupt_client->irq, interrupt_client->vector, process_id
	    ));

	    unregistered_interrupt_client = interrupt_client;
	    interrupt_client = interrupt_client->next;
	    if (prev_interrupt_client == 0) {
		bit3.interrupt_client_list[irq_index] = interrupt_client;
	    }
	    else {
		prev_interrupt_client->next = interrupt_client;
	    }

	    local_interrupt_client = dev_prop->local_interrupt_client;
	    prev_local_interrupt_client = 0;
	    while (local_interrupt_client != 0) {
		if (local_interrupt_client->interrupt_client != unregistered_interrupt_client) {
		    prev_local_interrupt_client = local_interrupt_client;
		    local_interrupt_client = local_interrupt_client->next;
		    continue;
		}
		    
		unregistered_local_interrupt_client = local_interrupt_client;
		local_interrupt_client = local_interrupt_client->next;
		if (prev_local_interrupt_client == 0) {
		    dev_prop->local_interrupt_client = local_interrupt_client;
		}
		else {
		    prev_local_interrupt_client->next = local_interrupt_client;
		}
		kfree(unregistered_local_interrupt_client);
	    }
	    
	    kfree(unregistered_interrupt_client);
	}
    }

    return 0;
}


static int wait_for_interrupt_notification(struct task_struct* task, int irq, int vector, int timeout)
{
    long remaining_time;
    int process_id;
    struct interrupt_client_t *interrupt_client;
    
    timeout *= HZ;
    
    /* find matching registered interrupt signature */
    process_id = task->pid;
    interrupt_client = bit3.interrupt_client_list[irq];
    while (interrupt_client != 0) {
	if (
	    (process_id == interrupt_client->task->pid) &&
	    (vector == interrupt_client->vector) &&
	    (interrupt_client->signal_id <= 0)
	){
	    break;
	}
	interrupt_client = interrupt_client->next;
    }
    if (interrupt_client == 0)
    {
	printk(KERN_WARNING "%s: no interrupt is registered to wait for\n", vmedrv_name);
	return -ENODEV;
    }
    
    /* now process or wait for interrupt */
    remaining_time = wait_event_interruptible_timeout(
        vmedrv_vmebusirq_wait_queue, 
	interrupt_client->interrupt_count > 0,
	timeout
    );
    if (remaining_time == 0) {
        return -ETIMEDOUT;
    }
    if (interrupt_client->interrupt_count == 0) {
        return -ERESTARTSYS;
    }
    
    interrupt_client->interrupt_count = 0;
    
    return 1;
}

  
static int check_interrupt_notification(int irq, int vector)
{
    int result = 0;
    struct interrupt_client_t *interrupt_client;

    interrupt_client = bit3.interrupt_client_list[irq];
    while (interrupt_client != 0) {
	if (vector == interrupt_client->vector) {
	    result += interrupt_client->interrupt_count;

	}
	interrupt_client = interrupt_client->next;
    }

    return result;
}

  
static int clear_interrupt_notification(int irq, int vector)
{
    struct interrupt_client_t *interrupt_client;

    interrupt_client = bit3.interrupt_client_list[irq];
    while (interrupt_client != 0) {
	if (vector == interrupt_client->vector) {
	    interrupt_client->interrupt_count = 0;
	}
	interrupt_client = interrupt_client->next;
    }

    return 0;
}
  

static int set_interrupt_autodisable(int irq, int vector)
{
    struct interrupt_client_t *interrupt_client;

    interrupt_client = bit3.interrupt_client_list[irq];
    while (interrupt_client != 0) {
	if (vector == interrupt_client->vector) {
	    interrupt_client->autodisable_flag = 1;
	}
	interrupt_client = interrupt_client->next;
    }

    return 0;
}
  

static int set_vector_mask(int irq, int vector, int vector_mask)
{
    struct interrupt_client_t *interrupt_client;

    interrupt_client = bit3.interrupt_client_list[irq];
    while (interrupt_client != 0) {
      if (vector == interrupt_client->vector) {
          interrupt_client->vector_mask = vector_mask;
      }
      interrupt_client = interrupt_client->next;
    }

    return 0;
}
  
static int reset_adapter(void)
{
    iowrite8(
	rcRESET_ADAPTER, 
	bit3.mapped_node_io_base + regREMOTE_COMMAND_1
    );

    return 0;
}


static int dma_read(struct dev_prop_t* dev_prop, char* buf, unsigned long vme_address, int count)
{
    unsigned long pci_address, size;
    unsigned long uncopied_length;
    int direction;
    int result;

    /* allocate DMA buffer, map them (only first time) */
    if ((result = prepare_dma(dev_prop)) < 0) {
	return result;
    }

    /* set transfer size and direction */
    if (count > bit3.dma_buffer_size) {
	size = bit3.dma_buffer_size;
    }
    else {
	size = count;
    }
    direction = tdREAD;

    pci_address = bit3.dma_buffer_mapped_pci_address;

    result = start_dma(dev_prop, pci_address, vme_address, size, direction);
    if (result < 0) {
	return result;
    }

    dma_cache_sync(&bit3.pci_dev->dev, bit3.dma_buffer, size, DMA_FROM_DEVICE);

    uncopied_length = copy_to_user(buf, bit3.dma_buffer, size);
    if (uncopied_length > 0) {
	return -EFAULT;
    }
    
    return size;
}


static int dma_write(struct dev_prop_t* dev_prop, const char* buf, unsigned long vme_address, int count)
{
    unsigned long pci_address, size;
    unsigned long uncopied_length;
    int direction;
    int result;

    /* allocate DMA buffer, map them (only first time) */
    if ((result = prepare_dma(dev_prop)) < 0) {
	return result;
    }

    /* set transfer size and direction */
    if (count > bit3.dma_buffer_size) {
	size = bit3.dma_buffer_size;
    }
    else {
	size = count;
    }
    direction = tdWRITE;

    uncopied_length = copy_from_user(bit3.dma_buffer, buf, size);
    if (uncopied_length > 0) {
	return -EFAULT;
    }

    dma_cache_sync(&bit3.pci_dev->dev, bit3.dma_buffer, size, DMA_TO_DEVICE);

    pci_address = bit3.dma_buffer_mapped_pci_address;

    result = start_dma(dev_prop, pci_address, vme_address, size, direction);

    return (result < 0) ? result : size;
}


static int prepare_dma(struct dev_prop_t* dev_prop)
{
    /* allocate and map DMA buffer, if it has not been allocated yet.*/
    if (bit3.dma_buffer_size == 0) {
	if (allocate_dma_buffer(DMA_BUFFER_SIZE) <= 0) {
	    printk(KERN_WARNING "%s: unable to allocate dma buffer.\n", vmedrv_name);
	    printk(KERN_WARNING "  requested size: %d\n", DMA_BUFFER_SIZE);
	    return -ENOMEM;
	}
	bit3.dma_buffer_mapped_pci_address = map_dma_windows(
	     bit3.dma_buffer_bus_address, 
	     bit3.dma_buffer_size, 
             /* byte swapping = */ 0
	);
    }

    return 0;
}

static int start_dma(struct dev_prop_t* dev_prop, unsigned long pci_address, unsigned long vme_address, unsigned long size, int direction)
{
    unsigned status;
    long timeout, remaining_time;
    int error_interrupt_count_before_dma;

    /* set DMA registers, and start */
    initiate_dma(dev_prop, pci_address, vme_address, size, direction);
    DEBUG_DMA(printk(KERN_DEBUG "now start dma transfer...\n"));

    error_interrupt_count_before_dma = error_interrupt_count;

    /* wait for DMA DONE interrupt */
    timeout = ((size*HZ)/100000 + HZ/10); /* 100kB/sec+100msec; slow enough */
    remaining_time = wait_event_timeout(
        vmedrv_dmadone_wait_queue, 
	(! is_dma_running),
	timeout
    );
    if (is_dma_running) {
        /* clear the DMA Command Register */
        /* this is done in the interrupt handler, but not for timeout */
        iowrite8(0, bit3.mapped_node_io_base + regDMA_COMMAND);  

	printk(KERN_WARNING "%s: dma transfer timed out.\n", vmedrv_name);
	release_dma();
	return -ETIMEDOUT;
    }

    /* release DMA settings */
    DEBUG_DMA(printk(KERN_DEBUG "dma transfer completed.\n"));    
    status = release_dma();

    /* release DMA settings */
    if (status & lsERROR_BITS) {
	printk(KERN_WARNING "%s: dma transfer failed.\n", vmedrv_name);
	printk(KERN_WARNING "  Local Status Register: 0x%02x\n", status);
	return -EIO;
    }
    if (error_interrupt_count != error_interrupt_count_before_dma) {
	status = local_status_on_error;
	printk(KERN_WARNING "%s: error during dma transfer.\n", vmedrv_name);
	printk(KERN_WARNING "  Local Status Register: 0x%02x\n", status);
	return -EIO;
    }

    return 0;
}


static void* allocate_dma_buffer(unsigned long size)
{
    dma_addr_t dma_handle;

    if (size > bit3_DMA_MAPPING_SIZE) {
	size = bit3_DMA_MAPPING_SIZE;
    }

    bit3.dma_buffer = pci_alloc_consistent(
	bit3.pci_dev, size, &dma_handle
    );

    if (bit3.dma_buffer > 0) {
	bit3.dma_buffer_size = size;
	bit3.dma_buffer_bus_address = dma_handle;

	DEBUG_DMA(printk(KERN_DEBUG "dma buffer is allocated.\n"));
	DEBUG_DMA(printk(KERN_DEBUG "  size: 0x%lx.\n", bit3.dma_buffer_size));
	DEBUG_DMA(printk(KERN_DEBUG "  virtual address: 0x%08lx.\n", (long) bit3.dma_buffer));
	DEBUG_DMA(printk(KERN_DEBUG "  bus address: 0x%08lx.\n", (long) bit3.dma_buffer_bus_address));
    }

    return bit3.dma_buffer;
}


static void release_dma_buffer(void)
{
    if (bit3.dma_buffer_size > 0) {
	pci_free_consistent(
	    bit3.pci_dev, bit3.dma_buffer_size, 
	    bit3.dma_buffer, bit3.dma_buffer_bus_address
	);
	bit3.dma_buffer_size = 0;
	DEBUG_DMA(printk(KERN_DEBUG "dma buffer is released.\n"));
    }
}


static unsigned map_dma_windows(unsigned pci_address, unsigned size, unsigned flags) 
{
    unsigned base_address, offset_address;
    unsigned number_of_windows;
    unsigned window_index;
    void* mapping_register_address;
    unsigned mapped_pci_address;
    int i;

    base_address = pci_address & bit3_DMA_PAGE_BASE_MASK;
    offset_address = pci_address & bit3_DMA_PAGE_OFFSET_MASK;
    number_of_windows = ((unsigned long) (size - 1)) / (unsigned long) bit3_DMA_WINDOW_SIZE + 1;
    if (offset_address > 0) {
        number_of_windows += 1;
    }

    window_index = 0;
    mapping_register_address = (
	bit3.dma_mapping_registers_base + 
	bit3_DMA_MAPPING_REGISTER_WIDTH * window_index
    );

    DEBUG_DMA(printk(KERN_DEBUG "writing dma mapping registers...\n"));
    for (i = 0; i < number_of_windows; i++) {
        iowrite32(
	    base_address | flags, 
	    mapping_register_address
	);

	DEBUG_DMA(printk(
	     KERN_DEBUG "  reg: 0x%08x, value: 0x%08x\n", 
	     (unsigned) mapping_register_address, 
	     base_address | flags
	));

        base_address += bit3_DMA_WINDOW_SIZE;
	mapping_register_address += bit3_DMA_MAPPING_REGISTER_WIDTH;
    }

    mapped_pci_address = window_index << bit3_DMA_MAPPING_REGISTER_INDEX_SHIFT;
    mapped_pci_address |= offset_address;

    DEBUG_DMA(printk(KERN_DEBUG "  mapped pci address: 0x%08x\n", mapped_pci_address));

    return mapped_pci_address;
}


static int initiate_dma(struct dev_prop_t* dev_prop, unsigned mapped_pci_address, unsigned vme_address, unsigned size, int direction)
{
    unsigned remainder_count, packet_count;
    unsigned dma_register_value, address_modifier, remote_command2_value;
    
    DEBUG_DMA(printk(KERN_DEBUG "setting dma parameters...\n"));

    /* program the Local DMA Command Register */
    dma_register_value = dcENABLE_DMA_DONE_INTERRUPT;
    if (direction == tdREAD) {
	dma_register_value |= dcDMA_TRANSFER_DIRECTION_READ;
    }
    else {
	dma_register_value |= dcDMA_TRANSFER_DIRECTION_WRITE;
    }
    if (dev_prop->data_width == dwWORD) {
	dma_register_value |= dcDMA_WORD_LONGWORD_SELECT_WORD;
    }
    else {
	dma_register_value |= dcDMA_WORD_LONGWORD_SELECT_LONGWORD;
    }
    iowrite8(
	dma_register_value,
	bit3.mapped_node_io_base + regDMA_COMMAND
    );
    DEBUG_DMA(printk(KERN_DEBUG "  dma reg value: 0x%02x\n", dma_register_value));

    /* program the Local DMA Address Register */
    iowrite8(
	(mapped_pci_address >> 0) & 0x000000ff,
	bit3.mapped_node_io_base + regDMA_PCI_ADDRESS_2_7
    );
    iowrite8(
	(mapped_pci_address >> 8) & 0x000000ff,
	bit3.mapped_node_io_base + regDMA_PCI_ADDRESS_8_15
    );
    iowrite8(
	(mapped_pci_address >> 16) & 0x000000ff,
	bit3.mapped_node_io_base + regDMA_PCI_ADDRESS_16_23
    );
    DEBUG_DMA(printk(KERN_DEBUG "  mapped pci address: 0x%08x\n", mapped_pci_address));
	
    /* load the Remote DMA Address Register */
    iowrite8(
	(vme_address >> 0) & 0x000000ff,
	bit3.mapped_node_io_base + regDMA_VME_ADDRESS_0_7
    );
    iowrite8(
	(vme_address >> 8) & 0x000000ff,
	bit3.mapped_node_io_base + regDMA_VME_ADDRESS_8_15
    );
    iowrite8(
	(vme_address >> 16) & 0x000000ff,
	bit3.mapped_node_io_base + regDMA_VME_ADDRESS_16_23
    );
    iowrite8(
	(vme_address >> 24) & 0x000000ff,
	bit3.mapped_node_io_base + regDMA_VME_ADDRESS_24_31
    );

    DEBUG_DMA(printk(KERN_DEBUG "  vme address: 0x%08x\n", vme_address));

    /* load the Remainder/Packet Count Register */
    remainder_count = size % bit3_DMA_PACKET_SIZE;
    packet_count = size / bit3_DMA_PACKET_SIZE;
    iowrite8(
	remainder_count,
	bit3.mapped_node_io_base + regDMA_REMOTE_REMAINDER_COUNT
    );
    iowrite8(
	remainder_count,
	bit3.mapped_node_io_base + regDMA_REMAINDER_COUNT
    );
    iowrite8(
	(packet_count >> 0) & 0x00ff,
	bit3.mapped_node_io_base + regDMA_PACKET_COUNT_0_7
    );
    iowrite8(
	(packet_count >> 8) & 0x00ff,
	bit3.mapped_node_io_base + regDMA_PACKET_COUNT_8_15
    );
    DEBUG_DMA(printk(KERN_DEBUG "  remainder count: 0x%02x\n", remainder_count));
    DEBUG_DMA(printk(KERN_DEBUG "  packet count: 0x%04x\n", packet_count));

    /* program the other CSRs */
    if (dev_prop->transfer_method == tmDMA) {
	address_modifier = dev_prop->dma_address_modifier;
	remote_command2_value = rcBLOCK_MODE_DMA | rcDISABLE_INTERRUPT_PASSING;
    }
    else /* Non-Blocking DMA */ {
	address_modifier = dev_prop->address_modifier;
	remote_command2_value = rcDISABLE_INTERRUPT_PASSING;
    }
    writeb(
	address_modifier,
	bit3.mapped_node_io_base + regADDRESS_MODIFIER
    );
    writeb(
        remote_command2_value,
	bit3.mapped_node_io_base + regREMOTE_COMMAND_2
    );
    DEBUG_DMA(printk(KERN_DEBUG "  dma am code: 0x%02x\n", dev_prop->dma_address_modifier));
    DEBUG_DMA(printk(KERN_DEBUG "  remote command 2: 0x%02x\n", remote_command2_value));

    /* enable normal interrupt*/
    save_interrupt_flags();
    enable_normal_interrupt();

    /* now, start the DMA transfer */
    is_dma_running = 1;
    wmb();
    iowrite8(
	dma_register_value | dcSTART_DMA,
	bit3.mapped_node_io_base + regDMA_COMMAND
    );
    
    return 0;
}


static int release_dma(void)
{
    unsigned status;

    status = ioread8(bit3.mapped_node_io_base + regLOCAL_STATUS);
    DEBUG_DMA(printk(KERN_DEBUG "Local Status Register: 0x%02x\n", status));

    rmb();
    iowrite8(lcCLEAR_STATUS, bit3.mapped_node_io_base + regLOCAL_COMMAND);

    /* restore VME interrupt flags */
    restore_interrupt_flags();
    iowrite8(
	0,
	bit3.mapped_node_io_base + regREMOTE_COMMAND_2
    );
    
    return status;
}
