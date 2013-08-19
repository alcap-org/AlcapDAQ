/********************************************************************\

  Name:         mlxspeaker.c
  Created by:   Pierre-Andre Amaudruz

  Contents:     Speaks midas messages (UNIX version)

  $Log: mlxspeaker.c,v $
  Revision 1.1.1.1  2005/06/20 23:37:38  mucap
  Importing release 1.9.5 of the MIDAS source code as distributed by PSI.
  (Next, I'll commit our local customizations.)

  Revision 1.11  2004/01/08 08:40:11  midas
  Implemented standard indentation

  Revision 1.10  2003/05/09 07:40:05  midas
  Added extra parameter to cm_get_environment

  Revision 1.9  2003/04/23 23:09:08  pierre
  Fixed compiler warning

  Revision 1.8  2002/09/26 00:09:17  pierre
  include arg for 'play beep'

  Revision 1.7  2002/06/03 06:07:15  midas
  Added extra parameter to ss_daemon_init to keep stdout

  Revision 1.6  2000/09/29 09:52:14  midas
  Replace "isblank" routine which doesn't exist under DECUnix

  Revision 1.5  1999/11/29 17:51:29  pierre
  - add daemon startup (-D)

  Revision 1.4  1999/07/15 23:49:29  pierre
  -Fix last char of the line being ignored

  Revision 1.3  1999/05/06 19:09:49  pierre
  - Fix empty trailing char on the message which would hold festival

  Revision 1.2  1998/10/12 12:19:03  midas
  Added Log tag in header


  Previous revision history
  ------------------------------------------------------------------
  date        by    modification
  --------    ---   ------------------------------------------------
  30-JAN-97   PAA   created
  05-JUN-98   Glenn Ported to "festival speech system"

\********************************************************************/

#include <stdio.h>
#include <string.h>
#include <signal.h>
#include "midas.h"
#include "msystem.h"

#define SPEECH_PROGRAM "festival --tts -"

static FILE *fp = NULL;
BOOL debug = FALSE;
char mtUserStr[128], mtTalkStr[128];
DWORD shutupTime = 10;
/*------------------------------------------------------------------*/

void sigpipehandler(int sig)
{
   cm_msg(MERROR, "Speaker", "No speech synthesizer attached");
   cm_disconnect_experiment();
   pclose(fp);
   exit(2);
}

void siginthandler(int sig)
{
   cm_msg(MINFO, "Speaker", "Speaker interrupted");
   cm_disconnect_experiment();
   pclose(fp);
   exit(0);
}

/*----- receive_message --------------------------------------------*/

void receive_message(HNDLE hBuf, HNDLE id, EVENT_HEADER * header, void *message)
{
   char str[256], *pc, *sp;
   static DWORD last_beep = 0;

   /* print message */
   printf("%s\n", (char *) (message));

   if (fp == NULL) {
      fputs("Speech synthesizer not enabled - terminating\n", stderr);
      cm_disconnect_experiment();
      exit(2);
   }

   if (debug) {
      printf("evID:%hx Mask:%hx Serial:%li Size:%ld\n", header->event_id,
             header->trigger_mask, header->serial_number, header->data_size);
      pc = strchr((char *) (message), ']') + 2;
   }

   /* skip none talking message */
   if (header->trigger_mask == MT_TALK || header->trigger_mask == MT_USER) {
      pc = strchr((char *) (message), ']') + 2;
      sp = pc + strlen(pc) - 1;
      while (*sp == ' ' || *sp == '\t')
         sp--;
      *(++sp) = '\0';
      if (debug) {
         printf("<%s>", pc);
         printf(" sending msg to festival\n");
      }

      /* Send beep first */
      // "play --volume=0.3 /etc/mt_talk.wav"
      if ((ss_time() - last_beep) > shutupTime) {
         switch (header->trigger_mask) {
         case MT_TALK:
            if (mtTalkStr[0])
               sprintf(str, mtTalkStr);
            break;
         case MT_USER:
            if (mtUserStr[0])
               sprintf(str, mtUserStr);
            break;
         }
         ss_system(str);
         last_beep = ss_time();
         ss_sleep(200);
      }

      fprintf(fp, "%s.\n.\n", pc);
      fflush(fp);
   }

   return;
}

/*------------------------------------------------------------------*/
int main(int argc, char *argv[])
{
   BOOL daemon = FALSE;
   INT status, i;
   char host_name[HOST_NAME_LENGTH];
   char exp_name[NAME_LENGTH];
   char *speech_program = SPEECH_PROGRAM;

   /* get default from environment */
   cm_get_environment(host_name, sizeof(host_name), exp_name, sizeof(exp_name));

   /* parse command line parameters */
   for (i = 1; i < argc; i++) {
      if (argv[i][0] == '-' && argv[i][1] == 'd')
         debug = TRUE;
      else if (argv[i][0] == '-' && argv[i][1] == 'D')
         daemon = TRUE;
      else if (argv[i][0] == '-') {
         if (i + 1 >= argc || argv[i + 1][0] == '-')
            goto usage;
         if (argv[i][1] == 'e')
            strcpy(exp_name, argv[++i]);
         else if (argv[i][1] == 'h')
            strcpy(host_name, argv[++i]);
         else if (argv[i][1] == 'c')
            speech_program = argv[++i];
         else if (argv[i][1] == 't')
            strcpy(mtTalkStr, argv[++i]);
         else if (argv[i][1] == 'u')
            strcpy(mtUserStr, argv[++i]);
         else if (argv[i][1] == 's')
            shutupTime = atoi(argv[++i]);
         else {
          usage:
            printf
                ("usage: mlxspeaker [-h Hostname] [-e Experiment] [-c command] [-D] daemon\n");
            printf("                  [-t mt_talk] Specify the mt_talk alert command\n");
            printf("                  [-u mt_user] Specify the mt_user alert command\n");
            printf
                ("                  [-s shut up time] Specify the min time interval between alert [s]\n");
            printf
                ("                  The -t & -u switch require a command equivalent to:\n");
            printf("                  '-t play --volume=0.3 file.wav'\n");
            printf
                ("                  [-c command] Used to start the speech synthesizer,\n");
            printf
                ("                              which should read text from it's standard input.\n");
            printf
                ("                              eg: mlxspeaker -c 'festival --tts -'\n\n");
            return 0;
         }
      }
   }

   if (daemon) {
      printf("Becoming a daemon...\n");
      ss_daemon_init(FALSE);
   }

   /* now connect to server */
   status = cm_connect_experiment(host_name, exp_name, "Speaker", NULL);
   if (status != CM_SUCCESS)
      return 1;

   cm_msg_register(receive_message);

   /* Handle SIGPIPE signals generated from errors on the pipe */
   signal(SIGPIPE, sigpipehandler);
   signal(SIGINT, siginthandler);
   if (NULL == (fp = popen(speech_program, "w"))) {
      cm_msg(MERROR, "Speaker", "Unable to start \"%s\": %s\n",
             speech_program, strerror(errno));
      cm_disconnect_experiment();
      exit(2);
   }

   printf("Midas Message Speaker connected to %s.\n",
          host_name[0] ? host_name : "local host");

   do {
      status = cm_yield(1000);
   } while (status != RPC_SHUTDOWN && status != SS_ABORT);

   pclose(fp);

   cm_disconnect_experiment();
   return 1;
}
