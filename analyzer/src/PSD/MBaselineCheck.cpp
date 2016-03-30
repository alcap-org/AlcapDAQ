////////////////////////////////////////////////////////////
/// \defgroup MBaselineCheck
/// \author Damien Alexander
///
/// \breif
/// Plots the calculated pedestal for each waveform along
/// with the projected pedestal for each waveform
////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <map>
#include <sstream>
#include <algorithm>
#include <cmath>

#include "midas.h"

#include "TH1.h"
#include "TH2.h"
#include "TH3.h"
#include "TDirectory.h"
#include "TFile.h"

#include "TGlobalData.h"
#include "TSetupData.h"
#include "TPulseIsland.h"

using std::string;
using std::map;
using std::vector;

INT MBaselineCheck_init(void);
INT MBaselineCheck_bor(INT);
INT MBaselineCheck(EVENT_HEADER*, void*);

extern TGlobalData* gData;
extern TSetupData* gSetup;

namespace{
  int runNumber;
  map<std::string, TH2D*> PedvRun, BasevRun;
  map<std::string, TH2D*> PeakvRun;
  //map<std::string, TH1F*> Pedestal, Baseline;
}

ANA_MODULE MBaselineCheck_module =
{
	"MBaselineCheck",              /* module name           */
	"Damien Alexander",            /* author                */
	MBaselineCheck,                /* event routine         */
	MBaselineCheck_bor,            /* BOR routine           */
	NULL,                          /* EOR routine           */
	MBaselineCheck_init,           /* init routine          */
	NULL,                          /* exit routine          */
	NULL,                          /* parameter structure   */
	0,                             /* structure size        */
	NULL,                          /* initial parameters    */
};


//////////////////////////////////////////////////////////////////
INT MBaselineCheck_init(){
  //initialize the histograms

  //set Directory(?)
  TDirectory* cwd = gDirectory;
  gDirectory->mkdir("BaselineCheck")->cd();

  std::map<std::string, std::string>Bank2DetMap=gSetup->fBankToDetectorMap;

  for(std::map<std::string, std::string>::iterator mIter = Bank2DetMap.begin(); mIter!= Bank2DetMap.end(); mIter++){

    std::string bankname = mIter->first;
    std::string detname = mIter->second;
    int n_bits = gSetup->GetNBits(bankname);
    int max_adc = std::pow(2, n_bits);
    if(gSetup->IsTDC(bankname) ) continue;
    /*
    std::string histname = "hPedestal_"+detname+"_"+bankname;
    std::string histtitle = "TPI calculated Pedestal for hits in " + detname;
    TH1F* hPedestal = new TH1F(histname.c_str(), histtitle.c_str(), max_adc/4, 0, max_adc);
    hPedestal->GetXaxis()->SetTitle("Pedestal value (adc_counts)");
    hPedestal->GetYaxis()->SetTitle("count");
    Pedestal[bankname]=hPedestal;

    histname = "hBaseline_"+detname+"_"+bankname;
    histtitle = "Projected baseline for hits in " + detname;
    TH1F* hBaseline = new TH1F(histname.c_str(), histtitle.c_str(), 800, max_adc - 800, max_adc);
    hBaseline->GetXaxis()->SetTitle("Baseline value (adc_counts)");
    hBaseline->GetYaxis()->SetTitle("count");
    Baseline[bankname]=hBaseline;
    */

    int pedStart = max_adc - 2000;
    int pedEnd = max_adc;
    if(detname == "GeCHEH" || detname == "GeCHEL"){
      pedStart = 0;
      pedEnd = 2000; 

    std::string histname = "hPedvRun_"+detname+"_"+bankname;
    std::string histtitle = "Pedestal as average of first 8 samples by run for " + detname;
    if(detname == "TSc" || detname == "GeCHT")
      std::string histtitle = "Pedestal as average of first 4 samples by run for " + detname;
    TH2D* hPedvRun = new TH2D(histname.c_str(), histtitle.c_str(), 
			      2000, pedStart, pedEnd, 1450,5976, 7426);
    hPedvRun->GetXaxis()->SetTitle("Pedestal (adc value)");
    hPedvRun->GetYaxis()->SetTitle("Run Number");
    PedvRun[bankname]=hPedvRun;

    histname = "hBasevRun_"+detname+"_"+bankname;
    histtitle = "Pedestal as most likely value by run for " + detname;
    TH2D* hBasevRun = new TH2D(histname.c_str(), histtitle.c_str(),
			       2000, pedStart, pedEnd, 1450, 5976, 7426);
    hBasevRun->GetXaxis()->SetTitle("Pedestal (adc value)");
    hBasevRun->GetYaxis()->SetTitle("Run Number");
    BasevRun[bankname]=hBasevRun;


    if(detname == "SyncCrate7"){


      histname = "hPeakvRun_"+detname;
      histtitle = "Peak value for " + detname;
      TH2D* hPeakvRun = new TH2D(histname.c_str(), histtitle.c_str(), 
				 3000, 8000, 11000, 1450, 5976, 7426);
      hPeakvRun->GetXaxis()->SetTitle("pulse height (adc_counts)");
      hPeakvRun->GetYaxis()->SetTitle("Run Number");
      PeakvRun[bankname]=hPeakvRun;
    }

  }
  cwd->cd();
  return SUCCESS;
}

INT MBaselineCheck_bor(int run_number)
{
  runNumber = run_number;
  return 0;
}

INT MBaselineCheck(EVENT_HEADER *pheader, void *pevent){
  const std::map<std::string, std::vector<TPulseIsland*> >& wfd_map = gData->fPulseIslandToChannelMap;

  for(std::map<std::string, std::vector<TPulseIsland*> >::const_iterator mIter = wfd_map.begin(); mIter != wfd_map.end(); mIter++){
    std::string bankname = mIter->first;
    const std::vector<TPulseIsland*>& Pulses = mIter->second;
    int polarity = gSetup->GetTriggerPolarity(bankname);
    int nbits = gSetup->GetNBits(bankname);
    int max_adc = std::pow(2, nbits);
    std::string detname = gSetup->GetDetectorName(bankname);
   
    float pct = 0.1;

    for(std::vector<TPulseIsland*>::const_iterator pIter = Pulses.begin(); pIter != Pulses.end(); pIter++){
      float nAvg = 8;
      if(detname == "TSc" || detname == "GeCHT") nAvg = 4;
      float pedestal = (*pIter)->GetPedestal(nAvg);  //avg of first 8 samples
      std::vector<int> Samples = (*pIter)->GetSamples();
      const int size = 16384;
      int count[size] = {0};



      for(std::vector<int>::iterator sIter=Samples.begin(); sIter!=Samples.end(); sIter++)
	count[*sIter] += 1;
      int baseline_loc = 0;
      for(int i =0; i< size; i++)
	if(count[i] > count[baseline_loc])
	  baseline_loc = i;

      PedvRun[bankname]->Fill(pedestal, runNumber);
      BasevRun[bankname]->Fill(baseline_loc, runNumber);

      if(detname != "SyncCrate7") continue;


      //avg adc values from 27 to 35
      float max = 0;
      int nCount = 8;
      for(int i = 27; i< 27 + nCount; i++)
	{
	  max += Samples.at(i);
	}
      max = max / nCount;

      PeakvRun[bankname]->Fill(max, runNumber);


      /*
      float integral = 0;
      float integral_ps = 0;
      float lInt = 0, sInt = 0, ratio = 0;
      float lIntPed = 0, sIntPed = 0;
      float tLstart = 0, tSstart = 0, tstop = 0;
      bool overflow = false;
      int max = std::pow(2, nbits), tMax = 0;
      float pedBegin = 0, pedEnd = 0, pedBeginSamp = 0, pedEndSamp = 0;
      float pedSlope = 0, pedInt = 0, pedMid = 0;


      const std::vector<int>& samples = (*pIter)->GetSamples();
      const int nSamp = samples.size();

      //////////////Pedestal /////////////////////////////////////

      for(std::vector<int>::const_iterator pulse_begin = samples.begin() + 1; pulse_begin != samples.begin() + 9; pulse_begin++){
	pedBegin += (*pulse_begin);
	pedBeginSamp += 1;
      }
      for(std::vector<int>::const_iterator pulse_end = samples.end() - 9; pulse_end != samples.end(); pulse_end++){
	pedEnd += (*pulse_end);
	pedEndSamp += 1;
      }
      pedBegin /= pedBeginSamp;
      pedEnd /= pedEndSamp;
      pedSlope = (pedEnd - pedBegin)/(float)nSamp;

      //integrate the pedestal
      pedMid = pedSlope * ((float)nSamp/2) + pedBegin;
      if(detname == "TSc"){
	pedBegin = (samples.at(0) + samples.at(1) + samples.at(2) + samples.at(3))/4;
	pedMid = pedBegin; pedEnd = pedBegin;   
      }
      pedInt = (float)(nSamp-1)/6 * (pedBegin + (4*pedMid) + pedEnd);

      //do each of these three integrations separately,

      //////////////full integral/////////////////////////////////
       
      for(int i = 0; i+2 < nSamp; i += 2){
	if((samples.at(i) == 0 || samples.at(i+1) == 0) || (samples.at(i) == max_adc || samples.at(i+1) == max_adc)){ 
	  overflow = true; break;
	}
	integral += (samples.at(i) + (4*samples.at(i+1)) + samples.at(i+2))/3;
	if(samples.at(i) < max){
	  max = samples.at(i); tMax = i;
	}
	if(samples.at(i+1) < max){
	  max = samples.at(i+1); tMax = i+1;
	}
      }
      if(nSamp % 2 == 0) //need to include last point
	integral += 0.5 * (samples.at(nSamp-2) + samples.at(nSamp-1));
      integral_ps = polarity * (integral - pedInt);

      float max_ps = ((pedSlope * tMax) + pedBegin) - max; // polarity -1

      /*
      if(detname != "NdetD" && detname != "NdetU") continue;

      ///////////////////set times///////////////////////////////////
      /*
      for(int j=tMax; j>0; j--){
	float pedestal = (pedSlope * j) + pedBegin;

	float tmp = pedestal - samples.at(j); //polarity -1
	if(tmp < pct * max_ps){//below CF threshold
	  //interpolate the point to CF, linear, ensure pedestal subtraction
	  float slope = 0;
	  if(samples.at(j) != samples.at(j+1))
	    slope = 1/(samples.at(j) - samples.at(j+1)); // polarity -1
	  float h1 = (pct*max_ps)  - (tmp);
	  tLstart = j + h1*slope;
	  break;
	}
      }
      */
      /*
      tLstart = tMax - 7;
      if(tLstart <= 2) continue;
      if(tLstart > nSamp - 3) continue; //late pulse
      
      //float tailOffset = 10 * (integral_ps) / (857 + integral_ps);

      //tSstart = (float)tMax + tailOffset;
      tSstart = tMax + 9;
      if(tSstart < 0) continue;

      //tstop = (float)tMax + tailOffset * 6;
      tstop = tMax + 81;
      if(tstop >= nSamp) continue; 

      //std::cout << tLstart << "   " << tSstart << "   " << tstop << std::endl;


      /////////////////long gate//////////////////////////////////////

      //integrate first sample (trapezoidal with linear interpolation)
      int lastSamp = 0;
      //integrate bulk, Simpson's rule
      for(int i = tLstart; i+2 <= tstop; i += 2){
	lInt += (samples.at(i) + (4*samples.at(i+1)) + samples.at(i+2))/3;
	lIntPed += 2 * (pedSlope*(i+1) + pedBegin);
	lastSamp = i+2;
      }
      //assume pulse tail not at endpoint, linearly interpolate last point
      if(lastSamp != tstop){
	lInt += samples.at(tstop);
	lIntPed += pedSlope*(tstop) + pedBegin;
      }

      lInt -= lIntPed;
      lInt *= polarity;

      ///////////////short gate///////////////////////////////////

      //integrate bulk, Simpson's rule
      for(int i = tSstart; i+2 <= tstop; i += 2){
	sInt += (samples.at(i) + (4*samples.at(i+1)) + samples.at(i+2))/3;
	sIntPed += 2 * (pedSlope*(i+1) + pedBegin);
	lastSamp = i+2;
      }
      //integrate last, trapezoidal with linear interpolation
      if(lastSamp != tstop){
	sInt += samples.at(tstop);
	sIntPed += pedSlope*(tstop) + pedBegin;
      }

      sInt -= sIntPed;
      sInt *= polarity;

      /////////////////////////////////////////////////////////

      if(overflow) continue;


      //fill the histograms
      ratio = sInt/lInt;
      float PedDelta = pedBegin - pedEnd;

      Delta[bankname]->Fill(PedDelta, max_ps);
      DeltaRatio[bankname]->Fill(PedDelta, ratio);
      DeltavPSD[bankname]->Fill(pedestal, ratio);
      */

			       
    }

      
  }
  return SUCCESS;
}

