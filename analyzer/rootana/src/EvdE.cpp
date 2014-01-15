#include "EvdE.h"
#include "TAnalysedPulse.h"

#include <iostream>
#include <cmath>
#include <string>
#include <vector>
#include <map>

#include "TH1D.h"
#include "TH2D.h"

extern std::map< std::string, std::vector<TAnalysedPulse*> > gAnalysedPulseMap;
static int nSec;            // Number of sections in the silicon thin detectors
static TH2D* hEvdE[2];      // Histograms (right, left)
static TH1D* hEvdE_log[2];

EvdE::EvdE(char *HistogramDirectoryName):FillHistBase(HistogramDirectoryName) 
{;}

EvdE::EvdE(char *HistogramDirectoryName, double t0, double t1) :
	FillHistBase(HistogramDirectoryName) 
{
	// Thin silicon is in quadrants
	nSec = 4;
	// Time in ns
	tCoincidence = 1000.;
	tPP = 10000.;
	tStart = t0;
	tStop = t1;
	// Energy cuts in ADC
	adcCutThick[0] = 80.;
	adcCutThick[1] = 80.;
	adcCutThin[0] = 160.;
	adcCutThin[1] = 160.;
	eCutThin[0] = adcCutThin[0] * 2.6;
	eCutThin[1] = adcCutThin[1] * 2.6;

	slopeThick[0] = 8.06;
	offsetThick[0] = -192.70;
	slopeThick[1] = 7.93;
	offsetThick[1] = -116.33;

	slopeThin[0][0] = 2.57;
	offsetThin[0][0] = -87.96;
	slopeThin[0][1] = 2.72;
	offsetThin[0][1] = -292.15;
	slopeThin[0][2] = 2.52;
	offsetThin[0][2] = -30.19;
	slopeThin[0][3] = 2.57;
	offsetThin[0][3] = -103.67;

	slopeThin[1][0] = 2.66;
	offsetThin[1][0] = -117.34;
	slopeThin[1][1] = 2.56;
	offsetThin[1][1] = -44.45;
	slopeThin[1][2] = 2.68;
	offsetThin[1][2] = -129.73;
	slopeThin[1][3] = 2.58;
	offsetThin[1][3] = -85.48;

	// Prepare histograms
	hEvdE[0] = new TH2D("hEvdE_Right", "dEdx vs E (Right);E + dE (keV);dE (keV)",
			1000, 0., 14000.,
			500, 0., 7000.);
	hEvdE[1] = new TH2D("hEvdE_Left", "dEdx vs E (Left);E + dE (keV);dE (keV)",
			1000, 0., 14000.,
			500, 0., 7000.);

	hEvdE_log[0] = new TH1D("hEvdE_Right_log", "logdE +logE (Right)", 100, 10., 30.);
	hEvdE_log[1] = new TH1D("hEvdE_Left_log", "logdE +logE (Left)", 100, 10., 30.);

	gDirectory->cd("/");
}

EvdE::~EvdE(){
}

int EvdE::ProcessEntry(TGlobalData *gData, TSetupData *gSetup) {

	// Look for muSc and at least one arm
	static std::vector<TAnalysedPulse*> *musc;
	static std::vector<TAnalysedPulse*> *sirThin[4], *silThin[4];
	static std::vector<TAnalysedPulse*> *sirThick, *silThick;
	if (gAnalysedPulseMap.count("muSc")) musc = &gAnalysedPulseMap.at("muSc");
	else return 0;

	if (gAnalysedPulseMap.count("SiR1-1-S")) sirThin[0] = &gAnalysedPulseMap.at("SiR1-1-S");
	else sirThin[0] = NULL;
	if (gAnalysedPulseMap.count("SiR1-2-S")) sirThin[1] = &gAnalysedPulseMap.at("SiR1-2-S");
	else sirThin[1] = NULL;
	if (gAnalysedPulseMap.count("SiR1-3-S")) sirThin[2] = &gAnalysedPulseMap.at("SiR1-3-S");
	else sirThin[2] = NULL;
	if (gAnalysedPulseMap.count("SiR1-4-S")) sirThin[3] = &gAnalysedPulseMap.at("SiR1-4-S");
	else sirThin[3] = NULL;

	if (gAnalysedPulseMap.count("SiL1-1-S")) silThin[0] = &gAnalysedPulseMap.at("SiL1-1-S");
	else silThin[0] = NULL;
	if (gAnalysedPulseMap.count("SiL1-2-S")) silThin[1] = &gAnalysedPulseMap.at("SiL1-2-S");
	else silThin[1] = NULL;
	if (gAnalysedPulseMap.count("SiL1-3-S")) silThin[2] = &gAnalysedPulseMap.at("SiL1-3-S");
	else silThin[2] = NULL;
	if (gAnalysedPulseMap.count("SiL1-4-S")) silThin[3] = &gAnalysedPulseMap.at("SiL1-4-S");
	else silThin[3] = NULL;

	if (gAnalysedPulseMap.count("SiR2-S")) sirThick = &gAnalysedPulseMap.at("SiR2-S");
	else sirThick = NULL;

	if (gAnalysedPulseMap.count("SiL2-S")) silThick = &gAnalysedPulseMap.at("SiL2-S");
	else silThick = NULL;

	// If there's nothing logical to get data from, don't bother analysing
	if (!(sirThick && (sirThin[0] || sirThin[1] || sirThin[2] || sirThin[3])) &&
			!(silThick && (silThin[0] || silThin[1] || silThin[2] || silThin[3])))
		return 0;


	// Iterators through aforementioned vectors
	static int iSec;
	static std::vector<TAnalysedPulse*>::iterator cMusc;
	static std::vector<TAnalysedPulse*>::iterator cSiRThick, cSiLThick;
	static std::vector<TAnalysedPulse*>::iterator cSiRThin[4], cSiLThin[4];
	cMusc = musc->begin();
	if (sirThick)
		cSiRThick = sirThick->begin();
	if (silThick)
		cSiLThick = silThick->begin();
	for (iSec = 0; iSec < nSec; ++iSec) 
	{
		if (sirThin[iSec]) 
			cSiRThin[iSec] = (sirThin[iSec])->begin();

		if (silThin[iSec])
			cSiLThin[iSec] = (silThin[iSec])->begin();
	}

	static double tMusc;
	static std::vector<double> tSiRThick, tSiLThick;
	static std::vector<double> eSiRThick, eSiLThick;
	static unsigned int iHit, nThickHits;

	for (; cMusc != musc->end(); ++cMusc) 
	{
		tMusc = (*cMusc)->GetTime();
		tSiRThick.clear(); tSiLThick.clear();
		eSiRThick.clear(); eSiLThick.clear();
		// If there's pile-up, advance beyond it
		if ((cMusc+1) != musc->end() && ((*(cMusc + 1))->GetTime() - tMusc) < tPP) 
		{
			++cMusc;
			continue;
		}

		// Advance silicon pulses beyond time of muSc
		if (sirThick)
			while (cSiRThick != sirThick->end() 
					&& ((*cSiRThick)->GetTime() < (tMusc + tStart)))
				++cSiRThick;

		if (silThick)
			while ((cSiLThick != silThick->end()) 
					&& ((*cSiLThick)->GetTime() < tMusc + tStart))
				++cSiLThick;

		for (iSec = 0; iSec < nSec; ++iSec) 
		{
			if (sirThin[iSec])
				while (cSiRThin[iSec] != sirThin[iSec]->end() 
						&& ((*(cSiRThin[iSec]))->GetTime() < tMusc + tStart))
					++(cSiRThin[iSec]);

			if (silThin[iSec])
				while (cSiLThin[iSec] != silThin[iSec]->end() 
						&& ((*(cSiLThin[iSec]))->GetTime() < tMusc + tStart))
					++(cSiLThin[iSec]);
		}

		// Check for hits
		// Look for hits in thick detectors first
		if (sirThick) // If there was at least one hit in the thick detector
		{
			while (cSiRThick != sirThick->end() 
					&& (((*cSiRThick)->GetTime() - tMusc) < tStop)) // Look for hits in the decay window after muSc hit
			{ 
				if ((*cSiRThick)->GetEnergy() > adcCutThick[0]) 
				{// If a hit passes the amplitude cut
					tSiRThick.push_back((*cSiRThick)->GetTime()); // Save the time
					eSiRThick.push_back((*cSiRThick)->GetEnergy()); // And the amplitude
				}
				++cSiRThick;
			}
		}

		if (silThick) 
		{
			while (cSiLThick != silThick->end() 
					&& (((*cSiLThick)->GetTime() - tMusc < tStop))) 
			{
				if ((*cSiLThick)->GetEnergy() > adcCutThick[1]) 
				{
					tSiLThick.push_back((*cSiLThick)->GetTime());
					eSiLThick.push_back((*cSiLThick)->GetEnergy());
				}
				++cSiLThick;
			}
		}

		if (tSiRThick.size() > 0) // If there was at least one hit in the thick window
		{
			nThickHits = tSiRThick.size();
			for (iHit = 0; iHit < nThickHits; ++iHit) 
			{// Loop through the times of all of the hits
				double dE = 0.; // Keep track of energy
				for (iSec = 0; iSec < nSec; ++iSec)// Loop through all the sections of the thin silicon 
				{
					if (sirThin[iSec]) 
					{// And, if there are hits in this thin detector
						while ((cSiRThin[iSec] != sirThin[iSec]->end()) &&
								(*(cSiRThin[iSec]))->GetTime() < (tSiRThick[iHit] - tCoincidence))  // Catch the iterator up to the beginning of the time window
							++(cSiRThin[iSec]);

						while (cSiRThin[iSec] != sirThin[iSec]->end() &&
								(*(cSiRThin[iSec]))->GetTime() < tSiRThick[iHit] + tCoincidence) 
						{ // And look for hits in this window
							dE += offsetThin[0][iSec] +                             // Increment the energy
								((*(cSiRThin[iSec]))->GetEnergy())*slopeThin[0][iSec];
							++(cSiRThin[iSec]);
						}
					}
				}
				if (dE> eCutThin[0])                                                      // If the energy is greater than our cut
				{
					double E = eSiRThick[iHit]*slopeThick[0] + offsetThick[0] + dE;
					hEvdE[0]->Fill(E, dE);// Record total energy deposited in thin detector
					hEvdE_log[0]->Fill(log(E) + log(dE));
				}
			}
		}

		if (tSiLThick.size() > 0) 
		{
			nThickHits = tSiLThick.size();
			for (iHit = 0; iHit < nThickHits; ++iHit) 
			{
				double dE = 0.;
				for (iSec = 0; iSec < nSec; ++iSec) 
				{
					if (silThin[iSec]) {
						while (cSiLThin[iSec] != silThin[iSec]->end() &&
								(*(cSiLThin[iSec]))->GetTime() < (tSiLThick[iHit]-tCoincidence))
							++(cSiLThin[iSec]);

						while (cSiLThin[iSec] != silThin[iSec]->end() &&
								(*(cSiLThin[iSec]))->GetTime() < tSiLThick[iHit] + tCoincidence) 
						{
							dE += offsetThin[1][iSec] +                             // Increment the energy
								((*(cSiLThin[iSec]))->GetEnergy())*slopeThin[1][iSec];
							++(cSiLThin[iSec]);
						}
					}
				}
				if (dE > eCutThin[1])
				{
					double E = eSiLThick[iHit]*slopeThick[1] + offsetThick[1] + dE;
					hEvdE[1]->Fill(E, dE);
					hEvdE_log[1]->Fill(log(E) + log(dE));
				}
			}
		}
	}
	return 0;
}
