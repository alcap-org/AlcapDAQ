
#define PARALLEL_PORT0 0x378
#define PARALLEL_PORT1 (0x378+1)

#include <stdio.h>
#include <sys/io.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdarg.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
//#include <asm/msr.h>
#include <sys/time.h>
#include <netinet/in.h>
//#include <dev/pci/sis1100_var.h>  
//#include <sis3100_calls/sis3100_vme_calls.h>



// INLINE 
int setPP(char v, int usec) { 

  outb(v,PARALLEL_PORT0);

  if(usec) usleep(usec); 
  return 0;

};


int main (void)
{
  int n=0;
 

  printf("Opening port %x ...\n",PARALLEL_PORT0);
  if(ioperm(PARALLEL_PORT0, 3, 1))
    {
      perror("parallel port opening error");
      return 1;
    }
  printf("Ok:\n");
  

  
  while(n>=0)
    {
      setPP(0,0);
      setPP(255,0);
      usleep(5);
      setPP(0,0);
      usleep(5);
      n++;
      
    }

  
  

}
