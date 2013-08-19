// Device driver for direct IO access
// under Windows NT 4.0, c't 1/97 Matthias Witthopf, Andreas Stiller
// Cyrix and PCI stuff removed, driver renamed to directIO, Stefan Ritt

extern "C"
{
#include <ntddk.h>
#include <string.h>
#include <stdlib.h>
#include <devioctl.h>
#include <conio.h>
#include "directio.h"
}

#define DIRECTIO_DEVICE_NAME L"\\Device\\DirectIO"
#define DOS_DEVICE_NAME      L"\\DosDevices\\DirectIO"
#define IOPM_SIZE 0x2000

typedef UCHAR IOPMTYP[IOPM_SIZE];
typedef struct {
  ULONG Dummy;
  IOPMTYP iopm;
} TLocalDevInfo,* PLocalDevInfo;

// Structure of input parameters
typedef struct {
  ULONG OpCode;
  ULONG Par1;
  ULONG Par2;
  ULONG Par3;
} TDirectIOInfo,* PDirectIOInfo;

extern "C" void Ke386SetIoAccessMap     (int,IOPMTYP);
extern "C" void Ke386QueryIoAccessMap   (int,IOPMTYP);
extern "C" void Ke386IoSetAccessProcess (PEPROCESS, int);

__declspec(dllimport)  BOOLEAN MmIsAddressValid  (IN PVOID );
__declspec(dllimport)  PVOID   MmMapIoSpace  (PHYSICAL_ADDRESS,ULONG,BOOLEAN);
__declspec(dllimport)  VOID    MmUnmapIoSpace (PVOID,ULONG);

NTSTATUS DirectIO_CreateDevice(IN  PWSTR PrototypeName,
    IN  DEVICE_TYPE DeviceType, IN  PDRIVER_OBJECT DriverObject,
    OUT PDEVICE_OBJECT *ppDevObj)
{
  UNICODE_STRING NtDeviceName;
  UNICODE_STRING Win32DeviceName;
  RtlInitUnicodeString(&NtDeviceName,PrototypeName);
  NTSTATUS Status = IoCreateDevice(DriverObject,sizeof(TLocalDevInfo),
                            &NtDeviceName,DeviceType,0,FALSE,ppDevObj);
  if (!NT_SUCCESS(Status)) return Status;
  RtlZeroMemory((*ppDevObj)->DeviceExtension,sizeof(TLocalDevInfo));
  RtlInitUnicodeString(&Win32DeviceName,DOS_DEVICE_NAME);
  Status = IoCreateSymbolicLink(&Win32DeviceName,&NtDeviceName);
  if (!NT_SUCCESS(Status))
    IoDeleteDevice(*ppDevObj);
  return Status;
}

void SetIOPermissionFree(int OnFlag,int pstart,int pend,IOPMTYP iopm)
{
  Ke386QueryIoAccessMap(1,iopm);
  for (int j=pstart; j<=pend; j++)
    iopm[j/8] &= ~(1 << (j%8));
  Ke386IoSetAccessProcess(PsGetCurrentProcess(), OnFlag);
  Ke386SetIoAccessMap (1,iopm);
}

void SetIOPermissionLock(int OnFlag,int pstart,int pend,IOPMTYP iopm)
{
  Ke386QueryIoAccessMap(1,iopm);
  for (int j=pstart; j<=pend; j++)
    iopm[j/8] |= (1 << (j%8));
  Ke386IoSetAccessProcess(PsGetCurrentProcess(), OnFlag);
  Ke386SetIoAccessMap (1,iopm);
}

static NTSTATUS
ProbePCI(ULONG VendorID, ULONG DeviceID, UCHAR *Irq, ULONG *BaseAdr)
{
PCI_SLOT_NUMBER     slotNumber;
PPCI_COMMON_CONFIG  pciData;
UCHAR               buf[PCI_COMMON_HDR_LENGTH];
ULONG               i, f, j, bus;
BOOLEAN             flag;

  pciData = (PPCI_COMMON_CONFIG) buf;
  slotNumber.u.bits.Reserved = 0;

  flag = TRUE;
  for (bus = 0; flag; bus++)
    {
    for (i = 0; i < PCI_MAX_DEVICES  &&  flag; i++)
      {
      slotNumber.u.bits.DeviceNumber = i;

      for (f = 0; f < PCI_MAX_FUNCTION; f++)
        {
        slotNumber.u.bits.FunctionNumber = f;

        j = HalGetBusData(PCIConfiguration, bus, slotNumber.u.AsULONG,
                          pciData, PCI_COMMON_HDR_LENGTH);

        if (j == 0)
          {
          /* out of buses */
          flag = FALSE;
          break;
          }

        if (pciData->VendorID == PCI_INVALID_VENDORID)
          {
          /* skip to next slot */
          break;
          }

        /* check for vendor and device id, return base address */
        if (pciData->VendorID == VendorID && pciData->DeviceID == DeviceID)
          {
          *Irq       = pciData->u.type0.InterruptLine;
          BaseAdr[0] = pciData->u.type0.BaseAddresses[0];
          BaseAdr[1] = pciData->u.type0.BaseAddresses[1];
          BaseAdr[2] = pciData->u.type0.BaseAddresses[2];
          BaseAdr[3] = pciData->u.type0.BaseAddresses[3];
          BaseAdr[4] = pciData->u.type0.BaseAddresses[4];
          BaseAdr[5] = pciData->u.type0.BaseAddresses[5];
          return STATUS_SUCCESS;
          }
        }
      }
    }

  return STATUS_NO_SUCH_DEVICE;
}

NTSTATUS DirectIO_Control(IN PLocalDevInfo pLDI, IN PIRP pIrp,
   IN PIO_STACK_LOCATION IrpStack, IN ULONG IoctlCode)
{
  PDirectIOInfo InBuf=(PDirectIOInfo)pIrp->AssociatedIrp.SystemBuffer;
  ULONG InBufSize=IrpStack->Parameters.DeviceIoControl.InputBufferLength;
  ULONG OpCode=InBuf->OpCode;
  ULONG status, BaseAdr[6];
  UCHAR Irq;
  void *OutBuf=(void *)pIrp->AssociatedIrp.SystemBuffer;
  ULONG OutBufSize=IrpStack->Parameters.DeviceIoControl.OutputBufferLength;
  PVOID Linadr;
  PHYSICAL_ADDRESS PADDR;

  switch(OpCode) {

  case OP_Check:
    *(PULONG)OutBuf = 0x12345678;
    pIrp->IoStatus.Information = sizeof(ULONG);
    return STATUS_SUCCESS;

  case OP_GiveIO:
    SetIOPermissionFree(1,InBuf->Par1,InBuf->Par2,pLDI->iopm);
    pIrp->IoStatus.Information = 0;
    return STATUS_SUCCESS;

  case OP_LockIO:
    SetIOPermissionLock(1,InBuf->Par1,InBuf->Par2,pLDI->iopm);
    pIrp->IoStatus.Information = 0;
    return STATUS_SUCCESS;

  case OP_ReadPortByte:
    *(PUCHAR)OutBuf = READ_PORT_UCHAR((PUCHAR)InBuf->Par1);
    pIrp->IoStatus.Information = sizeof(UCHAR);
    return STATUS_SUCCESS;

  case OP_WritePortByte:
    WRITE_PORT_UCHAR((PUCHAR)InBuf->Par1,(UCHAR)InBuf->Par2);
    pIrp->IoStatus.Information = 0;
    return STATUS_SUCCESS;

/*  case OP_ReadMemDword:

  if (MmIsAddressValid)
     {
     *(PULONG)OutBuf = *(ULONG*) (InBuf->Par1);
     pIrp->IoStatus.Information = sizeof(ULONG);
     return STATUS_SUCCESS;
   }
  else
   {
   pIrp->IoStatus.Information = 0;
     return STATUS_ACCESS_VIOLATION;
     }
*/
  case OP_ReadPhysMemDword:
    PADDR.LowPart=(ULONG) (InBuf->Par1);
    PADDR.HighPart=0;         // Reserve für > 4GByte
    Linadr=MmMapIoSpace (PADDR,4,MmNonCached);
    *(PULONG)OutBuf = *(ULONG*) Linadr;
    MmUnmapIoSpace (Linadr,4);
    pIrp->IoStatus.Information = sizeof(ULONG);
    return STATUS_SUCCESS;

  case OP_WritePhysMemDword:
    PADDR.LowPart=(ULONG) (InBuf->Par1);
    PADDR.HighPart=0;         // Reserve für > 4GByte
    Linadr=MmMapIoSpace (PADDR,4,MmNonCached);
    *(ULONG*) Linadr = *(PULONG)InBuf->Par2;
    MmUnmapIoSpace (Linadr,4);
    pIrp->IoStatus.Information = sizeof(ULONG);
    return STATUS_SUCCESS;

  case OP_PCIInfo:
    status = ProbePCI(InBuf->Par1, InBuf->Par2, &Irq, BaseAdr);
    if (status == STATUS_NO_SUCH_DEVICE)
      pIrp->IoStatus.Information = 0;
    else
      {
      DbgPrint("DirectIO: BaseAdr0 = %X\n", BaseAdr[0]);
      DbgPrint("DirectIO: BaseAdr1 = %X\n", BaseAdr[1]);
      *(PULONG)OutBuf = (ULONG)Irq;
      *((PULONG)OutBuf+1) = BaseAdr[0];
      *((PULONG)OutBuf+2) = BaseAdr[1];
      *((PULONG)OutBuf+3) = BaseAdr[2];
      *((PULONG)OutBuf+4) = BaseAdr[3];
      *((PULONG)OutBuf+5) = BaseAdr[4];
      *((PULONG)OutBuf+6) = BaseAdr[5];
      pIrp->IoStatus.Information = 7*sizeof(ULONG);
      }
    return STATUS_SUCCESS;


  }
  return STATUS_INVALID_PARAMETER;
}



NTSTATUS DirectIO_Dispatch(IN PDEVICE_OBJECT pDO,IN PIRP pIrp)
{
  pIrp->IoStatus.Information = 0;  // Anzahl Rückgabe-Bytes
  PLocalDevInfo      pLDI      = (PLocalDevInfo)pDO->DeviceExtension;
  PIO_STACK_LOCATION pIrpStack = IoGetCurrentIrpStackLocation(pIrp);
  NTSTATUS           Status    = STATUS_NOT_IMPLEMENTED;

  switch(pIrpStack->MajorFunction) {
  case IRP_MJ_CREATE:
  case IRP_MJ_CLOSE:
    Status = STATUS_SUCCESS;
    break;
  case IRP_MJ_DEVICE_CONTROL:
    switch(pIrpStack->Parameters.DeviceIoControl.IoControlCode) {
      case IOCTL_DIRECTIO_CONTROL:
        Status = DirectIO_Control(pLDI,pIrp,pIrpStack,
          pIrpStack->Parameters.DeviceIoControl.IoControlCode);
        break;
      }
    break;
  }
  pIrp->IoStatus.Status = Status;
  IoCompleteRequest(pIrp,IO_NO_INCREMENT);
  return Status;
}

VOID DirectIO_Unload(PDRIVER_OBJECT DriverObject)
{ // Symbolischen Link wieder auflösen
  UNICODE_STRING Win32DeviceName;
  RtlInitUnicodeString(&Win32DeviceName,DOS_DEVICE_NAME);
  IoDeleteSymbolicLink(&Win32DeviceName);
  IoDeleteDevice(DriverObject->DeviceObject);
}

extern "C" NTSTATUS DriverEntry(IN PDRIVER_OBJECT DriverObject,
                                IN PUNICODE_STRING RegistryPath)
{ // Symbolischen Link erzeugen, so daß Win32 darauf zugreifen kann
  DriverObject->MajorFunction[IRP_MJ_CREATE]         = DirectIO_Dispatch;
  DriverObject->MajorFunction[IRP_MJ_CLOSE]          = DirectIO_Dispatch;
  DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = DirectIO_Dispatch;
  DriverObject->DriverUnload                         = DirectIO_Unload;
  PDEVICE_OBJECT DeviceObject;
  return DirectIO_CreateDevice(DIRECTIO_DEVICE_NAME, DIRECTIO_TYPE,
                               DriverObject,&DeviceObject);
}

