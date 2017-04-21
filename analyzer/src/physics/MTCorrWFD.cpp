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
#include <TH1D.h>
#include <TH2D.h>
#include <TH3D.h>
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
static INT MTCorrWFD_init(void);
static INT MTCorrWFD(EVENT_HEADER*, void*);
INT MTCorrWFD_bor(int);
INT MTCorrWFD_eor(int);
float MTCorrWFD_Energy(std::string, float);
//float MTCorrWFD_IntEnergy(std::string, float);
float MTCorrWFD_nSamples(std::string);
float MTCorrWFD_Threshold(std::string);
bool MTCorrWFD_Neutron(std::string, double, float);


extern HNDLE hDB;
extern TGlobalData* gData;
extern TSetupData* gSetup;


using namespace AlCap;
namespace {
  std::string WFDBANKS[NCRATE][MAXNCHANWFD];
  std::string TDCBANKS[NCRATE][MAXNCHANWFD];
  //TH2D* vvhTCorrWFD_PHvTDiff[NCRATE][MAXNCHANWFD];
  TH2D* vvhTCorrWFD_EvTDiff[NCRATE][MAXNCHANWFD];
  TH2D* vvhTCorrWFD_FEvTDiff[NCRATE][MAXNCHANWFD];
  //TH2D* vvhTCorrWFD_IEvTDiff[NCRATE][MAXNCHANWFD];
  TH2D* vvhTCorrWFD_PSDCut[NCRATE][MAXNCHANWFD];
  TH2D* vvhTCorrWFD_PSD[NCRATE][MAXNCHANWFD];
  //TH2D* vvhTCorrWFD_EvTDiffG[NCRATE][MAXNCHANWFD];
  TH1D* vhTCorrWFD_Count_TSc;
  TH1D* vhTCorrWFD_TSc_Amp;
  const double TIME_LOW = -2e3, TIME_HIGH = 8e3;
  double TScCount;
}


ANA_MODULE MTCorrWFD_module =
{
  "MTCorrWFD",        /* module name           */
  "Damien Alexander",  /* author                */
  MTCorrWFD,          /* event routine         */
  MTCorrWFD_bor,      /* BOR routine           */
  MTCorrWFD_eor,      /* EOR routine           */
  MTCorrWFD_init,     /* init routine          */
  NULL,                /* exit routine          */
  NULL,                /* parameter structure   */
  0,                   /* structure size        */
  NULL,                /* initial parameters    */
};


INT MTCorrWFD_init() {
  TDirectory* cwd = gDirectory;
  gDirectory->mkdir("TCorrWFD")->cd();
  //select channels and corresponding vetos
  char histname[64];   char histtitle[64]; 


  sprintf(histname, "hTCorrWFD_TSc_Amp");
  sprintf(histtitle, "TSc Pulse Amplitudes");
  vhTCorrWFD_TSc_Amp = new TH1D(histname, histtitle, 15000, 0, 15000);
  vhTCorrWFD_TSc_Amp->GetXaxis()->SetTitle("TSc pulse Amplitudes");
  vhTCorrWFD_TSc_Amp->GetYaxis()->SetTitle("Count");

  sprintf(histname, "hTCorrWFD_Count_TSc");
  sprintf(histtitle, "Number of counts in TSc");
  vhTCorrWFD_Count_TSc = new TH1D(histname, histtitle, 1455, 5975, 7430);
  vhTCorrWFD_Count_TSc->GetXaxis()->SetTitle("RunNumber");
  vhTCorrWFD_Count_TSc->GetYaxis()->SetTitle("Number of valid pulses");

  for(int icrate = 0; icrate < NCRATE; ++icrate){
  
    for(int ich = 0; ich < NCHANWFD[icrate]; ++ich) {
      
      char bank[5]; sprintf(bank, "D%d%02d", icrate, ich);
      std::string det = gSetup->GetDetectorName(bank);
      
      //if(det == "NdetD" || det == "NdetU" /*|| det == "LaBr3"*/){
      //WFDBANKS[icrate][ich] = bank;
	//TDCBANKS[icrate][ich] = gSetup->GetBankName("T" + det);
      //}

      if(det == "GeCHEH" || det == "GeCHEL" /*|| det == "GeCHT"*/){
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
      float max_amp_e = MTCorrWFD_Energy(det, max_adc);
      //float max_int = max_adc * MTCorrWFD_nSamples(det); 
      //float max_int_e = MTCorrWFD_IntEnergy(det, max_int);

      float max_fit = 7.5;
      if(det == "GeCHEH") max_fit = 2.5;
      else if(det == "GeCHEL") max_fit = 6.0;
      else if(det == "LaBr3") max_fit = 15.0;
      else if(det == "GeCHT") max_fit = max_adc;

      
      //setup histograms

      ///////////// FE v TDiff //////////////////////
      if(det != "TSc"){
	sprintf(histname, "hTCorrWFD_FEvTDiff_%s", det.c_str());
	sprintf(histtitle, "Fit Energy vs TSC TDiff for %s", det.c_str());
	vvhTCorrWFD_FEvTDiff[icrate][ich] = new TH2D(histname, histtitle, (TIME_HIGH - TIME_LOW)/2, TIME_LOW, TIME_HIGH + 20, 7500, 0, max_fit);
	vvhTCorrWFD_FEvTDiff[icrate][ich]->GetXaxis()->SetTitle("TDiff (TDC) (ns)");
	vvhTCorrWFD_FEvTDiff[icrate][ich]->GetYaxis()->SetTitle("Energy (fit) (MeV)");
      }
      ///////////// IE v TDiff //////////////////////
      /*
      sprintf(histname, "hTCorrWFD_IEvTDiff_%s", det.c_str());
      sprintf(histtitle, "Integral Energy vs TSC TDiff for %s", det.c_str());
      vvhTCorrWFD_IEvTDiff[icrate][ich] = new TH2D(histname, histtitle, (TIME_HIGH - TIME_LOW)/14, TIME_LOW, TIME_HIGH, max_adc/2, 0, max_int_e);
      vvhTCorrWFD_IEvTDiff[icrate][ich]->GetXaxis()->SetTitle("TDiff (TDC) (ns)");
      vvhTCorrWFD_IEvTDiff[icrate][ich]->GetYaxis()->SetTitle("Energy (integral) (MeV)");
      */

      /*
      //PSD after time cuts/vetos
      if(det == "NdetD" || det == "NdetU"){
	
	sprintf(histname, "hTCorrWFD_PSDCut_%s", det.c_str());
	sprintf(histtitle, "Pulse Height vs PSD parameter for %s", det.c_str());
	vvhTCorrWFD_PSDCut[icrate][ich] = new TH2D(histname, histtitle, 640, 0, 8.0125, 600, 0, 0.45);
	vvhTCorrWFD_PSDCut[icrate][ich]->GetXaxis()->SetTitle("Energy (fit)");
	vvhTCorrWFD_PSDCut[icrate][ich]->GetYaxis()->SetTitle("PSD parameter");
	
	sprintf(histname, "hTCorrWFD_PSD_%s", det.c_str());
	sprintf(histtitle, "Pulse Height (Fit) vs PSD parameter for %s", det.c_str());
	vvhTCorrWFD_PSD[icrate][ich] = new TH2D(histname, histtitle, 640, 0, 8.0125, 600, 0, 0.4);
	vvhTCorrWFD_PSD[icrate][ich]->GetXaxis()->SetTitle("Pulse Height (Fit) (adc counts)");
	vvhTCorrWFD_PSD[icrate][ich]->GetYaxis()->SetTitle("PSD parameter");
      */
	////////////////gamma spectrum to check timed response /////////////
	/*
	sprintf(histname, "hTCorrWFD_EvTDiff_%s_%s", det.c_str(), "gamma");
	sprintf(histtitle, "Energy vs TSC TDiff for %s %s", det.c_str(), "gammas");
      
      vvhTCorrWFD_EvTDiffG[icrate][ich] = new TH2D(histname, histtitle, (TIME_HIGH - TIME_LOW)/7, TIME_LOW, TIME_HIGH, 7500, 0, max_fit);
      vvhTCorrWFD_EvTDiffG[icrate][ich]->GetXaxis()->SetTitle("TDiff (TDC) (ns)");
      vvhTCorrWFD_EvTDiffG[icrate][ich]->GetYaxis()->SetTitle("Energy (fit) (MeV)");
	*/

      //}
    }
  

  }
  cwd->cd();
  return SUCCESS;

}

INT MTCorrWFD_eor(int run_number){
  int bin = vhTCorrWFD_Count_TSc->FindBin(run_number);
  vhTCorrWFD_Count_TSc->SetBinContent(bin, TScCount);
  return 0;
}

INT MTCorrWFD_bor(int run_number){
  TScCount = 0;
  return 0;
}

INT MTCorrWFD(EVENT_HEADER *pheader, void *pevent) {
  const map< string, vector<TPulseIsland*> >& wfd_map = gData->fPulseIslandToChannelMap;
  //const map<string, vector<int64_t> >& tdc_map = gData->fTDCHitsToChannelMap;

  /*
  std::string ref_bank = gSetup->GetBankName("TTSc");
  if(!tdc_map.count(ref_bank)){
      printf("MTCorrWFD: No reference hits TTSc!\n");
      return SUCCESS;
  }
  const std::vector<int64_t>& ref_times = tdc_map.at(ref_bank);
  */  
  std::string ref_bank = gSetup->GetBankName("TSc");
  if(!wfd_map.count(ref_bank)){
      printf("MTCorrWFD: No reference hits TSc!\n");
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

	if(pulses[p]->GetVetoPulse()) continue; //vetoed
	if(pulses[p]->GetPileupPulse()) continue; //Pileup Protection
	//if(pulses[p]->GetTDCTime() < 0 ) continue; //no correlated TDC pulse
	if(det == "TSc" && pulses[p]->GetTDCTime() < 0) continue;

	//setup variables
	float pedestal = pulses[p]->GetPedestal(8);
	if(det == "TSc") pedestal = pulses[p]->GetPedestal(4);
	const std::vector<int>& samples = pulses[p]->GetSamples();
	const int nSamp = samples.size();
	if(nSamp < 8) continue;

	if(*(std::min_element( samples.begin(), samples.end() )) <= 0 && polarity == -1) continue;
	if(*(std::max_element( samples.begin(), samples.end() )) >= max_adc-1 && polarity == 1) continue;

	//float integral_ps = pulses[p]->GetIntegral();
	float max = pulses[p]->GetPulseHeight();
	float threshold = MTCorrWFD_Threshold(det);
	float fit_max = 0;
	if(det == "TSc" || det == "GeCHT") fit_max = max;	
	else fit_max = pulses[p]->GetFitMax();
	
	double PSD_ratio = pulses[p]->GetPSDParameter();

	//pulse detail checks
	if(max < polarity*(threshold-pedestal) ) continue;
	if(pedestal > threshold && polarity == 1 ) continue;
	else if(pedestal < threshold && polarity == -1) continue;

	//if(pulses[p]->GetDoublePulse()) continue; //Multiple hits near TSc

	
	//double energy_amp = pulses[p]->GetEnergyAmp(max);
	//double energy_int = pulses[p]->IntEnergy(integral_ps);
	double energy_fit = pulses[p]->GetEnergyFit(fit_max);

	//if( !MTCorrWFD_Neutron(det, PSD_ratio, energy_amp) ) continue; //gamma

	if(det == "TSc"){
	  vhTCorrWFD_TSc_Amp->Fill(max);
	  TScCount++;
	  continue;
	}

	double prior = -15000;
	bool hist = false;
	double tCorr = -1530;

	//loop over TSc times
	for(int t = t0; t<ref_hits.size(); ++t){
	  //if(ref_hits[t]->GetTDCTime() < 0) continue;
	  if(ref_hits[t]->GetPileupPulse() ) continue;
	  if(ref_hits[t]->GetVetoPulse() ) continue;
	  double dt = pulses[p]->GetWFDTime() - ref_hits[t]->GetWFDTime() + tCorr;

	  if(dt > 3*TIME_HIGH  && t >= 0) t0 = t-1;
	  if(dt < TIME_LOW ) break;
	  /*
	  else if(dt < TIME_HIGH){
	    if(det != "NdetD" && det != "NdetU"){
	      vvhTCorrWFD_FEvTDiff[icrate][ich]->Fill(dt, energy_fit);
	      //vvhTCorrWFD_IEvTDiff[icrate][ich]->Fill(dt, energy_int);
	    }
	    else if( MTCorrWFD_Neutron(det, PSD_ratio, energy_fit) ){
	      vvhTCorrWFD_FEvTDiff[icrate][ich]->Fill(dt, energy_fit);
	      //vvhTCorrWFD_IEvTDiff[icrate][ich]->Fill(dt, energy_int);
	      vvhTCorrWFD_PSDCut[icrate][ich]->Fill(energy_fit, PSD_ratio);
	      vvhTCorrWFD_PSD[icrate][ich]->Fill(energy_fit, PSD_ratio);

	    }
	    else{
	      //vvhTCorrWFD_EvTDiffG[icrate][ich]->Fill(dt, energy_fit);
	      vvhTCorrWFD_PSD[icrate][ich]->Fill(energy_fit, PSD_ratio);

	    }

	  }
	  */

	  else if(dt < TIME_HIGH && abs(dt) < abs(prior)){
	      prior = dt;
	      hist = true;
	  }


	}

	if(hist && prior > TIME_LOW) 
	  vvhTCorrWFD_FEvTDiff[icrate][ich]->Fill(prior, energy_fit);
	else if(hist) std::cout << "MTCorrTest : plotting incorrect time, check algorithm" << std::endl;
      
      }
    }
  }



  return SUCCESS;
} 
  

float MTCorrWFD_Energy(std::string detname, float amp) {
  float energy = 0;
  if(detname == "NdetD"){ energy = (amp * 0.0003999) + 0.008234;  }
  else if(detname == "NdetU"){ energy = (amp * 0.0004015) + 0.009037;  }
  else if(detname == "GeCHEH"){ energy = (amp * 0.0001522) - 0.0004239;  }
  else if(detname == "GeCHEL"){ energy = (amp * 0.0003858) - 0.0017;    }
  else if(detname == "LaBr3"){ energy = (amp * 0.00163022) - 0.00836618;  } 
  else energy = amp;
  return energy;
}
/*
float MTCorrWFD_IntEnergy(std::string detname, float Int) {
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
float MTCorrWFD_nSamples(std::string detname){
  float nSamples = 0;
  if(detname == "TSc") nSamples = 5;//12
  else if(detname == "NdetD" || detname == "NdetU") nSamples = 7.5;//150
  else if(detname == "GeCHEH" || detname == "GeCHEL") nSamples = 80;//300
  else if(detname == "LaBr3") nSamples = 24;//128
  else nSamples = 100;
  return nSamples;
}

float MTCorrWFD_Threshold(std::string detname){
  float thresh = 0;
  if(detname == "GeCHEH") thresh = 1500;
  else if(detname == "GeCHEL") thresh = 1050;
  else if(detname == "GeCHT") thresh = 15150;
  else if(detname == "NdetD") thresh = 15460;
  else if(detname == "NdetU") thresh = 15510;
  else if(detname == "LaBr3") thresh = 15645;
  else if(detname == "TSc") thresh = 14800;
  else thresh = 0;
  return thresh;
}

bool MTCorrWFD_Neutron(std::string det, double ratio, float energy){

  if(det == "NdetD"){  //need to update cuts for NdetD
    if(ratio < 0.13) return 0; //gamma
    else if(ratio < 0.155 && energy < 2.0) return 0; //gamma
    else if(ratio < 0.18 && energy < 0.7) return 0;// gamma
    else return 1; // neutron
  }
  if(det == "NdetU"){ 
    if(ratio < 0.14) return 0; //gamma
    else if(ratio < 0.17 && energy < 2.0) return 0; //gamma
    else if(ratio < 0.20 && energy < 0.7) return 0;// gamma
    else return 1; // neutron
  }
  return 0;
}
