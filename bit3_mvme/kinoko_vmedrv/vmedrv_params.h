/* vmedrv_params.h */
/* VME device driver for Bit3 Model 617 on Linux 2.0.x */
/* Created by Enomoto Sanshiro on 28 November 1999. */
/* Last updated by Enomoto Sanshiro on 19 January 2007. */


#ifndef __VMEDRV_PARAMS_H__
#define __VMEDRV_PARAMS_H__

#include <linux/pci.h>

enum vmedrv_minor_id_t {
    idANY = 0,
    idA16D16 = 1,
    idA16D32 = 2,
    idA24D16 = 3,
    idA24D32 = 4,
    idA32D16 = 5,
    idA32D32 = 6,
    idA24D16DMA = 7,
    idA24D32DMA = 8,
    idA32D16DMA = 9,
    idA32D32DMA = 10,
    idA24D16NBDMA = 11,
    idA24D32NBDMA = 12,
    idA32D16NBDMA = 13,
    idA32D32NBDMA = 14,
    vmedrv_NUMBER_OF_MINOR_IDS
};

static const int minor_to_access_mode[] = {
    /* idANY    */  VMEDRV_A32D32,
    /* idA16D16 */  VMEDRV_A16D16,
    /* idA16D32 */  VMEDRV_A16D32,
    /* idA24D16 */  VMEDRV_A24D16,
    /* idA24D32 */  VMEDRV_A24D32,
    /* idA32D16 */  VMEDRV_A32D16,
    /* idA32D32 */  VMEDRV_A32D32,
    /* idA24D16DMA */  VMEDRV_A24D16,
    /* idA24D32DMA */  VMEDRV_A24D32,
    /* idA32D16DMA */  VMEDRV_A32D16,
    /* idA32D32DMA */  VMEDRV_A32D32,
    /* idA24D16NBDMA */  VMEDRV_A24D16,
    /* idA24D32NBDMA */  VMEDRV_A24D32,
    /* idA32D16NBDMA */  VMEDRV_A32D16,
    /* idA32D32NBDMA */  VMEDRV_A32D32,
};

static const int minor_to_transfer_method[] = {
    /* idANY    */  VMEDRV_PIO,
    /* idA16D16 */  VMEDRV_PIO,
    /* idA16D32 */  VMEDRV_PIO,
    /* idA24D16 */  VMEDRV_PIO,
    /* idA24D32 */  VMEDRV_PIO,
    /* idA32D16 */  VMEDRV_PIO,
    /* idA32D32 */  VMEDRV_PIO,
    /* idA24D16DMA */  VMEDRV_DMA,
    /* idA24D32DMA */  VMEDRV_DMA,
    /* idA32D16DMA */  VMEDRV_DMA,
    /* idA32D32DMA */  VMEDRV_DMA,
    /* idA24D16NBDMA */  VMEDRV_NBDMA,
    /* idA24D32NBDMA */  VMEDRV_NBDMA,
    /* idA32D16NBDMA */  VMEDRV_NBDMA,
    /* idA32D32NBDMA */  VMEDRV_NBDMA,
};


enum bit3_vendor_id_t {
    viSBS = 0x108a,
};
enum bit3_device_id_t {
    di616 = 0x0003,
    di617 = 0x0001,
    di618 = 0x0010,
};

static const struct pci_device_id vmedrv_device_id_table[] = {
    { PCI_DEVICE(viSBS, di616), .driver_data = 0, },
    { PCI_DEVICE(viSBS, di617), .driver_data = 1, },
    { PCI_DEVICE(viSBS, di618), .driver_data = 2, },
    { }
};
static const char* vmedrv_model_name_table[] = {
    "SBS(Bit3) Model 616 VME-PCI Bus Adapter",
    "SBS(Bit3) Model 617 VME-PCI Bus Adapter",
    "SBS(Bit3) Model 618/620 VME-PCI Bus Adapter",
    NULL
};

enum bit3_parameters_t {
    bit3_IO_NODE_IO_BASE_INDEX = 0,
    bit3_IO_NODE_IO_SIZE = 32,
    bit3_MAPPED_NODE_IO_BASE_INDEX = 1,
    bit3_MAPPED_NODE_IO_SIZE = 32,
    bit3_MAPPING_REGISTERS_BASE_INDEX = 2,
    bit3_MAPPING_REGISTERS_SIZE = 65536, /* 4 [byte] * [(8+4+4) k regs] */
    bit3_MAPPING_REGISTER_WIDTH = 4,
    bit3_WINDOW_REGION_BASE_INDEX = 3,
    bit3_WINDOW_REGION_SIZE = 0x02000000,  /* 32MB */
    bit3_WINDOW_SIZE = 0x1000,             /* 4kB */
    bit3_NUMBER_OF_WINDOWS = 8192,
    bit3_DMA_MAPPING_REGISTERS_BASE_OFFSET = 0x0000c000,
    bit3_DMA_MAPPING_REGISTER_WIDTH = 4,
    bit3_DMA_MAPPING_SIZE = 0x01000000,    /* 16MB */
    bit3_DMA_WINDOW_SIZE = 0x1000,         /* 4kB */
    bit3_NUMBER_OF_DMA_WINDOWS = 4096,
    bit3_DMA_PACKET_SIZE = 256,
};

enum bit3_bitmasks_t {
    bit3_PAGE_BASE_MASK = 0xfffff000,
    bit3_PAGE_OFFSET_MASK = 0x00000fff,
    bit3_DMA_PAGE_BASE_MASK = 0xfffff000,
    bit3_DMA_PAGE_OFFSET_MASK = 0x00000fff,
    bit3_DMA_MAPPING_REGISTER_INDEX_SHIFT = 12,
    bit3_AM_MASK = 0x003f,
    bit3_AM_SHIFT = 6,
    bit3_FUNCTION_MASK = 0x0003,
    bit3_FUNCTION_SHIFT = 4,
    bit3_BYTESWAP_MASK = 0x0007,
    bit3_BYTESWAP_SHIFT = 1,
    bit3_DMA_BYTESWAP_MASK = 0x0003,
    bit3_DMA_BYTESWAP_SHIFT = 1,
};

enum data_width_t {
    dwBYTE = 1,
    dwWORD = 2,
    dwLONG = 4,
};

enum transfer_directions_t {
    tdREAD = 1,
    tdWRITE = 2,
};

enum transfer_methods_t {
    tmPIO = 1,
    tmDMA = 2,
    tmNBDMA = 3,
};

enum address_modifiers_t {
    amA16 = 0x29,
    amA24DATA = 0x39,
    amA24PROGRAM = 0x3a,
    amA24BLOCK = 0x3b,
    amA32DATA = 0x09,
    amA32PROGRAM = 0x0a,
    amA32BLOCK = 0x0b,
    amINVALID = 0xff,
};

enum function_codes_t {
    fcREMOTE_BUS_IO = 0x01,
    fcREMOTE_BUS_RAM = 0x02,
    fcREMOTE_DPM = 0x03,
};

enum byte_swapping_t {
    bsNONE = 0x00,
    bsENABLE_BYTE_SWAP_ON_NON_BYTE = 0x01,
    bsENABLE_WORD_SWAP = 0x02,
    bsENABLE_BYTE_SWAP_ON_BYTE = 0x04,
};

enum vme_parameters_t {
    vmeNUMBER_OF_IRQ_LINES = 8,
};


/* register offset addresses */

enum local_node_registers_t {
    regLOCAL_COMMAND = 0x00,
    regINTERRUPT_CONTROL = 0x01,
    regLOCAL_STATUS = 0x02,
    regINTERRUPT_STATUS = 0x03,
    regPCI_CONTROL = 0x04
};

enum remote_node_registers_t {
    regREMOTE_COMMAND_1 = 0x08,
    regREMOTE_COMMAND_2 = 0x09,
    regREMOTE_STATUS = 0x08,
    regADAPTER_ID = 0x0c,
    regADDRESS_MODIFIER = 0x0d,
    regIACK_READ = 0x0e,
    regIACK_READ_LOW = 0x0e,
    regIACK_READ_HIGH = 0x0f
};

enum local_dma_controller_registers_t {
    regDMA_COMMAND = 0x10,
    regDMA_REMAINDER_COUNT = 0x11,
    regDMA_PACKET_COUNT_0_7 = 0x12,
    regDMA_PACKET_COUNT_8_15 = 0x13,
    regDMA_PCI_ADDRESS_2_7 = 0x14,
    regDMA_PCI_ADDRESS_8_15 = 0x15,
    regDMA_PCI_ADDRESS_16_23 = 0x16
};

enum remote_dma_controller_registers_t {
    regDMA_REMOTE_REMAINDER_COUNT = 0x18,
    regDMA_VME_ADDRESS_16_23 = 0x1a,
    regDMA_VME_ADDRESS_24_31 = 0x1b,
    regDMA_VME_ADDRESS_0_7 = 0x1c,
    regDMA_VME_ADDRESS_8_15 = 0x1d,
    regDMA_SLAVE_STATUS = 0x1e
};


/* function of bits */

enum local_command_register_bits_t {
    lcCLEAR_STATUS = 0x80,
    lcCLEAR_PR_INTERRUPT = 0x40,
    lcSEND_PT_INTERRUPT = 0x20
};

enum interrupt_control_register_bits_t {
    icINTERRUPT_ACTIVE = 0x80,
    icNORMAL_INTERRUPT_ENABLE = 0x40,
    icERROR_INTERRUPT_ENABLE = 0x20,
    icPT_CINT_SEL2 = 0x04,
    icPT_CINT_SEL1 = 0x02,
    icPT_CINT_SEL0 = 0x01
};

enum local_status_register_bits_t {
    lsINTERFACE_PARITY_ERROR = 0x80,
    lsREMOTE_BUS_ERROR = 0x40,
    lsRECEIVING_PR_INTERRUPT = 0x20,
    lsINTERFACE_TIMEOUT = 0x04,
    lsLRC_ERROR = 0x02,
    lsREMOTE_BUS_POWER_OFF = 0x01,
    lsERROR_BITS = 0xe7
};

enum remote_command_1_register_bits_t {
    rcRESET_ADAPTER = 0x80,
    rcCLEAR_PT_INTERRUPT = 0x40,
    rcSEND_PR_INTERRUPT = 0x20,
    rcLOCK_VMEBUS = 0x10,
    rcIACK_ADDRESS_BIT_2 = 0x04,
    rcIACK_ADDRESS_BIT_1 = 0x02,
    rcIACK_ADDRESS_BIT_0 = 0x01
};

enum remote_command_2_register_bits_t {
    rcDMA_PAUSE_ON_16 = 0x80,
    rcBLOCK_MODE_DMA = 0x20,
    rcDISABLE_INTERRUPT_PASSING = 0x10,
};

enum remote_status_register_bits_t {
    rsVMEBUS_WAS_RESET = 0x80,
    rsIACK_ADDRESS_BIT_1 = 0x40,
    rsPR_WAS_SENT = 0x20,
    rsLOCK_BUS_NOT_SET = 0x10,
    rsIACK_ADDRESS_BIT_2 = 0x04,
    rsRECEIVING_PT_INTERRUPT = 0x02,
    rsIACK_ADDRESS_BIT_0 = 0x01
};

enum dma_command_register_bits_t {
    dcSTART_DMA = 0x80,
    dcDMA_DP = 0x40,
    dcDMA_TRANSFER_DIRECTION = 0x20,
    dcDMA_TRANSFER_DIRECTION_WRITE = 0x20,
    dcDMA_TRANSFER_DIRECTION_READ = 0x00,
    dcDMA_WORD_LONGWORD_SELECT = 0x10,
    dcDMA_WORD_LONGWORD_SELECT_LONGWORD = 0x10,
    dcDMA_WORD_LONGWORD_SELECT_WORD = 0x00,
    dcENABLE_DMA_DONE_INTERRUPT = 0x04,
    dcDMA_DONE_FLAG = 0x02,
    dcDMA_ACTIVE = 0x01
};


#endif
