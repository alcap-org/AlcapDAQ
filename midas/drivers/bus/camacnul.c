/*-----------------------------------------------------------------------------
 *  Copyright (c) 1998      TRIUMF Data Acquistion Group
 *  Please leave this header in any reproduction of that distribution
 * 
 *  TRIUMF Data Acquisition Group, 4004 Wesbrook Mall, Vancouver, B.C. V6T 2A3
 *  Email: online@triumf.ca           Tel: (604) 222-1047  Fax: (604) 222-1074
 *         amaudruz@triumf.ca
 * -----------------------------------------------------------------------------
 *  
 *  Description	: CAMAC interface following the ESONE CAMAC calls.
 *                for NUll drivers
 * 
 *  Requires 	: none
 *
 *  Application : null camac interface for debugging
 *     
 *  Author:  Pierre-Andre Amaudruz Data Acquisition Group
 * 
 *  $Log: camacnul.c,v $
 *  Revision 1.1.1.1  2005/06/20 23:37:06  mucap
 *  Importing release 1.9.5 of the MIDAS source code as distributed by PSI.
 *  (Next, I'll commit our local customizations.)
 *
 *  Revision 1.5  2004/01/08 08:40:08  midas
 *  Implemented standard indentation
 *
 *  Revision 1.4  2003/08/20 21:21:35  pierre
 *  add cam_interrupt/inhibit_test()
 *
 *  Revision 1.3  2001/09/07 18:09:23  pierre
 *  Fix args for interrupts
 *
 *  Revision 1.2  2000/08/11 13:41:27  midas
 *  Changed parameter of rpc_cam_init
 *
 *  Revision 1.1  1999/12/20 10:18:55  midas
 *  Added files
 *
 *  Revision 1.6  1999/02/22 19:02:23  pierre
 *  - Remove came_xxx
 *
 *  Revision 1.5  1998/10/23 14:21:49  midas
 *  - Modified version scheme from 1.06 to 1.6.0
 *  - cm_get_version() now returns versino as string
 *
 *  Revision 1.4  1998/10/12 09:43:11  midas
 *  -SR- Changed header comment
 *
 *  Revision 1.3  1998/10/12 09:30:48  midas
 *  -SR- cam_lam_read returns always 1 (needed for test frontends)
 *
 *  Revision 1.2  1998/10/09 22:57:13  midas
 *  -PAA- int to DWORD *lam
 *
 *  Revision 1.0  1998        Pierre	 Initial revision
 *
 *------------------------------------------------------------------------ */

#include <mcstd.h>

/*------------------------------------------------------------------*/

INLINE void cam16i(const int c, const int n, const int a, const int f, WORD * d)
{
   *d = 0;
}
INLINE void cam24i(const int c, const int n, const int a, const int f, DWORD * d)
{
   *d = 0;
}
INLINE void cam16i_q(const int c, const int n, const int a, const int f, WORD * d, int *x,
                     int *q)
{
   *d = 0;
   *q = *x = 1;
}
INLINE void cam24i_q(const int c, const int n, const int a, const int f, DWORD * d,
                     int *x, int *q)
{
   *d = 0;
   *q = *x = 1;
}
INLINE void cam16i_r(const int c, const int n, const int a, const int f, WORD ** d,
                     const int r)
{
}
INLINE void cam24i_r(const int c, const int n, const int a, const int f, DWORD ** d,
                     const int r)
{
}
INLINE void cam16i_rq(const int c, const int n, const int a, const int f, WORD ** d,
                      const int r)
{
}
INLINE void cam24i_rq(const int c, const int n, const int a, const int f, DWORD ** d,
                      const int r)
{
}
INLINE void cam16i_sa(const int c, const int n, const int a, const int f, WORD ** d,
                      const int r)
{
}
INLINE void cam24i_sa(const int c, const int n, const int a, const int f, DWORD ** d,
                      const int r)
{
}
INLINE void cam16i_sn(const int c, const int n, const int a, const int f, WORD ** d,
                      const int r)
{
}
INLINE void cam24i_sn(const int c, const int n, const int a, const int f, DWORD ** d,
                      const int r)
{
}
INLINE void cami(const int c, const int n, const int a, const int f, WORD * d)
{
   *d = 0;
}
INLINE void cam16o(const int c, const int n, const int a, const int f, WORD d)
{
}
INLINE void cam24o(const int c, const int n, const int a, const int f, DWORD d)
{
}
INLINE void cam16o_q(const int c, const int n, const int a, const int f, WORD d, int *x,
                     int *q)
{
   *q = *x = 1;
}
INLINE void cam24o_q(const int c, const int n, const int a, const int f, DWORD d, int *x,
                     int *q)
{
   *q = *x = 1;
}
INLINE void cam16o_r(const int c, const int n, const int a, const int f, WORD * d,
                     const int r)
{
}
INLINE void cam24o_r(const int c, const int n, const int a, const int f, DWORD * d,
                     const int r)
{
}
INLINE void camo(const int c, const int n, const int a, const int f, WORD d)
{
}
INLINE int camc_chk(const int c)
{
   return 0;
}
INLINE void camc(const int c, const int n, const int a, const int f)
{
}
INLINE void camc_q(const int c, const int n, const int a, const int f, int *q)
{
   *q = 1;
}
INLINE void camc_sa(const int c, const int n, const int a, const int f, const int r)
{
}
INLINE void camc_sn(const int c, const int n, const int a, const int f, const int r)
{
}
INLINE int cam_init(void)
{
   return 1;
}
INLINE int cam_init_rpc(char *host_name, char *exp_name, char *fe_name, char *client_name,
                        char *rpc_server)
{
   return 1;
}
INLINE void cam_exit(void)
{
}
INLINE void cam_inhibit_set(const int c)
{
}
INLINE int cam_inhibit_test(const int c)
{
   return 1;
}
INLINE void cam_inhibit_clear(const int c)
{
}
INLINE void cam_crate_clear(const int c)
{
}
INLINE void cam_crate_zinit(const int c)
{
}
INLINE void cam_lam_enable(const int c, const int n)
{
}
INLINE void cam_lam_disable(const int c, const int n)
{
}
INLINE void cam_lam_read(const int c, DWORD * lam)
{
   *lam = 1;
}
INLINE void cam_lam_clear(const int c, const int n)
{
}
INLINE void cam_interrupt_enable(const int c)
{
}
INLINE void cam_interrupt_disable(const int c)
{
}
INLINE int cam_interrupt_test(const int c)
{
   return 1;
}
INLINE void cam_interrupt_attach(const int c, const int n, void (*isr) (void))
{
}
INLINE void cam_interrupt_detach(const int c, const int n)
{
}
INLINE void cam_glint_enable(void)
{
}
INLINE void cam_glint_disable(void)
{
}
INLINE void cam_glint_attach(int lam, void (*isr) (void))
{
}
INLINE void cam_glint_detach()
{
}
