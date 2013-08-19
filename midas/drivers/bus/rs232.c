/********************************************************************\

  Name:         rs232.c
  Created by:   Stefan Ritt

  Contents:     RS232 communication routines for MS-DOS and NT

  $Log: rs232.c,v $
  Revision 1.1.1.1  2005/06/20 23:37:07  mucap
  Importing release 1.9.5 of the MIDAS source code as distributed by PSI.
  (Next, I'll commit our local customizations.)

  Revision 1.13  2004/05/07 19:40:10  midas
  Replaced min/max by MIN/MAX macros

  Revision 1.12  2004/01/08 08:40:08  midas
  Implemented standard indentation

  Revision 1.11  2001/06/20 09:10:36  midas
  Implemented flow control under NT (Fixed problems with W2k)

  Revision 1.10  2001/04/10 21:39:23  pierre
  - Fix rs232_read fprintf (...str)

  Revision 1.9  2001/04/05 05:53:34  midas
  Added CMD_NAME

  Revision 1.8  2001/02/26 11:56:55  midas
  Added READ and WRITE functions

  Revision 1.7  2001/01/05 15:30:56  midas
  *** empty log message ***

  Revision 1.6  2001/01/05 15:20:44  midas
  Fixed wrong error return value

  Revision 1.5  2001/01/05 15:08:22  midas
  Fine-tuned timeout in rs232_gets

  Revision 1.4  2001/01/04 11:25:19  midas
  Added flow control (0=off, 1=CTS/DSR, 2=XON/XOFF)

  Revision 1.3  2001/01/04 10:22:22  midas
  Read characters individually under Linux

  Revision 1.2  2001/01/03 16:05:10  midas
  Adapted Bus Driver scheme to rs232

  Revision 1.1  1999/12/20 10:18:11  midas
  Reorganized driver directory structure

  Revision 1.2  1998/10/12 12:18:58  midas
  Added Log tag in header


\********************************************************************/

#include "midas.h"

static int debug_flag = 0;

typedef struct {
   char port[NAME_LENGTH];
   int baud;
   char parity;
   int data_bit;
   int stop_bit;
   int flow_control;
} RS232_SETTINGS;

#define RS232_SETTINGS_STR "\
RS232 Port = STRING : [32] com1\n\
Baud = INT : 9600\n\
Parity = CHAR : N\n\
Data Bit = INT : 8\n\
Stop Bit = INT : 1\n\
Flow control = INT : 0\n\
"

typedef struct {
   RS232_SETTINGS settings;
   int fd;                      /* device handle for RS232 device */
} RS232_INFO;

/*--- OS_MSDOS -----------------------------------------------------*/

#ifdef OS_MSDOS

#include <stdio.h>
#include <string.h>
#include <conio.h>
#include <bios.h>
#include <dos.h>
#include <stdlib.h>
#include <sys\timeb.h>
#include "rs232.h"

#define XON  0x11
#define XOFF 0x13
#define RING_BUFF_LEN 2000          /**** adjust if necessary  ****/
#define NUM_PORTS     2             /**** number of COMx ports ****/

static void interrupt com1int();
static void interrupt com2int();
static void comint_handler(int port);
static void rs232_exit(void);

extern void (*error) (char *str);

struct {
   unsigned int port_base;
   unsigned int vector;
   void interrupt(*new_intr) ();
   void interrupt(*old_intr) ();
   unsigned char initialized;
   char ring_buff[RING_BUFF_LEN];
   unsigned char com_error, com_mode;
   int volatile write_mark, read_mark, send_enable;
} ccb[] = {
   {
   0x3F8, 0x0C, com1int}, {
   0x2F8, 0x0B, com2int}
};

/* ------------------------- rs232_open ----------------------------- */

int rs232_open(char *port, int baud, char parity, int data_bit, int stop_bit,
               int flow_control)
/*********************************************************************
* Initialize serial port according to parameters. Install interrupt  *
* handler and uninstall it on exit.                                  *
* flow_control == 0: no handshake; 1: CTS/DSR; 2: XON/XOFF           *
*********************************************************************/
{
#define CTS  0x10            /**** Bit masks for handshake lines ****/
#define DSR  0x20
#define RLSD 0x80

   int mode = 0;

   static int first_init = 1;
   int parity_mask[] = { 0x00, 0x08, 0x18 };
   int baud_list[] = { 110, 150, 300, 600, 1200, 2400, 4800, 9600 };
   int i;

   for (i = 0; i < 8; i++)
      if (baud_list[i] == baud)
         break;

  /**** init com port ****/
   bioscom(0, i * 0x20 | parity_mask[parity] | (stop_bit - 1) * 4
           | (data_bit - 5), port[3] - '1');

  /**** initialize ring buffer ****/
   ccb[port].write_mark = 0;
   ccb[port].read_mark = 0;
   ccb[port].com_error = 0;
   ccb[port].com_mode = mode;
   ccb[port].send_enable = 1;

   if (ccb[port].initialized)
    /**** reset old vector ****/
      setvect(ccb[port].vector, (void interrupt(far *) ()) ccb[port].old_intr);

  /**** save old vector, set new vector ****/
   ccb[port].old_intr = (void interrupt(far *) ()) getvect(ccb[port].vector);
   setvect(ccb[port].vector, (void interrupt(far *) ()) ccb[port].new_intr);

   ccb[port].initialized = 1;

   if (first_init) {
      first_init = 0;
      atexit(rs232_exit);       /* call rs232_exit routine at exit of the program */
   }

   outportb(ccb[port].port_base + 1, 0x05);
   /* enable Data Available Interrupt and     */
   /* Recieve Line Status Interrupt           */
   outportb(ccb[port].port_base + 4, 0x0B);     /* 8250 Interrupt enable         */

   inportb(ccb[port].port_base);        /* reset running interrupts      */
   inportb(ccb[port].port_base + 5);

  /**** 8259 Interrupt Controller init ****/
   outportb(0x21, inportb(0x21) & ~(0x10 >> port));
   outportb(0x20, 0x20);        /* end of interrupt */

   return port;
}

/* --------------------------- rs232_exit --------------------------- */

static void rs232_exit(void)
{
   int i;

  /**** reset old vector ****/
   for (i = 0; i < NUM_PORTS; i++)
      if (ccb[i].initialized) {
      /**** disable interrupt controller ****/
         outportb(0x21, inportb(0x21) | (0x10 >> i));

      /**** 8250 interrupt disable ****/
         outportb(ccb[i].port_base + 4, 0);
         outportb(ccb[i].port_base + 1, 0);

      /**** reset old vector ****/
         setvect(ccb[i].vector, (void interrupt(far *) ()) ccb[i].old_intr);
      }
}

/* --------------------------- send char -------------------------- */

void rs232_putc(int port, char c)
/*********************************************************************
* Send a single character over the previously opened COM port        *
*********************************************************************/
{
   char str[30];
   int retry;

#define TIMEOUT 3    /**** 3 seconds ****/

   port--;

  /**** wait until 8250 has sent previous character ****/
   while ((inportb(ccb[port].port_base + 5) & 0x60) == 0);

  /**** wait until receiver ready (CTS and DSR high) or XON received ****/

   retry = 0;
   do {
      if ((ccb[port].com_mode == 1 && (inportb(ccb[port].port_base + 6) & 0x30) != 0)
          || (ccb[port].com_mode == 2 && ccb[port].send_enable)
          || (ccb[port].com_mode == 0))
         break;
      retry++;
      sleep(1);
              /**** wait maximal retry_max sec. ****/
   } while ((retry < TIMEOUT));

   if (retry == TIMEOUT) {
      sprintf(str, "Timeout on COM%1d\n", port + 1);
      error(str);
   }

   outportb(ccb[port].port_base, c);         /**** send char ****/
}

void rs232_puts(RS232_INFO * info, char *str)
{
   while (*str)
      computc(info->fd, *(str++));

   if (debug_flag) {
      FILE *f;

      f = fopen("rs232.log", "a");
      fprintf(f, "puts: %s\n", str);
      fclose(f);
   }
}

/* -------------------------- get string --------------------------- */

int rs232_gets(int port, char *s, int size, int end_char)
{
/*********************************************************************
* Copies the ringbuffer into the string s. Return the number of valid*
* chars in the string.                                               *
* end_char == 0 : copy the whole buffer                              *
* end_char != 0 : copy until "char" encountered                      *
*********************************************************************/
   int temp_write_mark, i = 0, eos;
   char str[30];

   port--;

   if (ccb[port].com_error > 0) {
      if ((ccb[port].com_error & 0x01) == 0x01)
         sprintf(str, "COM%1d: Ringbuffer overflow\n", port + 1);
      if ((ccb[port].com_error & 0x04) == 0x04)
         sprintf(str, "COM%1d: Parity error\n", port + 1);
      if ((ccb[port].com_error & 0x08) == 0x08)
         sprintf(str, "COM%1d: Missing stop-bit\n", port + 1);
      if ((ccb[port].com_error & 0x10) == 0x10)
         sprintf(str, "COM%1d: Framing error\n", port + 1);
      error(str);
      ccb[port].com_error = 0;/**** reset error flag ****/
   }

   temp_write_mark = ccb[port].write_mark;
   if (temp_write_mark != ccb[port].read_mark) {
      temp_write_mark--;
      if (temp_write_mark < ccb[port].read_mark)
         temp_write_mark += RING_BUFF_LEN;

      /* if at least ten bytes room in the buffer, set DTR high / send XON */
      if (temp_write_mark - ccb[port].read_mark < RING_BUFF_LEN - 12) {
         if (ccb[port].com_mode == 2)
            computc(port, XON);
         if (ccb[port].com_mode == 1)
            outportb(ccb[port].port_base + 4, 9);
      }

      if (end_char == 0) {
         for (i = 0; (i <= temp_write_mark - ccb[port].read_mark) && (i < size - 1); i++)
            s[i] = ccb[port].ring_buff[(i + ccb[port].read_mark) % RING_BUFF_LEN];
         ccb[port].read_mark = (ccb[port].read_mark + i) % RING_BUFF_LEN;
      } else {
         eos = -1;
         for (i = 0; (i <= temp_write_mark - ccb[port].read_mark) && (i < size - 1); i++)
            if (ccb[port].ring_buff[(i + ccb[port].read_mark) % RING_BUFF_LEN]
                == end_char) {
               eos = i;
               break;
            }
         if (eos >= 0)
            for (i = 0; i <= eos; i++)
               s[i] = ccb[port].ring_buff[(i + ccb[port].read_mark) % RING_BUFF_LEN];
         ccb[port].read_mark = (ccb[port].read_mark + i) % RING_BUFF_LEN;
      }
   }
   s[i] = 0;

   if (debug_flag) {
      FILE *f;

      f = fopen("rs232.log", "a");
      fprintf(f, "gets: %s\n", s);
      fclose(f);
   }

   return (i);
}

/* ----------------------- wait for a string ----------------------- */

int rs232_waitfor(int port, char *str, char *retstr, int size, int seconds)
{
/*********************************************************************
* Wait for a specific string. Timeout after "seconds". Does NOT re-  *
* move the string from the ringbuffer.                               *
*********************************************************************/
   int temp_write_mark, i, j, k;
   char errstr[80];
   struct timeb start_time, act_time;

   port--;
   if (ccb[port].com_error > 0) {
      if ((ccb[port].com_error & 0x01) == 0x01)
         sprintf(errstr, "COM%1d: Ringbuffer overflow\n", port + 1);
      if ((ccb[port].com_error & 0x04) == 0x04)
         sprintf(errstr, "COM%1d: Parity error\n", port + 1);
      if ((ccb[port].com_error & 0x08) == 0x08)
         sprintf(errstr, "COM%1d: Missing stop-bit\n", port + 1);
      if ((ccb[port].com_error & 0x10) == 0x10)
         sprintf(errstr, "COM%1d: Framing error\n", port + 1);
      error(errstr);
      ccb[port].com_error = 0;/**** reset error flag ****/
   }

   ftime(&start_time);

   do {
      temp_write_mark = ccb[port].write_mark;
      if (temp_write_mark != ccb[port].read_mark) {
         temp_write_mark--;
         if (temp_write_mark < ccb[port].read_mark)
            temp_write_mark += RING_BUFF_LEN;

         /* if at least ten bytes room in the buffer, set DTR high / send XON */
         if (temp_write_mark - ccb[port].read_mark < RING_BUFF_LEN - 12) {
            if (ccb[port].com_mode == 2)
               computc(port, XON);
            if (ccb[port].com_mode == 1)
               outportb(ccb[port].port_base + 4, 9);
         }

         if (temp_write_mark - ccb[port].read_mark + 1 >= strlen(str)) {
            for (i = 0; i <= temp_write_mark - ccb[port].read_mark - strlen(str) + 1; i++) {
               if (str[0] ==
                   ccb[port].ring_buff[(i + ccb[port].read_mark) % RING_BUFF_LEN]) {
                  for (j = 1; j < strlen(str); j++)
                     if (str[j] !=
                         ccb[port].ring_buff[(i + j + ccb[port].read_mark) %
                                             RING_BUFF_LEN])
                        break;
                  if (j == strlen(str)) {
                     for (k = 0; k <= i + j - 1 && k < size - 1; k++)
                        retstr[k] =
                            ccb[port].ring_buff[(k + ccb[port].read_mark) %
                                                RING_BUFF_LEN];
                     ccb[port].read_mark = (ccb[port].read_mark + k) % RING_BUFF_LEN;
                     retstr[k] = 0;

                     if (debug_flag) {
                        FILE *f;

                        f = fopen("rs232.log", "a");
                        fprintf(f, "waitfor: %s\n", retstr);
                        fclose(f);
                     }

                     return (k);
                  }
               }
            }
         }
      }
      ftime(&act_time);
   } while ((double) act_time.time + act_time.millitm / 1000 -
            start_time.time - start_time.millitm / 1000 < seconds);

   sprintf(errstr, "Timeout WAITFOR (%s)", str);
   error(errstr);

   retstr[0] = 0;
   return (0);
}

/* -------------------------- Interrupt Handler -------------------------- */

void interrupt com1int()
{
   comint_handler(1);
}

void interrupt com2int()
{
   comint_handler(2);
}

void comint_handler(int port)
{
/*********************************************************************
* This is the seial interrupt handler. It is called when a char is   *
* received or an serial error occured.                               *
*********************************************************************/
   char c;
   unsigned char int_type;
   int old_write_mark;

   port--;
   while (((int_type = inportb(ccb[port].port_base + 2)) & 1) == 0)
  /**** as long as an interrupt is pending ****/
   {
      if (int_type == 6)        /* Receiver Line Status Error   */
         ccb[port].com_error = inportb(ccb[port].port_base + 5) & 0x1E;

      if (int_type == 4) {      /* Received Data available      */
         c = inportb(ccb[port].port_base);
         if (c == XON)
            ccb[port].send_enable = 1;
         else if (c == XOFF)
            ccb[port].send_enable = 0;
         else {
        /**** write received char into the ringbuffer ****/
            ccb[port].ring_buff[ccb[port].write_mark] = c;

            old_write_mark = ccb[port].write_mark;

            ccb[port].write_mark = (ccb[port].write_mark + 1) % RING_BUFF_LEN;

        /**** if ringbuffer is nearly full, set DTR low / send XOFF ****/
            if (((ccb[port].write_mark + 1) % RING_BUFF_LEN) == ccb[port].read_mark) {
               if (ccb[port].com_mode == 2)
                  computc(port, XOFF);
               else
                  outportb(ccb[port].port_base + 4, 8);
            }

        /**** if ringbuffer if full, shift endmark and set error ****/
            if (ccb[port].write_mark == ccb[port].read_mark) {
               ccb[port].com_error = 1;
               cprintf("old_write_mark: %d, write_mark: %d, read_mark: %d\n\r",
                       old_write_mark, ccb[port].write_mark, ccb[port].read_mark);

               ccb[port].read_mark = (ccb[port].read_mark + 1) % RING_BUFF_LEN;
            }
         }
      }
   }                            /* end-of-while */

   outportb(0x20, 0x20);        /* end of interrupt */
}

#endif                          /* OS_MSDOS */

/*---- Windows NT ------------------------------------------------------------*/

#ifdef OS_WINNT

#include <stdio.h>

int rs232_open(char *port, int baud, char parity, int data_bit, int stop_bit,
               int flow_control)
{
   char str[80];
   DCB dcb;
   HANDLE hDev;
   COMMTIMEOUTS CommTimeOuts;

   sprintf(str, "\\\\.\\%s", port);

   hDev = CreateFile(str, GENERIC_READ | GENERIC_WRITE, 0,      // exclusive access
                     NULL,      // no security attrs
                     OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | 0, NULL);

   if (hDev == (HANDLE) - 1)
      return -1;

   GetCommState(hDev, &dcb);

   sprintf(str, "baud=%d parity=%c data=%d stop=%d", baud, parity, data_bit, stop_bit);
   BuildCommDCB(str, &dcb);

   if (flow_control == 1) {
      dcb.fOutX = dcb.fInX = FALSE;
      //dcb.fDtrControl = DTR_CONTROL_HANDSHAKE;
      //dcb.fRtsControl = RTS_CONTROL_HANDSHAKE;
      dcb.fOutxCtsFlow = dcb.fOutxDsrFlow = TRUE;
   } else if (flow_control == 2) {
      dcb.fOutX = dcb.fInX = TRUE;
      dcb.fOutxCtsFlow = dcb.fOutxDsrFlow = FALSE;
   } else {
      dcb.fOutX = dcb.fInX = FALSE;
      dcb.fOutxCtsFlow = dcb.fOutxDsrFlow = FALSE;
   }

   if (SetCommState(hDev, &dcb) == FALSE)
      return -1;

   SetupComm(hDev, 4096, 4096);

   CommTimeOuts.ReadIntervalTimeout = 0;
   CommTimeOuts.ReadTotalTimeoutMultiplier = 0;
   CommTimeOuts.ReadTotalTimeoutConstant = 0;
   CommTimeOuts.WriteTotalTimeoutMultiplier = 0;
   CommTimeOuts.WriteTotalTimeoutConstant = 5000;

   SetCommTimeouts((HANDLE) hDev, &CommTimeOuts);

   return (INT) hDev;
}

/*----------------------------------------------------------------------------*/

INT rs232_exit(RS232_INFO * info)
{
   EscapeCommFunction((HANDLE) info->fd, CLRDTR);

   PurgeComm((HANDLE) info->fd, PURGE_TXABORT | PURGE_RXABORT |
             PURGE_TXCLEAR | PURGE_RXCLEAR);
   CloseHandle((HANDLE) info->fd);

   return SUCCESS;
}

/*----------------------------------------------------------------------------*/

int rs232_write(RS232_INFO * info, char *data, int size)
{
   DWORD written, i;

   if (debug_flag) {
      FILE *f;

      f = fopen("rs232.log", "a");
      fprintf(f, "write: ");
      for (i = 0; (int) i < size; i++)
         fprintf(f, "%X ", data[i]);
      fprintf(f, "\n");
      fclose(f);
   }

   WriteFile((HANDLE) info->fd, data, size, &written, NULL);
   return written;
}

/*----------------------------------------------------------------------------*/

int rs232_read(RS232_INFO * info, char *data, int size, int timeout)
{
   int status, i, l;
   COMMTIMEOUTS CommTimeOuts;

   GetCommTimeouts((HANDLE) info->fd, &CommTimeOuts);

   CommTimeOuts.ReadIntervalTimeout = 0;
   CommTimeOuts.ReadTotalTimeoutMultiplier = 0;
   CommTimeOuts.ReadTotalTimeoutConstant = timeout;

   SetCommTimeouts((HANDLE) info->fd, &CommTimeOuts);

   memset(data, 0, size);
   for (l = 0; l < size; l++) {
      status = ReadFile((HANDLE) info->fd, data + l, 1, &i, NULL);
      if (!status || i == 0)
         break;
   }

   if (debug_flag && l > 0) {
      FILE *f;

      f = fopen("rs232.log", "a");
      fprintf(f, "read: ");
      for (i = 0; i < size; i++)
         fprintf(f, "%X ", data[i]);
      fprintf(f, "\n");
      fclose(f);
   }

   return l;
}

/*----------------------------------------------------------------------------*/

int rs232_puts(RS232_INFO * info, char *str)
{
   DWORD written;

   if (debug_flag) {
      FILE *f;

      f = fopen("rs232.log", "a");
      fprintf(f, "puts: %s\n", str);
      fclose(f);
   }

   WriteFile((HANDLE) info->fd, str, strlen(str), &written, NULL);
   return written;
}

/*----------------------------------------------------------------------------*/

int rs232_gets(RS232_INFO * info, char *str, int size, char *pattern, int timeout)
{
   int status, i, l;
   COMMTIMEOUTS CommTimeOuts;

   GetCommTimeouts((HANDLE) info->fd, &CommTimeOuts);

   CommTimeOuts.ReadIntervalTimeout = 0;
   CommTimeOuts.ReadTotalTimeoutMultiplier = 0;
   CommTimeOuts.ReadTotalTimeoutConstant = timeout;

   SetCommTimeouts((HANDLE) info->fd, &CommTimeOuts);

   memset(str, 0, size);
   for (l = 0; l < size - 1; l++) {
      status = ReadFile((HANDLE) info->fd, str + l, 1, &i, NULL);
      if (!status || i == 0) {
         if (pattern && pattern[0])
            return 0;
         break;
      }

      if (pattern && pattern[0])
         if (strstr(str, pattern) != NULL)
            break;
   }

   if (debug_flag && l > 0) {
      FILE *f;

      f = fopen("rs232.log", "a");
      fprintf(f, "gets %s: %s\n", pattern, str);
      fclose(f);
   }

   return l;
}

#endif

/*---- Linux ----------------------------------------------------------------*/

#ifdef OS_LINUX

#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/timeb.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdarg.h>

int rs232_open(char *port, int baud, char parity, int data_bit, int stop_bit,
               int flow_control)
{
   int fd, i;
   struct termios tio;

   struct {
      int speed;
      int code;
   } baud_table[] = {
      {
      300, B300}, {
      600, B600}, {
      1200, B1200}, {
      1800, B1800}, {
      2400, B2400}, {
      4800, B4800}, {
      9600, B9600}, {
      19200, B19200}, {
      38400, B38400}, {
      0, 0}
   };


   fd = open(port, O_RDWR);
   if (fd < 0) {
      perror("rs232_open");
      return fd;
   }

   tio.c_iflag = 0;
   tio.c_oflag = 0;
   tio.c_cflag = CREAD | CLOCAL;
   if (data_bit == 7)
      tio.c_cflag |= CS7;
   else
      tio.c_cflag |= CS8;

   if (stop_bit == 2)
      tio.c_cflag |= CSTOPB;

   if (parity == 'E')
      tio.c_cflag |= PARENB;
   if (parity == 'O')
      tio.c_cflag |= PARENB | PARODD;

   if (flow_control == 1)
      tio.c_cflag |= CRTSCTS;
   if (flow_control == 2)
      tio.c_iflag |= IXON | IXOFF;

   tio.c_lflag = 0;
   tio.c_cc[VMIN] = 1;
   tio.c_cc[VTIME] = 0;

   /* check baud argument */
   for (i = 0; baud_table[i].speed; i++) {
      if (baud == baud_table[i].speed)
         break;
   }

   if (!baud_table[i].speed)
      i = 6;                    /* 9600 baud by default */

   cfsetispeed(&tio, baud_table[i].code);
   cfsetospeed(&tio, baud_table[i].code);

   tcsetattr(fd, TCSANOW, &tio);

   return fd;
}

/*----------------------------------------------------------------------------*/

int rs232_exit(RS232_INFO * info)
{
   close(info->fd);

   return SUCCESS;
}

/*----------------------------------------------------------------------------*/

int rs232_write(RS232_INFO * info, char *data, int size)
{
   int i;

   if (debug_flag) {
      FILE *f;

      f = fopen("rs232.log", "a");
      fprintf(f, "write: ");
      for (i = 0; (int) i < size; i++)
         fprintf(f, "%X ", data[i]);
      fprintf(f, "\n");
      fclose(f);
   }

   i = write(info->fd, data, size);

   return i;
}

/*----------------------------------------------------------------------------*/

int rs232_read(RS232_INFO * info, char *str, int size, int timeout)
{
   int i, l;
   struct timeb start, now;
   double fstart, fnow;

   ftime(&start);
   fstart = start.time + start.millitm / 1000.0;

   memset(str, 0, size);
   for (l = 0; l < size - 1;) {
      ioctl(info->fd, FIONREAD, &i);
      if (i > 0) {
         i = read(info->fd, str + l, 1);

         if (i == 1)
            l++;
         else
            perror("read");
      }

      ftime(&now);
      fnow = now.time + now.millitm / 1000.0;

      if (fnow - fstart >= timeout / 1000.0)
         break;

      if (i == 0)
         usleep(MIN(100000, timeout * 1000));
   }

   if (debug_flag && l > 0) {
      FILE *f;

      f = fopen("rs232.log", "a");
      fprintf(f, "read: ");
      for (i = 0; i < size; i++)
         fprintf(f, "%X ", str[i]);
      fprintf(f, "\n");
      fclose(f);
   }

   return l;
}

/*----------------------------------------------------------------------------*/

int rs232_puts(RS232_INFO * info, char *str)
{
   int i;

   if (debug_flag) {
      FILE *f;

      f = fopen("rs232.log", "a");
      fprintf(f, "puts: %s\n", str);
      fclose(f);
   }

   i = write(info->fd, str, strlen(str));

   return i;
}

/*----------------------------------------------------------------------------*/

int rs232_gets(RS232_INFO * info, char *str, int size, char *pattern, int timeout)
{
   int i, l;
   struct timeb start, now;
   double fstart, fnow;

   ftime(&start);
   fstart = start.time + start.millitm / 1000.0;

   memset(str, 0, size);
   for (l = 0; l < size - 1;) {
      ioctl(info->fd, FIONREAD, &i);
      if (i > 0) {
         i = read(info->fd, str + l, 1);

         if (i == 1)
            l++;
         else
            perror("read");
      }

      if (pattern && pattern[0])
         if (strstr(str, pattern) != NULL)
            break;

      ftime(&now);
      fnow = now.time + now.millitm / 1000.0;

      if (fnow - fstart >= timeout / 1000.0) {
         if (pattern && pattern[0])
            return 0;
         break;
      }

      if (i == 0)
         usleep(MIN(100000, timeout * 1000));

   };

   if (debug_flag && l > 0) {
      FILE *f;

      f = fopen("rs232.log", "a");
      fprintf(f, "getstr %s: %s\n", pattern, str);
      fclose(f);
   }

   return l;
}

#endif

/*----------------------------------------------------------------------------*/

int rs232_init(HNDLE hkey, void **pinfo)
{
   HNDLE hDB, hkeybd;
   INT size, status;
   RS232_INFO *info;

   /* allocate info structure */
   info = calloc(1, sizeof(RS232_INFO));
   *pinfo = info;

   cm_get_experiment_database(&hDB, NULL);

   /* create RS232 settings record */
   status = db_create_record(hDB, hkey, "BD", RS232_SETTINGS_STR);
   if (status != DB_SUCCESS)
      return FE_ERR_ODB;

   db_find_key(hDB, hkey, "BD", &hkeybd);
   size = sizeof(info->settings);
   db_get_record(hDB, hkeybd, &info->settings, &size, 0);

   /* open port */
   info->fd = rs232_open(info->settings.port,
                         info->settings.baud, info->settings.parity,
                         info->settings.data_bit, info->settings.stop_bit,
                         info->settings.flow_control);

   if (info->fd < 0)
      return FE_ERR_HW;

   return SUCCESS;
}

/*----------------------------------------------------------------------------*/

INT rs232(INT cmd, ...)
{
   va_list argptr;
   HNDLE hkey;
   INT status, size, timeout;
   void *info;
   char *str, *pattern;

   va_start(argptr, cmd);
   status = FE_SUCCESS;

   switch (cmd) {
   case CMD_INIT:
      hkey = va_arg(argptr, HNDLE);
      info = va_arg(argptr, void *);
      status = rs232_init(hkey, info);
      break;

   case CMD_EXIT:
      info = va_arg(argptr, void *);
      status = rs232_exit(info);
      break;

   case CMD_NAME:
      info = va_arg(argptr, void *);
      str = va_arg(argptr, char *);
      strcpy(str, "rs232");
      break;

   case CMD_WRITE:
      info = va_arg(argptr, void *);
      str = va_arg(argptr, char *);
      size = va_arg(argptr, int);
      status = rs232_write(info, str, size);
      break;

   case CMD_READ:
      info = va_arg(argptr, void *);
      str = va_arg(argptr, char *);
      size = va_arg(argptr, INT);
      timeout = va_arg(argptr, INT);
      status = rs232_read(info, str, size, timeout);
      break;

   case CMD_PUTS:
      info = va_arg(argptr, void *);
      str = va_arg(argptr, char *);
      status = rs232_puts(info, str);
      break;

   case CMD_GETS:
      info = va_arg(argptr, void *);
      str = va_arg(argptr, char *);
      size = va_arg(argptr, INT);
      pattern = va_arg(argptr, char *);
      timeout = va_arg(argptr, INT);
      status = rs232_gets(info, str, size, pattern, timeout);
      break;

   case CMD_DEBUG:
      status = va_arg(argptr, INT);
      debug_flag = status;
      break;
   }

   va_end(argptr);

   return status;
}
