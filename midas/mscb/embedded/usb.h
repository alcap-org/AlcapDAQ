/********************************************************************\

  Name:         usb.h
  Created by:   Stefan Ritt

  Contents:     USB header file for C8051F320

  $Log: usb.h,v $
  Revision 1.1.1.1  2005/06/20 23:37:20  mucap
  Importing release 1.9.5 of the MIDAS source code as distributed by PSI.
  (Next, I'll commit our local customizations.)

  Revision 1.1  2004/03/04 14:38:26  midas
  Initial revision


\********************************************************************/

/*---- USB registers -----------------------------------------------*/

// USB Core Registers
#define  BASE     0x00
#define  FADDR    BASE
#define  POWER    BASE + 0x01
#define  IN1INT   BASE + 0x02
#define  OUT1INT  BASE + 0x04
#define  CMINT    BASE + 0x06
#define  IN1IE    BASE + 0x07
#define  OUT1IE   BASE + 0x09
#define  CMIE     BASE + 0x0B
#define  FRAMEL   BASE + 0x0C
#define  FRAMEH   BASE + 0x0D
#define  INDEX    BASE + 0x0E
#define  CLKREC   BASE + 0x0F
#define  E0CSR    BASE + 0x11
#define  EINCSRL  BASE + 0x11
#define  EINCSRH  BASE + 0x12
#define  EOUTCSRL BASE + 0x14
#define  EOUTCSRH BASE + 0x15
#define  E0CNT    BASE + 0x16
#define  EOUTCNTL BASE + 0x16
#define  EOUTCNTH BASE + 0x17
#define  FIFO_EP0 BASE + 0x20
#define  FIF0_EP1 BASE + 0x21
#define  FIF0_EP2 BASE + 0x22
#define  FIFO_EP3 BASE + 0x23

// USB Core Register Bits

// POWER
#define  rbISOUD        0x80
#define  rbSPEED        0x40
#define  rbUSBRST       0x08
#define  rbRESUME       0x04
#define  rbSUSMD        0x02
#define  rbSUSEN        0x01

// IN1INT
#define  rbIN3          0x08
#define  rbIN2          0x04
#define  rbIN1          0x02
#define  rbEP0          0x01

// OUT1INT
#define  rbOUT3         0x08
#define  rbOUT2         0x04
#define  rbOUT1         0x02

// CMINT
#define  rbSOF          0x08
#define  rbRSTINT       0x04
#define  rbRSUINT       0x02
#define  rbSUSINT       0x01

// IN1IE
#define  rbIN3E         0x08
#define  rbIN2E         0x04
#define  rbIN1E         0x02
#define  rbEP0E         0x01

// OUT1IE
#define  rbOUT3E        0x08
#define  rbOUT2E        0x04
#define  rbOUT1E        0x02

// CMIE
#define  rbSOFE         0x08
#define  rbRSTINTE      0x04
#define  rbRSUINTE      0x02
#define  rbSUSINTE      0x01

// E0CSR
#define  rbSSUEND       0x80
#define  rbSOPRDY       0x40
#define  rbSDSTL        0x20
#define  rbSUEND        0x10
#define  rbDATAEND      0x08
#define  rbSTSTL        0x04
#define  rbINPRDY       0x02
#define  rbOPRDY        0x01

// EINCSR1
#define  rbInCLRDT      0x40
#define  rbInSTSTL      0x20
#define  rbInSDSTL      0x10
#define  rbInFLUSH      0x08
#define  rbInUNDRUN     0x04
#define  rbInFIFONE     0x02
#define  rbInINPRDY     0x01

// EINCSR2
#define  rbInDBIEN      0x80
#define  rbInISO        0x40
#define  rbInDIRSEL     0x20
#define  rbInFCDT       0x08
#define  rbInSPLIT      0x04

// EOUTCSR1
#define  rbOutCLRDT     0x80
#define  rbOutSTSTL     0x40
#define  rbOutSDSTL     0x20
#define  rbOutFLUSH     0x10
#define  rbOutDATERR    0x08
#define  rbOutOVRUN     0x04
#define  rbOutFIFOFUL   0x02
#define  rbOutOPRDY     0x01

// EOUTCSR2
#define  rbOutDBOEN     0x80
#define  rbOutISO       0x40

// USB device speed settings
#define  FULL_SPEED     0x20
#define  LOW_SPEED      0x00

// USB clock selections (SFR CLKSEL)
#define  USB_4X_CLOCK   0x00

// System clock selections (SFR CLKSEL)
#define  SYS_INT_OSC    0x00
#define  SYS_EXT_OSC    0x01

// Define endpoint status values
#define  EP_IDLE           0
#define  EP_TX             1
#define  EP_ERROR          2
#define  EP_HALTED         3
#define  EP_RX             4

// Define device states
#define  DEV_DEFAULT       0
#define  DEV_ADDRESS       1
#define  DEV_CONFIG        2

#define  EP_MASK_DIR    0x80

// Endpoint addresses
#define  EP1_IN         0x81
#define  EP1_OUT        0x01
#define  EP2_IN         0x82
#define  EP2_OUT        0x02
#define  EP3_IN         0x83
#define  EP3_OUT        0x03

/* Register read/write macros */
#define UREAD_BYTE(addr,target) USB0ADR = (0x80 | addr); while(USB0ADR & 0x80); target = USB0DAT
#define UWRITE_BYTE(addr,data) USB0ADR = addr; USB0DAT = data; while(USB0ADR & 0x80)

/*---- Basic types -------------------------------------------------*/

typedef unsigned char BYTE;
typedef unsigned int UINT;
typedef union {unsigned int i; unsigned char c[2];} WORD;

/*---- USB configuration -------------------------------------------*/

#define  NUM_CFG 1                         // Total number of defined
                                           // configurations

#define  MAX_IF_DSC 1                      // Maximum number of interface
                                           // descriptors for any defined
                                           // configuration

#define  MAX_IF 1                          // Maximum number of interfaces
                                           // for any defined configuration

#define  CFG1_IF_DSC 1                     // Total number of interface
                                           // descriptors for configuration1

#define  CFG1_EP_DSC 2                     // Total number of endpoint
                                           // descriptors for configuration1

#define  REMOTE_WAKEUP_SUPPORT 0           // This should be "ON" if the
                                           // device is capable of remote
                                           // wakeup (this does not mean that
                                           // remote wakeup is enabled)
                                           // Otherwise "OFF"

#define  SELF_POWERED_SUPPORT 0            // This should be "ON" if the
                                           // device is self-powered;
                                           // "OFF" if the device
                                           // is bus-powered.

#define  SUSPEND_ENABLE 0                  // This should be "ON" if the
                                           // device should respond to suspend
                                           // signaling on the bus.
                                           // Otherwise "OFF"

#define  ISO_UPDATE_ENABLE 0               // This should be "ON" if the ISO
                                           // Update feature should be turned
                                           // on for all ISO endpoints. It
                                           // should be "OFF" if the ISO
                                           // update feature should not be
                                           // enabled, or if no ISO endpoints
                                           // will be configured

// Endpoint buffer / packet sizes
// These constants should match the desired maximum packet size for each
// endpoint. Note that the size must not exceed the size of the FIFO
// allocated to the target endpoint. This size will depend on the configuration
// of the endpoint FIFO (split mode and double buffer options), as described
// in the device datasheet.
#define  EP0_PACKET_SIZE   64              // Endpoint0 maximum packet size
#define  EP1_PACKET_SIZE   64              // Endpoint1 maximum packet size
#define  EP2_PACKET_SIZE   64              // Endpoint2 maximum packet size

/*---- Structures --------------------------------------------------*/

/* Endpoint status (used for IN, OUT, and Endpoint0) */
typedef struct EP_STATUS {
   BYTE  bEp;                 // Endpoint number (address)
   UINT  uNumBytes;           // Number of bytes available to transmit
   UINT  uMaxP;               // Maximum packet size
   BYTE  bEpState;            // Endpoint state
   void *pData;               // Pointer to data to transmit
   WORD  wData;               // Storage for small data packets
} EP_STATUS;
typedef EP_STATUS * PEP_STATUS;

/* Interface status */
typedef struct IF_STATUS {
   BYTE bNumAlts;             // Number of alternate choices for this
                              // interface
   BYTE bCurrentAlt;          // Current alternate setting for this interface
                              // zero means this interface does not exist
                              // or the device is not configured
   BYTE bIfNumber;            // Interface number for this interface
                              // descriptor
} IF_STATUS;
typedef IF_STATUS * PIF_STATUS;

/* Device status */
typedef struct DEVICE_STATUS {
   BYTE bCurrentConfig;       // Index number for the selected config
   BYTE bDevState;            // Current device state
   BYTE bRemoteWakeupSupport; // Does this device support remote wakeup?
   BYTE bRemoteWakeupStatus;  // Device remote wakeup enabled/disabled
   BYTE bSelfPoweredStatus;   // Device self- or bus-powered
   BYTE bNumInterf;           // Number of interfaces for this configuration
   BYTE bTotalInterfDsc;      // Total number of interface descriptors for
                              // this configuration (includes alt.
                              // descriptors)
   BYTE* pConfig;             // Points to selected configuration desc
   IF_STATUS IfStatus[MAX_IF];// Array of interface status structures
} DEVICE_STATUS;
typedef DEVICE_STATUS * PDEVICE_STATUS;

/* Control endpoint command (from host) */
typedef struct EP0_COMMAND {
   BYTE  bmRequestType;       // Request type
   BYTE  bRequest;            // Specific request
   WORD  wValue;              // Misc field
   WORD  wIndex;              // Misc index
   WORD  wLength;             // Length of the data segment for this request
} EP0_COMMAND;

/*---- Descriptor related ------------------------------------------*/

// Descriptor sizes
#define  STD_DSC_SIZE     18               // Device
#define  CFG_DSC_SIZE      9               // Configuration
#define  IF_DSC_SIZE       9               // Interface
#define  EP_DSC_SIZE       7               // Endpoint

// Standard Device Descriptor Array Indicies
#define  std_bLength 0                     // Length of this descriptor
#define  std_bDescriptorType 1             // Device desc type (const. 0x01)
#define  std_bcdUSB 2                      // USB Specification used (BCD)
#define  std_bDeviceClass 4                // Device Class
#define  std_bDeviceSubClass 5             // Device SubClass
#define  std_bDeviceProtocol 6             // Device Protocol
#define  std_bMaxPacketSize0 7             // Maximum packet size for Endpoint0
#define  std_idVendor 8                    // Vendor ID 
#define  std_idProduct 10                  // Product ID
#define  std_bcdDevice 12                  // Device revision number
#define  std_iManufacturer 14              // Manufacturer name string index
#define  std_std_iProduct 15               // Product name string index
#define  std_iSerialNumber 16              // Serial number string index
#define  std_bNumConfigurations 17         // Number of supported configurations

// Configuration Descriptor Array Indicies
#define  cfg_bLength 0                     // Length of this desc
#define  cfg_bDescriptorType 1             // Config desc type (const. 0x02)
#define  cfg_wTotalLength_lsb 2            // Total length, including
#define  cfg_wTotalLength_msb 3            // interface & endpoints
#define  cfg_bNumInterfaces 4              // Number of supported int's
#define  cfg_bConfigurationValue 5         // Config index
#define  cfg_iConfiguration 6              // Index for string desc
#define  cfg_bmAttributes 7                // Power, wakeup options
#define  cfg_MaxPower 8                    // Max bus power consumed

// Interface Descriptor Array Indicies
#define  if_bLength 0                      // Length of this desc
#define  if_bDescriptorType 1              // Interface desc type (const. ??  )
#define  if_bInterfaceNumber 2             // This interface index
#define  if_bAlternateSetting 3            // Alternate index
#define  if_bNumEndpoints 4                // Endpoints used by this interface
#define  if_bInterfaceClass 5              // Device class
#define  if_bInterfaceSubClass 6           // Device subclass
#define  if_bInterfaceProcotol 7           // Class-specific protocol
#define  if_iInterface 8                   // Index for string desc

// Endpoint Descriptor Array Indicies
#define  ep_bLength 0                      // Length of this desc
#define  ep_bDescriptorType 1              // Endpoint desc type
#define  ep_bEndpointAddress 2             // This endpoint address
#define  ep_bmAttributes 3                 // Transfer type
#define  ep_wMaxPacketSize 4               // Max FIFO size
#define  ep_bInterval 6                    // Polling interval (int only)

// Descriptor structure
// This structure contains all usb descriptors for the device.
// The descriptors are held in array format, and are accessed with the offsets
// defined in the header file "usb.h". The constants used in the
// array declarations are also defined in header file "usb.h".
typedef struct DESCRIPTORS {
   BYTE bStdDevDsc[STD_DSC_SIZE];
   BYTE bCfg1[CFG_DSC_SIZE + IF_DSC_SIZE*CFG1_IF_DSC + EP_DSC_SIZE*CFG1_EP_DSC];
} DESCRIPTORS;

/*---- Standard requests -------------------------------------------*/

// Standard Request Codes
#define  GET_STATUS              0x00
#define  CLEAR_FEATURE           0x01
#define  SET_FEATURE             0x03
#define  SET_ADDRESS             0x05
#define  GET_DESCRIPTOR          0x06
#define  SET_DESCRIPTOR          0x07
#define  GET_CONFIGURATION       0x08
#define  SET_CONFIGURATION       0x09
#define  GET_INTERFACE           0x0A
#define  SET_INTERFACE           0x0B
#define  SYNCH_FRAME             0x0C

// bmRequestType Masks
#define  CMD_MASK_DIR            0x80     // Request direction bit mask
#define  CMD_MASK_TYPE           0x60     // Request type bit mask
#define  CMD_MASK_RECIP          0x1F     // Request recipient bit mask
#define  CMD_MASK_COMMON         0xF0     // Common request mask

// bmRequestType Direction Field
#define  CMD_DIR_IN              0x80     // IN Request
#define  CMD_DIR_OUT             0x00     // OUT Request

// bmRequestType Type Field
#define  CMD_TYPE_STD            0x00     // Standard Request
#define  CMD_TYPE_CLASS          0x20     // Class Request
#define  CMD_TYPE_VEND           0x40     // Vendor Request

// bmRequestType Recipient Field
#define  CMD_RECIP_DEV           0x00     // Device Request
#define  CMD_RECIP_IF            0x01     // Interface Request
#define  CMD_RECIP_EP            0x02     // Endpoint Request
#define  CMD_RECIP_OTHER         0x03     // Other Request

// bmRequestType Common Commands
#define  CMD_STD_DEV_OUT         0x00     // Standard Device Request OUT
#define  CMD_STD_DEV_IN          0x80     // Standard Device Request IN
#define  CMD_STD_IF_OUT          0x01     // Standard Interface Request OUT
#define  CMD_STD_IF_IN           0x81     // Standard Interface Request IN

// Standard Descriptor Types
#define  DSC_DEVICE              0x01     // Device Descriptor
#define  DSC_CONFIG              0x02     // Configuration Descriptor
#define  DSC_STRING              0x03     // String Descriptor
#define  DSC_INTERFACE           0x04     // Interface Descriptor
#define  DSC_ENDPOINT            0x05     // Endpoint Descriptor

#define  DSC_MASK_REMOTE         0x20     // Remote Wakeup Support Mask
                                          // (bmAttributes Config Desc field)

// Feature Selectors (used in set and clear feature commands)
#define  DEVICE_REMOTE_WAKEUP    0x01     // Remote Wakeup selector
#define  ENDPOINT_HALT           0x00     // Endpoint Halt selector

/*---- Function prototypes -----------------------------------------*/

void usb_init(unsigned char *buffer, unsigned char *size);
void usb_send(unsigned char *buffer, unsigned char size);

