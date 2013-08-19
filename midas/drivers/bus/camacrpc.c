/********************************************************************

  Name:         camacrpc.c
  Created by:   Stefan Ritt

  Contents:     CAMAC RPC driver, works together with CAMAC RPC server,
                either as dedicated server (camacsrv.c) or inside a
                CAMAC frontend (mfe.c)

  $Log: camacrpc.c,v $
  Revision 1.2  2005/06/20 23:46:25  mucap
  (Fred)
  The following construct (which appears repeatedly in this file):
              ((char *) (*d)) += size;
  is not accepted by gcc 4.0 (the standard compiler in Fedora Core 4), so I
  rewrote it as:
              *d = (DWORD *) ((char *) (*d) + size);

  Revision 1.6  2004/01/08 08:40:08  midas
  Implemented standard indentation

  Revision 1.5  2003/04/23 20:57:52  pierre
  add client_name arg to rpc_client_connect

  Revision 1.4  2002/05/08 19:54:40  midas
  Added extra parameter to function db_get_value()

  Revision 1.3  2001/08/28 08:43:46  midas
  Changed dummy interrupt function parameters

  Revision 1.2  2000/08/10 07:48:29  midas
  Added client name together with frontend name in cam_init_rpc

  Revision 1.1  1999/12/20 10:18:55  midas
  Added files

  Revision 1.7  1999/05/06 18:49:54  pierre
  - Fix return code using midas code.

  Revision 1.6  1999/02/22 19:04:50  pierre
  - Remove came_xxx

  Revision 1.5  1998/11/22 18:27:58  midas
  Fixed &x and &q pointer bugs in various functions

  Revision 1.4  1998/10/13 07:23:21  midas
  Added void to cam_interrupt_attach(void (*isr)(void))

  Revision 1.3  1998/10/12 09:34:08  midas
  -SR- added back CNAF_TEST instead of CNAF. This way, no CAMAC cycle is executed
       during the test.

  Revision 1.2  1998/10/09 22:56:49  midas
  -PAA- int to DWORD *lam

  Revision 1.0  1998/4/17
  -SR- created

\********************************************************************/

#include <stdio.h>
#include "midas.h"
#include "msystem.h"
#include "mcstd.h"

/*------------------------------------------------------------------*/

HNDLE hConn = 0;
BOOL midas_connect;

/*------------------------------------------------------------------*/

int cam_init_rpc(char *host_name, char *exp_name, char *fe_name,
                 char *client_name, char *rpc_server)
{
   INT status, i, size;
   char str[256];
   HNDLE hDB, hKey, hRootKey, hSubkey;

   if (rpc_server[0]) {
      /* connect directly to RPC server, not to MIDAS experiment */
      midas_connect = FALSE;

#ifdef OS_WINNT
      {
         WSADATA WSAData;

         /* Start windows sockets */
         if (WSAStartup(MAKEWORD(1, 1), &WSAData) != 0)
            return RPC_NET_ERROR;
      }
#endif

      rpc_set_name(client_name);
      rpc_register_functions(rpc_get_internal_list(0), NULL);
      rpc_register_functions(rpc_get_internal_list(1), NULL);

      status = rpc_client_connect(rpc_server, 1750, client_name, &hConn);
      if (status != RPC_SUCCESS) {
         printf("Cannot connect to RPC server running on %s at port 1750.\n", rpc_server);
         return status;
      }
   } else {
      /* connect to MIDAS experiment */
      midas_connect = TRUE;

      /* turn off message display, turn on message logging */
      cm_set_msg_print(MT_ALL, 0, NULL);

      /* connect to experiment */
      status = cm_connect_experiment(host_name, exp_name, client_name, 0);
      if (status != CM_SUCCESS)
         return CM_UNDEF_EXP;

      /* connect to the database */
      cm_get_experiment_database(&hDB, &hKey);

      /* find CNAF server if not specified */
      strcpy(str, fe_name);
      if (fe_name[0] == '\0') {
         /* find client which exports CNAF function */
         status = db_find_key(hDB, 0, "System/Clients", &hRootKey);
         if (status == DB_SUCCESS) {
            for (i = 0, status = 0;; i++) {
               status = db_enum_key(hDB, hRootKey, i, &hSubkey);
               if (status == DB_NO_MORE_SUBKEYS) {
                  printf("No client currently exports the CNAF functionality.\n");
                  cm_disconnect_experiment();
                  return CM_UNDEF_EXP;
               }

               sprintf(str, "RPC/%d", RPC_CNAF16);
               status = db_find_key(hDB, hSubkey, str, &hKey);
               if (status == DB_SUCCESS) {
                  size = sizeof(str);
                  db_get_value(hDB, hSubkey, "Name", str, &size, TID_STRING, TRUE);
                  break;
               }
            }
         }
      }

      /* register CNAF_RPC call */
      if (cm_connect_client(str, &hConn) == CM_SUCCESS) {
         DWORD data, size, q, x;

         /* test if CNAF function implemented */
         size = sizeof(WORD);
         status =
             rpc_client_call(hConn, RPC_CNAF16, CNAF_TEST, 0, 0, 0, 0, 0, &data, &size,
                             &q, &x);

         if (status != RPC_SUCCESS) {
            printf("CNAF functionality not implemented by frontend %s\n", fe_name);
            cm_disconnect_client(hConn, FALSE);
            return CM_NO_CLIENT;
         }
      } else {
         printf("Cannot connect to frontend %s\n", fe_name);
         return CM_NO_CLIENT;
      }
   }

   return SUCCESS;
}

/*------------------------------------------------------------------*/

void cami(const int c, const int n, const int a, const int f, WORD * d)
{
   INT status, size, x, q;

   size = sizeof(WORD);
   status = rpc_client_call(hConn, RPC_CNAF16, CNAF, 0, c, n, a, f, d, &size, &x, &q);
   if (status != RPC_SUCCESS)
      printf("RPC error %d\n", status);
}

/*------------------------------------------------------------------*/

void cam16i(const int c, const int n, const int a, const int f, WORD * d)
{
   cami(c, n, a, f, d);
}

/*------------------------------------------------------------------*/

void cam24i(const int c, const int n, const int a, const int f, DWORD * d)
{
   INT status, size, x, q;

   size = sizeof(DWORD);
   status = rpc_client_call(hConn, RPC_CNAF24, CNAF, 0, c, n, a, f, d, &size, &x, &q);
   if (status != RPC_SUCCESS)
      printf("RPC error %d\n", status);
}

/*------------------------------------------------------------------*/

void cam16i_q(const int c, const int n, const int a, const int f,
              WORD * d, int *x, int *q)
{
   INT status, size;

   size = sizeof(WORD);
   status = rpc_client_call(hConn, RPC_CNAF16, CNAF, 0, c, n, a, f, d, &size, x, q);
   if (status != RPC_SUCCESS)
      printf("RPC error %d\n", status);
}

/*------------------------------------------------------------------*/

void cam24i_q(const int c, const int n, const int a, const int f,
              DWORD * d, int *x, int *q)
{
   INT status, size;

   size = sizeof(DWORD);
   status = rpc_client_call(hConn, RPC_CNAF24, CNAF, 0, c, n, a, f, d, &size, x, q);
   if (status != RPC_SUCCESS)
      printf("RPC error %d\n", status);
}

/*------------------------------------------------------------------*/

void cam16i_r(const int c, const int n, const int a, const int f, WORD ** d, const int r)
{
   INT status, size, x, q;

   size = sizeof(WORD) * r;

   status = rpc_client_call(hConn, RPC_CNAF16, CNAF, 0, c, n, a, f, *d, &size, &x, &q);

   if (status != RPC_SUCCESS)
      printf("RPC error %d\n", status);
   else
      *d = (WORD *) ((char *) (*d) + size);
}

/*------------------------------------------------------------------*/

void cam24i_r(const int c, const int n, const int a, const int f, DWORD ** d, const int r)
{
   INT status, size, x, q;

   size = sizeof(DWORD) * r;

   status = rpc_client_call(hConn, RPC_CNAF24, CNAF, 0, c, n, a, f, *d, &size, &x, &q);

   if (status != RPC_SUCCESS)
      printf("RPC error %d\n", status);
   else
      *d = (DWORD *) ((char *) (*d) + size);
}

/*------------------------------------------------------------------*/

void cam16i_rq(const int c, const int n, const int a, const int f, WORD ** d, const int r)
{
   INT status, size, x, q;

   size = sizeof(WORD) * r;
   status = rpc_client_call(hConn, RPC_CNAF16, CNAF_nQ, 0, c, n, a, f, *d, &size, &x, &q);
   if (status != RPC_SUCCESS)
      printf("RPC error %d\n", status);
   else
      *d = (WORD *) ((char *) (*d) + size);
}

/*------------------------------------------------------------------*/

void cam24i_rq(const int c, const int n, const int a, const int f,
               DWORD ** d, const int r)
{
   INT status, size, x, q;

   size = sizeof(DWORD) * r;
   status = rpc_client_call(hConn, RPC_CNAF24, CNAF_nQ, 0, c, n, a, f, *d, &size, &x, &q);
   if (status != RPC_SUCCESS)
      printf("RPC error %d\n", status);
   else
      *d = (DWORD *) ((char *) (*d) + size);
}

/*------------------------------------------------------------------*/

void cam16i_sa(const int c, const int n, const int a, const int f, WORD ** d, const int r)
{
   INT status, size, i, x, q;

   for (i = 0; i < r; i++) {
      size = sizeof(WORD);
      status =
          rpc_client_call(hConn, RPC_CNAF16, CNAF, 0, c, n, a + i, f, *d, &size, &x, &q);

      if (status != RPC_SUCCESS)
         printf("RPC error %d\n", status);
      else
         *d = (WORD *) ((char *) (*d) + size);
   }
}

/*------------------------------------------------------------------*/

void cam24i_sa(const int c, const int n, const int a, const int f,
               DWORD ** d, const int r)
{
   INT status, size, i, x, q;

   for (i = 0; i < r; i++) {
      size = sizeof(DWORD);
      status =
          rpc_client_call(hConn, RPC_CNAF24, CNAF, 0, c, n, a + i, f, *d, &size, &x, &q);

      if (status != RPC_SUCCESS)
         printf("RPC error %d\n", status);
      else
         *d = (DWORD *) ((char *) (*d) + size);
   }
}

/*------------------------------------------------------------------*/

void cam16i_sn(const int c, const int n, const int a, const int f, WORD ** d, const int r)
{
   INT status, size, i, x, q;

   for (i = 0; i < r; i++) {
      size = sizeof(WORD);
      status =
          rpc_client_call(hConn, RPC_CNAF16, CNAF, 0, c, n + i, a, f, *d, &size, &x, &q);

      if (status != RPC_SUCCESS)
         printf("RPC error %d\n", status);
      else
         *d = (WORD *) ((char *) (*d) + size);
   }
}

/*------------------------------------------------------------------*/

void cam24i_sn(const int c, const int n, const int a, const int f,
               DWORD ** d, const int r)
{
   INT status, size, i, x, q;

   for (i = 0; i < r; i++) {
      size = sizeof(DWORD);
      status =
          rpc_client_call(hConn, RPC_CNAF24, CNAF, 0, c, n + i, a, f, *d, &size, &x, &q);

      if (status != RPC_SUCCESS)
         printf("RPC error %d\n", status);
      else
         *d = (DWORD *) ((char *) (*d) + size);
   }
}

/*------------------------------------------------------------------*/

void camo(const int c, const int n, const int a, const int f, WORD d)
{
   INT status, size, x, q;

   size = sizeof(WORD);
   status = rpc_client_call(hConn, RPC_CNAF16, CNAF, 0, c, n, a, f, &d, &size, &x, &q);
   if (status != RPC_SUCCESS)
      printf("RPC error %d\n", status);
}

/*------------------------------------------------------------------*/

void cam16o(const int c, const int n, const int a, const int f, WORD d)
{
   camo(c, n, a, f, d);
}

/*------------------------------------------------------------------*/

void cam24o(const int c, const int n, const int a, const int f, DWORD d)
{
   INT status, size, x, q;

   size = sizeof(DWORD);
   status = rpc_client_call(hConn, RPC_CNAF24, CNAF, 0, c, n, a, f, &d, &size, &x, &q);
   if (status != RPC_SUCCESS)
      printf("RPC error %d\n", status);
}

/*------------------------------------------------------------------*/

void cam16o_q(const int c, const int n, const int a, const int f, WORD d, int *x, int *q)
{
   INT status, size;

   size = sizeof(WORD);
   status = rpc_client_call(hConn, RPC_CNAF16, CNAF, 0, c, n, a, f, &d, &size, x, q);
   if (status != RPC_SUCCESS)
      printf("RPC error %d\n", status);
}

/*------------------------------------------------------------------*/

void cam24o_q(const int c, const int n, const int a, const int f, DWORD d, int *x, int *q)
{
   INT status, size;

   size = sizeof(DWORD);
   status = rpc_client_call(hConn, RPC_CNAF24, CNAF, 0, c, n, a, f, &d, &size, x, q);
   if (status != RPC_SUCCESS)
      printf("RPC error %d\n", status);
}

/*------------------------------------------------------------------*/

void cam16o_r(const int c, const int n, const int a, const int f, WORD * d, const int r)
{
   INT status, size, x, q;

   size = sizeof(WORD) * r;
   status = rpc_client_call(hConn, RPC_CNAF16, CNAF, 0, c, n, a, f, d, &size, &x, &q);
   if (status != RPC_SUCCESS)
      printf("RPC error %d\n", status);
}

/*------------------------------------------------------------------*/

void cam24o_r(const int c, const int n, const int a, const int f, DWORD * d, const int r)
{
   INT status, size, x, q;

   size = sizeof(DWORD) * r;
   status = rpc_client_call(hConn, RPC_CNAF24, CNAF, 0, c, n, a, f, d, &size, &x, &q);
   if (status != RPC_SUCCESS)
      printf("RPC error %d\n", status);
}

/*------------------------------------------------------------------*/

int camc_chk(const int c)
{
   return 0;
}

void camc(const int c, const int n, const int a, const int f)
{
   INT status, size, x, q;

   size = sizeof(WORD);
   status = 0;
   status =
       rpc_client_call(hConn, RPC_CNAF16, CNAF, 0, c, n, a, f, &status, &size, &x, &q);
   if (status != RPC_SUCCESS)
      printf("RPC error %d\n", status);
}

/*------------------------------------------------------------------*/

void camc_q(const int c, const int n, const int a, const int f, int *q)
{
   INT status, size, x;

   size = sizeof(WORD);
   status = 0;
   status =
       rpc_client_call(hConn, RPC_CNAF16, CNAF, 0, c, n, a, f, &status, &size, &x, q);
   if (status != RPC_SUCCESS)
      printf("RPC error %d\n", status);
}

/*------------------------------------------------------------------*/

void camc_sa(const int c, const int n, const int a, const int f, const int r)
{
   int i;

   for (i = 0; i < r; i++)
      camc(c, n, a + i, f);
}

/*------------------------------------------------------------------*/

void camc_sn(const int c, const int n, const int a, const int f, const int r)
{
   int i;

   for (i = 0; i < r; i++)
      camc(c, n + i, a, f);
}

/*------------------------------------------------------------------*/

int cam_init(void)
{
   return 1;
}

/*------------------------------------------------------------------*/

void cam_exit(void)
{
   if (midas_connect) {
      cm_disconnect_client(hConn, FALSE);
      cm_disconnect_experiment();
   } else {
      if (hConn)
         rpc_client_disconnect(hConn, FALSE);
   }
}

/*------------------------------------------------------------------*/

void cam_inhibit_set(const int c)
{
   INT status, size, dummy;

   dummy = 0;
   size = sizeof(INT);
   status = rpc_client_call(hConn, RPC_CNAF16, CNAF_INHIBIT_SET, 0, c, 0, 0, 0,
                            &dummy, &size, &dummy, &dummy);
   if (status != RPC_SUCCESS)
      printf("RPC error %d\n", status);
}

/*------------------------------------------------------------------*/

void cam_inhibit_clear(const int c)
{
   INT status, size, dummy;

   dummy = 0;
   size = sizeof(INT);
   status = rpc_client_call(hConn, RPC_CNAF16, CNAF_INHIBIT_CLEAR, 0, c, 0, 0, 0,
                            &dummy, &size, &dummy, &dummy);
   if (status != RPC_SUCCESS)
      printf("RPC error %d\n", status);
}

/*------------------------------------------------------------------*/

void cam_crate_clear(const int c)
{
   INT status, size, dummy;

   dummy = 0;
   size = sizeof(INT);
   status = rpc_client_call(hConn, RPC_CNAF16, CNAF_CRATE_CLEAR, 0, c, 0, 0, 0,
                            &dummy, &size, &dummy, &dummy);
   if (status != RPC_SUCCESS)
      printf("RPC error %d\n", status);
}

/*------------------------------------------------------------------*/

void cam_crate_zinit(const int c)
{
   INT status, size, dummy;

   dummy = 0;
   size = sizeof(INT);
   status = rpc_client_call(hConn, RPC_CNAF16, CNAF_CRATE_ZINIT, 0, c, 0, 0, 0,
                            &dummy, &size, &dummy, &dummy);
   if (status != RPC_SUCCESS)
      printf("RPC error %d\n", status);
}

/*------------------------------------------------------------------*/

void cam_lam_enable(const int c, const int n)
{
   /* enable LAM flip-flop in unit */
   camc(c, n, 0, 26);

   /* clear LAM flip-flop in unit */
   camc(c, n, 0, 10);
}

/*------------------------------------------------------------------*/

void cam_lam_disable(const int c, const int n)
{
   /* enable LAM flip-flop in unit */
   camc(c, n, 0, 24);
}

/*------------------------------------------------------------------*/

void cam_lam_read(const int c, DWORD * lam)
{
}
void cam_lam_clear(const int c, const int n)
{
   /* clear LAM flip-flop in unit */
   camc(c, n, 0, 10);
}

/*------------------------------------------------------------------*/
void cam_interrupt_enable(const int c)
{
}
void cam_interrupt_disable(const int c)
{
}
void cam_interrupt_attach(const int c, const int n, void (*isr) (void))
{
}
void cam_interrupt_detach(const int c, const int n)
{
}

/*------------------------------------------------------------------*/
