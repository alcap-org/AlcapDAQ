#include "debug_tools.h"

char DEBUG::time_log::str[]={0};
char DEBUG::mem_log::str[] = {0};


DEBUG::time_log::time_log(int c, int u) : cpu(c), user(u) {
      snprintf(str, 64, "CPU/User(ms) = %4d / %4d",cpu, user);
  }
  
DEBUG:: mem_log:: mem_log(int r, int v) : res(r), virt(v) {
     snprintf(str, 64, "Res/Virt(MiB) = %4d / %4d",res, virt);
    }

const DEBUG::mem_log DEBUG::check_mem(){
    static ProcInfo_t proc;
    gSystem->GetProcInfo(&proc);
    return mem_log(proc.fMemResident /1024., proc.fMemVirtual /1024.);
  }

DEBUG::time_log DEBUG::check_clock(){
    return check_clock<0xBADBEEF>();
  }
