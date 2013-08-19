/********************************************************************\

  Name:         tcpip.c
  Created by:   Stefan Ritt

  Contents:     TCP/IP socket communication routines

  $Log: tcpip.c,v $
  Revision 1.1.1.1  2005/06/20 23:37:07  mucap
  Importing release 1.9.5 of the MIDAS source code as distributed by PSI.
  (Next, I'll commit our local customizations.)

  Revision 1.6  2004/01/08 08:40:08  midas
  Implemented standard indentation

  Revision 1.5  2001/04/06 03:58:34  midas
  Moved debugging flag into ODB for BD

  Revision 1.4  2001/04/05 05:53:34  midas
  Added CMD_NAME

  Revision 1.3  2001/04/03 12:26:50  midas
  Improved logging

  Revision 1.2  2001/03/01 09:36:25  midas
  Put <TIMOUT> into debug messages

  Revision 1.1  2001/02/26 13:58:06  midas
  Added files

\********************************************************************/

#include "midas.h"
#include "msystem.h"

static int debug_last = 0, debug_first = TRUE;

typedef struct {
   char host[256];
   int port;
   int debug;
} TCPIP_SETTINGS;

#define TCPIP_SETTINGS_STR "\
Host = STRING : [256] myhost.my.domain\n\
Port = INT : 23\n\
Debug = INT : 0\n\
"

typedef struct {
   TCPIP_SETTINGS settings;
   int fd;                      /* device handle for socket device */
} TCPIP_INFO;

/*----------------------------------------------------------------------------*/

void tcpip_debug(TCPIP_INFO * info, char *dbg_str)
{
   FILE *f;
   int delta;

   if (debug_last == 0)
      delta = 0;
   else
      delta = ss_millitime() - debug_last;
   debug_last = ss_millitime();

   f = fopen("tcpip.log", "a");

   if (debug_first)
      fprintf(f, "\n==== new session =============\n\n");
   debug_first = FALSE;
   fprintf(f, "{%d} %s\n", delta, dbg_str);

   if (info->settings.debug > 1)
      printf("{%d} %s\n", delta, dbg_str);

   fclose(f);
}

/*------------------------------------------------------------------*/

int tcpip_open(char *host, int port)
{
   struct sockaddr_in bind_addr;
   struct hostent *phe;
   int status, fd;

#ifdef OS_WINNT
   {
      WSADATA WSAData;

      /* Start windows sockets */
      if (WSAStartup(MAKEWORD(1, 1), &WSAData) != 0)
         return RPC_NET_ERROR;
   }
#endif

   /* create a new socket for connecting to remote server */
   fd = socket(AF_INET, SOCK_STREAM, 0);
   if (fd == -1) {
      perror("tcpip_open: socket");
      return fd;
   }

   /* let OS choose any port number */
   memset(&bind_addr, 0, sizeof(bind_addr));
   bind_addr.sin_family = AF_INET;
   bind_addr.sin_addr.s_addr = 0;
   bind_addr.sin_port = 0;

   status = bind(fd, (void *) &bind_addr, sizeof(bind_addr));
   if (status < 0) {
      perror("tcpip_open:bind");
      return fd;
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
      printf("\ntcpip_open: unknown host name %s\n", host);
      return fd;
   }
   memcpy((char *) &(bind_addr.sin_addr), phe->h_addr, phe->h_length);
#endif

#ifdef OS_UNIX
   do {
      status = connect(fd, (void *) &bind_addr, sizeof(bind_addr));

      /* don't return if an alarm signal was cought */
   } while (status == -1 && errno == EINTR);
#else
   status = connect(fd, (void *) &bind_addr, sizeof(bind_addr));
#endif

   if (status != 0) {
      perror("tcpip_open:connect");
      return -1;
   }

   return fd;
}

/*----------------------------------------------------------------------------*/

int tcpip_exit(TCPIP_INFO * info)
{
   closesocket(info->fd);

   return SUCCESS;
}

/*----------------------------------------------------------------------------*/

int tcpip_write(TCPIP_INFO * info, char *data, int size)
{
   int i;

   if (info->settings.debug) {
      char dbg_str[256];

      sprintf(dbg_str, "write: ");
      for (i = 0; (int) i < size; i++)
         sprintf(dbg_str + strlen(dbg_str), "%X ", data[i]);

      tcpip_debug(info, dbg_str);
   }

   i = send(info->fd, data, size, 0);

   return i;
}

/*----------------------------------------------------------------------------*/

int tcpip_read(TCPIP_INFO * info, char *data, int size, int millisec)
{
   fd_set readfds;
   struct timeval timeout;
   int i, status, n;

   n = 0;
   memset(data, 0, size);

   do {
      if (millisec > 0) {
         FD_ZERO(&readfds);
         FD_SET(info->fd, &readfds);

         timeout.tv_sec = millisec / 1000;
         timeout.tv_usec = (millisec % 1000) * 1000;

         do {
            status = select(FD_SETSIZE, (void *) &readfds, NULL, NULL, (void *) &timeout);

            /* if an alarm signal was cought, restart select with reduced timeout */
            if (status == -1 && timeout.tv_sec >= WATCHDOG_INTERVAL / 1000)
               timeout.tv_sec -= WATCHDOG_INTERVAL / 1000;

         } while (status == -1);        /* dont return if an alarm signal was cought */

         if (!FD_ISSET(info->fd, &readfds))
            break;
      }

      i = recv(info->fd, data + n, 1, 0);

      if (i <= 0)
         break;

      n++;

      if (n >= size)
         break;

   } while (1);                 /* while (buffer[n-1] && buffer[n-1] != 10); */

   if (info->settings.debug) {
      char dbg_str[256];

      sprintf(dbg_str, "read: ");

      if (n == 0)
         sprintf(dbg_str + strlen(dbg_str), "<TIMEOUT>");
      else
         for (i = 0; i < n; i++)
            sprintf(dbg_str + strlen(dbg_str), "%X ", data[i]);

      tcpip_debug(info, dbg_str);
   }

   return n;
}

/*----------------------------------------------------------------------------*/

int tcpip_puts(TCPIP_INFO * info, char *str)
{
   int i;

   if (info->settings.debug) {
      char dbg_str[256];

      sprintf(dbg_str, "puts: %s", str);
      tcpip_debug(info, dbg_str);
   }

   i = send(info->fd, str, strlen(str), 0);
   if (i < 0)
      perror("tcpip_puts");

   return i;
}

/*----------------------------------------------------------------------------*/

int tcpip_gets(TCPIP_INFO * info, char *str, int size, char *pattern, int millisec)
{
   fd_set readfds;
   struct timeval timeout;
   int i, status, n;

   n = 0;
   memset(str, 0, size);

   do {
      if (millisec > 0) {
         FD_ZERO(&readfds);
         FD_SET(info->fd, &readfds);

         timeout.tv_sec = millisec / 1000;
         timeout.tv_usec = (millisec % 1000) * 1000;

         do {
            status = select(FD_SETSIZE, (void *) &readfds, NULL, NULL, (void *) &timeout);

            /* if an alarm signal was cought, restart select with reduced timeout */
            if (status == -1 && timeout.tv_sec >= WATCHDOG_INTERVAL / 1000)
               timeout.tv_sec -= WATCHDOG_INTERVAL / 1000;

         } while (status == -1);        /* dont return if an alarm signal was cought */

         if (!FD_ISSET(info->fd, &readfds))
            break;
      }

      i = recv(info->fd, str + n, 1, 0);

      if (i <= 0)
         break;

      n += i;

      if (pattern && pattern[0])
         if (strstr(str, pattern) != NULL)
            break;

      if (n >= size)
         break;

   } while (1);                 /* while (buffer[n-1] && buffer[n-1] != 10); */

   if (info->settings.debug) {
      char dbg_str[256];

      sprintf(dbg_str, "gets [%s]: ", pattern);

      if (str[0] == 0)
         sprintf(dbg_str + strlen(dbg_str), "<TIMEOUT>");
      else
         sprintf(dbg_str + strlen(dbg_str), "%s", str);

      tcpip_debug(info, dbg_str);
   }

   return n;
}

/*----------------------------------------------------------------------------*/

int tcpip_init(HNDLE hkey, void **pinfo)
{
   HNDLE hDB, hkeybd;
   INT size, status;
   TCPIP_INFO *info;

   /* allocate info structure */
   info = calloc(1, sizeof(TCPIP_INFO));
   *pinfo = info;

   cm_get_experiment_database(&hDB, NULL);

   /* create TCPIP settings record */
   status = db_create_record(hDB, hkey, "BD", TCPIP_SETTINGS_STR);
   if (status != DB_SUCCESS)
      return FE_ERR_ODB;

   db_find_key(hDB, hkey, "BD", &hkeybd);
   size = sizeof(info->settings);
   db_get_record(hDB, hkeybd, &info->settings, &size, 0);

   /* open port */
   info->fd = tcpip_open(info->settings.host, info->settings.port);
   if (info->fd < 0)
      return FE_ERR_HW;

   return SUCCESS;
}

/*----------------------------------------------------------------------------*/

INT tcpip(INT cmd, ...)
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
      status = tcpip_init(hkey, info);
      break;

   case CMD_EXIT:
      info = va_arg(argptr, void *);
      status = tcpip_exit(info);
      break;

   case CMD_NAME:
      info = va_arg(argptr, void *);
      str = va_arg(argptr, char *);
      strcpy(str, "tcpip");
      break;

   case CMD_WRITE:
      info = va_arg(argptr, void *);
      str = va_arg(argptr, char *);
      size = va_arg(argptr, int);
      status = tcpip_write(info, str, size);
      break;

   case CMD_READ:
      info = va_arg(argptr, void *);
      str = va_arg(argptr, char *);
      size = va_arg(argptr, INT);
      timeout = va_arg(argptr, INT);
      status = tcpip_read(info, str, size, timeout);
      break;

   case CMD_PUTS:
      info = va_arg(argptr, void *);
      str = va_arg(argptr, char *);
      status = tcpip_puts(info, str);
      break;

   case CMD_GETS:
      info = va_arg(argptr, void *);
      str = va_arg(argptr, char *);
      size = va_arg(argptr, INT);
      pattern = va_arg(argptr, char *);
      timeout = va_arg(argptr, INT);
      status = tcpip_gets(info, str, size, pattern, timeout);
      break;

   case CMD_DEBUG:
      info = va_arg(argptr, void *);
      status = va_arg(argptr, INT);
      ((TCPIP_INFO *) info)->settings.debug = status;
      break;
   }

   va_end(argptr);

   return status;
}
