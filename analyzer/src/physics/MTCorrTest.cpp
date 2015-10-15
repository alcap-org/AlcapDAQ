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
static INT MTCorrTest_init(void);
static INT MTCorrTest(EVENT_HEADER*, void*);
float MTCorrTest_Energy(std::string, float);
float MTCorrTest_IntEnergy(std::string, float);
float MTCorrTest_nSamples(std::string);
float MTCorrTest_Threshold(std::string);

extern HNDLE hDB;
extern TGlobalData* gData;
extern TSetupData* gSetup;


using namespace AlCap;
namespace {
  std::string WFDBANKS[NCRATE][MAXNCHANWFD];
  std::string TDCBANKS[NCRATE][MAXNCHANWFD];
  std::string VETOBANKS[NCRATE][MAXNCHANWFD];
  TH2D* vvhTCorrTest_EvTDiff[NCRATE][MAXNCHANWFD];
  TH2D* vvhTCorrTest_IEvTDiff[NCRATE][MAXNCHANWFD];
  const double TIME_LOW = -5e3, TIME_HIGH = 1e4;
  const double VETO_LOW = -2000, VETO_HIGH = 2000;
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
	TDCBANKS[icrate][ich] = gSetup->GetBankName("T" + det);
	VETOBANKS[icrate][ich] = gSetup->GetBankName("T" + det + "V");
	std::cout << WFDBANKS[icrate][ich] << "   " << TDCBANKS[icrate][ich] << "   " << VETOBANKS[icrate][ich] << std::endl;
      }
      else if(det == "GeCHEH" || det == "GeCHEL"){
	WFDBANKS[icrate][ich] = bank;
	TDCBANKS[icrate][ich] = gSetup->GetBankName("TGeCHT");
	VETOBANKS[icrate][ich] = gSetup->GetBankName("TGeV");
      }
      else if(det == "TSc") {
	WFDBANKS[icrate][ich] = bank;
	TDCBANKS[icrate][ich] = gSetup->GetBankName("TTSc");
	VETOBANKS[icrate][ich] = gSetup->GetBankName("TVSc");
      }
      else{
	WFDBANKS[icrate][ich] = "";
	TDCBANKS[icrate][ich] = "";
	VETOBANKS[icrate][ich] = "";
	continue;  // ignoring SYNC pulses, Rollover, LYSO, GeCHT, and empty
      }

      int nBits = gSetup->GetNBits(bank);
      int max_adc = std::pow(2, nBits);
      float max_amp_e = MTCorrTest_Energy(det, max_adc);
      float max_int = max_adc * MTCorrTest_nSamples(det); 
      float max_int_e = MTCorrTest_IntEnergy(det, max_int);

      
      //setup histograms

      ///////////// E vs TDiff  //////////////////////
      char histname[64]; sprintf(histname, "hTCorrTest_EvTDiff_%s", det.c_str());
      char histtitle[64]; sprintf(histtitle, "Energy vs TSC TDiff for %s", det.c_str());
      vvhTCorrTest_EvTDiff[icrate][ich] = new TH2D(histname, histtitle, (TIME_HIGH - TIME_LOW)/10, TIME_LOW, TIME_HIGH, max_adc/4, 0, max_amp_e);
      vvhTCorrTest_EvTDiff[icrate][ich]->GetXaxis()->SetTitle("TDiff (TDC) (ns)");
      vvhTCorrTest_EvTDiff[icrate][ich]->GetYaxis()->SetTitle("Energy (amplitude) (MeV)");

      ///////////// IE v TDiff //////////////////////
      sprintf(histname, "hTCorrTest_IEvTDiff_%s", det.c_str());
      sprintf(histtitle, "Energy vs TSC TDiff for %s", det.c_str());
      vvhTCorrTest_IEvTDiff[icrate][ich] = new TH2D(histname, histtitle, (TIME_HIGH - TIME_LOW)/10, TIME_LOW, TIME_HIGH, max_adc/4, 0, max_int_e);
      vvhTCorrTest_IEvTDiff[icrate][ich]->GetXaxis()->SetTitle("TDiff (TDC) (ns)");
      vvhTCorrTest_IEvTDiff[icrate][ich]->GetYaxis()->SetTitle("Energy (integral) (MeV)");
    }
  

  }
  cwd->cd();
  return SUCCESS;

}

INT MTCorrTest(EVENT_HEADER *pheader, void *pevent) {
  const map< string, vector<TPulseIsland*> >& wfd_map = gData->fPulseIslandToChannelMap;
  const map<string, vector<int64_t> >& tdc_map = gData->fTDCHitsToChannelMap;
  std::string ref_bank = gSetup->GetBankName("TTSc");
  if(!tdc_map.count(ref_bank)){
      printf("MTCorrTest: No reference hits TTSc!\n");
      return SUCCESS;
  }
    
  const std::vector<int64_t>& ref_times = tdc_map.at(ref_bank);
    
  for(int icrate = 0; icrate < NCRATE; ++icrate){
    for(int ich = 0; ich < NCHANWFD[icrate]; ++ich) {
      if(WFDBANKS[icrate][ich] == " ") continue;
      if(!wfd_map.count(WFDBANKS[icrate][ich])) continue;
      std::string det=gSetup->GetDetectorName(WFDBANKS[icrate][ich]);

      const int polarity = gSetup->GetTriggerPolarity(WFDBANKS[icrate][ich]);
      const int nBits = gSetup->GetNBits(WFDBANKS[icrate][ich]);
      const int max_adc = std::pow(2, nBits) - 1;

      const std::vector<TPulseIsland*>& pulses = wfd_map.at(WFDBANKS[icrate][ich]);
      if(!tdc_map.count(VETOBANKS[icrate][ich])) {
	std::cout << "No veto hits found for detector" << det << std::endl;
	continue;
      }

      const std::vector<int64_t>& veto_hits = tdc_map.at(VETOBANKS[icrate][ich]);

      //two loops, pulse loop
      int t0 = 0;
      for(int p = 0; p < pulses.size(); ++p){
	//setup variables
	float pedestal = pulses[p]->GetPedestal(8);
	if(det == "TSc") pedestal = pulses[p]->GetPedestal(4);
	const std::vector<int>& samples = pulses[p]->GetSamples();
	const int nSamp = samples.size();
	if(nSamp < 8) continue;
	bool VetoCheck = false;

	float integral_ps = pulses[p]->GetIntegral();
	float max = pulses[p]->GetPulseHeight();
	float threshold = MTCorrTest_Threshold(det);

	//pulse detail checks
	if(max < polarity*(threshold-pedestal)) continue;
	if(*(std::min_element(samples.begin(), samples.end())) == 0) continue;
	if(*(std::max_element(samples.begin(), samples.end())) >= max_adc) continue;
	if(gSetup->IsNeutron(WFDBANKS[icrate][ich]) && pulses[p]->GetPSDParameter() < 1) continue; //gamma
	
	//veto check
	for(int v = 0; v < veto_hits.size(); ++v){
	  double dt = TICKTDC*(pulses[p]->GetTDCTime() - veto_hits[v]);
	  
	  if(dt < VETO_LOW){
	    break;
	  }
	  else if(dt < VETO_HIGH){
	    VetoCheck = true;
	    break;
	  }
	}


	if(VetoCheck) continue;
	
	float energy_amp = MTCorrTest_Energy(det, max);
	float energy_int = MTCorrTest_IntEnergy(det, integral_ps);

	//loop over TTSc times
	for(int t = t0; t<ref_times.size(); ++t){
	  double dt = TICKTDC*pulses[p]->GetTDCTime() - TICKTDC*ref_times[t];

	  if(dt < TIME_LOW) break;
	  else if(dt < TIME_HIGH){
	    vvhTCorrTest_EvTDiff[icrate][ich]->Fill(dt, energy_amp);
	    vvhTCorrTest_IEvTDiff[icrate][ich]->Fill(dt, energy_int);
	  }
	  else ++t0;
	}
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

