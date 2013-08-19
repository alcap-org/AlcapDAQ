/********************************************************************\

  Name:         lcwp950.c
  Created by:   Stefan Ritt

  Contents:     LeCroy WavePro 950 Digital Storage Oscilloscope driver

  $Log: lcwp950.c,v $
  Revision 1.1.1.1  2005/06/20 23:37:09  mucap
  Importing release 1.9.5 of the MIDAS source code as distributed by PSI.
  (Next, I'll commit our local customizations.)

  Revision 1.3  2004/07/16 08:20:18  schneebeli
  Added timeout to scope_wait()

  Revision 1.1  2002/01/14 16:49:53  midas
  Initial revisioin


\********************************************************************/


#include <stdio.h>
#include <windows.h>
#include <math.h>
#include <midas.h>

#include "lcwp950.h"

/*-------------------------------------------------------------------*/

#define EOI_FLAG      0x01
#define SRQ_FLAG      0x08
#define CLEAR_FLAG    0x10
#define LOCKOUT_FLAG  0x20
#define REMOTE_FLAG   0x40
#define DATA_FLAG     0x80

typedef struct {
   char addr[256];
   int sock;
   int channel;
   double vertical_gain;
   double vertical_offset;
   double horiz_interval;
   double horiz_offset;
} SCOPE_CHN;


typedef struct {
   unsigned char flags;
   unsigned char reserverd[3];
   int length;
} SCOPE_HEADER;

SCOPE_CHN scope_chn[SCOPE_MAX_CHN];

/*-------------------------------------------------------------------*/

int scope_send(int sock, char *result, const char *command, ...)
{
   int i, n, len, size;
   fd_set readfds;
   struct timeval timeout;
   va_list argptr;
   char str[1000];
   unsigned char buffer[1100];
   int millisec = 1000;
   SCOPE_HEADER header;

   memset(buffer, 0, sizeof(buffer));
   memset(&header, 0, sizeof(header));

   va_start(argptr, command);
   vsprintf(str, (char *) command, argptr);
   va_end(argptr);

   len = strlen(str);

   header.flags = EOI_FLAG | DATA_FLAG;
   header.reserverd[0] = 1;
   header.length = htonl(len);

   send(sock, (void *) &header, sizeof(header), 0);
   send(sock, str, strlen(str), 0);

   if (!result)
      return 0;

   memset(buffer, 0, sizeof(buffer));
   size = n = 0;
   do {
      FD_ZERO(&readfds);
      FD_SET(sock, &readfds);

      timeout.tv_sec = millisec / 1000;
      timeout.tv_usec = (millisec % 1000) * 1000;

      select(FD_SETSIZE, (void *) &readfds, NULL, NULL, (void *) &timeout);

      if (!FD_ISSET(sock, &readfds))
         break;

      i = recv(sock, buffer + n, 1, 0);

      if (i <= 0)
         break;

      n++;

      if (n == 8)
         size = 8 + buffer[7] + buffer[6] * 256;

   } while (size == 0 || n < size);


   if (size == 0)
      return 0;

   memcpy(result, buffer + 8, size - 8);
   result[size - 8] = 0;

   return size - 8;
}

/*-------------------------------------------------------------------*/

int scope_open(char *addr, int channel)
{
   struct sockaddr_in bind_addr;
   struct hostent *phe;
   int index, i, s;
   char str[256];

   /* find new entry in channel table */
   for (index = 0; index < SCOPE_MAX_CHN; index++)
      if (scope_chn[index].sock == 0)
         break;

   /* check if maximum number exceeded */
   if (index == SCOPE_MAX_CHN)
      return -1;

   /* check if connection already established with scope */
   for (i = 0; i < index; i++)
      if (scope_chn[i].sock && strstr(scope_chn[i].addr, addr) != NULL) {
         strcpy(scope_chn[index].addr, addr);
         scope_chn[index].sock = scope_chn[i].sock;
         scope_chn[index].channel = channel;
         s = scope_chn[index].sock;
         break;
      }

   if (i == index) {
      /* open new connection */

#ifdef _MSC_VER
      WSADATA WSAData;

      /* Start windows sockets */
      if (WSAStartup(MAKEWORD(1, 1), &WSAData) != 0)
         return -1;
#endif

      /* create a new socket for connecting to remote server */
      s = socket(AF_INET, SOCK_STREAM, 0);
      if (s == -1)
         return -1;

      /* connect to remote node port 1861 */
      memset(&bind_addr, 0, sizeof(bind_addr));
      bind_addr.sin_family = AF_INET;
      bind_addr.sin_port = htons((short) 1861);

      phe = gethostbyname(addr);
      if (phe == NULL)
         return -1;
      memcpy((char *) &(bind_addr.sin_addr), phe->h_addr, phe->h_length);

      if (connect(s, (void *) &bind_addr, sizeof(bind_addr)) < 0) {
         closesocket(s);
         return -1;
      }

      /* check if scope alive */
      if (scope_send(s, str, "*idn?") < 10) {
         closesocket(s);
         return -1;
      }

      strcpy(scope_chn[index].addr, addr);
      scope_chn[index].sock = s;
      scope_chn[index].channel = channel;

      /* set header format, memory depth, trigger */
      scope_send(s, NULL, "CHDR OFF; CFMT OFF,BYTE,BIN; MSIZ %d; TRMD SINGLE; INE 1",
                 SCOPE_MEM_SIZE);
   }

   /* get vertical and horizontal scales */
   scope_send(s, str, "C%d:INSP? 'vertical_gain'", channel);
   scope_chn[index].vertical_gain = atof(str + 22);

   scope_send(s, str, "C%d:INSP? 'vertical_offset'", channel);
   scope_chn[index].vertical_offset = atof(str + 22);

   scope_send(s, str, "C%d:INSP? 'horiz_interval'", channel);
   scope_chn[index].horiz_interval = atof(str + 22);

   scope_send(s, str, "C%d:INSP? 'horiz_offset'", channel);
   scope_chn[index].horiz_offset = atof(str + 22);

   return index;
}

/*-------------------------------------------------------------------*/

void scope_close(int fh)
{
   closesocket(scope_chn[fh].sock);
   memset(&scope_chn[fh], 0, sizeof(SCOPE_CHN));
}

/*-------------------------------------------------------------------*/

void scope_arm()
{
   int i, j;

   /* go through all channels */
   for (i = 0; i < SCOPE_MAX_CHN; i++) {
      if (scope_chn[i].sock) {
         /* skip channel if that scope already armed */
         for (j = 0; j < i; j++)
            if (scope_chn[j].sock == scope_chn[i].sock)
               break;

         if (j == i)
            /* arm trigger */
            scope_send(scope_chn[i].sock, NULL, "ARM");
      }
   }
}

/*-------------------------------------------------------------------*/

int scope_wait(int fh, unsigned int timeout)
{
   char str[256];
   unsigned int start;

   start = ss_millitime();

   do {
      scope_send(scope_chn[fh].sock, str, "INR?");

      if (ss_millitime() - start > timeout)
         return FALSE;

   } while ((atoi(str) & 1) == 0);

   return TRUE;
}

/*-------------------------------------------------------------------*/

int scope_read(int fh, double x[SCOPE_MEM_SIZE], double y[SCOPE_MEM_SIZE])
{
   int i, n;
   char str[256], buffer[100000];

   n = scope_send(scope_chn[fh].sock, buffer, "C%d:WF? DAT1", scope_chn[fh].channel);

   /* read trailing LF */
   scope_send(scope_chn[fh].sock, str, "");

   if (n > SCOPE_MEM_SIZE)
      n = SCOPE_MEM_SIZE;

   /* try to understand error happening during data taking with wong fh */
   for (i = 0; i < n; i++) {
      x[i] = i * scope_chn[fh].horiz_interval + scope_chn[fh].horiz_offset;
      y[i] = scope_chn[fh].vertical_gain * buffer[i] - scope_chn[fh].vertical_offset;
   }

   return n;
}

/*-------------------------------------------------------------------*/

/*
Example of reading out two scopes 

main()
{
int    i, n, fh[8];
double x[SCOPE_MEM_SIZE], y[SCOPE_MEM_SIZE];

  for (i=0 ; i<4; i++)
    fh[i] = scope_open("129.129.228.45", i+1);

  for ( ; i<8; i++)
    fh[i] = scope_open("129.129.228.46", i-3);

  do
    {
    printf("Wait for trigger...\n");
    scope_arm();
    scope_wait(fh[0]);

    for (i=0 ; i<8 ; i++)
      {
      n = scope_read(fh[i], x, y);
      printf("CHN%d: %d samples\n", i, n);
      }

    } while (1);

  for (i=0 ; i<8 ; i++)
    scope_close(fh[i]);
}

*/
