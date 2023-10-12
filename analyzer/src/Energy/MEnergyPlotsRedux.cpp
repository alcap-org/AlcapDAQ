////////////////////////////////////////////////////////////////////////////////
/// \defgroup MEnergyPlotsRedux
/// \ingroup energy
/// \author Cole Kampa, Damien Alexander
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
#include "TNtuple.h"

/* AlCap includes */
#include "TGlobalData.h"
#include "TSetupData.h"
#include "TPulseIsland.h"

using namespace std;

/*-- Module declaration --------------------------------------------*/
INT  MEnergyPlotsRedux_init(void);
INT  MEnergyPlotsRedux(EVENT_HEADER*, void*);
INT  MEnergyPlotsRedux_eor(INT);
INT  MEnergyPlotsRedux_BookHistograms();
// flip polarity
std::vector<int> MEnergyPlotsRedux_FlipSamplePolarity(const std::vector<int>&, const int, int);
// define pedestal parameters
void MEnergyPlotsRedux_SetPedestalParams(const std::vector<int>&, int, int, std::string, float&, float&, float&, float&, float&);
// integration (analytic)
float MEnergyPlotsRedux_GetPedestalIntegral(int, int, float&, float&, float&);
// general integration (Simpson's)
float MEnergyPlotsRedux_GetIntegral(const std::vector<int>&, int, int);

extern HNDLE hDB;
extern TGlobalData* gData;
extern TSetupData* gSetup;

std::map<std::string, TNtuple*> Ntuple_map;
std::map<std::string, TH1F*> IntegralEnergy_map, AmplitudeEnergy_map, FitEnergy_map, Integral_map, Amplitude_map, Fit_map;
std::map<std::string, TH2F*> IntegralVAmplitude_map;

bool EnergyPlotsRedux_firstEvent = true;

ANA_MODULE MEnergyPlotsRedux_module =
{
	"MEnergyPlotsRedux",                  /* module name           */
	"Cole Kampa, Damien Alexander",            /* author                */
	MEnergyPlotsRedux,                    /* event routine         */
	NULL,                          /* BOR routine           */
	MEnergyPlotsRedux_eor,                /* EOR routine           */
	MEnergyPlotsRedux_init,               /* init routine          */
	NULL,                          /* exit routine          */
	NULL,                          /* parameter structure   */
	0,                             /* structure size        */
	NULL,                          /* initial parameters    */
};

/////////////////////////////////////////////////////////////////

INT MEnergyPlotsRedux_init(){
  return SUCCESS;
}

INT MEnergyPlotsRedux_BookHistograms() 
{
  TDirectory* cwd = gDirectory;
  gDirectory->mkdir("EnergyPlotsRedux")->cd();

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

    double max_bin_PS = max_adc * n_samples;
    if(detname == "NdetD" || detname == "NdetU") max_bin_PS /= 20;
    else if(detname == "LaBr3") max_bin_PS /= 6;
    else if(detname == "GeCHEH" || detname == "GeCHEL") max_bin_PS /= 4;
    else continue;
    double max_amp_e = (*firstPulse)->GetEnergyAmp(max_adc);
    double max_int_e = (*firstPulse)->GetEnergyInt(max_bin_PS);

    double max_fit = 7.5;
    if(detname == "GeCHEH") max_fit = 2.5;
    if(detname == "GeCHEL") max_fit = 6.0;
    if(detname == "TSc") max_fit = 15000.0;
    if(detname == "LaBr3") max_fit = 15.0;


    
    // ntuple
    // stores: Integral, Amplitude
    std::string ntupname = "ntup_" + detname + "_" + bankname;
    std::string ntuptitle = "ntuple for " + detname;
    //TNtuple* ntuple = new TNtuple(ntupname.c_str(), ntuptitle.c_str(), "Integral:Amplitude:SIntegral:LIntegral:IntegralRatio");
    //TNtuple* ntuple = new TNtuple(ntupname.c_str(), ntuptitle.c_str(), "Integral:Amplitude:iMax:nSamp:SIntegral:LIntegral:SIntegral_pedsub:LIntegral_pedsub:IntegralRatio:status_IntegralRatio");
    // debugging short and long integrals
    //TNtuple* ntuple = new TNtuple(ntupname.c_str(), ntuptitle.c_str(), "Integral:Amplitude:iMax:nSamp:SIntegral:LIntegral:SIntegral_pedsub:LIntegral_pedsub:IntegralRatio:status_IntegralRatio:Polarity:pedBegin:pedEnd:pedX0:pedSlope:pedInt");
    TNtuple* ntuple = new TNtuple(ntupname.c_str(), ntuptitle.c_str(), "Integral:Amplitude:iMax:nSamp:SIntegral:LIntegral:SIntegral_pedsub:LIntegral_pedsub:IntegralRatio:status_IntegralRatio:Polarity:pedBegin:pedEnd:pedX0:pedSlope:pedInt:pedIntS:pedIntL");
    Ntuple_map[bankname] = ntuple;

    //hIntegral_[detname]_[bankname]
    std::string histname = "hIntegral_" + detname + "_" + bankname;
    std::string histtitle = "Integral of Pulses in " + detname;
    TH1F* hIntHist = new TH1F(histname.c_str(), histtitle.c_str(), max_adc/4, 0, max_bin_PS);
    hIntHist->GetXaxis()->SetTitle("Integral (ADC value) ");
    hIntHist->GetYaxis()->SetTitle("Counts");
    Integral_map[bankname] = hIntHist;
    
    
    //hAmplitude_[detname]_[bankname]
    histname = "hAmplitude_" + detname + "_" + bankname;
    histtitle = "Amplitude of Pulses in " + detname;
    TH1F* hAmpHist = new TH1F(histname.c_str(), histtitle.c_str(), max_adc, 0, max_adc);
    hAmpHist->GetXaxis()->SetTitle("Amplitude (max bin)");
    hAmpHist->GetYaxis()->SetTitle("Count");
    Amplitude_map[bankname] = hAmpHist;
    
    
    //hFit_[detname]_[bankname]
    histname = "hFit_" + detname + "_" + bankname;
    histtitle = "Fit Amplitude of Pulses in " + detname;
    TH1F* hFitHist = new TH1F(histname.c_str(), histtitle.c_str(), max_adc+1500, 0, max_adc+1500);
    hFitHist->GetXaxis()->SetTitle("Amplitude (max of fit)");
    hFitHist->GetYaxis()->SetTitle("Count");
    Fit_map[bankname] = hFitHist;

    //hIntegralE_[detname]_[bankname]
    histname = "hIntegralE_" + detname + "_" + bankname;
    histtitle = "Energy of Pulses in " + detname + " (Integral)";
    TH1F* hIntEHist = new TH1F(histname.c_str(), histtitle.c_str(), max_adc/4, 0, max_int_e);
    hIntEHist->GetXaxis()->SetTitle("Energy (MeV(ee)) ");
    hIntEHist->GetYaxis()->SetTitle("Counts");
    IntegralEnergy_map[bankname] = hIntEHist;
    
    
    //hAmplitudeE_[detname]_[bankname]
    histname = "hAmplitudeE_" + detname + "_" + bankname;
    histtitle = "Energy of Pulses in " + detname + " (Amplitude)";
    TH1F* hAmpEHist = new TH1F(histname.c_str(), histtitle.c_str(), max_adc, 0, max_amp_e);
    hAmpEHist->GetXaxis()->SetTitle("Energy (MeV(ee))");
    hAmpEHist->GetYaxis()->SetTitle("Count");
    AmplitudeEnergy_map[bankname] = hAmpEHist;
    
    
    //hFitE_[detname]_[bankname]
    histname = "hFitE_" + detname + "_" + bankname;
    histtitle = "Energy of Pulses in " + detname + " (Fit Amplitude)";
    TH1F* hFitEHist = new TH1F(histname.c_str(), histtitle.c_str(), 18000, 0, max_fit);
    hFitEHist->GetXaxis()->SetTitle("Energy (MeV(ee))");
    hFitEHist->GetYaxis()->SetTitle("Count");
    FitEnergy_map[bankname] = hFitEHist;
    
    
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

INT MEnergyPlotsRedux_eor(INT run_number)
{
  return SUCCESS;  // currently nothing to do here
}

INT MEnergyPlotsRedux(EVENT_HEADER *pheader, void *pevent)
{
  if(EnergyPlotsRedux_firstEvent == true){
    MEnergyPlotsRedux_BookHistograms();
    EnergyPlotsRedux_firstEvent = false;
  }

  const std::map<std::string, std::vector<TPulseIsland*> >& tpi_map = gData->fPulseIslandToChannelMap;

  for(std::map<std::string, std::vector<TPulseIsland*> >::const_iterator mIter = tpi_map.begin(); mIter != tpi_map.end(); mIter++){
    const std::string bankname = mIter->first;
    const std::string detname = gSetup->GetDetectorName(bankname);
    const std::vector<TPulseIsland*>& pulses = mIter->second;
    int polarity = gSetup->GetTriggerPolarity(bankname);
    int nbits = gSetup->GetNBits(bankname);
    const int max_adc = std::pow(2, gSetup->GetNBits(bankname));

    /*  
    if(detname != "TSc"
       && detname != "GeCHEH"
       && detname != "GeCHEL"
       && detname != "NdetD"
       && detname != "NdetU"
       && detname != "LaBr3")
      continue;
    */
    if(detname != "NdetD"
       && detname != "NdetU")
      continue;

   
    //int threshold = 60;  //threshold (from baseline) for integration
    //float pct = 0.2;

    for(std::vector<TPulseIsland*>::const_iterator pIter = pulses.begin(); pIter != pulses.end(); pIter++){
      if(IntegralEnergy_map.find(bankname) == IntegralEnergy_map.end()) continue;

      float integral = 0, sintegral = 0, lintegral = 0;
      float integral_ps = 0, sintegral_ps = 0, lintegral_ps = 0;
      float integral_ratio =0;
      //bool overflow = false;
      float max = 0;
      int tMax = 0;
      //bool test = false;
      // get the samples
      const std::vector<int>& samples_init = (*pIter)->GetSamples();
      // adjust samples if polarity < 0
      const std::vector<int>& samples = MEnergyPlotsRedux_FlipSamplePolarity(samples_init, max_adc, polarity);
      //if (polarity < 0){
      //  const std::vector<int>& samples = MEnergyPlotsRedux_FlipSamplePolarity(samples_init, max_adc);
      //}
      //else{
      //  const std::vector<int>& samples = samples_init;
      //}
      const int nSamp = samples.size();
      // PEDESTAL      
      float pedBegin=0, pedEnd=0, pedSlope=0, pedX0=0, pedY0=0;
      int nstart=9, nend=9;
      // set the parameters
      MEnergyPlotsRedux_SetPedestalParams(samples, nstart, nend, detname, pedBegin, pedEnd, pedSlope, pedX0, pedY0);
      // now calculate the integral
      float pedInt=0, pedIntS=0, pedIntL=0;
      pedInt = MEnergyPlotsRedux_GetPedestalIntegral(0, nSamp-1, pedSlope, pedX0, pedY0);
      // Find max & index of the max
      // max after pedestal subtraction!!
      int i=0;
      for (std::vector<int>::const_iterator pulse=samples.begin(); pulse != samples.end(); pulse++){
        float val = (*pulse);
        float ped = pedBegin + pedSlope*(i - pedX0);
        val -= ped;
        // FIXME! is this correct to do for the polarity?
        //if (polarity < 0) val *= -1;
        if (val > max){
          max = val;
          tMax = i;
        }
        i++;
      }

      //do each of these three integrations separately,
       
      if(samples.size() < 8) continue;
      
      // full integral
      int start_ind=0, end_ind=nSamp;
      integral = MEnergyPlotsRedux_GetIntegral(samples, start_ind, end_ind);
      //integral_ps = polarity * (integral - pedInt);
      integral_ps = integral - pedInt;
      // short/long integral index definitions
      // variables indicate index distance from peak value (<0 to the left, >0 to the right)
      int dind_min_l = -9, dind_max_l = 81;
      int dind_min_s = 9, dind_max_s = 81;
      int ind_min_l = tMax + dind_min_l;
      int ind_max_l = tMax + dind_max_l;
      int ind_min_s = tMax + dind_min_s;
      int ind_max_s = tMax + dind_max_s;
      // check that we are not above the max and min integers
      // flag for any cases out of the range
      // =0 normal
      // =-1000 out of range
      int integral_ratio_status = 0;
      if (ind_min_l < 0) {
        ind_min_l = 0;
        integral_ratio_status = -1000;
      }
      if (ind_min_s < 0) {
        ind_min_s = 0;
        integral_ratio_status = -1000;
      }
      if (ind_min_l > nSamp -1) {
        ind_min_l = nSamp - 1;
        integral_ratio_status = -1000;
      }
      if (ind_min_s > nSamp - 1) {
        ind_min_s = nSamp - 1;
        integral_ratio_status = -1000;
      }
      if (ind_max_l < 0) {
        ind_max_l = 0;
        integral_ratio_status = -1000;
      }
      if (ind_max_s < 0) {
        ind_max_s = 0;
        integral_ratio_status = -1000;
      }
      if (ind_max_l > nSamp - 1) {
        ind_max_l = nSamp - 1;
        integral_ratio_status = -1000;
      }
      if (ind_max_s > nSamp - 1) {
        ind_max_s = nSamp - 1;
        integral_ratio_status = -1000;
      }
      // short integral
      sintegral = MEnergyPlotsRedux_GetIntegral(samples, ind_min_s, ind_max_s);
      pedIntS = MEnergyPlotsRedux_GetPedestalIntegral(ind_min_s, ind_max_s, pedSlope, pedX0, pedY0);
      //sintegral_ps = polarity * (sintegral - pedIntS);
      sintegral_ps = sintegral - pedIntS;
      // long integral
      lintegral = MEnergyPlotsRedux_GetIntegral(samples, ind_min_l, ind_max_l);
      pedIntL = MEnergyPlotsRedux_GetPedestalIntegral(ind_min_l, ind_max_l, pedSlope, pedX0, pedY0);
      //lintegral_ps = polarity * (lintegral - pedIntL);
      lintegral_ps = lintegral - pedIntL;
      // force integrals to be non-negative
      //float fill = 0.0;
      //float fill = -0.01;      
      //if (sintegral_ps < 0.) sintegral_ps = fill;
      //if (lintegral_ps < 0.) lintegral_ps = fill;
      // avoid division by zero
      float lintegral_ps_nonzero = lintegral_ps;
      if (lintegral_ps < 0.1){
        lintegral_ps_nonzero = 0.1;
      }
      // integral ratio
      integral_ratio = sintegral_ps / lintegral_ps_nonzero;
      //////////////  Fill Hists  ///////////////////////
      //if(overflow) continue;

      double fit_max = (*pIter)->GetFitMax();

      double energy_amp = (*pIter)->GetEnergyAmp(max);
      double energy_int = (*pIter)->GetEnergyInt(integral_ps);
      double energy_fit = (*pIter)->GetEnergyFit(fit_max);

      // min energy tossing. i can do this in downstream analysis      
      //if(energy_int < 0.025) continue;

      // ntuples
      //Ntuple_map[bankname]->Fill(integral_ps,max,tMax,nSamp,polarity*sintegral,polarity*lintegral,sintegral_ps,lintegral_ps,integral_ratio,integral_ratio_status);
      // debug long & short integrals
      // "Integral:Amplitude:iMax:nSamp:SIntegral:LIntegral:SIntegral_pedsub:LIntegral_pedsub:IntegralRatio:status_IntegralRatio:Polarity:pedBegin:pedEnd:pedX0:pedSlope:pedInt:pedIntS:pedIntL"
      //const float vals [18] = {integral_ps,max,tMax,nSamp,polarity*sintegral,polarity*lintegral,sintegral_ps,lintegral_ps,integral_ratio,integral_ratio_status,polarity,pedBegin,pedEnd,pedX0,pedSlope,pedInt,pedIntS,pedIntL};
      const float vals [18] = {integral_ps,max,tMax,nSamp,sintegral,lintegral,sintegral_ps,lintegral_ps,integral_ratio,integral_ratio_status,polarity,pedBegin,pedEnd,pedX0,pedSlope,pedInt,pedIntS,pedIntL};
      Ntuple_map[bankname]->Fill(vals);
      // fails if n args > 15 !!      
      //Ntuple_map[bankname]->Fill(integral_ps,max,tMax,nSamp,polarity*sintegral,polarity*lintegral,sintegral_ps,lintegral_ps,integral_ratio,integral_ratio_status,polarity,pedBegin,pedEnd,pedX0,pedSlope,pedInt);
      //Ntuple_map[bankname]->Fill(integral_ps,max,tMax,nSamp,polarity*sintegral,polarity*lintegral,sintegral_ps,lintegral_ps,integral_ratio,integral_ratio_status,polarity,pedBegin,pedEnd,pedX0,pedSlope,pedInt,pedIntS,pedIntL);      
      //Ntuple_map[bankname]->Fill(integral_ps,max,sintegral_ps, lintegral_ps, integral_ratio);
      // histograms
      Integral_map[bankname]->Fill(integral_ps);
      Amplitude_map[bankname]->Fill(max);
      Fit_map[bankname]->Fill(fit_max);
      IntegralEnergy_map[bankname]->Fill(energy_int);
      AmplitudeEnergy_map[bankname]->Fill(energy_amp);
      FitEnergy_map[bankname]->Fill(energy_fit);
      IntegralVAmplitude_map[bankname]->Fill(energy_int, energy_amp);

    }
  }

  return SUCCESS;
}


std::vector<int> MEnergyPlotsRedux_FlipSamplePolarity(const std::vector<int>& samples, const int max_adc, int polarity){  
  std::vector<int> samples_new;  
  if (polarity > 0){
    //std::vector<int> samples_new (samples);
    samples_new = samples;
  }
  else {
    const int nSamp = samples.size();
    //std::vector<int> samples_new [nSamp];
    //std::vector<int> samples_new;
    for (int i=0; i<nSamp; i++){
      samples_new.push_back(-1*(samples.at(i) - max_adc));
      //if (polarity < 0){    
      //  samples_new.at(i) = -1*(samples.at(i) - max_adc);
      //}
      //else {
      //  samples_new.at(i) = samples.at(i)
      //}
    }
  }
  return samples_new;
}


void MEnergyPlotsRedux_SetPedestalParams(const std::vector<int>& samples, int nstart, int nend, std::string detname, float& pedBegin, float& pedEnd, float& pedSlope, float& pedX0, float& pedY0){
  // why not take the first sample?  
  //for(std::vector<int>::const_iterator pulse_begin = samples.begin() + 1; pulse_begin != samples.begin() + nstart + 1; pulse_begin++){  
  const int nSamp = samples.size();
  int pedBeginSamp=0, pedEndSamp=0;
  float pedXf=0;
  // FIXME! (?) why is there a special rule for TSc?
  if(detname == "TSc"){   
    std::cout << "TSc -- usings a flat pedestal with mean from first four samples." << std::endl;    
    pedBegin = (samples.at(0) + samples.at(1) + samples.at(2) + samples.at(3))/4;
    pedEnd = pedBegin;
  }
  else { 
    for(std::vector<int>::const_iterator pulse_begin = samples.begin(); pulse_begin != samples.begin() + nstart; pulse_begin++){
	    pedBegin += (*pulse_begin);
	    pedBeginSamp += 1;
    }

    //for(std::vector<int>::const_iterator pulse_end = samples.end() - 9; pulse_end != samples.end(); pulse_end++){
    for(std::vector<int>::const_iterator pulse_end = samples.end() - nend; pulse_end != samples.end(); pulse_end++){
	    pedEnd += (*pulse_end);
	    pedEndSamp += 1;
    }

    // checking i have the indexing correct
    if (pedBeginSamp != nstart) {
      std::cout << "pedBeginSamp!=nstart: " << pedBeginSamp << "!=" << nstart << std::endl;
    }
    if (pedEndSamp != nend) {
      std::cout << "peEndSamp!=nend: " << pedEndSamp << "!=" << nend << std::endl;
    }
  }

  pedBegin /= pedBeginSamp;
  pedEnd /= pedEndSamp;
  // setting parameters we use later (except pedXf)
  // mean of regions
  // WARNING! Can lead to negative values for pedInt in certain regions.  
  pedX0 = 0.0 + (nstart-1)/2.0;
  pedXf = (nSamp-1.0) - (nend-1)/2.0;
  // exactly at beginning and end
  //pedX0 = 0.0;
  //pedXf = (nSamp-1.0);
  pedSlope = (pedEnd - pedBegin)/(pedXf - pedX0);
  pedY0 = pedBegin;
}


float MEnergyPlotsRedux_GetPedestalIntegral(int start, int end, float& pedSlope, float& pedX0, float& pedY0){
  float pedInt = (pedSlope / 2.0) * (std::pow(end, 2) - std::pow(start, 2)) + (pedY0 - pedSlope * pedX0) * (end - start);
  return pedInt;
}


float MEnergyPlotsRedux_GetIntegral(const std::vector<int>& samples, int start_ind, int end_ind){
  float integral = 0.0;  
  //int nInt = 0;
  int lastSamp = 0;
  const int nSamp = samples.size();
  if(start_ind == -1000) start_ind=0;
  if(end_ind == -1000) end_ind=nSamp;
  const int nSamp_ = end_ind - start_ind;
  for(int i = start_ind; i+2 < end_ind; i += 2){
    //if(samples.at(i) == 0 || samples.at(i+1) == 0) overflow = true;
    //if(samples.at(i) == std::pow(2, nbits) || samples.at(i+1) == std::pow(2, nbits)) overflow = true;
    integral += (samples.at(i) + (4*samples.at(i+1)) + samples.at(i+2))/3;
    //nInt += 2;
    lastSamp = i+2;
  }
  if(nSamp_ % 2 == 0){
    integral += 0.5 * (samples.at(lastSamp) + samples.at(lastSamp + 1));
    //nInt ++;
    lastSamp++;
  }
  if(nSamp_ % 2 == 1){
    std::cout << "Non-modulo 2 check" << std:: endl;
    // Cole -- I think this double counts contribution from the final bin. Should be a small effect, but good to correct it.  
    //integral += 0.5 * (samples.at(nSamp-1) + samples.at(nSamp));
    //nInt++;
  }

  return integral;
}



