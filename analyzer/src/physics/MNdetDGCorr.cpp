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
static INT MNdetDGCorr_init(void);
static INT MNdetDGCorr(EVENT_HEADER*, void*);
float MNdetDGCorr_IntEnergy(std::string, float);  //integral energies only
float MNdetDGCorr_nSamples(std::string);
float MNdetDGCorr_Threshold(std::string);

extern HNDLE hDB;
extern TGlobalData* gData;
extern TSetupData* gSetup;


using namespace AlCap;
namespace {
  std::string WFDBANKS[NCRATE][MAXNCHANWFD];
  std::string TDCBANKS[NCRATE][MAXNCHANWFD];
  TH2D* vvhNdetDGCorr_EGvTDiff[NCRATE][MAXNCHANWFD];
  TH2D* vvhNdetDGCorr_ENvTDiff[NCRATE][MAXNCHANWFD];
  TH2D* vvhNdetDGCorr_EGvGTCorr[NCRATE][MAXNCHANWFD];
  TH2D* vvhNdetDGCorr_ENvGTCorr[NCRATE][MAXNCHANWFD];
  TH2D* vvhNdetDGCorr_ENvEG[NCRATE][MAXNCHANWFD];
  const double TIME_LOW = -5e3, TIME_HIGH = 1e4;
  const double CORR_LOW = -200, CORR_HIGH = 200;
}


ANA_MODULE MNdetDGCorr_module =
{
  "MNdetDGCorr",       /* module name           */
  "Damien Alexander",  /* author                */
  MNdetDGCorr,         /* event routine         */
  NULL,                /* BOR routine           */
  NULL,                /* EOR routine           */
  MNdetDGCorr_init,    /* init routine          */
  NULL,                /* exit routine          */
  NULL,                /* parameter structure   */
  0,                   /* structure size        */
  NULL,                /* initial parameters    */
};


INT MNdetDGCorr_init() {
  TDirectory* cwd = gDirectory;
  gDirectory->mkdir("NdetDGCorr")->cd();
  //select channels and corresponding vetos
  for(int icrate = 0; icrate < NCRATE; ++icrate){
  
    for(int ich = 0; ich < NCHANWFD[icrate]; ++ich) {
      
      char bank[5]; sprintf(bank, "D%d%02d", icrate, ich);
      std::string det = gSetup->GetDetectorName(bank);

      if(det == "NdetU" || det == "LaBr3"){
	WFDBANKS[icrate][ich] = bank;
	TDCBANKS[icrate][ich] = gSetup->GetBankName("T" + det);
      }
      else if(det == "GeCHEH" || det == "GeCHEL"){
	WFDBANKS[icrate][ich] = bank;
	TDCBANKS[icrate][ich] = gSetup->GetBankName("TGeCHT");
      }
      else{
	WFDBANKS[icrate][ich] = "";
	TDCBANKS[icrate][ich] = "";
	continue;  // ignoring SYNC pulses, Rollover, LYSO, GeCHT, and empty
      }  //NdetD and TTSc handled separately

      int nBits = gSetup->GetNBits(bank);
      int max_adc = std::pow(2, nBits);
      float max_int = max_adc * MNdetDGCorr_nSamples(det); 
      float max_int_e = MNdetDGCorr_IntEnergy(det, max_int);

      int nBitsD = gSetup->GetNBits(gSetup->GetBankName("NdetD"));
      int max_adc_D = std::pow(2, nBitsD);
      float max_int_D = max_adc_D * MNdetDGCorr_nSamples("NdetD");
      float max_int_e_D = MNdetDGCorr_IntEnergy("NdetD", max_int_D);

      //setup histograms

      ///////////// Gamma E vs TTSc TDiff  //////////////////////
      char histname[64]; sprintf(histname, "hNdetDGCorr_EGvTDiff_%s", det.c_str());
      char histtitle[64]; sprintf(histtitle, "Gamma Energy vs TSC TDiff for %s", det.c_str());
      vvhNdetDGCorr_EGvTDiff[icrate][ich] = new TH2D(histname, histtitle, (TIME_HIGH - TIME_LOW)/10, TIME_LOW, TIME_HIGH, max_adc/4, 0, max_int_e);
      vvhNdetDGCorr_EGvTDiff[icrate][ich]->GetXaxis()->SetTitle("TDiff (TDC) (ns)");
      vvhNdetDGCorr_EGvTDiff[icrate][ich]->GetYaxis()->SetTitle("Gamma Energy (MeV)");

      ///////////// Neutron E v TTSc TDiff //////////////////////
      sprintf(histname, "hNdetDGCorr_ENvTDiff_%s", det.c_str());
      sprintf(histtitle, "Neutron Energy vs TSC TDiff for %s", det.c_str());
      vvhNdetDGCorr_ENvTDiff[icrate][ich] = new TH2D(histname, histtitle, (TIME_HIGH - TIME_LOW)/10, TIME_LOW, TIME_HIGH, max_adc_D/4, 0, max_int_e_D);
      vvhNdetDGCorr_ENvTDiff[icrate][ich]->GetXaxis()->SetTitle("TDiff (TDC) (ns)");
      vvhNdetDGCorr_ENvTDiff[icrate][ich]->GetYaxis()->SetTitle("Neutron Energy (MeV)");

      ///////////// Gamma E vs NG TDiff  //////////////////////
      sprintf(histname, "hNdetDGCorr_EGvGTCorr_%s", det.c_str());
      sprintf(histtitle, "Gamma Energy vs NG TDiff for %s", det.c_str());
      vvhNdetDGCorr_EGvGTCorr[icrate][ich] = new TH2D(histname, histtitle, (CORR_HIGH - CORR_LOW), CORR_LOW, CORR_HIGH, max_adc/4, 0, max_int_e);
      vvhNdetDGCorr_EGvGTCorr[icrate][ich]->GetXaxis()->SetTitle("TDiff (TDC) (ns)");
      vvhNdetDGCorr_EGvGTCorr[icrate][ich]->GetYaxis()->SetTitle("Gamma Energy (MeV)");

      ///////////// Neutron E v NG TDiff //////////////////////
      sprintf(histname, "hNdetDGCorr_ENvGTCorr_%s", det.c_str());
      sprintf(histtitle, "Neutron Energy vs NG TDiff for %s", det.c_str());
      vvhNdetDGCorr_ENvGTCorr[icrate][ich] = new TH2D(histname, histtitle, (CORR_HIGH - CORR_LOW), CORR_LOW, CORR_HIGH, max_adc_D/4, 0, max_int_e_D);
      vvhNdetDGCorr_ENvGTCorr[icrate][ich]->GetXaxis()->SetTitle("TDiff (TDC) (ns)");
      vvhNdetDGCorr_ENvGTCorr[icrate][ich]->GetYaxis()->SetTitle("Neutron Energy (MeV)");

      ///////////// Neutron E v Gamma E //////////////////////
      sprintf(histname, "hNdetDGCorr_ENvEG_%s", det.c_str());
      sprintf(histtitle, "Neutron Energy vs Gamma Energy for %s", det.c_str());
      vvhNdetDGCorr_ENvEG[icrate][ich] = new TH2D(histname, histtitle, max_adc_D/4, 0, max_int_e_D, max_adc/4, 0, max_int_e);
      vvhNdetDGCorr_ENvEG[icrate][ich]->GetXaxis()->SetTitle("Neutron Energy (MeV)");
      vvhNdetDGCorr_ENvEG[icrate][ich]->GetYaxis()->SetTitle("Gamma Energy (MeV)");

    }
  

  }
  cwd->cd();
  return SUCCESS;

}

INT MNdetDGCorr(EVENT_HEADER *pheader, void *pevent) {
  const map< string, vector<TPulseIsland*> >& wfd_map = gData->fPulseIslandToChannelMap;

  //setup common channels

  std::string ref_bank = gSetup->GetBankName("TSc");
  if(!wfd_map.count(ref_bank)){
      printf("MNdetDGCorr: No reference hits TSc!\n");
      return SUCCESS;
  }
  const std::vector<TPulseIsland*>& ref_pulses = wfd_map.at(ref_bank);

  std::string neutron_bank = gSetup->GetBankName("NdetD");
  if(!wfd_map.count(neutron_bank)){
    printf("MNdetDGCorr: No reference hits NdetD!\n");
    return SUCCESS;
  }
  const std::vector<TPulseIsland*>& neutron_pulses = wfd_map.at(neutron_bank);


  
  int nPolarity = gSetup->GetTriggerPolarity(gSetup->GetBankName("NdetD"));
  //cycle through neutron pulses
  for(int p = 0; p < neutron_pulses.size(); p++){
    float nIntegral = neutron_pulses[p]->GetIntegral();
    float nPSD = neutron_pulses[p]->GetPSDParameter();
    int64_t nTDC = neutron_pulses[p]->GetTDCTime();
    float nThreshold = MNdetDGCorr_Threshold("NdetD");
    float nMax = neutron_pulses[p]->GetPulseHeight();
    float nPedestal = neutron_pulses[p]->GetPedestal(8);
    bool nCoincCheck = false;
    double nTDiff = 0;

    const std::vector<int>& nSamples = neutron_pulses[p]->GetSamples();


    if(nPSD < 1) continue;//gamma, skip
    if(nMax < nPolarity*(nThreshold - nPedestal)) continue;
    if(*(std::min_element(nSamples.begin(), nSamples.end())) == 0) continue;
    if(*(std::max_element(nSamples.begin(), nSamples.end())) >= 16383) continue;

    
    if(neutron_pulses[p]->GetVetoPulse()) continue; // neutron signal vetoed
    

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


    //cycle through other channels
    for(int icrate = 0; icrate < NCRATE; ++icrate){
      for(int ich = 0; ich < NCHANWFD[icrate]; ++ich) {
	if(WFDBANKS[icrate][ich] == " ") continue;
	if(!wfd_map.count(WFDBANKS[icrate][ich])) continue;
	std::string det=gSetup->GetDetectorName(WFDBANKS[icrate][ich]);

	const int polarity = gSetup->GetTriggerPolarity(WFDBANKS[icrate][ich]);
	const int nBits = gSetup->GetNBits(WFDBANKS[icrate][ich]);
	const int max_adc = std::pow(2, nBits) - 1;

	double tcorr = 0;
	if(det == "GeCHEH" || det == "GeCHEL") tcorr = -120; 
	if(det == "LaBr3") tcorr = 110;
	if(det == "NdetU") tcorr = 0;


	if(!wfd_map.count(WFDBANKS[icrate][ich])){
	  printf("MNdetDGCorr: No reference hits %s!\n", WFDBANKS[icrate][ich].c_str());
	  return SUCCESS;
	}
	const std::vector<TPulseIsland*>& pulses = wfd_map.at(WFDBANKS[icrate][ich]);

	//look for correlated signal
      
	for(int g = 0; g < pulses.size(); ++g){
	  //setup variables
	  double gTCorr = TICKTDC*(nTDC - pulses[g]->GetTDCTime()) - tcorr;
	  if(gTCorr < CORR_LOW) break;
	  else if(gTCorr > CORR_HIGH) continue; // next pulse
	  //otherwise, keep going

	  //std::cout << "Neutron and Gamma correlated in " << WFDBANKS[icrate][ich] << std::endl;

	  float pedestal = pulses[g]->GetPedestal(8);
	  const std::vector<int>& samples = pulses[g]->GetSamples();
	  const int nSamp = samples.size();
	  if(nSamp < 8) continue;

	  float integral_ps = pulses[g]->GetIntegral();
	  float max = pulses[g]->GetPulseHeight();
	  float threshold = MNdetDGCorr_Threshold(det);

	  //pulse detail checks
	  if(max < polarity*(threshold-pedestal)) continue;
	  if(*(std::min_element(samples.begin(), samples.end())) == 0) continue;
	  if(*(std::max_element(samples.begin(), samples.end())) >= max_adc) continue;
	  if(gSetup->IsNeutron(det) && pulses[p]->GetPSDParameter() > 1) continue; //neutron, skip
	  if(pulses[g]->GetVetoPulse()) continue;
	  


	
	  float energy_int = MNdetDGCorr_IntEnergy(det, integral_ps);
	  float energy_int_D = MNdetDGCorr_IntEnergy("NdetD", nIntegral);

	  //two energies, energy_int and energy_int_D
	  //two times, nTDiff and gTCorr

	  vvhNdetDGCorr_EGvTDiff[icrate][ich]->Fill(nTDiff, energy_int);
	  vvhNdetDGCorr_ENvTDiff[icrate][ich]->Fill(nTDiff, energy_int_D);
	  vvhNdetDGCorr_EGvGTCorr[icrate][ich]->Fill(gTCorr, energy_int);
	  vvhNdetDGCorr_ENvGTCorr[icrate][ich]->Fill(gTCorr, energy_int_D);
	  vvhNdetDGCorr_ENvEG[icrate][ich]->Fill(energy_int_D, energy_int);
	}
      }
    }
  }

  return SUCCESS;
} 
  

float MNdetDGCorr_IntEnergy(std::string detname, float Int) {
  float energy = 0;
  if(detname == "NdetD"){ energy = (0.0000686 * Int) + 0.02117;  }
  else if(detname == "NdetU"){ energy = (0.00006766 * Int) + 0.02358;  }
  else if(detname == "GeCHEH"){ energy = 0.000002033 * Int;  }
  else if(detname == "GeCHEL"){ energy = 0.000005145 * Int;  }
  else if(detname == "LaBr3"){ energy = 0.0000823 * Int;  } 
  else energy = Int;
  return energy;
}

float MNdetDGCorr_nSamples(std::string detname){
  float nSamples = 0;
  if(detname == "TSc") nSamples = 5;//12
  else if(detname == "NdetD" || detname == "NdetU") nSamples = 7.5;//150
  else if(detname == "GeCHEH" || detname == "GeCHEL") nSamples = 80;//300
  else if(detname == "LaBr3") nSamples = 24;//128
  else nSamples = 100;
  return nSamples;
}

float MNdetDGCorr_Threshold(std::string detname){
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
