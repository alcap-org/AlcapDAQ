/********************************************************************\

  Name:         mscb.h
  Created by:   Stefan Ritt

  Contents:     Midas Slow Control Bus protocol commands

  $Log: mscb.h,v $
  Revision 1.1.1.1  2005/06/20 23:37:17  mucap
  Importing release 1.9.5 of the MIDAS source code as distributed by PSI.
  (Next, I'll commit our local customizations.)

  Revision 1.44  2004/09/25 01:14:54  midas
  Started implementing slave port on SCS-1000

  Revision 1.43  2004/09/10 12:27:22  midas
  Version 1.7.5

  Revision 1.42  2004/07/21 14:27:16  midas
  Added PS0 to F120

  Revision 1.41  2004/07/21 14:18:55  midas
  Restructured timer usage

  Revision 1.40  2004/07/20 16:04:40  midas
  Implemented scs-1000 code

  Revision 1.39  2004/07/09 07:47:26  midas
  Added limits in variables

  Revision 1.38  2004/06/16 11:40:54  midas
  Implemented SUBM_xxx defines

  Revision 1.37  2004/05/14 07:55:48  midas
  Remove EEPROM support for SCS_300

  Revision 1.36  2004/05/12 13:18:36  midas
  *** empty log message ***

  Revision 1.35  2004/04/07 11:06:17  midas
  Version 1.7.1

  Revision 1.34  2004/04/05 08:46:31  midas
  Fixed LED bug with hvr_300

  Revision 1.33  2004/03/19 12:09:06  midas
  Upload with simplified CRC

  Revision 1.32  2004/03/19 07:10:33  midas
  Changed pinout for GPIB

  Revision 1.31  2004/03/05 12:29:00  midas
  Fixed bugs with F020

  Revision 1.30  2004/03/04 14:34:22  midas
  Added SCS_250

  Revision 1.29  2004/02/24 13:30:21  midas
  Implemented C8051F310 code

  Revision 1.28  2004/01/07 12:56:15  midas
  Chaned line length

  Revision 1.27  2004/01/07 12:52:23  midas
  Changed indentation

  Revision 1.26  2003/09/09 14:43:22  midas
  Added unit farad

  Revision 1.25  2003/06/27 13:52:17  midas
  Added led_mode()

  Revision 1.24  2003/06/05 14:47:16  midas
  Added SCS-520

  Revision 1.23  2003/03/24 15:00:31  midas
  Implemented 16-bit magic at end of EEPROM data

  Revision 1.22  2003/03/23 10:20:43  midas
  Added LCD_SUPPORT flag

  Revision 1.21  2003/03/19 16:35:03  midas
  Eliminated configuration parameters

  Revision 1.20  2003/03/14 13:47:54  midas
  Added SCS_310 code

  Revision 1.19  2003/03/06 16:08:50  midas
  Protocol version 1.3 (change node name)

  Revision 1.18  2003/03/06 11:00:17  midas
  Added section for SCS-600

  Revision 1.17  2003/02/19 16:05:13  midas
  Added code for ADuC812

  Revision 1.16  2003/01/30 08:35:30  midas
  Added watchdog switch

  Revision 1.15  2002/11/28 14:44:02  midas
  Removed SIZE_XBIT

  Revision 1.14  2002/11/28 13:03:41  midas
  Protocol version 1.2

  Revision 1.13  2002/11/20 12:02:09  midas
  Fixed bug with secondary LED

  Revision 1.12  2002/11/06 16:45:42  midas
  Revised LED blinking scheme

  Revision 1.11  2002/10/22 15:05:13  midas
  Added UNIT_FACTOR

  Revision 1.10  2002/10/09 15:48:13  midas
  Fixed bug with download

  Revision 1.9  2002/10/09 11:06:46  midas
  Protocol version 1.1

  Revision 1.8  2002/10/07 15:16:32  midas
  Added upgrade facility

  Revision 1.7  2002/10/04 09:03:20  midas
  Small mods for scs_300

  Revision 1.6  2002/10/03 15:31:53  midas
  Various modifications

  Revision 1.5  2002/08/08 06:45:38  midas
  Added time functions

  Revision 1.4  2002/07/10 09:53:00  midas
  Finished EEPROM routines

  Revision 1.3  2002/07/08 08:50:42  midas
  Added mscbutil functions

  Revision 1.2  2002/07/05 15:27:23  midas
  *** empty log message ***

  Revision 1.1  2002/07/05 08:12:46  midas
  Moved files

\********************************************************************/

/*---- CPU specific items ------------------------------------------*/

/* default flags */
#define USE_WATCHDOG
#undef  LCD_SUPPORT
#undef  LCD_DEBUG      // debug output on LCD
#define EEPROM_SUPPORT

/*--------------------------------*/
#if defined(SCS_210)
#include <c8051F020.h>
#define CPU_C8051F020
#define CPU_CYGNAL

#define LED_0 P3 ^ 4
#define LED_1 P3 ^ 3
#define LED_ON 0
sbit RS485_ENABLE = P3 ^ 5;

/*--------------------------------*/
#elif defined(SUBM_250)
#include "c8051F320.h" // don't use the one from Keil !!
#define CPU_C8051F320
#define CPU_CYGNAL

#define LED_0 P1 ^ 2
#define LED_1 P1 ^ 3
#define LED_ON 0
sbit RS485_ENABLE = P1 ^ 0;

#undef EEPROM_SUPPORT

/*--------------------------------*/
#elif defined(SCS_300) || defined(SCS_310)
#include <c8051F020.h>
#define CPU_C8051F020
#define CPU_CYGNAL

#define LED_0 P3 ^ 3
#define LED_1 P3 ^ 4
#define LED_ON 0
sbit RS485_ENABLE = P3 ^ 5;

/*--------------------------------*/
#elif defined(SUBM_300)
#include <c8051F020.h>
#define CPU_C8051F020
#define CPU_CYGNAL

#define LED_0 P3 ^ 3
#define LED_1 P3 ^ 4
#define LED_ON 0
sbit RS485_ENABLE = P3 ^ 5;

#undef EEPROM_SUPPORT

/*--------------------------------*/
#elif defined(SCS_400) || defined(SCS_500)
#include <c8051F000.h>
#define CPU_C8051F000
#define CPU_CYGNAL

#define LED_0 P3 ^ 4
#define LED_ON 1
sbit RS485_ENABLE = P3 ^ 5;

/*--------------------------------*/
#elif defined(SCS_520) || defined(SCS_600) || defined(SCS_700) || defined (SCS_800) || defined (SCS_900) || defined (SCS_910)
#include <c8051F000.h>
#define CPU_C8051F000
#define CPU_CYGNAL

#define LED_0 P3 ^ 4
#define LED_ON 0
sbit RS485_ENABLE = P3 ^ 5;

/*--------------------------------*/
#elif defined(SCS_1000)
#include <c8051F120.h>
#define CPU_C8051F120
#define CPU_CYGNAL

#define LED_0 P2 ^ 0
#define LED_1 P0 ^ 6
#define LED_2 P0 ^ 7 // buzzer
#define LED_ON 1     // will be zero when 2981 will be used instead of 2982
sbit RS485_ENABLE = P0 ^ 5;
sbit RS485_SEC_ENABLE = P0 ^ 4;

#undef USE_WATCHDOG
#define LCD_SUPPORT

/*--------------------------------*/
#elif defined(HVR_300)
#include <aduc812.h>
#define CPU_ADUC812

#define LED_0 P3 ^ 4
#define LED_ON 1
sbit RS485_ENABLE = P3 ^ 5;

/*--------------------------------*/
#elif defined(HVR_200)
#include <c8051F310.h>
#define CPU_C8051F310
#define CPU_CYGNAL

#define LED_0 P2 ^ 4
#define LED_1 P2 ^ 5 
#define LED_2 P2 ^ 6 
#define LED_3 P2 ^ 7
#define LED_ON 1
sbit RS485_ENABLE = P0 ^ 7;

/*--------------------------------*/
#else
#error Please define SCS_xxx or HVR_xxx in project options
#endif

#define LED_OFF !LED_ON

#if defined(LED_4)
#define N_LED 5
#elif defined(LED_3)
#define N_LED 4
#elif defined(LED_2)
#define N_LED 3
#elif defined(LED_1)
#define N_LED 2
#elif defined(LED_0)
#define N_LED 1
#else
#define N_LED 0
#endif
       
/* map SBUF0 & Co. to SBUF */
#if !defined(CPU_C8051F020) && !defined(CPU_C8051F120) && !defined(CPU_C8051F310) && !defined(CPU_C8051F320)
#define SCON0    SCON
#define SBUF0    SBUF
#define TI0      TI
#define RI0      RI
#define RB80     RB8
#define ES0      ES
#define PS0      PS
#endif

#if defined(CPU_C8051F020) || defined(CPU_C8051F120)
#define PS0      PS
#endif

#if defined(CPU_C8051F310) || defined(CPU_C8051F320)
#define EEPROM_OFFSET 0x3A00 // 0x3A00-0x3DFF = 1024 bytes
#else
#define EEPROM_OFFSET 0x8000
#endif

/*---- MSCB commands -----------------------------------------------*/

#define VERSION 0x17            // version 1.7

/* Version history:

1.0 Initial
1.1 Add unit prefix, implement upgrade command
1.2 Add CMD_ECHO, CMD_READ with multiple channels
1.3 Add "set node name"
1.4 Remove CMD_xxxs_CONF
1.5 Return 0x0A for protected pages on upload
1.6 Upload subpages of 60 bytes with ACK
1.7 Upload with simplified CRC code
*/

#define CMD_ADDR_NODE8  0x09
#define CMD_ADDR_NODE16 0x0A
#define CMD_ADDR_BC     0x10
#define CMD_ADDR_GRP8   0x11
#define CMD_ADDR_GRP16  0x12
#define CMD_PING8       0x19
#define CMD_PING16      0x1A

#define CMD_INIT        0x20
#define CMD_GET_INFO    0x28
#define CMD_SET_ADDR    0x34
#define CMD_SET_BAUD    0x39

#define CMD_FREEZE      0x41
#define CMD_SYNC        0x49
#define CMD_UPGRADE     0x50
#define CMD_USER        0x58

#define CMD_ECHO        0x61
#define CMD_TOKEN       0x68
#define CMD_SET_FLAGS   0x69

#define CMD_ACK         0x78

#define CMD_WRITE_NA    0x80
#define CMD_WRITE_ACK   0x88

#define CMD_FLASH       0x98

#define CMD_READ        0xA0

#define CMD_WRITE_BLOCK 0xB5
#define CMD_READ_BLOCK  0xB9

#define GET_INFO_GENERAL   0
#define GET_INFO_VARIABLES 1

/*---- flags from the configuration and status register (CSR) ------*/

#define CSR_DEBUG       (1<<0)
#define CSR_LCD_PRESENT (1<<1)
#define CSR_SYNC_MODE   (1<<2)
#define CSR_FREEZE_MODE (1<<3)
#define CSR_WD_RESET    (1<<2)

/*---- baud rates used ---------------------------------------------*/

#define BD_9600            1
#define BD_19200           2
#define BD_28800           3
#define BD_57600           4
#define BD_115200          5
#define BD_172800          6
#define BD_345600          7

/*---- info structures ---------------------------------------------*/

typedef struct {
   unsigned char protocol_version;
   unsigned char node_status;
   unsigned char n_variables;
   unsigned short node_address;
   unsigned short group_address;
   unsigned short watchdog_resets;
   char node_name[16];
} MSCB_INFO;

typedef struct {
   unsigned char width;         // width in bytes
   unsigned char unit;          // physical units UNIT_xxxx
   unsigned char prefix;        // unit prefix PRFX_xxx
   unsigned char status;        // status (not yet used)
   unsigned char flags;         // flags MSCBF_xxx
   char name[8];                // name
   void *ud;                    // point to user data buffer

#ifdef SCS_1000
   float min, max, delta;       // limits for button control
   unsigned short node_address; // address for remote node on subbus
   unsigned char  channel;      // address for remote channel subbus
#endif
} MSCB_INFO_VAR;

#define MSCBF_FLOAT    (1<<0)   // channel in floating point format
#define MSCBF_SIGNED   (1<<1)   // channel is signed integer
#define MSCBF_DATALESS (1<<2)   // channel doesn't contain data
#define MSCBF_HIDDEN   (1<<3)   // used for internal config parameters
#define MSCBF_REMIN    (1<<4)   // get variable from remote node on subbus
#define MSCBF_REMOUT   (1<<5)   // send variable to remote node on subbus

/* physical units */

#define PRFX_PICO       -12
#define PRFX_NANO        -9
#define PRFX_MICRO       -6
#define PRFX_MILLI       -3
#define PRFX_NONE         0
#define PRFX_KILO         3
#define PRFX_MEGA         6
#define PRFX_GIGA         9
#define PRFX_TERA        12

#define UNIT_UNDEFINED    0

// SI base units
#define UNIT_METER        1
#define UNIT_GRAM         2
#define UNIT_SECOND       3
#define UNIT_MINUTE       4
#define UNIT_HOUR         5
#define UNIT_AMPERE       6
#define UNIT_KELVIN       7
#define UNIT_CELSIUS      8
#define UNIT_FARENHEIT    9

// SI derived units

#define UNIT_HERTZ       20
#define UNIT_PASCAL      21
#define UNIT_BAR         22
#define UNIT_WATT        23
#define UNIT_VOLT        24
#define UNIT_OHM         25
#define UNIT_TESLA       26
#define UNIT_LITERPERSEC 27
#define UNIT_RPM         28
#define UNIT_FARAD       29

// computer units

#define UNIT_BOOLEAN     50
#define UNIT_BYTE        52
#define UNIT_WORD        53
#define UNIT_DWORD       54
#define UNIT_ASCII       55
#define UNIT_STRING      56
#define UNIT_BAUD        57

// others

#define UNIT_PERCENT     90
#define UNIT_PPM         91
#define UNIT_COUNT       92
#define UNIT_FACTOR      93

/*------------------------------------------------------------------*/

typedef struct {                // system info stored in EEPROM
   unsigned int node_addr;
   unsigned int group_addr;
   unsigned int wd_counter;
   char node_name[16];
} SYS_INFO;

#define ENABLE_INTERRUPTS { EA = 1; }
#define DISABLE_INTERRUPTS { EA = 0; }

/*---- function declarations ---------------------------------------*/

void watchdog_refresh(void);
void yield(void);
void led_set(unsigned char led, unsigned char flag) reentrant;
void led_blink(unsigned char led, unsigned char n, int interval) reentrant;
void led_mode(unsigned char led, unsigned char flag) reentrant;
void delay_us(unsigned int us);
void delay_ms(unsigned int ms);

unsigned char crc8(unsigned char *buffer, int len) reentrant;
unsigned char crc8_add(unsigned char crc, unsigned int c);

void lcd_setup();
void lcd_clear();
void lcd_goto(char x, char y);
void lcd_putc(char c);
void lcd_puts(char *str);
char scs_lcd1_read();

char getchar_nowait(void);
unsigned char gets_wait(char *str, unsigned char size, unsigned char timeout);
void flush(void);

void eeprom_flash(void);
unsigned char eeprom_retrieve(void);

void uart_init(unsigned char port, unsigned char baud);
unsigned char uart1_send(char *buffer, int size);
unsigned char uart1_receive(char *buffer, int size);
void send_remote_var(unsigned char i);

void sysclock_init(void);
unsigned long time(void);

unsigned char user_func(unsigned char *data_in, unsigned char *data_out);

void set_n_sub_addr(unsigned char n);
unsigned char cur_sub_addr(void);
