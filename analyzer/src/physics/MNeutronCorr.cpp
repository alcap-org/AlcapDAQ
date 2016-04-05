/* Standard includes */
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
static INT MNeutronCorr_init(void);
static INT MNeutronCorr(EVENT_HEADER*, void*);
float MNeutronCorr_IntEnergy(std::string, float);  //integral energies only
float MNeutronCorr_nSamples(std::string);
float MNeutronCorr_Threshold(std::string);
bool MNeutronCorr_Neutron(std::string, double, float);
float MNeutronCorr_Energy(std::string, float);


extern HNDLE hDB;
extern TGlobalData* gData;
extern TSetupData* gSetup;


using namespace AlCap;
namespace {
  TH2D* vvhNeutronCorr_EDvTDiff;
  TH2D* vvhNeutronCorr_EUvTDiff;
  TH2D* vvhNeutronCorr_EDvNTCorr;
  TH2D* vvhNeutronCorr_EUvNTCorr;
  TH2D* vvhNeutronCorr_EDvEU;
  const double TIME_LOW = -5e3, TIME_HIGH = 1e4;
  const double CORR_LOW = -200, CORR_HIGH = 200;
}


ANA_MODULE MNeutronCorr_module =
{
  "MNeutronCorr",       /* module name           */
  "Damien Alexander",  /* author                */
  MNeutronCorr,         /* event routine         */
  NULL,                /* BOR routine           */
  NULL,                /* EOR routine           */
  MNeutronCorr_init,    /* init routine          */
  NULL,                /* exit routine          */
  NULL,                /* parameter structure   */
  0,                   /* structure size        */
  NULL,                /* initial parameters    */
};


INT MNeutronCorr_init() {
  TDirectory* cwd = gDirectory;
  gDirectory->mkdir("NeutronCorr")->cd();
  //select channels and corresponding vetos
      
  std::string bankD = gSetup->GetBankName("NdetD");
  std::string bankU = gSetup->GetBankName("NdetU");

  int nBitsU = gSetup->GetNBits(bankU);
  int max_adc_U = std::pow(2, nBitsU);
  float max_int_U = max_adc_U * MNeutronCorr_nSamples("NdetU"); 
  float max_int_e_U = MNeutronCorr_IntEnergy("NdetU", max_int_U);

  int nBitsD = gSetup->GetNBits(bankD);
  int max_adc_D = std::pow(2, nBitsD);
  float max_int_D = max_adc_D * MNeutronCorr_nSamples("NdetD");
  float max_int_e_D = MNeutronCorr_IntEnergy("NdetD", max_int_D);

  //setup histograms

  ///////////// NdetD E vs TTSc TDiff  //////////////////////
  char histname[64]; sprintf(histname, "hNeutronCorr_EDvTDiff");
  char histtitle[64]; sprintf(histtitle, "NdetD Energy vs TSC TDiff");
  vvhNeutronCorr_EDvTDiff = new TH2D(histname, histtitle, (TIME_HIGH - TIME_LOW)/10, TIME_LOW, TIME_HIGH, max_adc_D/4, 0, max_int_e_D);
  vvhNeutronCorr_EDvTDiff->GetXaxis()->SetTitle("TDiff (ns)");
  vvhNeutronCorr_EDvTDiff->GetYaxis()->SetTitle("Energy (MeV)");

  ///////////// NdetU E v TTSc TDiff //////////////////////
  sprintf(histname, "hNeutronCorr_EUvTDiff");
  sprintf(histtitle, "NdetU Energy vs TSC TDiff");
  vvhNeutronCorr_EUvTDiff = new TH2D(histname, histtitle, (TIME_HIGH - TIME_LOW)/10, TIME_LOW, TIME_HIGH, max_adc_U/4, 0, max_int_e_U);
  vvhNeutronCorr_EUvTDiff->GetXaxis()->SetTitle("TDiff (ns)");
  vvhNeutronCorr_EUvTDiff->GetYaxis()->SetTitle("Energy (MeV)");

  ///////////// NdetD E vs NN TDiff  //////////////////////
  sprintf(histname, "hNeutronCorr_EDvNTCorr");
  sprintf(histtitle, "Energy vs NN TDiff");
  vvhNeutronCorr_EDvNTCorr = new TH2D(histname, histtitle, (CORR_HIGH - CORR_LOW), CORR_LOW, CORR_HIGH, max_adc_D/4, 0, max_int_e_D);
  vvhNeutronCorr_EDvNTCorr->GetXaxis()->SetTitle("TDiff (ns)");
  vvhNeutronCorr_EDvNTCorr->GetYaxis()->SetTitle("Energy (MeV)");

  ///////////// NdetU E v NN TDiff //////////////////////
  sprintf(histname, "hNeutronCorr_EUvNTCorr");
  sprintf(histtitle, "NdetU Energy vs NN TDiff");
  vvhNeutronCorr_EUvNTCorr = new TH2D(histname, histtitle, (CORR_HIGH - CORR_LOW), CORR_LOW, CORR_HIGH, max_adc_U/4, 0, max_int_e_U);
  vvhNeutronCorr_EUvNTCorr->GetXaxis()->SetTitle("TDiff (ns)");
  vvhNeutronCorr_EUvNTCorr->GetYaxis()->SetTitle("Energy (MeV)");

  ///////////// NdetD E v NdetU E //////////////////////
  sprintf(histname, "hNeutronCorr_EDvEU");
  sprintf(histtitle, "NdetD Energy vs NdetU Energy");
  vvhNeutronCorr_EDvEU = new TH2D(histname, histtitle, max_adc_D/8, 0, max_int_e_D, max_adc_U/8, 0, max_int_e_U);
  vvhNeutronCorr_EDvEU->GetXaxis()->SetTitle("NdetD Energy (MeV)");
  vvhNeutronCorr_EDvEU->GetYaxis()->SetTitle("NdetU Energy (MeV)");

  cwd->cd();
  return SUCCESS;

}

INT MNeutronCorr(EVENT_HEADER *pheader, void *pevent) {
  const map< string, vector<TPulseIsland*> >& wfd_map = gData->fPulseIslandToChannelMap;

  //setup common channels

  std::string ref_bank = gSetup->GetBankName("TSc");
  if(!wfd_map.count(ref_bank)){
      printf("MNdetUGCorr: No reference hits TSc!\n");
      return SUCCESS;
  }
  const std::vector<TPulseIsland*>& ref_pulses = wfd_map.at(ref_bank);

  std::string nup_bank = gSetup->GetBankName("NdetU");
  if(!wfd_map.count(nup_bank)){
    printf("MNdetUGCorr: No reference hits NdetU!\n");
    return SUCCESS;
  }
  const std::vector<TPulseIsland*>& nup_pulses = wfd_map.at(nup_bank);

  std::string ndown_bank = gSetup->GetBankName("NdetD");
  if(!wfd_map.count(ndown_bank)){
    printf("MNdetUGCorr: No reference hits NdetD!\n");
    return SUCCESS;
  }
  const std::vector<TPulseIsland*>& ndown_pulses = wfd_map.at(ndown_bank);

  
  int nPolarity = gSetup->GetTriggerPolarity(nup_bank);
  //cycle through neutron pulses
  for(int p = 0; p < nup_pulses.size(); p++){
    float nIntegral = nup_pulses[p]->GetIntegral();
    float nPSD = nup_pulses[p]->GetPSDParameter();
    int64_t nTDC = nup_pulses[p]->GetTDCTime();
    float nThreshold = MNeutronCorr_Threshold("NdetU");
    float nMax = nup_pulses[p]->GetPulseHeight();
    float nPedestal = nup_pulses[p]->GetPedestal(8);
    bool nCoincCheck = false;
    double nTDiff = 0;

    const std::vector<int>& nSamples = nup_pulses[p]->GetSamples();
    float energy_U = MNeutronCorr_Energy("NdetU", nMax);
    float energyI_U = MNeutronCorr_IntEnergy("NdetU", nIntegral);


    if(!(MNeutronCorr_Neutron("NdetU", nPSD, energy_U))) continue; //gamma
    if(nMax < nPolarity*(nThreshold - nPedestal)) continue;
    if(*(std::min_element(nSamples.begin(), nSamples.end())) == 0) continue;
    if(*(std::max_element(nSamples.begin(), nSamples.end())) >= 16383) continue;

    
    if(nup_pulses[p]->GetVetoPulse()) continue; // neutron signal vetoed
    

    //correlate to TTSc, or throw away
    for(int t = 0; t < ref_pulses.size(); ++t){
      if(ref_pulses[t]->GetVetoPulse()) continue;
      double dt = TICKTDC * (nTDC - ref_pulses[t]->GetTDCTime())/* + 55*/;
      if(dt < TIME_LOW) break;
      else if(dt < TIME_HIGH){
	nCoincCheck = true;
	nTDiff = dt;
      }

    }
    if(!nCoincCheck) continue;

    const int polarity = gSetup->GetTriggerPolarity(ndown_bank);
    const int nBits = gSetup->GetNBits(ndown_bank);
    const int max_adc = std::pow(2, nBits) -1;
    for(int d = 0; d< ndown_pulses.size(); ++d){
      if(ndown_pulses[d]->GetVetoPulse()) continue;
      double dt_n = TICKTDC*(nTDC - ndown_pulses[d]->GetTDCTime());
      if(dt_n < CORR_LOW) break;
      else if (dt_n > CORR_HIGH) continue; //next pulse
      //otherwise, continue


      float pedestal = ndown_pulses[d]->GetPedestal(8);
      const std::vector<int>& samples = ndown_pulses[d]->GetSamples();

      float integral_ps = ndown_pulses[d]->GetIntegral();
      float max = ndown_pulses[d]->GetPulseHeight();
      float threshold = MNeutronCorr_Threshold("NdetD");

      //pulse detail checks
      if(max < polarity*(threshold-pedestal)) continue;
      if(*(std::min_element(samples.begin(), samples.end())) == 0) continue;
      if(*(std::max_element(samples.begin(), samples.end())) >= max_adc) continue;

      float energyI_D = MNeutronCorr_IntEnergy("NdetD", integral_ps);
      float energy_D = MNeutronCorr_Energy("NdetD", max);
      float dPSD = ndown_pulses[d]->GetPSDParameter();

      if(!(MNeutronCorr_Neutron("NdetD", dPSD, energy_D))) continue; //gamma
      if(ndown_pulses[d]->GetVetoPulse()) continue;

      //look for correlated signal
     

      //two energies, energy_D and energy_U
      //two times, nTDiff and dt_n

      vvhNeutronCorr_EUvTDiff->Fill(nTDiff, energy_U);
      vvhNeutronCorr_EDvTDiff->Fill(nTDiff, energy_D);
      vvhNeutronCorr_EUvNTCorr->Fill(dt_n, energy_U);
      vvhNeutronCorr_EDvNTCorr->Fill(dt_n, energy_D);
      vvhNeutronCorr_EDvEU->Fill(energy_D, energy_U);
	
      
    }
  }

  return SUCCESS;
}
  
float MNeutronCorr_Energy(std::string detname, float amp) {
  float energy = 0;
  if(detname == "NdetD"){ energy = (amp * 0.0003999) + 0.008234;  }
  else if(detname == "NdetU"){ energy = (amp * 0.0004015) + 0.009037;  }
  else if(detname == "GeCHEH"){ energy = (amp * 0.0001522) - 0.0004239;  }
  else if(detname == "GeCHEL"){ energy = (amp * 0.0003858) - 0.0017;    }
  else if(detname == "LaBr3"){ energy = (amp * 0.00163022) - 0.00836618;  } 
  else energy = amp;
  return energy;
}

float MNeutronCorr_IntEnergy(std::string detname, float Int) {
  float energy = 0;
  if(detname == "NdetD"){ energy = (0.0000686 * Int) + 0.02117;  }
  else if(detname == "NdetU"){ energy = (0.00006766 * Int) + 0.02358;  }
  else if(detname == "GeCHEH"){ energy = 0.000002033 * Int;  }
  else if(detname == "GeCHEL"){ energy = 0.000005145 * Int;  }
  else if(detname == "LaBr3"){ energy = 0.0000823 * Int;  } 
  else energy = Int;
  return energy;
}

float MNeutronCorr_nSamples(std::string detname){
  float nSamples = 0;
  if(detname == "TSc") nSamples = 5;//12
  else if(detname == "NdetU" || detname == "NdetD") nSamples = 7.5;//150
  else if(detname == "GeCHEH" || detname == "GeCHEL") nSamples = 80;//300
  else if(detname == "LaBr3") nSamples = 24;//128
  else nSamples = 100;
  return nSamples;
}

float MNeutronCorr_Threshold(std::string detname){
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

bool MNeutronCorr_Neutron(std::string det, double ratio, float energy){

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
