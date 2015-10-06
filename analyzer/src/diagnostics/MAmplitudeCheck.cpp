/* Standard includes */
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <map>
#include <utility>
#include <sstream>
#include <cmath>

/* MIDAS includes */
#include "midas.h"

/* ROOT includes */
#include <TH1.h>
#include <TH2.h>
#include <TF1.h>
#include <TDirectory.h>

/* AlCap includes */
#include "TGlobalData.h"
#include "TSetupData.h"
#include "TPulseIsland.h"

using std::string;
using std::map;
using std::vector;
using std::pair;

/*-- Module declaration --------------------------------------------*/
INT  MAmplitudeCheck_init(void);
INT  MAmplitudeCheck(EVENT_HEADER*, void*);
INT  MAmplitudeCheck_eor(INT);

extern HNDLE hDB;
extern TGlobalData* gData;
extern TSetupData* gSetup;


#include "TDirectory.h"
#include "TFile.h"
#include "TApplication.h"
#include "TROOT.h"
extern TDirectory * gManaHistsDir;
extern TFile * gManaOutputFile;
extern TApplication * manaApp;
extern TROOT * gROOT;

map <std::string, TH1F*> AmplitudeCheck_PH_map;
map <std::string, TH1F*> AmplitudeCheck_Gauss_map;


ANA_MODULE MAmplitudeCheck_module =
{
	"MAmplitudeCheck",             /* module name           */
	"Damien Alexander",            /* author                */
	MAmplitudeCheck,               /* event routine         */
	NULL,                          /* BOR routine           */
	MAmplitudeCheck_eor,           /* EOR routine           */
	MAmplitudeCheck_init,          /* init routine          */
	NULL,                          /* exit routine          */
	NULL,                          /* parameter structure   */
	0,                             /* structure size        */
	NULL,                          /* initial parameters    */
};

INT MAmplitudeCheck_init()
{
  if(!gDirectory->Cd("AmplitudeCheck")){
    gDirectory->mkdir("AmplitudeCheck/");
    gDirectory->Cd("AmplitudeCheck/");
  }

    map<string, string> Bank2DetMap = gSetup->fBankToDetectorMap;

    for(map<string, string>::iterator mIter = Bank2DetMap.begin();
	mIter != Bank2DetMap.end(); mIter++) {

      string bankname = mIter->first;
      if(!gSetup->IsWFD(bankname)) continue;
      string detname = gSetup->GetDetectorName(bankname);
      int n_bits = gSetup->GetNBits(bankname);
      int max_adc = std::pow(2, n_bits);

      if(detname != "TSc"
	 && detname != "GeCHEH"
	 && detname != "GeCHEL"
	 && detname != "NdetD"
	 && detname != "NdetU"
	 && detname != "LaBr3")
	continue;


      string histname = "hAmplitudeCheck_PH_" + detname + "_" + bankname;
      string histtitle = "Pulse Height distribution in " + detname;
      TH1F* hAmplitudeCheck_PH = new TH1F(histname.c_str(), histtitle.c_str(),
					  max_adc, 0, max_adc);
      hAmplitudeCheck_PH->GetXaxis()->SetTitle("Amplitude[adc]");
      hAmplitudeCheck_PH->GetYaxis()->SetTitle("counts");
      AmplitudeCheck_PH_map[bankname] = hAmplitudeCheck_PH;

      histname = "hAmplitudeCheck_Gauss_" + detname + "_" + bankname;
      histtitle = "Gaussian Fit Height distribution in " + detname;
      TH1F* hAmplitudeCheck_gaus = new TH1F(histname.c_str(), histtitle.c_str(),
					  max_adc, 0, max_adc);
      hAmplitudeCheck_gaus->GetXaxis()->SetTitle("Fit Amplitude[adc]");
      hAmplitudeCheck_gaus->GetYaxis()->SetTitle("counts");
      AmplitudeCheck_Gauss_map[bankname] = hAmplitudeCheck_gaus;
    }

    gDirectory->Cd("/MidasHists/");
    return SUCCESS;
}

INT MAmplitudeCheck_eor(INT run_number){
  //nothing yet
  return SUCCESS;
}

INT MAmplitudeCheck(EVENT_HEADER *pheader, void *pevent)
{
  map<string, vector<TPulseIsland*> > tpi_map = gData->fPulseIslandToChannelMap;

  for(map<string, vector<TPulseIsland*> >::iterator mIter = tpi_map.begin();
      mIter != tpi_map.end(); mIter++) {
    string bankname = mIter->first;
    if(!gSetup->IsWFD(bankname)) continue;
    int polarity = gSetup->GetTriggerPolarity(bankname);
    vector<TPulseIsland*> Pulses = mIter->second;
    std::string detname = gSetup->GetDetectorName(bankname);

    if(detname != "TSc"
       && detname != "GeCHEH"
       && detname != "GeCHEL"
       && detname != "NdetD"
       && detname != "NdetU"
       && detname != "LaBr3")
      continue;

    for(vector<TPulseIsland*>::iterator pIter = Pulses.begin();
	pIter != Pulses.end(); pIter++) {
      //if(AmplitudeCheck_PH_map.find(bankname) == AmplitudeCheck_PH_map.end())
      //continue;
      AmplitudeCheck_PH_map[bankname]->Fill((*pIter)->GetPulseHeight());

      vector<int> samples = (*pIter)->GetSamples();
      int pedestal = (*pIter)->GetPedestal(8);
      int tpeak = 0, peak = 0;
      //polarity == 1 ? tpeak = std::max_element(samples.begin(), samples.end()) :
      //tpeak = std::min_element(samples.begin(), samples.end());

      if(samples.size() < 8) continue;
      TH1F* hPulse = new TH1F("hPulse", "Waveform", samples.size(),0, samples.size());
      for(vector<int>::iterator sIt = samples.begin(); sIt != samples.end(); sIt ++) {
	int ph = polarity * ((*sIt) - pedestal);
	hPulse->Fill(std::distance(samples.begin(), sIt), ph);
	if(ph > peak){
	  peak = ph;
	  tpeak = std::distance(samples.begin(), sIt);
	}
      }
    
      TF1* fit1 = new TF1("fit1", "gaus", 0, samples.size());
      fit1->SetParameters(peak, tpeak, 2);
      hPulse->Fit("fit1", "Q", "", tpeak - 2, tpeak + 2);
      AmplitudeCheck_Gauss_map[bankname]->Fill(fit1->GetParameter(0));
      delete hPulse;
      delete fit1;
    }

  }

  return SUCCESS;
}
