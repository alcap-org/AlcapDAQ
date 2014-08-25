#ifndef DEBUG_TOOLS_H
#define DEBUG_TOOLS_H

#include "TSystem.h"
#include "TStopwatch.h"
#include <cstdio>
#include <ios>

//This file contains a bunch of handy snippets for debugging, mostly
//runtime checks like memory usage and stopwatches.

#define _NUM_ARGS(X100, X99, X98, X97, X96, X95, X94, X93, X92, X91, X90,\
        X89, X88, X87, X86, X85, X84, X83, X82, X81, X80, X79, X78, X77, X76,\
        X75, X74, X73, X72, X71, X70, X69, X68, X67, X66, X65, X64, X63, X62,\
        X61, X60, X59, X58, X57, X56, X55, X54, X53, X52, X51, X50, X49, X48,\
        X47, X46, X45, X44, X43, X42, X41, X40, X39, X38, X37, X36, X35, X34,\
        X33, X32, X31, X30, X29, X28, X27, X26, X25, X24, X23, X22, X21, X20,\
        X19, X18, X17, X16, X15, X14, X13, X12, X11, X10, X9, X8, X7, X6, X5,\
        X4, X3, X2, X1, N, ...)   N

#define NUM_ARGS(...) _NUM_ARGS(__VA_ARGS__, 100, 99, 98, 97, 96, 95, 94,\
        93, 92, 91, 90, 89, 88, 87, 86, 85, 84, 83, 82, 81, 80, 79, 78, 77,\
        76, 75, 74, 73, 72, 71, 70, 69, 68, 67, 66, 65, 64, 63, 62, 61, 60,\
        59, 58, 57, 56, 55, 54, 53, 52, 51, 50, 49, 48, 47, 46, 45, 44, 43,\
        42, 41, 40, 39, 38, 37, 36, 35, 34, 33, 32, 31, 30, 29, 28, 27, 26,\
        25, 24, 23, 22, 21, 20, 19, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8,\
        7, 6, 5, 4, 3, 2, 1)

#define DEBUG_PREFIX std::cout << "@ " << __FILE__ << ":" << __LINE__
#define DEBUG_VALUE_IMP(value) "\n  "#value"= [" << (value) <<"]"
#define DEBUG_VALUE_1(value)      DEBUG_VALUE_IMP(value)
#define DEBUG_VALUE_2(value, ...) DEBUG_VALUE_IMP(value)", " << DEBUG_VALUE_1(__VA_ARGS__)
#define DEBUG_VALUE_3(value, ...) DEBUG_VALUE_IMP(value)", " << DEBUG_VALUE_2(__VA_ARGS__)
#define DEBUG_VALUE_4(value, ...) DEBUG_VALUE_IMP(value)", " << DEBUG_VALUE_3(__VA_ARGS__)
#define DEBUG_VALUE_5(value, ...) DEBUG_VALUE_IMP(value)", " << DEBUG_VALUE_4(__VA_ARGS__)
#define DEBUG_VALUE_6(value, ...) DEBUG_VALUE_IMP(value)", " << DEBUG_VALUE_5(__VA_ARGS__)

#define DEBUG_VALUE_N_( n , ...) DEBUG_VALUE_##n(__VA_ARGS__)
#define DEBUG_VALUE_N( n , ...) DEBUG_VALUE_N_(n,__VA_ARGS__)

#ifndef ALCAP_NO_DEBUG
#define DEBUG_CHECKPOINT DEBUG_PREFIX << std::endl;
#define DEBUG_PRINT(output) DEBUG_PREFIX<<output << std::endl;
#define DEBUG_VALUE(...) DEBUG_PREFIX << ":" << std::boolalpha \
    << DEBUG_VALUE_N(NUM_ARGS(__VA_ARGS__),__VA_ARGS__) <<std::endl;
#else
#define DEBUG_CHECKPOINT 
#define DEBUG_PRINT(output) 
#define DEBUG_VALUE(...) 
#endif

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
