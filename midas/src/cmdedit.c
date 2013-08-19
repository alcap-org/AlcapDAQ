/********************************************************************\

  Name:         cmdedit.c
  Created by:   Stefan Ritt

  Contents:     Command-line editor for ODBEdit

  $Log: cmdedit.c,v $
  Revision 1.1.1.1  2005/06/20 23:37:30  mucap
  Importing release 1.9.5 of the MIDAS source code as distributed by PSI.
  (Next, I'll commit our local customizations.)

  Revision 1.11  2004/01/08 08:40:09  midas
  Implemented standard indentation

  Revision 1.10  2002/05/08 19:54:40  midas
  Added extra parameter to function db_get_value()

  Revision 1.9  2000/10/23 14:34:38  midas
  Fixed output of '%' when editing

  Revision 1.8  1999/06/23 09:35:41  midas
  Switched back cursor to end of line for editing

  Revision 1.7  1998/12/10 16:32:14  midas
  Command line completion now also adds values if <tab> is pressed after full key

  Revision 1.6  1998/11/02 08:41:41  midas
  - Added fflush() at the beginning
  - Fixed bug with <arrow up> under UNIX

  Revision 1.5  1998/10/29 14:52:17  midas
  <tab> completion now also works if not at the end of the line

  Revision 1.4  1998/10/12 12:23:51  midas
  Added run state character in ODBEdit prompt

  Revision 1.3  1998/10/12 12:19:01  midas
  Added Log tag in header

\********************************************************************/

#include "midas.h"
#include "msystem.h"

#ifdef OS_MSDOS
#define MAX_HISTORY 10
#else
#define MAX_HISTORY 50
#endif

#define LINE_LENGTH 256

char history[MAX_HISTORY][LINE_LENGTH];
INT his_index = 0;

#ifdef OS_VMS

INT cmd_edit(char *prompt, char *cmd, INT(*dir) (char *, INT *), INT(*idle) ())
{
   printf(prompt);
   ss_gets(cmd, 256);

   return strlen(cmd);
}

#else

INT cmd_edit(char *prompt, char *cmd, INT(*dir) (char *, INT *), INT(*idle) ())
{
   char line[LINE_LENGTH];
   INT i, j, k, c, hi;
   INT status;
   DWORD last_time = 0;
   BOOL escape_flag = 0;

   if (ss_getchar(0) == -1) {
      /* normal input if ss_getchar not supported */
      fputs(prompt, stdout);
      ss_gets(cmd, 256);
      return strlen(cmd);
   }

   fputs(prompt, stdout);
   fflush(stdout);

   hi = his_index;
   memset(line, 0, LINE_LENGTH);
   memset(history[hi], 0, LINE_LENGTH);
   strcpy(line, cmd);
   fputs(line, stdout);
   i = strlen(cmd);
   fflush(stdout);

   do {
      c = ss_getchar(0);

      if (c == 27)
         escape_flag = TRUE;

      if (c >= ' ' && c < CH_EXT && escape_flag) {
         escape_flag = FALSE;
         if (c == 'p')
            c = 6;
      }

      /* normal input */
      if (c >= ' ' && c < CH_EXT) {
         if (strlen(line) < LINE_LENGTH - 1) {
            for (j = strlen(line); j >= i; j--)
               line[j + 1] = line[j];
            if (i < LINE_LENGTH - 1) {
               line[i++] = c;
               fputc(c, stdout);
            }
            for (j = i; j < (INT) strlen(line); j++)
               fputc(line[j], stdout);
            for (j = i; j < (INT) strlen(line); j++)
               fputc('\b', stdout);
         }
      }

      /* BS */
      if (c == CH_BS && i > 0) {
         i--;
         fputc('\b', stdout);
         for (j = i; j <= (INT) strlen(line); j++) {
            line[j] = line[j + 1];
            if (line[j])
               fputc(line[j], stdout);
            else
               fputc(' ', stdout);
         }
         for (k = 0; k < j - i; k++)
            fputc('\b', stdout);
      }

      /* DELETE/Ctrl-D */
      if (c == CH_DELETE || c == 4) {
         for (j = i; j <= (INT) strlen(line); j++) {
            line[j] = line[j + 1];
            if (line[j])
               fputc(line[j], stdout);
            else
               fputc(' ', stdout);
         }
         for (k = 0; k < j - i; k++)
            fputc('\b', stdout);
      }

      /* Erase line: CTRL-W, CTRL-U */
      if (c == 23 || c == 21) {
         i = strlen(line);
         memset(line, 0, sizeof(line));
         printf("\r%s", prompt);
         for (j = 0; j < i; j++)
            fputc(' ', stdout);
         for (j = 0; j < i; j++)
            fputc('\b', stdout);
         i = 0;
      }

      /* Erase line from cursor: CTRL-K */
      if (c == 11) {
         for (j = i; j < (INT) strlen(line); j++)
            fputc(' ', stdout);
         for (j = i; j < (INT) strlen(line); j++)
            fputc('\b', stdout);
         for (j = strlen(line); j >= i; j--)
            line[j] = 0;
      }

      /* left arrow, CTRL-B */
      if ((c == CH_LEFT || c == 2) && i > 0) {
         i--;
         fputc('\b', stdout);
      }

      /* right arrow, CTRL-F */
      if ((c == CH_RIGHT || c == 6) && i < (INT) strlen(line))
         fputc(line[i++], stdout);

      /* HOME, CTRL-A */
      if ((c == CH_HOME || c == 1) && i > 0) {
         for (j = 0; j < i; j++)
            fputc('\b', stdout);
         i = 0;
      }

      /* END, CTRL-E */
      if ((c == CH_END || c == 5) && i < (INT) strlen(line)) {
         for (j = i; j < (INT) strlen(line); j++)
            fputc(line[i++], stdout);
         i = strlen(line);
      }

      /* up arrow / CTRL-P */
      if (c == CH_UP || c == 16) {
         if (history[(hi + MAX_HISTORY - 1) % MAX_HISTORY][0]) {
            hi = (hi + MAX_HISTORY - 1) % MAX_HISTORY;
            i = strlen(line);
            fputc('\r', stdout);
            fputs(prompt, stdout);
            for (j = 0; j < i; j++)
               fputc(' ', stdout);
            for (j = 0; j < i; j++)
               fputc('\b', stdout);
            memcpy(line, history[hi], 256);
            i = strlen(line);
            for (j = 0; j < i; j++)
               fputc(line[j], stdout);
         }
      }

      /* down arrow / CTRL-N */
      if (c == CH_DOWN || c == 14) {
         if (history[hi][0]) {
            hi = (hi + 1) % MAX_HISTORY;
            i = strlen(line);
            fputc('\r', stdout);
            fputs(prompt, stdout);
            for (j = 0; j < i; j++)
               fputc(' ', stdout);
            for (j = 0; j < i; j++)
               fputc('\b', stdout);
            memcpy(line, history[hi], 256);
            i = strlen(line);
            for (j = 0; j < i; j++)
               fputc(line[j], stdout);
         }
      }

      /* CTRL-F */
      if (c == 6) {
         for (j = (hi + MAX_HISTORY - 1) % MAX_HISTORY; j != hi;
              j = (j + MAX_HISTORY - 1) % MAX_HISTORY)
            if (history[j][0] && strncmp(line, history[j], i) == 0) {
               memcpy(line, history[j], 256);
               fputs(line + i, stdout);
               i = strlen(line);
               break;
            }
         if (j == hi)
            fputc(7, stdout);
      }

      /* tab */
      if (c == 9 && dir != NULL) {
         status = dir(line, &i);

         /* redraw line */
         fputc('\r', stdout);
         fputs(prompt, stdout);
         fputs(line, stdout);

         for (j = 0; j < (INT) strlen(line) - i; j++)
            fputc('\b', stdout);
      }

      if (c != 0) {
         last_time = ss_millitime();
         fflush(stdout);
      }

      if ((ss_millitime() - last_time > 300) && idle != NULL) {
         status = idle();

         if (status) {
            fputc('\r', stdout);
            fputs(prompt, stdout);
            fputs(line, stdout);

            for (j = 0; j < (INT) strlen(line) - i; j++)
               fputc('\b', stdout);

            fflush(stdout);
         }
      }

   } while (c != CH_CR);

   strcpy(cmd, line);

   if (dir != NULL)
      if (strcmp(cmd, history[(his_index + MAX_HISTORY - 1) % MAX_HISTORY]) != 0 &&
          cmd[0]) {
         strcpy(history[his_index], cmd);
         his_index = (his_index + 1) % MAX_HISTORY;
      }

   /* reset terminal */
   ss_getchar(1);

   fputc('\n', stdout);

   return strlen(line);
}

#endif
