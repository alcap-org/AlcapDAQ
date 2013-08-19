/*********************************************************************

Author:     Stefan Ritt
Date:       8/30/95, update 12/18/96 to use directIO driver
Program:    giveio.c
Compile:    cl -DWIN32 giveio.c

Purpose:    Open an IO address space in the calling window

*********************************************************************/

#include <stdio.h>
#include <windows.h>
#include <math.h>
#include <conio.h>


/*********************************************************************
  Communication routines with directio device driver
*********************************************************************/
static HANDLE _hdio = 0;

INT ss_directio_init()
{
  _hdio = CreateFile("\\\\.\\directio", GENERIC_READ, FILE_SHARE_READ, NULL,
			               OPEN_EXISTING, 0, NULL);
  return (INT) _hdio;
}

INT ss_directio_exit()
{
  return CloseHandle(_hdio);
}

INT ss_directio_give_port(INT start, INT end)
{
DWORD buffer[] = {6, start, end, 0};
DWORD size;

  if (_hdio <= 0)
    return 0;

  if (DeviceIoControl(_hdio, (DWORD) 0x9c406000, &buffer, sizeof(buffer), 
                      NULL, 0, &size, NULL))
    return 1;

  return GetLastError();
}

INT ss_directio_lock_port(INT start, INT end)
{
DWORD buffer[] = {7, start, end, 0};
DWORD size;

  if (_hdio <= 0)
    return 0;

  if (DeviceIoControl(_hdio, (DWORD) 0x9c406000, &buffer, sizeof(buffer), 
                      NULL, 0, &size, NULL))
    return 1;

  return GetLastError();
}

/*********************************************************************
  Main program. Request ports 0x42, 0x43 and 0x61 and play music
*********************************************************************/

int main(int argc, char* argv[])
{
int i, p1, p2;

  if (argc != 3)
    {
    printf("usage: giveio <port1> <port2>\n");
    printf("  to open port1 to port2 (inclusive).\n");
    printf("  Ports are specified hexadecimally.\n");
    return 0;
    }

  sscanf(argv[1], "%x", &p1);
  sscanf(argv[2], "%x", &p2);

  if (p1 == 0 || p2 == 0)
    {
    printf("Ports may not be zero\n");
    return 0;
    }

  if (p1 == 0 || p2 == 0 || p1 > p2)
    {
    printf("Second port must be larger or equal to first port\n");
    return 0;
    }

  if (ss_directio_init() == 0)
    {
    printf("DirectIO driver not installed or started\n");
    return 0;
    }

  ss_directio_give_port(p1, p2);
  ss_directio_exit();

  printf("Ports 0x%x to 0x%x enabled sucessfully.\n", p1, p2);

	return 0;
}

