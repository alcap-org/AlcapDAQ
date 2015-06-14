// ****************************************************************************
// *     Filename:  v1290.c
// *  Description:  
// *   Created by:  Nam Tran (nam at kuno-g.phys.sci.osaka-u.ac.jp)
// *     Time tag:  2013-04-07 11:05:36
// ****************************************************************************
#include <stdio.h>
#include <string.h>
#include "unistd.h"
#include "CAENComm.h"
#include "v1290.h"

uint16_t v1290_Read16(int handle, uint32_t offset)
{
  uint16_t data;
  CAENComm_Read16(handle, offset, &data);
  return data;
}

uint32_t v1290_Read32(int handle, uint32_t offset)
{
  uint32_t data;
  CAENComm_Read32(handle, offset, &data);
  return data;
}

void v1290_Write16(int handle, uint32_t offset, uint16_t value)
{
  CAENComm_Write16(handle, offset, value);
}

void v1290_Write32(int handle, uint32_t offset, uint32_t value)
{
  CAENComm_Write32(handle, offset, value);
}

/*****************************************************************/
int v1290_ReadEventBLT(int handle, uint32_t *pdest, int nwords32_request)
{
  int nwords32_read;
  CAENComm_BLTRead(handle, V1290_OUT_BUFFER, pdest, 4*nwords32_request, &nwords32_read);
  return nwords32_read;
}

int v1290_ReadEventMBLT(int handle, uint32_t *pdest, int nwords32_request)
{
  int nwords32_read;
  CAENComm_MBLTRead(handle, V1290_OUT_BUFFER, pdest, 4*nwords32_request, &nwords32_read);
  return nwords32_read;
}

/*****************************************************************/
int v1290_MicroWrite(int handle, uint16_t data)
{
  for (int i=0; i<1000; i++)
  {
    uint16_t microHS;
    CAENComm_Read16(handle, V1290_MICRO_HND, &microHS);
    if (microHS & V1290_MICRO_WR_OK) {
      CAENComm_Write16(handle, V1290_MICRO, data);
      return 1;
    }
    usleep(1);
  }
  printf("v1290_MicroWrite: Micro not ready for writing!\n");
  return -1;
}

uint16_t v1290_MicroRead(int handle)
{
  for (int i=100; i>0; i--) {
    uint16_t microHS;
    CAENComm_Read16(handle, V1290_MICRO_HND, &microHS);
    if (microHS & V1290_MICRO_RD_OK) {
      uint16_t reg;
      CAENComm_Read16(handle, V1290_MICRO, &reg);
      return reg;
    }
    usleep(1);
  };
  return -1;
}

uint16_t v1290_AskMicro1(int handle, uint16_t opcode)
{
  v1290_MicroWrite(handle, opcode);
  return v1290_MicroRead(handle);
}

void v1290_TellMicro1(int handle, uint16_t opcode, uint16_t data)
{
  v1290_MicroWrite(handle, opcode);
  v1290_MicroWrite(handle, data);
}

void v1290_AskMicroN(int handle, uint16_t opcode, uint16_t resp[], int n)
{
  v1290_MicroWrite(handle, opcode);
  for(int i = 0; i < n; ++i)
    resp[i] = v1290_MicroRead(handle);
}

void v1290_TellMicroN(int handle, uint16_t opcode, uint16_t data[], int n)
{
  v1290_MicroWrite(handle, opcode);
  for (int i = 0; i < n; ++i)
    v1290_MicroWrite(handle, data[i]);
}
/*****************************************************************/

void v1290_TriggerMatchingSet(int handle)
{
  v1290_MicroWrite(handle, V1290_TRG_MATCH_OPCODE);
}

void v1290_ContinuosSet(int handle)
{
  v1290_MicroWrite(handle, V1290_CONT_STORE_OPCODE);
}

int v1290_AcqModeRead(int handle)
{
  v1290_MicroWrite(handle, V1290_READ_ACQ_MOD_OPCODE);
  uint16_t resp = v1290_MicroRead(handle);
  if ((resp & 0x1) == 1)
  {
    printf("AcqMode: trigger matching.\n");
  }
  else if ((resp & 0x1) == 0)
  {
    printf("AcqMode: continuous.\n");
  }
  return (resp & 0x1);
}

v1290_trg_conf_t v1290_TriggerConfRead(int handle)
{
  v1290_trg_conf_t trg_conf;

  v1290_AskMicroN(handle, V1290_READ_TRG_CONF_OPCODE, (uint16_t*)(&trg_conf), 5);

  printf("Trigger settings:\n");
  printf("- match window width: %d ns\n", trg_conf.width*25);
  printf("- match window offset: %d ns\n", trg_conf.offset*25);
  printf("- extra search window width: %d ns\n", trg_conf.extra_width*25);
  printf("- reject margin: %d ns\n", trg_conf.reject_margin*25);
  printf("- trigger time subtraction: %d\n", trg_conf.subtraction>>15);

 return trg_conf;
}

void v1290_SetWindowWidth(int handle, uint32_t width)
{
  v1290_TellMicro1((int)handle, (uint16_t)V1290_SET_WIN_WIDTH_OPCODE, (uint16_t)(width/25));
}

void v1290_SetWindowOffset(int handle, int32_t offset)
{
  if (offset < -2048*25)
    offset = -2048*25;
  if (offset > +40*25)
    offset = 40*25;

  v1290_TellMicro1(handle, (uint16_t)V1290_SET_WIN_OFFSET_OPCODE, (uint16_t)(offset/25));
}

void v1290_SetExtraMargin(int handle, uint32_t margin)
{
  v1290_TellMicro1(handle, (uint16_t)V1290_SET_SW_MARGIN_OPCODE, (uint16_t)(margin/25));
}

void v1290_SetRejectMargin(int handle, uint32_t margin)
{
  v1290_TellMicro1(handle, (uint16_t)V1290_SET_REJ_MARGIN_OPCODE, (uint16_t)(margin/25));
}

void v1290_EnableTriggerSubtraction(int handle, bool en)
{
  uint32_t opcode = en ? V1290_EN_SUB_TRG_OPCODE : V1290_DIS_SUB_TRG_OPCODE;
  v1290_MicroWrite(handle, opcode);
}

// TDC edge and resolution
void v1290_SetEdgeDetection(int handle, uint16_t edge)
{
  v1290_TellMicro1(handle, (uint16_t)V1290_SET_DETECTION_OPCODE, edge);
}

void v1290_SetEdgeResolution(int handle, uint16_t res)
{
  v1290_TellMicro1(handle, (uint16_t)V1290_SET_TR_LEAD_LSB_OPCODE, res);
}

uint16_t v1290_ReadEdgeResolution(int handle)
{
  uint16_t ret = v1290_AskMicro1(handle, V1290_READ_RES_OPCODE);
  switch (ret)
  {
    case 0:
      printf("Resolution: 800 ps\n");
      break;
    case 1:
      printf("Resolution: 200 ps\n");
      break;
    case 2:
      printf("Resolution: 100 ps\n");
      break;
    case 3:
      printf("Resolution: 25 ps\n");
      break;
    default:
      printf("Invalid setting!\n");
      ret = 100;
      break;
  }
  return ret;
}

uint16_t v1290_ReadEdgeDetection(int handle)
{
  uint16_t ret = v1290_AskMicro1(handle, V1290_READ_DETECTION_OPCODE);
  switch (ret)
  {
    case 0:
      printf("Edge detection: pair\n");
      break;
    case 1:
      printf("Edge detection: trailing\n");
      break;
    case 2:
      printf("Edge detection: leading\n");
      break;
    case 3:
      printf("Edge detection: trailing and leading\n");
      break;
    default:
      printf("Invalid setting!\n");
      ret = 100;
      break;
  }
  return ret;
}

void v1290_SetDeadtime(int handle, uint16_t deadtime)
{
  v1290_TellMicro1(handle, (uint16_t)V1290_SET_DEAD_TIME_OPCODE, deadtime);
}

uint16_t v1290_ReadDeadtime(int handle)
{
  uint16_t ret = v1290_AskMicro1(handle, V1290_READ_DEAD_TIME_OPCODE);
  switch (ret)
  {
    case 0:
      printf("Channel deadtime: 5 ns\n");
      break;
    case 1:
      printf("Channel deadtime: 10 ns\n");
      break;
    case 2:
      printf("Channel deadtime: 30 ns\n");
      break;
    case 3:
      printf("Channel deadtime: 100 ns\n");
      break;
    default:
      printf("Invalid setting!\n");
      ret = 100;
      break;
  }

  return ret;
}

//TDC read out
void v1290_EnableHeader(int handle, bool en)
{
  uint32_t opcode = en ? V1290_EN_HEAD_TRAILER_OPCODE : V1290_DIS_HEAD_TRAILER_OPCODE;
  v1290_MicroWrite(handle, opcode);
}

bool v1290_HeaderIsEnabled(int handle)
{
	return (v1290_AskMicro1(1, V1290_READ_HEAD_TRAILER_OPCODE) & 0x1);
}

void v1290_SetMaxHits(int handle, uint16_t max)
{
  v1290_TellMicro1(handle, (uint16_t)V1290_SET_EVENT_SIZE_OPCODE, max);
}

uint16_t v1290_ReadMaxHits(int handle)
{
	return (v1290_AskMicro1(handle, V1290_READ_EVENT_SIZE_OPCODE) &0xf);
}

void v1290_EnableTDCErrorMark(int handle, bool en)
{
  uint32_t opcode = en ? V1290_EN_ERROR_MARK_OPCODE : V1290_DIS_ERROR_MARK_OPCODE;
  v1290_MicroWrite(handle, opcode);
}

void v1290_EnableBypassTDC(int handle, bool en)
{
  uint32_t opcode = en ? V1290_EN_ERROR_BYPASS_OPCODE : V1290_DIS_ERROR_BYPASS_OPCODE;
  v1290_MicroWrite(handle, opcode);
}

void v1290_SetFIFOSize(int handle, uint16_t size)
{
  v1290_TellMicro1(handle, (uint16_t)V1290_SET_FIFO_SIZE_OPCODE, size);
}

uint16_t v1290_ReadFIFOSize(int handle)
{
  return (v1290_AskMicro1(handle, V1290_READ_FIFO_SIZE_OPCODE) &0x7);
}
// Channel Enable
void v1290_EnableChannel(int handle, uint16_t chn, bool en)
{
  uint32_t opcode = en ? V1290_EN_CHANNEL_OPCODE : V1290_DIS_CHANNEL_OPCODE;
  opcode += chn;
  v1290_MicroWrite(handle, opcode);
}

void v1290_EnableAllChannels(int handle)
{
  v1290_MicroWrite(handle, V1290_EN_ALL_CH_OPCODE);
}

void v1290_DisableAllChannels(int handle)
{
  v1290_MicroWrite(handle, V1290_DIS_ALL_CH_OPCODE);
}

void v1290_WriteEnablePattern(int handle, uint32_t pattern)
{
  if (MODEL == 'A')
    {
      const int n = 2;
      uint16_t pattern2[n] = { (uint16_t)pattern, (uint16_t)(pattern >> 16) };
      v1290_TellMicroN(handle, (uint16_t)V1290_WRITE_EN_PATTERN_OPCODE, pattern2, n);
    }
  else
    {
      v1290_TellMicro1(handle, (uint16_t)V1290_WRITE_EN_PATTERN_OPCODE, (uint16_t)pattern);
    }
}

uint32_t v1290_ReadEnablePattern(int handle)
{
  if (MODEL == 'A')
    {
      const int n = 2;
      uint16_t chmask[n];
      v1290_AskMicroN(handle, V1290_READ_EN_PATTERN_OPCODE, chmask, n);
      return ((uint32_t)chmask[0]) & (((uint32_t)chmask[1]) << 16);
    }

  return v1290_AskMicro1(handle,V1290_READ_EN_PATTERN_OPCODE);
}

// Regiters
void v1290_ReadControlRegister(int handle)
{
  uint16_t reg = v1290_Read16(handle, V1290_CONTROL);
  printf("Control register: 0x%X\n", V1290_CONTROL);
  printf(" - Bus error enable: %d\n", reg & 0x1);
  printf(" - Empty event enable: %d\n", (reg & (0x1<<3))>>3);
  printf(" - Compensation enable: %d\n", (reg & (0x1<<5))>>5);
  printf(" - Event FIFO enable: %d\n", (reg & (0x1<<8))>>8);
}

void v1290_EnableBusError(int handle, bool en)
{
  uint16_t reg = v1290_Read16(handle, V1290_CONTROL);
  if (en)
    v1290_Write16(handle, V1290_CONTROL, reg + 0x1);
  else
    v1290_Write16(handle, V1290_CONTROL, reg & 0xfffe);
}

void v1290_EnableEmptyEvent(int handle, bool en)
{
  uint16_t reg = v1290_Read16(handle, V1290_CONTROL);
  if (en)
    v1290_Write16(handle, V1290_CONTROL, reg + (0x1<<3));
  else
    v1290_Write16(handle, V1290_CONTROL, reg & (0xfff7));
}

void v1290_EnableEventFIFO(int handle, bool en)
{
  uint16_t reg = v1290_Read16(handle, V1290_CONTROL);
  if (en)
    v1290_Write16(handle, V1290_CONTROL, reg + 0x100);
  else
    v1290_Write16(handle, V1290_CONTROL, reg & 0xfeff);
}

void v1290_ReadStatusRegister(int handle)
{
  uint16_t reg = v1290_Read16(handle, V1290_STATUS);
  printf("Status register: 0x%X\n", V1290_STATUS);
  printf(" - Data ready: %d\n", reg & 0x1);
  printf(" - Almost full: %d\n", (reg & (0x1<<1))>>1);
  printf(" - Full: %d\n", (reg & (0x1<<2))>>2);
  printf(" - Trigger match: %d\n", (reg & (0x1<<3))>>3);
  printf(" - Header enable: %d\n", (reg & (0x1<<4))>>4);
  printf(" - Term on: %d\n", (reg & (0x1<<5))>>5);
  printf(" - Error TDC0: %d\n", (reg & (0x1<<6))>>6);
  printf(" - Error TDC1: %d\n", (reg & (0x1<<7))>>7);
  printf(" - Error TDC2: %d\n", (reg & (0x1<<8))>>8);
  printf(" - Error TDC3: %d\n", (reg & (0x1<<9))>>9);
  printf(" - Bus error: %d\n", (reg & (0x1<<10))>>10);
  printf(" - Purged: %d\n", (reg & (0x1<<11))>>11);
  printf(" - Res 0: %d\n", (reg & (0x1<<12))>>12);
  printf(" - Res 1: %d\n", (reg & (0x1<<13))>>13);
  printf(" - Pair: %d\n", (reg & (0x1<<14))>>14);
  printf(" - Trigger lost: %d\n", (reg & (0x1<<15))>>15);
}

bool v1290_IsDataReady(int handle)
{
  uint16_t reg = v1290_Read16(handle, V1290_STATUS);
  return reg & 0x1;
}

bool v1290_IsAlmostFull(int handle)
{
  uint16_t reg = v1290_Read16(handle, V1290_STATUS);
  return (reg & (0x1 << 1)) >> 1;
}

bool v1290_IsFull(int handle)
{
  uint16_t reg = v1290_Read16(handle, V1290_STATUS);
  return (reg & (0x1 << 2)) >> 2;
}

void v1290_ModuleReset(int handle)
{
  v1290_Write16(handle, V1290_MOD_RESET, 0x1);
}

void v1290_SoftClear(int handle)
{
  v1290_Write16(handle, V1290_SW_CLEAR, 0x1);
}

void v1290_EventReset(int handle)
{
  v1290_Write16(handle, V1290_SW_EVENT_RESET, 0x1);
}

void v1290_SoftTrigger(int handle)
{
  v1290_Write16(handle, V1290_SW_TRIGGER, 0x1);
}

uint32_t v1290_ReadEventCounter(int handle)
{
  return v1290_Read32(handle, V1290_EVENT_COUNTER);
}

uint16_t v1290_ReadEventStored(int handle)
{
  return v1290_Read16(handle, V1290_EVENT_STORED);
}

void v1290_SetAlmostFullLevel(int handle, uint16_t level)
{
  v1290_Write16(handle, V1290_ALMOST_FULL_LVL, level);
}

uint16_t v1290_ReadAlmostFullLevel(int handle, uint16_t level)
{
  return v1290_Read16(handle, V1290_ALMOST_FULL_LVL);
}

uint16_t v1290_FWRev(int handle)
{
  return v1290_Read16(handle, V1290_FW_REV);
}

void v1290_ReadEventFIFO(int handle, uint16_t *dest)
{
  uint32_t reg = v1290_Read32(handle, V1290_EVENT_FIFO);
  dest[0] = (reg & 0xffff0000) >> 16;
  dest[1] = reg&0xffff;
}

uint16_t v1290_ReadEventFIFOStatus(int handle)
{
  return v1290_Read16(handle, V1290_EVENT_FIFO_STATUS) & 0x3;
}

void v1290_Decode(uint32_t data)
{
  uint32_t error_flag;
  switch((data & 0xf8000000) >> 27) {
    case 0x0:
      printf("Data:\t\t");
      printf("channel: %d\t",(data&V1290_CH_MASK)>>21);
      printf("value(ns): %.4f\t",(float)(data&V1290_DATA_MASK)/40);
      printf("trailing/leading(1/0):%d",(data&(1<<26))>>26);
      putchar('\n');
      break;
    case 0x8:
      printf("Global header\t");
      //showbits(data);
      printf("Event count:%d\t",(data&V1290_GLB_HDR_EVENT_COUNT_MSK)>>5);
      printf("Geo addr:%d",data&V1290_GLB_HDR_GEO_MSK);
      putchar('\n');
      break;
    case 0x1:
      printf("TDC header\t");
      //showbits(data);
      printf("TDC: %d\t",(data&V1290_TDC_HDR_TDC_MSK)>>24);
      printf("EventID: %d\t",(data&V1290_TDC_HDR_EVENT_ID_MSK)>>12);
      printf("BunchID: %d\t",(data&V1290_TDC_HDR_BUNCH_ID_MSK));
      putchar('\n');
      break;
    case 0x3:
      printf("TDC trailer\t");
      //showbits(data);
      printf("TDC: %d\t",(data&V1290_TDC_TRL_TDC_MSK)>>24);
      printf("EventID: %d\t",(data&V1290_TDC_TRL_EVENT_ID_MSK)>>12);
      printf("Word count: %d\t",(data&V1290_TDC_TRL_WCOUNT_MSK));
      putchar('\n');
      break;
    case 0x11:
      printf("Extented trigger time tag: %x\t",data&V1290_GLB_TRG_TIME_TAG_MSK);
      //showbits(data);
      putchar('\n');
      break;
    case 0x10:
      printf("Global Trailer\t\t");
      printf("Error:%d\t",(data&V1290_GLB_TRL_STATUS_MSK)>>24);
      printf("Word count:%d\t",(data&V1290_GLB_TRL_WCOUNT_MSK)>>5);
      printf("Geo addr:%d",data&V1290_GLB_TRL_GEO_MSK);
      putchar('\n');
      break;
    case 0x4:
      printf("TDC error\t");
      printf("TDC: %d\t",(data&V1290_TDC_HDR_TDC_MSK)>>24);
      error_flag = data&V1290_TDC_ERR_ERR_FLAGS_MSK;
      putchar('\n');
      break;
    default:
      break;
  }
}


void v1290_SetupTrigger(int handle)
{
  printf("Trigger matching set ...\n");
  v1290_TriggerMatchingSet(handle);
  sleep(1);
  v1290_SetWindowWidth(handle, 40000);
  sleep(1);
  v1290_SetWindowOffset(handle, -10000);
  sleep(1);
  v1290_EnableTriggerSubtraction(handle, true);
  sleep(1);
  v1290_AcqModeRead(handle);
  sleep(1);
}
void v1290_SetupTDC(int handle)
{
  printf("Setting TDC ...\n");
  v1290_SetEdgeDetection(handle, 0x1);
  sleep(1);
  v1290_SetEdgeResolution(handle, 0x1);
  sleep(1);
  printf("Edge: %d\n", v1290_ReadEdgeDetection(handle) );
  sleep(1);
  printf("Res: %d\n", v1290_ReadEdgeResolution(handle) );
  sleep(2);
  v1290_SetDeadtime(handle, 0x2);
  sleep(1);
  printf("Deadtime: %d\n", v1290_ReadDeadtime(handle) );
  sleep(1);
}
void v1290_SetupReadout(int handle)
{
  v1290_EnableHeader(handle, false);
  sleep(1);
  printf("header enable %d\n", v1290_HeaderIsEnabled(handle));
  sleep(1);
  printf("max hits %d\n", v1290_ReadMaxHits(handle));
  sleep(1);
  v1290_SetFIFOSize(handle, 0x6);
  sleep(1);
  printf("fifo size %d\n", v1290_ReadFIFOSize(handle));
  sleep(1);
  v1290_DisableAllChannels(handle);
  v1290_WriteEnablePattern(handle, 0x5);
  v1290_EnableChannel(handle, 0x0, false);
  sleep(1);
  printf("enable pattern %x\n", v1290_ReadEnablePattern(handle));
  v1290_EnableChannel(handle, 0, true);
}
