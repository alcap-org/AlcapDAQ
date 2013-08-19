/********************************************************************\

  Name:         usb.c
  Created by:   Stefan Ritt

  Contents:     USB routines for Cygnal USB sub-master
                SUBM250 running on Cygnal C8051F320

  $Log: usb.c,v $
  Revision 1.1.1.1  2005/06/20 23:37:20  mucap
  Importing release 1.9.5 of the MIDAS source code as distributed by PSI.
  (Next, I'll commit our local customizations.)

  Revision 1.1  2004/03/04 14:38:23  midas
  Initial revision


\********************************************************************/

#include "mscb.h"
#include "usb.h"

/*---- Globals -----------------------------------------------------*/

DEVICE_STATUS idata gDeviceStatus;
EP0_COMMAND   idata gEp0Command;
EP_STATUS     idata gEp0Status;
EP_STATUS     idata gEp1InStatus;
EP_STATUS     idata gEp2OutStatus;
PIF_STATUS    idata pIfStatus;

BYTE *prx_buf;
BYTE *pn_rx;

/*------------------------------------------------------------------*/

//---------------------------
// Descriptor Declarations
//---------------------------
// All descriptors are contained in the global structure <gDescriptorMap>.
// This structure contains BYTE arrays for the standard device descriptor
// and all configurations. The lengths of the configuration arrays are
// defined by the number of interface and endpoint descriptors required
// for the particular configuration (these constants are named
// CFG1_IF_DSC and CFG1_EP_DSC for configuration1).
//
// The entire gDescriptorMap structure is initialized below in
// codespace.

DESCRIPTORS code gDescriptorMap = {

//---------------------------
// Begin Standard Device Descriptor (structure element stddevdsc)
//---------------------------
   18,                        // bLength
   0x01,                      // bDescriptorType
   0x00, 0x02,                // bcdUSB (lsb first)
   0x00,                      // bDeviceClass
   0x00,                      // bDeviceSubClass
   0x00,                      // bDeviceProtocol
   EP0_PACKET_SIZE,           // bMaxPacketSize0
   0xC4, 0x10,                // idVendor (lsb first)
   0x75, 0x11,                // idProduct (lsb first)
   0x00, 0x00,                // bcdDevice (lsb first)
   0x01,                      // iManufacturer
   0x02,                      // iProduct
   0x00,                      // iSerialNumber
   0x01,                      // bNumConfigurations

//---------------------------
// Begin Configuration 1 (structure element cfg1)
//---------------------------

   // Begin Descriptor: Configuration 1
   0x09,                      // Length
   0x02,                      // Type
   0x20, 0x00,                // TotalLength (lsb first)
   0x01,                      // NumInterfaces
   0x01,                      // bConfigurationValue
   0x00,                      // iConfiguration
   0x80,                      // bmAttributes (no remote wakeup)
   0x0F,                      // MaxPower (*2mA)

   // Begin Descriptor: Interface0, Alternate0
   0x09,                      // bLength
   0x04,                      // bDescriptorType
   0x00,                      // bInterfaceNumber
   0x00,                      // bAlternateSetting
   0x02,                      // bNumEndpoints
   0x00,                      // bInterfaceClass
   0x00,                      // bInterfaceSubClass
   0x00,                      // bInterfaceProcotol
   0x00,                      // iInterface

   // Begin Descriptor: Endpoint1, Interface0, Alternate0
   0x07,                      // bLength
   0x05,                      // bDescriptorType
   0x81,                      // bEndpointAddress (ep1, IN)
   0x02,                      // bmAttributes (Bulk)
   EP1_PACKET_SIZE, 0x00,     // wMaxPacketSize (lsb first)
   0x20,                      // bInterval

   // Begin Descriptor: Endpoint2, Interface0, Alternate0
   0x07,                      // bLength
   0x05,                      // bDescriptorType
   0x02,                      // bEndpointAddress (ep2, OUT)
   0x02,                      // bmAttributes (Bulk)
   EP2_PACKET_SIZE, 0x00,     // wMaxPacketSize (lsb first)
   0x01,                      // bInterval

//---------------------------
// End Configuration 1
//---------------------------

};

//---------------------------
// String Descriptors
//---------------------------

code const BYTE String0Desc[4] =
{
   0x04, 0x03, 0x09, 0x04
};

#define STR1LEN sizeof("PSI S. Ritt")*2

code const BYTE String1Desc[STR1LEN] =
{
   STR1LEN, 0x03,
   'P', 0,
   'S', 0,
   'I', 0,
   ' ', 0,
   'S', 0,
   '.', 0,
   ' ', 0,
   'R', 0,
   'i', 0,
   't', 0,
   't', 0,
};

#define STR2LEN sizeof("MSCB Submaster SCS-250")*2

code const BYTE String2Desc[STR2LEN] =
{
   STR2LEN, 0x03,
   'M', 0,
   'S', 0,
   'C', 0,
   'B', 0,
   ' ', 0,
   'S', 0,
   'u', 0,
   'b', 0,
   'm', 0,
   'a', 0,
   's', 0,
   't', 0,
   'e', 0,
   'r', 0,
   ' ', 0,
   'S', 0,
   'C', 0,
   'S', 0,
   '-', 0,
   '2', 0,
   '5', 0,
   '0', 0,
};

code BYTE* const StringDescTable[] =
{
   String0Desc,
   String1Desc,
   String2Desc
};

/*---- Prototypes --------------------------------------------------*/

void USBReset(void);
void Endpoint0(void);
void BulkOrInterruptIn(PEP_STATUS pEpInStatus);
void BulkOrInterruptOut(PEP_STATUS pEpOutStatus);
void FIFORead(BYTE bEp, UINT uNumBytes, BYTE * pData);
void FIFOWrite(BYTE bEp, UINT uNumBytes, BYTE * pData) reentrant;
void SetAddressRequest(void);
void SetFeatureRequest(void);
void ClearFeatureRequest(void);
void SetConfigurationRequest(void);
void SetInterfaceRequest(void);
void GetStatusRequest(void);
void GetDescriptorRequest(void);
void GetConfigurationRequest(void);
void GetInterfaceRequest(void);
BYTE HaltEndpoint(UINT uEp);
BYTE EnableEndpoint(UINT uEp);
BYTE GetEpStatus(UINT uEp);
BYTE SetConfiguration(BYTE SelectConfig);
BYTE SetInterface(PIF_STATUS pIfStatus);

/*------------------------------------------------------------------*/

/*
  USB Initialization
    - Initialize USB0
    - Enable USB0 interrupts
    - Enable USB0 transceiver
    - Enable USB0
*/
void usb_init(unsigned char *buffer, unsigned char *psize)
{
   UWRITE_BYTE(POWER, 0x08);              // Asynch. reset

   UWRITE_BYTE(IN1IE, 0x0F);              // Enable Endpoint0 Interrupt
   UWRITE_BYTE(OUT1IE, 0x0F);
   UWRITE_BYTE(CMIE, 0x04);               // Enable Reset interrupt

   USB0XCN = 0xC0;                        // Enable transceiver
   USB0XCN |= FULL_SPEED;                 // Select device speed

   UWRITE_BYTE(CLKREC, 0x80);             // Enable clock recovery,
                                          // single-step mode disabled

   EIE1 |= 0x02;                          // Enable USB0 Interrupts

   EA = 1;                                // Enable global interrupts

   UWRITE_BYTE(POWER, 0x00);              // Enable USB0 by clearing the
                                          // USB Inhibit bit
                                          // Suspend mode disabled

   /* save pointers */
   prx_buf = buffer;
   pn_rx = psize;
}

/*------------------------------------------------------------------*/

/*
   This is the top level USB ISR. All endpoint interrupt/request
   handlers are called from this function.

   Handler routines for any configured interrupts should be
   added in the appropriate endpoint handler call slots.
*/
void USB_ISR() interrupt 8
{
   BYTE bCommonInt, bInInt, bOutInt;

   // Read interrupt registers
   UREAD_BYTE(CMINT, bCommonInt);
   UREAD_BYTE(IN1INT, bInInt);
   UREAD_BYTE(OUT1INT, bOutInt);

   // Check for reset interrupt
   if (bCommonInt & rbRSTINT)
      USBReset();

   // Check for Endpoint0 interrupt
   if (bInInt & rbEP0)
      Endpoint0();

   // Endpoint1 IN
//   if (bInInt & rbIN1)
//##      BulkOrInterruptIn(&gEp1InStatus);

   // Endpoint2 OUT
   if (bOutInt & rbOUT2)
      BulkOrInterruptOut(&gEp2OutStatus);
}

/*------------------------------------------------------------------*/

/*
   - Initialize the global Device Status structure (all zeros)
   - Resets all endpoints
*/
void USBReset()
{
   BYTE i, bPower = 0;
   BYTE * pDevStatus;

   // Reset device status structure to all zeros (undefined)
   pDevStatus = (BYTE *)&gDeviceStatus;
   for (i=0;i<sizeof(DEVICE_STATUS);i++)
      *pDevStatus++ = 0x00;

   // Set device state to default
   gDeviceStatus.bDevState = DEV_DEFAULT;

   // REMOTE_WAKEUP_SUPPORT and SELF_POWERED_SUPPORT
   // defined in file "usb_desc.h"
   gDeviceStatus.bRemoteWakeupSupport = REMOTE_WAKEUP_SUPPORT;
   gDeviceStatus.bSelfPoweredStatus = SELF_POWERED_SUPPORT;

   // Reset all endpoints

   // Reset Endpoint0
   gEp0Status.bEpState = EP_IDLE;         // Reset Endpoint0 state
   gEp0Status.bEp = 0;                    // Set endpoint number
   gEp0Status.uMaxP = EP0_PACKET_SIZE;    // Set maximum packet size

   // Reset Endpoint1 IN
   gEp1InStatus.bEpState = EP_HALTED;     // Reset state
   gEp1InStatus.uNumBytes = 0;            // Reset byte counter

   // Reset Endpoint2 OUT
   gEp2OutStatus.bEpState = EP_HALTED;    // Reset state
   gEp2OutStatus.uNumBytes = 0;           // Reset byte counter

   // Get Suspend enable/disable status. If enabled, prepare temporary
   // variable bPower.
   if (SUSPEND_ENABLE)
   {
      bPower = 0x01;                      // Set bit0 (Suspend Enable)
   }

   // Get ISO Update enable/disable status. If enabled, prepare temporary
   // variable bPower.
   if (ISO_UPDATE_ENABLE)
   {
      bPower |= 0x80;                     // Set bit7 (ISO Update Enable)
   }

   UWRITE_BYTE(POWER, bPower);
}

/*------------------------------------------------------------------*/

void Endpoint0()
{
   BYTE bTemp = 0;
   BYTE bCsr1, uTxBytes;

   UWRITE_BYTE(INDEX, 0);                 // Target ep0
   UREAD_BYTE(E0CSR, bCsr1);

   // Handle Setup End
   if (bCsr1 & rbSUEND)                   // Check for setup end
   {                                      // Indicate setup end serviced
      UWRITE_BYTE(E0CSR, rbSSUEND);
      gEp0Status.bEpState = EP_IDLE;      // ep0 state to idle
   }

   // Handle sent stall
   if (bCsr1 & rbSTSTL)                   // If last state requested a stall
   {                                      // Clear Sent Stall bit (STSTL)
      UWRITE_BYTE(E0CSR, 0);
      gEp0Status.bEpState = EP_IDLE;      // ep0 state to idle
   }

   // Handle incoming packet
   if (bCsr1 & rbOPRDY)
   {
      // Read the 8-byte command from Endpoint0 FIFO
      FIFORead(0, 8, (BYTE*)&gEp0Command);

      // Byte-swap the wIndex field
      bTemp = gEp0Command.wIndex.c[1];
      gEp0Command.wIndex.c[1] = gEp0Command.wIndex.c[0];
      gEp0Command.wIndex.c[0] = bTemp;

      // Byte-swap the wValue field
      bTemp = gEp0Command.wValue.c[1];
      gEp0Command.wValue.c[1] = gEp0Command.wValue.c[0];
      gEp0Command.wValue.c[0] = bTemp;

      // Byte-swap the wLength field
      bTemp = gEp0Command.wLength.c[1];
      gEp0Command.wLength.c[1] = gEp0Command.wLength.c[0];
      gEp0Command.wLength.c[0] = bTemp;

      // Decode received command
      switch (gEp0Command.bmRequestType & CMD_MASK_COMMON)
      {
         case  CMD_STD_DEV_OUT:           // Standard device requests
            // Decode standard OUT request
            switch (gEp0Command.bRequest)
            {
               case SET_ADDRESS:
                  SetAddressRequest();
                  break;
               case SET_FEATURE:
                  SetFeatureRequest();
                  break;
               case CLEAR_FEATURE:
                  ClearFeatureRequest();
                  break;
               case SET_CONFIGURATION:
                  SetConfigurationRequest();
                  break;
               case SET_INTERFACE:
                  SetInterfaceRequest();
                  break;
               // All other OUT requests not supported
               case SET_DESCRIPTOR:
               default:
                  gEp0Status.bEpState = EP_ERROR;
                  break;
            }
            break;

         // Decode standard IN request
         case CMD_STD_DEV_IN:
            switch (gEp0Command.bRequest)
            {
               case GET_STATUS:
                  GetStatusRequest();
                  break;
               case GET_DESCRIPTOR:
                  GetDescriptorRequest();
                  break;
               case GET_CONFIGURATION:
                  GetConfigurationRequest();
                  break;
               case GET_INTERFACE:
                  GetInterfaceRequest();
                  break;
               // All other IN requests not supported
               case SYNCH_FRAME:
               default:
                  gEp0Status.bEpState = EP_ERROR;
                  break;
            }
            break;
         // All other requests not supported
         default:
            gEp0Status.bEpState = EP_ERROR;
      }

      // Write E0CSR according to the result of the serviced out packet
      bTemp = rbSOPRDY;
      if (gEp0Status.bEpState == EP_ERROR)
      {
         bTemp |= rbSDSTL;                // Error condition handled
                                          // with STALL
         gEp0Status.bEpState = EP_IDLE;   // Reset state to idle
      }

      UWRITE_BYTE(E0CSR, bTemp);
   }

   bTemp = 0;                             // Reset temporary variable

   // If state is transmit, call transmit routine
   if (gEp0Status.bEpState == EP_TX)
   {
      // Check the number of bytes ready for transmit
      // If less than the maximum packet size, packet will
      // not be of the maximum size
      if (gEp0Status.uNumBytes <= EP0_PACKET_SIZE)
      {
         uTxBytes = gEp0Status.uNumBytes;
         gEp0Status.uNumBytes = 0;        // update byte counter
         bTemp |= rbDATAEND;              // This will be the last
                                          // packet for this transfer
         gEp0Status.bEpState = EP_IDLE;   // Reset endpoint state
      }

      // Otherwise, transmit maximum-length packet
      else
      {
         uTxBytes = EP0_PACKET_SIZE;
         gEp0Status.uNumBytes -= EP0_PACKET_SIZE;// update byte counter
      }

      // Load FIFO
      FIFOWrite(0, uTxBytes, (BYTE*)gEp0Status.pData);

      // Update data pointer
      gEp0Status.pData = (BYTE*)gEp0Status.pData + uTxBytes;

      // Update Endpoint0 Control/Status register
      bTemp |= rbINPRDY;                  // Always transmit a packet
                                          // when this routine is called
                                          // (may be zero-length)

      UWRITE_BYTE(E0CSR, bTemp);          // Write to Endpoint0 Control/Status
   }

}

/*------------------------------------------------------------------*/

//--------------------------------------------------------------------
//  Standard Request Routines
//--------------------------------------------------------------------
//
// These functions should be called when an endpoint0 command has
// been received with a corresponding "bRequest" field.
//
// - Each routine performs all command field checking, and
//   modifies fields of the Ep0Status structure accordingly
//
// After a call to a standard request routine, the calling routine
// should check Ep0Status.bEpState to determine the required action
// (i.e., send a STALL for EP_ERROR, load the FIFO for EP_TX).
// For transmit status, the data pointer (Ep0Status.pData),
// and data length (Ep0Status.uNumBytes) are prepared before the
// standard request routine returns. The calling routine must write
// the data to the FIFO and handle all data transfer


void SetAddressRequest()
{
   BYTE          bEpState;

   // Index and Length fields must be zero
   // Device state must be default or addressed
   if ((gEp0Command.wIndex.i) || (gEp0Command.wLength.i) ||
   (gDeviceStatus.bDevState == DEV_CONFIG))
   {
      bEpState = EP_ERROR;
   }

   else
   {
      // Assign new function address
      UWRITE_BYTE(FADDR, gEp0Command.wValue.c[1]);
      if (gDeviceStatus.bDevState == DEV_DEFAULT &&
      gEp0Command.wValue.c[1] != 0)
      {
         gDeviceStatus.bDevState = DEV_ADDRESS;
      }
      if (gDeviceStatus.bDevState == DEV_ADDRESS &&
      gEp0Command.wValue.c[1] == 0)
      {
         gDeviceStatus.bDevState = DEV_ADDRESS;
      }
      bEpState = EP_IDLE;
   }
   gEp0Status.bEpState = bEpState;
}

/*------------------------------------------------------------------*/

void SetFeatureRequest()
{
   BYTE          bEpState;

   // Length field must be zero
   // Device state must be configured, or addressed with Command Index
   // field == 0
   if ((gEp0Command.wLength.i != 0) ||
   (gDeviceStatus.bDevState == DEV_DEFAULT) ||
   (gDeviceStatus.bDevState == DEV_ADDRESS && gEp0Command.wIndex.i != 0))
   {
      bEpState = EP_ERROR;
   }

   // Handle based on recipient
   switch(gEp0Command.bmRequestType & CMD_MASK_RECIP)
   {
      // Device Request - Return error as remote wakeup is not supported
      case CMD_RECIP_DEV:
         bEpState = EP_ERROR;
         break;

      // Endpoint Request
      case CMD_RECIP_EP:
         if (gEp0Command.wValue.i == ENDPOINT_HALT)
         {
            bEpState = HaltEndpoint(gEp0Command.wIndex.i);
            break;
         }
         else
         {
            bEpState = EP_ERROR;
            break;
         }
      default:
         bEpState = EP_ERROR;
         break;
   }
   gEp0Status.bEpState = bEpState;
}

/*------------------------------------------------------------------*/

void ClearFeatureRequest()
{
   BYTE          bEpState;

   // Length field must be zero
   // Device state must be configured, or addressed with Command Index
   // field == 0
   if ((gEp0Command.wLength.i != 0) || (gDeviceStatus.bDevState == DEV_DEFAULT) ||
   (gDeviceStatus.bDevState == DEV_ADDRESS && gEp0Command.wIndex.i != 0))
   {
      bEpState = EP_ERROR;
   }

   // Handle based on recipient
   switch(gEp0Command.bmRequestType & CMD_MASK_RECIP)
   {
      // Device Request
      case CMD_RECIP_DEV:
         // Remote wakeup not supported
         bEpState = EP_ERROR;
         break;

      // Endpoint Request
      case CMD_RECIP_EP:
         if (gEp0Command.wValue.i == ENDPOINT_HALT)
         {
            // Enable the selected endpoint.
            bEpState = EnableEndpoint(gEp0Command.wIndex.i);
            break;
         }
         else
         {
           bEpState = EP_ERROR;
           break;
         }
      default:
         bEpState = EP_ERROR;
         break;
   }
   gEp0Status.bEpState = bEpState;
}

/*------------------------------------------------------------------*/

void SetConfigurationRequest()
{
   BYTE          bEpState;

   // Index and Length fields must be zero
   // Device state must be addressed or configured
   if ((gEp0Command.wIndex.i) || (gEp0Command.wLength.i) ||
   (gDeviceStatus.bDevState == DEV_DEFAULT))
   {
      bEpState = EP_ERROR;
   }

   else
   {
      // Make sure assigned configuration exists
      if (gEp0Command.wValue.c[1] >
      gDescriptorMap.bStdDevDsc[std_bNumConfigurations])
      {
         bEpState = EP_ERROR;
      }

      // Handle zero configuration assignment
      else if  (gEp0Command.wValue.c[1] == 0)
         gDeviceStatus.bDevState = DEV_ADDRESS;

      // Select the assigned configuration
      else
         bEpState = SetConfiguration(gEp0Command.wValue.c[1]);
   }
   gEp0Status.bEpState = bEpState;
}


/*------------------------------------------------------------------*/

void SetInterfaceRequest()
{
   BYTE          bEpState;

   // Length field must be zero
   if ((gEp0Command.wLength.i) || (gDeviceStatus.bDevState != DEV_CONFIG))
      bEpState = EP_ERROR;

   else
   {
      // Check that target interface exists for this configuration
      if(gEp0Command.wIndex.i > gDeviceStatus.bNumInterf - 1)
         bEpState = EP_ERROR;

      else
      {
         // Get pointer to interface status structure
         pIfStatus = (PIF_STATUS)&gDeviceStatus.IfStatus;

         // Check that alternate setting exists for the interface
         if (gEp0Command.wValue.i > pIfStatus->bNumAlts)
            bEpState = EP_ERROR;

         // Assign alternate setting
         else
         {
            pIfStatus->bCurrentAlt = gEp0Command.wValue.i;
            bEpState = SetInterface(pIfStatus);
         }
      }
   }
   gEp0Status.bEpState = bEpState;
}

/*------------------------------------------------------------------*/

void GetStatusRequest()
{
   BYTE          bEpState;

   // Value field must be zero; Length field must be 2
   if ((gEp0Command.wValue.i != 0) || (gEp0Command.wLength.i != 0x02) ||
   (gDeviceStatus.bDevState == DEV_DEFAULT) ||
   (gDeviceStatus.bDevState == DEV_ADDRESS && gEp0Command.wIndex.i != 0))
   {
      bEpState = EP_ERROR;
   }

   else
   {
      // Check for desired status (device, interface, endpoint)
      switch (gEp0Command.bmRequestType & CMD_MASK_RECIP)
      {
         // Device
         case CMD_RECIP_DEV:
            // Index must be zero for a Device status request
            if (gEp0Command.wIndex.i != 0)
               bEpState = EP_ERROR;
            else
            {
               // Prepare data_out for transmission
               gEp0Status.wData.c[1] = 0;
               gEp0Status.wData.c[0] = gDeviceStatus.bRemoteWakeupStatus;
               gEp0Status.wData.c[0] |= gDeviceStatus.bSelfPoweredStatus;
            }
            break;

         // Interface
         case CMD_RECIP_IF:
            // Prepare data_out for transmission
            gEp0Status.wData.i = 0;
            break;

         // Endpoint
         case CMD_RECIP_EP:
            // Prepare data_out for transmission
            gEp0Status.wData.i = 0;
            if (GetEpStatus(gEp0Command.wIndex.i) == EP_HALTED)
               gEp0Status.wData.c[0] |= 0x01;
            break;

         // Other cases unsupported
         default:
            bEpState = EP_ERROR;
            break;
      }

      // Endpoint0 state assignment
      bEpState = EP_TX;

      // Point ep0 data pointer to transmit data_out
      gEp0Status.pData = (BYTE *)&gEp0Status.wData.i;
      gEp0Status.uNumBytes = 2;
   }
   gEp0Status.bEpState = bEpState;
}

/*------------------------------------------------------------------*/

void GetDescriptorRequest()
{
   WORD wTempInt;
   UINT uNumBytes;
   BYTE bEpState;

   // This request is valid in all device states
   // Switch on requested descriptor (Value field)
   switch (gEp0Command.wValue.c[0])
   {
      // Device Descriptor Request
      case DSC_DEVICE:
         // Get size of the requested descriptor
         uNumBytes = STD_DSC_SIZE;
         // Prep to send the requested length
         if (uNumBytes > gEp0Command.wLength.i)
         {
            uNumBytes = gEp0Command.wLength.i;
         }
         // Point data pointer to the requested descriptor
         gEp0Status.pData = (void*)&gDescriptorMap.bStdDevDsc;
         bEpState = EP_TX;
         break;

      // Configuration Descriptor Request
      case DSC_CONFIG:
         // Make sure requested descriptor exists
         if (gEp0Command.wValue.c[1] >
         gDescriptorMap.bStdDevDsc[std_bNumConfigurations])
         {
            bEpState = EP_ERROR;
         }
         else
         {
            // Get total length of this configuration descriptor
            // (includes all associated interface and endpoints)
            wTempInt.c[1] = gDescriptorMap.bCfg1[cfg_wTotalLength_lsb];
            wTempInt.c[0] = gDescriptorMap.bCfg1[cfg_wTotalLength_msb];
            uNumBytes = wTempInt.i;

            // Prep to transmit the requested length
            if (uNumBytes > gEp0Command.wLength.i)
            {
               uNumBytes = gEp0Command.wLength.i;
            }
            // Point data pointer to requested descriptor
            gEp0Status.pData = &gDescriptorMap.bCfg1;
            bEpState = EP_TX;
         }
         break;

      // String Descriptor Request
      case DSC_STRING:
         gEp0Status.pData = (void*)StringDescTable[gEp0Command.wValue.c[1]];
         uNumBytes = *((BYTE *)gEp0Status.pData);
         bEpState = EP_TX;
         break;

   }
   gEp0Status.uNumBytes = uNumBytes;
   gEp0Status.bEpState = bEpState;
}

/*------------------------------------------------------------------*/

void GetConfigurationRequest()
{
   BYTE          bEpState;

   // Length field must be 1; Index field must be 0;
   // Value field must be 0
   if ((gEp0Command.wLength.i != 1) || (gEp0Command.wIndex.i) ||
   (gEp0Command.wValue.i) || (gDeviceStatus.bDevState == DEV_DEFAULT))
   {
      bEpState = EP_ERROR;
   }

   else if (gDeviceStatus.bDevState == DEV_ADDRESS)
   {
      // Prepare data_out for transmission
      gEp0Status.wData.i = 0;
      // Point ep0 data pointer to transmit data_out
      gEp0Status.pData = (BYTE *)&gEp0Status.wData.i;
      // ep0 state assignment
      bEpState = EP_TX;
   }

   else
   {
      // Index to desired field
      gEp0Status.pData = (void *)&gDescriptorMap.bCfg1[cfg_bConfigurationValue];

      // ep0 state assignment
      bEpState = EP_TX;
   }
   gEp0Status.uNumBytes = 1;
   gEp0Status.bEpState = bEpState;
}

/*------------------------------------------------------------------*/

void GetInterfaceRequest()
{
   BYTE          bEpState;

   // Value field must be 0; Length field must be 1
   if ((gEp0Command.wValue.i) || (gEp0Command.wLength.i != 1) ||
   (gDeviceStatus.bDevState != DEV_CONFIG))
   {
      bEpState = EP_ERROR;
   }

   else
   {
      // Make sure requested interface exists
      if (gEp0Command.wIndex.i > gDeviceStatus.bNumInterf - 1)
         bEpState = EP_ERROR;
      else
      {
         // Get current interface setting
         gEp0Status.pData = (void *)&gDeviceStatus.IfStatus->bCurrentAlt;

         // Length must be 1
         gEp0Status.uNumBytes = 1;
         bEpState = EP_TX;
      }
   }
   gEp0Status.bEpState = bEpState;
}

/*------------------------------------------------------------------*/

BYTE HaltEndpoint(UINT uEp)
{
   BYTE bReturnState, bIndex;

   // Save current INDEX value and target selected endpoint
   UREAD_BYTE (INDEX, bIndex);
   UWRITE_BYTE (INDEX, (BYTE)uEp & 0x00EF);

   // Halt selected endpoint and update its status flag
   switch (uEp)
   {
      case EP1_IN:
         UWRITE_BYTE (EINCSRL, rbInSDSTL);
         gEp1InStatus.bEpState = EP_HALTED;
         bReturnState = EP_IDLE;          // Return success flag
         break;
      case EP2_OUT:
         UWRITE_BYTE (EOUTCSRL, rbOutSDSTL);
         gEp2OutStatus.bEpState = EP_HALTED;
         bReturnState = EP_IDLE;          // Return success flag
         break;
      default:
         bReturnState = EP_ERROR;         // Return error flag
                                          // if endpoint not found
         break;
   }

   UWRITE_BYTE (INDEX, bIndex);           // Restore saved INDEX
   return bReturnState;
}

/*------------------------------------------------------------------*/

BYTE EnableEndpoint(UINT uEp)
{
   BYTE bReturnState, bIndex;

   // Save current INDEX value and target selected endpoint
   UREAD_BYTE (INDEX, bIndex);
   UWRITE_BYTE (INDEX, (BYTE)uEp & 0x00EF);

   // Flag selected endpoint has HALTED
   switch (uEp)
   {
      case EP1_IN:
         // Disable STALL condition and clear the data toggle
         UWRITE_BYTE (EINCSRL, rbInCLRDT);
         gEp1InStatus.bEpState = EP_IDLE; // Return success
         bReturnState = EP_IDLE;
         break;
      case EP2_OUT:
         // Disable STALL condition and clear the data toggle
         UWRITE_BYTE (EOUTCSRL, rbOutCLRDT);
         gEp2OutStatus.bEpState = EP_IDLE;// Return success
         bReturnState = EP_IDLE;
         break;
      default:
         bReturnState = EP_ERROR;         // Return error
                                          // if no endpoint found
         break;
   }

   UWRITE_BYTE (INDEX, bIndex);           // Restore INDEX

   return bReturnState;
}

/*------------------------------------------------------------------*/

BYTE GetEpStatus(UINT uEp)
{
   BYTE bReturnState;

   // Get selected endpoint status
   switch (uEp)
   {
      case EP1_IN:
         bReturnState = gEp1InStatus.bEpState;
         break;
      case EP2_OUT:
         bReturnState = gEp2OutStatus.bEpState;
         break;
      default:
         bReturnState = EP_ERROR;
         break;
   }

   return bReturnState;
}

/*------------------------------------------------------------------*/

BYTE SetConfiguration(BYTE SelectConfig)
{
   BYTE bReturnState = EP_IDLE;           // Endpoint state return value

   PIF_STATUS pIfStatus;                  // Pointer to interface status
                                          // structure

   // Store address of selected config desc
   gDeviceStatus.pConfig = &gDescriptorMap.bCfg1;

   // Confirm that this configuration descriptor matches the requested
   // configuration value
   if (gDeviceStatus.pConfig[cfg_bConfigurationValue] != SelectConfig)
   {
      bReturnState = EP_ERROR;
   }

   else
   {
      // Store number of interfaces for this configuration
      gDeviceStatus.bNumInterf = gDeviceStatus.pConfig[cfg_bNumInterfaces];

      // Store total number of interface descriptors for this configuration
      gDeviceStatus.bTotalInterfDsc = MAX_IF;

      // Get pointer to the interface status structure
      pIfStatus = (PIF_STATUS)&gDeviceStatus.IfStatus[0];

      // Build Interface status structure for Interface0
      pIfStatus->bIfNumber = 0;           // Set interface number
      pIfStatus->bCurrentAlt = 0;         // Select alternate number zero
      pIfStatus->bNumAlts = 0;            // No other alternates

      SetInterface(pIfStatus);            // Configure Interface0, Alternate0

      gDeviceStatus.bDevState = DEV_CONFIG;// Set device state to configured
      gDeviceStatus.bCurrentConfig = SelectConfig;// Store current config
   }

   return bReturnState;
}

/*------------------------------------------------------------------*/

BYTE SetInterface(PIF_STATUS pIfStatus)
{
   BYTE bReturnState = EP_IDLE;
   BYTE bIndex;

   // Save current INDEX value
   UREAD_BYTE (INDEX, bIndex);

   // Add actions for each possible interface alternate selections
   switch(pIfStatus->bIfNumber)
   {
      // Configure endpoints for interface0
      case 0:
         // Configure Endpoint1 IN
         UWRITE_BYTE(INDEX, 1);           // Index to Endpoint1 registers
         UWRITE_BYTE(EINCSRH, 0x20);      // FIFO split disabled,
                                          // direction = IN
         UWRITE_BYTE(EOUTCSRH, 0);        // Double-buffering disabled
         gEp1InStatus.uNumBytes = 0;      // Reset byte counter
         gEp1InStatus.uMaxP = EP1_PACKET_SIZE;// Set maximum packet size
         gEp1InStatus.bEp = EP1_IN;       // Set endpoint address
         gEp1InStatus.bEpState = EP_IDLE; // Set endpoint state

         // Endpoint2 OUT
         UWRITE_BYTE(INDEX, 2);           // Index to Endpoint2 registers
         UWRITE_BYTE(EINCSRH, 0x00);      // FIFO split disabled,
                                          // direction = OUT
         gEp2OutStatus.uNumBytes = 0;     // Reset byte counter
         gEp2OutStatus.uMaxP = EP2_PACKET_SIZE;// Set maximum packet size
         gEp2OutStatus.bEp = EP2_OUT;     // Set endpoint number
         gEp2OutStatus.bEpState = EP_IDLE;// Set endpoint state

         // Load first outgoing (IN) packet into FIFO
//##!!!         BulkOrInterruptIn(&gEp1InStatus);

         UWRITE_BYTE(INDEX, 0);           // Return to index 0

         break;

      // Configure endpoints for interface1
      case 1:

      // Configure endpoints for interface2
      case 2:

      // Default (error)
      default:
         bReturnState = EP_ERROR;
   }
   UWRITE_BYTE (INDEX, bIndex);           // Restore INDEX

   return bReturnState;
}

/*------------------------------------------------------------------*/

void BulkOrInterruptOut(PEP_STATUS pEpOutStatus)
{
   UINT uBytes;
   BYTE bTemp = 0;
   BYTE bCsrL, bCsrH;

   UWRITE_BYTE(INDEX, pEpOutStatus->bEp); // Index to current endpoint
   UREAD_BYTE(EOUTCSRL, bCsrL);
   UREAD_BYTE(EOUTCSRH, bCsrH);

   // Make sure this endpoint is not halted
   if (pEpOutStatus->bEpState != EP_HALTED)
   {
      // Handle STALL condition sent
      if (bCsrL & rbOutSTSTL)
      {
         // Clear Send Stall, Sent Stall, and data toggle
         UWRITE_BYTE(EOUTCSRL, rbOutCLRDT);
      }

      // Read received packet(s)
      // If double-buffering is enabled, multiple packets may be ready
      while(bCsrL & rbOutOPRDY)
      {
         // Get packet length
         UREAD_BYTE(EOUTCNTL, bTemp);     // Low byte
         uBytes = (UINT)bTemp & 0x00FF;

         UREAD_BYTE(EOUTCNTH, bTemp);     // High byte
         uBytes |= (UINT)bTemp << 8;

         if (uBytes <= EP2_PACKET_SIZE)
         {
            // Read FIFO
            FIFORead(pEpOutStatus->bEp, uBytes, (BYTE*)prx_buf);
            pEpOutStatus->uNumBytes = uBytes;

            // Signal new data
            *pn_rx = uBytes;
         }

         // If a data packet of anything but 8 bytes is received, ignore
         // and flush the FIFO
         else
         {
            UWRITE_BYTE(EOUTCSRL, rbOutFLUSH);
         }

         // Clear out-packet-ready
         UWRITE_BYTE(INDEX, pEpOutStatus->bEp);
         UWRITE_BYTE(EOUTCSRL, 0);

         // Read updated status register
         UREAD_BYTE(EOUTCSRL, bCsrL);
      }
   }
}

/*------------------------------------------------------------------*/

void BulkOrInterruptIn(PEP_STATUS pEpInStatus)
{
   BYTE bCsrL, bCsrH;

   UWRITE_BYTE(INDEX, pEpInStatus->bEp);  // Index to current endpoint
   UREAD_BYTE(EINCSRL, bCsrL);
   UREAD_BYTE(EINCSRH, bCsrH);

   // Make sure this endpoint is not halted
   if (pEpInStatus->bEpState != EP_HALTED)
   {
      // Handle STALL condition sent
      if (bCsrL & rbInSTSTL)
      {
         UWRITE_BYTE(EINCSRL, rbInCLRDT); // Clear Send Stall and Sent Stall,
                                          // and clear data toggle
      }

      // If a FIFO slot is open, write a new packet to the IN FIFO
      if (!(bCsrL & rbInINPRDY))
      {
         led_blink(0, 1, 300);

         pEpInStatus->uNumBytes = 8;
         pEpInStatus->pData = (BYTE*)prx_buf;

         // Write <uNumBytes> bytes to the <bEp> FIFO
         FIFOWrite(pEpInStatus->bEp, pEpInStatus->uNumBytes,
            (BYTE*)pEpInStatus->pData);

         // Set Packet Ready bit (INPRDY)
         UWRITE_BYTE(EINCSRL, rbInINPRDY);

         // Check updated endopint status
         UREAD_BYTE(EINCSRL, bCsrL);
      }
   }
}

/*------------------------------------------------------------------*/

void usb_send(unsigned char *buffer, unsigned char size)
{
   PEP_STATUS pEpInStatus;
   BYTE bCsrL, bCsrH;

   pEpInStatus = &gEp1InStatus;
   UWRITE_BYTE(INDEX, pEpInStatus->bEp);  // Index to current endpoint
   UREAD_BYTE(EINCSRL, bCsrL);
   UREAD_BYTE(EINCSRH, bCsrH);

   // Make sure this endpoint is not halted
   if (pEpInStatus->bEpState != EP_HALTED)
   {
      // Handle STALL condition sent
      if (bCsrL & rbInSTSTL)
      {
         UWRITE_BYTE(EINCSRL, rbInCLRDT); // Clear Send Stall and Sent Stall,
                                          // and clear data toggle
      }

      // If a FIFO slot is open, write a new packet to the IN FIFO
      if (!(bCsrL & rbInINPRDY))
      {
         // If data available, copy it to IN FIFO
         if (size > 0) {
            pEpInStatus->uNumBytes = size;
            pEpInStatus->pData = (BYTE*)buffer;

            // Write <uNumBytes> bytes to the <bEp> FIFO
            FIFOWrite(pEpInStatus->bEp, pEpInStatus->uNumBytes,
               (BYTE*)pEpInStatus->pData);

            // Set Packet Ready bit (INPRDY)
            UWRITE_BYTE(EINCSRL, rbInINPRDY);

            // Check updated endopint status
            UREAD_BYTE(EINCSRL, bCsrL);
         }
      }
   }
}

/*------------------------------------------------------------------*/

/*
   Read from the selected endpoint FIFO

   Inputs:
     bEp: target endpoint
     uNumBytes: number of bytes to unload
     pData: read data destination
*/
void FIFORead(BYTE bEp, UINT uNumBytes, BYTE * pData)
{
   BYTE TargetReg;
   UINT i;

   // If >0 bytes requested,
   if (uNumBytes)
   {
      TargetReg = FIFO_EP0 + bEp;         // Find address for target
                                          // endpoint FIFO

      USB0ADR = (TargetReg & 0x3F);       // Set address (mask out bits7-6)
      USB0ADR |= 0xC0;                    // Set auto-read and initiate
                                          // first read

      // Unload <NumBytes - 1> from the selected FIFO
      for(i=0; i<(uNumBytes-1); i++)
      {
         while(USB0ADR & 0x80);           // Wait for BUSY->'0' (data ready)
         pData[i] = USB0DAT;              // Copy data byte
      }

      // Disable auto read and copy last byte
      USB0ADR = (TargetReg & 0x3F);       // Set address (mask out bits7-6)
      while(USB0ADR & 0x80);              // Wait for BUSY->'0' (data ready)
      pData[i] = USB0DAT;                 // Copy final data byte

  }
}

/*------------------------------------------------------------------*/

/*
   Write to the selected endpoint FIFO

   Inputs:
     bEp: target endpoint
     uNumBytes: number of bytes to write
     pData: location of source data
*/
void FIFOWrite(BYTE bEp, UINT uNumBytes, BYTE * pData) reentrant
{
   BYTE TargetReg;
   UINT i;

   // If >0 bytes requested,
   if (uNumBytes)
   {
      TargetReg = FIFO_EP0 + bEp;         // Find address for target
                                          // endpoint FIFO

      while(USB0ADR & 0x80);              // Wait for BUSY->'0'
                                          // (register available)
      USB0ADR = (TargetReg & 0x3F);       // Set address (mask out bits7-6)

      // Write <NumBytes> to the selected FIFO
      for(i=0;i<uNumBytes;i++)
      {
         USB0DAT = pData[i];
         while(USB0ADR & 0x80);           // Wait for BUSY->'0' (data ready)
      }
   }
}
