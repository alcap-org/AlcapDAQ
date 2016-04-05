/*Standard includes */
#include <cstdio>
#include <cstdlib>
#include <string>
#include <map>
#include <utility>
#include <cmath>
#include <algorithm>

/* MIDAS includes */
#include "midas.h"

/* ROOT includes */
#include <TH1F.h>
#include <TH2F.h>
#include <TDirectory.h>

/* AlCap includes */
#include "AlCap.h"
#include "TGlobalData.h"
#include "TSetupData.h"

using std::string;
using std::map;
using std::vector;
using std::pair;

/*-- Module declaration --------------------------------------------*/
static INT MTCorrTest_init(void);
static INT MTCorrTest(EVENT_HEADER*, void*);
//float MTCorrTest_Energy(std::string, float);
//float MTCorrTest_IntEnergy(std::string, float);
float MTCorrTest_GetEnergyFit(std::string, double);
float MTCorrTest_nSamples(std::string);
float MTCorrTest_Threshold(std::string);
bool MTCorrTest_Neutron(std::string, double, float);


extern HNDLE hDB;
extern TGlobalData* gData;
extern TSetupData* gSetup;


using namespace AlCap;
namespace {
  std::string WFDBANKS[NCRATE][MAXNCHANWFD];
  std::string TDCBANKS[NCRATE][MAXNCHANWFD];
  //TH2D* vvhTCorrTest_PHvTDiff[NCRATE][MAXNCHANWFD];
  //TH2D* vvhTCorrTest_EvTDiff[NCRATE][MAXNCHANWFD];
  TH2D* vvhTCorrTest_FEvTDiff[NCRATE][MAXNCHANWFD];
  //TH2D* vvhTCorrTest_IEvTDiff[NCRATE][MAXNCHANWFD];
  TH2D* vvhTCorrTest_PSDCut[NCRATE][MAXNCHANWFD];
  TH2D* vvhTCorrTest_PSD[NCRATE][MAXNCHANWFD];
  TH2D* vvhTCorrTest_EvTDiffG[NCRATE][MAXNCHANWFD];
  const double TIME_LOW = -2e3, TIME_HIGH = 8e3;
}


ANA_MODULE MTCorrTest_module =
{
  "MTCorrTest",        /* module name           */
  "Damien Alexander",  /* author                */
  MTCorrTest,          /* event routine         */
  NULL,                /* BOR routine           */
  NULL,                /* EOR routine           */
  MTCorrTest_init,     /* init routine          */
  NULL,                /* exit routine          */
  NULL,                /* parameter structure   */
  0,                   /* structure size        */
  NULL,                /* initial parameters    */
};


INT MTCorrTest_init() {
  TDirectory* cwd = gDirectory;
  gDirectory->mkdir("TCorrTest")->cd();
  //select channels and corresponding vetos
  for(int icrate = 0; icrate < NCRATE; ++icrate){
  
    for(int ich = 0; ich < NCHANWFD[icrate]; ++ich) {
      
      char bank[5]; sprintf(bank, "D%d%02d", icrate, ich);
      std::string det = gSetup->GetDetectorName(bank);

      if(det == "NdetD" || det == "NdetU" || det == "LaBr3"){
	WFDBANKS[icrate][ich] = bank;
	//TDCBANKS[icrate][ich] = gSetup->GetBankName("T" + det);
      }
      else if(det == "GeCHEH" || det == "GeCHEL"){
	WFDBANKS[icrate][ich] = bank;
	//TDCBANKS[icrate][ich] = gSetup->GetBankName("TGeCHT");
      }
      else if(det == "TSc") {
	WFDBANKS[icrate][ich] = bank;
	//TDCBANKS[icrate][ich] = gSetup->GetBankName("TTSc");
      }
      else{
	WFDBANKS[icrate][ich] = "";
	//TDCBANKS[icrate][ich] = "";
	continue;  // ignoring SYNC pulses, Rollover, LYSO, GeCHT, and empty
      }

      int nBits = gSetup->GetNBits(bank);
      int max_adc = std::pow(2, nBits);
      //float max_amp_e = MTCorrTest_Energy(det, max_adc);
      //float max_int = max_adc * MTCorrTest_nSamples(det); 
      //float max_int_e = MTCorrTest_IntEnergy(det, max_int);

      float max_fit = 7.5;
      if(det == "GeCHEH") max_fit = 2.5;
      if(det == "GeCHEL") max_fit = 6.0;
      if(det == "TSc") max_fit = 15000.0;
      if(det == "LaBr3") max_fit = 15.0;

      
      //setup histograms

      ///////////// E vs TDiff  //////////////////////
      char histname[64];   char histtitle[64]; 
      /*
      sprintf(histname, "hTCorrTest_EvTDiff_%s", det.c_str());
      sprintf(histtitle, "Energy vs TSC TDiff for %s", det.c_str());
      vvhTCorrTest_EvTDiff[icrate][ich] = new TH2D(histname, histtitle, (TIME_HIGH - TIME_LOW)/14, TIME_LOW, TIME_HIGH, max_adc/4, 0, max_amp_e);
      vvhTCorrTest_EvTDiff[icrate][ich]->GetXaxis()->SetTitle("TDiff (TDC) (ns)");
      vvhTCorrTest_EvTDiff[icrate][ich]->GetYaxis()->SetTitle("Energy (amplitude) (MeV)");
      */

      ///////////// FE v TDiff //////////////////////
      sprintf(histname, "hTCorrTest_FEvTDiff_%s", det.c_str());
      sprintf(histtitle, "Fit Energy vs TSC TDiff for %s", det.c_str());
      vvhTCorrTest_FEvTDiff[icrate][ich] = new TH2D(histname, histtitle, (TIME_HIGH - TIME_LOW)/5, TIME_LOW, TIME_HIGH, 7500, 0, max_fit);
      vvhTCorrTest_FEvTDiff[icrate][ich]->GetXaxis()->SetTitle("TDiff (TDC) (ns)");
      vvhTCorrTest_FEvTDiff[icrate][ich]->GetYaxis()->SetTitle("Energy (fit) (MeV)");

      ///////////// IE v TDiff //////////////////////
      /*
      sprintf(histname, "hTCorrTest_IEvTDiff_%s", det.c_str());
      sprintf(histtitle, "Integral Energy vs TSC TDiff for %s", det.c_str());
      vvhTCorrTest_IEvTDiff[icrate][ich] = new TH2D(histname, histtitle, (TIME_HIGH - TIME_LOW)/14, TIME_LOW, TIME_HIGH, max_adc/2, 0, max_int_e);
      vvhTCorrTest_IEvTDiff[icrate][ich]->GetXaxis()->SetTitle("TDiff (TDC) (ns)");
      vvhTCorrTest_IEvTDiff[icrate][ich]->GetYaxis()->SetTitle("Energy (integral) (MeV)");
      */
      /*
      ///////////// PH v TDiff //////////////////////
      sprintf(histname, "hTCorrTest_PHvTDiff_%s", det.c_str());
      sprintf(histtitle, "Pulse Height vs TSC TDiff for %s", det.c_str());
      vvhTCorrTest_PHvTDiff[icrate][ich] = new TH2D(histname, histtitle, (TIME_HIGH - TIME_LOW)/20, TIME_LOW, TIME_HIGH, max_adc/2, 0, max_adc);
      vvhTCorrTest_PHvTDiff[icrate][ich]->GetXaxis()->SetTitle("TDiff (TDC) (ns)");
      vvhTCorrTest_PHvTDiff[icrate][ich]->GetYaxis()->SetTitle("Pulse Height (fit)");
      */

      //PSD after time cuts/vetos
      if(det == "NdetD" || det == "NdetU"){
	sprintf(histname, "hTCorrTest_PSDCut_%s", det.c_str());
	sprintf(histtitle, "Pulse Height vs PSD parameter for %s", det.c_str());
	vvhTCorrTest_PSDCut[icrate][ich] = new TH2D(histname, histtitle, 7500, 0, max_fit, 600, 0, 0.45);
	vvhTCorrTest_PSDCut[icrate][ich]->GetXaxis()->SetTitle("Energy (fit)");
	vvhTCorrTest_PSDCut[icrate][ich]->GetYaxis()->SetTitle("PSD parameter");

	sprintf(histname, "hTCorrTest_PSD_%s", det.c_str());
	sprintf(histtitle, "Pulse Height (Fit) vs PSD parameter for %s", det.c_str());
	vvhTCorrTest_PSD[icrate][ich] = new TH2D(histname, histtitle, 7500, 0, max_fit, 600, 0, 0.45);
	vvhTCorrTest_PSD[icrate][ich]->GetXaxis()->SetTitle("Pulse Height (Fit) (adc counts)");
	vvhTCorrTest_PSD[icrate][ich]->GetYaxis()->SetTitle("PSD parameter");

	////////////////gamma spectrum to check timed response /////////////
	sprintf(histname, "hTCorrTest_EvTDiff_%s_%s", det.c_str(), "gamma");
	sprintf(histtitle, "Energy vs TSC TDiff for %s %s", det.c_str(), "gammas");
      
      vvhTCorrTest_EvTDiffG[icrate][ich] = new TH2D(histname, histtitle, (TIME_HIGH - TIME_LOW)/7, TIME_LOW, TIME_HIGH, 7500, 0, max_fit);
      vvhTCorrTest_EvTDiffG[icrate][ich]->GetXaxis()->SetTitle("TDiff (TDC) (ns)");
      vvhTCorrTest_EvTDiffG[icrate][ich]->GetYaxis()->SetTitle("Energy (fit) (MeV)");


      }
    }
  

  }
  cwd->cd();
  return SUCCESS;

}

INT MTCorrTest(EVENT_HEADER *pheader, void *pevent) {
  const map< string, vector<TPulseIsland*> >& wfd_map = gData->fPulseIslandToChannelMap;
  //const map<string, vector<int64_t> >& tdc_map = gData->fTDCHitsToChannelMap;

  /*
  std::string ref_bank = gSetup->GetBankName("TTSc");
  if(!tdc_map.count(ref_bank)){
      printf("MTCorrTest: No reference hits TTSc!\n");
      return SUCCESS;
  }
  const std::vector<int64_t>& ref_times = tdc_map.at(ref_bank);
  */  
  std::string ref_bank = gSetup->GetBankName("TSc");
  if(!wfd_map.count(ref_bank)){
      printf("MTCorrTest: No reference hits TSc!\n");
      return SUCCESS;
  }
  const std::vector<TPulseIsland*>& ref_hits = wfd_map.at(ref_bank);

  for(int icrate = 0; icrate < NCRATE; ++icrate){
    for(int ich = 0; ich < NCHANWFD[icrate]; ++ich) {
      if(WFDBANKS[icrate][ich] == " ") continue;
      if(!wfd_map.count(WFDBANKS[icrate][ich])) continue;
      std::string det=gSetup->GetDetectorName(WFDBANKS[icrate][ich]);

      const int polarity = gSetup->GetTriggerPolarity(WFDBANKS[icrate][ich]);
      const int nBits = gSetup->GetNBits(WFDBANKS[icrate][ich]);
      const int max_adc = std::pow(2, nBits) - 1;

      const std::vector<TPulseIsland*>& pulses = wfd_map.at(WFDBANKS[icrate][ich]);

      //two loops, pulse loop
      int t0 = 0;
      for(int p = 0; p < pulses.size(); ++p){
	//setup variables
	float pedestal = pulses[p]->GetPedestal(8);
	if(det == "TSc") pedestal = pulses[p]->GetPedestal(4);
	const std::vector<int>& samples = pulses[p]->GetSamples();
	const int nSamp = samples.size();
	if(nSamp < 8) continue;

	//float integral_ps = pulses[p]->GetIntegral();
	float max = pulses[p]->GetPulseHeight();
	float threshold = MTCorrTest_Threshold(det);
	double fit_max = pulses[p]->GetFitMax();

	double PSD_ratio = pulses[p]->GetPSDParameter();

	//pulse detail checks
	if(max < polarity*(threshold-pedestal)) continue;
	if(*(std::min_element( samples.begin(), samples.end() )) <= 0) continue;
	if(*(std::max_element( samples.begin(), samples.end() )) >= max_adc-1) continue;


	if(pulses[p]->GetVetoPulse()) continue; //vetoed
	if(pulses[p]->GetPileupPulse()) continue; //Pileup Protection
	//if(pulses[p]->GetDoublePulse()) continue; //Multiple hits near TSc

	
	//float energy_amp = MTCorrTest_Energy(det, max);
	//float energy_int = MTCorrTest_IntEnergy(det, integral_ps);
	float energy_fit = MTCorrTest_GetEnergyFit(det, fit_max);

	//if( !MTCorrTest_Neutron(det, PSD_ratio, energy_amp) ) continue; //gamma

	//loop over TTSc times
	if(pulses[p]->GetTDCTime() < 0) continue;
	for(int t = t0; t<ref_hits.size(); ++t){
	  if(ref_hits[t]->GetTDCTime() < 0) continue;
	  double dt = TICKTDC*(pulses[p]->GetTDCTime() - ref_hits[t]->GetTDCTime());
	  if(dt < TIME_LOW) break;
	  else if(dt < TIME_HIGH){
	    if(det != "NdetD" && det != "NdetU"){
	      //vvhTCorrTest_EvTDiff[icrate][ich]->Fill(dt, energy_amp);
	      vvhTCorrTest_FEvTDiff[icrate][ich]->Fill(dt, energy_fit);
	      //vvhTCorrTest_IEvTDiff[icrate][ich]->Fill(dt, energy_int);
	      //vvhTCorrTest_PHvTDiff[icrate][ich]->Fill(dt, max);
	    }
	    else if( MTCorrTest_Neutron(det, PSD_ratio, energy_fit) ){
	      //vvhTCorrTest_EvTDiff[icrate][ich]->Fill(dt, energy_amp);
	      vvhTCorrTest_FEvTDiff[icrate][ich]->Fill(dt, energy_fit);
	      //vvhTCorrTest_IEvTDiff[icrate][ich]->Fill(dt, energy_int);
	      vvhTCorrTest_PSDCut[icrate][ich]->Fill(energy_fit, PSD_ratio);
	      vvhTCorrTest_PSD[icrate][ich]->Fill(energy_fit, PSD_ratio);

	    }
	    else{
	      vvhTCorrTest_EvTDiffG[icrate][ich]->Fill(dt, energy_fit);
	      vvhTCorrTest_PSD[icrate][ich]->Fill(energy_fit, PSD_ratio);

	    }

	  }
	  else ++t0;
	}
      }
    }
  }



  return SUCCESS;
} 
  
/*
float MTCorrTest_Energy(std::string detname, float amp) {
  float energy = 0;
  if(detname == "NdetD"){ energy = (amp * 0.0003999) + 0.008234;  }
  else if(detname == "NdetU"){ energy = (amp * 0.0004015) + 0.009037;  }
  else if(detname == "GeCHEH"){ energy = (amp * 0.0001522) - 0.0004239;  }
  else if(detname == "GeCHEL"){ energy = (amp * 0.0003858) - 0.0017;    }
  else if(detname == "LaBr3"){ energy = (amp * 0.00163022) - 0.00836618;  } 
  else energy = amp;
  return energy;
}

float MTCorrTest_IntEnergy(std::string detname, float Int) {
  float energy = 0;
  if(detname == "NdetD"){ energy = (0.0000686 * Int) + 0.02117;  }
  else if(detname == "NdetU"){ energy = (0.00006766 * Int) + 0.02358;  }
  else if(detname == "GeCHEH"){ energy = 0.000002033 * Int;  }
  else if(detname == "GeCHEL"){ energy = 0.000005145 * Int;  }
  else if(detname == "LaBr3"){ energy = 0.0000823 * Int;  } 
  else energy = Int;
  return energy;
}
*/
float MTCorrTest_nSamples(std::string detname){
  float nSamples = 0;
  if(detname == "TSc") nSamples = 5;//12
  else if(detname == "NdetD" || detname == "NdetU") nSamples = 7.5;//150
  else if(detname == "GeCHEH" || detname == "GeCHEL") nSamples = 80;//300
  else if(detname == "LaBr3") nSamples = 24;//128
  else nSamples = 100;
  return nSamples;
}

float MTCorrTest_Threshold(std::string detname){
  float thresh = 0;
  if(detname == "GeCHEH") thresh = 1500;
  else if(detname == "GeCHEL") thresh = 1050;
  else if(detname == "NdetD") thresh = 15450;
  else if(detname == "NdetU") thresh = 15510;
  else if(detname == "LaBr3") thresh = 15645;
  else if(detname == "TSc") thresh = 15650;
  else thresh = 0;
  return thresh;
}

float MTCorrTest_GetEnergyFit(std::string detname, double fit_amp){
  float energy = 0;
  if(detname == "NdetD"){ energy = (fit_amp * 0.0003914) + 0.0289;  }
  if(detname == "NdetU"){ energy = (fit_amp * 0.0004381) + 0.0257;  }
  if(detname == "GeCHEH"){ energy = (fit_amp * 0.0001519) - 0.0002643;  }
  if(detname == "GeCHEL"){ energy = (fit_amp * 0.0003846) - 0.0003008;    }
  if(detname == "LaBr3"){ energy = (fit_amp * 0.00163022) - 0.00836618;  } 

  return energy;
}

bool MTCorrTest_Neutron(std::string det, double ratio, float energy){

  if(det == "NdetD"){  //need to update cuts for NdetD
    if(ratio < 0.13) return 0; //gamma
    else if(ratio < 0.155 && energy < 2.0) return 0; //gamma
    else if(ratio < 0.18 && energy < 0.7) return 0;// gamma
    else return 1; // neutron
  }
  if(det == "NdetU"){ 
    if(ratio < 0.14) return 0; //gamma
    else if(ratio < 0.17 && energy < 2.0) return 0; //gamma
    else if(ratio < 0.2 && energy < 0.7) return 0;// gamma
    else return 1; // neutron
  }
  return 0;
}
