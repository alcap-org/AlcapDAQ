/*-----------------------------------------------------------------------------
 *  Copyright (c) 1998      TRIUMF Data Acquistion Group
 *  Please leave this header in any reproduction of that distribution
 * 
 *  TRIUMF Data Acquisition Group, 4004 Wesbrook Mall, Vancouver, B.C. V6T 2A3
 *  Email: online@triumf.ca           Tel: (604) 222-1047  Fax: (604) 222-1074
 *         amaudruz@triumf.ca
 * -----------------------------------------------------------------------------
 *  
 *  Description : Midas Camac Standard calls. 
 *  Requires : 
 *  Application : Used in any camac driver
 *  Author:  Pierre-Andre Amaudruz Data Acquisition Group
 *
 *  $Log: mcstd.h,v $
 *  Revision 1.1.1.1  2005/06/20 23:37:14  mucap
 *  Importing release 1.9.5 of the MIDAS source code as distributed by PSI.
 *  (Next, I'll commit our local customizations.)
 *
 *  Revision 1.13  2004/01/13 00:50:53  pierre
 *  fix dox comment for vxworks
 *
 *  Revision 1.12  2004/01/08 08:40:09  midas
 *  Implemented standard indentation
 *
 *  Revision 1.11  2004/01/08 06:30:02  pierre
 *  Doxygen the file
 *
 *  Revision 1.10  2002/02/08 06:46:42  pierre
 *  fix doc++ comments
 *
 *  Revision 1.9  2001/08/21 11:18:28  midas
 *  Added c,n to cam_interrupt_attach/detach, added comments
 *
 *  Revision 1.8  2001/08/21 09:32:02  midas
 *  Added cam_lam_wait
 *
 *  Revision 1.7  2001/08/13 11:25:34  midas
 *  Added some new functions
 *
 *  Revision 1.6  2000/08/10 07:49:04  midas
 *  Added client name together with frontend name in cam_init_rpc
 *
 *  Revision 1.5  2000/04/17 17:38:10  pierre
 *  - First round of doc++ comments
 *
 *  Revision 1.4  1999/02/19 21:59:59  pierre
 *  - Moved came_xxx to esone
 *
 *  Revision 1.3  1998/10/13 07:04:29  midas
 *  Added Log in header
 *
 *
 *  Previous revision 1.0  1998        Pierre  Initial revision
 *                    1.1  JUL 98      SR      Added 8-bit functions, BYTE definition
 *---------------------------------------------------------------------------*/

/**dox***************************************************************/
/** @file mcstd.h
The Midas CAMAC include file
*/

/** @defgroup mcstdinclude Midas CAMAC standard 
 */
/** @defgroup mcstdfunctionh Camac Functions (camxxx) 
 */

/**dox***************************************************************/
/** @addtogroup mcstdinclude
 *  
 *  @{  */

/**dox***************************************************************/
#ifndef DOXYGEN_SHOULD_SKIP_THIS

#ifndef INLINE
#if defined( _MSC_VER )
#define INLINE __inline
#elif defined(__GNUC__)
#define INLINE __inline__
#else
#define INLINE
#endif
#endif

/* make functions under WinNT dll exportable */
#if defined(_MSC_VER) && defined(MIDAS_DLL)
#define EXPRT __declspec(dllexport)
#else
#define EXPRT
#endif

#define EXTERNAL extern

#ifndef MIDAS_TYPE_DEFINED
#define MIDAS_TYPE_DEFINED

typedef unsigned char BYTE;
typedef unsigned short int WORD;

#ifdef __alpha
typedef unsigned int DWORD;
#else
typedef unsigned long int DWORD;
#endif

#define SUCCESS  1

#endif                          /* MIDAS_TYPE_DEFINED */


/*------------------------------------------------------------------*/

/* make functions callable from a C++ program */
#ifdef __cplusplus
extern "C" {
#endif

   EXTERNAL INLINE void EXPRT cam8i(const int c, const int n, const int a, const int f,
                                    BYTE * d);
   EXTERNAL INLINE void EXPRT cam8i_r(const int c, const int n, const int a, const int f,
                                      BYTE ** d, const int r);

/**dox***************************************************************/
#endif                          /* DOXYGEN_SHOULD_SKIP_THIS */

/**dox***************************************************************/
/** @addtogroup mcstdfunctionh
 *  
 *  @{  */

/********************************************************************/
/**
16 bits input.
@param c crate number (0..)
@param n station number (0..30)
@param a sub-address (0..15)
@param f function (0..7)
@param d data read out data
@return void
*/
   EXTERNAL INLINE void EXPRT cam16i(const int c, const int n, const int a, const int f,
                                     WORD * d);

/********************************************************************/
/**
24 bits input.
@param c crate number (0..)
@param n station number (0..30)
@param a sub-address (0..15)
@param f function (0..7)
@param d data read out data
@return void
*/
   EXTERNAL INLINE void EXPRT cam24i(const int c, const int n, const int a, const int f,
                                     DWORD * d);

/********************************************************************/
/**
8 bits input with Q response.
@param c crate number (0..)
@param n station number (0..30)
@param a sub-address (0..15)
@param f function (0..7)
@param d data read out data
@param x X response (0:failed,1:success)
@param q Q resonpse (0:no Q, 1: Q)
@return void
*/
   EXTERNAL INLINE void EXPRT cam8i_q(const int c, const int n, const int a, const int f,
                                      BYTE * d, int *x, int *q);

/********************************************************************/
/**
16 bits input with Q response.
@param c crate number (0..)
@param n station number (0..30)
@param a sub-address (0..15)
@param f function (0..7)
@param d data read out data
@param x X response (0:failed,1:success)
@param q Q resonpse (0:no Q, 1: Q)
@return void
*/
   EXTERNAL INLINE void EXPRT cam16i_q(const int c, const int n, const int a, const int f,
                                       WORD * d, int *x, int *q);

/********************************************************************/
/**
24 bits input with Q response.
@param c crate number (0..)
@param n station number (0..30)
@param a sub-address (0..15)
@param f function (0..7)
@param d data read out data
@param x X response (0:failed,1:success)
@param q Q resonpse (0:no Q, 1: Q)
@return void
*/
   EXTERNAL INLINE void EXPRT cam24i_q(const int c, const int n, const int a, const int f,
                                       DWORD * d, int *x, int *q);


/********************************************************************/
/**
Repeat 16 bits input.
@param c crate number (0..)
@param n station number (0..30)
@param a sub-address (0..15)
@param f function (0..7)
@param d data read out data
@param r repeat time
@return void
*/
   EXTERNAL INLINE void EXPRT cam16i_r(const int c, const int n, const int a, const int f,
                                       WORD ** d, const int r);

/********************************************************************/
/**
Repeat 24 bits input.
@param c crate number (0..)
@param n station number (0..30)
@param a sub-address (0..15)
@param f function (0..7)
@param d data read out
@param r repeat time
@return void
*/
   EXTERNAL INLINE void EXPRT cam24i_r(const int c, const int n, const int a, const int f,
                                       DWORD ** d, const int r);

/********************************************************************/
/**
Repeat 8 bits input with Q stop.
@param c crate number (0..)
@param n station number (0..30)
@param a sub-address (0..15)
@param f function (0..7)
@param d pointer to data read out
@param r repeat time
@return void
*/
   EXTERNAL INLINE void EXPRT cam8i_rq(const int c, const int n, const int a, const int f,
                                       BYTE ** d, const int r);

/********************************************************************/
/**
Repeat 16 bits input with Q stop.
@param c crate number (0..)
@param n station number (0..30)
@param a sub-address (0..15)
@param f function (0..7)
@param d pointer to data read out
@param r repeat time
@return void
*/
   EXTERNAL INLINE void EXPRT cam16i_rq(const int c, const int n, const int a,
                                        const int f, WORD ** d, const int r);

/********************************************************************/
/**
Repeat 24 bits input with Q stop.
@param c crate number (0..)
@param n station number (0..30)
@param a sub-address (0..15)
@param f function (0..7)
@param d pointer to data read out
@param r repeat time
@return void
*/
   EXTERNAL INLINE void EXPRT cam24i_rq(const int c, const int n, const int a,
                                        const int f, DWORD ** d, const int r);

/********************************************************************/
/**
Read the given CAMAC address and increment the sub-address by one. Repeat r times.

\code
BYTE pbkdat[4];
cam8i_sa(crate, 5, 0, 2, &pbkdat, 4);
\endcode
equivalent to :
\code 
cam8i(crate, 5, 0, 2, &pbkdat[0]);
cam8i(crate, 5, 1, 2, &pbkdat[1]);
cam8i(crate, 5, 2, 2, &pbkdat[2]);
cam8i(crate, 5, 3, 2, &pbkdat[3]);
\endcode
@param c crate number (0..)
@param n station number (0..30)
@param a sub-address (0..15)
@param f function (0..7)
@param d pointer to data read out
@param r number of consecutive sub-address to read
@return void
*/
   EXTERNAL INLINE void EXPRT cam8i_sa(const int c, const int n, const int a, const int f,
                                       BYTE ** d, const int r);

/********************************************************************/
/**
Read the given CAMAC address and increment the sub-address by one. Repeat r times.

\code
WORD pbkdat[4];
cam16i_sa(crate, 5, 0, 2, &pbkdat, 4);
\endcode equivalent to :
\code 
cam16i(crate, 5, 0, 2, &pbkdat[0]);
cam16i(crate, 5, 1, 2, &pbkdat[1]);
cam16i(crate, 5, 2, 2, &pbkdat[2]);
cam16i(crate, 5, 3, 2, &pbkdat[3]);
\endcode
@param c crate number (0..)
@param n station number (0..30)
@param a sub-address (0..15)
@param f function (0..7)
@param d pointer to data read out
@param r number of consecutive sub-address to read
@return void
*/
   EXTERNAL INLINE void EXPRT cam16i_sa(const int c, const int n, const int a,
                                        const int f, WORD ** d, const int r);

/********************************************************************/
/**
Read the given CAMAC address and increment the sub-address by one. Repeat r times.

\code
DWORD pbkdat[8];
cam24i_sa(crate, 5, 0, 2, &pbkdat, 8);
\endcode
equivalent to
\code
cam24i(crate, 5, 0, 2, &pbkdat[0]);
cam24i(crate, 6, 0, 2, &pbkdat[1]);
cam24i(crate, 7, 0, 2, &pbkdat[2]);
cam24i(crate, 8, 0, 2, &pbkdat[3]);
\endcode
@param c crate number (0..)
@param n station number (0..30)
@param a sub-address (0..15)
@param f function (0..7)
@param d pointer to data read out
@param r number of consecutive sub-address to read
@return void
*/
   EXTERNAL INLINE void EXPRT cam24i_sa(const int c, const int n, const int a,
                                        const int f, DWORD ** d, const int r);

/********************************************************************/
/**
Read the given CAMAC address and increment the station number by one. Repeat r times.

\code
BYTE pbkdat[4];
cam8i_sa(crate, 5, 0, 2, &pbkdat, 4);
\endcode
equivalent to :
\code
cam8i(crate, 5, 0, 2, &pbkdat[0]);
cam8i(crate, 6, 0, 2, &pbkdat[1]);
cam8i(crate, 7, 0, 2, &pbkdat[2]);
cam8i(crate, 8, 0, 2, &pbkdat[3]);
\endcode
@param c crate number (0..)
@param n station number (0..30)
@param a sub-address (0..15)
@param f function (0..7)
@param d pointer to data read out
@param r number of consecutive station to read
@return void
*/
   EXTERNAL INLINE void EXPRT cam8i_sn(const int c, const int n, const int a, const int f,
                                       BYTE ** d, const int r);

/********************************************************************/
/**
Read the given CAMAC address and increment the station number by one. Repeat r times.

\code
WORD pbkdat[4];
cam16i_sa(crate, 5, 0, 2, &pbkdat, 4);
\endcode
equivalent to :
\code
cam16i(crate, 5, 0, 2, &pbkdat[0]);
cam16i(crate, 6, 0, 2, &pbkdat[1]);
cam16i(crate, 7, 0, 2, &pbkdat[2]);
cam16i(crate, 8, 0, 2, &pbkdat[3]);
\endcode
@param c crate number (0..)
@param n station number (0..30)
@param a sub-address (0..15)
@param f function (0..7)
@param d pointer to data read out
@param r number of consecutive station to read
@return void
*/
   EXTERNAL INLINE void EXPRT cam16i_sn(const int c, const int n, const int a,
                                        const int f, WORD ** d, const int r);

/********************************************************************/
/**
Read the given CAMAC address and increment the station number by one. Repeat r times.

\code
DWORD pbkdat[4];
cam24i_sa(crate, 5, 0, 2, &pbkdat, 4);
\endcode
equivalent to :
\code
cam24i(crate, 5, 0, 2, &pbkdat[0]);
cam24i(crate, 6, 0, 2, &pbkdat[1]);
cam24i(crate, 7, 0, 2, &pbkdat[2]);
cam24i(crate, 8, 0, 2, &pbkdat[3]);
\endcode
@param c crate number (0..)
@param n station number (0..30)
@param a sub-address (0..15)
@param f function (0..7)
@param d pointer to data read out
@param r number of consecutive station to read
@return void
*/
   EXTERNAL INLINE void EXPRT cam24i_sn(const int c, const int n, const int a,
                                        const int f, DWORD ** d, const int r);
/********************************************************************/

/**
Same as cam16i()
*/
   EXTERNAL INLINE void EXPRT cami(const int c, const int n, const int a, const int f,
                                   WORD * d);

/********************************************************************/
/**
Write data to given CAMAC address.
@param c crate number (0..)
@param n station number (0..30)
@param a sub-address (0..15)
@param f function (16..31)
@param d data to be written to CAMAC
@return void
*/
   EXTERNAL INLINE void EXPRT cam8o(const int c, const int n, const int a, const int f,
                                    BYTE d);

/********************************************************************/
/**
Write data to given CAMAC address.
@param c crate number (0..)
@param n station number (0..30)
@param a sub-address (0..15)
@param f function (16..31)
@param d data to be written to CAMAC
@return void
*/
   EXTERNAL INLINE void EXPRT cam16o(const int c, const int n, const int a, const int f,
                                     WORD d);

/********************************************************************/
/**
Write data to given CAMAC address.
@param c crate number (0..)
@param n station number (0..30)
@param a sub-address (0..15)
@param f function (16..31)
@param d data to be written to CAMAC
@return void
*/
   EXTERNAL INLINE void EXPRT cam24o(const int c, const int n, const int a, const int f,
                                     DWORD d);

/********************************************************************/
/**
Write data to given CAMAC address with Q response.
@param c crate number (0..)
@param n station number (0..30)
@param a sub-address (0..15)
@param f function (16..31)
@param d data to be written to CAMAC
@param x X response (0:failed,1:success)
@param q Q resonpse (0:no Q, 1: Q)
@return void
*/
   EXTERNAL INLINE void EXPRT cam8o_q(const int c, const int n, const int a, const int f,
                                      BYTE d, int *x, int *q);

/********************************************************************/
/**
Write data to given CAMAC address with Q response.
@param c crate number (0..)
@param n station number (0..30)
@param a sub-address (0..15)
@param f function (16..31)
@param d data to be written to CAMAC
@param x X response (0:failed,1:success)
@param q Q resonpse (0:no Q, 1: Q)
@return void
*/
   EXTERNAL INLINE void EXPRT cam16o_q(const int c, const int n, const int a, const int f,
                                       WORD d, int *x, int *q);

/********************************************************************/
/**
Write data to given CAMAC address with Q response.
@param c crate number (0..)
@param n station number (0..30)
@param a sub-address (0..15)
@param f function (16..31)
@param d data to be written to CAMAC
@param x X response (0:failed,1:success)
@param q Q response (0:no Q, 1: Q)
@return void
*/
   EXTERNAL INLINE void EXPRT cam24o_q(const int c, const int n, const int a, const int f,
                                       DWORD d, int *x, int *q);

/********************************************************************/
/**
Repeat write data to given CAMAC address r times.
@param c crate number (0..)
@param n station number (0..30)
@param a sub-address (0..15)
@param f function (16..31)
@param d data to be written to CAMAC
@param r number of repeatition
@return void
*/
   EXTERNAL INLINE void EXPRT cam8o_r(const int c, const int n, const int a, const int f,
                                      BYTE * d, const int r);

/********************************************************************/
/**
Repeat write data to given CAMAC address r times.
@param c crate number (0..)
@param n station number (0..30)
@param a sub-address (0..15)
@param f function (16..31)
@param d data to be written to CAMAC
@param r number of repeatition
@return void
*/
   EXTERNAL INLINE void EXPRT cam16o_r(const int c, const int n, const int a, const int f,
                                       WORD * d, const int r);

/********************************************************************/
/**
Repeat write data to given CAMAC address r times.
@param c crate number (0..)
@param n station number (0..30)
@param a sub-address (0..15)
@param f function (16..31)
@param d data to be written to CAMAC
@param r number of repeatition
@return void
*/
   EXTERNAL INLINE void EXPRT cam24o_r(const int c, const int n, const int a, const int f,
                                       DWORD * d, const int r);

/********************************************************************/
/**
Same as cam16o()
*/
   EXTERNAL INLINE void EXPRT camo(const int c, const int n, const int a, const int f,
                                   WORD d);

/********************************************************************/
/**
Crate presence check.
@param c crate number (0..)
@return 0:Success, -1:No CAMAC response
*/
   EXTERNAL INLINE int EXPRT camc_chk(const int c);

/********************************************************************/
/**
CAMAC command (no data).
@param c crate number (0..)
@param n station number (0..30)
@param a sub-address (0..15)
@param f function (8..15, 24..31)
@return void
*/
   EXTERNAL INLINE void EXPRT camc(const int c, const int n, const int a, const int f);

/********************************************************************/
/**
CAMAC command with Q response (no data).
@param c crate number (0..)
@param n station number (0..30)
@param a sub-address (0..15)
@param f function (8..15, 24..31)
@param q Q response (0:no Q, 1:Q)
@return void
*/
   EXTERNAL INLINE void EXPRT camc_q(const int c, const int n, const int a, const int f,
                                     int *q);

/********************************************************************/
/**
Scan CAMAC command on sub-address.
@param c crate number (0..)
@param n station number (0..30)
@param a sub-address (0..15)
@param f function (8..15, 24..31)
@param r number of consecutive sub-address to read
@return void
*/
   EXTERNAL INLINE void EXPRT camc_sa(const int c, const int n, const int a, const int f,
                                      const int r);

/********************************************************************/
/**
Scan CAMAC command on station.
@param c crate number (0..)
@param n station number (0..30)
@param a sub-address (0..15)
@param f function (8..15, 24..31)
@param r number of consecutive station to read
@return void
*/
   EXTERNAL INLINE void EXPRT camc_sn(const int c, const int n, const int a, const int f,
                                      const int r);

/********************************************************************/
/**
Initialize CAMAC access.
@return 1: success
*/
   EXTERNAL INLINE int EXPRT cam_init(void);

/********************************************************************/
/**
Initialize CAMAC access for rpc calls
@internal 
@param host_name Midas host to contact
@param exp_name  Midas experiment to contact
@param fe_name   frontend application name to contact
@param client_name RPC host name
@param rpc_server RPC server name
@return 1: success
*/
   EXTERNAL INLINE int EXPRT cam_init_rpc(char *host_name, char *exp_name, char *fe_name,
                                          char *client_name, char *rpc_server);

/********************************************************************/
/**
Close CAMAC accesss.
*/
   EXTERNAL INLINE void EXPRT cam_exit(void);

/********************************************************************/
/**
Set Crate inhibit.
@param c crate number (0..)
@return void
*/
   EXTERNAL INLINE void EXPRT cam_inhibit_set(const int c);

/********************************************************************/
/**
Clear Crate inhibit.
@param c crate number (0..)
@return void
*/
   EXTERNAL INLINE void EXPRT cam_inhibit_clear(const int c);

/********************************************************************/
/**
Test Crate Inhibit.
@param c crate number (0..)
@return 1 for set, 0 for cleared
*/
   EXTERNAL INLINE int EXPRT cam_inhibit_test(const int c);

/********************************************************************/
/**
Issue CLEAR to crate.
@param c crate number (0..)
@return void
*/
   EXTERNAL INLINE void EXPRT cam_crate_clear(const int c);

/********************************************************************/
/**
Issue Z to crate.
@param c crate number (0..)
@return void
*/
   EXTERNAL INLINE void EXPRT cam_crate_zinit(const int c);

/********************************************************************/
/**
Enable LAM generation for given station to the Crate controller.
It doesn't enable the LAM of the actual station itself.
@param c crate number (0..)
@param n LAM station
@return void
*/
   EXTERNAL INLINE void EXPRT cam_lam_enable(const int c, const int n);

/********************************************************************/
/**
Disable LAM generation for given station to the Crate controller.
It doesn't disable the LAM of the actual station itself.
@param c crate number (0..)
@param n LAM station
@return void
*/
   EXTERNAL INLINE void EXPRT cam_lam_disable(const int c, const int n);

/********************************************************************/
/**
Reads in lam the lam pattern of the entire crate.
@param c crate number (0..)
@param lam LAM pattern of the crate
@return void
*/
   EXTERNAL INLINE void EXPRT cam_lam_read(const int c, DWORD * lam);

/********************************************************************/
/**
Clear the LAM register of the crate controller.
It doesn't clear the LAM of the particular station.
@param c crate number (0..)
@param n LAM station
@return void
*/
   EXTERNAL INLINE void EXPRT cam_lam_clear(const int c, const int n);

/********************************************************************/
/**
Wait for a LAM to occur with a certain timeout. Return
crate and station if LAM occurs.
@param c crate number (0..)
@param n LAM station
@param millisec If there is no LAM after this timeout, the routine returns
@return 1 if LAM occured, 0 else
*/
   EXTERNAL INLINE int EXPRT cam_lam_wait(int *c, DWORD * n, const int millisec);

/********************************************************************/
/**
Enable interrupts in specific crate
@param c crate number (0..)
@return void
*/
   EXTERNAL INLINE void EXPRT cam_interrupt_enable(const int c);

/********************************************************************/
/**
Disables interrupts in specific crate
@param c crate number (0..)
@return void
*/
   EXTERNAL INLINE void EXPRT cam_interrupt_disable(const int c);

/********************************************************************/
/**
Test Crate Interrupt.
@param c crate number (0..)
@return 1 for set, 0 for cleared
*/
   EXTERNAL INLINE int EXPRT cam_interrupt_test(const int c);

/********************************************************************/
/**
Attach service routine to LAM of specific crate and station.
@param c crate number (0..)
@param n station number
@param (*isr) Function pointer to attach to the LAM
@return void
*/
   EXTERNAL INLINE void EXPRT cam_interrupt_attach(const int c, const int n,
                                                   void (*isr) (void));

/********************************************************************/
/**
Detach service routine from LAM.
@param c crate number (0..)
@param n station number
@return void
*/
   EXTERNAL INLINE void EXPRT cam_interrupt_detach(const int c, const int n);

#ifdef __cplusplus
}
#endif

/**dox***************************************************************/
/** @} */ /* end of mcstdfunctinoh */
/**dox***************************************************************/
/** @} */ /* end of mcstdinclude */
