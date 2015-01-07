#include "TestVetoMuscTiming.h"
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


extern std::map<std::string, std::vector<TAnalysedPulse*> > gAnalysedPulseMap;

TestVetoMuscTiming::TestVetoMuscTiming(char *HistogramDirectoryName) :
  FillHistBase(HistogramDirectoryName)
  {
    //create histogram		  
    std::string histname = "Test TDiff";
    std::string histtitle = "Test of time difference between MuSc and Veto ";
    TH1F* hTDiff = new TH1F(histname.c_str(), histtitle.c_str(), 1000, -3000, 1000);
    hTDiff->GetXaxis()->SetTitle("time difference (ns)");
    hTDiff->GetYaxis()->SetTitle("Count");
    dir->cd("/");
  }
//initialization of histogram location and some options


TestVetoMuscTiming::~TestVetoMuscTiming(){}
//needs nothing, simple deconstructor


int TestVetoMuscTiming::ProcessEntry(TGlobalData *gData, TSetupData *gSetup)
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
  std::vector<TAnalysedPulse*>::iterator currentMuscPulse = Musc_pulses.begin();
  std::vector<TPulseIsland*>::iterator currentMuscTPI = MuscTPI.begin();



  //clocks
  int MuonClock = gSetup->GetClockTick(MuScBank);
  int VetoClock = gSetup->GetClockTick(VetoBank);

  //TimeShifts
  int MuonTS = gSetup->GetTimeShift(MuScBank);
  int VetoTS = gSetup->GetTimeShift(VetoBank);



  for(std::vector<TPulseIsland*>::iterator VetoPulseIter = VetoTPI.begin(); VetoPulseIter != VetoTPI.end(); VetoPulseIter++)
    {

      //first I should verify there is a pulse in muSc
     
      std::vector<int> vSamples = (*VetoPulseIter)->GetSamples();


      int vSum = 0, vSum2= 0, count = 10, tVeto = 0;
      float vPed = 0, threshold = 7, vSigma = 0;
      bool Veto = false;

      for(std::vector<int>::iterator sIt = vSamples.begin(); sIt != vSamples.begin() + count; ++sIt)
	{
	  vSum += *sIt;
	  vSum2 += (*sIt) * (*sIt);
	}

      vPed = vSum/count;
      vSum2 = abs(vSum2 - (vPed * vPed *count));
      vSigma = vSum2 - (vSum*vSum);

      //if(sigma > 2)
      //{
      //  cout << "MuSc is illbehaved at some point, debug further" << endl;
      //  pedestal = gSetup->GetPedestal("muSc");
      //}


      //if(5*sigma > threshold)
      //threshold = 5* sigma;

      for(std::vector<int>::iterator sIt = vSamples.begin(); sIt != vSamples.end(); ++sIt)
	{
	  int vSamp = abs((*sIt) - vPed);
	  if(vSamp > threshold)
	    {
	      Veto = true;
	      int tSamp = std::distance(vSamples.begin(), sIt);
	      int tStamp = (*VetoPulseIter)->GetTimeStamp();
	      tVeto = (tStamp + tSamp) * VetoClock - VetoTS;
	      break;
	    }
	}

      if(Veto == false)
	{
	  continue;
	  cout << "no veto found" << endl;
	}
      

     //next I need to loop through ScVe pulses

      for(std::vector<TPulseIsland*>::iterator MuscPulseIt = MuscTPI.begin(); MuscPulseIt != MuscTPI.end(); ++MuscPulseIt)
	{
	  //check timestamp and size.
	  std::vector<int> mSamples = (*MuscPulseIt)->GetSamples();

	  bool coincidence = false;
	  int MuscStamp = (*MuscPulseIt)->GetTimeStamp();
	  int tMuscMin = MuscStamp * MuonClock - MuonTS;
	  int tMuscMax = tMuscMin + (mSamples.size() * MuonClock);


	  int tDiff = (tVeto - tMuscMin)/MuonClock;

	  int mBegin = tDiff - 3000/MuonClock;
	  int mEnd = tDiff + 1000/MuonClock;


	  if(tMuscMax < (tVeto - 3000))  // this needs work
	    {
	      currentMuscTPI++;
	      continue;
	    }
	  else if((tVeto + 1000) > tMuscMin)
	    {
	      currentMuscTPI--;
	      break;
	    }
          else
	    {

	      //This should have escaped if not the right pulse
	      //I need to search for a peaks around the right time.
	      //start by getting pedestal and threshold (5*sigma?)
	  
	      float mPed = 0, mThresh = 10, mSigma = 0;
	      int mSum = 0, mSum2 = 0;

	      
	      for(std::vector<int>::iterator sIter = mSamples.begin(); sIter != mSamples.begin() + count; ++sIter)
		{
		  mSum += *sIter;
		  mSum2 += (*sIter) * (*sIter);
		}

	      mPed = mSum/count;
	      mSum2 = abs(mSum2 - (mPed * mPed * count));
	      mSigma = mSum2 - (mSum*mSum);
	      
	      //if(vSigma > 2)
	      //{
	      //  cout << "Veto is illbehaved at some point, debug further" << endl;
	      //vPed = gSetup->GetPedestal("ScVe");
	      //}


	      //if(5*vSigma > vThresh)
	      //vThresh = 5* vSigma;


	      //find the time difference between the veto and the muon
	      //Divide by the clocktick and add to the iterator.
	      //+ 100ns to be safe to -1500 ns

	      if(tMuscMin <= (tVeto - 3000))
		mBegin = (tVeto - 3000 - tMuscMin)/MuonClock;
	      if((tVeto - 3000) < tMuscMin)
		mBegin = 0;

	      if((tVeto + 1000) >= tMuscMax)
		mEnd = mSamples.size();
	      if(tMuscMax > (tVeto + 1000))
		mEnd = mSamples.size() - (tMuscMax - tVeto + 1000)/MuonClock;	      


	      for(std::vector<int>::iterator sIter = mSamples.begin() + mBegin; sIter != mSamples.begin() + mEnd; sIter++)
		{
		  int sample = abs((*sIter) - vPed);
		  if(sample > mThresh)
		    {
		      coincidence = true;
		      int tMuon = (MuscStamp + std::distance(mSamples.begin(), sIter))* MuonClock;
		      int time = tVeto - tMuon;
		      hTDiff->Fill(time);
		      break;   // need some way to deal with multiple signals
		    }
		}
	    }
	}
    }
  return 0;
}
      //if not found, continue

      //Later I would need to implement a veto of other channels:	  
      //loop thoough each bank looking for a corresponding time
      //These signals should be removed
