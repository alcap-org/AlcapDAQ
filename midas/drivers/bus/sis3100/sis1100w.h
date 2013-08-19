
struct SIS1100_Device_Struct {
	HANDLE              plx_hDevice ;
	VIRTUAL_ADDRESSES   plx_Va;
} ;


int EXPORT sis1100w_Find_No_Of_sis1100(U32* no_of_sis1100_devices) ;
int EXPORT sis1100w_Get_PciSlot_Information(U32 sis1100_device_no, U32* pci_bus, U32* pci_slot) ;


int EXPORT sis1100w_Get_Handle_And_Open(U32 sis1100_device_no,  struct SIS1100_Device_Struct* sis1100_Device) ;
int EXPORT sis1100w_Close(struct SIS1100_Device_Struct* sis1100_Device) ;

int EXPORT sis1100w_Init(struct SIS1100_Device_Struct* sis1100_Device, int logfile_flag) ;
int EXPORT sis1100w_Init_sis3100(struct SIS1100_Device_Struct* sis1100_Device, int logfile_flag) ;

int EXPORT sis1100w_Get_IdentVersion(struct SIS1100_Device_Struct* sis1100_Device, U32* sis1100_IdentVersion) ;


// SIS1100 register access
int EXPORT sis1100w_sis1100_Control_Read(struct SIS1100_Device_Struct* sis1100_Device, U32 offset, U32* data) ;
int EXPORT sis1100w_sis1100_Control_Write(struct SIS1100_Device_Struct* sis1100_Device, U32 offset, U32 data) ;


// SIS3100 register access
int EXPORT sis1100w_sis3100_Control_Read(struct SIS1100_Device_Struct* sis1100_Device, U32 offset, U32* data) ;
int EXPORT sis1100w_sis3100_Control_Write(struct SIS1100_Device_Struct* sis1100_Device, U32 offset, U32 data) ;


// SIS3100 VME access

int EXPORT sis1100w_VmeSysreset(struct SIS1100_Device_Struct* sis1100_Device)  ;



int EXPORT sis1100w_Vme_Single_Read(struct SIS1100_Device_Struct* sis1100_Device, U32 addr, U32 am, U32 size,  U32* data) ;
int EXPORT sis1100w_Vme_Single_Write(struct SIS1100_Device_Struct* sis1100_Device, U32 addr, U32 am, U32 size, U32 data) ;

int EXPORT sis1100w_Vme_Dma_Read(struct SIS1100_Device_Struct* sis1100_Device, U32 addr, U32 am, U32 size, U32 fifo_mode,
								 U32* dmabufs, U32 req_num_data, U32* got_num_data) ;

int EXPORT sis1100w_Vme_Dma_Write(struct SIS1100_Device_Struct* sis1100_Device, U32 addr, U32 am, U32 size, U32 fifo_mode,
								 U32* dmabufs, U32 req_num_data, U32* put_num_data) ;




int EXPORT sis1100w_SdramSharc_Single_Read(struct SIS1100_Device_Struct* sis1100_Device, U32 addr, U32* data)  ;
int EXPORT sis1100w_SdramSharc_Single_Write(struct SIS1100_Device_Struct* sis1100_Device, U32 addr, U32 data)  ;


int EXPORT sis1100w_SdramSharc_Dma_Read(struct SIS1100_Device_Struct* sis1100_Device, U32 addr, 
								 U32* dmabufs, U32 req_num_data, U32* got_num_data)  ;

int EXPORT sis1100w_SdramSharc_Dma_Write(struct SIS1100_Device_Struct* sis1100_Device, U32 addr, 
								 U32* dmabufs, U32 req_num_data, U32* put_num_data) ; 


