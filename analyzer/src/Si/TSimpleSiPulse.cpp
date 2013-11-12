#include "TOctalFADCIsland.h"
#include "TSimpleSiPulse.h"
#include "TH1.h"
#include "TH1I.h"
#include "TH1D.h"
#include "DetectorMap.h"
#include <math.h>

TSimpleSiPulse::~TSimpleSiPulse()
{ ; }

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
	
	fWaveform = NULL;

	double weight = 0;
	for (unsigned int i = 0; i < fData.size(); ++i)
	{
		weight += fData.at(i) - fPedestal;
	}
	if (weight >= 0)
		fIsPositive = true;
	else
		fIsPositive = false;
	
	if (detname.find("Slow") != std::string::npos) {
		fIsSlowPulse = true;
		fSlowPulseFit = NULL;
	}
	else
		fIsSlowPulse = false;
}

// Use this constructor if you are getting pulses from an island since
// we should probably use the pedestal from the island for all its pulses
TSimpleSiPulse::TSimpleSiPulse(TOctalFADCIsland *island, std::string detname, double pedestal)
{
	fTime = island->GetTime();
	fData = island->GetSampleVector();
	fPedestal = pedestal;
	fDetName = detname;
	
	fNPedSamples = 0;
	fRMSNoise = 0;
	fThreshold = 0;
	
	fWaveform = NULL;

	double weight = 0;
	for (unsigned int i = 0; i < fData.size(); ++i)
	{
		weight += fData.at(i) - fPedestal;
	}
	if (weight >= 0)
		fIsPositive = true;
	else
		fIsPositive = false;
	
	if (detname.find("Slow") != std::string::npos) {
		fIsSlowPulse = true;
		fSlowPulseFit = NULL;
	}
	else
		fIsSlowPulse = false;
	
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

	TSimpleSiPulse *invertedPulse = new TSimpleSiPulse(invertedIsland, fDetName, -fPedestal);
	return invertedPulse;
}

TH1I * TSimpleSiPulse::GetWaveform(std::string histname)
{
	if (fWaveform == NULL) {
	
		fWaveform = new TH1I(histname.c_str(),histname.c_str(),GetNSamples(),GetTime(),GetTime()+GetNSamples());
		fWaveform->SetBit(TH1::kCanRebin);
		for (unsigned int i = 0; i < fData.size(); ++i)
		{
			fWaveform->Fill(GetTime()+i,fData.at(i));
		}
	
		fWaveform->GetXaxis()->SetTitle("clock ticks");
		fWaveform->GetYaxis()->SetTitle("ADC value");
	}
	
	return fWaveform;
}

double TSimpleSiPulse::GetPulseHeight() {

	double pulseheight;
	
	if (fIsSlowPulse && fSlowPulseFit != NULL) {
		// If it's a slow pulse get the pulse height from the fit
		if (fIsPositive) {
			pulseheight = fSlowPulseFit->GetMaximum() - fPedestal;
		}
		else {
			pulseheight = fPedestal - fSlowPulseFit->GetMinimum();
		}
	}
	else {
		if (fIsPositive)
		{
			pulseheight = GetMax() - fPedestal;
		}
		else
		{
			TSimpleSiPulse *invertedPulse = this->Invert();
			pulseheight = invertedPulse->GetMax() - invertedPulse->GetPedestal();
		}
	}
	
	return pulseheight;			
}

double TSimpleSiPulse::GetPulseTime() {
	
	double pulsetime;
	
	if (fIsSlowPulse && fSlowPulseFit != NULL) {
	
		if (fIsPositive) {
			pulsetime = fSlowPulseFit->GetMaximumX();
		}
		else {
			pulsetime = fSlowPulseFit->GetMinimumX();
		}
	}
	else
		pulsetime = GetMaxBinTime();
		
	return pulsetime;
}
