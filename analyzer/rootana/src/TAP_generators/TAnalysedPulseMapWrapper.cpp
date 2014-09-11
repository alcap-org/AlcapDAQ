#include "TAnalysedPulseMapWrapper.h"
#include <iostream>
using std::cout;
using std::endl;

ClassImp(TAnalysedPulseMapWrapper)

TAnalysedPulseMapWrapper::TAnalysedPulseMapWrapper()
{
    Reset();
}

TAnalysedPulseMapWrapper::TAnalysedPulseMapWrapper(
        const SourceAnalPulseMap& aMap):fChn2PulseMap(aMap)
{
}

void TAnalysedPulseMapWrapper::Reset()
{
    fChn2PulseMap.clear();
}

TAnalysedPulseMapWrapper::~TAnalysedPulseMapWrapper()
{
}

void TAnalysedPulseMapWrapper::ShowInfo()
{
    cout<<"map size: "<<fChn2PulseMap.size()<<endl;
    SourceAnalPulseMap::iterator mapIter;

    for (mapIter = fChn2PulseMap.begin(); 
            mapIter != fChn2PulseMap.end(); ++mapIter)
    {
        cout<<"map size: "<<mapIter->second.size()<<endl;
        AnalysedPulseList* pulseVector = &mapIter->second;
        AnalysedPulseList::const_iterator pulseIter;
        for (pulseIter = pulseVector->begin(); 
                pulseIter != pulseVector->end(); ++pulseIter)
        {
            printf("height: %.2f\n", (*pulseIter)->GetAmplitude());
        }

    }
}
