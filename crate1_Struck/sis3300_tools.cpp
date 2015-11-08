#define BIT(x) (1<<x)

static u_int32_t sis3300_ID[sis3300_n_boards]; // Module ID and firmware revision register

static const int  sis3300_event_size = 128; // event size (samples);
//static const int  sis3300_event_size = 256; // event size (samples);
//static const int  sis3300_event_size = 1024; // event size (samples);
//static const int  sis3300_event_size = 4096; // event size (samples);
//static const int  sis3300_event_size = 12288; // event size (samples);
static const int  sis3300_max_number_of_events = 3072; // Max. number of evens for data buffers

static u_int32_t  sis3300_event_counter[sis3300_n_boards];
static u_int32_t *sis3300_ADC12_data   [sis3300_n_boards];
static u_int32_t *sis3300_ADC34_data   [sis3300_n_boards];
static u_int32_t *sis3300_ADC56_data   [sis3300_n_boards];
static u_int32_t *sis3300_ADC78_data   [sis3300_n_boards];
static u_int32_t *sis3300_TimeStamps   [sis3300_n_boards];
static u_int32_t *sis3300_Trigger_data [sis3300_n_boards];
static u_int32_t  sis3300_err          [sis3300_n_boards];

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


static INT sis3300_readout(const int board_nr, const int bank_nr)
{
  u_int32_t addr ;
  u_int32_t data ;
  int return_code ;
  u_int32_t got_lwords ;
  int no_of_lwords;

  printf("SIS3300: Reading out board %i, bank %i\n", board_nr, bank_nr);

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
  printf("SIS3300 Board %i Bank %i event counter: %i\n", board_nr, bank_nr+1, n_events);

  if ( sis3300_event_counter[board_nr] + n_events > sis3300_max_number_of_events )
    {
      cm_msg(MERROR, "sis3300_readout()", "not enough space to read data from digitizer. Plese increase parameter 'sis3300_max_number_of_events' (%i)",sis3300_max_number_of_events);
      sis3300_err[board_nr] += BIT(0);
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
