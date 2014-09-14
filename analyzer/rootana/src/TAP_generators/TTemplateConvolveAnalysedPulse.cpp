#include "TTemplateConvolveAnalysedPulse.h"

TTemplateConvolveAnalysedPulse::TTemplateConvolveAnalysedPulse(
       const IDs::source& sourceID, const TPulseIslandID& parentID, const TPulseIsland* parentTPI):
            TAnalysedPulse(sourceID,parentID,parentTPI){}

TTemplateConvolveAnalysedPulse::~TTemplateConvolveAnalysedPulse(){
}

