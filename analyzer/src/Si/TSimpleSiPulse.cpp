#include "TOctalFADCIsland.h"
#include "TSimpleSiPulse.h"
#include "TH1.h"
#include "TH1I.h"
#include "TH1D.h"
#include <math.h>

TSimpleSiPulse::~TSimpleSiPulse()
{ ; }

TSimpleSiPulse::TSimpleSiPulse(TOctalFADCIsland *island, unsigned int nped)
{
	fTime = island->GetTime();
	fData = island->GetSampleVector();
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

TSimpleSiPulse::TSimpleSiPulse(TOctalFADCIsland *island, std::string detname,
		unsigned int nped)
{
	fTime = island->GetTime();
	fData = island->GetSampleVector();
	
	if (fData.size() < nped)
		fNPedSamples = 0;
	else
		fNPedSamples = nped;
	fDetName = detname;
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

// Use this constructor if you are getting pulses from an island since
// we should probably use the pedestal from the island for all its pulses
TSimpleSiPulse::TSimpleSiPulse(TOctalFADCIsland *island, double pedestal)
{
	fTime = island->GetTime();
	fData = island->GetSampleVector();
	fPedestal = pedestal;
	
	fNPedSamples = 0;
	fRMSNoise = 0;
	fThreshold = 0;

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

	TSimpleSiPulse *invertedPulse = new TSimpleSiPulse(invertedIsland, -fPedestal);
	return invertedPulse;
}

TH1I * TSimpleSiPulse::GetWaveform(std::string histname)
{
	TH1I *h = new TH1I(histname.c_str(),histname.c_str(),100,0,100);
	h->SetBit(TH1::kCanRebin);
	for (unsigned int i = 0; i < fData.size(); ++i)
	{
		h->SetBinContent(i+1,fData.at(i));
	}
	return h;
}

double TSimpleSiPulse::GetPulseHeight() {

	double pulseheight;
	if (fIsPositive)
	{
		pulseheight = GetMax() - fPedestal;
	}
	else
	{
		TSimpleSiPulse *invertedPulse = this->Invert();
		pulseheight = invertedPulse->GetMax() - invertedPulse->GetPedestal();
	}
	
	return pulseheight;			
}

double TSimpleSiPulse::GetPulseTime() {
	
	return GetMaxBinTime();
}
