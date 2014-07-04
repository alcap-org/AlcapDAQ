#ifndef FORMAT_H_
#define FORMAT_H_

/// This holds constants relating to the format of data on disk.
/// Changes to these constants are unlikely to backward-compatible
namespace AlCapFormat {
  namespace Raw {
    const char* SetupTreeName = "SetupTree";
    const char* DataTreeName = "EventTree";
    const char* DataBranchName = "fPulseIslandToChannelMap";
  };
};

#endif //FORMAT_H_
