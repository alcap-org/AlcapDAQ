static const unsigned int sis3350_n_boards   = 2; // must be < 10; modify midas bank name otherwise!
static const unsigned int sis3350_n_channels = 4;

extern HNDLE hDB;

typedef struct {
  DWORD     vme_base;
  BOOL      enabled;
  WORD      clock_src;
  struct {
    BOOL    enabled;
    WORD    gain;     // 0-Low; 1-High
    WORD    VGA;      // 0-128
    DWORD   offset;   // DAC offset; 0x8000 == middle
    BOOL    trigger_enable; // 1-trigger Enabled; 0-trigger disabled;
    WORD    trigger_logic;  // 1-GT; 0-LT 
    WORD    trigger_mode;   // 0-threshold; 1-FIR    
    DWORD   trigger_threshold_ON;      // trigger threshold / Threshold value ON for Gate mode
    DWORD   trigger_threshold_OFF;     // Threshold value OFF for Gate mode
    WORD    trigger_pulse_length;   // 0-256
    WORD    trigger_sumG;   // 0-32
    WORD    trigger_P;      // 0-32
  } ch[sis3350_n_channels];
  struct {
    DWORD   wf_len;         // Ring Buffer Sample Length (0-16376 or 65328), mult 8
    DWORD   presamples;     // mult 2    
  } ch_group[2];
} S_SIS3350_ODB_DEF;

static S_SIS3350_ODB_DEF sis3350_odb[sis3350_n_boards];

#define S_SIS3350_ODB_STR "\
[.]\n\
VME Base = DWORD : 805306368\n\
Enabled = BOOL : n\n\
Clock src (0-int 3-ext) = WORD : 0 \n\
[Ch 1]\n\
Enabled = BOOL : n\n\
gain (0-Low,1-High) = WORD : 0\n\
VGA (0-128) = WORD : 0\n\
offset (0-65536) = DWORD : 32768\n\
trig enable = BOOL : n\n\
trig logic (1-GT; 0-LT) = WORD : 1\n\
trig mode (0-thr,1-FIR) = WORD : 0\n\
trig thresh ON (0-4095) = DWORD : 4095\n\
trig thresh OFF (0-4095) = DWORD : 4095\n\
trig pulse len (0-256)= WORD : 100\n\
trig sumG (0-32)= WORD : 10\n\
trig P (0-32) = WORD : 10\n\
[Ch 2]\n\
Enabled = BOOL : n\n\
gain (0-Low,1-High) = WORD : 0\n\
VGA (0-128) = WORD : 0\n\
offset (0-65536) = DWORD : 32768\n\
trig enable = BOOL : n\n\
trig logic (1-GT; 0-LT) = WORD : 1\n\
trig mode (0-thr,1-FIR) = WORD : 0\n\
trig thresh ON (0-4095) = DWORD : 4095\n\
trig thresh OFF (0-4095) = DWORD : 4095\n\
trig pulse len (0-256)= WORD : 100\n\
trig sumG (0-32)= WORD : 10\n\
trig P (0-32) = WORD : 10\n\
[Ch 3]\n\
Enabled = BOOL : n\n\
gain (0-Low,1-High) = WORD : 0\n\
VGA (0-128) = WORD : 0\n\
offset (0-65536) = DWORD : 32768\n\
trig enable = BOOL : n\n\
trig logic (1-GT; 0-LT) = WORD : 1\n\
trig mode (0-thr,1-FIR) = WORD : 0\n\
trig thresh ON (0-4095) = DWORD : 4095\n\
trig thresh OFF (0-4095) = DWORD : 4095\n\
trig pulse len (0-256)= WORD : 100\n\
trig sumG (0-32)= WORD : 10\n\
trig P (0-32) = WORD : 10\n\
[Ch 4]\n\
Enabled = BOOL : n\n\
gain (0-Low,1-High) = WORD : 0\n\
VGA (0-128) = WORD : 0\n\
offset (0-65536) = DWORD : 32768\n\
trig enable = BOOL : n\n\
trig logic (1-GT; 0-LT) = WORD : 1\n\
trig mode (0-thr,1-FIR) = WORD : 0\n\
trig thresh ON (0-4095) = DWORD : 4095\n\
trig thresh OFF (0-4095) = DWORD : 4095\n\
trig pulse len (0-256)= WORD : 100\n\
trig sumG (0-32)= WORD : 10\n\
trig P (0-32) = WORD : 10\n\
[Ch 1,2]\n\
wf len (mult 8) = DWORD : 64\n\
presamples (mult 2) = DWORD : 8\n\
[Ch 3,4]\n\
wf len (mult 8) = DWORD : 64\n\
presamples (mult 2) = DWORD : 8\n\
"

static INT sis3350_odb_init()
{
  INT status = SUCCESS;

  for (unsigned int i=0; i<sis3350_n_boards; i++)
    {

      /* Link to Database */
      char key_str[1024];
      sprintf( key_str, "/Equipment/Crate %i/Settings/SIS3350/Board %i", crate_number, i+1);

      int status = db_check_record(hDB, 0, key_str, S_SIS3350_ODB_STR, FALSE);

      if ( status==DB_NO_KEY || status==DB_STRUCT_MISMATCH ) 
	{
	  status = db_create_record(hDB, 0, key_str, S_SIS3350_ODB_STR);
	  cm_msg(MINFO, "Creating key [%s] with status: %d \n", key_str, status);
	} 
      else if ( status == DB_INVALID_HANDLE ) 
	{
	  cm_msg(MERROR, "Cannot connect to DB key [%s]",key_str);
	  return FE_ERR_HW;	  
	}

      HNDLE hKey;
      status = db_find_key(hDB, 0, key_str, &hKey);
      status = db_open_record(hDB, hKey, &(sis3350_odb[i]), sizeof(sis3350_odb[0]), MODE_READ, NULL, NULL);

    }

  return status;

}

