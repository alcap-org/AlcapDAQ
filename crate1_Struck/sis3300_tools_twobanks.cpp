static int sis3300_bank_nr[sis3300_n_boards]; // active bank nr., 0 or 1;

static INT sis3300_open()
{

  u_int32_t addr ;
  u_int32_t data ;
  u_int32_t D ;
  int return_code ;

  for (unsigned int i=0; i<sis3300_n_boards; i++)
    {
      if ( sis3300_odb[i].enabled == 0 ) 
	{
	  printf("SIS3300 board %i disabled in ODB (base address 0x%04x)\n",i+1, sis3300_odb[i].vme_base);
	  continue;
	}     

      int sis3300_ADC_data_maxsize = sis3300_max_number_of_events*sis3300_event_size;
      sis3300_ADC12_data[i] = new u_int32_t[sis3300_ADC_data_maxsize];  
      sis3300_ADC34_data[i] = new u_int32_t[sis3300_ADC_data_maxsize]; 
      sis3300_ADC56_data[i] = new u_int32_t[sis3300_ADC_data_maxsize]; 
      sis3300_ADC78_data[i] = new u_int32_t[sis3300_ADC_data_maxsize];  

      if ( sis3300_ADC78_data[i] == NULL )
	{
	  cm_msg(MERROR, "sis3300_open()", "cannot allocate memory for data buffer, board %i", i);
	  return FE_ERR_HW;
	}

      sis3300_TimeStamps[i] = new u_int32_t[sis3300_max_number_of_events];
      if ( sis3300_TimeStamps[i] == NULL )
	{
	  cm_msg(MERROR, "sis3300_open()", "cannot allocate memory for TimeStamps buffer, board %i", i);
	  return FE_ERR_HW;
	}

      sis3300_Trigger_data[i] = new u_int32_t[sis3300_max_number_of_events];
      if ( sis3300_Trigger_data[i] == NULL )
	{
	  cm_msg(MERROR, "sis3300_open()", "cannot allocate memory for Trigger buffer, board %i", i);
	  return FE_ERR_HW;
	}

      printf("Opening board %i at base address 0x%08x ... ",i+1, sis3300_odb[i].vme_base);
      if ( sis3300_A32D32_read(i, 0x4, sis3300_ID[i]) != SUCCESS ) return FE_ERR_HW;
      printf(" SUCCESS!\nModul Identification register:  0x%08x\n", sis3300_ID[i] );

      // *** Reset *** 
      if ( sis3300_A32D32_write(i, SIS3300_KEY_RESET, 0x0) != SUCCESS ) return FE_ERR_HW;

      // ==============================================================
      // *** configure acquisition register 0x10 ***
      // ==============================================================

      data = 0x0;
      //data += (1<<0);  // Enable sample clock for Memory Bank 1
      //data += (1<<1);  // Enable sample clock for Memory Bank 2
      data += (1<<2);  // Enable auto bank switch mode
      //data += (1<<4);  // Enable autostart (in multievent mode only)
      data += (1<<5);  // Enable multi event mode (clock will be cleared at end of bank only)
#if 0
      data += (1<<7);   // Enable Stop Delay
#endif
      data += (1<<8);   // Enable front panel Start / Stop logic
      //data += (1<<10);  // Enable front panel Gate mode
      //data += (1<<17);  // Disable sample clock for Memory Bank 2
      //data += (1<<18);  // Disable autobank switch mode
      //data += (1<<20);  // Disable autostart in multievent mode
      //data += (1<<22);  // Disable external start delay
      //data += (1<<23);  // Disable external stop delay
      //data += (1<<24);  // Disable Front Panel Start/Stop logic
      data += (1<<25);  // Disable P2 Start/Stop logic
      data += (1<<27);  // Disable external clock random mode
      data += (1<<31);  // Clear multiplexer mode

      // clock source
      D = sis3300_odb[i].clock_src & 0x7;
      data += (D<<12);

      if ( sis3300_A32D32_write(i, SIS3300_ACQ_CONTROL, data) != SUCCESS ) return FE_ERR_HW;
     
    }
  
  return SUCCESS;
}

static INT sis3300_arm_all()
{

  u_int32_t addr ;
  u_int32_t data ;
  int return_code ;

  for (unsigned int i=0; i<sis3300_n_boards; i++)
    {

      sis3300_bank_nr[i] = 0;

      if ( sis3300_odb[i].enabled == 0 ) continue;

      // *** clear BANK FULL flags ***
      sis3300_bank_full_clear(i, 0);
      sis3300_bank_full_clear(i, 1);

#if 0      
      // *** Reconfigure the acquisition register 0x10 ***
      data = 0x0;
      data += (1<<4);  // Enable autostart (in multievent mode only)
      if ( sis3300_A32D32_write(i, SIS3300_ACQ_CONTROL, data) != SUCCESS ) return FE_ERR_HW;
#endif

      // *** Re-Enable  auto bank switch mode to clear event counter ***
      //data = 0x0;
      //data += (1<<18);  // Disable auto bank switch mode
      //if ( sis3300_A32D32_write(i, SIS3300_ACQ_CONTROL, data) != SUCCESS ) return FE_ERR_HW;

      // *** reset time counter ***
      if ( sis3300_A32D32_write(i, 0x38, 0x0 ) != SUCCESS ) return FE_ERR_HW;
  
      data = 0x0;
      data += (1<<2);  // Enable auto bank switch mode
      data += (1<<0);  // Enable Sample Clock  For Memory Bank 1
      data += (1<<1);  // Enable Sample Clock  For Memory Bank 2
      if ( sis3300_A32D32_write(i, SIS3300_ACQ_CONTROL, data) != SUCCESS ) return FE_ERR_HW;


#if 0
      // Limit the max. no of events for testing
      // This does not work for 
      if ( sis3300_A32D32_write(i, 0x10002C, 8) != SUCCESS ) return FE_ERR_HW;
#endif

      // Start sampling in auto bank switch mode      
      if ( sis3300_A32D32_write(i, 0x40, 0x0) != SUCCESS ) return FE_ERR_HW;
    
#if 1
      // Enable autostart mode after issuing the "Start sampling in auto bank switch mode
      // Otherwise the board starts data taking autmatically (prior to MIDAS segment)
      data = 0x0;
      data += (1<<4);  // Enable autostart (in multievent mode only)
      if ( sis3300_A32D32_write(i, SIS3300_ACQ_CONTROL, data) != SUCCESS ) return FE_ERR_HW;
#endif 

#if 0
      // Start sampling in auto bank switch mode      
      if ( sis3300_A32D32_write(i, 0x40, 0x0) != SUCCESS ) return FE_ERR_HW;
#endif
    }
      
  return SUCCESS;

}


static INT sis3300_disarm_all()
{

  u_int32_t addr ;
  u_int32_t data ;
  int return_code ;

  for (unsigned int i=0; i<sis3300_n_boards; i++)
    {      
      if ( sis3300_odb[i].enabled == 0 ) continue;

#if 1
      // Read ACQUISITION STATUS register 0x10
      if ( sis3300_A32D32_read(i, 0x10, data) != SUCCESS ) return FE_ERR_HW;
      printf("acquisition register 0x10: 0x%08x\n",data);
#endif

      // Stop sampling in auto bank switch mode

      // DISABLE AUTOSTART
      data = 0x0; 
      data += (1<<20); // Disable autostart in multievent mode
      //data += (1<<16); // Disable sample clock for memory bank 1
      //data += (1<<18); // Disable auto bank switch node
      if ( sis3300_A32D32_write(i, 0x10, data) != SUCCESS ) return FE_ERR_HW;

      // KEY_STOP_AUTO_BANK_SWITCH
      if ( sis3300_A32D32_write(i, 0x44, 0x0) != SUCCESS ) return FE_ERR_HW;

      // KEY_STOP
      if ( sis3300_A32D32_write(i, 0x34, 0x0) != SUCCESS ) return FE_ERR_HW;

      // DISABLE SAMPLE CLOCK
      data = 0x0; 
      //data += (1<<1); // Enable Sample Clock  For Memory Bank 2
      data += (1<<18); // Disable auto bank switch node
      //data += (1<<20); // Disable autostart in multievent mode
      data += (1<<16); // Disable sample clock for memory bank 1
      data += (1<<17); // Disable sample clock for memory bank 2
      //data += (1<<21); // Disable multi event mode
      if ( sis3300_A32D32_write(i, 0x10, data) != SUCCESS ) return FE_ERR_HW;
#if 1
      // Read ACQUISITION STATUS register 0x10
      if ( sis3300_A32D32_read(i, 0x10, data) != SUCCESS ) return FE_ERR_HW;
      printf("SIS3300 board %i Acquisition register 0x10: 0x%08x\n",i,data);
#endif

    }  

  return SUCCESS;
  
}

static INT sis3300_bor()
{

  u_int32_t addr ;
  u_int32_t data ;
  int return_code ;

  for (unsigned int i=0; i<sis3300_n_boards; i++)
    {
      
      if ( sis3300_odb[i].enabled == 0 ) continue;
      
      // reset the event counter
      sis3300_event_counter[i] = 0;

      // reset errors
      sis3300_err[i] = 0;


      // ==============================================================
      // *** configure Control register 0x0 ***
      // ==============================================================

      data = 0x0;
      data += (1<<0);  // Switch ON user LED
      data += (1<<5);  // Activate Trigger Upon Armed and Started
      data += (1<<2);  // Enable Trigger Output on Output ch. 1
      data += (1<<6);  // Enable Internal Trigger Routing
      data += (1<<12); // Set Don't clear time stamp bit
      
      if ( sis3300_A32D32_write(i, SIS3300_CONTROL, data) != SUCCESS ) return FE_ERR_HW;

      // ==============================================================
      // *** configure acquisition register 0x10 ***
      // ==============================================================

      data = 0x0;
      //data += (1<<0);  // Enable sample clock for Memory Bank 1
      //data += (1<<1);  // Enable sample clock for Memory Bank 2
      data += (1<<2);  // Enable auto bank switch mode
      //data += (1<<4);  // Enable autostart (in multievent mode only)
      data += (1<<5);  // Enable multi event mode (clock will be cleared at end of bank only)
#if 0
      data += (1<<7);   // Enable Stop Delay
#endif
      data += (1<<8);   // Enable front panel Start / Stop logic
      //data += (1<<10);  // Enable front panel Gate mode
      //data += (1<<17);  // Disable sample clock for Memory Bank 2
      //data += (1<<18);  // Disable autobank switch mode
      //data += (1<<20);  // Disable autostart in multievent mode
      //data += (1<<22);  // Disable external start delay
      //data += (1<<23);  // Disable external stop delay
      //data += (1<<24);  // Disable Front Panel Start/Stop logic
      //data += (1<<25);  // Disable P2 Start/Stop logic
      //data += (1<<27);  // Disable external clock random mode
      //data += (1<<31);  // Clear multiplexer mode

      // clock source
      // Don't whant to do this on every run!
      //D = sis3300_odb[i].clock_src & 0x7;
      //data += (D<<12);

 
      if ( sis3300_A32D32_write(i, SIS3300_ACQ_CONTROL, data) != SUCCESS ) return FE_ERR_HW;

      // ==============================================================
      // *** configure Event Configuration register 0x100000 ***
      // ==============================================================
      
      data = 0x1000; // bit 12 has to be 1
      //data += 0x7;   // 1024 Events each 128 samples; Disable Wrap around mode
      data += (1<<3);  // Enable Wrap around mode
      //data += (1<<1);  //   32 Events 4K Samples
      data += 0x7;   // 1024 Events each 128 samples
      
      if ( sis3300_A32D32_write(i, SIS3300_EVT_CONFIG, data) != SUCCESS ) return FE_ERR_HW;

      // ==============================================================
      // *** configure Threshold Registers 0x200004 - 0x380004 ***
      // ==============================================================
      
      // *** ADC group 1 ***

      data = 0x84008400 ; 	/* ADC1: LE (lower_equal 0x400); ADC2 : LE (lower_equal 0x400) */

      if ( sis3300_A32D32_write(i, SIS3300_GROUP1_THRESHOLD, data) != SUCCESS ) return FE_ERR_HW;
	
      // ==============================================================
      // *** configure Trigger Setup Registers 0x200028 - 0x380028 ***
      // ==============================================================
      
      // *** ADC group 1 ***

      data = 0x0; // disabled
      data += (1<<28); // Enable Pulse Mode
      data += 0x00090000; // Pulse = 10 clocks
      data += (1<<24); // Enable N M mode
      data += 0x00000008;  // 9 clocks under threshold
      data += 0x00000006;  // 9 clocks over  threshold
      
      if ( sis3300_A32D32_write(i, 0x200028, data) != SUCCESS ) return FE_ERR_HW;

      // ==============================================================
      // *** Arm sampling logic ***
      // ==============================================================
      //if ( sis3300_arm( i ) != SUCCESS ) return FE_ERR_HW;

#if 1
      // Stop Delay
      //if ( sis3300_A32D32_write(i, 0x18, 500) != SUCCESS ) return FE_ERR_HW;
      if ( sis3300_A32D32_write(i, 0x18, 0) != SUCCESS ) return FE_ERR_HW;
      //if ( sis3300_A32D32_write(i, 0x18, 50) != SUCCESS ) return FE_ERR_HW;
#endif
      
    }
  

  // ==============================================================
  // *** Start sampling in auto bank switch mode ***
  // ==============================================================
  if ( sis3300_arm_all() != SUCCESS ) return FE_ERR_HW;

  return SUCCESS;

}

static INT sis3300_eor()
{

  // Disarm sampling logic
  //if ( sis3300_disarm_all() != SUCCESS ) return FE_ERR_HW;

  // Stop sampling in auto bank switch mode
  sis3300_disarm_all();

  // switch off user LED
  for (unsigned int i=0; i<sis3300_n_boards; i++)
    {
      if ( sis3300_odb[i].enabled == 0 ) continue;
      if ( sis3300_A32D32_write(i, 0x0, (1<<16)) != SUCCESS ) return FE_ERR_HW;
    }

  return SUCCESS;

}


static INT sis3300_poll() 
{

  u_int32_t addr ;
  u_int32_t data ;
  int return_code ;

  for (unsigned int i=0; i<sis3300_n_boards; i++)
    {
      if ( sis3300_odb[i].enabled == 0 ) continue;
      
      // *** Read Acquisition Status Register 0x10 ***  
      if ( sis3300_A32D32_read(i, SIS3300_ACQ_STATUS, data ) != SUCCESS ) return FE_ERR_HW;;
      
      /*
      printf("Acquisition Status Register: %i\n",data);      
      printf("Bank 1 busy status: %i\n",data&(1<<20));
      printf("Bank 2 busy status: %i\n",data&(1<<22));
      printf("Bank 1 full status: %i\n",data&(1<<21));
      printf("Bank 2 full status: %i\n",data&(1<<23));
      */
      
      int bank_1_full = (data>>21)&1;
      int bank_2_full = (data>>23)&1;

      if ( bank_1_full ) 
	{
	  printf("================================>  Bank 1 full\n");

	  // Readout data from Bank 1
	  //if ( sis3300_readout(i, 0) != SUCCESS ) cm_transition(TR_STOP, 0, NULL, 0, ASYNC, FALSE);;
	  if ( sis3300_readout(i, 0) != SUCCESS ) return FE_ERR_HW;

	  sis3300_bank_nr[i] = 1;

	  // Clear BANK 1 FULL flag
	  if ( sis3300_bank_full_clear(i, 0) != SUCCESS ) return FE_ERR_HW;

	  // TODO: Read status reg. and ensure that BANK 2 did not get full
	  //       while we were reading BANK 1

	  if ( sis3300_A32D32_read(i, SIS3300_ACQ_STATUS, data ) != SUCCESS ) return FE_ERR_HW;
	  bank_2_full = (data>>23)&1;
	  
	}

      if ( bank_2_full ) 
	{
	  printf("===================================> Bank 2 full\n");

	  // Readout data from Bank 2
	  sis3300_readout(i, 1);
	  
	  sis3300_bank_nr[i] = 0;

	  // Clear BANK 2 FULL flag
	  if ( sis3300_bank_full_clear(i, 1) != SUCCESS ) return FE_ERR_HW;
	  
	  // TODO: Read status reg. and ensure that BANK 1 did not get full
	  //       while we were reading BANK 2
	  if ( sis3300_A32D32_read(i, SIS3300_ACQ_STATUS, data ) != SUCCESS ) return FE_ERR_HW;
	  bank_1_full = (data>>23)&1;

	}

      if ( bank_1_full && bank_2_full )
	{
	  // *** TODO: Problem: board memory overflow.
	  // handle this situation
	  ;
	}

      
    }

  return SUCCESS;

}


static INT sis3300_readout_eob() 
{
  u_int32_t addr ;
  u_int32_t data ;
  int return_code ;

  // *** Disarm (disable sample clock) ***
  sis3300_disarm_all();

#if 0
  // Check which if the banks is full. 
  // If none of alternate banks

#endif

#if 1
  for (unsigned int i=0; i<sis3300_n_boards; i++)
    {
      if ( sis3300_odb[i].enabled == 0 ) continue;

      // *** readout from active bank ***
      if ( sis3300_readout(i, sis3300_bank_nr[i]) != SUCCESS ) return FE_ERR_HW; 

      // *** Read Acquisition Status Register 0x10 ***  
      if ( sis3300_A32D32_read(i, SIS3300_ACQ_STATUS, data ) != SUCCESS ) return FE_ERR_HW;;      
      int bank_1_full = (data>>21)&1;
      int bank_2_full = (data>>23)&1;

      if ( sis3300_bank_nr[i] == 0 && bank_1_full )
	if ( sis3300_readout(i, 1) != SUCCESS ) return FE_ERR_HW; 

      if ( sis3300_bank_nr[i] == 1 && bank_2_full )
	if ( sis3300_readout(i, 0) != SUCCESS ) return FE_ERR_HW;       

    }
#endif

  // *** Start sampling in auto bank switch mode ***
  if ( sis3300_arm_all() != SUCCESS ) return FE_ERR_HW;

  return SUCCESS;

}
