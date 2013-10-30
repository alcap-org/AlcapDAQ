/********************************************************************\

  Name:         frontend.c
  Created by:   Stefan Ritt

  Contents:     Example Slow Control Frontend program. Defines two
                slow control equipments, one for a HV device and one
                for a multimeter (usually a general purpose PC plug-in
                card with A/D inputs/outputs. As a device driver,
                the "null" driver is used which simulates a device
                without accessing any hardware. The used class drivers
                cd_hv and cd_multi act as a link between the ODB and
                the equipment and contain some functionality like
                ramping etc. To form a fully functional frontend,
                the device driver "null" has to be replaces with
                real device drivers.


  $Log: frontend.c,v $
  Revision 1.1.1.1  2004/10/20 18:41:18  admin
  Trying again to import midas-1.9.5.  There are NT executables, but I'm importing them as well (w/o -kb switch so watch out!)

  Revision 1.11  2004/01/08 08:40:09  midas
  Implemented standard indentation

  Revision 1.10  2002/06/10 07:15:17  midas
  Use new DF_xxx flags

  Revision 1.9  2002/05/29 13:35:47  midas
  Added max_event_size_frag

  Revision 1.8  2002/05/13 22:21:08  midas
  Fixed typo

  Revision 1.7  2002/03/13 08:39:28  midas
  Use bus drivers in examples

  Revision 1.6  2000/08/21 10:49:11  midas
  Added max_event_size

  Revision 1.5  2000/03/02 22:00:47  midas
  Added number of subevents in equipment list

  Revision 1.4  1999/12/21 09:38:23  midas
  Use new driver names

  Revision 1.3  1998/10/23 08:46:26  midas
  Added #include "null.h"

  Revision 1.2  1998/10/12 12:18:59  midas
  Added Log tag in header


\********************************************************************/

#include <stdio.h>
#include "midas.h"
//#include "class/hv.h"
//#include "class/multi.h"
//#include "device/nulldev.h"
//#include "bus/null.h"

/* make frontend functions callable from the C framework */
#ifdef __cplusplus
extern "C" {
#endif

INT crate_number=0;

HNDLE hDB;
HNDLE hKey;
int size;
int sepDemand;
char Alarm[128] = "/Equipment/SeparatorMonitor/Variables/Alarm_Triggered";

INT read_separator_event(char *pevent, INT off);
INT pre_begin_of_run();
INT frontend_early_init();

/*-- Globals -------------------------------------------------------*/

/* The frontend name (client name) as seen by other MIDAS clients   */
char *frontend_name = "SeparatorMonitor";
/* The frontend file name, don't change it */
char *frontend_file_name = __FILE__;

/* frontend_loop is called periodically if this variable is TRUE    */
BOOL frontend_call_loop = TRUE;

/* a frontend status page is displayed with this frequency in ms    */
INT display_period = 1000;

/* maximum event size produced by this frontend */
INT max_event_size = 10000;

/* maximum event size for fragmented events (EQ_FRAGMENTED) */
INT max_event_size_frag = 5 * 1024 * 1024;

/* buffer size to hold events */
INT event_buffer_size = 10 * 10000;

/*-- Equipment list ------------------------------------------------*/

BANK_LIST separator_bank_list[] = {
  { "SEPA", TID_FLOAT,  4, NULL },
  { "" },
};

typedef struct SEPARATOR_DATA_Struct {
  float hv;
  float current;
  float vac;
  float sep_stat;
} SEPARATOR_DATA;

#undef USE_INT

EQUIPMENT equipment[] = {
   {"SeparatorMonitor",            /* equipment name */
     { 22, 0,                       /* event ID, trigger mask */
       "SYSTEM",                   /* event buffer */
       EQ_PERIODIC,                  /* equipment type */
       0,                          /* event source */
       "MIDAS",                    /* format */
       TRUE,                       /* enabled */
       RO_ALWAYS|RO_ODB,           /* read all the time */
       50000,                      /* reads spaced by this many ms */
       0,                          /* stop run after this event limit */
       0,                          /* number of sub events */
       0,                          /* log history every event */
       "", "", "", },
    read_separator_event,              /* readout routine */
    NULL, NULL, 
    separator_bank_list,
   },

   {""}
};


#ifdef __cplusplus
}
#endif

/*-- Dummy routines ------------------------------------------------*/

INT frontend_early_init()
{
  return CM_SUCCESS;
}

INT pre_begin_of_run()
{
  return CM_SUCCESS;
}

INT poll_event(INT source[], INT count, BOOL test)
{
   return 1;
};
INT interrupt_configure(INT cmd, INT source[], PTYPE adr)
{
   return 1;
};

/*-- Frontend Init -------------------------------------------------*/

INT frontend_init()
{

  // get the LabView filename
  cm_get_experiment_database(&hDB, NULL);
  
  if(db_find_key(hDB, 0, Alarm, &hKey) != SUCCESS){
    db_create_key(hDB,0,Alarm,TID_INT);
  }

  return CM_SUCCESS;
}

/*-- Frontend Exit -------------------------------------------------*/

INT frontend_exit()
{
  return CM_SUCCESS;
}

/*-- Frontend Loop -------------------------------------------------*/

INT frontend_loop()
{
   ss_sleep(100);
   return CM_SUCCESS;
}

/*-- Begin of Run --------------------------------------------------*/

INT begin_of_run(INT run_number, char *error)
{
   return CM_SUCCESS;
}

/*-- End of Run ----------------------------------------------------*/

INT end_of_run(INT run_number, char *error)
{
   return CM_SUCCESS;
}

/*-- Pause Run -----------------------------------------------------*/

INT pause_run(INT run_number, char *error)
{
   return CM_SUCCESS;
}

/*-- Resuem Run ----------------------------------------------------*/

INT resume_run(INT run_number, char *error)
{
   return CM_SUCCESS;
}

/*------------------------------------------------------------------*/

INT read_separator_event(char *pevent, INT off){
  static SEPARATOR_DATA separator_data;

  static char oldtimestamp[80];
  char timestamp[80];
  char timestring[80];
  FILE *infile=NULL;
  float *pdata;
  static float lastHVs[4] = {-1., -1., -1., -1.};
  static int initDone = 0;
  int i;

  size = sizeof(sepDemand);
  db_get_value(hDB,0, "/Experiment/Edit on start/Separator HV",
	       &sepDemand, &size, TID_INT, 1);

  timestamp[0]='\0';

  infile=fopen("/tmp/separator.txt","r");
  if (infile==NULL){
    cm_msg(MERROR,frontend_name,
	   "Could not open tempfile. separator_daemon.pl not running?");
    separator_data.hv =-1;
    separator_data.vac =-1;
    separator_data.sep_stat =-1;
  }
  else{
    fscanf(infile,"%s\n%f\n%f\n%f\n%f",timestamp,&separator_data.hv,
	   &separator_data.current,&separator_data.vac,
	   &separator_data.sep_stat);
    fclose(infile);
    
    if(!strcmp(oldtimestamp, timestamp)){
      separator_data.hv=-1;
      separator_data.vac=-1;
      separator_data.sep_stat=-1;
    }
  }  
  
  if(initDone == 0){
    for(i=0; i<4; i++) lastHVs[i] = separator_data.hv;
    initDone = 1;
  }
  else{
    for(i=0; i<3; i++) lastHVs[i] = lastHVs[i+1];
    lastHVs[3] = separator_data.hv;
  }

  //for(i=0; i<4; i++) printf("HV[%d]=%d\n",i,(int)lastHVs[i]);
  //printf("Demand = %d\n", sepDemand);

  //Send to midas:
  bk_init(pevent);
  bk_create(pevent,"SEPA",TID_FLOAT,&pdata);
  pdata[0] = separator_data.hv;
  pdata[1] = separator_data.current;
  pdata[2] = separator_data.vac;
  pdata[3] = separator_data.sep_stat;
  bk_close(pevent,pdata+4);

  int sepError = 1;

  for(i=0; i<4; i++){
    if((int)lastHVs[i] == sepDemand) sepError = 0;
  }

  if(sepError==1){
    if(strcmp(oldtimestamp, timestamp)){
      cm_msg(MERROR,frontend_name,"Separator high voltage not same as demand!");
      printf("Demand = %d", sepDemand);
    }
    else{
      cm_msg(MERROR,frontend_name,"Old and new timestamps identical."
	     " Refresh too high or separator_daemon.pl not running.");
    }
    int on = 1;
    db_set_value(hDB, 0, Alarm, &on, sizeof(on), 1, TID_INT);
  }
  else{
    int off = 0;
    db_set_value(hDB, 0, Alarm, &off, sizeof(off), 1, TID_INT);
  }

  strcpy(oldtimestamp,timestamp);
  
  return bk_size(pevent);
}
