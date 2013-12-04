#include <stdio.h>
#include "midas.h"

#include <termios.h>  // Serial port configuration
#include <fcntl.h>    // Defines modes for opening serial port
#include <unistd.h>   // Read and write to serial port
#include <string.h>   // Set char[] to all same value
#include <stdlib.h>   // Take char array
#include <errno.h>    // Set by read, write, and termios functions
#include <sys/time.h> // For timeval structure

/* make frontend functions callable from the C framework */
#ifdef __cplusplus
extern "C" {
#endif

INT crate_number=10;

HNDLE hDB;
HNDLE hKey;
int size;

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
static int vacuum;
static struct termios vacuum_config;
static struct timeval vac_timeout;
static fd_set vac_ready;
static const char vacuum_port[] = "/dev/ttyS0";
static const resp_size = 256;
static char resp[256];
static int missed_reads;
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
static const char CONT_MODE[] = "COM,1\r\n";
static const char SETPNT_STAT[] = "SPS\r\n";
static const char SETPNT_1[] = "SP1\r\n";
static const char SETPNT_2[] = "SP2\r\n";


/*-- Equipment list ------------------------------------------------*/

BANK_LIST vacuum_bank_list[] = {
  { "PRS0", TID_INT,   1, NULL },
  { "PRM0", TID_FLOAT, 1, NULL },
  { "SP1S", TID_BOOL,  1, NULL },
  { "SP2S", TID_BOOL,  1, NULL },
  { "SP1L", TID_FLOAT, 1, NULL },
  { "SP1H", TID_FLOAT, 1, NULL },
  { "SP2L", TID_FLOAT, 1, NULL },
  { "SP2H", TID_FLOAT, 1, NULL },
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
       10000,              /* reads spaced by this many ms */
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

INT frontend_init() {
  int ret, err;
  cm_get_experiment_database(&hDB, NULL);

  // Open serial port for vacuum
  vacuum = open(vacuum_port, O_RDWR | O_NOCTTY);
  printf("Opening vacuum gauge port...");
  if (vacuum < 0) {
    err = errno;
    printf("failed!\n");
    cm_msg(MERROR, "vacuum_gauge_init",
	   "Error opening vacuum gauge (errno %d)", err);
    return FE_ERR_HW;
  }
  printf("done.\n");
  printf("Getting current serial port attributes...");
  ret = tcgetattr(vacuum, &vacuum_config);
  if (ret < 0) {
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
  vacuum_config.c_cc[VTIME] = 10;         // Interbyte timeout of 1 sec
  vacuum_config.c_cc[VMIN]  = 28;         // Longest msg is 2 measurements, 28 chars
  /*** Timeout ***/
  vac_timeout.tv_sec = 1;                 // Timeout to check if buffer is ready to read is 1 sec
  vac_timeout.tv_usec = 0;                // And 0 microseconds
  /**********************************************************/

  // Apply the attributes
  printf("Setting new serial port attributes...");
  ret = tcsetattr(vacuum, TCSAFLUSH, &vacuum_config); // Flush the output, discard input, and apply changes
  if (ret < 0) {
    err = errno;
    printf("failed!\n");
    cm_msg(MERROR, "vacuum_gauge_init",
	   "Error setting vacuum gauge serial port attributes (errno %d)", err);
    return FE_ERR_HW;
  }
  printf("done.\n");

  // Tell the vacuum gauge to await an inquiry
  printf("Setting vacuum gauge to measurement request mode...");
  ret = write(vacuum, INQ_MODE, sizeof(INQ_MODE)-1);
  if (ret < 0) {
    err = errno;
    printf("failed!\n");
    cm_msg(MERROR, "vacuum_gauge_init",
	   "Error writing to vacuum gauge to set inquire mode (errno %d)", errno);
    return FE_ERR_HW;
  }
  printf("done.\n");
  printf("Checking vacuum acknowledgement...");

  FD_ZERO(&vac_ready);
  FD_SET(vacuum, &vac_ready);
  select(FD_SETSIZE, &vac_ready, NULL, NULL, &vac_timeout);  
  if(!FD_ISSET(vacuum, &vac_ready)) {
    printf("failed!\n");
    cm_msg(MERROR, "vacuum_gauge_init",
	   "Vacuum gauge did not acknowledge measurement request mode (nothing received)");
    return FE_ERR_HW;
  }
  ret = read(vacuum, resp, 3);
  if (ret < 0) {
    err = errno;
    printf("failed!\n");
    cm_msg(MERROR, "vacuum_gauge_init",
	   "Vacuum gauge did not acknowledge measurement request mode (errno %d)", errno);
    return FE_ERR_HW;
  } else if (resp[0] != ACK) {
    err = errno;
    printf("failed!\n");
    cm_msg(MERROR, "vacuum_gauge_init",
	   "Vacuum gauge did not acknowledge measurement request mode (something other than ACK received)");
    return FE_ERR_HW;
  }
  printf("done.\n");

  missed_reads = 0;

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

  // Work variables
  static int    i;                   // Looping
  static int    ret,           err;  // Error reporting
  static INT    stat;                // Temperaroy status holder for print statements
  // All of the numbers we're measuring
  static INT   *status;              // Status of gauge
  static BOOL  *sp1,          *sp2;  // Set points on or off
  static float *sp1l,         *sp1h; // First set point lower and upper trigger
  static float *sp2l,         *sp2h; // Second set point lower and upper trigger
  static float *pres;                // Pressure reading
  // And their corresponding strings (with terminating NULL)
  // Status:         1 character
  // Measurements:  10 characters
  static char  status_str[1+1];
  static char  sp1_str[1+1],   sp2_str[1+1];
  static char  sp1l_str[10+1], sp1h_str[10+1];
  static char  sp2l_str[10+1], sp2h_str[10+1];
  static char  pres_str[10+1];
  // Null terminate for conversion to numbers
  status_str[1] = NUL;
  sp1_str[1] = NUL; sp2_str[1] = NUL;
  sp1l_str[10] = NUL; sp1h_str[10] = NUL;
  sp2l_str[10] = NUL; sp2h_str[10] = NUL;
  pres_str[10] = NUL;

  // Keep track of erroneous calls to this function. Reset at end.
  missed_reads++;

  /*** Get status of set points ***/
  // Set mode (status)
  ret = write(vacuum, SETPNT_STAT, sizeof(SETPNT_STAT)-1);
  if (ret < 0) {
    err = errno;
    printf("Error writing to vacuum gauge to enable set point request mode (errno %d)\n", err);
    return 0;
  }
  FD_ZERO(&vac_ready);
  FD_SET(vacuum, &vac_ready);
  select(FD_SETSIZE, &vac_ready, NULL, NULL, &vac_timeout);
  if(!FD_ISSET(vacuum, &vac_ready)) {
    printf("Vacuum gauge did not acknowledge set point request mode (nothing received)\n");
    return 0;
  }
  ret = read(vacuum, resp, 3);
  if (ret < 0) {
    err = errno;
    printf("Vacuum gauge did not acknowledge set point request mode (errno %d)\n", err);
    return 0;
  } else if (resp[0] != ACK) {
    printf("Vacuum gauge did not acknowledge set point request mode (something other than ACK received)\n");
    return 0;
  }
  // Request set points
  ret = write(vacuum, &ENQ, sizeof(ENQ));
  if (ret < 0){
    err = errno;
    printf("Error writing to vacuum gauge to request status of set points (errno %d)\n", err);
    return 0;
  }
  FD_ZERO(&vac_ready);
  FD_SET(vacuum, &vac_ready);
  select(FD_SETSIZE, &vac_ready, NULL, NULL, &vac_timeout);
  if(!FD_ISSET(vacuum, &vac_ready)) {
    printf("Vacuum gauge did not send status of set points.\n");
    return 0;
  }
  ret = read(vacuum, resp, 9);
  if (ret < 0) {
    err = errno;
    printf("Error reading status of set points from vacuum gaige (errno %d)\n", err);
    return 0;
  } else if (ret != 9) {
    printf("Received incorrect response size (expected 9, received %d)", ret);
    return 0;
  }
  // Record status
  sp1_str[0] = resp[0];
  sp2_str[0] = resp[2];

  /*** Get set point values ***/
  /* SP1 */
  // Set mode
  ret = write(vacuum, SETPNT_1, sizeof(SETPNT_1)-1);
  if (ret < 0) {
    err = errno;
    printf("Error writing to vacuum gauge to enable set point request mode (errno %d)\n", err);
    return 0;
  }
  FD_ZERO(&vac_ready);
  FD_SET(vacuum, &vac_ready);
  select(FD_SETSIZE, &vac_ready, NULL, NULL, &vac_timeout);
  if(!FD_ISSET(vacuum, &vac_ready)) {
    printf("Vacuum gauge did not acknowledge set point request mode (nothing received)\n");
    return 0;
  }
  ret = read(vacuum, resp, 3);
  if (ret < 0) {
    err = errno;
    printf("Vacuum gauge did not acknowledge set point request mode (errno %d)\n", err);
    return 0;
  } else if (resp[0] != ACK) {
    printf("Vacuum gauge did not acknowledge set point request mode (something other than ACK received)\n");
    return 0;
  }
  // Request set points
  ret = write(vacuum, &ENQ, sizeof(ENQ));
  if (ret < 0){
    err = errno;
    printf("Error writing to vacuum gauge to request status of set points (errno %d)\n", err);
    return 0;
  }
  FD_ZERO(&vac_ready);
  FD_SET(vacuum, &vac_ready);
  select(FD_SETSIZE, &vac_ready, NULL, NULL, &vac_timeout);
  if(!FD_ISSET(vacuum, &vac_ready)) {
    printf("Vacuum gauge did not send status of set points.\n");
    return 0;
  }
  ret = read(vacuum, resp, 25);
  if (ret < 0) {
    err = errno;
    printf("Error reading status of set points from vacuum gaige (errno %d)\n", err);
    return 0;
  } else if (ret != 9) {
    printf("Received incorrect response size (expected 25, received %d)\n", ret);
    return 0;
  }
  if (resp[0] != '1') {
    printf("Received incorrect set point value (expect 1, received %c)\n", resp[0]);
    return 0;
  }
  for (i = 0; i < 10; i++) {
    sp1l_str[i] = resp[i + 2];
    sp1h_str[i] = resp[i + 13];
  }
  /* SP2 */
  // Set mode
  ret = write(vacuum, SETPNT_2, sizeof(SETPNT_2)-1);
  if (ret < 0) {
    err = errno;
    printf("Error writing to vacuum gauge to enable set point request mode (errno %d)\n", err);
    return 0;
  }
  FD_ZERO(&vac_ready);
  FD_SET(vacuum, &vac_ready);
  select(FD_SETSIZE, &vac_ready, NULL, NULL, &vac_timeout);
  if(!FD_ISSET(vacuum, &vac_ready)) {
    printf("Vacuum gauge did not acknowledge set point request mode (nothing received)\n");
    return 0;
  }
  ret = read(vacuum, resp, 3);
  if (ret < 0) {
    err = errno;
    printf("Vacuum gauge did not acknowledge set point request mode (errno %d)\n", err);
    return 0;
  } else if (resp[0] != ACK) {
    printf("Vacuum gauge did not acknowledge set point request mode (something other than ACK received)\n");
    return 0;
  }
  // Request set points
  ret = write(vacuum, &ENQ, sizeof(ENQ));
  if (ret < 0){
    err = errno;
    printf("Error writing to vacuum gauge to request status of set points (errno %d)\n", err);
    return 0;
  }
  FD_ZERO(&vac_ready);
  FD_SET(vacuum, &vac_ready);
  select(FD_SETSIZE, &vac_ready, NULL, NULL, &vac_timeout);
  if(!FD_ISSET(vacuum, &vac_ready)) {
    printf("Vacuum gauge did not send status of set points.\n");
    return 0;
  }
  ret = read(vacuum, resp, 25);
  if (ret < 0) {
    err = errno;
    printf("Error reading status of set points from vacuum gaige (errno %d)\n", err);
    return 0;
  } else if (ret != 9) {
    printf("Received incorrect response size (expected 25, received %d)\n", ret);
    return 0;
  }
  if (resp[0] != '2') {
    printf("Received incorrect set point value (expect 2, received %c)\n", resp[0]);
    return 0;
  }
  for (i = 0; i < 10; i++) {
    sp2l_str[i] = resp[i + 2];
    sp2h_str[i] = resp[i + 13];
  }
  
  /*** Get pressure and status of gauge ***/
  // Set mode
  ret = write(vacuum, INQ_MODE, sizeof(INQ_MODE)-1);
  if (ret < 0) {
    err = errno;
    printf("Error writing to vacuum gauge to set inquire mode (errno %d)\n", err);
    return 0;
  }
  FD_ZERO(&vac_ready);
  FD_SET(vacuum, &vac_ready);
  select(FD_SETSIZE, &vac_ready, NULL, NULL, &vac_timeout);  
  if(!FD_ISSET(vacuum, &vac_ready)) {
    printf("Vacuum gauge did not acknowledge measurement request mode (nothing received)\n");
    return 0;
  }
  ret = read(vacuum, resp, 3);
  if (ret < 0) {
    err = errno;
    printf("Vacuum gauge did not acknowledge measurement request mode (errno %d)\n", errno);
    return 0;
  } else if (resp[0] != ACK) {
    err = errno;
    printf("Vacuum gauge did not acknowledge measurement request mode (something other than ACK received)\n");
    return 0;
  }  
  // Request pressure
  ret = write(vacuum, &ENQ, sizeof(ENQ));
  if (ret < 0) {
    printf("Cannot write to vacuum gauge to request measurement (errno %d)", errno);
    return 0;
  }
  FD_ZERO(&vac_ready);
  FD_SET(vacuum, &vac_ready);
  select(FD_SETSIZE, &vac_ready, NULL, NULL, &vac_timeout);
  if(!FD_ISSET(vacuum, &vac_ready)) {
    printf("Nothing to be read from vacuum gauge");
    return 0;
  }
  ret = read(vacuum, resp, 15);
  if (ret < 0) {
    printf("Cannot read from vacuum gauge to get requested reading (errno %d)", errno);
    return 0;
  } else if (ret != 15) {
    printf("Received incorrect size response (expected 15, received %d)", ret);
    return 0;
  }
  status_str[0] = resp[0];
  for (i = 0; i < 10; i++) {
    pres_str[i] = resp[i + 4];
  }

  // Record in banks
  bk_init(pevent);
  bk_create(pevent, "PRS0", TID_INT, &status);
  *status++ = atoi(status_str); stat = *status;
  bk_close(pevent, status);
  bk_create(pevent, "PRM0", TID_FLOAT, &pres);
  *pres++ =   atof(pres_str);
  bk_close(pevent, pres);
  bk_create(pevent, "SP1S", TID_BOOL, &sp1);
  *sp1++ =    atoi(sp1_str);
  bk_close(pevent, sp1);
  bk_create(pevent, "SP2S", TID_BOOL, &sp2);
  *sp2 =      atoi(sp2_str);
  bk_close(pevent, sp2);
  bk_create(pevent, "SP1L", TID_FLOAT, &sp1l);
  *sp1l =     atof(sp1l_str);
  bk_close(pevent, sp1l);
  bk_create(pevent, "SP1H", TID_FLOAT, &sp1h);  
  *sp1h =     atof(sp1h_str);
  bk_close(pevent, sp1h);
  bk_create(pevent, "SP2L", TID_FLOAT, &sp2l);
  *sp2l =     atof(sp2l_str);
  bk_close(pevent, sp2l);
  bk_create(pevent, "SP2H", TID_FLOAT, &sp2h);
  *sp2h =     atof(sp2h_str);
  bk_close(pevent, sp2h);
  
  // Report on bad status
  
  switch (stat) {
  case 0:                                                                break;
  case 1:  printf("Vacuum gauge under range.\n");                        break;
  case 2:  printf("Vacuum gauge over range.\n");                         break;
  case 3:  printf("Vacuum gauge error.\n");                              break;
  case 4:  printf("Vacuum gauge off.\n");                                break;
  case 5:  printf("Vacuum gauge not attached.\n");                       break;
  case 6:  printf("Vacuum gauge unidentifiable.\n");                     break;
  default: printf("Vacuum gauge unknown status (status %d)\n", *status); break;
  }

  // Had a succesful read.
  missed_reads = 0;

  return bk_size(pevent);
}
