// ****************************************************************************
// *     Filename:  v1290.h
// *  Description:  
// *   Created by:  Nam Tran (nam at kuno-g.phys.sci.osaka-u.ac.jp)
// *     Time tag:  2013-04-07 10:31:01
// ****************************************************************************
#ifndef V1290_7NX5IH0R
#define V1290_7NX5IH0R

#include <stdint.h>

const char MODEL = 'A';
const int  NCHAN = 32;
//const int  NCHAN = 16;

typedef enum
{
  V1190_CH_PER_TDC = 32,
  V1290_CH_PER_TDC = 8,
} V1X90_CH_PER_TDC;

typedef struct v1290_trg_conf_t
{
  uint16_t width;
  int16_t offset;
  uint16_t extra_width;
  uint16_t reject_margin;
  uint16_t subtraction;
} v1290_trg_conf_t;

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
// Modified by VT on 06/05/2015
// I think the mask is incorrect
//#define V1290_TDC_MSR_MEASURE_MSK		0x0007ffff
#define V1290_TDC_MSR_MEASURE_MSK		0X001FFFFF
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
uint16_t v1290_Read16       (int handle, uint32_t offset);
uint32_t v1290_Read32       (int handle, uint32_t offset);
void     v1290_Write16      (int handle, uint32_t offset, uint16_t value);
void     v1290_Write32      (int handle, uint32_t offset, uint16_t value);
int      v1290_ReadEventBLT (int handle, uint32_t* dest, int nwords32_request=256);
int      v1290_ReadEventMBLT(int handle, uint32_t* dest, int nwords32_request=256);

// Opcode related functions
int      v1290_MicroWrite(int handle, uint16_t data);
void     v1290_TellMicro1(int handle, uint16_t opcode, uint16_t data);
void     v1290_TellMicroN(int handle, uint16_t opcode, uint16_t data[], int n);
uint16_t v1290_MicroRead (int handle);
uint16_t v1290_AskMicro1 (int handle, uint16_t opcode);
void     v1290_AskMicroN (int handle, uint16_t opcode, uint16_t resp[], int n);

// Acq mode settings
void v1290_TriggerMatchingSet(int handle);
void v1290_ContinuosSet(int handle);
int v1290_AcqModeRead(int handle);
// Trigger matching mode related settings
//int v1290_TriggerConfRead(int handle, uint16_t *conf);
v1290_trg_conf_t v1290_TriggerConfRead(int handle);
void v1290_SetWindowWidth(int handle, uint32_t width);
void v1290_SetWindowOffset(int handle, int32_t offset);
void v1290_SetExtraMargin(int handle, uint32_t margin);
void v1290_SetRejectMargin(int handle, uint32_t margin);
void v1290_EnableTriggerSubtraction(int handle, bool en);
// TDC edge and resolution settings
// only 2 lsb bits matter (see 5.4 @ manual)
// edge code: 00-pair mode, 01-only trailing, 10-only leading, 11-both edges
// resolution code (for edges): 00-800 ps, 01-200 ps, 10-100 ps, 11-25 ns
void v1290_SetEdgeDetection(int handle, uint16_t edge);
uint16_t v1290_ReadEdgeDetection(int handle);
void v1290_SetEdgeResolution(int handle, uint16_t res);
uint16_t v1290_ReadEdgeResolution(int handle);
void v1290_SetDeadtime(int handle, uint16_t deadtime);
uint16_t v1290_ReadDeadtime(int handle);
// TDC readout: headers, trailers, event composition, section 5.5 @ manual
void v1290_EnableHeader(int handle, bool en);
bool v1290_HeaderIsEnabled(int handle);
void v1290_SetMaxHits(int handle, uint16_t max);
uint16_t v1290_ReadMaxHits(int handle);
void v1290_EnableTDCErrorMark(int handle, bool en);
void v1290_EnableBypassTDC(int handle, bool en);
void v1290_SetFIFOSize(int handle, uint16_t size);
uint16_t v1290_ReadFIFOSize(int handle);
// Channel enable, section 5.6
void v1290_EnableChannel(int handle, uint16_t chn, bool en);
void v1290_EnableAllChannels(int handle);
void v1290_DisableAllChannels(int handle);
void v1290_WriteEnablePattern(int handle, uint32_t pattern);
uint32_t v1290_ReadEnablePattern(int handle);

// Control/status registers
void v1290_ReadControlRegister(int handle);
void v1290_EnableBusError(int handle, bool en);
void v1290_EnableEmptyEvent(int handle, bool en);
void v1290_EnableEventFIFO(int handle, bool en);

void v1290_ReadStatusRegister(int handle);
bool v1290_IsDataReady(int handle);
bool v1290_IsAlmostFull(int handle);
bool v1290_IsFull(int handle_addr);

void v1290_ModuleReset(int handle);
void v1290_SoftClear(int handle);
void v1290_EventReset(int handle);
void v1290_SoftTrigger(int handle);
uint32_t v1290_ReadEventCounter(int handle);
uint16_t v1290_ReadEventStored(int handle);
void v1290_SetAlmostFullLevel(int handle, 
		uint16_t level);
uint16_t v1290_ReadAlmostFullLevel(int handle, 
		uint16_t level);
uint16_t v1290_FWRev(int handle);

void v1290_ReadEventFIFO(int handle, uint16_t *dest);
uint16_t v1290_ReadEventFIFOStored(int handle);
uint16_t v1290_ReadEventFIFOStatus(int handle);
void v1290_Decode(uint32_t data);
#endif /* end of include guard: V1290_7NX5IH0R */
