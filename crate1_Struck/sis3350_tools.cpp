#define BIT(x) (1<<x)

static int sis3350_sampling_bank_nr; // active bank nr., 0 or 1;
static int sis3350_event_ready[sis3350_n_boards];
static u_int32_t sis3350_err[sis3350_n_boards][sis3350_n_channels];
static u_int32_t sis3350_last_sample_addr[sis3350_n_boards][sis3350_n_channels];
static u_int32_t *sis3350_ADC_data[sis3350_n_boards][sis3350_n_channels];
static u_int32_t sis3350_ADC_data_size[sis3350_n_boards][sis3350_n_channels];
static u_int32_t sis3350_ADC_max_data_size = MAX_EVENT_SIZE/sis3350_n_channels;

extern INT send_ready_for_cycle();

static INT sis3350_A32D32_write(const int board_nr, const u_int32_t reg, const u_int32_t data)
{
  
  u_int32_t addr = sis3350_odb[board_nr].vme_base + reg;

  int return_code = vme_A32D32_write(fd_vme, addr, data) ;

  if ( return_code != 0 ) 
    { 
      cm_msg(MERROR, "sis3350_A32D32_write()", "return_code = 0x%08x  at address = 0x%08x for board %i", return_code, addr, board_nr);
      return FE_ERR_HW;
    }

  return SUCCESS;

}

static INT sis3350_A32D32_read(const int board_nr, const u_int32_t reg, u_int32_t &data)
{
  
  u_int32_t addr = sis3350_odb[board_nr].vme_base + reg;

  int return_code = vme_A32D32_read(fd_vme, addr, &data) ;

  if ( return_code != 0 ) 
    { 
      cm_msg(MERROR, "sis3350_A32D32_read()", "return_code = 0x%08x  at address = 0x%08x for board %i", return_code, addr, board_nr);
      return FE_ERR_HW;
    }

  return SUCCESS;

}



static INT sis3350_write_dac_offset(const int board_nr, 
				    const u_int32_t module_dac_control_status_addr, 
				    const unsigned int dac_select_no, 
				    const unsigned int dac_value )
{
  u_int32_t    data, addr;
  unsigned int max_timeout, timeout_cnt;
  
  data =  dac_value ;
  addr = module_dac_control_status_addr + 4 ; // DAC_DATA
  if ( sis3350_A32D32_write(board_nr, addr, data) != SUCCESS ) return FE_ERR_HW; 
  
  data =  1 + (dac_select_no << 4); // write to DAC Register
  addr = module_dac_control_status_addr ;
  if ( sis3350_A32D32_write(board_nr, addr, data) != SUCCESS ) return FE_ERR_HW; 

  max_timeout = 5000 ;
  timeout_cnt = 0 ;
  addr = module_dac_control_status_addr  ;
  do 
    { 
      if ( sis3350_A32D32_read(board_nr, addr, data) != SUCCESS ) return FE_ERR_HW;      
      timeout_cnt++;
    } while ( ((data & 0x8000) == 0x8000) && (timeout_cnt <  max_timeout) )    ;
  
  if (timeout_cnt >=  max_timeout) return FE_ERR_HW;
  
  data =  2 + (dac_select_no << 4); // Load DACs 
  addr = module_dac_control_status_addr;
  if ( sis3350_A32D32_write(board_nr, addr, data) != SUCCESS ) return FE_ERR_HW;

  timeout_cnt = 0 ;
  addr = module_dac_control_status_addr  ;
  do {
    if ( sis3350_A32D32_read(board_nr, addr, data) != SUCCESS ) return FE_ERR_HW;
    timeout_cnt++;
  } while ( ((data & 0x8000) == 0x8000) && (timeout_cnt <  max_timeout) );
  
  if (timeout_cnt >=  max_timeout) return FE_ERR_HW;

  return SUCCESS;
  
}


static INT sis3350_gain_offset_configuration(const int board_nr)
{
  u_int32_t data;
  const u_int32_t reg[sis3350_n_channels] = {SIS3350_ADC_VGA_ADC1, SIS3350_ADC_VGA_ADC2, SIS3350_ADC_VGA_ADC3, SIS3350_ADC_VGA_ADC4};

  for (int i=0; i<sis3350_n_channels; i++)    
    {  
      // VGA 
      data = sis3350_odb[board_nr].ch[i].VGA; 
      if ( data > 128 ) data = 128;
      if ( sis3350_odb[board_nr].ch[i].gain == 1)
	{
	  data += BIT(7);
	}
      
      if ( sis3350_A32D32_write(board_nr, reg[i], data) != SUCCESS ) return FE_ERR_HW; 

    }

  if ( sis3350_write_dac_offset(board_nr, SIS3350_ADC12_DAC_CONTROL_STATUS, 0, sis3350_odb[board_nr].ch[0].offset) != SUCCESS ) return FE_ERR_HW; 
  if ( sis3350_write_dac_offset(board_nr, SIS3350_ADC12_DAC_CONTROL_STATUS, 1, sis3350_odb[board_nr].ch[1].offset ) != SUCCESS ) return FE_ERR_HW;
  if ( sis3350_write_dac_offset(board_nr, SIS3350_ADC34_DAC_CONTROL_STATUS, 0, sis3350_odb[board_nr].ch[2].offset ) != SUCCESS ) return FE_ERR_HW;
  if ( sis3350_write_dac_offset(board_nr ,SIS3350_ADC34_DAC_CONTROL_STATUS, 1, sis3350_odb[board_nr].ch[3].offset ) != SUCCESS ) return FE_ERR_HW;
  
  return SUCCESS;
}

static INT SIS3350_External_Input_Offset_Configuration(const int board_nr)
{

  // 0x7B0C - NIM level
  // 0xFFFF/2 - zero level
  // clock offset 
  if ( sis3350_write_dac_offset(board_nr, SIS3350_EXT_CLOCK_TRIGGER_DAC_CONTROL_STATUS, 0, 0xFFFF/2 ) != SUCCESS ) return FE_ERR_HW;
  // trigger offset
  if ( sis3350_write_dac_offset(board_nr, SIS3350_EXT_CLOCK_TRIGGER_DAC_CONTROL_STATUS, 1, 0x7B0C ) != SUCCESS ) return FE_ERR_HW;

  return SUCCESS;
}


static INT sis3350_trigger_configuration(const int board_nr)
{
  u_int32_t data;
  u_int32_t D;
  const u_int32_t reg_trig[sis3350_n_channels] = {SIS3350_TRIGGER_SETUP_ADC1, SIS3350_TRIGGER_SETUP_ADC2, SIS3350_TRIGGER_SETUP_ADC3, SIS3350_TRIGGER_SETUP_ADC4};
  const u_int32_t reg_thr[sis3350_n_channels] = {SIS3350_TRIGGER_THRESHOLD_ADC1, SIS3350_TRIGGER_THRESHOLD_ADC2, SIS3350_TRIGGER_THRESHOLD_ADC3, SIS3350_TRIGGER_THRESHOLD_ADC4};

  for (int i=0; i<sis3350_n_channels; i++)    
    {  
      data = 0x0;

      // enable trigger 
      if ( sis3350_odb[board_nr].ch[i].enabled && sis3350_odb[board_nr].ch[i].trigger_enable )
	data += BIT(26);

      // GT condition
      if ( sis3350_odb[board_nr].ch[i].trigger_logic == 1 )
	data += BIT(25);
      
      // Trigger mode (0-thrreshold, 1-FIR)
      if ( sis3350_odb[board_nr].ch[i].trigger_mode == 1 )
	data += BIT(24);
      
      // Trigger Pulse Length
      D = sis3350_odb[board_nr].ch[i].trigger_pulse_length;
      D = D & 0xFF;
      D = (D<<16);
      data += D;

      // Trigger SumG
      D = sis3350_odb[board_nr].ch[i].trigger_sumG;
      D = D & 0x1F;
      D = (D<<8);
      data += D;
      
      // Peaking time P
      D = sis3350_odb[board_nr].ch[i].trigger_P;
      D = D & 0x1F;
      data += D;
      
      printf("Board %i channel %i trigger register: 0x%08x\n", board_nr, i, data);
      if ( sis3350_A32D32_write(board_nr, reg_trig[i], data) != SUCCESS ) return FE_ERR_HW; 


      // Trigger threshold
      data = sis3350_odb[board_nr].ch[i].trigger_threshold_ON + (sis3350_odb[board_nr].ch[i].trigger_threshold_OFF<<16);
      if ( sis3350_A32D32_write(board_nr, reg_thr[i], data) != SUCCESS ) return FE_ERR_HW; 

      // Trigger output, register 0x38
      data = 0x0;
      data += BIT(4); // ADC1 trigger is ored to LEMO out
      data += BIT(5); // ADC1 trigger is ored to LEMO out
      data += BIT(6); // ADC1 trigger is ored to LEMO out
      data += BIT(7); // ADC1 trigger is ored to LEMO out

      if ( sis3350_A32D32_write(board_nr, 0x38, data) != SUCCESS ) return FE_ERR_HW; 




#if 1
      if ( sis3350_A32D32_read(board_nr, reg_trig[i], data) != SUCCESS ) return FE_ERR_HW; 
      printf("Board %i chan %i trigger register READBACK: 0x%08x\n", board_nr, i, data);
#endif

    }

  return SUCCESS;
}






static INT sis3350_open()
{

  u_int32_t addr ;
  u_int32_t data ;
  u_int32_t D    ;
  int return_code ;

  for (unsigned int i=0; i<sis3350_n_boards; i++)
    {
      if ( sis3350_odb[i].enabled == 0 ) 
	{
	  printf("SIS3350 board %i disabled in ODB (base address 0x%04x)\n",i+1, sis3350_odb[i].vme_base);
	  continue;
	}     

      printf("Opening board %i at base address 0x%08x ... ",i+1, sis3350_odb[i].vme_base);
      if ( sis3350_A32D32_read(i, 0x4, data) != SUCCESS ) return FE_ERR_HW;
      printf(" SUCCESS!\nModul Identification register:  0x%08x\n", data );

      // *** Reset *** 
      if ( sis3350_A32D32_write(i, SIS3350_KEY_RESET, 0x0) != SUCCESS ) return FE_ERR_HW;


      // ==============================================================
      // *** Configure Acquisition register 0x10 ***
      // ==============================================================

      data = 0x0;

      //data += SIS3350_ACQ_OPERATION_RINGBUFFER_ASYNCH;
      //data += SIS3350_ACQ_OPERATION_RINGBUFFER_SYNCH; 
      //data += SIS3350_ACQ_OPERATION_DIRECT_MEMORY_GATE_ASYNCH;
      //data += SIS3350_ACQ_OPERATION_DIRECT_MEMORY_GATE_SYNCH;
      //data += SIS3350_ACQ_OPERATION_DIRECT_MEMORY_STOP;
      //data += SIS3350_ACQ_OPERATION_DIRECT_MEMORY_START;

      //data += 0;                     // RingBuffer Asynchronous Mode
      //data += BIT(0);             // RingBuffer Synchronous Mode
      //data += BIT(1);             // Direct Memory Gate Asynchronous Mode
      //data += BIT(1) + BIT(0); // Direct Memory Gate Synchronous Mode
      //data += BIT(2);             // Direct Memory Trigger Stop Mode
      //data += BIT(2) + BIT(0); // Direct Memory Trigger Start Mode

      data += BIT(4); // Invet Bit for external Lemo TRG IN (1==NIM)
      data += BIT(5); // Enable Multi Event mode

      data += BIT(6); // Enable internal channel triggers as Gate/Trigger
      data += BIT(8); // Enable External Lemo TRG IN as Gate/Trigger
      //data += BIT(9); // Enable External LVDS TRG IN as Gate/Trigger
      
      // clock source
      //data += 0; // Frequency Synthesizer
      //data += BIT(12); // internal 100 MHz
      //data += BIT(13); // external LVDS
      //data += BIT(12) + BIT(13); // external BNC

      D = sis3350_odb[i].clock_src & 0x3;
      data += (D<<12);
            
      if ( sis3350_A32D32_write(i, SIS3350_ACQUISTION_CONTROL, data) != SUCCESS ) return FE_ERR_HW;


      // ==============================================================
      // *** Configure Frequency Synthesizer Register 0x1C ***
      // ==============================================================
      // The frequency is defined by the expression: f = 25 MHz * M / 2^N
      // M: bits 0-8
      // N: bits 9-10
      // Power up defaul value is 0x14 => f = 25 * 20 = 500 MHz      

      data = 0x14;
      if ( sis3350_A32D32_write(i, 0x1C, data) != SUCCESS ) return FE_ERR_HW;
      
      // ==============================================================
      // *** ADC gain and offset configuration ***
      // ==============================================================
      
      if ( sis3350_gain_offset_configuration(i) != SUCCESS ) return FE_ERR_HW;

      // ==============================================================
      // *** Trigger configuration ***
      // ==============================================================
      if ( sis3350_trigger_configuration(i) != SUCCESS ) return FE_ERR_HW;

            
      if ( SIS3350_External_Input_Offset_Configuration(i) != SUCCESS ) return FE_ERR_HW;

      // ==============================================================
      // *** Allocate memory for data buffers ***
      // ==============================================================
      
      for ( int j=0; j<sis3350_n_channels; j++ )
	{
	  sis3350_ADC_data[i][j] = new u_int32_t[sis3350_ADC_max_data_size];
	  if ( ! sis3350_ADC_data[i][j] )
	    {
	      cm_msg(MERROR, "sis3350_open", "Cannot allocate memory for ADC data buffer");
	      return FE_ERR_HW;
	    }
	}
      
    }
  
  return SUCCESS;
}




static INT sis3350_arm_all()
{

  u_int32_t addr ;
  u_int32_t data ;
  int return_code ;

  unsigned int sample_start_addr;

  if ( sis3350_sampling_bank_nr == 0 )
    {
      sample_start_addr  = 0;
    }
  else
    {
      sample_start_addr  = SIS3350_MAX_SAMPLE_LENGTH/2;
    }


  for (unsigned int i=0; i<sis3350_n_boards; i++)
    {
      

      if ( sis3350_odb[i].enabled == 0 ) continue;

      // Set ADC sampling start address 
      // SIS3350_SAMPLE_START_ADDRESS_ALL_ADC = 0x01000008
      if ( sis3350_A32D32_write(i, 0x01000008, sample_start_addr) != SUCCESS ) return FE_ERR_HW;


      // clear Timestamps
      // SIS3350_KEY_TIMESTAMP_CLR = 0x41C
      if ( sis3350_A32D32_write(i, 0x41C, 0x0) != SUCCESS ) return FE_ERR_HW;
      
      // arm
      // SIS3350_KEY_ARM = 0x410
      if ( sis3350_A32D32_write(i, 0x410, 0x0) != SUCCESS ) return FE_ERR_HW;

#if 1
      // Read ACQUISITION STATUS register 0x10
      if ( sis3350_A32D32_read(i, 0x10, data) != SUCCESS ) return FE_ERR_HW;
      printf("SIS3350: Board %i acquisition register 0x10: 0x%08x\n", i, data);
#endif      

    }
      
  return SUCCESS;

}


static INT sis3350_disarm_all()
{

  u_int32_t addr ;
  u_int32_t data ;
  int return_code ;

  for (unsigned int i=0; i<sis3350_n_boards; i++)
    {      
      if ( sis3350_odb[i].enabled == 0 ) continue;


      // disarm
      // SIS3350_KEY_DISARM = 0x414
      if ( sis3350_A32D32_write(i, 0x414, 0x0) != SUCCESS ) return FE_ERR_HW;

#if 0
      // Read ACQUISITION STATUS register 0x10
      if ( sis3350_A32D32_read(i, 0x10, data) != SUCCESS ) return FE_ERR_HW;
      printf("acquisition register 0x10: 0x%08x\n",data);
#endif

    }  
  
  return SUCCESS;
  
}

static INT sis3350_bor()
{

  u_int32_t addr ;
  u_int32_t data ;
  int return_code ;

  sis3350_sampling_bank_nr = 0;

  for (unsigned int i=0; i<sis3350_n_boards; i++)
    {
      
      if ( sis3350_odb[i].enabled == 0 ) continue;

      sis3350_event_ready[i] = 0;

      // ==============================================================
      // *** configure Control register 0x0 ***
      // ==============================================================

      data = 0x0;
      data += BIT(0); // Switch ON user LED

      if ( sis3350_A32D32_write(i, SIS3350_CONTROL_STATUS, data) != SUCCESS ) return FE_ERR_HW;

      // ==============================================================
      // *** Configure Acquisition register 0x10 ***
      // ==============================================================

      data = 0x0;

      //data += 0;                     // RingBuffer Asynchronous Mode
      //data += BIT(0);             // RingBuffer Synchronous Mode
      //data += BIT(1);             // Direct Memory Gate Asynchronous Mode
      //data += BIT(1) + BIT(0); // Direct Memory Gate Synchronous Mode
      //data += BIT(2);             // Direct Memory Trigger Stop Mode
      //data += BIT(2) + BIT(0); // Direct Memory Trigger Start Mode

      //data += BIT(5); // Enable Multi Event mode

      //data += BIT(6); // Enable internal channel triggers as Gate/Trigger
      //data += BIT(8); // Enable External Lemo TRG IN as Gate/Trigger
      //data += BIT(9); // Enable External LVDS TRG IN as Gate/Trigger
      
      // clock source
      //data += 0; // Frequency Synthesizer
      //data += BIT(12); // internal 100 MHz
      //data += BIT(13); // external LVDS
      //data += BIT(12) + BIT(13); // external BNC
      
      //if ( sis3350_A32D32_write(i, SIS3350_ACQUISTION_CONTROL, data) != SUCCESS ) return FE_ERR_HW;

      // Ringbuffer Sample Length Register
      //if ( sis3350_A32D32_write(i, SIS3350_RINGBUFFER_SAMPLE_LENGTH_ADC12, sis3350_odb[i].ch_group[0].wf_len) != SUCCESS ) return FE_ERR_HW;
      if ( sis3350_A32D32_write(i, 0x02000020, sis3350_odb[i].ch_group[0].wf_len) != SUCCESS ) return FE_ERR_HW;
      //if ( sis3350_A32D32_write(i, SIS3350_RINGBUFFER_SAMPLE_LENGTH_ADC34, sis3350_odb[i].ch_group[1].wf_len) != SUCCESS ) return FE_ERR_HW;
      if ( sis3350_A32D32_write(i, 0x03000020, sis3350_odb[i].ch_group[1].wf_len) != SUCCESS ) return FE_ERR_HW;

      // Ringbuffer PreDelay register
      //if ( sis3350_A32D32_write(i, SIS3350_RINGBUFFER_PRE_DELAY_ADC12, sis3350_odb[i].ch_group[0].presamples) != SUCCESS ) return FE_ERR_HW;
      if ( sis3350_A32D32_write(i, 0x02000024, sis3350_odb[i].ch_group[0].presamples) != SUCCESS ) return FE_ERR_HW;
      //if ( sis3350_A32D32_write(i, SIS3350_RINGBUFFER_PRE_DELAY_ADC34, sis3350_odb[i].ch_group[1].presamples) != SUCCESS ) return FE_ERR_HW;
      if ( sis3350_A32D32_write(i, 0x03000024, sis3350_odb[i].ch_group[1].presamples) != SUCCESS ) return FE_ERR_HW;

#if 1
      // Direct Memory Sample Length register
      if ( sis3350_A32D32_write(i, 0x18, sis3350_odb[i].ch_group[0].wf_len) != SUCCESS ) return FE_ERR_HW;
#endif

#if 1
      // Gate Extend 
      if ( sis3350_A32D32_write(i, 0x2C, 64) != SUCCESS ) return FE_ERR_HW;
#endif


#if 1
      // Direct Memory Stop Mode Sample Wrap Length
      if ( sis3350_A32D32_write(i, 0x02000004, sis3350_odb[i].ch_group[0].wf_len) != SUCCESS ) return FE_ERR_HW;
      if ( sis3350_A32D32_write(i, 0x03000004, sis3350_odb[i].ch_group[1].wf_len) != SUCCESS ) return FE_ERR_HW;
#endif

#if 0
      // Read ACQUISITION STATUS register 0x10
      if ( sis3350_A32D32_read(i, 0x10, data) != SUCCESS ) return FE_ERR_HW;
      printf("acquisition register 0x10: 0x%08x\n",data);
#endif      

      // reset errors
      for ( unsigned int j=0; j<sis3350_n_channels; j++)
	sis3350_err[i][j] = 0;

    }
  

  // ==============================================================
  // *** Start sampling in auto bank switch mode ***
  // ==============================================================
  if ( sis3350_arm_all() != SUCCESS ) return FE_ERR_HW;

  return SUCCESS;

}

static INT sis3350_eor()
{

  // Disarm sampling logic
  if ( sis3350_disarm_all() != SUCCESS ) return FE_ERR_HW;

  // Stop sampling in auto bank switch mode
  //sis3350_disarm_all();

  u_int32_t addr ;
  u_int32_t data ;
  int return_code ;

  for (unsigned int i=0; i<sis3350_n_boards; i++)
    {
      
      if ( sis3350_odb[i].enabled == 0 ) continue;
      

      // ==============================================================
      // *** configure Control register 0x0 ***
      // ==============================================================

      data = 0x0;
      data += (1<<16); // Switch OFF user LED

      if ( sis3350_A32D32_write(i, SIS3350_CONTROL_STATUS, data) != SUCCESS ) return FE_ERR_HW;

    }
  
  return SUCCESS;

}


static INT sis3350_Read_ADC_Channel(const int board_nr,    // 
				    const int channel_nr,  // 0 to 3
				    const u_int32_t sample_start_addr,
				    const u_int32_t sample_length)
{
  
  unsigned int max_page_sample_length  = SIS3350_MAX_PAGE_SAMPLE_LENGTH;
  unsigned int page_sample_length_mask = max_page_sample_length - 1;
  
  unsigned int next_event_sample_start_addr =  (sample_start_addr &  (SIS3350_MAX_SAMPLE_LENGTH-1));
  unsigned int rest_event_sample_length     =  (sample_length ); 
  
  if ( rest_event_sample_length >= SIS3350_MAX_SAMPLE_LENGTH ) 
    {
      cm_msg(MERROR, "sis3350_read_ADC_channel","Board %i channel %i Event size is too large :%ui. Truncating data...\n",board_nr,channel_nr,rest_event_sample_length);
      rest_event_sample_length = SIS3350_MAX_SAMPLE_LENGTH;
      sis3350_err[board_nr][channel_nr] += BIT(0);
    }

  if ( rest_event_sample_length >= sis3350_ADC_max_data_size ) 
    {
      cm_msg(MERROR, "sis3350_read_ADC_channel","Board %i channel %i Event size is too large :%ui. Truncating data...\n",board_nr,channel_nr,rest_event_sample_length);
      rest_event_sample_length = sis3350_ADC_max_data_size;
      sis3350_err[board_nr][channel_nr] += BIT(0);
    }

  sis3350_ADC_data_size[board_nr][channel_nr] = rest_event_sample_length;
  
  int return_code = 0 ;
  do {
    
    unsigned int sub_event_sample_addr = (next_event_sample_start_addr & page_sample_length_mask);
    unsigned int sub_max_page_sample_length = max_page_sample_length - sub_event_sample_addr;
    unsigned int sub_event_sample_length;
    
    if ( rest_event_sample_length >= sub_max_page_sample_length )
      {
	sub_event_sample_length = sub_max_page_sample_length;
      }
    else 
      {
	sub_event_sample_length = rest_event_sample_length;
      }
    
    unsigned int sub_page_addr_offset =  (next_event_sample_start_addr >> 23) & 0xf ;
    
    unsigned int dma_request_nof_lwords     =  (sub_event_sample_length) / 2  ; // Lwords
    unsigned int dma_adc_addr_offset_bytes  =  (sub_event_sample_addr) * 2    ; // Bytes
    
    // set page
    if ( sis3350_A32D32_write(board_nr, SIS3350_ADC_MEMORY_PAGE_REGISTER, sub_page_addr_offset) != SUCCESS ) return FE_ERR_HW;

    // read		
    u_int32_t addr = sis3350_odb[board_nr].vme_base
      + SIS3350_ADC1_OFFSET 
      + (SIS3350_NEXT_ADC_OFFSET * channel_nr)
      + (dma_adc_addr_offset_bytes);

    u_int32_t req_nof_lwords = dma_request_nof_lwords ; //   
    u_int32_t got_nof_lwords = 0;

    return_code = vme_A32MBLT64_read(fd_vme, addr, sis3350_ADC_data[board_nr][channel_nr], req_nof_lwords, &got_nof_lwords);

    if(return_code != 0) 
      {
	printf("\nReturn Error vme_A32MBLT64_read: return_code = 0x%08x at addr = 0x%08x\n", return_code, addr );         
	printf("sub_req_lwords = 0x%08x  got_nof_lwords = 0x%08x\n", req_nof_lwords, got_nof_lwords );         
	return FE_ERR_HW;
      }

    if ( req_nof_lwords != got_nof_lwords )
      {
	printf("Length Error vme_A32MBLT64_read:   req_nof_lwords = 0x%08x  got_nof_lwords = 0x%08x\n", req_nof_lwords, got_nof_lwords );
	return FE_ERR_HW;
      }
    
    next_event_sample_start_addr =  next_event_sample_start_addr + sub_event_sample_length;
    rest_event_sample_length     =  rest_event_sample_length - sub_event_sample_length;
    
  } while ( (return_code == 0) && (rest_event_sample_length>0) );
  
  
  return SUCCESS;
}



static INT sis3350_readout(const int board_nr, const int bank_nr)
{
  return SUCCESS;
}


static INT sis3350_poll() 
{

  u_int32_t addr ;
  u_int32_t data ;
  int return_code ;

  for (unsigned int i=0; i<sis3350_n_boards; i++)
    {
      if ( sis3350_odb[i].enabled == 0 ) continue;      
    }

  return SUCCESS;

}

static int sis3350_check_event_ready(const unsigned int board_nr)
{

  u_int32_t data;
  
  if ( sis3350_A32D32_read(board_nr, SIS3350_ACQUISTION_CONTROL, data) != SUCCESS ) return FE_ERR_HW;   

  return ( (data & SIS3350_ACQ_STATUS_ARMED_FLAG ) == SIS3350_ACQ_STATUS_ARMED_FLAG  );
  
}

/** 
 * Check all boards for event_ready (Wait till all boards finish digitization)
 * Read out Stop Address Counter from all boards
 * 
 * @return SUCCESS if success
 */
static INT sis3350_ReadStopAddressCounterAllBoards()
{

  u_int32_t reg[sis3350_n_channels] = {SIS3350_ACTUAL_SAMPLE_ADDRESS_ADC1, SIS3350_ACTUAL_SAMPLE_ADDRESS_ADC2, SIS3350_ACTUAL_SAMPLE_ADDRESS_ADC3, SIS3350_ACTUAL_SAMPLE_ADDRESS_ADC4};

  u_int32_t data ;
  int return_code ;

  int timeout = 10000; /* how long to wait for event ready */
  int counter = 0;     /* time counter */
  int i; 
  int status;

  /* reset the event_ready flag */
  for (i=0; i<sis3350_n_boards; i++)
    {
      sis3350_event_ready[i] = 0;
    }

  /* make sure that all boards are ready */
  /* \todo break if a board is not ready after some time. */
  /* \todo handle error cases. */
  int all_boards_ready = 1;
  do {

    all_boards_ready = 1;

    for (i=0; i<sis3350_n_boards; i++) 
      {
	if ( ! sis3350_odb[i].enabled ) continue; 
	if ( sis3350_event_ready[i] ) continue;
	
	status = sis3350_check_event_ready(i);
	if ( status != 0 )
	  {
	    all_boards_ready = 0;
	  }
	else 
	  {
	    sis3350_event_ready[i] = 1;
	    
	    //******************************************
	    // Read Stop Sample (Address) Counter
	    //******************************************	    

	    for (int j=0; j<sis3350_n_channels; j++)
	      {
		if ( sis3350_A32D32_read(i, reg[j], data) != SUCCESS ) return FE_ERR_HW;   
		sis3350_last_sample_addr[i][j] = data ;
		printf("SIS3350: Board %i ADC %i stop sample: 0x%08x\n",i, j, data);
	      }	    
	  }
      } 

    counter++;

  } while ( !all_boards_ready && counter < timeout);
  

  if (  !all_boards_ready ) return FE_ERR_HW; 

  return SUCCESS;

}


// separate function, callable from sis3300 readtout
INT sis3350_readout_eob_early()
{
  u_int32_t addr ;
  u_int32_t data ;

  // *** Disarm (disable sample clock) ***
  sis3350_disarm_all();

#if 1
  // testing
  for (unsigned int i=0; i<sis3350_n_boards; i++)
    {
      if ( sis3350_odb[i].enabled == 0 ) continue;
      // read the multievent register
      if ( sis3350_A32D32_read(i, 0x24, data) != SUCCESS ) return FE_ERR_HW;   
      printf("SIS3350: Board %i Multi-event counter: %i\n",i, data);
    }
#endif


  // *** readout ADC data size ***
  if ( ! sis3350_ReadStopAddressCounterAllBoards() == SUCCESS ) return FE_ERR_HW;

  // ==============================================================
  // *** Start sampling on alternate block ***
  // ==============================================================  
  sis3350_sampling_bank_nr = (sis3350_sampling_bank_nr+1)%2;  
  if ( sis3350_arm_all() != SUCCESS ) return FE_ERR_HW;

  // Can start new segment while we are reading out the date from alternate 
  // memory bank
  send_ready_for_cycle();

  return SUCCESS;
}


static INT sis3350_readout_eob() 
{
  u_int32_t addr ;
  u_int32_t data ;
  int return_code ;

#if 0
  sis3350_A32D32_write(0, SIS3350_KEY_TRIGGER, 0x0);
  ss_sleep(10);
  sis3350_A32D32_write(0, SIS3350_KEY_TRIGGER, 0x0);
#endif

#ifdef AUTOBANK_SWITCH_MODE_DEF
  if ( sis3350_readout_eob_early() != SUCCESS ) return FE_ERR_HW;
#endif

  // *** readout data from ADC memories ***  
#if 1
  unsigned int readout_start_addr;  
  if ( sis3350_sampling_bank_nr == 0 ) 
    { 
      readout_start_addr = SIS3350_MAX_SAMPLE_LENGTH/2  ;   
    }
  else 
    { 
      readout_start_addr = 0  ;   
    }


  for (unsigned int i=0; i<sis3350_n_boards; i++)
    {
      if ( sis3350_odb[i].enabled == 0 ) continue;
      
      for ( unsigned int i_ch=0; i_ch<4; i_ch++ ) 
	{
	  
	  sis3350_ADC_data_size[i][i_ch] = 0;
	  if ( ! sis3350_odb[i].ch[i_ch].enabled  ) continue;
	  
	  // data block length
	  unsigned int readout_sample_length = (sis3350_last_sample_addr[i][i_ch] - readout_start_addr) ; // length in 16-bit words
	  
	  if (readout_sample_length != 0)  
	    {
	      printf("Readout:  i_mod = %d  i_ch = %d  readout_sample_start_addr = 0x%08x   readout_sample_length = 0x%08x\n", i, i_ch, readout_start_addr , readout_sample_length);
	      if (  sis3350_Read_ADC_Channel( i, 
					      i_ch, 
					      readout_start_addr, 
					      readout_sample_length ) 
		    != SUCCESS ) return FE_ERR_HW;
	    }
	}

    }
#endif
  

  return SUCCESS;

}
