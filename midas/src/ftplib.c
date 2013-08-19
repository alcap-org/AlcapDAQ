/********************************************************************\

  Name:         ftplib.c
  Created by:   Originally written by Oleg Orel (orel@lpuds.oea.ihep.su),
                translated from UNIX to NT by Urs Rohrer (rohrer@psi.ch),
                simplified and adapted for MIDAS by Stefan Ritt.

  Contents:     File Transfer Protocol library

  **** muCap patches ***
   - expanded string that holds output from server, since the PSI archive
     has a very long greeting banner
   - changes to extract numerical status properly even when there are
     continuation lines in the server output

  $Log: ftplib.c,v $
  Revision 1.2  2005/06/21 00:18:40  mucap
  (Fred)
     - expanded string that holds output from server, since the PSI archive
       has a very long greeting banner
     - changes to extract numerical status properly even when there are
       continuation lines in the server output

  Revision 1.6  2004/01/08 08:40:09  midas
  Implemented standard indentation

  Revision 1.5  2003/04/14 13:01:59  midas
  Fixed compiler warning

  Revision 1.4  2003/04/09 13:42:40  midas
  Made file compile under C++

  Revision 1.3  1999/02/12 11:52:13  midas
  Fixed bug that i was not initialized in ftp_get

  Revision 1.2  1998/10/12 12:19:01  midas
  Added Log tag in header


\********************************************************************/

#include "midas.h"
#include "msystem.h"
#include "ftplib.h"

#ifndef FTP_SUCCESS
#define FTP_SUCCESS                   1
#define FTP_NET_ERROR               802
#define FTP_FILE_ERROR              803
#define FTP_RESPONSE_ERROR          804
#define FTP_INVALID_ARG             805
#endif

static char bars[] = "/-\\|";
int (*ftp_debug_func) (char *message);
int (*ftp_error_func) (char *message);

/*------------------------------------------------------------------*/

void ftp_debug(int (*debug_func) (char *message), int (*error_func) (char *message))
/* set message display functions for debug and error messages */
{
   ftp_debug_func = debug_func;
   ftp_error_func = error_func;
}

/*------------------------------------------------------------------*/

int ftp_connect(FTP_CON ** con, char *host_name, unsigned short port)
/* Connect to a FTP server on a host at a given port (usually 21).
   Return a FTP_CON structure if successful */
{
   struct sockaddr_in bind_addr;
   int sock;
   char str[4000];
   int status;
   struct hostent *phe;

   *con = NULL;

#ifdef OS_WINNT
   {
      WSADATA WSAData;

      /* Start windows sockets */
      if (WSAStartup(MAKEWORD(1, 1), &WSAData) != 0)
         return FTP_NET_ERROR;
   }
#endif

   if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
      if (ftp_error_func)
         ftp_error_func("cannot create socket");
      return FTP_NET_ERROR;
   }

   /* connect to remote node */
   memset(&bind_addr, 0, sizeof(bind_addr));
   bind_addr.sin_family = AF_INET;
   bind_addr.sin_addr.s_addr = 0;
   bind_addr.sin_port = htons(port);

#ifdef OS_VXWORKS
   {
      INT host_addr;

      host_addr = hostGetByName(host_name);
      memcpy((char *) &(bind_addr.sin_addr), &host_addr, 4);
   }
#else
   phe = gethostbyname(host_name);
   if (phe == NULL) {
      if (ftp_error_func)
         ftp_error_func("cannot get host name");
      return RPC_NET_ERROR;
   }
   memcpy((char *) &(bind_addr.sin_addr), phe->h_addr, phe->h_length);
#endif

#ifdef OS_UNIX
   do {
      status = connect(sock, (struct sockaddr *) &bind_addr, sizeof(bind_addr));

      /* don't return if an alarm signal was cought */
   } while (status == -1 && errno == EINTR);
#else
   status = connect(sock, (struct sockaddr *) &bind_addr, sizeof(bind_addr));
#endif

   if (status != 0) {
      sprintf(str, "cannot connect to host %s, port %d", host_name, port);
      if (ftp_error_func)
         ftp_error_func(str);
      return FTP_NET_ERROR;
   }


   *con = (FTP_CON *) malloc(sizeof(FTP_CON));
   (*con)->sock = sock;
   (*con)->data = 0;

   memset(str, 0, sizeof(str));
   status = ftp_get_message(*con, str);

   /* check for status */
   if (status == QUIT || !ftp_good(status, 120, 220, EOF)) {
      closesocket(sock);
      free(*con);
      return FTP_NET_ERROR;
   }

   return FTP_SUCCESS;
}

/*------------------------------------------------------------------*/

int ftp_send_message(FTP_CON * con, char *message)
/* send a message to a FTP server */
{
   if (send(con->sock, message, strlen(message), 0) == -1)
      return FTP_NET_ERROR;

   if (send(con->sock, "\r\n", 2, 0) == -1)
      return FTP_NET_ERROR;

   if (ftp_debug_func != NULL)
      ftp_debug_func(message);

   return FTP_SUCCESS;
}

/*------------------------------------------------------------------*/

int ftp_command(FTP_CON * con, char *command, char *param, ...)
/* execute FTP command, check for return codes */
{
   va_list args;
   char str[2048];
   int status, code;
   BOOL result_ok;

   /* compose command string */
   sprintf(str, command, param);

   /* send command */
   if (ftp_send_message(con, str) != FTP_SUCCESS)
      return FTP_NET_ERROR;

   /* read reply */
   status = ftp_get_message(con, str);
   if (status == QUIT)
      return FTP_NET_ERROR;

   /* check reply code */
   va_start(args, param);

   result_ok = FALSE;
   do {
      code = va_arg(args, int);
      if (code == EOF)
         break;

      if (code == status)
         result_ok = TRUE;

   } while (!result_ok);

   va_end(args);

   if (!result_ok) {
      if (ftp_error_func != NULL)
         ftp_error_func(str);

      return FTP_RESPONSE_ERROR;
   }

   return -status;
}

/*------------------------------------------------------------------*/

int ftp_get_message(FTP_CON * con, char *message)
/* read message from FTP server */
{
   int i, err_no;

   for (i = 0;; i++) {
      if (recv(con->sock, &message[i], 1, 0) != 1)
         return QUIT;

      if (i > 1 && message[i] == 10 && message[i - 1] == 13)
         break;
   }

   message[i - 1] = 0;

   err_no = atoi(message);
   if(err_no != 0) {
     con->err_no = err_no;
  }

   if (ftp_debug_func != NULL)
      ftp_debug_func(message);

   /* check for continuation message */
   if (message[3] == '-' || err_no == 0)
      ftp_get_message(con, message + strlen(message));

   return con->err_no;
}

/*------------------------------------------------------------------*/

BOOL ftp_good(int number, ...)
/* check if number matches any code from argument list */
{
   va_list args;
   BOOL result;
   int code;

   va_start(args, number);
   result = FALSE;

   do {
      code = va_arg(args, int);
      if (code == EOF)
         break;

      if (code == number)
         result = TRUE;

   } while (!result);

   va_end(args);

   return result;
}

/*------------------------------------------------------------------*/

int ftp_data(FTP_CON * con, char *command, char *file)
/* open data socket */
{
   struct sockaddr_in data, from;
   struct hostent *host;
   char host_name[256];
   int listen_socket, data_socket;
   int len = sizeof(data), one = 1, fromlen = sizeof(from), status;
   char *a, *b;

   memset(&data, 0, sizeof(data));
   memset(&from, 0, sizeof(from));

   if (gethostname(host_name, sizeof(host_name)) == -1)
      return FTP_NET_ERROR;

   if ((host = (struct hostent *) gethostbyname(host_name)) == 0)
      return FTP_NET_ERROR;

   if ((listen_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1)
      return FTP_NET_ERROR;

   if (setsockopt(listen_socket, SOL_SOCKET, SO_REUSEADDR,
                  (char *) &one, sizeof(one)) < 0) {
      closesocket(listen_socket);
      return FTP_NET_ERROR;
   }

   data.sin_family = AF_INET;
   data.sin_port = htons(0);
   data.sin_addr.s_addr = *(unsigned long *) *(host->h_addr_list);

   if (bind(listen_socket, (struct sockaddr *) &data, sizeof(data)) == -1) {
      closesocket(listen_socket);
      return FTP_NET_ERROR;
   }

   if (getsockname(listen_socket, (struct sockaddr *) &data, &len) < 0) {
      closesocket(listen_socket);
      return FTP_NET_ERROR;
   }

   if (listen(listen_socket, 1) != 0) {
      closesocket(listen_socket);
      return FTP_NET_ERROR;
   }

   a = (char *) &data.sin_addr;
   b = (char *) &data.sin_port;

   status = ftp_port(con, CUT(a[0]), CUT(a[1]), CUT(a[2]),
                     CUT(a[3]), CUT(b[0]), CUT(b[1]));
   if (status != FTP_SUCCESS)
      return FTP_NET_ERROR;

   status = ftp_command(con, command, file, 200, 120, 150, 125, 250, EOF);
   if (status >= 0)
      return status;

   data_socket = accept(listen_socket, (struct sockaddr *) &from, &fromlen);

   if (data_socket == -1) {
      closesocket(listen_socket);
      return FTP_NET_ERROR;
   }

   closesocket(listen_socket);

   con->data = data_socket;

   return status;
}

/*------------------------------------------------------------------*/

int ftp_close(FTP_CON * con)
/* close data connection */
{
   char str[256];

   closesocket(con->data);

   return ftp_get_message(con, str);
}

/*------------------------------------------------------------------*/

int ftp_send(int sock, char *buffer, int n_bytes_to_write)
/* send number of bytes over socket */
{
   int n_bytes_left, n_written;

   n_bytes_left = n_bytes_to_write;

   while (n_bytes_left > 0) {
      n_written = send(sock, buffer, n_bytes_left > 8192 ? 8192 : n_bytes_left, 0);
      if (n_written <= 0)
         return n_bytes_to_write - n_bytes_left;

      n_bytes_left -= n_written;
      buffer += n_written;
   }

   return n_bytes_to_write;
}

/*------------------------------------------------------------------*/

int ftp_receive(int sock, char *buffer, int bsize)
/* receive buffer from socket, return number of received bytes */
{
   int count, i;

   i = 0;
   while (TRUE) {
      count = recv(sock, buffer + i, bsize - 1 - i, 0);
      if (count <= 0)
         return i;

      i += count;
      buffer[i] = 0;

      if (i >= bsize - 1)
         return i;
   }
}

/*------------------------------------------------------------------*/

int ftp_login(FTP_CON ** con, char *host, unsigned short port,
              char *user, char *password, char *account)
/* FTP login with username and password */
{
   int status;

   status = ftp_connect(con, host, port);
   if (status != FTP_SUCCESS)
      return status;

   status = ftp_user(*con, user);
   if (status >= 0)
      return status;

   if (status == -230)
      return status;

   if (status == -332) {
      if (account == NULL)
         return FTP_NET_ERROR;

      status = ftp_account(*con, account);
      if (status < 1)
         return status;

      if (status == -230)
         return status;
   }

   return ftp_password(*con, password);
}

/*------------------------------------------------------------------*/

int ftp_bye(FTP_CON * con)
/* disconnect from FTP server */
{
   char str[256];

   ftp_send_message(con, "QUIT");
   ftp_get_message(con, str);

   closesocket(con->sock);
   free(con);

   return FTP_SUCCESS;
}

/*------------------------------------------------------------------*/

int ftp_port(FTP_CON * con, int a, int b, int c, int d, int e, int f)
/* set port for data connection */
{
   char cmd[256];
   int status;

   sprintf(cmd, "PORT %d,%d,%d,%d,%d,%d", a, b, c, d, e, f);
   if (ftp_send_message(con, cmd) != FTP_SUCCESS)
      return FTP_NET_ERROR;

   status = ftp_get_message(con, cmd);
   if (status == QUIT)
      return QUIT;

   if (!ftp_good(status, 200, EOF))
      return FTP_NET_ERROR;

   return FTP_SUCCESS;
}

/*------------------------------------------------------------------*/

int ftp_move(FTP_CON * con, char *oldname, char *newname)
/* move/rename file */
{
   int status;

   status = ftp_command(con, "RNFR %s", oldname, 200, 350, EOF);

   if (status < 0)
      return ftp_command(con, "RNTO %s", newname, 200, 250, EOF);
   else
      return status;
}

/*------------------------------------------------------------------*/

int ftp_get(FTP_CON * con, char *local_name, char *remote_name)
/* get file */
{
   int fh;
   int status;
   char buff[8192];
   char str[256];
   int count, i;
   long total = 0;
   DWORD start, stop;

   if (ftp_open_read(con, remote_name) >= 0)
      return con->err_no;

   if ((fh = open(local_name, O_RDWR | O_CREAT | O_TRUNC | O_BINARY, 0644)) == -1)
      return FTP_FILE_ERROR;

   start = ss_millitime();

   while ((count = ftp_receive(con->data, buff, sizeof(buff))) > 0) {
      total += write(fh, buff, count);
      i = 0;
      if (ftp_debug_func != NULL) {
         printf("%c\r", bars[(i++) % 4]);
         fflush(stdout);
      }
   }

   close(fh);
   stop = ss_millitime();

   status = ftp_close(con);
   if (ftp_debug_func != NULL) {
      sprintf(str, "%ld bytes received in %1.2f seconds (%1.2lf kB/sec).",
              total, (stop - start) / 1000.0, total / 1024.0 / ((stop - start) / 1000.0));
      ftp_debug_func(str);
   }

   return status;
}

/*------------------------------------------------------------------*/

int ftp_put(FTP_CON * con, char *local_name, char *remote_name)
/* put file */
{
   int fh;
   int status;
   char buff[8193];
   char str[256];
   int count, i = 0;
   long total = 0;
   DWORD start, stop;

   if (ftp_open_write(con, remote_name) >= 0)
      return con->err_no;

   if ((fh = open(local_name, O_BINARY)) == -1)
      return FTP_FILE_ERROR;

   start = ss_millitime();

   while ((count = read(fh, buff, 8192)) > 0) {
      total += ftp_send(con->data, buff, count);
      if (ftp_debug_func != NULL) {
         printf("%c\r", bars[(i++) % 4]);
         fflush(stdout);
      }
   }

   close(fh);
   stop = ss_millitime();

   status = ftp_close(con);
   if (ftp_debug_func != NULL) {
      sprintf(str, "%ld bytes sent in %1.2f seconds (%1.2lf kB/sec).",
              total, (stop - start) / 1000.0, total / 1024.0 / ((stop - start) / 1000.0));
      ftp_debug_func(str);
   }

   return status;
}

/*------------------------------------------------------------------*/

char *ftp_pwd(FTP_CON * con)
/* show present working directory */
{
   static char str[256];
   char tmp[256];
   int status;

   str[0] = 0;
   if (ftp_send_message(con, "PWD") != FTP_SUCCESS)
      return str;

   status = ftp_get_message(con, tmp);

   if (status != 257) {
      if (ftp_error_func != NULL)
         ftp_error_func(tmp);

      return str;
   }

   sscanf(tmp, "%*[^\"]%*c%[^\"]%*s", str);

   return str;
}

/*------------------------------------------------------------------*/

int ftp_dir(FTP_CON * con, char *file)
/* display directory */
{
   char command[256], buffer[8192];

   if (file == NULL || *file == '\0')
      strcpy(command, "LIST");
   else
      sprintf(command, "LIST %s", file);

   if (ftp_data(con, command, "") >= 0)
      return con->err_no;

   while (ftp_receive(con->data, buffer, sizeof(buffer)))
      printf(buffer);

   return ftp_close(con);
}

/*------------------------------------------------------------------*/
