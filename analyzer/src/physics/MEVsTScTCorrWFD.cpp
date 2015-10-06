////////////////////////////////////////////////////////////////////////////////
/// \defgroup MEVsTScTCorrWFD
///
/// \brief
/// Timing correlations from within WFD plotted against energy
/// @{
////////////////////////////////////////////////////////////////////////////////

/* Standard includes */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string>
#include <map>
#include <algorithm>
#include <cmath>

/* MIDAS includes */
#include "midas.h"

/* ROOT includes */
#include "TH2D.h"
#include "TDirectory.h"

/* AlCap includes */
#include "TGlobalData.h"
#include "TSetupData.h"
#include "AlCap.h"


/*-- Module declaration --------------------------------------------*/
static INT MEVsTScTCorrWFD_init(void);
static INT MEVsTScTCorrWFD(EVENT_HEADER*, void*);
float MEVsTScTCorrWFD_Energy(std::string, float);
float MEVsTScTCorrWFD_IntEnergy(std::string, float);
float MEVsTScTCorrWFD_nSamples(std::string);

extern HNDLE hDB;
extern TGlobalData* gData;
extern TSetupData* gSetup;

using namespace AlCap;
namespace {
  const double TIME_LOW = -4e3, TIME_HIGH = 8e3;
  TH2D* vhEVsTScTCorrWFD[NCRATE][MAXNCHANWFD];
  TH2D* vhIEVsTScTCorrWFD[NCRATE][MAXNCHANWFD];
  std::string WFDBANKS[NCRATE][MAXNCHANWFD];
}

ANA_MODULE MEVsTScTCorrWFD_module =
{
  "MEVsTScTCorrWFD",       /* module name           */
  "Damien Alexander",      /* author                */
  MEVsTScTCorrWFD,         /* event routine         */
  NULL,                    /* BOR routine           */
  NULL,                    /* EOR routine           */
  MEVsTScTCorrWFD_init,    /* init routine          */
  NULL,                    /* exit routine          */
  NULL,                    /* parameter structure   */
  0,                       /* structure size        */
  NULL,                    /* initial parameters    */
};

/*--module init routine --------------------------------------------*/
INT MEVsTScTCorrWFD_init() {
  TDirectory* cwd = gDirectory;
  gDirectory->mkdir("EVsTDiff")->cd();

  for(int icrate = 0; icrate < NCRATE; ++icrate){
    for(int ich = 0; ich < NCHANWFD[icrate]; ++ich) {
      char bank[5]; sprintf(bank, "D%d%02d", icrate, ich);
      std::string det = gSetup->GetDetectorName(bank);
      int nBits = gSetup->GetNBits(bank);
      int max_adc = std::pow(2, nBits);
      float max_amp_e = MEVsTScTCorrWFD_Energy(det, max_adc);
      float max_int = max_adc * MEVsTScTCorrWFD_nSamples(det); 
      float max_int_e = MEVsTScTCorrWFD_IntEnergy(det, max_int);

      char histname[64]; sprintf(histname, "hEVsTDiff_%s", bank);
      char histtitle[64]; sprintf(histtitle, "Energy vs TDiff for %s", det.c_str());
      vhEVsTScTCorrWFD[icrate][ich] = new TH2D(histname, histtitle, 2000, TIME_LOW, TIME_HIGH, max_adc/8, 0, max_amp_e);
      vhEVsTScTCorrWFD[icrate][ich]->GetXaxis()->SetTitle("TSc TDiff (ns)");
      vhEVsTScTCorrWFD[icrate][ich]->GetYaxis()->SetTitle("Energy (MeVee)");

      sprintf(histname, "hIEVsTDiff_%s", bank);
      sprintf(histtitle, "Energy (integral) vs TDiff for %s", det.c_str());
      vhIEVsTScTCorrWFD[icrate][ich] = new TH2D(histname, histtitle, 2000, TIME_LOW, TIME_HIGH, max_adc/8, 0, max_int_e);
      vhIEVsTScTCorrWFD[icrate][ich]->GetXaxis()->SetTitle("TSc TDiff(ns)");
      vhIEVsTScTCorrWFD[icrate][ich]->GetYaxis()->SetTitle("Energy (Integral) (MeVee)"); 

      WFDBANKS[icrate][ich] = bank;
    }
  }
  cwd->cd();
  return SUCCESS;
}


/*-- module event routine -----------------------------------------*/
INT MEVsTScTCorrWFD(EVENT_HEADER *pheader, void *pevent) {
  const std::map< std::string, std::vector<TPulseIsland*> >& wfd_map = gData->fPulseIslandToChannelMap;
  const std::string ref_bank = gSetup->GetBankName("TSc");
  if (!wfd_map.count(ref_bank)) {
    printf("MEVsTScTCorrWFD: No reference hits TSc!\n");
    return SUCCESS;
  }

  const std::vector<TPulseIsland*>& ref_pulses = wfd_map.at(ref_bank);
  const static double tickref = TICKWFD[ref_bank[1]-'0'];

  for(int icrate = 0; icrate < NCRATE; ++icrate) {
    for(int ich = 0; ich < NCHANWFD[icrate]; ++ich) {
      if(!wfd_map.count(WFDBANKS[icrate][ich])) continue;
      std::string detname = gSetup->GetDetectorName(WFDBANKS[icrate][ich]);
      const int polarity = gSetup->GetTriggerPolarity(WFDBANKS[icrate][ich]);
      const int nBits = gSetup->GetNBits(WFDBANKS[icrate][ich]);

      const std::vector<TPulseIsland*>& pulses = wfd_map.at(WFDBANKS[icrate][ich]);
      for(int i = 0, j0 = 0; i < pulses.size(); ++i){
	//////// Analyse the pulses get max adc and int at least
	// Need to consider other timing methods
	//walk is low for  GeCHT, LaBr3 and Ndets

	const std::vector<int> samples = pulses[i]->GetSamples();
	const int nSamp = samples.size();

	float integral = 0;
	float integral_ps = 0;
	bool overflow = false;
	float max = 0;
	int tMax = 0;
	float pedBegin = 0, pedEnd = 0, pedBeginSamp = 0, pedEndSamp = 0;
	float pedSlope = 0, pedInt = 0, pedMid = 0;

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

	if(samples.size() < 8) continue;
	TH1F* hPulse = new TH1F("hPulse", "Waveform", samples.size(),0, samples.size());

	int nInt = 0;
	int lastSamp = 0;
	for(int k = 0; k+2 < nSamp; k += 2){
	  if(samples.at(k) == 0 || samples.at(k+1) == 0) overflow = true;
	  if(samples.at(k) == std::pow(2, nBits) || samples.at(k+1) == std::pow(2, nBits)) overflow = true;
	  integral += (samples.at(k) + (4*samples.at(k+1)) + samples.at(k+2))/3;
	  nInt += 2;
	  lastSamp = k+2;
	  float tmp = polarity * (samples.at(k) - (pedSlope*k + pedBegin));
	  hPulse->SetBinContent(k, tmp);
	  if(tmp > max){
	    max = tmp; tMax = k;
	  }

	  tmp = polarity * (samples.at(k+1) - (pedSlope*(k+1) + pedBegin));
	  hPulse->SetBinContent(k+1, tmp);
	  if(tmp > max){
	    max = tmp; tMax = k+1;
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

	delete hPulse;
	if(overflow) continue;

	//compare to TSc time
	const Double_t t = TICKWFD[icrate] * pulses[i]->GetTimeStamp();
	float energy = MEVsTScTCorrWFD_Energy(detname, max);
	float energy_int = MEVsTScTCorrWFD_IntEnergy(detname, integral_ps);
	float toff = 0;
	if(detname == "GeCHEH" || detname == "GeCHEL") toff = 15000;

	for(int j=j0; j< ref_pulses.size(); ++j){
	  const double dt = t - tickref* ref_pulses[j]->GetTimeStamp() - toff;
	if(dt < TIME_LOW)
	  break;
	else if(dt < TIME_HIGH){
	  vhEVsTScTCorrWFD[icrate][ich]->Fill(dt, energy);
	  vhIEVsTScTCorrWFD[icrate][ich]->Fill(dt, energy_int);
	}
	else ++j0;
	}//j loop (ref bank)
      }//i loop (wfd pulses)
    }//ich
  }//icrate
  return SUCCESS;
}


float MEVsTScTCorrWFD_Energy(std::string detname, float amp){
  float energy = 0;
  if(detname == "NdetD"){ energy = (amp * 0.0003999) + 0.008234;  }
  else if(detname == "NdetU"){ energy = (amp * 0.0004015) + 0.009037;  }
  else if(detname == "GeCHEH"){ energy = (amp * 0.0001522) - 0.0004239;  }
  else if(detname == "GeCHEL"){ energy = (amp * 0.0003858) - 0.0017;    }
  else if(detname == "LaBr3"){ energy = (amp * 0.00163022) - 0.00836618;  } 
  else energy = amp;
  return energy;
}

float MEVsTScTCorrWFD_IntEnergy(std::string detname, float Int){
  float energy = 0;
  if(detname == "NdetD"){ energy = (0.0000686 * Int) + 0.02117;  }
  else if(detname == "NdetU"){ energy = (0.00006766 * Int) + 0.02358;  }
  else if(detname == "GeCHEH"){ energy = 0.000002033 * Int;  }
  else if(detname == "GeCHEL"){ energy = 0.000005145 * Int;  }
  else if(detname == "LaBr3"){ energy = 0.0000823 * Int;  } 
  else energy = Int;
  return energy;
}

float MEVsTScTCorrWFD_nSamples(std::string detname){
  float nSamples = 0;
  if(detname == "TSc") nSamples = 6;//12
  else if(detname == "NdetD" || detname == "NdetU") nSamples = 8;//150
  else if(detname == "GeCHEH" || detname == "GeCHEL") nSamples = 200;//300
  else if(detname == "LaBr3") nSamples = 28;//128
  else nSamples = 100;
  return nSamples;
}
