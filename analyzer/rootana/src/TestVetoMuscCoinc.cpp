#include "TestVetoMuscCoinc.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <map>
#include <utility>
#include <algorithm>
#include <cmath>

#include "TAnalysedPulse.h"
#include "TDetectorPulse.h"
#include "TH2F.h"
#include "TH1.h"
#include <sstream>

using namespace std;
using std::string;
using std::map;


int Vetos = 0, Muons = 0;

extern std::map<std::string, std::vector<TAnalysedPulse*> > gAnalysedPulseMap;

TestVetoMuscCoinc::TestVetoMuscCoinc(char *HistogramDirectoryName) :
  FillHistBase(HistogramDirectoryName)
  {
    dir->cd("/");
  }
//initialization of histogram location and some options


TestVetoMuscCoinc::~TestVetoMuscCoinc(){}
//needs nothing, simple deconstructor


int TestVetoMuscCoinc::ProcessEntry(TGlobalData *gData, TSetupData *gSetup)
{
  //some typedefs

  //banks
  std::string MuScBank = gSetup->GetBankName("muSc");
  std::string VetoBank = gSetup->GetBankName("ScVe");

  //start by finding the pulses for these two detectors (MuSc and MuVe)
  std::vector<TAnalysedPulse*> Musc_pulses = gAnalysedPulseMap[MuScBank];
  std::vector<TAnalysedPulse*> Veto_pulses = gAnalysedPulseMap[VetoBank];

  std::vector<TPulseIsland*> MuscTPI = gData->fPulseIslandToChannelMap[MuScBank];
  std::vector<TPulseIsland*> VetoTPI = gData->fPulseIslandToChannelMap[VetoBank];


  //location placeholder
  std::vector<TAnalysedPulse*>::iterator currentVetoPulse = Veto_pulses.begin();
  std::vector<TPulseIsland*>::iterator currentVetoTPI = VetoTPI.begin();



  //clocks
  int MuonClock = gSetup->GetClockTick(MuScBank);
  int VetoClock = gSetup->GetClockTick(VetoBank);

  //TimeShifts
  int MuonTS = gSetup->GetTimeShift(MuScBank);
  int VetoTS = gSetup->GetTimeShift(VetoBank);

  for(std::vector<TPulseIsland*>::iterator MuscPulseIter = MuscTPI.begin(); MuscPulseIter != MuscTPI.end(); MuscPulseIter++)
    {

      //first I should verify there is a pulse in muSc
     
      std::vector<int> mSamples = (*MuscPulseIter)->GetSamples();


      int sum = 0, sum2= 0, count = 10, tMuon = 0;
      float pedestal = 0, threshold = 7, sigma = 0;
      bool Muon = false;

      for(std::vector<int>::iterator sIt = mSamples.begin(); sIt != mSamples.begin() + count; ++sIt)
	{
	  sum += *sIt;
	  sum2 += (*sIt) * (*sIt);
	}

      pedestal = sum/count;
      sum2 = abs(sum2 - (pedestal * pedestal *count));
      sigma = sum2 - (sum*sum);

      //if(sigma > 2)
      //{
      //  cout << "MuSc is illbehaved at some point, debug further" << endl;
      //  pedestal = gSetup->GetPedestal("muSc");
      //}


      //if(5*sigma > threshold)
      //threshold = 5* sigma;

      for(std::vector<int>::iterator sIt = mSamples.begin(); sIt != mSamples.end(); ++sIt)
	{
	  int samp = abs((*sIt) - pedestal);
	  if(samp > threshold)
	    {
	      Muon = true;
	      Muons++;
	      int tSamp = std::distance(mSamples.begin(), sIt);
	      int tStamp = (*MuscPulseIter)->GetTimeStamp();
	      tMuon = (tStamp + tSamp) * MuonClock - MuonTS;
	      break;
	    }
	}

      if(Muon == false)
	{
	  continue;
	  cout << "no muon found" << endl;
	}
      

     //next I need to loop through ScVe pulses

      for(std::vector<TPulseIsland*>::iterator VetoPulseIt = VetoTPI.begin(); VetoPulseIt != VetoTPI.end(); ++VetoPulseIt)
	{
	  //check timestamp and size.
	  std::vector<int> vSamples = (*VetoPulseIt)->GetSamples();

	  bool coincidence = false;
	  int VetoStamp = (*VetoPulseIt)->GetTimeStamp();
	  int tVetoMin = VetoStamp * VetoClock - VetoTS;
	  int tVetoMax = tVetoMin + (vSamples.size() * VetoClock);

	  if(tVetoMax < tMuon)
	    {
	      currentVetoTPI++;
	      continue;
	    }
	  else if(tMuon < tVetoMin)
	    {
	      currentVetoTPI--;
	      break;
	    }
	  else if(!((tVetoMin <= tMuon) && (tVetoMax >= tMuon)))
	    {
	      cout << "error, coincident pulse finding not working" << endl;
	      currentVetoTPI--;
	      break;
	    }
          else
	    {

	      //This should have escaped if not the right pulse
	      //I need to search for a peaks around the right time.
	      //start by getting pedestal and threshold (5*sigma?)
	  
	      float vPed = 0, vThresh = 10, vSigma = 0;
	      int vSum = 0, vSum2 = 0;

	      
	      for(std::vector<int>::iterator sIter = vSamples.begin(); sIter != vSamples.begin() + count; ++sIter)
		{
		  vSum += *sIter;
		  vSum2 += (*sIter) * (*sIter);
		}

	      vPed = vSum/count;
	      vSum2 = abs(vSum2 - (vPed * vPed * count));
	      vSigma = vSum2 - (vSum*vSum);
	      
	      //if(vSigma > 2)
	      //{
	      //  cout << "Veto is illbehaved at some point, debug further" << endl;
	      //vPed = gSetup->GetPedestal("ScVe");
	      //}


	      //if(5*vSigma > vThresh)
	      //vThresh = 5* vSigma;


	      //find the difference between tMuon and the timestamp
	      //for this signal.  Divide by the clocktick and add to
	      //the iterator.  Note I should probably look for the
	      //range rather than the signal itself.  +- 30ns to be
	      //safe

	      int tDiff = (tMuon - tVetoMin)/VetoClock;
	      int vDelta = 30/VetoClock;

	      int vBegin = tDiff - vDelta - 1;
	      int vEnd = tDiff + vDelta + 1;

	      if(vSamples.size() < tDiff + vDelta + 1)
		vEnd = vSamples.size();

	      if(vBegin <= 0)
		vBegin = 0;
	      
	      for(std::vector<int>::iterator sIter = vSamples.begin() + vBegin; sIter != vSamples.begin() + vEnd; sIter++)
		{
		  int sample = abs((*sIter) - vPed);
		  if(sample > vThresh)
		    {
		      coincidence = true;
		      Vetos++;
		      break;
		    }
		}
	      //I need to print waveform of veto if coincidence  = true
	      if((coincidence == true) && (Vetos < 51))
		{
		  //create histogram
		  
		  std::stringstream ss;
		  ss<<Vetos;
		  std::string histname = "Veto_" + ss.str();
		  std::string histtitle = "Plot of veto Signal " + ss.str();
		  TH1F* hpulse = new TH1F(histname.c_str(), histtitle.c_str(), vSamples.size(), 0, vSamples.size());
		  hpulse->GetXaxis()->SetTitle("time");
		  hpulse->GetYaxis()->SetTitle("ADC Count");

		  for(std::vector<int>::iterator sIter = vSamples.begin(); sIter != vSamples.end(); ++sIter)
		    {
		      int time2 = std::distance(vSamples.begin(), sIter);
		      int sample2 = (*sIter);
		      hpulse->Fill(time2, sample2);
		    }

		}
	      break;
	    }
	}
    }

  return 0;
}

int TestVetoMuscCoinc::AfterLastEntry(TGlobalData *gData, TSetupData *gSetup)
{
  cout << Muons << " muons seen" << endl;
  cout << Vetos << " vetos found" << endl;
  return 0;
}


      //if not found, continue

      //Later I would need to implement a veto of other channels:	  
      //loop thoough each bank looking for a corresponding time
      //These signals should be removed
