/********************************************************************\

  Name:         bl_psi.c
  Created by:   Stefan Ritt

  Contents:     Device Driver for Urs Rohrer's beamline control at PSI
                (http://www1.psi.ch/~rohrer/secblctl.htm)

  $Log: bl_psi.c,v $
  Revision 1.2  2005/06/20 23:53:19  mucap
  (Fred)
  Extended timeout used in reading from PSI beamline server from 0.5 s to 60 s.
  The short timeout caused occasional mis-synchronizations between the
  beamline server and the MIDAS client, sometimes with results that were more
  tragic than comic (e.g. beam collimators commanded to move to negative
  openings).

  Revision 1.2  2004/01/08 08:40:08  midas
  Implemented standard indentation

  Revision 1.1  1999/12/20 10:18:19  midas
  Reorganized driver directory structure

  Revision 1.5  1999/11/15 11:20:51  midas
  Added switch on of combis

  Revision 1.4  1999/11/12 09:38:04  midas
  Added notification message upon reconnect

  Revision 1.3  1999/11/12 09:24:52  midas
  Added reconnect

  Revision 1.2  1999/11/09 15:08:44  midas
  Added trailing zero when sending commands

  Revision 1.1  1999/11/04 15:54:52  midas
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
} BL_PSI_SETTINGS;

#define BL_PSI_SETTINGS_STR "\
Frontend PC = STRING : [32] PC130\n\
Port = INT : 10000\n\
"
typedef struct {
   BL_PSI_SETTINGS bl_psi_settings;
   INT num_channels;
   char *name;
   float *demand;
   float *measured;
   INT sock;
} BL_PSI_INFO;

static DWORD last_update;
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

INT bl_psi_init(HNDLE hKey, void **pinfo, INT channels)
{
   int status, i, j, size;
   HNDLE hDB;
   char str[1024];
   BL_PSI_INFO *info;

   /* allocate info structure */
   info = calloc(1, sizeof(BL_PSI_INFO));
   *pinfo = info;

   cm_get_experiment_database(&hDB, NULL);

   /* create BL_PSI settings record */
   status = db_create_record(hDB, hKey, "", BL_PSI_SETTINGS_STR);
   if (status != DB_SUCCESS)
      return FE_ERR_ODB;

   size = sizeof(info->bl_psi_settings);
   db_get_record(hDB, hKey, &info->bl_psi_settings, &size, 0);

   /* initialize driver */
   info->num_channels = channels;
   info->name = calloc(channels, NAME_LENGTH);
   info->demand = calloc(channels, sizeof(float));
   info->measured = calloc(channels, sizeof(float));

   /* contact frontend pc */
   status = tcp_connect(info->bl_psi_settings.frontend_pc,
                        info->bl_psi_settings.port, &info->sock);
   if (status != FE_SUCCESS)
      return status;

   /* switch combis on */
   send(info->sock, "SWON", 5, 0);
   status = recv_string(info->sock, str, sizeof(str), 2000);
   if (status <= 0) {
      cm_msg(MERROR, "bl_psi", "cannot retrieve data from %s",
             info->bl_psi_settings.frontend_pc);
      return FE_ERR_HW;
   }

   /* get channel names and initial values */
   status = send(info->sock, "RALL", 5, 0);
   if (status <= 0) {
      cm_msg(MERROR, "bl_psi", "cannot retrieve data from %s",
             info->bl_psi_settings.frontend_pc);
      return FE_ERR_HW;
   }

   status = recv_string(info->sock, str, sizeof(str), 2000);
   if (status <= 0) {
      cm_msg(MERROR, "bl_psi", "cannot retrieve data from %s",
             info->bl_psi_settings.frontend_pc);
      return FE_ERR_HW;
   }

   for (i = 0; i < channels; i++) {
      recv_string(info->sock, str, sizeof(str), 500);

      for (j = 0; j < (int) strlen(str) && str[j] != ' '; j++)
         info->name[i * NAME_LENGTH + j] = str[j];
      info->name[i * NAME_LENGTH + j] = 0;

      info->demand[i] = (float) atof(str + j + 1);

      for (j++; j < (int) strlen(str) && str[j] != ' '; j++);

      /* round measured to four digits */
      info->measured[i] = (float) ((int) (atof(str + j + 1) * 10000) / 10000.0);
   }

   last_update = ss_time();

   return FE_SUCCESS;
}

/*----------------------------------------------------------------------------*/

INT bl_psi_exit(BL_PSI_INFO * info)
{
   closesocket(info->sock);

   if (info->name)
      free(info->name);

   if (info->demand)
      free(info->demand);

   if (info->measured)
      free(info->measured);

   free(info);

   return FE_SUCCESS;
}

/*----------------------------------------------------------------------------*/

INT bl_psi_set(BL_PSI_INFO * info, INT channel, float value)
{
   char str[80];
   INT status;

   sprintf(str, "WDAC %s %d", info->name + channel * NAME_LENGTH, (int) value);
   status = send(info->sock, str, strlen(str) + 1, 0);
   if (status < 0) {
      cm_msg(MERROR, "bl_psi_get", "cannot read data from %s",
             info->bl_psi_settings.frontend_pc);
      return FE_ERR_HW;
   }
   recv_string(info->sock, str, sizeof(str), 60000);

   info->demand[channel] = value;

   return FE_SUCCESS;
}

/*----------------------------------------------------------------------------*/

INT bl_psi_rall(BL_PSI_INFO * info)
{
   INT status, i, j;
   char str[1024];

   if (ss_time() - last_update > 10) {
      last_update = ss_time();

      status = send(info->sock, "RALL", 5, 0);
      if (status <= 0) {
         if (info->sock > 0) {
            closesocket(info->sock);
            info->sock = -1;
         }

         /* try to reconnect every 10 minutes */
         if (ss_time() - last_reconnect > 600) {
            last_reconnect = ss_time();
            status = tcp_connect(info->bl_psi_settings.frontend_pc,
                                 info->bl_psi_settings.port, &info->sock);

            if (status != FE_SUCCESS)
               return FE_ERR_HW;
            else
               cm_msg(MINFO, "bl_psi_rall", "sucessfully reconneccted to %s",
                      info->bl_psi_settings.frontend_pc);
         } else
            return FE_SUCCESS;
      }

      status = recv_string(info->sock, str, sizeof(str), 5000);
      if (status <= 0) {
         cm_msg(MERROR, "bl_psi_rall", "cannot retrieve data from %s",
                info->bl_psi_settings.frontend_pc);
         return FE_ERR_HW;
      }

      for (i = 0; i < info->num_channels; i++) {
         recv_string(info->sock, str, sizeof(str), 1000);

         /* skip name */
         for (j = 0; j < (int) strlen(str) && str[j] != ' '; j++)

            /* extract demand value */
            info->demand[i] = (float) atof(str + j + 1);
         for (j++; j < (int) strlen(str) && str[j] != ' '; j++);

         /* round measured to four digits */
         info->measured[i] = (float) ((int) (atof(str + j + 1) * 10000) / 10000.0);
      }
   }

   return FE_SUCCESS;
}

/*----------------------------------------------------------------------------*/

INT bl_psi_get(BL_PSI_INFO * info, INT channel, float *pvalue)
{
   INT status;

   status = bl_psi_rall(info);
   *pvalue = info->measured[channel];

   return status;
}

/*----------------------------------------------------------------------------*/

INT bl_psi_get_demand(BL_PSI_INFO * info, INT channel, float *pvalue)
{
   INT status;

   status = bl_psi_rall(info);
   *pvalue = info->demand[channel];

   return status;
}

/*----------------------------------------------------------------------------*/

INT bl_psi_get_default_threshold(BL_PSI_INFO * info, INT channel, float *pvalue)
{
   /* threshold is 2 bit in 12 bits */
   *pvalue = 0.0005f;
   return FE_SUCCESS;
}

/*----------------------------------------------------------------------------*/

INT bl_psi_get_default_name(BL_PSI_INFO * info, INT channel, char *name)
{
   strcpy(name, info->name + channel * NAME_LENGTH);
   return FE_SUCCESS;
}

/*---- device driver entry point -----------------------------------*/

INT bl_psi(INT cmd, ...)
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
      status = bl_psi_init(hKey, info, channel);
      break;

   case CMD_EXIT:
      info = va_arg(argptr, void *);
      status = bl_psi_exit(info);
      break;

   case CMD_SET:
      info = va_arg(argptr, void *);
      channel = va_arg(argptr, INT);
      value = (float) va_arg(argptr, double);
      status = bl_psi_set(info, channel, value);
      break;

   case CMD_GET:
      info = va_arg(argptr, void *);
      channel = va_arg(argptr, INT);
      pvalue = va_arg(argptr, float *);
      status = bl_psi_get(info, channel, pvalue);
      break;

   case CMD_GET_DEMAND:
      info = va_arg(argptr, void *);
      channel = va_arg(argptr, INT);
      pvalue = va_arg(argptr, float *);
      status = bl_psi_get_demand(info, channel, pvalue);
      break;

   case CMD_GET_DEFAULT_NAME:
      info = va_arg(argptr, void *);
      channel = va_arg(argptr, INT);
      name = va_arg(argptr, char *);
      status = bl_psi_get_default_name(info, channel, name);
      break;

   case CMD_GET_DEFAULT_THRESHOLD:
      info = va_arg(argptr, void *);
      channel = va_arg(argptr, INT);
      pvalue = va_arg(argptr, float *);
      status = bl_psi_get_default_threshold(info, channel, pvalue);
      break;

   default:
      break;
   }

   va_end(argptr);

   return status;
}

/*------------------------------------------------------------------*/
