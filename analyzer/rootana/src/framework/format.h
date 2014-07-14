#ifndef FORMAT_H_
#define FORMAT_H_

#ifdef FORMAT_CPP_
///@brief This marco is just a trick to save us from needing to write
///values in a separate definition file. 
///@detail There are other ways to do this, unnamed namespace but this
///makes all includes of format.h refer to the same constants
#define WITH_VALUE(value) = value
#else
#define WITH_VALUE(value) 
#endif


/// This holds constants relating to the format of data on disk.
/// Changes to these constants are unlikely to backward-compatible
namespace AlCapFormat {
  namespace Raw {
    extern const char* DataTreeName WITH_VALUE("EventTree");
    extern const char* DataBranchName WITH_VALUE("Event");
    extern const char* SetupTreeName WITH_VALUE("SetupTree");
    extern const char* SetupBranchName WITH_VALUE("Setup");
  };
  
  namespace Setup {
    // TODO find out if these names are remotely appropriate
    //fBankToDetectorMap
    //fBankToClockTickMap
    //fBankToBitMap
    //fBankToADCSlopeCalibMap
    //fBankToADCOffsetCalibMap
    //fBankToEnableBitMap
    
  };
};

#undef WITH_VALUE
#endif //FORMAT_H_
