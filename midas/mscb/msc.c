/********************************************************************\

  Name:         msc.c
  Created by:   Stefan Ritt

  Contents:     Command-line interface for the Midas Slow Control Bus

  $Log: msc.c,v $
  Revision 1.1.1.1  2005/06/20 23:37:15  mucap
  Importing release 1.9.5 of the MIDAS source code as distributed by PSI.
  (Next, I'll commit our local customizations.)

  Revision 1.69  2004/10/03 18:02:33  olchansk
  Do automatic system dependance (copied from midas/Makefile)
  add support for MacOSX aka darwin

  Revision 1.68  2004/09/10 12:27:21  midas
  Version 1.7.5

  Revision 1.67  2004/07/21 14:18:19  midas
  Added more debugging info

  Revision 1.66  2004/05/17 09:32:52  midas
  Added Sleep

  Revision 1.65  2004/04/05 10:10:27  midas
  Fixed linux warnings

  Revision 1.64  2004/03/19 12:09:16  midas
  Upload with simplified CRC

  Revision 1.63  2004/03/11 09:57:46  midas
  Changed error strings

  Revision 1.62  2004/03/10 13:28:25  midas
  mscb_init returns device name

  Revision 1.61  2004/03/10 12:41:22  midas
  *** empty log message ***

  Revision 1.60  2004/03/10 10:28:47  midas
  Implemented test block write for speed tests

  Revision 1.59  2004/03/09 15:37:09  midas
  Fixed problems with small strings

  Revision 1.58  2004/03/09 15:02:26  midas
  Changed indentation

  Revision 1.57  2004/03/09 14:19:32  midas
  Fixed problems with write/read of strings

  Revision 1.56  2004/03/09 11:56:04  midas
  Mention hotplug script

  Revision 1.55  2004/03/05 19:40:01  midas
  Better usb error reporting

  Revision 1.54  2004/03/05 16:29:41  midas
  Fixed compiler warnings

  Revision 1.53  2004/03/05 14:00:01  midas
  Fixed bug with string transfer

  Revision 1.52  2004/03/04 16:10:49  midas
  Fixed compiler warning

  Revision 1.51  2004/03/04 15:29:16  midas
  Added USB support

  Revision 1.50  2004/01/07 12:56:15  midas
  Chaned line length

  Revision 1.49  2004/01/07 12:52:23  midas
  Changed indentation

  Revision 1.48  2003/10/03 14:08:07  midas
  Added locking parameter to mscb_addr

  Revision 1.47  2003/10/01 17:50:46  midas
  Scan every 100's and 1000's address

  Revision 1.46  2003/09/30 08:03:41  midas
  Implemented multiple RPC connections

  Revision 1.45  2003/09/23 09:25:05  midas
  Improved error display

  Revision 1.44  2003/09/10 11:11:17  midas
  Better error report on upload failure

  Revision 1.43  2003/09/09 14:43:22  midas
  Added unit farad

  Revision 1.42  2003/07/17 13:56:04  midas
  Fixed bug when outputting signed 16-bit data

  Revision 1.41  2003/06/26 11:56:13  midas
  Use retry in ping

  Revision 1.40  2003/06/11 14:13:33  midas
  Version 1.4.5

  Revision 1.39  2003/06/05 14:46:44  midas
  Added load/save facility

  Revision 1.38  2003/06/05 12:35:26  midas
  Display 8-bit signed values correctly

  Revision 1.37  2003/05/12 13:49:17  midas
  Added address check for SET_ADDR command

  Revision 1.36  2003/05/12 10:30:45  midas
  Fixed name collisions with midas library

  Revision 1.35  2003/04/03 08:21:44  midas
  Added submaster check

  Revision 1.34  2003/03/25 09:42:57  midas
  Added debugging facility

  Revision 1.33  2003/03/19 16:35:03  midas
  Eliminated configuration parameters

  Revision 1.32  2003/03/06 16:08:50  midas
  Protocol version 1.3 (change node name)

  Revision 1.31  2003/03/05 15:58:36  midas
  Added bit output

  Revision 1.30  2003/02/27 10:43:33  midas
  Changed 'address' command to non-acknowledge

  Revision 1.29  2003/02/18 11:21:12  midas
  Added repeat scan mode

  Revision 1.28  2003/01/30 08:34:06  midas
  Added [cf] flags to echo

  Revision 1.27  2003/01/24 13:45:12  midas
  Fixed bug in argument analysis

  Revision 1.26  2003/01/15 15:51:11  midas
  Moved error explanation (-1 could be DirectIO)

  Revision 1.25  2003/01/15 13:15:20  midas
  Added hints for problem solving

  Revision 1.24  2002/11/29 08:02:52  midas
  Fixed linux warnings

  Revision 1.23  2002/11/28 14:44:09  midas
  Removed SIZE_XBIT

  Revision 1.22  2002/11/28 13:04:36  midas
  Implemented protocol version 1.2 (echo, read_channels)

  Revision 1.21  2002/11/27 15:40:05  midas
  Added version, fixed few bugs

  Revision 1.20  2002/11/22 15:43:30  midas
  Ouput cycle number in test mode

  Revision 1.19  2002/11/20 12:01:23  midas
  Added host to mscb_init

  Revision 1.18  2002/11/06 16:47:37  midas
  Address only needs 'a'

  Revision 1.17  2002/11/06 16:46:29  midas
  Keep address on reboot

  Revision 1.16  2002/11/06 14:01:20  midas
  Fixed small bugs

  Revision 1.15  2002/10/28 14:26:30  midas
  Changes from Japan

  Revision 1.14  2002/10/22 15:04:34  midas
  Added repeat readout mode

  Revision 1.13  2002/10/09 15:48:13  midas
  Fixed bug with download

  Revision 1.12  2002/10/09 11:06:46  midas
  Protocol version 1.1

  Revision 1.11  2002/10/07 15:16:32  midas
  Added upgrade facility

  Revision 1.10  2002/10/03 15:30:40  midas
  Added linux support

  Revision 1.9  2002/09/27 11:09:08  midas
  Modified test mode

  Revision 1.8  2002/08/12 12:10:30  midas
  Added reset command

  Revision 1.7  2002/07/10 09:51:25  midas
  Introduced mscb_flash()

  Revision 1.6  2002/07/08 15:00:40  midas
  Added reboot command

  Revision 1.5  2001/10/31 11:15:17  midas
  Added IO check function

  Revision 1.4  2001/09/04 07:33:15  midas
  Added test function

  Revision 1.3  2001/08/31 12:23:50  midas
  Added mutex protection

  Revision 1.2  2001/08/31 11:35:20  midas
  Added "wp" command in msc.c, changed parport to device in mscb.c

  Revision 1.1  2001/08/31 11:05:50  midas
  Initial revision

\********************************************************************/

#ifdef _MSC_VER

#include <windows.h>
#include <conio.h>
#include <io.h>

#elif defined(OS_UNIX)

#define O_BINARY 0

#include <unistd.h>
#include <ctype.h>

#endif

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <time.h>
#include "mscb.h"
#include "rpc.h"

/*------------------------------------------------------------------*/

typedef struct {
   unsigned char id;
   char name[32];
} NAME_TABLE;

NAME_TABLE prefix_table[] = {
   {PRFX_PICO, "pico",},
   {PRFX_NANO, "nano",},
   {PRFX_MICRO, "micro",},
   {PRFX_MILLI, "milli",},
   {PRFX_NONE, "",},
   {PRFX_KILO, "kilo",},
   {PRFX_MEGA, "mega",},
   {PRFX_GIGA, "giga",},
   {PRFX_TERA, "tera",},
   {0}
};

NAME_TABLE unit_table[] = {

   {UNIT_METER, "meter",},
   {UNIT_GRAM, "gram",},
   {UNIT_SECOND, "second",},
   {UNIT_MINUTE, "minute",},
   {UNIT_HOUR, "hour",},
   {UNIT_AMPERE, "ampere",},
   {UNIT_KELVIN, "kelvin",},
   {UNIT_CELSIUS, "deg. celsius",},
   {UNIT_FARENHEIT, "deg. farenheit",},

   {UNIT_HERTZ, "hertz",},
   {UNIT_PASCAL, "pascal",},
   {UNIT_BAR, "bar",},
   {UNIT_WATT, "watt",},
   {UNIT_VOLT, "volt",},
   {UNIT_OHM, "ohm",},
   {UNIT_TESLA, "tesls",},
   {UNIT_LITERPERSEC, "liter/sec",},
   {UNIT_RPM, "RPM",},
   {UNIT_FARAD, "farad",},

   {UNIT_BOOLEAN, "boolean",},
   {UNIT_BYTE, "byte",},
   {UNIT_WORD, "word",},
   {UNIT_DWORD, "dword",},
   {UNIT_ASCII, "ascii",},
   {UNIT_STRING, "string",},
   {UNIT_BAUD, "baud",},

   {UNIT_PERCENT, "percent",},
   {UNIT_PPM, "RPM",},
   {UNIT_COUNT, "counts",},
   {UNIT_FACTOR, "factor",},
   {0}
};

/*------------------------------------------------------------------*/

void print_help()
{
   puts("Available commands:\n");
   puts("addr <addr>                Set address");
   puts("debug 0/1                  Turn debuggin off/on on node (LCD output)");
   puts("echo [fc]                  Perform echo test [fast,continuous]");
   puts("flash                      Flash parameters into EEPROM");
   puts("gaddr <addr>               Set group address");
   puts("info                       Retrive node info");
   puts("load <file>                Load node variables");
   puts("ping <addr> [r]            Ping node and set address [repeat mode]");
   puts("read <index> [r]           Read node variable [repeat mode]");
   puts("reboot                     Reboot addressed node");
   puts("reset                      Reboot whole MSCB system");
   puts("sa <addr> <gaddr>          Set node and group address of addressed node");
   puts("save <file>                Save current node variables");
   puts("scan [r]                   Scan bus for nodes [repeat mode]");
   puts("sn <name>                  Set node name (up to 16 characters)");
   puts("terminal                   Enter teminal mode for SCS-210");
   puts("upload <hex-file> [debug]  Upload new firmware to node [with debug info]");
   puts("version                    Display version number");
   puts("write <index> <value> [r]  Write node variable");

   puts("");
}

/*------------------------------------------------------------------*/

void print_channel_str(int index, MSCB_INFO_VAR * info_chn, void *pdata, int verbose, char *line)
{
   char str[80];
   int i, data;

   line[0] = 0;
   if (verbose) {
      memset(str, 0, sizeof(str));
      strncpy(str, info_chn->name, 8);
      for (i = strlen(str); i < 9; i++)
         str[i] = ' ';
      sprintf(line + strlen(line), "%3d: %s ", index, str);
   }

   if (info_chn->unit == UNIT_STRING) {
      memset(str, 0, sizeof(str));
      strncpy(str, pdata, info_chn->width);
      sprintf(line + strlen(line), "STR%02d    \"", info_chn->width);
      for(i=0 ; i<(int)strlen(str); i++)
         switch (str[i]) {
         case 1: strcat(line, "\\001"); break;
         case 2: strcat(line, "\\002"); break;
         case 9: strcat(line, "\\t"); break;
         case 10: strcat(line, "\\n"); break;
         case 13: strcat(line, "\\r"); break;
         default: line[strlen(line)+1] = 0; line[strlen(line)] = str[i]; break;
      }
      strcat(line, "\"");
   } else {
      data = *((int *) pdata);
      switch (info_chn->width) {
      case 0:
         printf(" 0bit");
         break;

      case 1:
         if (info_chn->flags & MSCBF_SIGNED)
            sprintf(line + strlen(line), " 8bit S %15d (0x%02X/", (char) data, data);
         else
            sprintf(line + strlen(line), " 8bit U %15u (0x%02X/", data, data);
         for (i = 0; i < 8; i++)
            if (data & (0x80 >> i))
               sprintf(line + strlen(line), "1");
            else
               sprintf(line + strlen(line), "0");
         sprintf(line + strlen(line), ")");
         break;

      case 2:
         if (info_chn->flags & MSCBF_SIGNED)
            sprintf(line + strlen(line), "16bit S %15d (0x%04X)", (short) data, data);
         else
            sprintf(line + strlen(line), "16bit U %15u (0x%04X)", data, data);
         break;

      case 3:
         if (info_chn->flags & MSCBF_SIGNED)
            sprintf(line + strlen(line), "24bit S %15ld (0x%06X)", (long) data, data);
         else
            sprintf(line + strlen(line), "24bit U %15u (0x%06X)", data, data);
         break;

      case 4:
         if (info_chn->flags & MSCBF_FLOAT)
            sprintf(line + strlen(line), "32bit F %15.6lg", *((float *) &data));
         else {
            if (info_chn->flags & MSCBF_SIGNED)
               sprintf(line + strlen(line), "32bit S %15d (0x%08X)", data, data);
            else
               sprintf(line + strlen(line), "32bit U %15u (0x%08X)", data, data);
         }
         break;
      }
   }

   sprintf(line + strlen(line), " ");

   /* evaluate prfix */
   if (info_chn->prefix) {
      for (i = 0; prefix_table[i].id; i++)
         if (prefix_table[i].id == info_chn->prefix)
            break;
      if (prefix_table[i].id)
         sprintf(line + strlen(line), prefix_table[i].name);
   }

   /* evaluate unit */
   if (info_chn->unit && info_chn->unit != UNIT_STRING) {
      for (i = 0; unit_table[i].id; i++)
         if (unit_table[i].id == info_chn->unit)
            break;
      if (unit_table[i].id)
         sprintf(line + strlen(line), unit_table[i].name);
   }

   if (verbose)
      sprintf(line + strlen(line), "\n");
   else
      sprintf(line + strlen(line), "                    \r");
}

void print_channel(int index, MSCB_INFO_VAR * info_chn, void *pdata, int verbose)
{
   char str[256];

   print_channel_str(index, info_chn, pdata, verbose, str);
   fputs(str, stdout);
}

/*------------------------------------------------------------------*/

int match(char *str, char *cmd)
{
   int i;

   if (str[0] == '\r' || str[0] == '\n')
      return 0;

   for (i = 0; i < (int) strlen(str); i++) {
      if (toupper(str[i]) != toupper(cmd[i]) && str[i] != '\r' && str[i] != '\n')
         return 0;
   }

   return 1;
}

/*------------------------------------------------------------------*/

void cmd_loop(int fd, char *cmd, int adr)
{
   int i, j, fh, status, size, nparam, addr, gaddr, current_addr, current_group,
      majv, minv;
   unsigned int data;
   unsigned char c;
   float value;
   char str[256], line[256], dbuf[100*1024];
   char param[10][100];
   char *pc, *p, *buffer;
   char name[256], chn_name[256][9];
   char lib[32], prot[32];
   FILE *f, *cmd_file = NULL;
   MSCB_INFO info;
   MSCB_INFO_VAR info_var;
   time_t start, now;


   /* open command file */
   if (cmd[0] == '@') {
      cmd_file = fopen(cmd + 1, "r");
      if (cmd_file == NULL) {
         printf("Command file %s not found.\n", cmd + 1);
         return;
      }
   }

   if (cmd[0])
      /* assumed some node correctly addressed in command mode */
      current_addr = 0;
   else
      current_addr = -1;

   current_group = -1;

   if (adr)
      current_addr = adr;

   do {
      /* print prompt */
      if (!cmd[0]) {
         if (current_addr >= 0)
            printf("node%d(0x%X)> ", current_addr, current_addr);
         else if (current_group >= 0)
            printf("group%d> ", current_group);
         else
            printf("> ");
         memset(line, 0, sizeof(line));
         fgets(line, sizeof(line), stdin);
      } else if (cmd[0] != '@')
         strcpy(line, cmd);
      else {
         memset(line, 0, sizeof(line));
         fgets(line, sizeof(line), cmd_file);

         if (line[0] == 0)
            break;

         /* cut off CR */
         while (strlen(line) > 0 && line[strlen(line) - 1] == '\n')
            line[strlen(line) - 1] = 0;

         if (line[0] == 0)
            continue;
      }

      /* analyze line */
      nparam = 0;
      pc = line;
      while (*pc == ' ')
         pc++;

      memset(param, 0, sizeof(param));
      do {
         if (*pc == '"') {
            pc++;
            for (i = 0; *pc && *pc != '"'; i++)
               param[nparam][i] = *pc++;
            if (*pc)
               pc++;
         } else if (*pc == '\'') {
            pc++;
            for (i = 0; *pc && *pc != '\''; i++)
               param[nparam][i] = *pc++;
            if (*pc)
               pc++;
         } else if (*pc == '`') {
            pc++;
            for (i = 0; *pc && *pc != '`'; i++)
               param[nparam][i] = *pc++;
            if (*pc)
               pc++;
         } else
            for (i = 0; *pc && *pc != ' '; i++)
               param[nparam][i] = *pc++;
         param[nparam][i] = 0;
         while (*pc == ' ' || *pc == '\r' || *pc == '\n')
            pc++;
         nparam++;
      } while (*pc);

      /* help ---------- */
      if ((param[0][0] == 'h' && param[0][1] == 'e') || param[0][0] == '?')
         print_help();

      /* version ---------- */
      else if (match(param[0], "version")) {
         mscb_get_version(lib, prot);
         printf("MSCB library version  : %s\n", lib);
         printf("MSCB protocol version : %s\n", prot);
      }

      /* scan ---------- */
      else if (match(param[0], "scan")) {
         do {
            /* decimal search */
            for (i = 0; i < 0x10000; i++) {
               printf("Test address %d    \r", i);
               fflush(stdout);

               status = mscb_ping(fd, i);

               if (status == MSCB_SUCCESS) {
                  status = mscb_info(fd, i, &info);
                  strncpy(str, info.node_name, sizeof(info.node_name));
                  str[16] = 0;

                  if (status == MSCB_SUCCESS) {
                     printf
                         ("Found node \"%s\", node addr. %d (0x%04X), group addr. %d (0x%04X)      \n",
                          str, i, i, info.group_address, info.group_address);
                  }
               } else if (status == MSCB_SUBM_ERROR) {
                  printf("Error: Submaster not responding\n");
                  break;
               }

               if (i && i % 1000 == 0 && status != MSCB_SUCCESS) {
                  i += 999;
                  if (i == 64999)
                     i = 0xFFFE;
               }

               if (i && i % 100 == 0 && status != MSCB_SUCCESS) {
                  i += 99;
                  if (i == 65499)
                     i = 0xFFFE;
               }

               if (kbhit())
                  break;
            }

            /* hexadecimal search */
            for (i = 0x100; i < 0x10000; i++) {
               printf("Test address 0x%x    \r", i);
               fflush(stdout);

               status = mscb_ping(fd, i);

               if (status == MSCB_SUCCESS) {
                  status = mscb_info(fd, i, &info);
                  strncpy(str, info.node_name, sizeof(info.node_name));
                  str[16] = 0;

                  if (status == MSCB_SUCCESS) {
                     printf
                         ("Found node \"%s\", node addr. %d (0x%04X), group addr. %d (0x%04X)      \n",
                          str, i, i, info.group_address, info.group_address);
                  }
               } else if (status == MSCB_SUBM_ERROR) {
                  printf("Error: Submaster not responding\n");
                  break;
               }

               if (i && (i & 0xFF) == 0 && status != MSCB_SUCCESS) {
                  i += 0xFF;
               }

               if (kbhit())
                  break;
            }

         } while (param[1][0] == 'r' && !kbhit());

         while (kbhit())
            getch();

         printf("                       \n");
      }

      /* info ---------- */
      else if (match(param[0], "info")) {
         if (current_addr < 0)
            printf("You must first address an individual node\n");
         else {
            status = mscb_info(fd, current_addr, &info);
            if (status == MSCB_CRC_ERROR)
               puts("CRC Error");
            else if (status != MSCB_SUCCESS)
               puts("No response from node");
            else {
               strncpy(str, info.node_name, sizeof(info.node_name));
               str[16] = 0;
               printf("Node name:        %s\n", str);
               printf("Node address:     %d (0x%X)\n", info.node_address, info.node_address);
               printf("Group address:    %d (0x%X)\n", info.group_address, info.group_address);
               printf("Protocol version: %d.%d\n", info.protocol_version / 16, info.protocol_version % 16);
               printf("Watchdog resets:  %d\n", info.watchdog_resets);

               printf("\nVariables:\n");
               for (i = 0; i < info.n_variables; i++) {
                  mscb_info_variable(fd, current_addr, i, &info_var);
                  size = info_var.width;
                  memset(dbuf, 0, sizeof(dbuf));
                  mscb_read(fd, current_addr, (unsigned char) i, dbuf, &size);

                  print_channel(i, &info_var, dbuf, 1);
               }

               mscb_get_version(lib, prot);
               majv = atoi(prot);
               if (strchr(prot, '.'))
                  minv = atoi(strchr(prot, '.')+1);
               else
                  minv = 0;
               if (info.protocol_version / 16 != majv || info.protocol_version % 16 != minv) {
                  printf("\nWARNING: Protocol version on node (%d.%d) differs from local version (%s).\n",
                     info.protocol_version / 16, info.protocol_version % 16, prot);
                  printf("Problems may arise communicating with this node.\n\n");
               }
            }
         }
      }

      /* ping ---------- */
      else if (match(param[0], "ping")) {
         if (!param[1][0])
            puts("Please specify node address");
         else {
            if (param[1][1] == 'x')
               sscanf(param[1] + 2, "%x", &addr);
            else
               addr = atoi(param[1]);

            do {
               status = mscb_addr(fd, MCMD_PING16, addr, 1, 1);
               if (status != MSCB_SUCCESS) {
                  if (status == MSCB_MUTEX)
                     printf("MSCB used by other process\n");
                  else if (status == MSCB_SUBM_ERROR)
                     printf("Error: Submaster not responding\n");
                  else
                     printf("Node %d does not respond\n", addr);
                  current_addr = -1;
                  current_group = -1;
               } else {
                  printf("Node %d addressed\n", addr);
                  current_addr = addr;
                  current_group = -1;
               }

               if (param[2][0] == 'r' && !kbhit())
                  Sleep(1000);

            } while (param[2][0] == 'r' && !kbhit());
         }
      }

      /* address ---------- */
      else if (match(param[0], "addr")) {
         if (!param[1][0])
            puts("Please specify node address");
         else {
            if (param[1][1] == 'x')
               sscanf(param[1] + 2, "%x", &addr);
            else
               addr = atoi(param[1]);

            mscb_addr(fd, MCMD_ADDR_NODE16, addr, 1, 1);
            current_addr = addr;
            current_group = -1;
         }
      }

      /* ga, set group address ---------- */
      else if (match(param[0], "ga")) {
         if (!param[1][0])
            puts("Please specify node address");
         else {
            if (param[1][1] == 'x')
               sscanf(param[1] + 2, "%x", &addr);
            else
               addr = atoi(param[1]);

            current_addr = -1;
            current_group = addr;
         }
      }

      /* sa, set node address ---------- */
      else if (match(param[0], "sa")) {
         if (current_addr < 0)
            printf("You must first address an individual node\n");
         else {
            if (!param[1][0] || !param[2][0])
               puts("Please specify node and group address");
            else {
               if (param[1][1] == 'x')
                  sscanf(param[1] + 2, "%x", &addr);
               else
                  addr = atoi(param[1]);

               if (param[2][1] == 'x')
                  sscanf(param[2] + 2, "%x", &gaddr);
               else
                  gaddr = atoi(param[2]);

               status = mscb_set_addr(fd, current_addr, addr, gaddr);
               if (status == MSCB_ADDR_EXISTS)
                  printf("Error: Address %d exists already on this network\n", addr);
               else
                  current_addr = addr;
            }
         }
      }

      /* sn, set node name ---------- */
      else if (match(param[0], "sn")) {
         if (current_addr < 0)
            printf("You must first address an individual node\n");
         else {
            if (!param[1][0])
               puts("Please specify node name");
            else {
               strcpy(str, param[1]);
               while (strlen(str) > 0 && (str[strlen(str) - 1] == '\r' || str[strlen(str) - 1] == '\n'))
                  str[strlen(str) - 1] = 0;

               mscb_set_name(fd, current_addr, str);
            }
         }
      }

      /* debug ---------- */
      else if (match(param[0], "debug")) {
         if (current_addr < 0)
            printf("You must first address an individual node\n");
         else {
            if (!param[1][0])
               puts("Please specify 0/1");
            else {
               i = atoi(param[1]);

               /* write CSR register */
               c = i ? CSR_DEBUG : 0;
               mscb_write(fd, current_addr, 0xFF, &c, 1);
            }
         }
      }

      /* write ---------- */
      else if (match(param[0], "write")) {
         if (current_addr < 0 && current_group < 0)
            printf("You must first address a node or a group\n");
         else {
            if (!param[1][0] || !param[2][0])
               puts("Please specify channel number and data");
            else {
               addr = atoi(param[1]);

               if (current_addr >= 0) {
                  mscb_info_variable(fd, current_addr, addr, &info_var);

                  if (info_var.unit == UNIT_STRING) {
                     memset(str, 0, sizeof(str));
                     strncpy(str, param[2], info_var.width);
                     if (strlen(str) > 0 && str[strlen(str) - 1] == '\n')
                        str[strlen(str) - 1] = 0;

                     status = mscb_write(fd, current_addr, (unsigned char) addr, str, strlen(str)+1);
                  } else {
                     if (info_var.flags & MSCBF_FLOAT) {
                        value = (float) atof(param[2]);
                        memcpy(&data, &value, sizeof(float));
                     } else {
                        if (param[2][1] == 'x')
                           sscanf(param[2] + 2, "%x", &data);
                        else
                           data = atoi(param[2]);
                     }

                     do {

                        status = mscb_write(fd, current_addr, (unsigned char) addr, &data, info_var.width);
                        Sleep(100);

                     } while (param[3][0] && !kbhit());

                  }
               } else if (current_group >= 0) {
                  if (param[2][1] == 'x')
                     sscanf(param[2] + 2, "%x", &data);
                  else
                     data = atoi(param[2]);

                  status = mscb_write_group(fd, current_group, (unsigned char) addr, &data, 4);
               }

               if (status != MSCB_SUCCESS)
                  printf("Error: %d\n", status);
            }
         }
      }

      /* read ---------- */
      else if (match(param[0], "read")) {
         if (current_addr < 0)
            printf("You must first address an individual node\n");
         else {
            if (!param[1][0])
               puts("Please specify channel number");
            else {
               addr = atoi(param[1]);

               status = mscb_info_variable(fd, current_addr, addr, &info_var);

               if (status == MSCB_CRC_ERROR)
                  puts("CRC Error");
               else if (status != MSCB_SUCCESS)
                  puts("Timeout or invalid channel number");
               else {
                  do {
                     memset(dbuf, 0, sizeof(dbuf));
                     size = info_var.width;
                     status = mscb_read(fd, current_addr, (unsigned char) addr, dbuf, &size);
                     if (status != MSCB_SUCCESS)
                        printf("Error: %d\n", status);
                     else if (size != info_var.width)
                        printf("Error: Received %d bytes instead of %d", size, info_var.width);
                     else
                        print_channel(addr, &info_var, dbuf, 0);

                     Sleep(10);
                  } while (param[2][0] && !kbhit());

                  while (kbhit())
                     getch();

                  printf("\n");
               }
            }
         }
      }

      /* save ---------- */
      else if (match(param[0], "save")) {
         if (current_addr < 0)
            printf("You must first address an individual node\n");
         else {
            if (param[1][0] == 0) {
               printf("Enter file name: ");
               fgets(str, sizeof(str), stdin);
            } else
               strcpy(str, param[1]);

            if (str[strlen(str) - 1] == '\n')
               str[strlen(str) - 1] = 0;

            f = fopen(str, "wt");
            if (f == NULL) {
               printf("Cannot open file %s\n", str);
            } else {
               status = mscb_info(fd, current_addr, &info);
               if (status == MSCB_CRC_ERROR)
                  puts("CRC Error");
               else if (status != MSCB_SUCCESS)
                  puts("No response from node");
               else {
                  strncpy(str, info.node_name, sizeof(info.node_name));
                  str[16] = 0;
                  fprintf(f, "Node name:        %s\n", str);
                  fprintf(f, "Node address:     %d (0x%X)\n", info.node_address, info.node_address);
                  fprintf(f, "Group address:    %d (0x%X)\n", info.group_address, info.group_address);
                  fprintf(f, "Protocol version: %d.%d\n",
                          info.protocol_version / 16, info.protocol_version % 16);
                  fprintf(f, "Watchdog resets:  %d\n", info.watchdog_resets);

                  fprintf(f, "\nVariables:\n");
                  for (i = 0; i < info.n_variables; i++) {
                     mscb_info_variable(fd, current_addr, i, &info_var);
                     size = sizeof(data);
                     mscb_read(fd, current_addr, (unsigned char) i, dbuf, &size);

                     print_channel_str(i, &info_var, dbuf, 1, str);
                     fprintf(f, str);
                  }
               }

               fclose(f);
            }
         }
      }

      /* load ---------- */
      else if (match(param[0], "load")) {
         if (current_addr < 0)
            printf("You must first address an individual node\n");
         else {
            if (param[1][0] == 0) {
               printf("Enter file name: ");
               fgets(str, sizeof(str), stdin);
            } else
               strcpy(str, param[1]);

            if (str[strlen(str) - 1] == '\n')
               str[strlen(str) - 1] = 0;

            /* get list of channel names */
            status = mscb_info(fd, current_addr, &info);
            if (status == MSCB_CRC_ERROR)
               puts("CRC Error");
            else if (status != MSCB_SUCCESS)
               puts("No response from node");
            else {
               memset(chn_name, 0, sizeof(chn_name));
               for (i = 0; i < info.n_variables; i++) {
                  mscb_info_variable(fd, current_addr, i, &info_var);
                  strcpy(chn_name[i], info_var.name);
               }

               f = fopen(str, "rt");
               if (f) {
                  do {
                     line[0] = 0;
                     fgets(line, sizeof(line), f);

                     if (line[3] == ':' && line[4] == ' ') {
                        /* variable found, extract it */
                        for (i = 0; i < 8 && line[i + 5] != ' '; i++)
                           name[i] = line[i + 5];
                        name[i] = 0;

                        p = line + 23;
                        while (*p && *p == ' ')
                           p++;

                        /* search for channel with same name */
                        for (i = 0; chn_name[i][0]; i++)
                           if (strcmp(chn_name[i], name) == 0) {
                              mscb_info_variable(fd, current_addr, i, &info_var);

                              if (info_var.unit == UNIT_STRING) {
                                 memset(str, 0, sizeof(str));
                                 strncpy(str, p, info_var.width);
                                 if (strlen(str) > 0 && str[strlen(str) - 1] == '\n')
                                    str[strlen(str) - 1] = 0;

                                 status =
                                     mscb_write(fd, current_addr, (unsigned char) i, str, info_var.width);
                              } else {
                                 if (info_var.flags & MSCBF_FLOAT) {
                                    value = (float) atof(p);
                                    memcpy(&data, &value, sizeof(float));
                                 } else {
                                    data = atoi(p);
                                 }

                                 status =
                                     mscb_write(fd, current_addr, (unsigned char) i, &data, info_var.width);

                                 /* blank padding */
                                 for (j = strlen(name); j < 8; j++)
                                    name[j] = ' ';
                                 name[j] = 0;

                                 printf("%03d: %s %s", i, name, p);
                              }

                              break;
                           }
                     }
                  } while (!feof(f));

                  fclose(f);
               } else
                  printf("File \"%s\" not found\n", str);
            }
         }
      }

      /* terminal ---------- */
      else if (match(param[0], "terminal")) {
         int d, status;

         puts("Exit with <ESC>\n");

         /* switch SCS-210 into terminal mode */
         c = 0;
         status = mscb_write(fd, current_addr, 0, &c, 1);

         do {
            if (kbhit()) {
               c = getch();
               putchar(c);
               status = mscb_write(fd, current_addr, 0, &c, 1);
               if (status != MSCB_SUCCESS) {
                  printf("\nError: %d\n", status);
                  break;
               }

               if (c == '\r') {
                  putchar('\n');
                  c = '\n';
                  mscb_write(fd, current_addr, 0, &c, 1);
               }

            }

            size = sizeof(d);
            mscb_read(fd, current_addr, 0, &d, &size);
            if (d > 0)
               putchar(d);

            Sleep(10);
         } while (c != 27);

         puts("\n");
         while (kbhit())
            getch();
      }

      /* flash ---------- */
      else if (match(param[0], "flash")) {
         if (current_addr < 0)
            printf("You must first address an individual node\n");
         else {
            status = mscb_flash(fd, current_addr);

            if (status != MSCB_SUCCESS)
               printf("Error: %d\n", status);
         }
      }

      /* upload ---------- */
      else if (match(param[0], "upload")) {
         if (current_addr < 0)
            printf("You must first address an individual node\n");
         else {
            if (param[1][0] == 0) {
               printf("Enter name of HEX-file: ");
               fgets(str, sizeof(str), stdin);
            } else
               strcpy(str, param[1]);

            if (str[strlen(str) - 1] == '\n')
               str[strlen(str) - 1] = 0;

            fh = open(str, O_RDONLY | O_BINARY);
            if (fh > 0) {
               if (param[2][0])
                  printf("Found file %s\n", str);
               size = lseek(fh, 0, SEEK_END) + 1;
               lseek(fh, 0, SEEK_SET);
               buffer = malloc(size);
               memset(buffer, 0, size);
               read(fh, buffer, size - 1);
               close(fh);
               status = mscb_upload(fd, current_addr, buffer, size, param[2][0]);
               if (status == MSCB_FORMAT_ERROR)
                  printf("Syntax error in file \"%s\"\n", str);
               else if (status == MSCB_TIMEOUT)
                  printf("Node %d does not respond\n", current_addr);

               free(buffer);
            } else
               printf("File \"%s\" not found\n", str);
         }
      }

      /* reboot ---------- */
      else if (match(param[0], "reboot")) {
         mscb_reboot(fd, current_addr);
      }

      /* reset ---------- */
      else if (match(param[0], "reset")) {
         mscb_reset(fd);
      }

      /* echo test ---------- */
      else if (match(param[0], "echo")) {
         unsigned char d1, d2;
         int i, status;

         if (current_addr < 0)
            printf("You must first address an individual node\n");
         else {
            d1 = i = 0;
            /* first echo with adressing */
            status = mscb_echo(fd, current_addr, d1, &d2);
            while (!kbhit()) {
               d1 = rand();

               /* following echos without adressing */
               status = mscb_echo(fd, 0, d1, &d2);

               if (d2 != d1) {
                  printf("%d\nReceived: %02X, should be %02X, status = %d\n", i, d2, d1, status);

                  if (param[1][0] != 'c' && param[1][1] != 'c')
                     break;
               }

               i++;
               if (i % 100 == 0) {
                  printf("%d\r", i);
                  fflush(stdout);
               }

               if (param[1][0] != 'f' && param[1][1] != 'c')
                  Sleep(10);
            }
            printf("%d\n", i);
            while (kbhit())
               getch();
         }
      }

      /* block transfer test ---------- */
      else if (match(param[0], "block")) {
         int i, status;

         if (current_addr < 0)
            printf("You must first address an individual node\n");
         else {
            i = 0;
            time(&start);
            while (!kbhit()) {
               status = mscb_write_block(fd, current_addr, 0, dbuf, 1024);
               i += 1024;

               time(&now);
               if (now > start+1) {
                  start = now;
                  printf("%4.2lf kB/sec\r", i/1024.0);
                  i = 0;
                  fflush(stdout);
               }
            }
            printf("%d\n", i);
            while (kbhit())
               getch();
         }
      }

      /* test1 ---------- */
      else if (match(param[0], "t1")) {
         data = atoi(param[1]);
         mscb_link(fd, current_addr, 8, &data, 4);

         printf("Data: %d\n", data);
      }

      /* exit/quit ---------- */
      else if (match(param[0], "exit") || match(param[0], "quit"))
         break;

      else if (param[0][0] == 0);

      else if (param[0][0] == '\n');

      else
         printf("Unknown command %s %s %s\n", param[0], param[1], param[2]);

      /* exit after single command */
      if (cmd[0] && cmd[0] != '@')
         break;

   } while (1);
}

/*------------------------------------------------------------------*/

int main(int argc, char *argv[])
{
   int i, fd, adr, server;
   char cmd[256], device[256];
   int debug, check_io;

   cmd[0] = 0;
   adr = 0;

   debug = check_io = server = 0;
   device[0] = 0;

   /* parse command line parameters */
   for (i = 1; i < argc; i++) {
      if (argv[i][0] == '-' && argv[i][1] == 'v')
         debug = 1;
      else if (argv[i][0] == '-' && argv[i][1] == 'i')
         check_io = 1;
      else if (argv[i][0] == '-' && argv[i][1] == 's')
         server = 1;
      else if (argv[i][0] == '-') {
         if (i + 1 >= argc || argv[i + 1][0] == '-')
            goto usage;
         else if (argv[i][1] == 'd')
            strcpy(device, argv[++i]);
         else if (argv[i][1] == 'a')
            adr = atoi(argv[++i]);
         else if (argv[i][1] == 'c') {
            if (strlen(argv[i]) >= 256) {
               printf("error: command line too long (>256).\n");
               return 0;
            }
            strcpy(cmd, argv[++i]);
         } else {
          usage:
            printf("usage: msc [-d host:device] [-a addr] [-c Command] [-c @CommandFile] [-v] [-i]\n\n");
            printf("       -d     device, usually \"%s\" for parallel port,\n", device);
            printf("              or 0x378 for direct parallel port address,\n");
            printf("              or \"<host>:%s\" for RPC connection\n", device);
            printf("       -s     Start RPC server\n");
            printf("       -a     Address node before executing command\n");
            printf("       -c     Execute command immediately\n");
            printf("       -v     Produce verbose debugging output\n\n");
            printf("       -i     Check IO pins of port\n\n");
            printf("For a list of valid commands start msc interactively\n");
            printf("and type \"help\".\n");
            return 0;
         }
      }
   }

   if (server) {
      mrpc_server_loop();
      return 0;
   }

   if (check_io) {
      mscb_check(device, sizeof(device));
      return 0;
   }

   /* open port */
   fd = mscb_init(device, sizeof(device), debug ? 1 : 0);
   if (fd < 0) {
      if (fd == -2) {
         printf("No MSCB submaster present at port \"%s\"\n", device);

         puts("\nMake sure that");
         puts("");
         puts("o The MSCB submaster is correctly connected to the parallel port");
         puts("o The MSCB submaster has power (green LED on)");
         puts("");
         puts("If this is a new installation, check in the PC BIOS that");
         puts("");
         puts("o LPT1 is in bi-directional (or EPP) mode, NOT ECP mode");
         puts("o LPT1 is configured to address 0x378 (hex)");
         puts("o If BIOS seems ok, you can check the pin I/O with a logic probe");
         puts("  and the \"msc -i\" command which toggles the pins of the parallel port");
         puts("o If the port is configured to another address than 0x378, use");
         puts("  \"msc -d 0xabc\" with \"0xabc\" the correct address");
      } else if (fd == -3) {
         printf("MSCB system locked by other process\n");
      } else if (fd == -4) {
         printf("\nCannot communicate with MSCB submaster at %s\n", device);
         puts("Please disconnect and reconnect submaster\n");
      } else if (fd == -5) {
         printf("\nNo write access to MSCB submaster at %s\n", device);
         puts("Please install hotplug script \"/etc/hotplug/usb/scs_250\".\n");
      } else
         if (device[0])
            printf("Cannot connect to device \"%s\"\n", device);\
         else
            printf("No MSCB submaster found\n");

#ifdef _MSC_VER
      puts("\n-- hit any key to exit --");

      while (!kbhit()) 
         Sleep(100);
      while (kbhit())
         getch();
#endif

      return 0;
   }

   printf("Connected to submaster at %s\n", device);

   cmd_loop(fd, cmd, adr);

   mscb_exit(fd);

   return 0;
}
