/********************************************************************\

  Name:         webpaw.c
  Created by:   Stefan Ritt

  Contents:     Web server for remote PAW display

  $Log: webpaw.c,v $
  Revision 1.1.1.1  2005/06/20 23:37:39  mucap
  Importing release 1.9.5 of the MIDAS source code as distributed by PSI.
  (Next, I'll commit our local customizations.)

  Revision 1.40  2004/01/18 09:58:56  olchansk
  fix compilation on macosx

  Revision 1.39  2004/01/08 08:40:11  midas
  Implemented standard indentation

  Revision 1.38  2003/05/14 13:53:49  midas
  Fixed compiler warning

  Revision 1.37  2003/04/25 14:37:35  midas
  Fixed compiler warnings

  Revision 1.36  2002/04/22 07:40:37  midas
  Added "[Links]" into webpaw.cfg for optional URL display

  Revision 1.35  2001/08/02 12:31:11  midas
  Fixed type

  Revision 1.34  2001/08/02 12:30:00  midas
  Fixed unix bug

  Revision 1.33  2001/08/02 07:33:30  midas
  Open Elog form in new window

  Revision 1.32  2001/06/15 07:29:52  midas
  Version 1.0.10

  Revision 1.31  2001/06/15 07:26:24  midas
  Fixed Genera/Global bug in configuration file

  Revision 1.30  2001/05/18 08:52:48  midas
  Fixed unterminated string in configuration file

  Revision 1.29  2000/11/06 12:29:18  midas
  Increase PAW communication buffers to 100000

  Revision 1.28  2000/11/06 12:03:30  midas
  Version 1.0.9

  Revision 1.27  2000/11/06 12:02:58  midas
  Added -s [prompt] flag

  Revision 1.26  2000/10/16 07:39:45  midas
  Changed parameter separation character from '#' to '&'

  Revision 1.25  2000/10/05 14:46:16  midas
  Added optional parameters

  Revision 1.24  2000/08/24 14:08:05  midas
  Version 1.0.7

  Revision 1.23  2000/08/24 14:04:42  midas
  Fixed bug with removing '+' in commands like fun/plot 1+x 0 1

  Revision 1.22  2000/06/07 10:47:09  midas
  Version 1.0.6

  Revision 1.21  2000/06/06 09:35:57  midas
  Added refresh option

  Revision 1.20  2000/05/26 13:02:30  midas
  Added body1/2/3 tag to display frames in different colors

  Revision 1.19  2000/05/26 07:01:20  midas
  Added <br> after "Create ELog ..."

  Revision 1.18  2000/05/25 10:07:05  midas
  Check y/n character in text tag

  Revision 1.17  2000/05/25 09:22:30  midas
  - Added text output
  - Fixed a few bugs

  Revision 1.16  2000/05/24 14:30:35  midas
  Increased PAW timeout to 15 sec

  Revision 1.15  2000/05/24 14:22:08  midas
  - Added comment display for macros
  - Fixed problem with reusing port numbers

  Revision 1.14  2000/05/23 13:46:09  midas
  Added Elog configuration switch

  Revision 1.13  2000/05/18 12:44:01  midas
  Changed realm

  Revision 1.12  2000/05/18 12:41:18  midas
  Added password protection

  Revision 1.11  2000/05/17 07:17:57  midas
  Added version number

  Revision 1.10  2000/05/17 07:08:54  midas
  Disable shell command via "visibility"

  Revision 1.9  2000/05/16 15:18:12  midas
  Disable "shell" command

  Revision 1.8  2000/05/16 11:27:23  midas
  - Added -d [file] option
  - Check DISPLAY variable
  - Fixed bug with "quit" and "restart"

  Revision 1.7  2000/05/15 15:07:38  midas
  Fixed bug with command line parameters

  Revision 1.6  2000/05/15 14:27:11  midas
  Embed images in .html file

  Revision 1.5  2000/05/15 13:47:13  midas
  Increase banner height

  Revision 1.4  2000/05/15 12:44:52  midas
  Switched to new webpaw.cfg format, added logo display

  Revision 1.3  2000/05/12 12:59:48  midas
  Adjusted WebPAW home directory

  Revision 1.2  2000/05/12 09:00:39  midas
  Added execute button

  Revision 1.1  2000/05/11 14:11:45  midas
  Added file

\********************************************************************/

#include <stdio.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>

#ifdef _MSC_VER
#include <windows.h>
#include <io.h>
#else
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <signal.h>
#include <ctype.h>
#ifdef OS_DARWIN
#include <util.h>
#else
#include <pty.h>
#endif
#include <arpa/inet.h>
#define closesocket(s) close(s)
#endif

/* Version of WebPAW */
#define VERSION "1.0.10"

#define WEB_BUFFER_SIZE 100000
#define MAX_PARAM           10
#define VALUE_SIZE         256
#define PARAM_LENGTH       256

#define SUCCESS              1
#define SHUTDOWN             2
#define ERR_TIMEOUT          3
#define ERR_PIPE             4

char return_buffer[WEB_BUFFER_SIZE];
int return_length;
char webpaw_url[256];

char _param[MAX_PARAM][PARAM_LENGTH];
char _value[MAX_PARAM][VALUE_SIZE];
char host_name[256];
char remote_host_name[256];
int _debug;
char debug_file[256];
int _sock = 0, _quit;

char paw_prompt[80];

#ifndef O_BINARY
#define O_BINARY 0
#endif

/*------------------------------------------------------------------*/

void debug_message(char *str)
{
   FILE *f;

   if (!_debug)
      return;

   if (debug_file[0]) {
      f = fopen(debug_file, "a");
      if (f) {
         fprintf(f, str);
         fclose(f);
      }
   } else {
      printf(str);
      fflush(stdout);
   }
}

/*------------------------------------------------------------------*/

void rsputs(const char *str, ...)
{
   if (strlen(return_buffer) + strlen(str) > sizeof(return_buffer))
      strcpy(return_buffer, "<H1>Error: return buffer too small</H1>");
   else
      strcat(return_buffer, str);
}

/*------------------------------------------------------------------*/

void rsprintf(const char *format, ...)
{
   va_list argptr;
   char str[10000];

   va_start(argptr, format);
   vsprintf(str, (char *) format, argptr);
   va_end(argptr);

   if (strlen(return_buffer) + strlen(str) > sizeof(return_buffer))
      strcpy(return_buffer, "<H1>Error: return buffer too small</H1>");
   else
      strcat(return_buffer, str);
}

int equal_ustring(char *str1, char *str2)
{
   if (str1 == NULL && str2 != NULL)
      return 0;
   if (str1 != NULL && str2 == NULL)
      return 0;
   if (str1 == NULL && str2 == NULL)
      return 1;

   while (*str1)
      if (toupper(*str1++) != toupper(*str2++))
         return 0;

   if (*str2)
      return 0;

   return 1;
}

void format(char *str, char *result)
{
   int i;
   char *p;

   p = result;
   for (i = 0; i < (int) strlen(str); i++) {
      switch (str[i]) {
      case '\n':
         sprintf(p, "<br>\n");
         break;
      case '<':
         sprintf(p, "&lt;");
         break;
      case '>':
         sprintf(p, "&gt;");
         break;
      case '&':
         sprintf(p, "&amp;");
         break;
      case '\"':
         sprintf(p, "&quot;");
         break;
      default:
         *p = str[i];
         *(p + 1) = 0;
      }
      p += strlen(p);
   }
   *p = 0;
}

void format1(char *str, char *result)
{
   int i;
   char *p;

   p = result;
   for (i = 0; i < (int) strlen(str); i++) {
      switch (str[i]) {
      case '<':
         sprintf(p, "&lt;");
         break;
      case '>':
         sprintf(p, "&gt;");
         break;
      case '&':
         sprintf(p, "&amp;");
         break;
      case '\"':
         sprintf(p, "&quot;");
         break;
      default:
         *p = str[i];
         *(p + 1) = 0;
      }
      p += strlen(p);
   }
   *p = 0;
}

/*------------------------------------------------------------------*/

/* Parameter extraction from webpaw.cfg similar to win.ini */

char *cfgbuffer;

int getcfg(char *group, char *param, char *value)
{
   char str[256], *p, *pstr;
   int length;
   int fh;

   /* read configuration file on init */
   if (!cfgbuffer) {
      if (cfgbuffer)
         free(cfgbuffer);

      fh = open("webpaw.cfg", O_RDONLY | O_BINARY);
      if (fh < 0)
         return 0;
      length = lseek(fh, 0, SEEK_END);
      lseek(fh, 0, SEEK_SET);
      cfgbuffer = malloc(length + 1);
      if (cfgbuffer == NULL) {
         close(fh);
         return 0;
      }
      read(fh, cfgbuffer, length);
      cfgbuffer[length] = 0;
      close(fh);
   }

   /* search group */
   p = cfgbuffer;
   do {
      if (*p == '[') {
         p++;
         pstr = str;
         while (*p && *p != ']' && *p != '\n' && *p != '\r')
            *pstr++ = *p++;
         *pstr = 0;
         if (equal_ustring(str, group)) {
            /* search parameter */
            p = strchr(p, '\n');
            if (p)
               p++;
            while (p && *p && *p != '[') {
               pstr = str;
               while (*p && *p != '=' && *p != '\n' && *p != '\r')
                  *pstr++ = *p++;
               *pstr-- = 0;
               while (pstr > str && (*pstr == ' ' || *pstr == '='))
                  *pstr-- = 0;

               if (equal_ustring(str, param)) {
                  if (*p == '=') {
                     p++;
                     while (*p == ' ')
                        p++;
                     pstr = str;
                     while (*p && *p != '\n' && *p != '\r')
                        *pstr++ = *p++;
                     *pstr-- = 0;
                     while (*pstr == ' ')
                        *pstr-- = 0;

                     strcpy(value, str);
                     return 1;
                  }
               }

               if (p)
                  p = strchr(p, '\n');
               if (p)
                  p++;
            }
         }
      }
      if (p)
         p = strchr(p, '\n');
      if (p)
         p++;
   } while (p);

   return 0;
}

int enumcfg(char *group, char *param, char *value, int index)
{
   char str[256], *p, *pstr;
   int i;

   /* open configuration file */
   if (!cfgbuffer)
      getcfg("dummy", "dummy", str);
   if (!cfgbuffer)
      return 0;

   /* search group */
   p = cfgbuffer;
   do {
      if (*p == '[') {
         p++;
         pstr = str;
         while (*p && *p != ']' && *p != '\n' && *p != '\r')
            *pstr++ = *p++;
         *pstr = 0;
         if (equal_ustring(str, group)) {
            /* enumerate parameters */
            i = 0;
            p = strchr(p, '\n');
            if (p)
               p++;
            while (p && *p && *p != '[') {
               pstr = str;
               while (*p && *p != '=' && *p != '\n' && *p != '\r')
                  *pstr++ = *p++;
               *pstr-- = 0;
               while (pstr > str && (*pstr == ' ' || *pstr == '='))
                  *pstr-- = 0;

               if (i == index) {
                  strcpy(param, str);
                  if (*p == '=') {
                     p++;
                     while (*p == ' ')
                        p++;
                     pstr = str;
                     while (*p && *p != '\n' && *p != '\r')
                        *pstr++ = *p++;
                     *pstr-- = 0;
                     while (*pstr == ' ')
                        *pstr-- = 0;

                     strcpy(value, str);
                     return 1;
                  }
               }

               if (p)
                  p = strchr(p, '\n');
               if (p)
                  p++;
               i++;
            }
         }
      }
      if (p)
         p = strchr(p, '\n');
      if (p)
         p++;
   } while (p);

   return 0;
}

/*------------------------------------------------------------------*/

/* Parameter handling functions similar to setenv/getenv */

void initparam()
{
   memset(_param, 0, sizeof(_param));
   memset(_value, 0, sizeof(_value));
}

void setparam(char *param, char *value)
{
   int i;

   for (i = 0; i < MAX_PARAM; i++)
      if (_param[i][0] == 0)
         break;

   if (i < MAX_PARAM) {
      strcpy(_param[i], param);

      if (strlen(value) >= VALUE_SIZE)
         printf("Error: parameter value too big\n");

      strncpy(_value[i], value, VALUE_SIZE);
      _value[i][VALUE_SIZE - 1] = 0;
   } else {
      printf("Error: parameter array too small\n");
   }
}

char *getparam(char *param)
{
   int i;

   for (i = 0; i < MAX_PARAM && _param[i][0]; i++)
      if (equal_ustring(param, _param[i]))
         break;

   if (i < MAX_PARAM && _param[i][0])
      return _value[i];

   return NULL;
}

int isparam(char *param)
{
   int i;

   for (i = 0; i < MAX_PARAM && _param[i][0]; i++)
      if (equal_ustring(param, _param[i]))
         break;

   if (i < MAX_PARAM && _param[i][0])
      return 1;

   return 0;
}

/*------------------------------------------------------------------*/

void urlDecode(char *p)
/********************************************************************\
   Decode the given string in-place by expanding %XX escapes
\********************************************************************/
{
   char *pD, str[3];
   int i;

   pD = p;
   while (*p) {
      if (*p == '%') {
         /* Escape: next 2 chars are hex representation of the actual character */
         p++;
         if (isxdigit(p[0]) && isxdigit(p[1])) {
            str[0] = p[0];
            str[1] = p[1];
            str[2] = 0;
            sscanf(p, "%02X", &i);

            *pD++ = (char) i;
            p += 2;
         } else
            *pD++ = '%';
      } else if (*p == '+') {
         /* convert '+' to ' ' */
         *pD++ = ' ';
         p++;
      } else {
         *pD++ = *p++;
      }
   }
   *pD = '\0';
}

void urlEncode(char *ps)
/********************************************************************\
   Encode the given string in-place by adding %XX escapes
\********************************************************************/
{
   char *pd, *p, str[256];

   pd = str;
   p = ps;
   while (*p) {
      if (strchr(" %&=#+/\\", *p)) {
         sprintf(pd, "%%%02X", *p);
         pd += 3;
         p++;
      } else {
         *pd++ = *p++;
      }
   }
   *pd = '\0';
   strcpy(ps, str);
}

/*------------------------------------------------------------------*/

int read_paw(int pipe, char *delim, char *result)
{
   fd_set readfds;
   char str[100000], tmp[100000];
   struct timeval timeout;
   int i;

   sprintf(str, "\n##WAIT FOR: \"%s\"\n", delim);
   debug_message(str);

   memset(str, 0, sizeof(str));

   do {
      FD_ZERO(&readfds);
      FD_SET(pipe, &readfds);

      timeout.tv_sec = 15;
      timeout.tv_usec = 0;
      select(FD_SETSIZE, (void *) &readfds, NULL, NULL, (void *) &timeout);

      if (FD_ISSET(pipe, &readfds)) {
         memset(tmp, 0, sizeof(tmp));
         i = read(pipe, tmp, sizeof(tmp) - 1);
         if (i <= 0)
            break;

         debug_message(tmp);

         if (strlen(tmp) + strlen(str) < sizeof(str))
            strcat(str, tmp);
      } else {
         if (result)
            strcpy(result, str);

         return ERR_TIMEOUT;
      }

      /*
         if (strlen(str) >= strlen(delim))
         printf("compare1:%s\ncompare2:%s\ncompare3:%s\n", delim, str, str+strlen(str)-strlen(delim));
       */

      if (strlen(str) >= strlen(delim) &&
          strcmp(str + strlen(str) - strlen(delim), delim) == 0)
         break;

   } while (1);

   if (result)
      strcpy(result, str);

   if (i <= 0)
      return ERR_PIPE;

   return SUCCESS;
}


int submit_paw(char *kumac, char *result, int save_gif)
{
#ifndef _MSC_VER
   static int pid = 0, pipe;

   char str[100000];
   int status;

   if (equal_ustring(kumac, "restart") && pid) {
      kill(pid, SIGKILL);
      close(pipe);
      pid = pipe = 0;
   }

   if (pid == 0) {
      if ((pid = forkpty(&pipe, str, NULL, NULL)) < 0)
         return 0;

      if (pid > 0) {
         /* wait for workstation question */
         memset(str, 0, sizeof(str));
         status = read_paw(pipe, "<CR>=1 : ", str);
         if (status != SUCCESS)
            return status;

         /* select default workstation */
         sprintf(str, "1\n");
         write(pipe, str, 2);

         /* wait for prompt */
         status = read_paw(pipe, paw_prompt, str);
         if (status != SUCCESS)
            return status;

         /* disable shell command */
         sprintf(str, "vis shell off; vis vis off;\n");
         write(pipe, str, strlen(str));
         status = read_paw(pipe, paw_prompt, str);
         if (status != SUCCESS)
            return status;
      }
   }

   if (pid > 0) {
      /* parent */
      if (equal_ustring(kumac, "quit")) {
         strcpy(str, kumac);
         strcat(str, "\n");
         debug_message(str);
         write(pipe, str, strlen(str));
         close(pipe);
         return SHUTDOWN;
      }

      if (equal_ustring(kumac, "restart")) {
         strcpy(str, "pict/print webpaw.gif\n");

         write(pipe, str, strlen(str));

         status = read_paw(pipe, paw_prompt, str);
         return status;
      }

      /* submit PAW command */
      strcpy(str, kumac);
      strcat(str, "\n");

      write(pipe, str, strlen(str));

      /* wait for prompt */
      status = read_paw(pipe, paw_prompt, result);
      if (status != SUCCESS)
         return status;

      if (save_gif) {
         /* send print command */
         strcpy(str, "pict/print webpaw.gif\n");
         write(pipe, str, strlen(str));

         /* wait for prompt */
         status = read_paw(pipe, paw_prompt, str);
         if (status != SUCCESS)
            return status;
      }

      if (strstr(str, paw_prompt) && result) {
         strcpy(result, str);
         return SUCCESS;
      } else
         return 0;
   } else {
      /* close inherited network socket */
      if (_sock)
         closesocket(_sock);

      /* start PAW */
      execlp("paw", "paw", NULL);
   }

#endif
   return SUCCESS;
}

/*------------------------------------------------------------------*/

void display_kumac(char *display_name, char *kumac_name)
{
   char str[256], comment_name[256], param_name[256], *p;

   /* single kumac found */
   comment_name[0] = 0;
   param_name[0] = 0;
   if (strchr(kumac_name, '?')) {
      /* extract comment, trim leading blanks */
      p = strchr(kumac_name, '?') + 1;
      while (*p && *p == ' ')
         p++;
      strcpy(comment_name, p);
      /* trim trailing blanks */
      p = comment_name;
      while (*p && *p != ' ' && *p != '\r' && *p != '\n')
         p++;
      *p = 0;
   }
   if (strchr(kumac_name, '&')) {
      /* extract param, trim leading blanks */
      p = strchr(kumac_name, '&') + 1;
      while (*p && *p == ' ')
         p++;
      strcpy(param_name, p);
      /* trim trailing blanks */
      p = param_name;
      while (*p && *p != ' ' && *p != '\r' && *p != '\n')
         p++;
      *p = 0;
   }

   if (strchr(kumac_name, '?'))
      *strchr(kumac_name, '?') = 0;
   if (strchr(kumac_name, '&'))
      *strchr(kumac_name, '&') = 0;

   urlEncode(kumac_name);
   format(display_name, str);

   if (param_name[0]) {
      if (comment_name[0])
         rsprintf
             ("<li><a href=\"/%s.html?comment=%s&param=%s\" target=_blank>%s</a></li>\r\n",
              kumac_name, comment_name, param_name, str);
      else
         rsprintf("<li><a href=\"/%s.html?param=%s\" target=_blank>%s</a></li>\r\n",
                  kumac_name, param_name, str);
   } else if (comment_name[0])
      rsprintf("<li><a href=\"/%s.html?comment=%s\" target=contents>%s</a></li>\r\n",
               kumac_name, comment_name, str);
   else
      rsprintf("<li><a href=\"/%s.html\" target=contents>%s</a></li>\r\n",
               kumac_name, str);
}

/*------------------------------------------------------------------*/

void interprete(char *path)
/********************************************************************\

  Routine: interprete

  Purpose: Interprete parameters and generate HTML output

  Input:
    char *kumac             Kumac to execute

  <implicit>
    _param/_value array accessible via getparam()

\********************************************************************/
{
   char str[100000], str2[100000], group_name[256], display_name[256], kumac_name[256];
   char cmd[256], param_name[256], param_opt[256], *p, *ps;
   char cur_group[256], tmp[256], elog[256];
   int fh, i, j, length, status, height;

   if (!path[0] && !getparam("submit") && !getparam("cmd")) {
      rsprintf("HTTP/1.0 200 Document follows\r\n");
      rsprintf("Server: WebPAW\r\n");
      rsprintf("Content-Type: text/html\r\n\r\n");

      /* return frameset if no path is given */
      rsprintf("<html>\r\n");
      rsprintf("<head><title>WebPAW V%s on %s</title></head>\r\n\r\n", VERSION,
               host_name);

      if (getcfg("Global", "Logo height", str))
         height = atoi(str);
      else
         height = 25;

      rsprintf("<frameset cols=\"300,*\">\r\n");
      rsprintf
          ("  <frame name=kumacs src=kumacs.html marginwidth=1 marginheight=1 scrollling=auto>\r\n");
      rsprintf("  <frameset rows=\"%d,*\" frameborder=no>\r\n", height + 10);
      rsprintf
          ("    <frame name=banner src=banner.html marginwidth=10 marginheight=1 scrolling=no>\r\n");
      rsprintf
          ("    <frame name=contents src=contents.html marginwidth=10 marginheight=5 scrolling=auto>\r\n");
      rsprintf("  </frameset>\r\n");
      rsprintf("</frameset>\r\n\r\n");

      rsprintf("<noframes>\r\n");
      rsprintf("To view this page, your browser must support frames.\r\n");
      rsprintf
          ("<AHREF=\"http://home.netscape.com/comprod/mirror/index.html\">Download</A>\r\n");
      rsprintf("Netscape Navigator 2.0 or later for frames support.\r\n");
      rsprintf("</noframes>\r\n\r\n");

      rsprintf("</html>\r\n");
      return;
   }

   /* display banner */
   if (equal_ustring(path, "banner.html")) {
      rsprintf("HTTP/1.0 200 Document follows\r\n");
      rsprintf("Server: WebPAW\r\n");
      rsprintf("Content-Type: text/html\r\n\r\n");

      if (getcfg("Global", "Body2", str))
         rsprintf("<html><body %s>\r\n", str);
      else
         rsprintf("<html><body>\r\n");

      if (getcfg("Global", "Logo", str)) {
         rsprintf("<a target=_top href=\"http://midas.psi.ch/webpaw/\">\r\n");
         rsprintf("<img src=banner.gif alt=banner.gif border=0></a>\r\n");
      } else {
         /* title row */
         rsprintf
             ("<b><a target=_top href=\"http://midas.psi.ch/webpaw/\">WebPAW</a> V%s on %s</b>\r\n",
              VERSION, host_name);
      }

      rsprintf("</body></html>\r\n");
      return;
   }

   /* display banner */
   if (equal_ustring(path, "banner.gif")) {
      if (getcfg("Global", "Logo", str)) {
         fh = open(str, O_RDONLY | O_BINARY);
         if (fh > 0) {
            length = lseek(fh, 0, SEEK_END);
            lseek(fh, 0, SEEK_SET);

            rsprintf("HTTP/1.0 200 Document follows\r\n");
            rsprintf("Server: WebPAW\r\n");
            rsprintf("Accept-Ranges: bytes\r\n");
            rsprintf("Content-Type: image/gif\r\n");
            rsprintf("Content-Length: %d\r\n\r\n", length);

            /* return if file too big */
            if (length > (int) (sizeof(return_buffer) - strlen(return_buffer))) {
               printf("return buffer too small\n");
               close(fh);
               return;
            }

            return_length = strlen(return_buffer) + length;
            read(fh, return_buffer + strlen(return_buffer), length);

            close(fh);
         }
      }
      return;
   }

   /* display kumacs */
   if (strstr(path, "kumacs.html")) {
      rsprintf("HTTP/1.0 200 Document follows\r\n");
      rsprintf("Server: WebPAW\r\n");
      rsprintf("Content-Type: text/html\r\n\r\n");
      if (getcfg("Global", "Body1", str))
         rsprintf("<html><body %s>\r\n", str);
      else
         rsprintf("<html><body>\r\n");

      rsprintf("<form method=GET action=\"%s\" target=contents>\r\n", webpaw_url);

      /* display input field */
      status = getcfg("Global", "Allow submit", str);
      if (status == 0 || str[0] == 'y') {
         rsprintf("<center><table border=0 cellpadding=1>\r\n");
         rsprintf
             ("<tr><td colspan=2 align=center><input type=text name=cmd size=30 maxlength=256></tr>\r\n");
         rsprintf
             ("<tr><td align=center><input type=submit name=submit value=\" Execute! \">\r\n");
         rsprintf
             ("<td align=center><input type=submit name=restart value=\"Restart PAW!\">\r\n");
         rsprintf("</tr></table><hr>\r\n");
      }

      if (enumcfg("Links", display_name, kumac_name, 0)) {
         rsprintf("<h3>Links</h3></center>\r\n");

         rsprintf("<ul>\r\n");

         for (i = 0;; i++) {
            if (!enumcfg("Links", display_name, kumac_name, i))
               break;

            rsprintf("<li><a target=_blank href=\"%s\">%s</a>\n", kumac_name,
                     display_name);
         }

         rsprintf("</ul>\r\n");
      }


      if (!enumcfg("Kumacs", display_name, kumac_name, 0)) {
         rsprintf
             ("<center>No macros defined in <i>webpaw.cfg</i></center></body></html>\r\n");
         return;
      } else
         rsprintf("<h3>Macros</h3></center>\r\n");


      if (strchr(path, '/')) {
         strcpy(cur_group, path);
         *strchr(cur_group, '/') = 0;
         urlDecode(cur_group);
      } else
         cur_group[0] = 0;

      rsprintf("<ul>\r\n");

      for (i = 0;; i++) {
         if (!enumcfg("Kumacs", display_name, kumac_name, i))
            break;

         if (strncmp(display_name, "Group", 5) == 0) {
            /* whole group found */
            strcpy(group_name, kumac_name);
            format(group_name, str);
            strcpy(str2, group_name);
            urlEncode(str2);

            /* display group */
            rsprintf
                ("<li><b><a href=\"/%s/kumacs.html\" target=kumacs>%s</a></b></li>\r\n",
                 str2, str);

            if (equal_ustring(group_name, cur_group)) {
               rsprintf("<ul>\r\n");

               for (j = 0;; j++) {
                  if (!enumcfg(group_name, display_name, kumac_name, j))
                     break;

                  display_kumac(display_name, kumac_name);
               }

               rsprintf("</ul>\r\n");
            }
         } else
            display_kumac(display_name, kumac_name);
      }

      rsprintf("</ul>\r\n");

      rsprintf("</body></html>\r\n");
      return;
   }

   /* display parameter page */
   if (strstr(path, ".html") && getparam("param")) {
      rsprintf("HTTP/1.0 200 Document follows\r\n");
      rsprintf("Server: WebPAW\r\n");

      rsprintf("Content-Type: text/html\r\n\r\n");

      rsprintf("<head><title>Please enter parameters</title>\r\n");
      rsprintf("</head>\r\n");

      if (getcfg("Global", "Body3", str))
         rsprintf("<html><body %s>\r\n", str);
      else
         rsprintf("<html><body>\r\n");

      rsprintf("<form method=GET action=\"%s\" target=contents>\r\n", webpaw_url);

      rsprintf("<center><h2>Please enter parameters</h2>\r\n");
      rsprintf("for function<p>\r\n");

      strcpy(cmd, path);
      *strstr(cmd, ".html") = 0;
      urlDecode(cmd);

      rsprintf("<code>%s</code><p>\r\n\r\n", cmd);
      rsprintf("<input type=\"hidden\" name=\"cmd\" value=\"%s\">\r\n", cmd);

      rsprintf("<table border=0 cellpadding=5>\r\n");
      for (i = 0;; i++) {
         if (!enumcfg(getparam("param"), param_name, param_opt, i))
            break;

         rsprintf("<tr><td align=right>%s</td><td>", param_name);

         if (strchr(param_opt, ';') == NULL)
            rsprintf("<input type=text name=\"p%d\" value=\"%s\"</td></tr>\r\n", i,
                     param_opt);
         else {
            /* display individual options in drop down box */
            rsprintf("<select size=1 name=\"p%d\">\r\n", i);
            p = param_opt;
            for (j = 0; *p; j++) {
               while (*p && *p == ' ')
                  p++;
               ps = str;
               while (*p && *p != ';')
                  *ps++ = *p++;
               *ps = 0;
               if (*p)
                  p++;
               rsprintf("<option>%s\r\n", str);
            }

            rsprintf("</select>\r\n");
         }
      }
      rsprintf("</table><P>");

      rsprintf("<input type=submit value=\"Submit\">\r\n");

      rsprintf("</body></html>\r\n");
      return;
   }

   /* display contents */
   if (strstr(path, ".html") || getparam("submit") || getparam("cmd")) {
      rsprintf("HTTP/1.0 200 Document follows\r\n");
      rsprintf("Server: WebPAW\r\n");

      rsprintf("Content-Type: text/html\r\n\r\n");

      rsprintf("<head>\r\n");
      if (getcfg("Global", "Refresh", str))
         rsprintf("<meta http-equiv=REFRESH content=%s>", str);
      rsprintf("</head>\r\n");

      if (getcfg("Global", "Body3", str))
         rsprintf("<html><body %s>\r\n", str);
      else
         rsprintf("<html><body>\r\n");

      if (getparam("restart"))
         strcpy(cmd, "restart");
      else if (getparam("cmd")) {
         strcpy(cmd, getparam("cmd"));
         urlEncode(cmd);
      } else
         strcpy(cmd, path);

      /* add parameters if present */
      param_opt[0] = 0;
      for (i = 0;; i++) {
         sprintf(str, "p%d", i);
         if (getparam(str)) {
            strcat(param_opt, " ");
            strcat(param_opt, getparam(str));
         } else
            break;
      }
      if (param_opt[0]) {
         urlEncode(param_opt);
         strcat(cmd, param_opt);
      }

      if (equal_ustring(str, "quit")) {
         status = submit_paw("quit", str, 0);
         if (status == SHUTDOWN)
            rsprintf("<h1>WebPAW shut down successfully</h1>\r\n");
         else
            rsprintf
                ("<h1>Error talking to PAW, return string :</h1>\r\n<pre>%s</pre>\r\n",
                 str);

         _quit = 1;
      } else {
         if (strstr(cmd, ".html"))
            *strstr(cmd, ".html") = 0;

         /* display optional comment */
         if (getparam("comment")) {
            fh = open(getparam("comment"), O_RDONLY);
            if (fh > 0) {
               length = lseek(fh, 0, SEEK_END);
               lseek(fh, 0, SEEK_SET);

               /* return if file too big */
               if (length < (int) (sizeof(return_buffer) - strlen(return_buffer))) {
                  *(return_buffer + strlen(return_buffer) + length) = 0;
                  read(fh, return_buffer + strlen(return_buffer), length);
                  close(fh);
               }
            }
         }

         if (getcfg("Global", "Elog", elog)) {
            rsprintf
                ("<p><a target=_blank href=\"%s\">Create ELog with this picture</a><br>\r\n",
                 elog);
         }

         /* put reference to image */
         rsprintf("<img src=\"/%s.gif\" alt=contents.gif></a>\r\n", cmd);

         /* display optional PAW text output */
         if (!equal_ustring(cmd, "contents"))
            if (getcfg("Global", "Text", str) && str[0] == 'y') {
               rsprintf("<p><pre>\r\n");

               urlDecode(cmd);
               status = submit_paw(cmd, str, 0);

               format1(str, str2);
               rsprintf(str2);

               rsprintf("</pre>\r\n");
            }
      }

      rsprintf("</body></html>\r\n");
      return;
   }

   /* forward command to paw and display result */
   if (strstr(path, ".gif") || getparam("submit") || getparam("cmd")) {
      strcpy(str, path);
      urlDecode(str);
      if (strstr(path, ".gif"))
         *strstr(str, ".gif") = 0;

      if (equal_ustring(path, "contents.gif"))
         str[0] = 0;

      /* disable $shell command */
      for (i = 0; i < (int) strlen(str); i++)
         tmp[i] = toupper(str[i]);
      tmp[i] = 0;
      if (strstr(tmp, "$SHELL"))
         str[0] = 0;

#ifndef _MSC_VER
      status = submit_paw(str, str, 1);
      if (status != SUCCESS) {
         rsprintf("HTTP/1.0 200 Document follows\r\n");
         rsprintf("Server: WebPAW\r\n");
         rsprintf("Content-Type: text/html\r\n\r\n");
         rsprintf
             ("<html><body><h1>Error talking to PAW, return string :</h1>\r\n<pre>%s</pre>\r\n",
              str);
         rsprintf("</body></html>\r\n");
      } else
#endif
      {
         fh = open("webpaw.gif", O_RDONLY | O_BINARY);
         if (fh > 0) {
            length = lseek(fh, 0, SEEK_END);
            lseek(fh, 0, SEEK_SET);

            rsprintf("HTTP/1.0 200 Document follows\r\n");
            rsprintf("Server: WebPAW\r\n");
            rsprintf("Accept-Ranges: bytes\r\n");

            /* return proper header for file type */
            rsprintf("Content-Type: image/gif\r\n");
            rsprintf("Content-Length: %d\r\n", length);
            rsprintf("Pragma: no-cache\r\n");
            rsprintf("Expires: Fri, 01 Jan 1983 00:00:00 GMT\r\n\r\n");

            /* return if file too big */
            if (length > (int) (sizeof(return_buffer) - strlen(return_buffer))) {
               printf("return buffer too small\n");
               close(fh);
               return;
            }

            return_length = strlen(return_buffer) + length;
            read(fh, return_buffer + strlen(return_buffer), length);

            close(fh);
         }
      }

      return;
   }
}

/*------------------------------------------------------------------*/

void decode(char *string)
{
   char path[256];
   char *p, *pitem;

   initparam();

   strncpy(path, string + 1, sizeof(path));     /* strip leading '/' */
   path[255] = 0;
   if (strchr(path, '?'))
      *strchr(path, '?') = 0;
   setparam("path", path);

   if (strchr(string, '?')) {
      p = strchr(string, '?') + 1;

      /* cut trailing "/" from netscape */
      if (p[strlen(p) - 1] == '/')
         p[strlen(p) - 1] = 0;

      p = strtok(p, "&");
      while (p != NULL) {
         pitem = p;
         p = strchr(p, '=');
         if (p != NULL) {
            *p++ = 0;
            urlDecode(pitem);
            urlDecode(p);

            setparam(pitem, p);

            p = strtok(NULL, "&");
         }
      }
   }

   interprete(path);
}

/*------------------------------------------------------------------*/

void sighup(int sig)
{
   /* reread configuration file */
   if (_debug)
      printf("Reread configuration file.\n");

   if (cfgbuffer)
      free(cfgbuffer);
   cfgbuffer = 0;
}

/*------------------------------------------------------------------*/

char *map = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

int cind(char c)
{
   int i;

   if (c == '=')
      return 0;

   for (i = 0; i < 64; i++)
      if (map[i] == c)
         return i;

   return -1;
}

void base64_decode(char *s, char *d)
{
   unsigned int t;

   while (*s) {
      t = cind(*s++) << 18;
      t |= cind(*s++) << 12;
      t |= cind(*s++) << 6;
      t |= cind(*s++) << 0;

      *(d + 2) = (char) (t & 0xFF);
      t >>= 8;
      *(d + 1) = (char) (t & 0xFF);
      t >>= 8;
      *d = (char) (t & 0xFF);

      d += 3;
   }
   *d = 0;
}

void base64_encode(char *s, char *d)
{
   unsigned int t, pad;

   pad = 3 - strlen(s) % 3;
   while (*s) {
      t = (*s++) << 16;
      if (*s)
         t |= (*s++) << 8;
      if (*s)
         t |= (*s++) << 0;

      *(d + 3) = map[t & 63];
      t >>= 6;
      *(d + 2) = map[t & 63];
      t >>= 6;
      *(d + 1) = map[t & 63];
      t >>= 6;
      *(d + 0) = map[t & 63];

      d += 4;
   }
   *d = 0;
   while (pad--)
      *(--d) = '=';
}

/*------------------------------------------------------------------*/


char net_buffer[WEB_BUFFER_SIZE];

void server_loop(int tcp_port, int daemon)
{
   int status, i, n_error;
   struct sockaddr_in bind_addr, acc_addr;
   int lsock, len, flag, header_length, authorized;
   struct hostent *phe;
   struct linger ling;
   fd_set readfds;
   struct timeval timeout;
   char pwd[256], cl_pwd[256], str[256], *p;

   _quit = 0;

#ifdef _MSC_VER
   {
      WSADATA WSAData;

      /* Start windows sockets */
      if (WSAStartup(MAKEWORD(1, 1), &WSAData) != 0)
         return;
   }
#else

   /* set signal handler for HUP signal */
   signal(SIGHUP, sighup);

   /* check DISPLAY variable */
   if (!getenv("DISPLAY")) {
      printf("Please define DISPLAY variable so that PAW can open its HIGZ window.\n");
      return;
   }
#endif

   /* create a new socket */
   lsock = socket(AF_INET, SOCK_STREAM, 0);

   if (lsock == -1) {
      printf("Cannot create socket\n");
      return;
   }

   /* bind local node name and port to socket */
   memset(&bind_addr, 0, sizeof(bind_addr));
   bind_addr.sin_family = AF_INET;
   bind_addr.sin_addr.s_addr = 0;
   bind_addr.sin_port = htons((short) tcp_port);

   gethostname(host_name, sizeof(host_name));

   phe = gethostbyname(host_name);
   if (phe == NULL) {
      printf("Cannot retrieve host name\n");
      return;
   }
   phe = gethostbyaddr(phe->h_addr, sizeof(int), AF_INET);
   if (phe == NULL) {
      printf("Cannot retrieve host name\n");
      return;
   }

   /* if domain name is not in host name, hope to get it from phe */
   if (strchr(host_name, '.') == NULL)
      strcpy(host_name, phe->h_name);

   memcpy((char *) &(bind_addr.sin_addr), phe->h_addr, phe->h_length);

   /* try reusing address */
   flag = 1;
   setsockopt(lsock, SOL_SOCKET, SO_REUSEADDR, (char *) &flag, sizeof(int));

   status = bind(lsock, (struct sockaddr *) &bind_addr, sizeof(bind_addr));
   if (status < 0) {
      printf
          ("Cannot bind to port %d.\nPlease try later or use the \"-p\" flag to specify a different port\n",
           tcp_port);
      return;
   }
#ifndef _MSC_VER
   /* give up root privilege */
   setuid(getuid());
   setgid(getgid());

   /* start paw */
   status = submit_paw("restart", NULL, 1);
   if (status != 1) {
      printf("Error starting PAW: ");
      if (status == ERR_TIMEOUT)
         printf("Timeout\n");
      else if (status == ERR_PIPE)
         printf("PAW stopped unexpectedly\n");
      else
         printf("Unknown error %d\n", status);

      return;
   }

   if (daemon) {
      int i, fd, pid;

      printf("Becoming a daemon...\n");

      if ((pid = fork()) < 0)
         return;
      else if (pid != 0)
         exit(0);               /* parent finished */

      /* child continues here */

      /* try and use up stdin, stdout and stderr, so other
         routines writing to stdout etc won't cause havoc. Copied from smbd */
      for (i = 0; i < 3; i++) {
         close(i);
         fd = open("/dev/null", O_RDWR, 0);
         if (fd < 0)
            fd = open("/dev/null", O_WRONLY, 0);
         if (fd < 0)
            return;
         if (fd != i)
            return;
      }

      setsid();                 /* become session leader */
      umask(0);                 /* clear our file mode createion mask */
   }
#endif

   /* listen for connection */
   status = listen(lsock, SOMAXCONN);
   if (status < 0) {
      printf("Cannot listen\n");
      return;
   }

   /* set my own URL */
   if (tcp_port == 80)
      sprintf(webpaw_url, "http://%s/", host_name);
   else
      sprintf(webpaw_url, "http://%s:%d/", host_name, tcp_port);

   if (!_debug)
      printf("Server listening...\n");

   do {
      FD_ZERO(&readfds);
      FD_SET(lsock, &readfds);

      timeout.tv_sec = 0;
      timeout.tv_usec = 100000;

#ifdef OS_UNIX
      do {
         status = select(FD_SETSIZE, (void *) &readfds, NULL, NULL, (void *) &timeout);
         /* if an alarm signal was cought, restart with reduced timeout */
      } while (status == -1 && errno == EINTR);
#else
      status = select(FD_SETSIZE, (void *) &readfds, NULL, NULL, (void *) &timeout);
#endif

      if (FD_ISSET(lsock, &readfds)) {
         len = sizeof(acc_addr);
         _sock = accept(lsock, (struct sockaddr *) &acc_addr, &len);

         /* turn on lingering (borrowed from NCSA httpd code) */
         ling.l_onoff = 1;
         ling.l_linger = 600;
         setsockopt(_sock, SOL_SOCKET, SO_LINGER, (char *) &ling, sizeof(ling));

         /* ret remote host name */
         phe = gethostbyaddr((char *) &acc_addr.sin_addr, 4, PF_INET);
         if (phe == NULL) {
            /* use IP number instead */
            strcpy(remote_host_name, (char *) inet_ntoa(acc_addr.sin_addr));
         } else
            strcpy(remote_host_name, phe->h_name);

         memset(net_buffer, 0, sizeof(net_buffer));
         len = 0;
         header_length = 0;
         n_error = 0;
         do {
            FD_ZERO(&readfds);
            FD_SET(_sock, &readfds);

            timeout.tv_sec = 6;
            timeout.tv_usec = 0;

#ifdef OS_UNIX
            do {
               status =
                   select(FD_SETSIZE, (void *) &readfds, NULL, NULL, (void *) &timeout);
               /* if an alarm signal was cought, restart with reduced timeout */
            } while (status == -1 && errno == EINTR);
#else
            status = select(FD_SETSIZE, (void *) &readfds, NULL, NULL, (void *) &timeout);
#endif
            if (FD_ISSET(_sock, &readfds))
               i = recv(_sock, net_buffer + len, sizeof(net_buffer) - len, 0);
            else
               goto error;

            /* abort if connection got broken */
            if (i < 0)
               goto error;

            if (i > 0)
               len += i;

            if (i == 0) {
               n_error++;
               if (n_error == 100)
                  goto error;
            }

            /* finish when empty line received */
            if (strstr(net_buffer, "GET") != NULL) {
               if (len > 4 && strcmp(&net_buffer[len - 4], "\r\n\r\n") == 0)
                  break;
               if (len > 6 && strcmp(&net_buffer[len - 6], "\r\r\n\r\r\n") == 0)
                  break;
            } else {
               printf(net_buffer);
               goto error;
            }

         } while (1);

         if (!strchr(net_buffer, '\r'))
            goto error;

         memset(return_buffer, 0, sizeof(return_buffer));
         return_length = 0;

         /* ask for password if configured */
         authorized = 1;
         if (getcfg("Global", "Password", pwd)) {
            authorized = 0;

            /* decode authorization */
            if (strstr(net_buffer, "Authorization:")) {
               p = strstr(net_buffer, "Authorization:") + 14;
               if (strstr(p, "Basic")) {
                  p = strstr(p, "Basic") + 6;
                  while (*p == ' ')
                     p++;
                  i = 0;
                  while (*p && *p != ' ' && *p != '\r')
                     str[i++] = *p++;
                  str[i] = 0;
               }
               base64_decode(str, cl_pwd);
               if (strchr(cl_pwd, ':')) {
                  p = strchr(cl_pwd, ':') + 1;
                  base64_encode(p, str);
                  strcpy(cl_pwd, str);

                  /* check authorization */
                  if (strcmp(str, pwd) == 0)
                     authorized = 1;
               }
            }
         }

         if (!authorized) {
            /* return request for authorization */
            rsprintf("HTTP/1.1 401 Authorization Required\r\n");
            rsprintf("Server: WebPAW\r\n");
            rsprintf("WWW-Authenticate: Basic realm=\"WebPAW\"\r\n");
            rsprintf("Connection: close\r\n");
            rsprintf("Content-Type: text/html\r\n\r\n");

            rsprintf("<HTML><HEAD>\r\n");
            rsprintf("<TITLE>401 Authorization Required</TITLE>\r\n");
            rsprintf("</HEAD><BODY>\r\n");
            rsprintf("<H1>Authorization Required</H1>\r\n");
            rsprintf("This server could not verify that you\r\n");
            rsprintf("are authorized to access the document\r\n");
            rsprintf("requested.  Either you supplied the wrong\r\n");
            rsprintf("credentials (e.g., bad password), or your\r\n");
            rsprintf("browser doesn't understand how to supply\r\n");
            rsprintf("the credentials required.<P>\r\n");
            rsprintf("</BODY></HTML>\r\n");
         } else {
            /* extract path and commands */
            *strchr(net_buffer, '\r') = 0;

            if (!strstr(net_buffer, "HTTP"))
               goto error;
            *(strstr(net_buffer, "HTTP") - 1) = 0;

            /* decode command and return answer */
            decode(net_buffer + 4);
         }

         if (return_length != -1) {
            if (return_length == 0)
               return_length = strlen(return_buffer) + 1;

            send(_sock, return_buffer, return_length, 0);

          error:

            closesocket(_sock);
         }
      }

   } while (!_quit);
}

/*------------------------------------------------------------------*/

void create_password(char *pwd)
{
   int fh, length, i;
   char *cfgbuffer, str[256], *p;

   fh = open("webpaw.cfg", O_RDONLY);
   if (fh < 0) {
      /* create new file */
      fh = open("webpaw.cfg", O_CREAT | O_WRONLY, 0640);
      if (fh < 0) {
         printf("Cannot create \"webpaw.cfg\".\n");
         return;
      }
      sprintf(str, "[Global]\nPassword=%s\n", pwd);
      write(fh, str, strlen(str));
      close(fh);
      printf("File \"webpaw.cfg\" created with password.\n");
      return;
   }

   /* read existing file and add password */
   length = lseek(fh, 0, SEEK_END);
   lseek(fh, 0, SEEK_SET);
   cfgbuffer = malloc(length + 1);
   if (cfgbuffer == NULL) {
      close(fh);
      return;
   }
   length = read(fh, cfgbuffer, length);
   cfgbuffer[length] = 0;
   close(fh);

   fh = open("webpaw.cfg", O_TRUNC | O_WRONLY, 0640);

   if (strstr(cfgbuffer, "Password")) {
      /* replace existing password */
      p = strstr(cfgbuffer, "Password");
      i = (int) p - (int) cfgbuffer;
      write(fh, cfgbuffer, i);
      sprintf(str, "Password=%s\n", pwd);
      write(fh, str, strlen(str));

      /* write remainder of file */
      while (*p && *p != '\n')
         p++;
      if (*p && *p == '\n')
         p++;
      write(fh, p, strlen(p));
   } else if (strstr(cfgbuffer, "[Global]")) {
      /* add password to [Global] section */
      p = strstr(cfgbuffer, "[Global]") + 9;
      while (*p && (*p == '\r' || *p == '\n'))
         p++;
      i = (int) p - (int) cfgbuffer;
      write(fh, cfgbuffer, i);
      sprintf(str, "Password=%s\n", pwd);
      write(fh, str, strlen(str));

      /* write remainder of file */
      write(fh, p, strlen(p));
   } else {
      sprintf(str, "[Global]\nPassword=%s\n\n", pwd);
      write(fh, str, strlen(str));
      write(fh, cfgbuffer, length);
   }

   free(cfgbuffer);
   close(fh);
}

/*------------------------------------------------------------------*/

int main(int argc, char *argv[])
{
   int i;
   int tcp_port = 80, daemon = 0;
   char pwd[256], str[256];

   /* parse command line parameters */
   _debug = 0;
   pwd[0] = 0;
   strcpy(paw_prompt, "PAW > ");
   for (i = 1; i < argc; i++) {
      if (argv[i][0] == '-' && argv[i][1] == 'D')
         daemon = 1;
      else if (argv[i][0] == '-' && argv[i][1] == 'd') {
         _debug = 1;
         if (argv[i + 1] && argv[i + 1][0] != '-')
            strcpy(debug_file, argv[++i]);
      } else if (argv[i][0] == '-') {
         if (i + 1 >= argc || argv[i + 1][0] == '-')
            goto usage;
         if (argv[i][1] == 'p')
            tcp_port = atoi(argv[++i]);
         else if (argv[i][1] == 's')
            strcpy(paw_prompt, argv[++i]);
         else if (argv[i][1] == 'w')
            strcpy(pwd, argv[++i]);
         else {
          usage:
            printf("usage: %s [-p port] [-s prompt] [-d [file]] [-D] [-w password]\n\n",
                   argv[0]);
            printf("       -p set port under which webpaw runs (default: 80)\n");
            printf("       -s use alternative PAW prompt (instead \"PAW > \")\n");
            printf
                ("       -d [file] display debug messages, optionally put them into file\n");
            printf("       -D become a daemon\n");
            printf("       -w create/overwrite password in webpaw.cfg\n\n");
            return 1;
         }
      }
   }

   if (pwd[0]) {
      base64_encode(pwd, str);
      create_password(str);
      return 0;
   }

   if (_debug && daemon && !debug_file[0]) {
      printf("Error: when -D flag is given, -d must contain a filename.\n");
      return 1;
   }

   server_loop(tcp_port, daemon);

   return 0;
}
