#ifndef DEBUG_TOOLS_H
#define DEBUG_TOOLS_H

#include "TSystem.h"
#include "TStopwatch.h"
#include <cstdio>

//This file contains a bunch of handy snippets for debugging, mostly
//runtime checks like memory usage and stopwatches.

#define DEBUG_PREFIX std::cout << "@ " << __FILE__ << ":" << __LINE__
#define DEBUG_PRINT DEBUG_PREFIX << std::endl;
#define DEBUG_VALUE(value) DEBUG_PREFIX <<std::boolalpha<<" " #value "= |"<<value<<"|"<<std::endl;

namespace DEBUG {
  //--------------------------------------------------------------------
  struct time_log {
    time_log(int c, int u) ;
    
    static char str[64];
    int cpu; 
    int user;
  };

  template<int I> 
    const time_log check_clock(){
    static TStopwatch watch;
    watch.Stop();
    time_log log(1000 * watch.CpuTime(), 1000 * watch.RealTime());
    watch.Start();
    return log;
  }

  time_log check_clock();

  //--------------------------------------------------------------------
  struct mem_log {
    mem_log(int r, int v) ;

    static char str[64];
    int res;
    int virt;
  };

  const mem_log check_mem();
};

#endif
