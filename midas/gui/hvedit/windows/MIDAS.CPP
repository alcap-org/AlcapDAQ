/********************************************************************\

  Name:         midas.c (in hvedit project)
  Created by:   Stefan Ritt

  Contents:     Midas routines for HVEDIT program


  Revision history
  ------------------------------------------------------------------
  date         by    modification
  ---------    ---   ------------------------------------------------
  9-NOV-95    SR    created


\********************************************************************/

#include "stdafx.h"
#include <midas.h>
#include "hvedit.h"
#include "hvedidlg.h"

/*------------------------------------------------------------------*/

/*
void make_single_line(HNDLE hDB, char *key_name, char *result)
{
HNDLE hKey;
KEY   key;
char  data[20000], data_str[256];
int   i, size, type;

  db_find_key(hDB, 0, key_name, &hKey, FALSE);
  db_get_key(hDB, hKey, &key);

  size = sizeof(data);
  db_get_data(hDB, hKey, data, &size, &type);

  result[0] = 0;
  for (i=0 ; i<key.num_values ; i++)
    {
    strcat(result, "{");
    db_sprintf(data_str, data, key.data_size, i, type);
    strcat(result, data_str);
    strcat(result, "} ");
    }
}
*/

/*------------------------------------------------------------------*/

midas_connect(char *host_name, char *experiment_name)
{
HNDLE           m_hDB, m_hKey;
INT             status;
char            str[256];

  /* route all messages to MessageBox */
  //cm_set_msg_print(10, DoMessageBox);

  status = cm_connect_experiment(host_name, experiment_name, "HVEdit", 0);
  if (status != CM_SUCCESS)
    {
    cm_get_error(status, str);
    DoMessageBox(str);
    return 0;
    }

  cm_get_experiment_database(&m_hDB, &m_hKey);
  
  return CM_SUCCESS;
}


midas_disconnect()
{
  return cm_disconnect_experiment();
}


/*
  
  make_single_line(hDB, "experiment/event definition/hv/variables/names", line);
  printf("%s\n", line);

  make_single_line(hDB, "experiment/event definition/hv/variables/demand", line);
  printf("%s\n", line);

  f     = (float) atof("1234.5");
  index = 3;

  db_find_key(hDB, 0, "experiment/event definition/hv/variables/demand", &hKey, FALSE);
  db_set_data_index(hDB, hKey, &f, sizeof(float), index, TID_FLOAT);

  rpc_disconnect();
  return 1;
}

*/  
