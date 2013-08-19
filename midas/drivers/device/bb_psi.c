/********************************************************************\

  Name:         bb_psi.c
  Created by:   Stefan Ritt

  Contents:     Device Driver for PSI beam blocker through 
                Urs Rohrer's beamline control
                (http://www1.psi.ch/~rohrer/secblctl.htm)

  $Log: bb_psi.c,v $
  Revision 1.1.1.1  2005/06/20 23:37:09  mucap
  Importing release 1.9.5 of the MIDAS source code as distributed by PSI.
  (Next, I'll commit our local customizations.)

  Revision 1.2  2004/01/08 08:40:08  midas
  Implemented standard indentation

  Revision 1.1  1999/12/20 10:18:19  midas
  Reorganized driver directory structure

  Revision 1.5  1999/11/12 09:38:04  midas
  Added notification message upon reconnect

  Revision 1.4  1999/11/12 09:24:52  midas
  Added reconnect

  Revision 1.3  1999/11/11 11:50:52  midas
  Added default name and update threshold

  Revision 1.2  1999/11/11 10:41:44  midas
  Fixed compiler warning

  Revision 1.1  1999/11/11 10:30:02  midas
  Added files

\********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "midas.h"
#include "msystem.h"

/*---- globals -----------------------------------------------------*/

typedef struct {
   char frontend_pc[32];
   int port;
   int ncom;
   int acom;
   int com_open;
   int com_close;
   int nstat;
   int astat;
   int stat_open;
   int stat_closed;
} BB_PSI_SETTINGS;

/*
Device Server NCom  ACom ComOpen ComClose NStat AStat StatOpen StatClosed StatPSA  
KSG51  PC130  15    1    1       2        16    0     1        2          4
*/

#define BB_PSI_SETTINGS_STR "\
Frontend PC = STRING : [32] PC130\n\
Port = INT : 10000\n\
NCom = INT : 15\n\
ACom = INT : 1\n\
Com Open = INT : 1\n\
Com Close = INT : 2\n\
NStat = INT : 16\n\
AStat = INT : 0\n\
Stat Open = INT : 1\n\
Stat Closed = INT : 2\n\
"

typedef struct {
   BB_PSI_SETTINGS bb_psi_settings;
   float open;
   INT sock;
} BB_PSI_INFO;

INT bb_psi_get(BB_PSI_INFO * info, INT channel, float *pvalue);

static DWORD last_reconnect;

/*---- network TCP connection --------------------------------------*/

static INT tcp_connect(char *host, int port, int *sock)
{
   struct sockaddr_in bind_addr;
   struct hostent *phe;
   int status;

#ifdef OS_WINNT
   {
      WSADATA WSAData;

      /* Start windows sockets */
      if (WSAStartup(MAKEWORD(1, 1), &WSAData) != 0)
         return RPC_NET_ERROR;
   }
#endif

   /* create a new socket for connecting to remote server */
   *sock = socket(AF_INET, SOCK_STREAM, 0);
   if (*sock == -1) {
      cm_msg(MERROR, "tcp_connect", "cannot create socket");
      return FE_ERR_HW;
   }

   /* let OS choose any port number */
   memset(&bind_addr, 0, sizeof(bind_addr));
   bind_addr.sin_family = AF_INET;
   bind_addr.sin_addr.s_addr = 0;
   bind_addr.sin_port = 0;

   status = bind(*sock, (void *) &bind_addr, sizeof(bind_addr));
   if (status < 0) {
      cm_msg(MERROR, "tcp_connect", "cannot bind");
      return RPC_NET_ERROR;
   }

   /* connect to remote node */
   memset(&bind_addr, 0, sizeof(bind_addr));
   bind_addr.sin_family = AF_INET;
   bind_addr.sin_addr.s_addr = 0;
   bind_addr.sin_port = htons((short) port);

#ifdef OS_VXWORKS
   {
      INT host_addr;

      host_addr = hostGetByName(host);
      memcpy((char *) &(bind_addr.sin_addr), &host_addr, 4);
   }
#else
   phe = gethostbyname(host);
   if (phe == NULL) {
      cm_msg(MERROR, "tcp_connect", "cannot get host name");
      return RPC_NET_ERROR;
   }
   memcpy((char *) &(bind_addr.sin_addr), phe->h_addr, phe->h_length);
#endif

#ifdef OS_UNIX
   do {
      status = connect(*sock, (void *) &bind_addr, sizeof(bind_addr));

      /* don't return if an alarm signal was cought */
   } while (status == -1 && errno == EINTR);
#else
   status = connect(*sock, (void *) &bind_addr, sizeof(bind_addr));
#endif

   if (status != 0) {
      closesocket(*sock);
      *sock = -1;
      cm_msg(MERROR, "tcp_connect", "cannot connect to %s", host);
      return FE_ERR_HW;
   }

   return FE_SUCCESS;
}

/*---- device driver routines --------------------------------------*/

INT bb_psi_init(HNDLE hKey, void **pinfo, INT channels)
{
   int status, size;
   HNDLE hDB;
   BB_PSI_INFO *info;
   float value;

   /* only allow single channel */
   if (channels != 1) {
      cm_msg(MERROR, "bb_psi_init", "Only single channel possible for beam blocker");
      return FE_ERR_HW;
   }

   /* allocate info structure */
   info = calloc(1, sizeof(BB_PSI_INFO));
   *pinfo = info;

   cm_get_experiment_database(&hDB, NULL);

   /* create BB_PSI settings record */
   status = db_create_record(hDB, hKey, "", BB_PSI_SETTINGS_STR);
   if (status != DB_SUCCESS)
      return FE_ERR_ODB;

   size = sizeof(info->bb_psi_settings);
   db_get_record(hDB, hKey, &info->bb_psi_settings, &size, 0);

   /* contact frontend pc */
   status = tcp_connect(info->bb_psi_settings.frontend_pc,
                        info->bb_psi_settings.port, &info->sock);
   if (status != FE_SUCCESS)
      return status;

   /* get initial state */
   bb_psi_get(info, 0, &value);

   return FE_SUCCESS;
}

/*------------------------------------------------------------------*/

INT bb_psi_exit(BB_PSI_INFO * info)
{
   closesocket(info->sock);

   free(info);

   return FE_SUCCESS;
}

/*------------------------------------------------------------------*/

static DWORD last_action;
static int last_demand;

INT bb_psi_set(BB_PSI_INFO * info, INT channel, float value)
{
   char str[80];
   INT status;

   sprintf(str, "WCAM %d %d 16 %d", info->bb_psi_settings.ncom,
           info->bb_psi_settings.acom,
           ((int) value) ==
           1 ? info->bb_psi_settings.com_open : info->bb_psi_settings.com_close);

   status = send(info->sock, str, strlen(str) + 1, 0);
   if (status < 0) {
      cm_msg(MERROR, "bb_psi_get", "cannot read data from %s",
             info->bb_psi_settings.frontend_pc);
      return FE_ERR_HW;
   }
   recv_string(info->sock, str, sizeof(str), 500);

   last_demand = (int) value;
   last_action = ss_time();

   return FE_SUCCESS;
}

/*------------------------------------------------------------------*/

INT bb_psi_get(BB_PSI_INFO * info, INT channel, float *pvalue)
{
   INT status, i;
   char str[80];
   static DWORD last_update;

   /* update every minute or every 5 seconds if blocker has been moved not
      more than one minute ago */
   if (ss_time() - last_update > 60 ||
       (ss_time() - last_action < 60 && ss_time() - last_update > 5)) {
      last_update = ss_time();

      sprintf(str, "RCAM %d %d 0", info->bb_psi_settings.nstat,
              info->bb_psi_settings.astat);
      status = send(info->sock, str, strlen(str) + 1, 0);
      if (status <= 0) {
         if (info->sock > 0) {
            closesocket(info->sock);
            info->sock = -1;
         }

         /* try to reconnect every 10 minutes */
         if (ss_time() - last_reconnect > 600) {
            last_reconnect = ss_time();
            status = tcp_connect(info->bb_psi_settings.frontend_pc,
                                 info->bb_psi_settings.port, &info->sock);

            if (status != FE_SUCCESS)
               return FE_ERR_HW;
            else
               cm_msg(MINFO, "bb_psi_get", "sucessfully reconneccted to %s",
                      info->bb_psi_settings.frontend_pc);
         } else
            return FE_SUCCESS;
      }

      status = recv_string(info->sock, str, sizeof(str), 3000);
      if (status <= 0) {
         cm_msg(MERROR, "bb_psi_get", "cannot retrieve data from %s",
                info->bb_psi_settings.frontend_pc);
         return FE_ERR_HW;
      }

      /* skip *RCAM* name */
      for (i = 0; i < (int) strlen(str) && str[i] != ' '; i++)
         i++;

      /* skip X&Q */
      for (i++; i < (int) strlen(str) && str[i] != ' '; i++);
      i++;

      /* round measured to four digits */
      status = atoi(str + i);

      if ((status & info->bb_psi_settings.stat_open) > 0)
         info->open = 1.f;
      else if ((status & info->bb_psi_settings.stat_closed) > 0)
         info->open = 0.f;
      else
         info->open = 0.5f;

      *pvalue = (float) info->open;
   }

   return FE_SUCCESS;
}

/*------------------------------------------------------------------*/

INT bb_psi_get_demand(BB_PSI_INFO * info, INT channel, float *pvalue)
{
   if (ss_time() - last_action < 60)
      *pvalue = (float) last_demand;
   else
      *pvalue = info->open;

   return FE_SUCCESS;
}

/*---- device driver entry point -----------------------------------*/

INT bb_psi(INT cmd, ...)
{
   va_list argptr;
   HNDLE hKey;
   INT channel, status;
   float value, *pvalue;
   void *info;
   char *name;

   va_start(argptr, cmd);
   status = FE_SUCCESS;

   switch (cmd) {
   case CMD_INIT:
      hKey = va_arg(argptr, HNDLE);
      info = va_arg(argptr, void *);
      channel = va_arg(argptr, INT);
      status = bb_psi_init(hKey, info, channel);
      break;

   case CMD_EXIT:
      info = va_arg(argptr, void *);
      status = bb_psi_exit(info);
      break;

   case CMD_SET:
      info = va_arg(argptr, void *);
      channel = va_arg(argptr, INT);
      value = (float) va_arg(argptr, double);
      status = bb_psi_set(info, channel, value);
      break;

   case CMD_GET:
      info = va_arg(argptr, void *);
      channel = va_arg(argptr, INT);
      pvalue = va_arg(argptr, float *);
      status = bb_psi_get(info, channel, pvalue);
      break;

   case CMD_GET_DEMAND:
      info = va_arg(argptr, void *);
      channel = va_arg(argptr, INT);
      pvalue = va_arg(argptr, float *);
      status = bb_psi_get_demand(info, channel, pvalue);
      break;

   case CMD_GET_DEFAULT_NAME:
      info = va_arg(argptr, void *);
      channel = va_arg(argptr, INT);
      name = va_arg(argptr, char *);
      strcpy(name, "BBlocker");
      status = FE_SUCCESS;
      break;

   case CMD_GET_DEFAULT_THRESHOLD:
      info = va_arg(argptr, void *);
      channel = va_arg(argptr, INT);
      pvalue = va_arg(argptr, float *);
      *pvalue = 0.1f;
      status = FE_SUCCESS;
      break;

   default:
      break;
   }

   va_end(argptr);

   return status;
}

/*------------------------------------------------------------------*/
