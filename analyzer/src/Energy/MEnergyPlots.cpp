////////////////////////////////////////////////////////////////////////////////
/// \defgroup MEnergyPlots
/// \ingroup energy
/// \author Damien Alexander
///
///////////////////////////////////////////////////////////////////////////////

/* Standard includes */
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <map>
#include <utility>
#include <sstream>
#include <cmath>
#include <algorithm>

/* MIDAS includes */
#include "midas.h"

/* ROOT includes */
#include <TH1.h>
#include <TH2.h>
#include <TDirectory.h>
#include "TF1.h"
#include "TFile.h"
#include "TApplication.h"
#include "TROOT.h"
//#include "TMath.h"
//#include "Math/Interpolator.h"

/* AlCap includes */
#include "TGlobalData.h"
#include "TSetupData.h"
#include "TPulseIsland.h"

using namespace std;

/*-- Module declaration --------------------------------------------*/
INT  MEnergyPlots_init(void);
INT  MEnergyPlots(EVENT_HEADER*, void*);
INT  MEnergyPlots_eor(INT);
INT  MEnergyPlots_BookHistograms();
float MEnergyPlots_GetEnergyAmp(std::string, int);
float MEnergyPlots_GetEnergyInt(std::string, float);
float MEnergyPlots_GetEnergyFit(std::string, double);

extern HNDLE hDB;
extern TGlobalData* gData;
extern TSetupData* gSetup;

std::map<std::string, TH1F*> IntegralEnergy_map, AmplitudeEnergy_map, FitEnergy_map;
std::map<std::string, TH2F*> IntegralVAmplitude_map;

bool EnergyPlots_firstEvent = true;

ANA_MODULE MEnergyPlots_module =
{
	"MEnergyPlots",                  /* module name           */
	"Damien Alexander",            /* author                */
	MEnergyPlots,                    /* event routine         */
	NULL,                          /* BOR routine           */
	MEnergyPlots_eor,                /* EOR routine           */
	MEnergyPlots_init,               /* init routine          */
	NULL,                          /* exit routine          */
	NULL,                          /* parameter structure   */
	0,                             /* structure size        */
	NULL,                          /* initial parameters    */
};

/////////////////////////////////////////////////////////////////

INT MEnergyPlots_init(){
  return SUCCESS;
}

INT MEnergyPlots_BookHistograms() 
{
  TDirectory* cwd = gDirectory;
  gDirectory->mkdir("EnergyPlots")->cd();

  const std::map<std::string, std::vector<TPulseIsland*> >& tpi_map = gData->fPulseIslandToChannelMap;

  for(std::map<std::string, std::vector<TPulseIsland*> >::const_iterator mIter = tpi_map.begin(); mIter != tpi_map.end(); mIter++){
    std::string bankname = mIter->first;
    std::string detname = gSetup->GetDetectorName(bankname);
    //at this point I'd like the length of TPulseIslands to set my maximum
    const std::vector<TPulseIsland*>& pulses = mIter->second;
    std::vector<TPulseIsland*>::const_iterator firstPulse = pulses.begin();
    if(pulses.size() <= 1) continue;
    firstPulse++; // just in case
    std::vector<int> samples = (*firstPulse)->GetSamples();
    int n_samples = samples.size();
    const int max_adc = std::pow(2, gSetup->GetNBits(bankname));

    float max_bin_PS = max_adc * n_samples;
    if(detname == "NdetD" || detname == "NdetU") max_bin_PS /= 20;
    else if(detname == "LaBr3") max_bin_PS /= 6;
    else if(detname == "GeCHEH" || detname == "GeCHEL") max_bin_PS /= 2;
    else continue;
    float max_amp_e = MEnergyPlots_GetEnergyAmp(detname, max_adc);
    float max_int_e = MEnergyPlots_GetEnergyInt(detname, max_bin_PS);


    //hIntegralE_[detname]_[bankname]
    std::string histname = "hIntegralE_" + detname + "_" + bankname;
    std::string histtitle = "Energy of Pulses in " + detname + " (Integral)";
    TH1F* hIntHist = new TH1F(histname.c_str(), histtitle.c_str(), max_adc, 0, max_int_e);
    hIntHist->GetXaxis()->SetTitle("Energy (MeV(ee)) ");
    hIntHist->GetYaxis()->SetTitle("Counts");
    IntegralEnergy_map[bankname] = hIntHist;

    //hAmplitudeE_[detname]_[bankname]
    histname = "hAmplitudeE_" + detname + "_" + bankname;
    histtitle = "Energy of Pulses in " + detname + " (Amplitude)";
    TH1F* hAmpHist = new TH1F(histname.c_str(), histtitle.c_str(), max_adc, 0, max_amp_e);
    hAmpHist->GetXaxis()->SetTitle("Energy (MeV(ee))");
    hAmpHist->GetYaxis()->SetTitle("Count");
    AmplitudeEnergy_map[bankname] = hAmpHist;

    //hFitE_[detname]_[bankname]
    histname = "hFitE_" + detname + "_" + bankname;
    histtitle = "Energy of Pulses in " + detname + " (Fit Amplitude)";
    TH1F* hFitHist = new TH1F(histname.c_str(), histtitle.c_str(), max_adc, 0, max_amp_e);
    hFitHist->GetXaxis()->SetTitle("Energy (MeV(ee))");
    hFitHist->GetYaxis()->SetTitle("Count");
    FitEnergy_map[bankname] = hFitHist;

    histname = "hIntegralVAmp" + bankname;
    histtitle = "Integral vs Amplitude for " + bankname;
    TH2F* hIntVAmp = new TH2F(histname.c_str(), histtitle.c_str(), max_adc/4, 0, max_int_e, max_adc/4, 0, max_amp_e);
    hIntVAmp->GetYaxis()->SetTitle("Amplitude (pedestal_subtracted)");
    hIntVAmp->GetXaxis()->SetTitle("Integral (pedestal_subtracted)");
    IntegralVAmplitude_map[bankname] = hIntVAmp;

  }

  cwd->cd();
  return SUCCESS;
}

INT MEnergyPlots_eor(INT run_number)
{
  return SUCCESS;  // currently nothing to do here
}

INT MEnergyPlots(EVENT_HEADER *pheader, void *pevent)
{
  if(EnergyPlots_firstEvent == true){
    MEnergyPlots_BookHistograms();
    EnergyPlots_firstEvent = false;
  }

  const std::map<std::string, std::vector<TPulseIsland*> >& tpi_map = gData->fPulseIslandToChannelMap;

  for(std::map<std::string, std::vector<TPulseIsland*> >::const_iterator mIter = tpi_map.begin(); mIter != tpi_map.end(); mIter++){
    const std::string bankname = mIter->first;
    const std::string detname = gSetup->GetDetectorName(bankname);
    const std::vector<TPulseIsland*>& pulses = mIter->second;
    int polarity = gSetup->GetTriggerPolarity(bankname);
    int nbits = gSetup->GetNBits(bankname);

    if(detname != "TSc"
       && detname != "GeCHEH"
       && detname != "GeCHEL"
       && detname != "NdetD"
       && detname != "NdetU"
       && detname != "LaBr3")
      continue;


   
    //int threshold = 60;  //threshold (from baseline) for integration
    //float pct = 0.2;

    for(std::vector<TPulseIsland*>::const_iterator pIter = pulses.begin(); pIter != pulses.end(); pIter++){
      if(IntegralEnergy_map.find(bankname) == IntegralEnergy_map.end()) continue;

      float integral = 0;
      float integral_ps = 0;
      bool overflow = false;
      float max = 0;
      int tMax = 0;
      //bool test = false;
      float pedBegin = 0, pedEnd = 0, pedBeginSamp = 0, pedEndSamp = 0;
      float pedSlope = 0, pedInt = 0, pedMid = 0;

      const std::vector<int>& samples = (*pIter)->GetSamples();
      const int nSamp = samples.size();

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
       
      if(samples.size() < 8) continue;
      TH1F* hPulse = new TH1F("hPulse", "Waveform", samples.size(),0, samples.size());

      int nInt = 0;
      int lastSamp = 0;
      for(int i = 0; i+2 < nSamp; i += 2){
	if(samples.at(i) == 0 || samples.at(i+1) == 0) overflow = true;
	if(samples.at(i) == std::pow(2, nbits) || samples.at(i+1) == std::pow(2, nbits)) overflow = true;
	integral += (samples.at(i) + (4*samples.at(i+1)) + samples.at(i+2))/3;
	nInt += 2;
	lastSamp = i+2;
	float tmp = polarity * (samples.at(i) - (pedSlope*i + pedBegin));
	hPulse->SetBinContent(i, tmp);
	if(tmp > max){
	  max = tmp; tMax = i;
	}

	tmp = polarity * (samples.at(i+1) - (pedSlope*(i+1) + pedBegin));
	hPulse->SetBinContent(i+1, tmp);
	if(tmp > max){
	  max = tmp; tMax = i+1;
	}
      }
      if(nSamp % 2 == 0){
	integral += 0.5 * (samples.at(lastSamp) + samples.at(lastSamp + 1));
	float tmp = polarity * (samples.at(lastSamp+1) - (pedSlope*(lastSamp+1) + pedBegin));
	hPulse->SetBinContent(lastSamp+1, tmp);
	nInt ++;
	lastSamp++;
      }
      if(nSamp % 2 == 1){
	std::cout << "Non-modulo 2 check" << std:: endl;
	integral += 0.5 * (samples.at(nSamp-1) + samples.at(nSamp));
	nInt++;
      }
      integral_ps = polarity * (integral - pedInt);

      //////////////   fit ////////////////////////
      /*
    
      TF1* fit1 = new TF1("fit1", "gaus", 0, samples.size());
      fit1->SetParameters(max, tMax, 2);
      hPulse->Fit("fit1", "Q", "", tMax - 2, tMax + 2);
      double fit_max = fit1->GetParameter(0);
      delete hPulse;
      delete fit1;
      */   
      //////////////  Fill Hists  ///////////////////////
      delete hPulse;
      if(overflow) continue;

      float energy_amp = MEnergyPlots_GetEnergyAmp(detname, max);
      float energy_int = MEnergyPlots_GetEnergyInt(detname, integral_ps);
      //float energy_fit = MEnergyPlots_GetEnergyFit(detname, fit_max);

      if(energy_int < 0.025) continue;

      IntegralEnergy_map[bankname]->Fill(energy_int);
      AmplitudeEnergy_map[bankname]->Fill(energy_amp);
      //FitEnergy_map[bankname]->Fill(energy_fit);
      IntegralVAmplitude_map[bankname]->Fill(energy_int, energy_amp);

    }
  }

  return SUCCESS;
}

float MEnergyPlots_GetEnergyAmp(std::string detname, int amp){
  float energy = 0;
  if(detname == "NdetD"){ energy = (amp * 0.0003999) + 0.008234;  }
  if(detname == "NdetU"){ energy = (amp * 0.0004015) + 0.009037;  }
  if(detname == "GeCHEH"){ energy = (amp * 0.0001522) - 0.0004239;  }
  if(detname == "GeCHEL"){ energy = (amp * 0.0003858) - 0.0017;    }
  if(detname == "LaBr3"){ energy = (amp * 0.00163022) - 0.00836618;  } 

  return energy;
}



float MEnergyPlots_GetEnergyInt(std::string detname, float Int){
  float energy = 0;
  if(detname == "NdetD"){ energy = (0.0000686 * Int) + 0.02117;  }
  if(detname == "NdetU"){ energy = (0.00006766 * Int) + 0.02358;  }
  if(detname == "GeCHEH"){ energy = 0.000002033 * Int;  }
  if(detname == "GeCHEL"){ energy = 0.000005145 * Int;  }
  if(detname == "LaBr3"){ energy = 0.0000823 * Int;  } 
  return energy;
}

float MEnergyPlots_GetEnergyFit(std::string detname, double fit_amp){
  float energy = 0;
  if(detname == "NdetD"){ energy = (fit_amp * 0.0003914) + 0.0289;  }
  if(detname == "NdetU"){ energy = (fit_amp * 0.0004381) + 0.0257;  }
  if(detname == "GeCHEH"){ energy = (fit_amp * 0.0001522) - 0.0004239;  }
  if(detname == "GeCHEL"){ energy = (fit_amp * 0.0003858) - 0.0017;    }
  if(detname == "LaBr3"){ energy = (fit_amp * 0.00163022) - 0.00836618;  } 

  return energy;
}
