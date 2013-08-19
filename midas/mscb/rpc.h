/********************************************************************\

  Name:         rpc.h
  Created by:   Stefan Ritt

  Contents:     Header fiel for MSCB RPC funcions

  $Log: rpc.h,v $
  Revision 1.1.1.1  2005/06/20 23:37:16  mucap
  Importing release 1.9.5 of the MIDAS source code as distributed by PSI.
  (Next, I'll commit our local customizations.)

  Revision 1.17  2004/10/03 18:08:43  olchansk
  in server_execute(), abort() on unknown commands
  replace ALIGN with ALIGN8 (same as in midas.h) to dodge namespace pollution on Darwin

  Revision 1.16  2004/03/10 13:28:25  midas
  mscb_init returns device name

  Revision 1.15  2004/03/10 10:28:48  midas
  Implemented test block write for speed tests

  Revision 1.14  2004/03/09 14:19:33  midas
  Fixed problems with write/read of strings

  Revision 1.13  2004/03/04 15:29:27  midas
  Added USB support

  Revision 1.12  2004/01/08 08:40:09  midas
  Implemented standard indentation

  Revision 1.11  2004/01/07 12:56:15  midas
  Chaned line length

  Revision 1.10  2004/01/07 12:52:23  midas
  Changed indentation

  Revision 1.9  2003/09/30 08:03:41  midas
  Implemented multiple RPC connections

  Revision 1.8  2003/09/23 09:25:27  midas
  Added RPC call for mscb_addr

  Revision 1.7  2003/05/12 10:30:45  midas
  Fixed name collisions with midas library

  Revision 1.6  2003/03/19 16:35:03  midas
  Eliminated configuration parameters

  Revision 1.5  2003/03/06 16:08:50  midas
  Protocol version 1.3 (change node name)

  Revision 1.4  2002/11/28 13:04:36  midas
  Implemented protocol version 1.2 (echo, read_channels)

  Revision 1.3  2002/11/27 16:26:22  midas
  Fixed errors under linux

  Revision 1.2  2002/11/20 12:01:39  midas
  Added host to mscb_init

  Revision 1.1  2002/10/28 14:26:30  midas
  Changes from Japan


\********************************************************************/

/* Data types */                /*                      min      max    */

#define TID_BYTE      1         /* unsigned byte         0       255    */
#define TID_SBYTE     2         /* signed byte         -128      127    */
#define TID_CHAR      3         /* single character      0       255    */
#define TID_WORD      4         /* two bytes             0      65535   */
#define TID_SHORT     5         /* signed word        -32768    32767   */
#define TID_DWORD     6         /* four bytes            0      2^32-1  */
#define TID_INT       7         /* signed dword        -2^31    2^31-1  */
#define TID_BOOL      8         /* four bytes bool       0        1     */
#define TID_FLOAT     9         /* 4 Byte float format                  */
#define TID_DOUBLE   10         /* 8 Byte float format                  */
#define TID_BITFIELD 11         /* 32 Bits Bitfield      0  111... (32) */
#define TID_STRING   12         /* zero terminated string               */
#define TID_ARRAY    13         /* array with unknown contents          */
#define TID_STRUCT   14         /* structure with fixed length          */
#define TID_KEY      15         /* key in online database               */
#define TID_LINK     16         /* link in online database              */
#define TID_LAST     17         /* end of TID list indicator            */

#define CBYTE(_i)        (* ((unsigned char *)   prpc_param[_i]))
#define CPBYTE(_i)       (  ((unsigned char *)   prpc_param[_i]))

#define CSHORT(_i)       (* ((short *)           prpc_param[_i]))
#define CPSHORT(_i)      (  ((short *)           prpc_param[_i]))

#define CINT(_i)         (* ((int *)             prpc_param[_i]))
#define CPINT(_i)        (  ((int *)             prpc_param[_i]))

#define CWORD(_i)        (* ((unsigned short *)  prpc_param[_i]))
#define CPWORD(_i)       (  ((unsigned short *)  prpc_param[_i]))

#define CLONG(_i)        (* ((long *)            prpc_param[_i]))
#define CPLONG(_i)       (  ((long *)            prpc_param[_i]))

#define CDWORD(_i)       (* ((unsigned int *)    prpc_param[_i]))
#define CPDWORD(_i)      (  ((unsigned int *)    prpc_param[_i]))

#define CBOOL(_i)        (* ((int *)             prpc_param[_i]))
#define CPBOOL(_i)       (  ((int *)             prpc_param[_i]))

#define CFLOAT(_i)       (* ((float *)           prpc_param[_i]))
#define CPFLOAT(_i)      (  ((float *)           prpc_param[_i]))

#define CDOUBLE(_i)      (* ((double *)          prpc_param[_i]))
#define CPDOUBLE(_i)     (  ((double *)          prpc_param[_i]))

#define CSTRING(_i)      (  ((char *)            prpc_param[_i]))
#define CARRAY(_i)       (  ((void *)            prpc_param[_i]))

/* flags */
#define RPC_IN       (1 << 0)
#define RPC_OUT      (1 << 1)
#define RPC_POINTER  (1 << 2)
#define RPC_FIXARRAY (1 << 3)
#define RPC_VARARRAY (1 << 4)
#define RPC_OUTGOING (1 << 5)

/* rpc parameter list */

typedef struct {
   unsigned short tid;
   unsigned short flags;
   int n;
} RPC_PARAM;


typedef struct {
   int id;
   char *name;
   RPC_PARAM param[20];
} RPC_LIST;

/* function list */

#define RPC_MSCB_INIT              1
#define RPC_MSCB_EXIT              2
#define RPC_MSCB_RESET             3
#define RPC_MSCB_REBOOT            4
#define RPC_MSCB_PING              5
#define RPC_MSCB_INFO              6
#define RPC_MSCB_INFO_VARIABLE     7
#define RPC_MSCB_SET_ADDR          8
#define RPC_MSCB_WRITE             9
#define RPC_MSCB_WRITE_GROUP      10
#define RPC_MSCB_FLASH            11
#define RPC_MSCB_UPLOAD           12
#define RPC_MSCB_READ             13
#define RPC_MSCB_READ_RANGE       14
#define RPC_MSCB_USER             15
#define RPC_MSCB_ECHO             16
#define RPC_MSCB_SET_NAME         17
#define RPC_MSCB_ADDR             18
#define RPC_MSCB_WRITE_BLOCK      19
#define RPC_MSCB_GET_DEVICE       20

/*------------------------------------------------------------------*/

/* Network structures */

typedef struct {
   int routine_id;              /* routine ID like ID_BM_xxx    */
   int param_size;              /* size in Bytes of parameter   */
} NET_COMMAND_HEADER;

typedef struct {
   NET_COMMAND_HEADER header;
   char param[32];              /* parameter array              */
} NET_COMMAND;

/* default listen port */
#define MSCB_RPC_PORT           1176

/* rpc timeout in milliseconds */
#define RPC_TIMEOUT            10000

/* RPC error codes */
#define RPC_SUCCESS                1
#define RPC_NET_ERROR              2
#define RPC_INVALID_ID             3
#define RPC_EXCEED_BUFFER          4
#define RPC_ERR_TIMEOUT            5

/* Align macro for data alignment on 8-byte boundary */
#define ALIGN8(x)  (((x)+7) & ~7)

/* maximal network packed size */
#define NET_TCP_SIZE           65535

/*------------------------------------------------------------------*/

/* function declarations */

void mrpc_server_loop(void);
int mrpc_connect(char *host_name);
int mrpc_connected(int fd);
int mrpc_disconnect(int sock);
int mrpc_call(int fdi, const int routine_id, ...);
