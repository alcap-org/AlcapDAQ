#include "TAnalysedPulseMapWrapper.h"

ClassImp(TAnalysedPulseMapWrapper)

TAnalysedPulseMapWrapper::TAnalysedPulseMapWrapper()
{
	Reset();
}

TAnalysedPulseMapWrapper::TAnalysedPulseMapWrapper 
(StringAnalPulseMap aMap)
{
	Reset();
	fChn2PulseMap = aMap;
}

void TAnalysedPulseMapWrapper::Reset()
{
	fChn2PulseMap.clear();
}

TAnalysedPulseMapWrapper::~TAnalysedPulseMapWrapper()
{
	;
}

void TAnalysedPulseMapWrapper::ShowInfo()
{
	printf("map size: %lu\n", fChn2PulseMap.size());
	StringAnalPulseMap::iterator mapIter;
	
	for (mapIter = fChn2PulseMap.begin(); 
			mapIter != fChn2PulseMap.end(); ++mapIter)
	{
		printf("%s\n", mapIter->first.c_str());
		AnalysedPulseList pulseVector = mapIter->second;
		AnalysedPulseList::iterator pulseIter;
		for (pulseIter = pulseVector.begin(); 
				pulseIter != pulseVector.end(); ++pulseIter)
		{
			printf("height: %.2f\n", (*pulseIter)->GetAmplitude());
		}

	}
}
