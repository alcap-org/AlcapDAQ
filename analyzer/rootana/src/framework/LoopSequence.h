#ifndef LOOPSEQUENCE_H_
#define LOOPSEQUENCE_H_


//C++/STL

//ROOT
#include "Rtypes.h"

//Local
#include "CommandLine.h"
class TGlobalData;

class LoopSequence {
  friend class EventNavigator;
public:
  void Run() const;

  Long64_t StartEntry() const {return fStart;}
  Long64_t StopEntry() const {return fStop;}

  const ARGUMENTS& GetCommandLineArgs() {return fArgs;}

protected:
  //LoopSequence();
  LoopSequence(const ARGUMENTS& args)
    : fArgs(args)
  {
    Init();
  }
  LoopSequence(const LoopSequence& src);
  LoopSequence& operator=(const LoopSequence& rhs);

  void Init();
  void Preprocess() const;
  void Process() const;
  void Postprocess() const;
  void Checkpoint(Long64_t entry) const;
  //void ClearGlobalData(TGlobalData* data);

private:  
  const ARGUMENTS fArgs;
  Long64_t fStart;
  Long64_t fStop;
};

#endif //LOOPSEQUENCE_H_
