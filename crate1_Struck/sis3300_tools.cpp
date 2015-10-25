static int sis3300_bank_nr[sis3300_n_boards]; // active bank nr., 0 or 1;

static const int  sis3300_event_size = 128; // event size (samples);
//static const int  sis3300_event_size = 4096; // event size (samples);
//static const int  sis3300_event_size = 12288; // event size (samples);
static const int  sis3300_max_number_of_events = 4096; // Max. number of evens for data buffers

static u_int32_t  sis3300_event_counter[sis3300_n_boards];
static u_int32_t *sis3300_ADC12_data   [sis3300_n_boards];
static u_int32_t *sis3300_ADC34_data   [sis3300_n_boards];
static u_int32_t *sis3300_ADC56_data   [sis3300_n_boards];
static u_int32_t *sis3300_ADC78_data   [sis3300_n_boards];
static u_int32_t *sis3300_TimeStamps   [sis3300_n_boards];
static u_int32_t *sis3300_Trigger_data [sis3300_n_boards];

INT sis3300_A32D32_write(const int board_nr, const u_int32_t reg, const u_int32_t data)
{
  
  u_int32_t addr = sis3300_odb[board_nr].vme_base + reg;

  int return_code = vme_A32D32_write(fd_vme, addr, data) ;

  if ( return_code != 0 ) 
    { 
      cm_msg(MERROR, "sis3300_A32D32_write()", "return_code = 0x%08x  at address = 0x%08x for board %i", return_code, addr, board_nr);
      return FE_ERR_HW;
    }

  return SUCCESS;

}

INT sis3300_A32D32_read(const int board_nr, const u_int32_t reg, u_int32_t &data)
{
  
  u_int32_t addr = sis3300_odb[board_nr].vme_base + reg;

  int return_code = vme_A32D32_read(fd_vme, addr, &data) ;

  if ( return_code != 0 ) 
    { 
      cm_msg(MERROR, "sis3300_A32D32_read()", "return_code = 0x%08x  at address = 0x%08x for board %i", return_code, addr, board_nr);
      return FE_ERR_HW;
    }

  return SUCCESS;

}

static INT sis3300_open()
{

  u_int32_t addr ;
  u_int32_t data ;
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
      if ( sis3300_A32D32_read(i, 0x4, data) != SUCCESS ) return FE_ERR_HW;
      printf(" SUCCESS!\nModul Identification register:  0x%08x\n", data );

      // *** Reset *** 
      if ( sis3300_A32D32_write(i, SIS3300_KEY_RESET, 0x0) != SUCCESS ) return FE_ERR_HW;
      
    }
  
  return SUCCESS;
}

#if 0
/* Arm sampling logic on Bank 1 or Bank 2*/
static INT sis3300_arm( const int board_nr )
{

  u_int32_t addr ;
  u_int32_t data ;
  int return_code ;

  addr = sis3300_odb[board_nr].vme_base + 0x10; 

  // arm on active bank
  data = (1<<sis3300_bank_nr[board_nr]);
      
  return_code =  vme_A32D32_write(fd_vme, addr, data) ;
  if ( return_code != 0 ) 
    { 
      cm_msg(MERROR, "sis3300_arm()", "vme_A32D32_write: return_code = 0x%08x  at address = 0x%08x, file %s, board %i", return_code, addr, __FILE__, board_nr);
      return FE_ERR_HW;
    }
      
  return SUCCESS;

}

/* Disarm sampling logic */
static INT sis3300_disarm_all()
{

  u_int32_t addr ;
  u_int32_t data ;
  int return_code ;

  for (unsigned int i=0; i<sis3300_n_boards; i++)
    {
      if ( sis3300_odb[i].enabled == 0 ) continue;

      addr = sis3300_odb[i].vme_base + 0x10; 

      data = 0x0;
      data += (1<<16); 
      data += (1<<17); 

      return_code =  vme_A32D32_write(fd_vme, addr, data) ;
      if ( return_code != 0 ) 
	{ 
	  cm_msg(MERROR, "sis3300_disarm_all", "vme_A32D32_write: return_code = 0x%08x  at address = 0x%08x, file %s, board %i", return_code, addr, __FILE__, i);
	  return FE_ERR_HW;
	}

    }

  return SUCCESS;
  
}
#endif

static INT sis3300_bank_full_clear(const int board_nr, const int bank_nr)
{

  u_int32_t addr ;
  u_int32_t data ;
  int return_code ;

  if ( bank_nr == 0 )
    {
      addr = sis3300_odb[board_nr].vme_base + 0x48;
    }
  else if ( bank_nr == 1 )
    {
      addr = sis3300_odb[board_nr].vme_base + 0x4C;
    }
  else 
    {
      return FE_ERR_HW;
    }

  return_code =  vme_A32D32_write(fd_vme, addr, 0x0 ) ;
  if ( return_code != 0 )    
    {
      cm_msg(MERROR, "sis3300_bank_full_clear()", "vme_A32D32_read:  return_code = 0x%08x  at address = 0x%08x file %s board %i", return_code, addr, __FILE__, board_nr);
      return FE_ERR_HW;
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
      printf("acquisition register 0x10: 0x%08x\n",data);
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
      
      sis3300_event_counter[i] = 0;

      // ==============================================================
      // *** configure Control register 0x0 ***
      // ==============================================================

      data = 0x0;
      data += (1<<5); // Activate Trigger Upon Armed and Started
      data += (1<<6); // Enable Internal Trigger Routing
      data += (1<<2); // Enable Trigger Output on Output ch. 1

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
      data += (1<<25);  // Disable P2 Start/Stop logic
      data += (1<<27);  // Disable external clock random mode
      data += (1<<31);  // Clear multiplexer mode
 
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

  return SUCCESS;

}



static INT sis3300_readout(const int board_nr, const int bank_nr)
{
  u_int32_t addr ;
  u_int32_t data ;
  int return_code ;
  u_int32_t got_lwords ;
  int no_of_lwords;

  printf("Reading out board %i, bank %i\n", board_nr, bank_nr);

  // *** read Event Counter (0x200010/0x200014 for Bank 1/2) ***  
  u_int32_t n_events = 0;
  if ( bank_nr == 0 )
    {
      addr = SIS3300_BANK1_GROUP1_EVT_COUNTER; // 0x00200010
    }
  else if ( bank_nr == 1 )
    {
      addr = SIS3300_BANK2_GROUP1_EVT_COUNTER; // 0x00200014
    }
  else
    {      
      return FE_ERR_HW;
    }

  if ( sis3300_A32D32_read(board_nr, addr, n_events) != SUCCESS ) return FE_ERR_HW;

#if 0
  n_events = 1024;
#endif
  printf("Bank %i event counter: %i\n", bank_nr+1, n_events);

  if ( sis3300_event_counter[board_nr] + n_events > sis3300_max_number_of_events )
    {
      cm_msg(MERROR, "sis3300_readout()", "not enough space to read data from digitizer. Plese increase prameter 'sis3300_max_number_of_events' (%i)",sis3300_max_number_of_events);
      return FE_ERR_HW;      
    }
  
  // *** read Event Time Stamp Directory (0x1000/0x2000 for Bank 1/2) ***
  if ( bank_nr == 0 )
    {
      addr = sis3300_odb[board_nr].vme_base + 0x1000; 
    }
  else
    {
      addr = sis3300_odb[board_nr].vme_base + 0x2000; 
    }
  return_code =   vme_A32BLT32_read(fd_vme, addr, (sis3300_TimeStamps[board_nr]+sis3300_event_counter[board_nr]), n_events, &got_lwords) ;
  if ( got_lwords != n_events )  
    { 
      cm_msg(MERROR, "sis3300_readout():TimeStamps", "number of got words != nr_of_events in board %i (%i/%i)",board_nr,got_lwords,n_events);
      return FE_ERR_HW;
    }

  // *** read Trigger Event Directory (0x101000/0x102000 for Bank 1/2) ***
  if ( bank_nr == 0 )
    {
      addr = sis3300_odb[board_nr].vme_base + 0x101000; 
    }
  else
    {
      addr = sis3300_odb[board_nr].vme_base + 0x102000; 
    }
  return_code =   vme_A32BLT32_read(fd_vme, addr, (sis3300_Trigger_data[board_nr]+sis3300_event_counter[board_nr]), n_events, &got_lwords) ;
  if ( got_lwords != n_events )  
    { 
      cm_msg(MERROR, "sis3300_readout():Triggers", "number of got words != nr_of_events in board %i (%i/%i)",board_nr,got_lwords,n_events);
      return FE_ERR_HW;
    }

  // *** Bank 1/2 memory (0x400000/0x600000) ***

  /* ADC 1,2 */
  if ( bank_nr == 0 )
    {
      addr = sis3300_odb[board_nr].vme_base + 0x400000; 
    }
  else
    {
      addr = sis3300_odb[board_nr].vme_base + 0x600000; 
    }
  no_of_lwords = n_events*sis3300_event_size; 
  return_code = vme_A32BLT32_read(fd_vme, addr, sis3300_ADC12_data[board_nr]+sis3300_event_counter[board_nr]*sis3300_event_size, no_of_lwords, &got_lwords) ;
  if ( got_lwords != no_of_lwords )  
    { 
      cm_msg(MERROR, "sis3300_readout():ADC12_data", "number of got words != nr_of_events in board %i (%i/%i)",board_nr,got_lwords,no_of_lwords);
      return FE_ERR_HW;
    }
  
  /* ADC 3,4 */
  if ( bank_nr == 0 )
    {
      addr = sis3300_odb[board_nr].vme_base + 0x480000; 
    }
  else
    {
      addr = sis3300_odb[board_nr].vme_base + 0x680000; 
    }

  return_code = vme_A32BLT32_read(fd_vme, addr, sis3300_ADC34_data[board_nr]+sis3300_event_counter[board_nr]*sis3300_event_size, no_of_lwords, &got_lwords) ;
  if ( got_lwords != no_of_lwords )  
    { 
      cm_msg(MERROR, "sis3300_readout():ADC34_data", "number of got words != nr_of_events in board %i (%i/%i)",board_nr,got_lwords,no_of_lwords);
      return FE_ERR_HW;
    }

  /* ADC 5,6 */
  if ( bank_nr == 0 )
    {
      addr = sis3300_odb[board_nr].vme_base + 0x500000; 
    }
  else
    {
      addr = sis3300_odb[board_nr].vme_base + 0x700000; 
    }

  return_code = vme_A32BLT32_read(fd_vme, addr, sis3300_ADC56_data[board_nr]+sis3300_event_counter[board_nr]*sis3300_event_size, no_of_lwords, &got_lwords) ;
  if ( got_lwords != no_of_lwords )  
    { 
      cm_msg(MERROR, "sis3300_readout():ADC56_data", "number of got words != nr_of_events in board %i (%i/%i)",board_nr,got_lwords,no_of_lwords);
      return FE_ERR_HW;
    }

  /* ADC 7,8 */
  if ( bank_nr == 0 )
    {
      addr = sis3300_odb[board_nr].vme_base + 0x580000; 
    }
  else
    {
      addr = sis3300_odb[board_nr].vme_base + 0x780000; 
    }

  return_code = vme_A32BLT32_read(fd_vme, addr, sis3300_ADC78_data[board_nr]+sis3300_event_counter[board_nr]*sis3300_event_size, no_of_lwords, &got_lwords) ;
  if ( got_lwords != no_of_lwords )  
    { 
      cm_msg(MERROR, "sis3300_readout():ADC78_data", "number of got words != nr_of_events in board %i (%i/%i)",board_nr,got_lwords,no_of_lwords);
      return FE_ERR_HW;
    }

#if 0
  // read address counter
  if ( sis3300_A32D32_read(board_nr, 0x200008, data) != SUCCESS ) return FE_ERR_HW;
  printf("address counter 0x200008: 0x%08x\n",data);
#endif

  sis3300_event_counter[board_nr] += n_events;

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
	  sis3300_readout(i, 0);

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
  

  // ==============================================================
  // *** Start sampling in auto bank switch mode ***
  // ==============================================================
  if ( sis3300_arm_all() != SUCCESS ) return FE_ERR_HW;

  return SUCCESS;

}
