#include <stdio.h>
#include <stdarg.h>

#include "midas.h"

#include "odb_wrapper.h"

/* ******************************************************************* */
HNDLE hDB;

int odb_connect(char *format, ...)
{
  char s[MAX_ODB_PATH];
  int  status;
  HNDLE hKey;

  va_list argp;
  va_start(argp, format);
  vsprintf(s, format, argp);
  va_end(argp);
  
  /* connect to experiment */
  status = cm_connect_experiment("pc620", "muCap", s, NULL);
  if (status != CM_SUCCESS)
    return 0;

  /* get handle to online database */
  cm_get_experiment_database(&hDB, &hKey);

  return SUCCESS;
}

/* ******************************************************************* */
int odb_find_key(char *format, ...)
{
  char s[MAX_ODB_PATH];
  int  status;
  HNDLE hKey;
  
  va_list argp;
  va_start(argp, format);
  vsprintf(s, format, argp);
  va_end(argp);

  status = db_find_key(hDB, 0, s, &hKey);

  if (status != DB_SUCCESS)
    return 0;

  return SUCCESS;
}

/* ******************************************************************* */
int odb_get_int(char *format, ...)
{
  char s[MAX_ODB_PATH];
  int  retval, size, status;

  va_list argp;
  va_start(argp, format);
  vsprintf(s, format, argp);
  va_end(argp);

  size = sizeof(retval);
  status = db_get_value(hDB, 0, s, &retval, &size, 
			TID_INT, TRUE);
  return retval;
}

/* ******************************************************************* */
WORD odb_get_word(char *format, ...)
{
  char  s[MAX_ODB_PATH];
  int   size, status;
  WORD retval;

  va_list argp;
  va_start(argp, format);
  vsprintf(s, format, argp);
  va_end(argp);

  size = sizeof(retval);
  status = db_get_value(hDB, 0, s, &retval, &size, 
			TID_WORD, TRUE);
  return retval;
}

/* ******************************************************************* */
DWORD odb_get_dword(char *format, ...)
{
  char  s[MAX_ODB_PATH];
  int   size, status;
  DWORD retval;

  va_list argp;
  va_start(argp, format);
  vsprintf(s, format, argp);
  va_end(argp);

  size = sizeof(retval);
  status = db_get_value(hDB, 0, s, &retval, &size, 
			TID_DWORD, TRUE);
  return retval;
}

/* ******************************************************************* */
BOOL odb_get_bool(char *format, ...)
{
  char  s[MAX_ODB_PATH];
  int   size, status;
  BOOL  retval;

  va_list argp;
  va_start(argp, format);
  vsprintf(s, format, argp);
  va_end(argp);

  size = sizeof(retval);
  status = db_get_value(hDB, 0, s, &retval, &size, 
			TID_BOOL, TRUE);
  return retval;
}

/* ******************************************************************* */
void odb_get_boolarray(BOOL *array_out, 
		       int num_array_elements, 
		       char *format, ...)
{
  char  s[MAX_ODB_PATH];
  int   size, status;
  BOOL  rettype;

  va_list argp;
  va_start(argp, format);
  vsprintf(s, format, argp);
  va_end(argp);

  size = sizeof(rettype) * num_array_elements;
  status = db_get_value(hDB, 0, s, array_out, &size, 
			TID_BOOL, TRUE);
}
