/********************************************************************\

  Name:         sendmail.c
  Created by:   Stefan Ritt

  Contents:     SMTP interface for sending an email

  $Log: sendmail.c,v $
  Revision 1.1.1.1  2005/06/20 23:37:16  mucap
  Importing release 1.9.5 of the MIDAS source code as distributed by PSI.
  (Next, I'll commit our local customizations.)

  Revision 1.3  2004/01/07 12:56:15  midas
  Chaned line length

  Revision 1.2  2004/01/07 12:52:23  midas
  Changed indentation

  Revision 1.1  2003/09/18 07:46:44  midas
  Initial revision

\********************************************************************/

#if defined(_MSC_VER)

#include <windows.h>
#include <time.h>
#include <stdio.h>

#define snprintf _snprintf

#elif defined(__linux__)

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>

#define closesocket(s) close(s)

#else
#error Sendmail.c cannot be compiled under this operating system
#endif

int verbose = 0;

#if defined(_MSC_VER) && defined(_USRDLL)
#define EXPRT __declspec(dllexport)
#else
#define EXPRT
#endif

int EXPRT sendmail(char *smtp_host, char *to, char *subject, char *text);

/*-------------------------------------------------------------------*/

int recv_string(int sock, char *buffer, int buffer_size, int millisec)
{
   int i, n;
   fd_set readfds;
   struct timeval timeout;

   n = 0;
   memset(buffer, 0, buffer_size);

   do {
      if (millisec > 0) {
         FD_ZERO(&readfds);
         FD_SET(sock, &readfds);

         timeout.tv_sec = millisec / 1000;
         timeout.tv_usec = (millisec % 1000) * 1000;

         select(FD_SETSIZE, (void *) &readfds, NULL, NULL, (void *) &timeout);

         if (!FD_ISSET(sock, &readfds))
            break;
      }

      i = recv(sock, buffer + n, 1, 0);

      if (i <= 0)
         break;

      n++;

      if (n >= buffer_size)
         break;

   } while (buffer[n - 1] && buffer[n - 1] != 10);

   return n - 1;
}

/*-------------------------------------------------------------------*/

int sendmail(char *smtp_host, char *to, char *subject, char *text)
{
   struct sockaddr_in bind_addr;
   struct hostent *phe;
   int s;
   char buf[80], host_name[256];
   char str[10000];
   time_t now;
   struct tm *ts;

#ifdef _MSC_VER
   WSADATA WSAData;

   /* Start windows sockets */
   if (WSAStartup(MAKEWORD(1, 1), &WSAData) != 0)
      return -1;
#endif

   gethostname(host_name, sizeof(host_name));

   /* create a new socket for connecting to remote server */
   s = socket(AF_INET, SOCK_STREAM, 0);
   if (s == -1)
      return -1;

   /* connect to remote node port 25 */
   memset(&bind_addr, 0, sizeof(bind_addr));
   bind_addr.sin_family = AF_INET;
   bind_addr.sin_port = htons((short) 25);

   phe = gethostbyname(smtp_host);
   if (phe == NULL)
      return -1;
   memcpy((char *) &(bind_addr.sin_addr), phe->h_addr, phe->h_length);

   if (connect(s, (void *) &bind_addr, sizeof(bind_addr)) < 0) {
      closesocket(s);
      return -1;
   }

   recv_string(s, str, sizeof(str), 10000);
   if (verbose)
      puts(str);

   snprintf(str, sizeof(str) - 1, "HELO %s\r\n", host_name);
   send(s, str, strlen(str), 0);
   if (verbose)
      puts(str);
   recv_string(s, str, sizeof(str), 3000);
   if (verbose)
      puts(str);

   //snprintf(str, sizeof(str) - 1, "MAIL FROM: <labview@%s>\r\n", host_name);  -- needs real mail address
   snprintf(str, sizeof(str) - 1, "MAIL FROM: <stefan.ritt@psi.ch>\r\n");
   send(s, str, strlen(str), 0);
   if (verbose)
      puts(str);
   recv_string(s, str, sizeof(str), 3000);
   if (verbose)
      puts(str);

   snprintf(str, sizeof(str) - 1, "RCPT TO: <%s>\r\n", to);
   send(s, str, strlen(str), 0);
   if (verbose)
      puts(str);
   recv_string(s, str, sizeof(str), 3000);
   if (verbose)
      puts(str);

   snprintf(str, sizeof(str) - 1, "DATA\r\n");
   send(s, str, strlen(str), 0);
   if (verbose)
      puts(str);
   recv_string(s, str, sizeof(str), 3000);
   if (verbose)
      puts(str);

   snprintf(str, sizeof(str) - 1,
            "To: %s\r\nFrom: labview@%s\r\nSubject: %s\r\n", to, host_name, subject);
   send(s, str, strlen(str), 0);
   if (verbose)
      puts(str);

   time(&now);
   ts = localtime(&now);
   strftime(buf, sizeof(buf), "%a, %d %b %Y %H:%M:%S", ts);
   snprintf(str, sizeof(str) - 1, "Date: %s %+03d%02d\r\n\r\n", buf,
            (int) (-timezone / 3600), (int) ((abs(timezone) / 60) % 60));
   send(s, str, strlen(str), 0);
   if (verbose)
      puts(str);

   snprintf(str, sizeof(str) - 1, "%s\r\n.\r\n", text);
   send(s, str, strlen(str), 0);
   if (verbose)
      puts(str);
   recv_string(s, str, sizeof(str), 3000);
   if (verbose)
      puts(str);

   snprintf(str, sizeof(str) - 1, "QUIT\r\n");
   send(s, str, strlen(str), 0);
   if (verbose)
      puts(str);
   recv_string(s, str, sizeof(str), 3000);
   if (verbose)
      puts(str);

   closesocket(s);

   return 1;
}

#if !defined(_USRDLL)
main()
{
   verbose = 1;
   sendmail("mailsend.psi.ch", "stefan.ritt@psi.ch",
            "here comes the subject", "here is the main text");
}
#endif
