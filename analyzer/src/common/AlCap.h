#include "TPulseIsland.h"
#include "TAnalysedPulse.h"

#include <vector>
#include <pair>
#include <map>

namespace AlCap {

  typedef std::map< std::string, std::vector<TPulseIsland*> > TStringPulseIslandPair
  typedef std::map< std::string, std::vector<TPulseIsland*> > TStringPulseIslandMap;
  typedef std::map< std::string, std::vector<TPulseIsland*> >::iterator TStringPulseIslandIter;

}
