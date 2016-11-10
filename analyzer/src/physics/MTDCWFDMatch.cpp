/* Standard includes */
#include <cstdio>
#include <cstdlib>
#include <string>
#include <map>
#include <utility>
#include <cmath>
#include <sstream>

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
static INT MTDCWFDMatch_init(void);
static INT MTDCWFDMatch(EVENT_HEADER*, void*);
float MTDCWFDMatch_Threshold(std::string);

extern HNDLE hDB;
extern TGlobalData* gData;
extern TSetupData* gSetup;


using namespace AlCap;
namespace {
  std::string WFDBANKS[NCRATE][MAXNCHANWFD];
  std::string TDCBANKS[NCRATE][MAXNCHANWFD];
  std::string VETOBANKS[NCRATE][MAXNCHANWFD];
  TH2D* vvhTDCWFDMatch_Align[NCRATE][MAXNCHANWFD];
  TH2D* vvhTDCWFDMatch_CutAlign[NCRATE][MAXNCHANWFD];
  const double TIME_LOW = -5e3, TIME_HIGH = 1e4;
  const double ALIGN_LOW = -100, ALIGN_HIGH = 100;
  const double VETO_LOW = -80, VETO_HIGH = 80;
  const double PILEUP_LOW = -1e4, PILEUP_HIGH=1e4;
  const double DOUBLE_LOW = -1e3, DOUBLE_HIGH=1e3;
}

INT plotCount = 0;

ANA_MODULE MTDCWFDMatch_module =
{
  "MTDCWFDMatch",        /* module name           */
  "Damien Alexander",  /* author                */
  MTDCWFDMatch,          /* event routine         */
  NULL,                /* BOR routine           */
  NULL,                /* EOR routine           */
  MTDCWFDMatch_init,     /* init routine          */
  NULL,                /* exit routine          */
  NULL,                /* parameter structure   */
  0,                   /* structure size        */
  NULL,                /* initial parameters    */
};


INT MTDCWFDMatch_init() {
  TDirectory* cwd = gDirectory;
  gDirectory->mkdir("TDCWFDMatch")->cd();
  //select channels and corresponding vetos
  for(int icrate = 0; icrate < NCRATE; ++icrate){
  
    for(int ich = 0; ich < NCHANWFD[icrate]; ++ich) {
      
      char bank[5]; sprintf(bank, "D%d%02d", icrate, ich);
      std::string det = gSetup->GetDetectorName(bank);
      //std::cout << "testing MTCorrTest_init " << bank << std::endl;
      if(det == "NdetD" || det == "NdetU"/* || det == "LaBr3"*/){
	WFDBANKS[icrate][ich] = bank;
	TDCBANKS[icrate][ich] = gSetup->GetBankName("T" + det);
	VETOBANKS[icrate][ich] = gSetup->GetBankName("T" + det + "V");
      }
      else if(det == "GeCHEH"/* || det == "GeCHEL"*/){
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
	WFDBANKS[icrate][ich] = " ";
	continue;  // ignoring SYNC pulses, Rollover, LYSO, GeCHT, and empty
      }

      int nBits = gSetup->GetNBits(bank);
      int max_adc = std::pow(2, nBits);

      //setup histograms
      ////////////////Alignment//////////////////
      char histname[64]; sprintf(histname, "hTCorrTest_Align_%s", det.c_str());
      char histtitle[64]; sprintf(histtitle, "TDC WFD Alignment for %s", det.c_str());
      vvhTDCWFDMatch_Align[icrate][ich] = new TH2D(histname, histtitle, 10*(ALIGN_HIGH-ALIGN_LOW), ALIGN_LOW*50, ALIGN_HIGH*50, max_adc/4, 0, max_adc);
      vvhTDCWFDMatch_Align[icrate][ich]->GetXaxis()->SetTitle("Alignment Difference (ns)");
      vvhTDCWFDMatch_Align[icrate][ich]->GetYaxis()->SetTitle("Energy (MeV)");

      /////////////// Cut Alignment //////////////////
      sprintf(histname, "hTCorrTest_CutAlign_%s", det.c_str());
      sprintf(histtitle, "TDC WFD Alignment for %s after cut", det.c_str());
      vvhTDCWFDMatch_CutAlign[icrate][ich] = new TH2D(histname, histtitle, 5*(ALIGN_HIGH-ALIGN_LOW), ALIGN_LOW*10, ALIGN_HIGH*10, max_adc/4, 0, max_adc);
      vvhTDCWFDMatch_CutAlign[icrate][ich]->GetXaxis()->SetTitle("Alignment Difference (ns)");
      vvhTDCWFDMatch_CutAlign[icrate][ich]->GetYaxis()->SetTitle("Energy (MeV)");

    }
  
  }
  cwd->cd();
  return SUCCESS;

}

INT MTDCWFDMatch(EVENT_HEADER *pheader, void *pevent) {
  const map< string, vector<TPulseIsland*> >& wfd_map = gData->fPulseIslandToChannelMap;
  const map<string, vector<int64_t> >& tdc_map = gData->fTDCHitsToChannelMap;
      

  for(int icrate = 0; icrate < NCRATE; ++icrate){
    for(int ich = 0; ich < NCHANWFD[icrate]; ++ich) {
      if(WFDBANKS[icrate][ich] == " ") continue;
      std::string det=gSetup->GetDetectorName(WFDBANKS[icrate][ich]);

      const int polarity = gSetup->GetTriggerPolarity(WFDBANKS[icrate][ich]);
      const int nBits = gSetup->GetNBits(WFDBANKS[icrate][ich]);
      const int max_adc = std::pow(2, nBits);

      if(!wfd_map.count(WFDBANKS[icrate][ich])){
	printf("MTDCWFDMatch: No hits in channel %s!\n", WFDBANKS[icrate][ich].c_str());
	return SUCCESS;
      }
      const std::vector<TPulseIsland*>& pulses = wfd_map.at(WFDBANKS[icrate][ich]);

      if(!tdc_map.count(TDCBANKS[icrate][ich])){
	printf("MTDCWFDMatch: No hits in channel %s!\n", TDCBANKS[icrate][ich].c_str());
	return SUCCESS;
      }
      const std::vector<int64_t>& times = tdc_map.at(TDCBANKS[icrate][ich]);

      if(!tdc_map.count(VETOBANKS[icrate][ich])){
	printf("MTDCWFDMatch: No hits in channel %s!\n", VETOBANKS[icrate][ich].c_str());
	return SUCCESS;
      }
      const std::vector<int64_t>& vetos = tdc_map.at(VETOBANKS[icrate][ich]);


      double toff = gData->fTDCSynchronizationPulseOffset[icrate];
      //toff is a standard alignment parameter for the crate.
      //individual channels still need tweaking

      double tcorr = 0, aligncorr = 0, vcorr = 0;
      //tcorr corrects spectrum to place value near zero in correlation
      //aligncorr is a correction to the align range size, mostly for Ge
      //vcorr is the time difference between veto and channel in MTTScTcorrTDC
      if(det == "GeCHEH" || det == "GeCHEL"){ tcorr = 500; aligncorr = 600; vcorr = 127;}
      if(det == "TSc") {aligncorr = -60; tcorr = -50; vcorr = -10;};
      if(det == "LaBr3") vcorr = 7;
      if(det == "NdetD") vcorr = 52;
      if(det == "NdetU") vcorr = 40;

      int trigCount = 0;


      //two loops, pulse loop
      int a0 = 0;
      for(int p = 0; p < pulses.size(); ++p){
	//setup variables
	float pedestal = pulses[p]->GetPedestal(8);
	if(det == "TSc") pedestal = pulses[p]->GetPedestal(4);
	const std::vector<int>& samples = pulses[p]->GetSamples();
	const int nSamp = samples.size();
	if(nSamp < 8) continue;

	bool overflow = false;
	float max = 0;
	int tMax = 0;
	//bool aligned = false;
	float tThreshold = 0;
	float threshold = MTDCWFDMatch_Threshold(det);
	float cf = 0, cft_init = 0, cf_init = 0, cft = 0;
	float tdiff_align = -15000;

	//plot waveform for CFT
	TH1I* hpulse = new TH1I("hpulse", "Waveform", nSamp, 0, nSamp);
	for(int i = 0; i< nSamp; i++){
	  float adc = samples.at(i);
	  if(adc == 0 || adc >= max_adc-1){ overflow = true; break; }
	  hpulse->SetBinContent(i, samples.at(i));
	  float tmp = polarity*(samples.at(i) - pedestal);
	  if(tmp > max){
	    max = tmp; tMax = i;
	  }
	}
	if(overflow){ delete hpulse; continue; }

	float thresh_tmp = polarity * (threshold - pedestal);
	if(max < thresh_tmp) {delete hpulse; continue; }

	//find timestamp location, does this affect all detectors?
	for(int i = 0; i< tMax; i++) {
	  float tmp = polarity * (samples.at(i) - pedestal);
	  if(tmp < thresh_tmp) {
	    tThreshold = i;
	    break;
	  }
	}

	////////////////GetCF timing/////////////////////////////////
	cf = 0.02*max;
	for(int i = tMax; i > 0; i--){
	  double tmp = polarity * (samples.at(i) - pedestal);
	  if(tmp < cf){
	    cft_init = i; cf_init = tmp; break;
	  }
	}

	double x1 = cft_init, x2 = cft_init + 1;
	double y1 = cf_init, y2 = polarity * (samples.at(x2) - pedestal);
	cft = ( (cf - y1)*(x2 - x1)/(y2 - y1) ) + x1;  //linear interpolation
      
	delete hpulse;


	//max = amp, cft = time, integral_ps = int
	double CFT = (pulses[p]->GetTimeStamp() - tThreshold + cft) * TICKWFD[icrate];
	//if(det == "GeCHEH" || det == "GeCHEL") CFT += tThreshold * TICKWFD[icrate]; 

	//std::cout << "Times : CFT " << CFT << " timestamp " << pulses[p]->GetTimeStamp() * TICKWFD[icrate] << std::endl;
	//if(p+1 < pulses.size()) std::cout << "TDiff : " << pulses[p+1]->GetTimeStamp() - pulses[p]->GetTimeStamp() << std::endl;

	//a0 set to zero before pulse loop


	//////////////// alignment loop ////////////////////////////////

	int64_t a1 = 0;
	int nMatch = 0;
	bool plot = false;
	double trigTimes[6] = {0, 0, 0, 0, 0, 0};

	if(det == "TSc" && pulses[p]->GetPileupPulse()) continue;

	for(int a = a0; a<times.size(); ++a){
	  double dt_cft = TICKTDC*times[a] - CFT - toff+tcorr;

	  if(dt_cft > 2*(ALIGN_LOW - aligncorr)){
	    vvhTDCWFDMatch_Align[icrate][ich]->Fill(dt_cft, max);
	    //plot = true;
	    //trigTimes[trigCount] = times[a];
	    //trigCount++;
	  }

	  if(dt_cft < ALIGN_LOW-aligncorr) { a0++; continue; }
	  else if(dt_cft < ALIGN_HIGH + aligncorr) {
	    if(nMatch == 0) {
	      tdiff_align = dt_cft;
	      pulses[p]->SetTDCTime(times[a]);
	      a1 = times[a];
	    }
	    //if(nMatch >= 1) plot = true;
	    nMatch++;
	    if(det == "TSc"){  	    //check this pulse for pileup
	      if(p-1 <= 0) continue;
	      double pileupTime = TICKWFD[icrate] * (pulses[p]->GetTimeStamp() - pulses[p-1]->GetTimeStamp() );
	      if(PILEUP_LOW < pileupTime && PILEUP_HIGH > pileupTime){
		pulses[p]->SetPileupPulse(true);
		pulses[p-1]->SetPileupPulse(true);
		if(nMatch == 1) trigCount++;
		//plot = true;
	      }
	      if(p+1 >= pulses.size() ) continue;
	      pileupTime = TICKWFD[icrate] * (pulses[p+1]->GetTimeStamp() - pulses[p]->GetTimeStamp() );
	      if(PILEUP_LOW < pileupTime && PILEUP_HIGH > pileupTime){
		if(pulses[p]->GetPileupPulse() != true)
		  pulses[p]->SetPileupPulse(true);
		pulses[p+1]->SetPileupPulse(true);
		//std::cout << "Pileup pulse : " << pileupTime << " " << pulses[p+1]->GetTimeStamp() << " " << pulses[p]->GetTimeStamp() << std::endl;
		if(nMatch == 1) trigCount++;
		//plot = true;
	      }
	    }
	    /*
	    else{  	    //check this pulse for pileup
	      if(p+1 >= pulses.size()) continue;
	      double pileupTime = TICKWFD[icrate] * (pulses[p+1]->GetTimeStamp() - pulses[p]->GetTimeStamp() );
	      if(DOUBLE_LOW < pileupTime && DOUBLE_HIGH > pileupTime){
		pulses[p]->SetPileupPulse(true);
		pulses[p+1]->SetPileupPulse(true);
		//std::cout << "Pileup pulse : " << pileupTime << " " << pulses[p+1]->GetTimeStamp() << " " << pulses[p]->GetTimeStamp() << std::endl;
		if(nMatch == 1) trigCount++;
		plot = true;
	      }
	      //if(PILEUP_HIGH > TICKTDC * (times[a+1] - times[a]))
	      //pulses[p]->SetPileupPulse(true);
	      }*/
	  }
	  else {
	    if(a0 > 1) a0--;//just in case we passed the next pulse
	    
	    break;
	  }
	}
	//back in pulse loop
	//check for pulse discard
	
	if(nMatch >= 2 && det != "TSc"){
	  pulses[p]->SetDoublePulse(true);
	  continue;
	}
	
	if(det == "GeCHEL" && tdiff_align > 100  && tdiff_align < 300) plot = true;

	for(int v = 0; v<vetos.size(); ++v){
	  double dt = TICKTDC*(a1 - vetos[v]) - vcorr; // a1 = pulse TDC time

	  if(dt < VETO_LOW) break;
	  else if(dt < VETO_HIGH){
	    pulses[p]->SetVetoPulse(true);
	    break;
	  }

	}
	
	if(plot == true && det == "GeCHEL" && plotCount < 20 && max > 4000){
	  plotCount++;
	  //if further needed, plot waveforms
	  std::stringstream ss;
	  ss << plotCount;
	  std::string histname = "TDCWFDMatch_" + ss.str();
	  std::string histtitle = "TSc odd time waveform " + ss.str();
	  TH1F* hIPulse = new TH1F(histname.c_str(), histtitle.c_str(), nSamp, 0, nSamp);
	  for(int l = 0; l< nSamp; l++)
	    hIPulse->Fill(l, samples.at(l));

	}
	

	if(pulses[p]->GetVetoPulse()) continue;
	//if(pulses[p]->GetPileupPulse() ) continue;
	vvhTDCWFDMatch_CutAlign[icrate][ich]->Fill(tdiff_align, max);

      }
      //std::cout << trigCount << std::endl;
    }
  }



  return SUCCESS;
} 
  


float MTDCWFDMatch_nSamples(std::string detname){
  float nSamples = 0;
  if(detname == "TSc") nSamples = 5;//12
  else if(detname == "NdetD" || detname == "NdetU") nSamples = 7.5;//150
  else if(detname == "GeCHEH" || detname == "GeCHEL") nSamples = 80;//300
  else if(detname == "LaBr3") nSamples = 24;//128
  else nSamples = 100;
  return nSamples;
}

float MTDCWFDMatch_Threshold(std::string detname){
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

