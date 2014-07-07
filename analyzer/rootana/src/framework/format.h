#ifndef FORMAT_H_
#define FORMAT_H_

#ifdef FORMAT_CPP_
///@brief This marco is just a trick to save us from needing to write
///values in a seperate definition file. 
#define WITH_VALUE(value) = value
#else
#define WITH_VALUE(value) 
#endif


/// This holds constants relating to the format of data on disk.
/// Changes to these constants are unlikely to backward-compatible
namespace AlCapFormat {
  namespace Raw {
    extern const char* SetupTreeName WITH_VALUE("Setup Tree");
    extern const char* DataTreeName WITH_VALUE("EventTree");
    extern const char* DataBranchName WITH_VALUE("Event");
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
