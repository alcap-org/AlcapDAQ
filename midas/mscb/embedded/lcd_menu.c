/********************************************************************\

  Name:         lcd_menu.c
  Created by:   Stefan Ritt


  Contents:     Menu functions for SCS-1000 using LCD display
                and four buttons

  $Log: lcd_menu.c,v $
  Revision 1.1.1.1  2005/06/20 23:37:17  mucap
  Importing release 1.9.5 of the MIDAS source code as distributed by PSI.
  (Next, I'll commit our local customizations.)

  Revision 1.2  2004/09/25 01:14:54  midas
  Started implementing slave port on SCS-1000

  Revision 1.1  2004/07/20 16:05:32  midas
  Initial revision

\********************************************************************/

#include <stdio.h>
#include <string.h>
#include "mscb.h"

/*------------------------------------------------------------------*/

extern unsigned char application_display(bit init);
extern void user_write(unsigned char index) reentrant;
extern MSCB_INFO_VAR code variables[];
extern SYS_INFO sys_info;
extern bit flash_param, reboot;

extern bit b0, b1, b2, b3;     // buttons defined in scs-1000.c
extern unsigned char idata n_variables;

/*------------------------------------------------------------------*/

bit startup = 1;               // true for the first 3 seconds
bit in_menu = 0;               // false for application display
bit system_menu = 0;           // true if in system menu
bit enter_mode = 0;            // true if editing vars
unsigned char var_index = 0;   // variable to display

idata float f_var;

/*------------------------------------------------------------------*/

typedef struct {
   unsigned char id;
   char name[4];
} NAME_TABLE;

NAME_TABLE code prefix_table[] = {
   {PRFX_PICO, "p",},
   {PRFX_NANO, "n",},
   {PRFX_MICRO, "\344",},
   {PRFX_MILLI, "m",},
   {PRFX_NONE, "",},
   {PRFX_KILO, "k",},
   {PRFX_MEGA, "M",},
   {PRFX_GIGA, "G",},
   {PRFX_TERA, "T",},
   {0}
};

NAME_TABLE code unit_table[] = {

   {UNIT_METER, "m",},
   {UNIT_GRAM, "g",},
   {UNIT_SECOND, "s",},
   {UNIT_MINUTE, "m",},
   {UNIT_HOUR, "h",},
   {UNIT_AMPERE, "A",},
   {UNIT_KELVIN, "K",},
   {UNIT_CELSIUS, "\337C",},
   {UNIT_FARENHEIT, "\337F",},

   {UNIT_HERTZ, "Hz",},
   {UNIT_PASCAL, "Pa",},
   {UNIT_BAR, "ba",},
   {UNIT_WATT, "W",},
   {UNIT_VOLT, "V",},
   {UNIT_OHM, "\364",},
   {UNIT_TESLA, "T",},
   {UNIT_LITERPERSEC, "l/s",},
   {UNIT_RPM, "RPM",},
   {UNIT_FARAD, "F",},

   {UNIT_PERCENT, "%%",},
   {UNIT_PPM, "RPM",},
   {UNIT_COUNT, "cnt",},
   {UNIT_FACTOR, "x",},
   {0}
};

/*------------------------------------------------------------------*/

unsigned char n_sysvar = 4;

MSCB_INFO_VAR code sysvar[] = {

   { 2, UNIT_WORD, 0, 0, 0, "Node Adr", &sys_info.node_addr, 1, 0xFFFE, 1 },
   { 2, UNIT_WORD, 0, 0, 0, "Grp Adr",  &sys_info.group_addr, 1, 0xFFFE, 1 },

   { 0, UNIT_BOOLEAN, 0, 0, MSCBF_DATALESS, "Flash" },
   { 0, UNIT_BOOLEAN, 0, 0, MSCBF_DATALESS, "Reboot" },

};

/*------------------------------------------------------------------*/

void display_value(MSCB_INFO_VAR *pvar, void *pd)
{
   unsigned char i, unit_len;
   char idata str[10];
   
   lcd_goto(10, 0);

   if (pvar->unit == UNIT_STRING) {
      strncpy(str, (char *)pd, 8);
      puts(str);
   } else {

      str[0] = 0;

      /* evaluate prefix */
      if (pvar->prefix) {
         for (i = 0; prefix_table[i].id; i++)
            if (prefix_table[i].id == pvar->prefix)
               break;
         if (prefix_table[i].id)
            strcat(str, prefix_table[i].name);
      }
   
      /* evaluate unit */
      if (pvar->unit) {
         for (i = 0; unit_table[i].id; i++)
            if (unit_table[i].id == pvar->unit)
               break;
         if (unit_table[i].id)
            strcat(str, unit_table[i].name);
      }

      unit_len = strlen(str);

      switch (pvar->width) {
      case 0:
         puts("          ");
         break;

      case 1:
         if (pvar->flags & MSCBF_SIGNED)
            printf("%b*bd", 10-unit_len, *((char *) pd));
         else
            printf("%b*bu", 10-unit_len, *((unsigned char *) pd));
         break;

      case 2:
         if (pvar->flags & MSCBF_SIGNED)
            printf("%b*d", 10-unit_len, *((short *) pd));
         else
            printf("%b*u", 10-unit_len, *((unsigned short *) pd));
         break;

      case 4:
         if (pvar->flags & MSCBF_FLOAT)
            printf("%b*.3f", 10-unit_len, *((float *) pd));
         else {
            if (pvar->flags & MSCBF_SIGNED)
               printf("%b*ld", 10-unit_len, *((long *) pd));
            else
               printf("%b*lu", 10-unit_len, *((unsigned long *) pd));
         }
         break;
      }

      puts(str);
   }
}

/*------------------------------------------------------------------*/

void var_inc(MSCB_INFO_VAR *pvar, char sign)
{
   if (pvar->unit == UNIT_STRING)
      return;

   switch (pvar->width) {
   case 0:
      return;
      break;

   case 1:
      if (pvar->flags & MSCBF_SIGNED) {
         *((char *) &f_var) += pvar->delta * sign;
         if (*((char *) &f_var) < pvar->min)
            *((char *) &f_var) = pvar->min;
         if (*((char *) &f_var) > pvar->max)
            *((char *) &f_var) = pvar->max;
      } else {
         if (sign == -1 && *((unsigned char *) &f_var) == pvar->min)
            break;
         *((unsigned char *) &f_var) += pvar->delta * sign;
         if (*((unsigned char *) &f_var) < pvar->min)
            *((unsigned char *) &f_var) = pvar->min;
         if (*((unsigned char *) &f_var) > pvar->max)
            *((unsigned char *) &f_var) = pvar->max;
      }
      break;

   case 2:
      if (pvar->flags & MSCBF_SIGNED) {
         *((short *) &f_var) += pvar->delta * sign;
         if (*((short *) &f_var) < pvar->min)
            *((short *) &f_var) = pvar->min;
         if (*((short *) &f_var) > pvar->max)
            *((short *) &f_var) = pvar->max;
      } else {
         if (sign == -1 && *((unsigned short *) &f_var) == pvar->min)
            break;
         *((unsigned short *) &f_var) += pvar->delta * sign;
         if (*((unsigned short *) &f_var) < pvar->min)
            *((unsigned short *) &f_var) = pvar->min;
         if (*((unsigned short *) &f_var) > pvar->max)
            *((unsigned short *) &f_var) = pvar->max;
      }
      break;


   case 4:
      if (pvar->flags & MSCBF_FLOAT) {
         f_var += pvar->delta * sign;
         if (f_var < pvar->min)
            f_var = pvar->min;
         if (f_var > pvar->max)
            f_var = pvar->max;
      } else {
         if (pvar->flags & MSCBF_SIGNED) {
            *((long *) &f_var) += pvar->delta * sign;
            if (*((long *) &f_var) < pvar->min)
               *((long *) &f_var) = pvar->min;
            if (*((long *) &f_var) > pvar->max)
               *((long *) &f_var) = pvar->max;
         } else {
            if (sign == -1 && *((unsigned long *) &f_var) == pvar->min)
               break;
            *((unsigned long *) &f_var) += pvar->delta * sign;
            if (*((unsigned long *) &f_var) < pvar->min)
               *((unsigned long *) &f_var) = pvar->min;
            if (*((unsigned long *) &f_var) > pvar->max)
               *((unsigned long *) &f_var) = pvar->max;
         }
      }
      break;
   }
}

/*------------------------------------------------------------------*/

void display_name(unsigned char index, MSCB_INFO_VAR *pvar)
{
   char idata str[10];

   /* display variable name */
   lcd_goto(0, 0);
   memcpy(str, pvar->name, 8);
   str[8] = 0;
   printf("%2bd:%s", index, str);
   puts("        ");
}

/*------------------------------------------------------------------*/

void lcd_menu()
{
   static idata unsigned long last_disp = 0, last_b2 = 0, last_b3 = 0;
   static bit b0_old = 0, b1_old = 0, b2_old = 0, b3_old = 0;
   MSCB_INFO_VAR *pvar;
   unsigned char i;

   /* clear startup screen after 3 sec. */
   if (startup) {
      if(time() > 300) {
         startup = 0;
         lcd_clear();
      } else
        return;
   }

   /* do menu business only every 100ms */
   if (time() > last_disp+10) {
      last_disp = time();

      if (system_menu)
         pvar = &sysvar[var_index];
      else
         pvar = &variables[var_index];

      if (!in_menu) {

         /* if not in menu, call application display */
         if (application_display(0))
            in_menu = 1;
      
      } else {

         if (enter_mode) {

            /* display variables */
            display_value(pvar, &f_var);
            lcd_goto(0, 1);
            puts("ESC ENTER   -    + ");

            /* evaluate ESC button */
            if (b0 && !b0_old)
               enter_mode = 0;

            /* evaluate ENTER button */
            if (b1 && !b1_old) {
               enter_mode = 0;
               memcpy(pvar->ud, &f_var, pvar->width);
               
               if (system_menu) {
                  /* flash new address */
                  if (var_index == 0 || var_index == 1)
                     flash_param = 1;
               } else {
                  user_write(var_index);
#ifdef SCS_1000
                  if (variables[var_index].flags & MSCBF_REMOUT)
                     send_remote_var(var_index);
#endif
               }
            }

            /* evaluate "-" button */
            if (b2 && !b2_old) {
               var_inc(pvar, -1);
               last_b2 = time();
            }
            if (b2 && time() > last_b2 + 70)
               var_inc(pvar, -1);
            if (!b2)
               last_b2 = 0;

            /* evaluate "+" button */
            if (b3 && !b3_old) {
               var_inc(pvar, 1);
               last_b3 = time();
            }
            if (b3 && time() > last_b3 + 70)
               var_inc(pvar, 1);
            if (!b3)
               last_b3 = 0;

         } else {
            
            /* display variables */
            display_name(var_index, pvar);
            display_value(pvar, pvar->ud);
            lcd_goto(0, 1);

            if (pvar->delta > 0 || (pvar->flags & MSCBF_DATALESS))
               puts("ESC ENTER  PREV NEXT");
            else
               puts("ESC        PREV NEXT");
   
            /* evaluate ESC button */
            if (b0 && !b0_old) {
               if (system_menu) {
                  system_menu = 0;
                  var_index = 0;
               } else {
                  in_menu = 0;
                  application_display(1);
               }
            }
   
            /* evaluate ENTER button */
            if (b1 && !b1_old) {

               if (pvar->width <= 4 && pvar->width > 0) {
                  enter_mode = 1;
                  memcpy(&f_var, pvar->ud, pvar->width);
               }

               /* check for flash command */
               if (system_menu && var_index == 2) {
                  flash_param = 1;
                  lcd_clear();
                  lcd_goto(0, 0);
                  puts("Parameters written");
                  lcd_goto(0, 1);
                  puts("to flash memory.");
                  delay_ms(3000);
               }
 
               /* check for reboot command */
               if (system_menu && var_index == 3)
                  reboot = 1;
            }

            /* evaluate prev button */
            if (b2 && !b2_old) {
               if (var_index == 0)
                 var_index = n_variables-1;
               else
                 var_index--;
               last_b2 = time();
            }
            if (b2 && time() > last_b2 + 70) {
               if (var_index == 0)
                 var_index = n_variables-1;
               else
                 var_index--;
            }
            if (!b2)
               last_b2 = 0;

            if (system_menu)
               i = n_sysvar;
            else
               i = n_variables;

            /* evaluate next button */
            if (b3 && !b3_old) {
               var_index = (var_index + 1) % i;
               last_b3 = time();
            }
            if (b3 && time() > last_b3 + 70)
               var_index = (var_index + 1) % i;
            if (!b3)
               last_b3 = 0;

            /* check for system menu */
            if (b2 && b3 && !system_menu) {
               var_index = 0;
               system_menu = 1;
            }
         }
      }

      b0_old = b0;
      b1_old = b1;
      b2_old = b2;
      b3_old = b3;
   }
}

