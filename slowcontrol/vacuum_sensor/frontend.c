#include <stdio.h>
#include "midas.h"

#include <termios.h> // Serial port configuration
#include <fcntl.h>  // Defines modes for opening serial port
#include <unistd.h>  // Read and write to serial port
#include <string.h>  // Set char[] to all same value
#include <stdlib.h>  // Take char array
#include <errno.h>   // Set by read, write, and termios functions

/* make frontend functions callable from the C framework */
#ifdef __cplusplus
extern "C" {
#endif

INT crate_number=0;

HNDLE hDB;
HNDLE hKey;
int size;
char kName_Alarm[128] = "/Equipment/VacuumMonitor/Variables/Alarm_Triggered";

INT vacuum_gauge_read(char *pevent, INT off);

/*-- Globals -------------------------------------------------------*/

/* The frontend name (client name) as seen by other MIDAS clients   */
char *frontend_name = "VacuumMonitor";
/* The frontend file name, don't change it */
char *frontend_file_name = __FILE__;

/* frontend_loop is called periodically if this variable is TRUE    */
BOOL frontend_call_loop = TRUE;

/* a frontend status page is displayed with this frequency in ms    */
INT display_period = 1000;

/* maximum event size produced by this frontend */
INT max_event_size = 10000;

/* maximum event size for fragmented events (EQ_FRAGMENTED) */
INT max_event_size_frag = 5 * 1024 * 1024;

/* buffer size to hold events */
INT event_buffer_size = 10 * 10000;

// Globals to this file only
static HNDLE kAlarm;
static int vacuum;
static struct termios vacuum_config;
static char vacuum_port[] = "/dev/ttyS0";
// Characters we will use
static const char NUL = '\x00'; // For NULL terminated strings
static const char ACK = '\x06'; // For acknowledging
static const char NAK = '\x15'; // For something wrong
static const char ENQ = '\x05'; // For asking for something
static const char ETX = '\x03'; // For clearing input buffer (of gauge?)
static const char LF  = '\n';   // New line
static const char CR  = '\r';   // Carriage return.
static const char EOM[] = "\n\r"; // End of message
static const char INQ_MODE[] = "PR1\r\n";
static const char CONT_MOD[] = "COM,1\r\n";


/*-- Equipment list ------------------------------------------------*/

BANK_LIST vacuum_bank_list[] = {
  { "PRS0", TID_FLOAT,  1, NULL },
  { "PRM0", TID_INT,    1, NULL },
  { "" },
};

#undef USE_INT

EQUIPMENT equipment[] = {
   {"VacuumMonitor",       /* equipment name */
     { 24, 0,              /* event ID, trigger mask */
       "SYSTEM",           /* event buffer */
       EQ_PERIODIC,        /* equipment type */
       0,                  /* event source */
       "MIDAS",            /* format */
       TRUE,               /* enabled */
       RO_ALWAYS | RO_ODB, /* read all the time */
       1000,               /* reads spaced by this many ms */
       0,                  /* stop run after this event limit */
       0,                  /* number of sub events */
       0,                  /* log history every event */
       "", "", "", },
    vacuum_gauge_read,     /* readout routine */
    NULL, NULL, 
    vacuum_bank_list,
   },

   {""}
};


#ifdef __cplusplus
}
#endif

/*-- Dummy routines ------------------------------------------------*/

INT frontend_early_init()
{
  return CM_SUCCESS;
}

INT pre_begin_of_run()
{
  return CM_SUCCESS;
}

INT poll_event(INT source[], INT count, BOOL test)
{
   return 1;
};
INT interrupt_configure(INT cmd, INT source[], PTYPE adr)
{
   return 1;
};

/*-- Frontend Init -------------------------------------------------*/

INT frontend_init()
{

  int ret, err;
  char resp[256];

  // Create alarm and value if doesn't exist
  cm_get_experiment_database(&hDB, NULL);  
  if (db_find_key(hDB, 0, kName_Alarm, &kAlarm) != SUCCESS) {
    db_create_key(hDB, 0, kName_Alarm, TID_INT);
    db_find_key(hDB, 0, kName_Alarm, &kAlarm);
  }

  // Open serial port for vacuum
  vacuum = open(vacuum_port, O_RDWR | O_NOCTTY);
  printf("Opening vacuum gauge port...");
  if (vacuum == -1) {
    err = errno;
    printf("failed!\n");
    cm_msg(MERROR, "vacuum_gauge_init",
	   "Error opening vacuum gauge (errno %d)", err);
    return FE_ERR_HW;
  }
  printf("done.\n");
  printf("Getting current serial port attributes...");
  ret = tcgetattr(vacuum, &vacuum_config);
  if (ret == -1) {
    err = errno;
    printf("failed!\n");
    cm_msg(MERROR, "vacuum_gauge_init",
	   "Error getting vacuum gauge serial port attributes (errno %d)", err);
    return FE_ERR_HW;
  }
  printf("done.\n");

  /**********************************************************/
  /*** Serial Modes ***/
  /* Disable everything */
  vacuum_config.c_iflag &= ~(BRKINT | ICRNL | IGNBRK | IGNCR |
			     IGNPAR | INLCR | INPCK | ISTRIP |
			     IUCLC  | IXANY | IXOFF | IXON   |
			     PARMRK);
  vacuum_config.c_oflag &= ~(OPOST);
  vacuum_config.c_cflag &= ~(CLOCAL | CREAD  | CSIZE | CSTOPB |
			     HUPCL  | PARENB | PARODD);
  vacuum_config.c_lflag &= ~(ECHO   | ECHOE  | ECHOK | ECHONL |
			     ICANON | IEXTEN | ISIG  | NOFLSH |
			     TOSTOP | XCASE);
  /* Input */
  // Nothing.
  /* Output */
  // Nothing.
  /* Local */
  vacuum_config.c_cflag |= CLOCAL;        // Do not become owner of device?
  vacuum_config.c_cflag |= CREAD;         // Enable reading
  vacuum_config.c_cflag |= (CSIZE & CS8); // 8-bit bytes
  cfsetispeed(&vacuum_config, B9600);     // Default baudrate for vacuum is 9600
  cfsetospeed(&vacuum_config, B9600);     // Default baudrate for vacuum is 9600
  /* Control */
  vacuum_config.c_cc[VTIME] = 10; // Interbyte timeout of 1 sec
  vacuum_config.c_cc[VMIN]  = 28; // Longest msg is 2 measurements, 28 chars
  /**********************************************************/

  // Apply the attributes
  printf("Setting new serial port attributes...");
  ret = tcsetattr(vacuum, TCSAFLUSH, &vacuum_config); // Flush the output, discard input, and apply changes
  if (ret == -1) {
    err = errno;
    printf("failed!\n");
    cm_msg(MERROR, "vacuum_gauge_init",
	   "Error setting vacuum gauge serial port attributes (errno %d)", err);
    return FE_ERR_HW;
  }
  printf("done.\n");

  // Tell the vacuum gauge to await an inquiry
  printf("Setting vacuum gauge to measurement request mode...");
  ret = write(vacuum,INQ_MODE,sizeof(INQ_MODE)-1);
  if (ret == -1) {
    err = errno;
    printf("failed!\n");
    cm_msg(MERROR, "vacuum_gauge_init",
	   "Error writing to vacuum gauge to set inquire mode (errno %d)", errno);
    return FE_ERR_HW;
  }
  printf("done.\n");
  printf("Checking vacuum acknowledgement...");
  ret = read(vacuum, resp, 3);
  if (ret == -1) {
    err = errno;
    printf("failed!\n");
    cm_msg(MERROR, "vacuum_gauge_init",
	   "Error reading from vacuum gauge to get acknowledgment (errno %d)", errno);
    return FE_ERR_HW;
  } else if (resp[0] != ACK) {
    err = errno;
    printf("failed!\n");
    cm_msg(MERROR, "vacuum_gauge_init",
	   "Did not receive acknowledgement from vacuum.");
    return FE_ERR_HW;
  }
  printf("done.\n");

  return CM_SUCCESS;
}

/*-- Frontend Exit -------------------------------------------------*/

INT frontend_exit()
{
  return CM_SUCCESS;
}

/*-- Frontend Loop -------------------------------------------------*/

INT frontend_loop()
{
  return CM_SUCCESS;
}

/*-- Begin of Run --------------------------------------------------*/

INT begin_of_run(INT run_number, char *error)
{
   return CM_SUCCESS;
}

/*-- End of Run ----------------------------------------------------*/

INT end_of_run(INT run_number, char *error)
{
   return CM_SUCCESS;
}

/*-- Pause Run -----------------------------------------------------*/

INT pause_run(INT run_number, char *error)
{
   return CM_SUCCESS;
}

/*-- Resuem Run ----------------------------------------------------*/

INT resume_run(INT run_number, char *error)
{
   return CM_SUCCESS;
}

/*------------------------------------------------------------------*/

INT vacuum_gauge_read(char *pevent, INT off) {

  int i;
  int ret, *status;
  float* pressure;
  char resp[256], pressure_str[11];

  bk_init(pevent);

  ret = write(vacuum, &ENQ, sizeof(ENQ) - 1);
  if (ret == -1) {
    cm_msg(MERROR, "read_vacuum_gauge",
	   "Cannot write to vacuum gauge to request measurement (errno %d)", errno);
    return FE_ERR_HW;
  }
  ret = read(vacuum, resp, 15);
  if (ret == -1) {
    cm_msg(MERROR, "read_vacuum_gauge",
	   "Cannot read from vacuum gauge to get requested reading (errno %d)", errno);
    return FE_ERR_HW;
  } else if (ret != 15) {
    cm_msg(MERROR, "read_vacuum_gauge",
	   "Received incorrect size response (expected 15, received %d)", ret);
    return FE_ERR_HW;
  }

  // Response should be format "S,sX.XXXXEsXX\r\n"
  // Where s is sign, S is status number, and X are numbers
  // Extract strings we need
  const static int reading_offset = 3;
  const static int num_digits;
  // Record status
  bk_create(pevent, "PRS0", TID_INT, &status);
  *status++ = atoi(resp);
  if (*status == 0) {
    bk_close(pevent, status);
    // Record pressure
    bk_create(pevent, "PRM0", TID_FLOAT, &pressure);
    for (i = 0; i < num_digits; i++)
      pressure_str[i] = resp[i + reading_offset];
    pressure_str[num_digits] = NUL;
    *pressure++ = atof(pressure_str);
    bk_close(pevent, pressure);
  } else if (*status == 1) {
    cm_msg(MINFO, "read_vacuum_gauge", "Vacuum gauge under range.");
  } else if (*status == 2) {
    cm_msg(MINFO, "read_vacuum_gauge", "Vacuum gauge over range.");
  } else if (*status == 3) {
    cm_msg(MERROR, "read_vacuum_gauge", "Vacuum gauge error.");
  } else if (*status == 4) {
    cm_msg(MINFO, "read_vacuum_gauge", "Vacuum gauge off.");
  } else if (*status == 5) {
    cm_msg(MINFO, "read_vacuum_gauge", "Vacuum gauge not attached.");
  } else if (*status == 6) {
    cm_msg(MERROR, "read_vacuum_gauge", "Vacuum gauge unidentifiable.");
  } else {
    cm_msg(MERROR, "read_vacuum_gauge", "Vacuum gauge unknown status (status %d)", *status);
  }

  bk_close(pevent, status);
  return CM_SUCCESS;
}
