#include "TAnalysedPulseMapWrapper.h"

ClassImp(TAnalysedPulseMapWrapper)

TAnalysedPulseMapWrapper::TAnalysedPulseMapWrapper()
{
	Reset();
}

TAnalysedPulseMapWrapper::TAnalysedPulseMapWrapper 
(map<string, vector<TAnalysedPulse *> > aMap)
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
	printf("map size: %d\n", fChn2PulseMap.size());
	map<string, vector<TAnalysedPulse *> >::iterator mapIter;
	
	for (mapIter = fChn2PulseMap.begin(); 
			mapIter != fChn2PulseMap.end(); ++mapIter)
	{
		printf("%s\n", mapIter->first.c_str());
		vector<TAnalysedPulse *> pulseVector = mapIter->second;
		vector<TAnalysedPulse *>::iterator pulseIter;
		for (pulseIter = pulseVector.begin(); 
				pulseIter != pulseVector.end(); ++pulseIter)
		{
			printf("height: %.2f\n", (*pulseIter)->GetAmplitude());
		}

	}
}
