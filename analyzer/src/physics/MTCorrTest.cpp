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
static INT MTCorrTest_init(void);
static INT MTCorrTest(EVENT_HEADER*, void*);
INT MTCorrTest_bor(int);
INT MTCorrTest_eor(int);
float MTCorrTest_Energy(std::string, float);
//float MTCorrTest_IntEnergy(std::string, float);
//float MTCorrTest_GetEnergyFit(std::string, double);
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
  TH2D* vvhTCorrTest_EvTDiff[NCRATE][MAXNCHANWFD];
  TH2D* vvhTCorrTest_FEvTDiff[NCRATE][MAXNCHANWFD];
  TH1D* vhTCorrTest_TDiff[NCRATE][MAXNCHANWFD];
  //TH2D* vvhTCorrTest_IEvTDiff[NCRATE][MAXNCHANWFD];
  TH2D* vvhTCorrTest_PSDCut[NCRATE][MAXNCHANWFD];
  TH2D* vvhTCorrTest_PSD[NCRATE][MAXNCHANWFD];
  //TH2D* vvhTCorrTest_EvTDiffG[NCRATE][MAXNCHANWFD];
  TH1D* vhTCorrTest_Count_TSc;
  TH1D* vhTCorrTest_TSc_Amp;
  const double TIME_LOW = -1e4, TIME_HIGH = 1e4;
  double TScCount;
}


ANA_MODULE MTCorrTest_module =
{
  "MTCorrTest",        /* module name           */
  "Damien Alexander",  /* author                */
  MTCorrTest,          /* event routine         */
  MTCorrTest_bor,      /* BOR routine           */
  MTCorrTest_eor,      /* EOR routine           */
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
  char histname[64];   char histtitle[64]; 


  sprintf(histname, "hTCorrTest_TSc_Amp");
  sprintf(histtitle, "TSc Pulse Amplitudes");
  vhTCorrTest_TSc_Amp = new TH1D(histname, histtitle, 15000, 0, 15000);
  vhTCorrTest_TSc_Amp->GetXaxis()->SetTitle("TSc pulse Amplitudes");
  vhTCorrTest_TSc_Amp->GetYaxis()->SetTitle("Count");

  sprintf(histname, "hTCorrTest_Count_TSc");
  sprintf(histtitle, "Number of counts in TSc");
  vhTCorrTest_Count_TSc = new TH1D(histname, histtitle, 1455, 5975, 7430);
  vhTCorrTest_Count_TSc->GetXaxis()->SetTitle("RunNumber");
  vhTCorrTest_Count_TSc->GetYaxis()->SetTitle("Number of valid pulses");

  for(int icrate = 0; icrate < NCRATE; ++icrate){
  
    for(int ich = 0; ich < NCHANWFD[icrate]; ++ich) {
      
      char bank[5]; sprintf(bank, "D%d%02d", icrate, ich);
      std::string det = gSetup->GetDetectorName(bank);

      if(det == "NdetD" || det == "NdetU" /*|| det == "LaBr3"*/){
	WFDBANKS[icrate][ich] = bank;
	//TDCBANKS[icrate][ich] = gSetup->GetBankName("T" + det);
      }
      
      else if(det == "GeCHEH" || det == "GeCHEL" || det == "GeCHT"){
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
      float max_amp_e = MTCorrTest_Energy(det, max_adc);
      //float max_int = max_adc * MTCorrTest_nSamples(det); 
      //float max_int_e = MTCorrTest_IntEnergy(det, max_int);

      float max_fit = 7.5;
      if(det == "GeCHEH") max_fit = 2.5;
      else if(det == "GeCHEL") max_fit = 6.0;
      else if(det == "LaBr3") max_fit = 15.0;
      else if(det == "GeCHT") max_fit = max_adc;


      
      //setup histograms

      ///////////// E vs TDiff  //////////////////////
      /*
      if(det != "TSc" && det != "NdetD" && det != "NdetU"){
	
	sprintf(histname, "hTCorrTest_EvTDiff_%s", det.c_str());
	sprintf(histtitle, "Energy vs TSC TDiff for %s", det.c_str());
	vvhTCorrTest_EvTDiff[icrate][ich] = new TH2D(histname, histtitle, (TIME_HIGH - TIME_LOW)/14, TIME_LOW, TIME_HIGH, max_adc/4, 0, max_amp_e);
	vvhTCorrTest_EvTDiff[icrate][ich]->GetXaxis()->SetTitle("TDiff (TDC) (ns)");
	vvhTCorrTest_EvTDiff[icrate][ich]->GetYaxis()->SetTitle("Energy (amplitude) (MeV)");
      }
      */
      ///////////// FE v TDiff //////////////////////
      if(det != "TSc"){
	sprintf(histname, "hTCorrTest_FEvTDiff_%s", det.c_str());
	sprintf(histtitle, "Fit Energy vs TSC TDiff for %s", det.c_str());
	vvhTCorrTest_FEvTDiff[icrate][ich] = new TH2D(histname, histtitle, 1e4, -2e3, 8e3, 800, 0, 10.0125);
	vvhTCorrTest_FEvTDiff[icrate][ich]->GetXaxis()->SetTitle("TDiff (TDC) (ns)");
	vvhTCorrTest_FEvTDiff[icrate][ich]->GetYaxis()->SetTitle("Energy (fit) (MeV)");


	/////////// TDiff ///////////////////////////////////////
	sprintf(histname, "hTCorrTest_TDiff_%s", det.c_str());
	sprintf(histtitle, "TSC TDiff for %s", det.c_str());
	vhTCorrTest_TDiff[icrate][ich] = new TH1D(histname, histtitle, 1e4, -1e4, 1e4);
	vhTCorrTest_TDiff[icrate][ich]->GetXaxis()->SetTitle("TDiff (TDC) (ns)");
	vhTCorrTest_TDiff[icrate][ich]->GetYaxis()->SetTitle("Count");

      }
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
	sprintf(histtitle, "Pulse Height vs PSD parameter for %s, Time Cut", det.c_str());
	vvhTCorrTest_PSDCut[icrate][ich] = new TH2D(histname, histtitle, 640, 0, 8.0125, 600, 0, 0.45);
	vvhTCorrTest_PSDCut[icrate][ich]->GetXaxis()->SetTitle("Energy (fit)");
	vvhTCorrTest_PSDCut[icrate][ich]->GetYaxis()->SetTitle("PSD parameter");
	
	sprintf(histname, "hTCorrTest_PSD_%s", det.c_str());
	sprintf(histtitle, "Pulse Height (Fit) vs PSD parameter for %s BG", det.c_str());
	vvhTCorrTest_PSD[icrate][ich] = new TH2D(histname, histtitle, 640, 0, 8.0125, 600, 0, 0.4);
	vvhTCorrTest_PSD[icrate][ich]->GetXaxis()->SetTitle("Pulse Height (Fit) (adc counts)");
	vvhTCorrTest_PSD[icrate][ich]->GetYaxis()->SetTitle("PSD parameter");

	////////////////gamma spectrum to check timed response /////////////
	/*
	sprintf(histname, "hTCorrTest_EvTDiff_%s_%s", det.c_str(), "gamma");
	sprintf(histtitle, "Energy vs TSC TDiff for %s %s", det.c_str(), "gammas");
      
      vvhTCorrTest_EvTDiffG[icrate][ich] = new TH2D(histname, histtitle, (TIME_HIGH - TIME_LOW)/7, TIME_LOW, TIME_HIGH, 7500, 0, max_fit);
      vvhTCorrTest_EvTDiffG[icrate][ich]->GetXaxis()->SetTitle("TDiff (TDC) (ns)");
      vvhTCorrTest_EvTDiffG[icrate][ich]->GetYaxis()->SetTitle("Energy (fit) (MeV)");
	*/

      }
    }
  

  }
  cwd->cd();
  return SUCCESS;

}

INT MTCorrTest_eor(int run_number){
  int bin = vhTCorrTest_Count_TSc->FindBin(run_number);
  vhTCorrTest_Count_TSc->SetBinContent(bin, TScCount);
  return 0;
}

INT MTCorrTest_bor(int run_number){
  TScCount = 0;
  return 0;
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
	double tcorr = 0;
	if(det == "NdetD") tcorr = -57;
	if(det == "NdetU") tcorr = -39;
	if(det == "GeCHEH" || det == "GeCHEL") tcorr = -120;

	if(pulses[p]->GetVetoPulse()) continue; //vetoed
	if(pulses[p]->GetPileupPulse()) continue; //Pileup Protection
	if(pulses[p]->GetTDCTime() < 0 ) continue; //no correlated TDC pulse

	float pedestal = pulses[p]->GetPedestal(8);
	if(det == "TSc") pedestal = pulses[p]->GetPedestal(4);
	const std::vector<int>& samples = pulses[p]->GetSamples();
	const int nSamp = samples.size();
	if(nSamp < 8) continue;

	if(*(std::min_element( samples.begin(), samples.end() )) <= 0 && polarity == -1) continue;
	if(*(std::max_element( samples.begin(), samples.end() )) >= max_adc-1 && polarity == 1) continue;

	//float integral_ps = pulses[p]->GetIntegral();
	float threshold = MTCorrTest_Threshold(det);
	float fit_max = 0;
	if(det == "TSc" || det == "GeCHT") fit_max = pulses[p]->GetPulseHeight();	
	else fit_max = pulses[p]->GetFitMax();
	
	double PSD_ratio = pulses[p]->GetPSDParameter();

	//pulse detail checks
	if(fit_max < polarity*(threshold-pedestal)) continue;
	if(pedestal > threshold && polarity == 1 ) continue;
	else if(pedestal < threshold && polarity == -1) continue;


	//if(pulses[p]->GetDoublePulse()) continue; //Multiple hits near TSc

	
	//double energy_amp = pulses[p]->GetEnergyAmp(max);
	//double energy_int = pulses[p]->IntEnergy(integral_ps);
	double energy_fit = pulses[p]->GetEnergyFit(fit_max);

	//if( !MTCorrTest_Neutron(det, PSD_ratio, energy_amp) ) continue; //gamma

	if(det == "TSc"){
	  vhTCorrTest_TSc_Amp->Fill(fit_max);
	  TScCount++;
	  continue;
	}
	double prior = -15000;
	bool hist = false;
	//bool hist_cut = false;

	//loop over TTSc times
	for(int t = t0; t<ref_hits.size(); ++t){
	  if(ref_hits[t]->GetTDCTime() < 0) continue;
	  if(ref_hits[t]->GetPileupPulse() ) continue;
	  if(ref_hits[t]->GetVetoPulse() ) continue;
	  double dt = TICKTDC*(pulses[p]->GetTDCTime() - ref_hits[t]->GetTDCTime() ) + tcorr;

	  if(dt > 3*TIME_HIGH && t >= 1) t0 = t-1;
	  if(dt < TIME_LOW) break;
	  else if(dt < TIME_HIGH && abs(dt) < abs(prior)){
	      prior = dt;
	      hist = true;
	      //vvhTCorrTest_EvTDiff[icrate][ich]->Fill(dt, energy_amp);
	      //vvhTCorrTest_FEvTDiff[icrate][ich]->Fill(dt, energy_fit);
	      //vvhTCorrTest_IEvTDiff[icrate][ich]->Fill(dt, energy_int);
	      //vvhTCorrTest_PHvTDiff[icrate][ich]->Fill(dt, max);
	      /*
	    else if( MTCorrTest_Neutron(det, PSD_ratio, energy_fit) ){
	      //vvhTCorrTest_EvTDiff[icrate][ich]->Fill(dt, energy_amp);
	      vvhTCorrTest_FEvTDiff[icrate][ich]->Fill(dt, energy_fit);
	      //vvhTCorrTest_IEvTDiff[icrate][ich]->Fill(dt, energy_int);
	      vvhTCorrTest_PSDCut[icrate][ich]->Fill(energy_fit, PSD_ratio);
	      vvhTCorrTest_PSD[icrate][ich]->Fill(energy_fit, PSD_ratio);

	    }
	    else{
	      //vvhTCorrTest_EvTDiffG[icrate][ich]->Fill(dt, energy_fit);
	      vvhTCorrTest_PSD[icrate][ich]->Fill(energy_fit, PSD_ratio);
	      */
	      //if((det == "NdetD" || det == "NdetU") && MTCorrTest_Neutron(det, PSD_ratio, energy_fit) ) hist_cut = true;
	  }

	  
	}

	//float cutLow = 100., cutHigh = 1100.;  //Pb
	float cutLow = 200., cutHigh = 4200.;  //Al
	//float cutLow = 200., cutHigh = 2200.;  //Pb
	//float cutLow = 200., cutHigh = 6200.;  //Pb


	if(hist && prior > TIME_LOW){
	  vvhTCorrTest_FEvTDiff[icrate][ich]->Fill(prior, energy_fit);
	  vhTCorrTest_TDiff[icrate][ich]->Fill(prior);
	  if(det == "NdetD" || det == "NdetU"){
	    if(prior > (-1 * cutHigh) && prior < (-1*cutLow) )vvhTCorrTest_PSD[icrate][ich]->Fill(energy_fit, PSD_ratio);
	    if(prior > cutLow && prior < cutHigh) vvhTCorrTest_PSDCut[icrate][ich]->Fill(energy_fit, PSD_ratio);
	  }
	}
	else if(hist) std::cout << "MTCorrTest : plotting incorrect time, check algorithm" << std::endl;
      

      }
    }
  }



  return SUCCESS;
} 
  

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
  else if(detname == "GeCHT") thresh = 15150;
  else if(detname == "NdetD") thresh = 15460;
  else if(detname == "NdetU") thresh = 15510;
  else if(detname == "LaBr3") thresh = 15645;
  else if(detname == "TSc") thresh = 14800;
  else thresh = 0;
  return thresh;
}

bool MTCorrTest_Neutron(std::string det, double ratio, float energy){

  if(det == "NdetD"){  //need to update cuts for NdetD
    if(ratio >= 0.1153 + exp(-3.749 -0.649*energy) + exp(-3.021-0.3311*energy) )
      return 1; // neutron
    else return 0; //gamma (> 2*Sigma from neutron mean)

  }
  if(det == "NdetU"){ 
    if(ratio >= 0.1085 + exp(-2.89 -0.6968*energy) + exp(-2.915-0.0762*energy) )
      return 1; // neutron
    else return 0; //gamma (> 2*Sigma from neutron mean)
  }
  return 0;
}
