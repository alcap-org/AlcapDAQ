#ifndef DEBUG_TOOLS_H
#define DEBUG_TOOLS_H

#include "TSystem.h"
#include "TStopwatch.h"
#include <cstdio>

//This file contains a bunch of handy snippets for debugging, mostly
//runtime checks like memory usage and stopwatches.

#define DEBUG_PRINT std::cout << "@ " << __FILE__ << ":" << __LINE__ << std::endl;

namespace DEBUG {
  //--------------------------------------------------------------------
  struct time_log {
    time_log(int c, int u) : cpu(c), user(u) {
      snprintf(str, 64, "CPU/User(ms) = %4d / %4d",cpu, user);
    };
    
    static char str[64];
    int cpu; 
    int user;
  };

  char time_log::str[] = {0};
  
  template<int I> 
    const time_log check_clock(){
    static TStopwatch watch;
    watch.Stop();
    time_log log(1000 * watch.CpuTime(), 1000 * watch.RealTime());
    watch.Start();
    return log;
  }

  time_log check_clock() {
    return check_clock<0xBADBEEF>();
  }

  //--------------------------------------------------------------------
  struct mem_log {
    mem_log(int r, int v) : res(r), virt(v) 
    {
     snprintf(str, 64, "Res/Virt(MiB) = %4d / %4d",res, virt);
    };

    static char str[64];
    int res;
    int virt;
  };

  char mem_log::str[] = {0};

  const mem_log check_mem(){
    static ProcInfo_t proc;
    gSystem->GetProcInfo(&proc);
    return mem_log(proc.fMemResident /1024., proc.fMemVirtual /1024.);
  }
};

#endif
