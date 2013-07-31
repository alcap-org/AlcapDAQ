// ****************************************************************************
// *     Filename:  v1290.h
// *  Description:  
// *   Created by:  Nam Tran (nam at kuno-g.phys.sci.osaka-u.ac.jp)
// *     Time tag:  2013-04-07 10:31:01
// ****************************************************************************
#ifndef V1290_7NX5IH0R
#define V1290_7NX5IH0R

#include "mvmestd.h"
#include <stdint.h>

#define V1290_AM MVME_AM_A32_ND
#define V1290_BUFF_DW MVME_DMODE_D32
#define V1290_REG_DW  MVME_DMODE_D16

typedef enum
{
	V1190_CH_PER_TDC = 32,
	V1290_CH_PER_TDC = 8,
} V1X90_CH_PER_TDC;

#define V1190_NUM_TDC_A		4
#define V1290_NUM_TDC_B		2
#define V1190_NUM_TDC_A		4
#define V1290_NUM_TDC_N		2

#define V1290_OUT_BUFFER 0x0000
// Register map
#define V1290_CONTROL					0x1000	
#define V1290_STATUS					0x1002	
#define V1290_INT_LEVEL					0x100A
#define V1290_INT_VECTOR				0x100C
#define V1290_GEORESS				0x100E
#define V1290_MCST_CBLTRESS			0x1010
#define V1290_MCST_CBLT_CTRL			0x1012	
#define V1290_MOD_RESET					0x1014		
#define V1290_SW_CLEAR					0x1016		
#define V1290_SW_EVENT_RESET			0x1018	
#define V1290_SW_TRIGGER				0x101A		
#define V1290_EVENT_COUNTER				0x101C	
#define V1290_EVENT_STORED				0x1020	
#define V1290_ALMOST_FULL_LVL			0x1022	
#define V1290_BLT_EVENT_NUM				0x1024	
#define V1290_FW_REV					0x1026			
#define V1290_TESTREG					0x1028			
#define V1290_OUT_PROG_CTRL				0x102C	
#define V1290_MICRO						0x102E			
#define V1290_MICRO_HND					0x1030		
#define V1290_SEL_FLASH					0x1032		
#define V1290_FLASH						0x1034			
#define V1290_COMP_SRAM_PAGE			0x1036	
#define V1290_EVENT_FIFO				0x1038		
#define V1290_EVENT_FIFO_STORED			0x103C
#define V1290_EVENT_FIFO_STATUS			0x103E
#define V1290_DUMMY32					0x1200			
#define V1290_DUMMY16					0x1204			
#define V1290_ROM_OUI_2					0x4024		
#define V1290_ROM_OUI_1					0x4028		
#define V1290_ROM_OUI_0					0x402C		
#define V1290_ROM_VERSION				0x4030		
#define V1290_ROM_BOARD_ID_2			0x4034	
#define V1290_ROM_BOARD_ID_1			0x4038	
#define V1290_ROM_BOARD_ID_0			0x403C	
#define V1290_ROM_REVISION_3			0x4040	
#define V1290_ROM_REVISION_2			0x4044	
#define V1290_ROM_REVISION_1			0x4048	
#define V1290_ROM_REVISION_0			0x404C	
#define V1290_ROM_SERIAL_1				0x4080	
#define V1290_ROM_SERIAL_0				0x4084	

// Micro register opcodes: ACQUISITION MODE
#define V1290_MICRO_WR_OK 0x0001
#define V1290_MICRO_RD_OK 0x0002
#define V1290_TRG_MATCH_OPCODE					 0x0000		
#define V1290_CONT_STORE_OPCODE					 0x0100		
#define V1290_READ_ACQ_MOD_OPCODE				 0x0200		
#define V1290_SET_KEEP_TOKEN_OPCODE				 0x0300	
#define V1290_CLEAR_KEEP_TOKEN_OPCODE			 0x0400	
#define V1290_LOAD_DEF_CONFIG_OPCODE			 0x0500	
#define V1290_SAVE_USER_CONFIG_OPCODE			 0x0600	
#define V1290_LOAD_USER_CONFIG_OPCODE			 0x0700	
#define V1290_AUTOLOAD_USER_CONFIG_OPCODE		 0x0800
#define V1290_AUTOLOAD_DEF_CONFIG_OPCODE		 0x0900
// Micro register opcodes: TRIGGER
#define V1290_SET_WIN_WIDTH_OPCODE				 0x1000	 
#define V1290_SET_WIN_OFFSET_OPCODE				 0x1100	 
#define V1290_SET_SW_MARGIN_OPCODE				 0x1200	 
#define V1290_SET_REJ_MARGIN_OPCODE				 0x1300	 
#define V1290_EN_SUB_TRG_OPCODE					 0x1400		
#define V1290_DIS_SUB_TRG_OPCODE				 0x1500	
#define V1290_READ_TRG_CONF_OPCODE				 0x1600	 
// Micro register opcodes: TDC EDGE DETECTION
#define V1290_SET_DETECTION_OPCODE				 0x2200	 
#define V1290_READ_DETECTION_OPCODE				 0x2300	 
#define V1290_SET_TR_LEAD_LSB_OPCODE			 0x2400	 
#define V1290_SET_PAIR_RES_OPCODE				 0x2500		
#define V1290_READ_RES_OPCODE					 0x2600		
#define V1290_SET_DEAD_TIME_OPCODE				 0x2800	 
#define V1290_READ_DEAD_TIME_OPCODE				 0x2900	 
// Micro register opcodes: TDC READOUT
#define V1290_EN_HEAD_TRAILER_OPCODE			 0x3000	 
#define V1290_DIS_HEAD_TRAILER_OPCODE			 0x3100	 
#define V1290_READ_HEAD_TRAILER_OPCODE			 0x3200
#define V1290_SET_EVENT_SIZE_OPCODE				 0x3300	 
#define V1290_READ_EVENT_SIZE_OPCODE			 0x3400	 
#define V1290_EN_ERROR_MARK_OPCODE				 0x3500	 
#define V1290_DIS_ERROR_MARK_OPCODE				 0x3600	 
#define V1290_EN_ERROR_BYPASS_OPCODE			 0x3700	 
#define V1290_DIS_ERROR_BYPASS_OPCODE			 0x3800	 
#define V1290_SET_ERROR_TYPES_OPCODE			 0x3900	 
#define V1290_READ_ERROR_TYPES_OPCODE			 0x3A00	 
#define V1290_SET_FIFO_SIZE_OPCODE				 0x3B00	 
#define V1290_READ_FIFO_SIZE_OPCODE				 0x3C00	 
// Micro register opcodes: CHANNEL ENABLE
#define V1290_EN_CHANNEL_OPCODE					 0x4000		
#define V1290_DIS_CHANNEL_OPCODE				 0x4100		
#define V1290_EN_ALL_CH_OPCODE					 0x4200		
#define V1290_DIS_ALL_CH_OPCODE					 0x4300		
#define V1290_WRITE_EN_PATTERN_OPCODE			 0x4400	 
#define V1290_READ_EN_PATTERN_OPCODE			 0x4500	 
#define V1290_WRITE_EN_PATTERN32_OPCODE			 0x4600
#define V1290_READ_EN_PATTERN32_OPCODE			 0x4700
// Micro register opcodes: ADJUST
#define V1290_SET_GLOB_OFFSET_OPCODE			 0x5000	 
#define V1290_READ_GLOB_OFFSET_OPCODE			 0x5100	 
#define V1290_SET_ADJUST_CH_OPCODE				 0x5200	 
#define V1290_READ_ADJUST_CH_OPCODE				 0x5300	 
#define V1290_SET_RC_ADJ_OPCODE					 0x5400		
#define V1290_READ_RC_ADJ_OPCODE				 0x5500		
#define V1290_SAVE_RC_ADJ_OPCODE				 0x5600		
// Micro register opcodes: MISCELLANEOUS
#define V1290_READ_TDC_ID_OPCODE				 0x6000		
#define V1290_READ_MICRO_REV_OPCODE				 0x6100	
#define V1290_RESET_DLL_PLL_OPCODE				 0x6200	
// Micro register opcodes: ADVANCED
#define V1290_WRITE_SETUP_REG_OPCODE			 0x7000	
#define V1290_READ_SETUP_REG_OPCODE				 0x7100	 
#define V1290_UPDATE_SETUP_REG_OPCODE			 0x7200	 
#define V1290_DEFAULT_SETUP_REG_OPCODE			 0x7300
#define V1290_READ_ERROR_STATUS_OPCODE			 0x7400
#define V1290_READ_DLL_LOCK_OPCODE				 0x7500	 
#define V1290_READ_STATUS_STREAM_OPCODE			 0x7600
#define V1290_UPDATE_SETUP_TDC_OPCODE			 0x7700	 
// Micro register opcodes: DEBUG AND TEST
#define V1290_WRITE_EEPROM_OPCODE				 0xC000		
#define V1290_READ_EEPROM_OPCODE				 0xC100		
#define V1290_MICROCONTROLLER_FW_OPCODE			 0xC200
#define V1290_WRITE_SPARE_OPCODE				 0xC300		
#define V1290_READ_SPARE_OPCODE					 0xC400		
#define V1290_EN_TEST_MODE_OPCODE				 0xC500		
#define V1290_DIS_TEST_MODE_OPCODE				 0xC600	
#define V1290_SET_TDC_TEST_OUTPUT_OPCODE		 0xC700
#define V1290_SET_DLL_CLOCK_OPCODE				 0xC800	
#define V1290_READ_TDC_SETUP_SCAN_PATH_OPCODE	 0xC800		

// Output buffer macros
#define V1290_DATA_TYPE_MASK				0xf8000000		
#define V1290_DATA_MASK							0x1fffff
#define V1290_CH_MASK								0x03e00000

#define V1290_GLOBAL_HEADER				0x40000000	
#define V1290_GLOBAL_TRAILER			0x80000000	
#define V1290_TDC_HEADER				0x08000000		
#define V1290_TDC_MEASURE				0x00000000		
#define V1290_TDC_ERROR					0x20000000		
#define V1290_TDC_TRAILER				0x18000000		
#define V1290_GLOBAL_TRIGGER_TIME		0x88000000
#define V1290_FILLER					0xc0000000		

#define V1290_GLB_HDR_MAX_EVENT_COUNT	(0x003fffff+ 1)
#define V1290_GLB_HDR_EVENT_COUNT_MSK	0x07ffffe0		
#define V1290_GLB_HDR_GEO_MSK			0x0000001f		
#define V1290_GLB_TRG_TIME_TAG_MSK		0x07ffffff
#define V1290_GLB_TRL_STATUS_MSK		0x07000000
#define V1290_GLB_TRL_WCOUNT_MSK		0x001fffe0
#define V1290_GLB_TRL_GEO_MSK			0x0000001f

#define V1290_TDC_HDR_TDC_MSK			0x03000000
#define V1290_TDC_HDR_EVENT_ID_MSK		0x00fff000
#define V1290_TDC_HDR_BUNCH_ID_MSK		0x00000fff
#define V1290_TDC_MSR_TRAILING_MSK		0x04000000
#define V1290_TDC_MSR_CHANNEL_MSK		0x03f80000
#define V1290_TDC_MSR_MEASURE_MSK		0x0007ffff
#define V1290_TDC_TRL_TDC_MSK			0x03000000
#define V1290_TDC_TRL_EVENT_ID_MSK		0x00fff000
#define V1290_TDC_TRL_WCOUNT_MSK		0x00000fff
#define V1290_TDC_ERR_TDC_MSK			0x03000000
#define V1290_TDC_ERR_ERR_FLAGS_MSK		0x00003fff

#define V1290_IS_GLOBAL_HEADER(data)		((data& V1290_DATA_TYPE_MASK)== V1290_GLOBAL_HEADER)			
#define V1290_IS_GLOBAL_TRAILER(data)		((data& V1290_DATA_TYPE_MASK)== V1290_GLOBAL_TRAILER)		
#define V1290_IS_TDC_HEADER(data)			((data& V1290_DATA_TYPE_MASK)== V1290_TDC_HEADER)			
#define V1290_IS_TDC_MEASURE(data)			((data& V1290_DATA_TYPE_MASK)== V1290_TDC_MEASURE)
#define V1290_IS_TDC_ERROR(data)			((data& V1290_DATA_TYPE_MASK)== V1290_TDC_ERROR)	
#define V1290_IS_TDC_TRAILER(data)			((data& V1290_DATA_TYPE_MASK)== V1290_TDC_TRAILER)
#define V1290_IS_GLOBAL_TRIGGER_TIME(data)	((data& V1290_DATA_TYPE_MASK)== V1290_GLOBAL_TRIGGER_TIME)	
#define V1290_IS_FILLER(data)				((data& V1290_DATA_TYPE_MASK)== V1290_FILLER)				
#define V1290_GET_TDC_MSR_CHANNEL(data)		((uint32_t)((((uint32_t)data)& V1290_TDC_MSR_CHANNEL_MSK)>>21))			
#define V1290_GET_TDC_MSR_MEASURE(data)		((uint32_t)(((uint32_t)data)& V1290_TDC_MSR_MEASURE_MSK))				

//Basic read/write
uint16_t v1290_Read16(MVME_INTERFACE *mvme, uint32_t base, int offset);
uint32_t v1290_Read32(MVME_INTERFACE *mvme, uint32_t base, int offset);
void v1290_Write16(MVME_INTERFACE *mvme, uint32_t base, 
		int offset, uint16_t value);
int v1290_EventRead(MVME_INTERFACE *mvme, uint32_t base, 
		uint32_t *pdest, int *nentry);
int  v1290_DataRead(MVME_INTERFACE *mvme, 
		uint32_t base, uint32_t *pdest, int nentry);

// Opcode related functions
int v1290_MicroWrite(MVME_INTERFACE *mvme, uint32_t base, uint16_t data);
void v1290_WriteMicro(MVME_INTERFACE *mvme, uint32_t base, 
		uint16_t opcode, uint16_t data);
int v1290_MicroRead(MVME_INTERFACE *mvme, const uint32_t base);
uint16_t v1290_ReadMicro(MVME_INTERFACE *mvme, const uint32_t base, 
		uint16_t opcode);

// Acq mode settings
void v1290_TriggerMatchingSet(MVME_INTERFACE *mvme, uint32_t base);
void v1290_ContinuousSet(MVME_INTERFACE *mvme, uint32_t base);
int v1290_AcqModeRead(MVME_INTERFACE *mvme, uint32_t base);
// Trigger matching mode related settings
int v1290_TriggerConfRead(MVME_INTERFACE *mvme, uint32_t base, uint16_t *conf);
void v1290_SetWindowWidth(MVME_INTERFACE *mvme, uint32_t base, uint16_t width);
void v1290_SetWindowOffset(MVME_INTERFACE *mvme, uint32_t base, int16_t offset);
void v1290_SetExtraMargin(MVME_INTERFACE *mvme, uint32_t base, uint16_t margin);
void v1290_SetRejectMargin(MVME_INTERFACE *mvme, uint32_t base, uint16_t margin);
void v1290_EnableTriggerSubtraction(MVME_INTERFACE *mvme, uint32_t base, bool en);
// TDC edge and resolution settings
// only 2 lsb bits matter (see 5.4 @ manual)
// edge code: 00-pair mode, 01-only trailing, 10-only leading, 11-both edges
// resolution code (for edges): 00-800 ps, 01-200 ps, 10-100 ps, 11-25 ns
void v1290_SetEdgeDetection(MVME_INTERFACE *mvme, uint32_t base, uint16_t edge);
uint16_t v1290_ReadEdgeDetection(MVME_INTERFACE *mvme, uint32_t base);
void v1290_SetEdgeResolution(MVME_INTERFACE *mvme, uint32_t base, uint16_t res);
uint16_t v1290_ReadEdgeResolution(MVME_INTERFACE *mvme, uint32_t base);
void v1290_SetDeadtime(MVME_INTERFACE *mvme, uint32_t base, uint16_t deadtime);
uint16_t v1290_ReadDeadtime(MVME_INTERFACE *mvme, uint32_t base);
// TDC readout: headers, trailers, event composition, section 5.5 @ manual
void v1290_EnableHeader(MVME_INTERFACE *mvme, uint32_t base, bool en);
bool v1290_HeaderIsEnabled(MVME_INTERFACE *mvme, uint32_t base);
void v1290_SetMaxHits(MVME_INTERFACE *mvme, uint32_t base, uint16_t max);
uint16_t v1290_ReadMaxHits(MVME_INTERFACE *mvme, uint32_t base);
void v1290_EnableTDCErrorMark(MVME_INTERFACE *mvme, uint32_t base, bool en);
void v1290_EnableBypassTDC(MVME_INTERFACE *mvme, uint32_t base, bool en);
void v1290_SetFIFOSize(MVME_INTERFACE *mvme, uint32_t base, uint16_t size);
uint16_t v1290_ReadFIFOSize(MVME_INTERFACE *mvme, uint32_t base);
// Channel enable, section 5.6
void v1290_EnableChannel(MVME_INTERFACE *mvme, uint32_t base,uint16_t chn,bool en);
void v1290_EnableAllChannels(MVME_INTERFACE *mvme, uint32_t base);
void v1290_DisableAllChannels(MVME_INTERFACE *mvme, uint32_t base);
void v1290_WriteEnablePattern(MVME_INTERFACE *mvme,uint32_t base,uint16_t pattern);
uint16_t v1290_ReadEnablePattern(MVME_INTERFACE *mvme,uint32_t base);

// Control/status registers
void v1290_ReadControlRegister(MVME_INTERFACE *mvme, uint32_t base);
void v1290_EnableBusError(MVME_INTERFACE *mvme, uint32_t base, bool en);
void v1290_EnableEmptyEvent(MVME_INTERFACE *mvme, uint32_t base, bool en);
void v1290_EnableEventFIFO(MVME_INTERFACE *mvme, uint32_t base, bool en);

void v1290_ReadStatusRegister(MVME_INTERFACE *mvme, uint32_t base);
bool v1290_IsDataReady(MVME_INTERFACE *mvme, uint32_t base_addr);
bool v1290_IsAlmostFull(MVME_INTERFACE *mvme, uint32_t base_addr);
bool v1290_IsFull(MVME_INTERFACE *mvme, uint32_t base_addr);

void v1290_ModuleReset(MVME_INTERFACE *mvme, uint32_t base);
void v1290_SoftClear(MVME_INTERFACE *mvme, uint32_t base);
void v1290_EventReset(MVME_INTERFACE *mvme, uint32_t base);
void v1290_SoftTrigger(MVME_INTERFACE *mvme, uint32_t base);
uint32_t v1290_ReadEventCounter(MVME_INTERFACE *mvme, uint32_t base);
uint16_t v1290_ReadEventStored(MVME_INTERFACE *mvme, uint32_t base);
void v1290_SetAlmostFullLevel(MVME_INTERFACE *mvme, uint32_t base, 
		uint16_t level);
uint16_t v1290_ReadAlmostFullLevel(MVME_INTERFACE *mvme, uint32_t base, 
		uint16_t level);
void v1290_FWRev(MVME_INTERFACE *mvme, uint32_t base);

void v1290_ReadEventFIFO(MVME_INTERFACE *mvme, uint32_t base, uint16_t *dest);
uint16_t v1290_ReadEventFIFOStored(MVME_INTERFACE *mvme, uint32_t base);
uint16_t v1290_ReadEventFIFOStatus(MVME_INTERFACE *mvme, uint32_t base);
#endif /* end of include guard: V1290_7NX5IH0R */
