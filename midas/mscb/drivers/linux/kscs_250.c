/*
 * MSCB Submaster SCS-250 USB driver
 * 
 * Based on usb-skeleton.c from Greg Kroah-Hartman (greg@kroah.com)
 *
 */

#define __KERNEL__
#define MODULE

#include <linux/config.h>

#if defined(CONFIG_MODVERSIONS) && !defined(MODVERSIONS)
#  define MODVERSIONS
#endif

#ifdef MODVERSIONS
#  include <linux/modversions.h>
#endif

#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/signal.h>
#include <linux/errno.h>
#include <linux/poll.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/fcntl.h>
#include <linux/module.h>
#include <linux/spinlock.h>
#include <linux/list.h>
#include <linux/smp_lock.h>
#include <linux/devfs_fs_kernel.h>
#include <linux/usb.h>

#ifdef CONFIG_USB_DEBUG
	static int debug = 1;
#else
	static int debug;
#endif

/* Use our own dbg macro */
#undef dbg
#define dbg(format, arg...) do { if (debug) printk(KERN_DEBUG __FILE__ ": " format "\n" , ## arg); } while (0)


/* Version Information */
#define DRIVER_VERSION "v1.0"
#define DRIVER_AUTHOR "Stefan Ritt, PSI, stefan.ritt@psi.ch"
#define DRIVER_DESC "USB SCS-250 Driver"

/* Module paramaters */
MODULE_PARM(debug, "i");
MODULE_PARM_DESC(debug, "Debug enabled or not");


/* Define these values to match your device */
#define USB_SCS_250_VENDOR_ID	0x10C4
#define USB_SCS_250_PRODUCT_ID	0x1175

/* table of devices that work with this driver */
static struct usb_device_id scs_250_table [] = {
	{ USB_DEVICE(USB_SCS_250_VENDOR_ID, USB_SCS_250_PRODUCT_ID) },
	{ }					/* Terminating entry */
};

MODULE_DEVICE_TABLE (usb, scs_250_table);



/* Get a minor range for your devices from the usb maintainer */
#define USB_SCS_250_MINOR_BASE	193	

/* we can have up to this number of device plugged in at once */
#define MAX_DEVICES		16

/* Structure to hold all of our device specific stuff */
struct usb_scs_250 {
	struct usb_device *	udev;			/* save off the usb device pointer */
	struct usb_interface *	interface;		/* the interface for this device */
	devfs_handle_t		devfs;			/* devfs device node */
	unsigned char		minor;			/* the starting minor number for this device */
	unsigned char		num_ports;		/* the number of ports this device has */
	char			num_interrupt_in;	/* number of interrupt in endpoints we have */
	char			num_bulk_in;		/* number of bulk in endpoints we have */
	char			num_bulk_out;		/* number of bulk out endpoints we have */

	unsigned char *		bulk_in_buffer;		/* the buffer to receive data */
	int			bulk_in_size;		/* the size of the receive buffer */
	__u8			bulk_in_endpointAddr;	/* the address of the bulk in endpoint */

	unsigned char *		bulk_out_buffer;	/* the buffer to send data */
	int			bulk_out_size;		/* the size of the send buffer */
	struct urb *		write_urb;		/* the urb used to send data */
	__u8			bulk_out_endpointAddr;	/* the address of the bulk out endpoint */

	struct tq_struct	tqueue;			/* task queue for line discipline waking up */
	int			open_count;		/* number of times this port has been opened */
	struct semaphore	sem;			/* locks this structure */
};


/* the global usb devfs handle */
extern devfs_handle_t usb_devfs_handle;


/* local function prototypes */
static ssize_t scs_250_read	(struct file *file, char *buffer, size_t count, loff_t *ppos);
static ssize_t scs_250_write	(struct file *file, const char *buffer, size_t count, loff_t *ppos);
static int scs_250_ioctl	(struct inode *inode, struct file *file, unsigned int cmd, unsigned long arg);
static int scs_250_open		(struct inode *inode, struct file *file);
static int scs_250_release	(struct inode *inode, struct file *file);
	
static void * scs_250_probe	(struct usb_device *dev, unsigned int ifnum, const struct usb_device_id *id);
static void scs_250_disconnect	(struct usb_device *dev, void *ptr);

static void scs_250_write_bulk_callback	(struct urb *urb);


/* array of pointers to our devices that are currently connected */
static struct usb_scs_250	*minor_table[MAX_DEVICES];

/* lock to protect the minor_table structure */
static DECLARE_MUTEX (minor_table_mutex);

/*
 * File operations needed when we register this driver.
 * This assumes that this driver NEEDS file operations,
 * of course, which means that the driver is expected
 * to have a node in the /dev directory. If the USB
 * device were for a network interface then the driver
 * would use "struct net_driver" instead, and a serial
 * device would use "struct tty_driver". 
 */
static struct file_operations scs_250_fops = {
	/*
	 * The owner field is part of the module-locking
	 * mechanism. The idea is that the kernel knows
	 * which module to increment the use-counter of
	 * BEFORE it calls the device's open() function.
	 * This also means that the kernel can decrement
	 * the use-counter again before calling release()
	 * or should the open() function fail.
	 *
	 * Not all device structures have an "owner" field
	 * yet. "struct file_operations" and "struct net_device"
	 * do, while "struct tty_driver" does not. If the struct
	 * has an "owner" field, then initialize it to the value
	 * THIS_MODULE and the kernel will handle all module
	 * locking for you automatically. Otherwise, you must
	 * increment the use-counter in the open() function
	 * and decrement it again in the release() function
	 * yourself.
	 */
	owner:		THIS_MODULE,

	read:		scs_250_read,
	write:		scs_250_write,
	ioctl:		scs_250_ioctl,
	open:		scs_250_open,
	release:	scs_250_release,
};      


/* usb specific object needed to register this driver with the usb subsystem */
static struct usb_driver scs_250_driver = {
	name:		"scs_250",
	probe:		scs_250_probe,
	disconnect:	scs_250_disconnect,
	fops:		&scs_250_fops,
	minor:		USB_SCS_250_MINOR_BASE,
	id_table:	scs_250_table,
};





/**
 *	usb_scs_250_debug_data
 */
static inline void usb_scs_250_debug_data (const char *function, int size, const unsigned char *data)
{
	int i;

	if (!debug)
		return;
	
	printk (KERN_DEBUG __FILE__": %s - length = %d, data = ", 
		function, size);
	for (i = 0; i < size; ++i) {
		printk ("%.2x ", data[i]);
	}
	printk ("\n");
}


/**
 *	scs_250_delete
 */
static inline void scs_250_delete (struct usb_scs_250 *dev)
{
	minor_table[dev->minor] = NULL;
	if (dev->bulk_in_buffer != NULL)
		kfree (dev->bulk_in_buffer);
	if (dev->bulk_out_buffer != NULL)
		kfree (dev->bulk_out_buffer);
	if (dev->write_urb != NULL)
		usb_free_urb (dev->write_urb);
	kfree (dev);
}


/**
 *	scs_250_open
 */
static int scs_250_open (struct inode *inode, struct file *file)
{
	struct usb_scs_250 *dev = NULL;
	int subminor;
	int retval = 0;
	
	dbg(__FUNCTION__);

	subminor = MINOR (inode->i_rdev) - USB_SCS_250_MINOR_BASE;
	if ((subminor < 0) ||
	    (subminor >= MAX_DEVICES)) {
		return -ENODEV;
	}

	/* Increment our usage count for the module.
	 * This is redundant here, because "struct file_operations"
	 * has an "owner" field. This line is included here soley as
	 * a reference for drivers using lesser structures... ;-)
	 */
	MOD_INC_USE_COUNT;

	/* lock our minor table and get our local data for this minor */
	down (&minor_table_mutex);
	dev = minor_table[subminor];
	if (dev == NULL) {
		up (&minor_table_mutex);
		MOD_DEC_USE_COUNT;
		return -ENODEV;
	}

	/* lock this device */
	down (&dev->sem);

	/* unlock the minor table */
	up (&minor_table_mutex);

	/* increment our usage count for the driver */
	++dev->open_count;

	/* save our object in the file's private structure */
	file->private_data = dev;

	/* unlock this device */
	up (&dev->sem);

	return retval;
}


/**
 *	scs_250_release
 */
static int scs_250_release (struct inode *inode, struct file *file)
{
	struct usb_scs_250 *dev;
	int retval = 0;

	dev = (struct usb_scs_250 *)file->private_data;
	if (dev == NULL) {
		dbg (__FUNCTION__ " - object is NULL");
		return -ENODEV;
	}

	dbg(__FUNCTION__ " - minor %d", dev->minor);

	/* lock our minor table */
	down (&minor_table_mutex);

	/* lock our device */
	down (&dev->sem);

	if (dev->open_count <= 0) {
		dbg (__FUNCTION__ " - device not opened");
		retval = -ENODEV;
		goto exit_not_opened;
	}

	if (dev->udev == NULL) {
		/* the device was unplugged before the file was released */
		up (&dev->sem);
		scs_250_delete (dev);
		up (&minor_table_mutex);
		MOD_DEC_USE_COUNT;
		return 0;
	}

	/* decrement our usage count for the device */
	--dev->open_count;
	if (dev->open_count <= 0) {
		/* shutdown any bulk writes that might be going on */
		usb_unlink_urb (dev->write_urb);
		dev->open_count = 0;
	}

	/* decrement our usage count for the module */
	MOD_DEC_USE_COUNT;

exit_not_opened:
	up (&dev->sem);
	up (&minor_table_mutex);

	return retval;
}


/**
 *	scs_250_read
 */
static ssize_t scs_250_read (struct file *file, char *buffer, size_t count, loff_t *ppos)
{
	struct usb_scs_250 *dev;
	int retval = 0;

	dev = (struct usb_scs_250 *)file->private_data;
	
	dbg(__FUNCTION__ " - minor %d, count = %d", dev->minor, count);

	/* lock this object */
	down (&dev->sem);

	/* verify that the device wasn't unplugged */
	if (dev->udev == NULL) {
		up (&dev->sem);
		return -ENODEV;
	}
	
	/* do an immediate bulk read to get data from the device */
	retval = usb_bulk_msg (dev->udev,
			       usb_rcvbulkpipe (dev->udev, 
						dev->bulk_in_endpointAddr),
			       dev->bulk_in_buffer, dev->bulk_in_size,
			       &count, HZ*10);

	/* if the read was successful, copy the data to userspace */
	if (!retval) {
		if (copy_to_user (buffer, dev->bulk_in_buffer, count))
			retval = -EFAULT;
		else
			retval = count;
	}
	
	/* unlock the device */
	up (&dev->sem);
	return retval;
}


/**
 *	scs_250_write
 */
static ssize_t scs_250_write (struct file *file, const char *buffer, size_t count, loff_t *ppos)
{
	struct usb_scs_250 *dev;
	ssize_t bytes_written = 0;
	int retval = 0;

	dev = (struct usb_scs_250 *)file->private_data;

	dbg(__FUNCTION__ " - minor %d, count = %d", dev->minor, count);

	/* lock this object */
	down (&dev->sem);

	/* verify that the device wasn't unplugged */
	if (dev->udev == NULL) {
		retval = -ENODEV;
		goto exit;
	}

	/* verify that we actually have some data to write */
	if (count == 0) {
		dbg(__FUNCTION__ " - write request of 0 bytes");
		goto exit;
	}

	/* see if we are already in the middle of a write */
	if (dev->write_urb->status == -EINPROGRESS) {
		dbg (__FUNCTION__ " - already writing");
		goto exit;
	}

	/* we can only write as much as 1 urb will hold */
	bytes_written = (count > dev->bulk_out_size) ? 
				dev->bulk_out_size : count;

	/* copy the data from userspace into our urb */
	if (copy_from_user(dev->write_urb->transfer_buffer, buffer, 
			   bytes_written)) {
		retval = -EFAULT;
		goto exit;
	}

	usb_scs_250_debug_data (__FUNCTION__, bytes_written, 
			     dev->write_urb->transfer_buffer);

	/* set up our urb */
	FILL_BULK_URB(dev->write_urb, dev->udev, 
		      usb_sndbulkpipe(dev->udev, dev->bulk_out_endpointAddr),
		      dev->write_urb->transfer_buffer, bytes_written,
		      scs_250_write_bulk_callback, dev);

	/* send the data out the bulk port */
	retval = usb_submit_urb(dev->write_urb);
	if (retval) {
		err(__FUNCTION__ " - failed submitting write urb, error %d",
		    retval);
	} else {
		retval = bytes_written;
	}

exit:
	/* unlock the device */
	up (&dev->sem);

	return retval;
}


/**
 *	scs_250_ioctl
 */
static int scs_250_ioctl (struct inode *inode, struct file *file, unsigned int cmd, unsigned long arg)
{
	struct usb_scs_250 *dev;

	dev = (struct usb_scs_250 *)file->private_data;

	/* lock this object */
	down (&dev->sem);

	/* verify that the device wasn't unplugged */
	if (dev->udev == NULL) {
		up (&dev->sem);
		return -ENODEV;
	}

	dbg(__FUNCTION__ " - minor %d, cmd 0x%.4x, arg %ld", 
	    dev->minor, cmd, arg);


	/* fill in your device specific stuff here */
	
	/* unlock the device */
	up (&dev->sem);
	
	/* return that we did not understand this ioctl call */
	return -ENOTTY;
}


/**
 *	scs_250_write_bulk_callback
 */
static void scs_250_write_bulk_callback (struct urb *urb)
{
	struct usb_scs_250 *dev = (struct usb_scs_250 *)urb->context;

	dbg(__FUNCTION__ " - minor %d", dev->minor);

	if ((urb->status != -ENOENT) && 
	    (urb->status != -ECONNRESET)) {
		dbg(__FUNCTION__ " - nonzero write bulk status received: %d",
		    urb->status);
		return;
	}

	return;
}


/**
 *	scs_250_probe
 *
 *	Called by the usb core when a new device is connected that it thinks
 *	this driver might be interested in.
 */
static void * scs_250_probe(struct usb_device *udev, unsigned int ifnum, const struct usb_device_id *id)
{
	struct usb_scs_250 *dev = NULL;
	struct usb_interface *interface;
	struct usb_interface_descriptor *iface_desc;
	struct usb_endpoint_descriptor *endpoint;
	int minor;
	int buffer_size;
	int i;
	char name[10];

	
	/* See if the device offered us matches what we can accept */
	if ((udev->descriptor.idVendor != USB_SCS_250_VENDOR_ID) ||
	    (udev->descriptor.idProduct != USB_SCS_250_PRODUCT_ID)) {
		return NULL;
	}

	/* select a "subminor" number (part of a minor number) */
	down (&minor_table_mutex);
	for (minor = 0; minor < MAX_DEVICES; ++minor) {
		if (minor_table[minor] == NULL)
			break;
	}
	if (minor >= MAX_DEVICES) {
		info ("Too many devices plugged in, can not handle this device.");
		goto exit;
	}

	/* allocate memory for our device state and intialize it */
	dev = kmalloc (sizeof(struct usb_scs_250), GFP_KERNEL);
	if (dev == NULL) {
		err ("Out of memory");
		goto exit;
	}
	memset (dev, 0x00, sizeof (*dev));
	minor_table[minor] = dev;

	interface = &udev->actconfig->interface[ifnum];

	init_MUTEX (&dev->sem);
	dev->udev = udev;
	dev->interface = interface;
	dev->minor = minor;

	/* set up the endpoint information */
	/* check out the endpoints */
	iface_desc = &interface->altsetting[0];
	for (i = 0; i < iface_desc->bNumEndpoints; ++i) {
		endpoint = &iface_desc->endpoint[i];

		if ((endpoint->bEndpointAddress & 0x80) &&
		    ((endpoint->bmAttributes & 3) == 0x02)) {
			/* we found a bulk in endpoint */
			buffer_size = endpoint->wMaxPacketSize;
			dev->bulk_in_size = buffer_size;
			dev->bulk_in_endpointAddr = endpoint->bEndpointAddress;
			dev->bulk_in_buffer = kmalloc (buffer_size, GFP_KERNEL);
			if (!dev->bulk_in_buffer) {
				err("Couldn't allocate bulk_in_buffer");
				goto error;
			}
		}
		
		if (((endpoint->bEndpointAddress & 0x80) == 0x00) &&
		    ((endpoint->bmAttributes & 3) == 0x02)) {
			/* we found a bulk out endpoint */
			dev->write_urb = usb_alloc_urb(0);
			if (!dev->write_urb) {
				err("No free urbs available");
				goto error;
			}
			buffer_size = endpoint->wMaxPacketSize;
			dev->bulk_out_size = buffer_size;
			dev->bulk_out_endpointAddr = endpoint->bEndpointAddress;
			dev->bulk_out_buffer = kmalloc (buffer_size, GFP_KERNEL);
			if (!dev->bulk_out_buffer) {
				err("Couldn't allocate bulk_out_buffer");
				goto error;
			}
			FILL_BULK_URB(dev->write_urb, udev, 
				      usb_sndbulkpipe(udev, 
						      endpoint->bEndpointAddress),
				      dev->bulk_out_buffer, buffer_size,
				      scs_250_write_bulk_callback, dev);
		}
	}

	info("bulk in addr=0x%x, bulk out addr=0x%x", dev->bulk_in_endpointAddr, dev->bulk_out_endpointAddr);

	/* initialize the devfs node for this device and register it */
	sprintf(name, "scs_250_%d", dev->minor);
	
	dev->devfs = devfs_register (usb_devfs_handle, name,
				     DEVFS_FL_DEFAULT, USB_MAJOR,
				     USB_SCS_250_MINOR_BASE + dev->minor,
				     S_IFCHR | S_IRUSR | S_IWUSR | 
				     S_IRGRP | S_IWGRP | S_IROTH, 
				     &scs_250_fops, NULL);

	/* let the user know what node this device is now attached to */
	info ("USB scs_250 device now attached to kscs_250_%d", dev->minor);
	goto exit;
	
error:
	scs_250_delete (dev);
	dev = NULL;

exit:
	up (&minor_table_mutex);
	return dev;
}


/**
 *	scs_250_disconnect
 *
 *	Called by the usb core when the device is removed from the system.
 */
static void scs_250_disconnect(struct usb_device *udev, void *ptr)
{
	struct usb_scs_250 *dev;
	int minor;

	dev = (struct usb_scs_250 *)ptr;
	
	down (&minor_table_mutex);
	down (&dev->sem);
		
	minor = dev->minor;

	/* remove our devfs node */
	devfs_unregister(dev->devfs);

	/* if the device is not opened, then we clean up right now */
	if (!dev->open_count) {
		up (&dev->sem);
		scs_250_delete (dev);
	} else {
		dev->udev = NULL;
		up (&dev->sem);
	}

	info("USB Scs_250eton #%d now disconnected", minor);
	up (&minor_table_mutex);
}



/**
 *	usb_scs_250_init
 */
static int __init usb_scs_250_init(void)
{
	int result;

	/* register this driver with the USB subsystem */
	result = usb_register(&scs_250_driver);
	if (result < 0) {
		err("usb_register failed for the "__FILE__" driver. Error number %d",
		    result);
		return -1;
	}

	info(DRIVER_DESC " " DRIVER_VERSION);
	return 0;
}


/**
 *	usb_scs_250_exit
 */
static void __exit usb_scs_250_exit(void)
{
	/* deregister this driver with the USB subsystem */
	usb_deregister(&scs_250_driver);
}


module_init (usb_scs_250_init);
module_exit (usb_scs_250_exit);

MODULE_AUTHOR(DRIVER_AUTHOR);
MODULE_DESCRIPTION(DRIVER_DESC);
MODULE_LICENSE("GPL");

