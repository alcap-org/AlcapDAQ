/********************************************************************\

  Name:         msgdump.c
  Created by:   Stefan Ritt

  Contents:     MIDAS message dump program. Attaches to the system
                message buffer and extracts messages.

  $Log: msgdump.c,v $
  Revision 1.1.1.1  2005/06/20 23:37:10  mucap
  Importing release 1.9.5 of the MIDAS source code as distributed by PSI.
  (Next, I'll commit our local customizations.)

  Revision 1.3  2004/01/08 08:40:08  midas
  Implemented standard indentation

  Revision 1.2  1998/10/12 12:18:58  midas
  Added Log tag in header


\********************************************************************/

#include "midas.h"
#include "msystem.h"

/*------------------------------------------------------------------*/

char *type_name[] = {
   "ERROR",
   "INFO",
   "DEBUG" "USER",
   "LOG",
   "TALK",
   "CALL",
};

main(int argc, char **argv)
{
   int msg_buf, i, status, size, id;
   char event[10000];
   EVENT_HEADER *pheader;
   BUFFER_HEADER buffer_header;
   char *pstring;
   char type_string[100];
   FILE *f;

   if (argc < 2) {
      printf("Usage: msgdump [-dwc]                             \n");
      printf("                                                  \n");
      printf("       -d display messages on screen              \n");
      printf("       -c display clients producing messages      \n");
      printf("       -w write messages to MSGDUMP.ASC           \n\n");
      return 1;
   }

   status = cm_connect_experiment("", "", "MsgDump", NULL);
   if (status != CM_SUCCESS)
      return 1;

   bm_open_buffer(MESSAGE_BUFFER_NAME, MESSAGE_BUFFER_SIZE, &msg_buf);

   bm_get_buffer_info(msg_buf, &buffer_header);

   bm_request_event(msg_buf, EVENTID_ALL, TRIGGER_ALL, GET_SOME, &id, NULL);

   pstring = event + sizeof(EVENT_HEADER);
   pheader = (EVENT_HEADER *) event;

   if (argv[1][1] == 'd') {
      printf("Displaying messages. Hit Ctrl-C to abort.\n");
      printf("=========================================\n\n");

      do {
         size = 10000;
         status = bm_receive_event(msg_buf, event, &size, SYNC);

         if (status != BM_SUCCESS) {
            printf("Error receiving event.\n");
            return 0;
         }

         type_string[0] = 0;
         for (i = 0; i < 8; i++)
            if (pheader->trigger_mask & (1 << i))
               strcat(type_string, type_name[i]);

         printf("serial %d, type %s, time %1.3lf\n",
                pheader->serial_number, type_string, pheader->time_stamp / 1000.0);
         printf("%s\n\n", pstring);
      } while (1);
   }

   if (argv[1][1] == 'c') {
      printf("Clients attached to current message system:\n");
      printf("===========================================\n\n");

      for (i = 0; i < buffer_header.max_client_index; i++)
         if (buffer_header.client[i].pid)
            printf("#%d: pid=%d name=%s\n", i, buffer_header.client[i].pid,
                   buffer_header.client[i].name);
   }

   if (argv[1][1] == 'w') {
      printf("Message collecting active, hit ENTER to stop.\n");

      getchar();

      printf("Writing MSGDUMP.ASC ...\n");

      f = fopen("MSGDUMP.ASC", "w");

      bm_get_buffer_info(msg_buf, &buffer_header);

      do {
         size = 10000;
         status = bm_receive_event(msg_buf, event, &size, ASYNC);
         if (status == BM_ASYNC_RETURN) {
            fclose(f);
            break;
         }

         type_string[0] = 0;
         for (i = 0; i < 8; i++)
            if (pheader->trigger_mask & (1 << i))
               strcat(type_string, type_name[i]);

         fprintf(f, "serial %d, type %s, time %1.3lf\n",
                 pheader->serial_number, type_string, pheader->time_stamp / 1000.0);
         fprintf(f, "%s\n\n", pstring);
      } while (1);
   }


   cm_disconnect_experiment();
   return 1;
}
