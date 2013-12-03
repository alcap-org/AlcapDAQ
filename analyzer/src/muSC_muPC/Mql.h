#ifndef __MQL_H
#define __MQL_H

#include <math.h>

#ifndef MIN
#define MIN(a,b) (((a) < (b))? (a) : (b)) 
#endif

#ifndef MAX
#define MAX(a,b) (((a) > (b))? (a) : (b))
#endif
 
static inline double safe_fmod(double x, double y)
{
  if(x >= 0) return fmod(x, y); 
  else {
    x += (((int)(-x/y))+1)*y;
    return fmod(x,y);
  }
}

class TTree;
TTree *MqlRegisterTree(char *treeName);

#endif
