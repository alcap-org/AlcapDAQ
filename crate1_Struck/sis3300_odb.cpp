static const unsigned int sis3300_n_boards   = 6; // must be < 10; modify midas bank name otherwise!
static const unsigned int sis3300_n_channels = 8;

extern HNDLE hDB;

typedef struct {
  DWORD     vme_base;
  BOOL      enabled;
  WORD      clock_src;
  BYTE      averaging;
  WORD      stop_delay;
  struct {
    BOOL    enabled;
    DWORD   threshold;
    BYTE    thr_logic;
  } ch[sis3300_n_channels];
} S_SIS3300_ODB_DEF;

static S_SIS3300_ODB_DEF sis3300_odb[sis3300_n_boards];

#define S_SIS3300_ODB_STR "\
[.]\n\
VME Base = DWORD : 1073741824\n\
Enabled = BOOL : n\n\
Clock src (0-int 6-ext) = WORD : 0\n\
Averaging (0-7) = BYTE : 0\n\
Stop Delay (0-65536) = WORD : 0\n\
[Ch 1]\n\
Enabled = BOOL : n\n\
Threshold (0-4095) = DWORD : 4095\n\
Threshold logic (0:GT 1:LE)= BYTE : 1\n\
[Ch 2]\n\
Enabled = BOOL : n\n\
Threshold (0-4095) = DWORD : 4095\n\
Threshold logic (0:GT 1:LE)= BYTE : 1\n\
[Ch 3]\n\
Enabled = BOOL : n\n\
Threshold (0-4095) = DWORD : 4095\n\
Threshold logic (0:GT 1:LE)= BYTE : 1\n\
[Ch 4]\n\
Enabled = BOOL : n\n\
Threshold (0-4095) = DWORD : 4095\n\
Threshold logic (0:GT 1:LE)= BYTE : 1\n\
[Ch 5]\n\
Enabled = BOOL : n\n\
Threshold (0-4095) = DWORD : 4095\n\
Threshold logic (0:GT 1:LE)= BYTE : 1\n\
[Ch 6]\n\
Enabled = BOOL : n\n\
Threshold (0-4095) = DWORD : 4095\n\
Threshold logic (0:GT 1:LE)= BYTE : 1\n\
[Ch 7]\n\
Enabled = BOOL : n\n\
Threshold (0-4095) = DWORD : 4095\n\
Threshold logic (0:GT 1:LE)= BYTE : 1\n\
[Ch 8]\n\
Enabled = BOOL : n\n\
Threshold (0-4095) = DWORD : 4095\n\
Threshold logic (0:GT 1:LE)= BYTE : 1\n\
"

static INT sis3300_odb_init()
{
  INT status = SUCCESS;

  for (unsigned int i=0; i<sis3300_n_boards; i++)
    {

      /* Link to Database */
      char key_str[1024];
      sprintf( key_str, "/Equipment/Crate %i/Settings/SIS3300/Board %i", crate_number, i+1);

      int status = db_check_record(hDB, 0, key_str, S_SIS3300_ODB_STR, FALSE);

      if ( status==DB_NO_KEY || status==DB_STRUCT_MISMATCH ) 
	{
	  status = db_create_record(hDB, 0, key_str, S_SIS3300_ODB_STR);
	  cm_msg(MINFO, "Creating key [%s] with status: %d \n", key_str, status);
	} 
      else if ( status == DB_INVALID_HANDLE ) 
	{
	  cm_msg(MERROR, "Cannot connect to DB key [%s]",key_str);
	  return FE_ERR_HW;	  
	}

      HNDLE hKey;
      status = db_find_key(hDB, 0, key_str, &hKey);
      status = db_open_record(hDB, hKey, &(sis3300_odb[i]), sizeof(sis3300_odb[0]), MODE_READ, NULL, NULL);

    }

  return status;

}

