/********************************************************************\

  Name:         mscb.h
  Created by:   Stefan Ritt

  Contents:     Header fiel for MSCB funcions

  $Log: mscb.h,v $
  Revision 1.1.1.1  2005/06/20 23:37:16  mucap
  Importing release 1.9.5 of the MIDAS source code as distributed by PSI.
  (Next, I'll commit our local customizations.)

  Revision 1.36  2004/10/03 18:27:13  olchansk
  add MacOSX (aka darwin) native USB support
  make libusb support conditional on HAVE_LIBUSB (this allows one to use libusb on MacOSX)

  Revision 1.35  2004/07/08 11:14:45  midas
  Added size to mscb_select_device()

  Revision 1.34  2004/03/19 12:09:16  midas
  Upload with simplified CRC

  Revision 1.33  2004/03/11 09:58:10  midas
  mscb_init() does not ask for device if running under labview

  Revision 1.32  2004/03/10 13:28:25  midas
  mscb_init returns device name

  Revision 1.31  2004/03/10 10:28:48  midas
  Implemented test block write for speed tests

  Revision 1.30  2004/03/04 15:29:31  midas
  Added USB support

  Revision 1.29  2004/01/07 12:56:15  midas
  Chaned line length

  Revision 1.28  2004/01/07 12:52:23  midas
  Changed indentation

  Revision 1.27  2003/10/03 14:08:07  midas
  Added locking parameter to mscb_addr

  Revision 1.26  2003/09/23 09:25:26  midas
  Added RPC call for mscb_addr

  Revision 1.25  2003/09/09 14:43:22  midas
  Added unit farad

  Revision 1.24  2003/05/12 13:49:17  midas
  Added address check for SET_ADDR command

  Revision 1.23  2003/05/12 10:30:45  midas
  Fixed name collisions with midas library

  Revision 1.22  2003/04/03 08:21:44  midas
  Added submaster check

  Revision 1.21  2003/03/25 09:42:57  midas
  Added debugging facility

  Revision 1.20  2003/03/23 11:48:42  midas
  Added mscb_link()

  Revision 1.19  2003/03/19 16:35:03  midas
  Eliminated configuration parameters

  Revision 1.18  2003/03/06 16:08:50  midas
  Protocol version 1.3 (change node name)

  Revision 1.17  2002/11/28 14:44:09  midas
  Removed SIZE_XBIT

  Revision 1.16  2002/11/28 13:04:36  midas
  Implemented protocol version 1.2 (echo, read_channels)

  Revision 1.15  2002/11/27 15:40:05  midas
  Added version, fixed few bugs

  Revision 1.14  2002/11/20 12:01:30  midas
  Added host to mscb_init

  Revision 1.13  2002/10/28 14:26:30  midas
  Changes from Japan

  Revision 1.12  2002/10/22 15:03:38  midas
  Added UNIT_FACTOR

  Revision 1.11  2002/10/16 15:25:07  midas
  xxx16 now does 32 bit exchange

  Revision 1.10  2002/10/09 11:06:46  midas
  Protocol version 1.1

  Revision 1.9  2002/10/07 15:16:32  midas
  Added upgrade facility

  Revision 1.8  2002/10/03 15:30:40  midas
  Added linux support

  Revision 1.7  2002/08/12 12:11:20  midas
  Added mscb_reset1

  Revision 1.6  2002/07/10 09:51:13  midas
  Introduced mscb_flash()

  Revision 1.5  2001/10/31 11:16:54  midas
  Added IO check function

  Revision 1.4  2001/08/31 12:23:51  midas
  Added mutex protection

  Revision 1.3  2001/08/31 11:35:20  midas
  Added "wp" command in msc.c, changed parport to device in mscb.c

  Revision 1.2  2001/08/31 11:05:18  midas
  Added write16 and read16 (for LabView)


\********************************************************************/

/*---- MSCB commands -----------------------------------------------*/

#define MCMD_ADDR_NODE8  0x09
#define MCMD_ADDR_NODE16 0x0A
#define MCMD_ADDR_BC     0x10
#define MCMD_ADDR_GRP8   0x11
#define MCMD_ADDR_GRP16  0x12
#define MCMD_PING8       0x19
#define MCMD_PING16      0x1A

#define MCMD_INIT        0x20
#define MCMD_GET_INFO    0x28
#define MCMD_SET_ADDR    0x34
#define MCMD_SET_BAUD    0x39

#define MCMD_FREEZE      0x41
#define MCMD_SYNC        0x49
#define MCMD_UPGRADE     0x50
#define MCMD_USER        0x58

#define MCMD_ECHO        0x61
#define MCMD_TOKEN       0x68
#define MCMD_SET_FLAGS   0x69

#define MCMD_ACK         0x78

#define MCMD_WRITE_NA    0x80
#define MCMD_WRITE_ACK   0x88

#define MCMD_FLASH       0x98

#define MCMD_READ        0xA0

#define MCMD_WRITE_BLOCK 0xB5
#define MCMD_READ_BLOCK  0xB9

#define GET_INFO_GENERAL   0
#define GET_INFO_VARIABLE  1

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
} MSCB_INFO_VAR;

#define MSCBF_FLOAT  (1<<0)     // variable in floating point format
#define MSCBF_SIGNED (1<<1)     // variable is signed integer

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


/*---- file descriptor ---------------------------------------------*/

#define MSCB_MAX_FD      10

#define MSCB_TYPE_LPT     1
#define MSCB_TYPE_USB     2
#define MSCB_TYPE_COM     3
#define MSCB_TYPE_RPC     4

typedef struct {
   char device[256];
   int type;
   int fd;
   int remote_fd;
   int hr, hw;
} MSCB_FD;

extern MSCB_FD mscb_fd[MSCB_MAX_FD];

/*---- status codes ------------------------------------------------*/

#define MSCB_SUCCESS       1
#define MSCB_CRC_ERROR     2
#define MSCB_TIMEOUT       3
#define MSCB_INVAL_PARAM   4
#define MSCB_MUTEX         5
#define MSCB_FORMAT_ERROR  6
#define MSCB_NO_MEM        7
#define MSCB_SUBM_ERROR    8
#define MSCB_ADDR_EXISTS   9

/*---- Byte and Word swapping big endian <-> little endian ---------*/
#define WORD_SWAP(x) { unsigned char _tmp;                               \
                       _tmp= *((unsigned char *)(x));                    \
                       *((unsigned char *)(x)) = *(((unsigned char *)(x))+1);     \
                       *(((unsigned char *)(x))+1) = _tmp; }

#define DWORD_SWAP(x) { unsigned char _tmp;                              \
                       _tmp= *((unsigned char *)(x));                    \
                       *((unsigned char *)(x)) = *(((unsigned char *)(x))+3);     \
                       *(((unsigned char *)(x))+3) = _tmp;               \
                       _tmp= *(((unsigned char *)(x))+1);                \
                       *(((unsigned char *)(x))+1) = *(((unsigned char *)(x))+2); \
                       *(((unsigned char *)(x))+2) = _tmp; }

/*---- function declarations ---------------------------------------*/

/* make functions callable from a C++ program */
#ifdef __cplusplus
extern "C" {
#endif

/* make functions under WinNT dll exportable */
#if defined(_MSC_VER) && defined(_USRDLL)
#define EXPRT __declspec(dllexport)
#else
#define EXPRT
#endif

   int EXPRT mscb_init(char *device, int size, int debug);
   int EXPRT mscb_select_device(char *data, int size, int select);
   void EXPRT mscb_get_device(int fd, char *device, int bufsize);
   void EXPRT mscb_get_version(char *lib_version, char *prot_version);
   void EXPRT mscb_check(char *device, int size);
   int EXPRT mscb_exit(int fd);
   int EXPRT mscb_reset(int fd);
   int EXPRT mscb_reboot(int fd, int adr);
   int EXPRT mscb_ping(int fd, int adr);
   int EXPRT mscb_echo(int fd, int addr, unsigned char d1, unsigned char *d2);
   int EXPRT mscb_info(int fd, int adr, MSCB_INFO * info);
   int EXPRT mscb_info_variable(int fd, int adr, int index, MSCB_INFO_VAR * info);
   int EXPRT mscb_set_addr(int fd, int adr, int node, int group);
   int EXPRT mscb_set_name(int fd, int adr, char *name);
   int EXPRT mscb_write(int fd, int adr, unsigned char index, void *data, int size);
   int EXPRT mscb_write_group(int fd, int adr, unsigned char index, void *data, int size);
   int EXPRT mscb_write_block(int fd, int adr, unsigned char index, void *data, int size);
   int EXPRT mscb_flash(int fd, int adr);
   int EXPRT mscb_upload(int fd, int adr, char *buffer, int size, int debug);
   int EXPRT mscb_read(int fd, int adr, unsigned char index, void *data, int *size);
   int EXPRT mscb_read_range(int fd, int adr, unsigned char index1,
                             unsigned char index2, void *data, int *size);
   int EXPRT mscb_user(int fd, int adr, void *param, int size, void *result, int *rsize);
   int EXPRT mscb_link(int fd, int adr, unsigned char index, void *data, int size);
   int EXPRT mscb_addr(int fd, int cmd, int adr, int retry, int lock);

#ifdef __cplusplus
}
#endif
/* define missing linux functions */
#if defined(OS_UNIX)
int kbhit();
#define getch() getchar()
#define Sleep(x) usleep(x*1000)

#endif
