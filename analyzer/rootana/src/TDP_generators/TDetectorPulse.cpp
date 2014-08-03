#include "TDetectorPulse.h"

#include <cmath>
#include <sstream>
#include <cstdlib>

using std::vector;
using std::string;

ClassImp(TDetectorPulse);

TDetectorPulse::~TDetectorPulse(){}

TDetectorPulse::TDetectorPulse(){
  Reset();
}

TDetectorPulse::TDetectorPulse(const IDs::source& sourceID,
                int f_parentID, const TAnalysedPulse* f_parent,
                int s_parentID, const TAnalysedPulse* s_parent
                ){
  Reset();
  fParentPulse[kSlow] = s_parent;
  fParentPulse[kFast] = f_parent;
  if(s_parent) fParentSource[kSlow]= s_parent->GetSource();
  if(f_parent) fParentSource[kFast]= f_parent->GetSource();
  fParentID[kSlow]=s_parentID;
  fParentID[kFast]=f_parentID;
  fSource=sourceID;
}

void TDetectorPulse::Reset(Option_t* o) {
    for(int i=0;i<kNumParents;++i){
        fParentPulse[i] = NULL;
        fParentID[i]=-1;
        //fParentSource[i].Reset();
    }
    fCheckedForPileUp=false;
    fPileUpSafe=false;
}
