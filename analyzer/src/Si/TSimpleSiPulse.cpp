#include "TOctalFADCIsland.h"
#include "TSimpleSiPulse.h"
#include "TH1I.h"
#include <math.h>

TSimpleSiPulse::~TSimpleSiPulse()
{
	;
}

TSimpleSiPulse::TSimpleSiPulse(TOctalFADCIsland *island, unsigned int nped)
{
	fTime = island->GetTime();
	fData = island->GetSampleVector();
	
	if (fData.size() < nped)
		fNPedSamples = 0;
	else
		fNPedSamples = nped;

	fPedestal = island->GetAverage(0,fNPedSamples);

	double dev2 = 0;
	for (unsigned int i = 0; i < fNPedSamples; ++i)
	{
		dev2 += (fData.at(i) - fPedestal)*(fData.at(i) - fPedestal);
	}
	fRMSNoise = sqrt(dev2/(fNPedSamples - 1));
	fThreshold = 5*fRMSNoise;

	double weight = 0;
	for (unsigned int i = 0; i < fData.size(); ++i)
	{
		weight += fData.at(i) - fPedestal;
	}
	if (weight >= 0)
		fIsPositive = true;
	else
		fIsPositive = false;
}

void TSimpleSiPulse::PrintInfo()
{
	printf("Pulse info: ped %.2f, rms %.2f, pos %d \n", 
			fPedestal, fRMSNoise, fIsPositive);
}

TSimpleSiPulse *TSimpleSiPulse::Invert()
{
	std::vector<int> invertedSamples;
	for (unsigned int i = 0; i < fData.size(); ++i)
		invertedSamples.push_back(-1*fData.at(i));

	TOctalFADCIsland *invertedIsland = 
		new TOctalFADCIsland(fTime, invertedSamples);

	TSimpleSiPulse *invertedPulse = new TSimpleSiPulse(invertedIsland);
	return invertedPulse;
}
