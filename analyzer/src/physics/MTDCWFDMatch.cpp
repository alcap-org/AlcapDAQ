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
float MTDCWFDMatch_calc_cft(std::vector<int>, float, float, int);
float MTDCWFDMatch_calc_threshold(std::vector<int>, int, float, int);

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
  const double TIME_LOW = -5e4, TIME_HIGH = 3e5;
  const double ALIGN_LOW = -100, ALIGN_HIGH = 100;
  const double VETO_LOW = -80, VETO_HIGH = 80;
  const double PILEUP_LOW = -1e4, PILEUP_HIGH=1e4;
  const double DOUBLE_LOW = -1e3, DOUBLE_HIGH=1e3;
  INT plotCount = 0, plotCountLow = 0;

}


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
      else if(det == "GeCHEH" || det == "GeCHEL" || det == "GeCHT"){
	WFDBANKS[icrate][ich] = bank;
	//VETOBANKS[icrate][ich] = gSetup->GetBankName("TGeCHT");
	//TDCBANKS[icrate][ich] = gSetup->GetBankName("TGeV");
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
      vvhTDCWFDMatch_Align[icrate][ich] = new TH2D(histname, histtitle, (ALIGN_HIGH-ALIGN_LOW)/10, ALIGN_LOW*2, ALIGN_HIGH*2, max_adc/4, 0, max_adc);
      vvhTDCWFDMatch_Align[icrate][ich]->GetXaxis()->SetTitle("Alignment Difference (ns)");
      vvhTDCWFDMatch_Align[icrate][ich]->GetYaxis()->SetTitle("Energy (MeV)");

      /////////////// Cut Alignment //////////////////
      sprintf(histname, "hTCorrTest_CutAlign_%s", det.c_str());
      sprintf(histtitle, "TDC WFD Alignment for %s after cut", det.c_str());
      vvhTDCWFDMatch_CutAlign[icrate][ich] = new TH2D(histname, histtitle, (ALIGN_HIGH-ALIGN_LOW)/10, ALIGN_LOW, ALIGN_HIGH, max_adc/4, 0, max_adc);
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

      double tcorr = 0, aligncorr = 0, vcorr = 0, vRange = 0;
      //tcorr corrects spectrum to place value near zero in correlation
      //aligncorr is a correction to the align range size, mostly for Ge
      //vcorr is the time difference between veto and channel in MTTScTcorrTDC
      if(det == "GeCHEH" || det == "GeCHEL"){ tcorr = 800; aligncorr = 1000; vcorr = 127;}
      //if(det == "GeCHEH" || det == "GeCHEL"){ tcorr = -1600; aligncorr = 1000; vcorr = 127;}
      if(det == "TSc") {aligncorr = -70; tcorr = -70; vcorr = -10; vRange = 9920; };
      if(det == "LaBr3") vcorr = 7;
      if(det == "NdetD") {vcorr = 52; tcorr = -60;}
      if(det == "NdetU") {vcorr = 40; tcorr = -60;}
      if(det == "GeCHT") {vcorr = 40; tcorr = -60;}


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
	int tMax = 0, max_r = 0;


	//plot waveform for CFT
	for(int i = 0; i< nSamp; i++){
	  float adc = samples.at(i);
	  if(adc == 0 || adc >= max_adc-1){ overflow = true; break; }
	  float tmp = polarity*(adc - pedestal);
	  if(tmp > max){
	    max = tmp; tMax = i;
	  }
	}
	if(overflow){ /*delete hpulse; continue;*/ }

	float threshold = MTDCWFDMatch_Threshold(det);
	float threshold_ps = polarity * (threshold - pedestal);
	float tThreshold = MTDCWFDMatch_calc_threshold(samples, polarity, threshold, max_r);



	float cft = MTDCWFDMatch_calc_cft(samples, pedestal, max, polarity);
      

	//max = amp, cft = time, integral_ps = int
	double CFT = (pulses[p]->GetTimeStamp() - tThreshold + cft) * TICKWFD[icrate];


	//a0 set to zero before pulse loop


	//////////////// alignment loop ////////////////////////////////

	int64_t a1 = 0;
	int nMatch = 0;
	bool plot = false, plotLow = false;

	//if(det == "TSc" && pulses[p]->GetPileupPulse()) continue;

	if(det == "GeCHEH" || det == "GeCHEL") // correlate to GeCHT?
	  ;

	float prior = -1.5e5;
	float tdiff_align = -1.5e5;

        pulses[p]->SetWFDTime(CFT + toff);

	for(int a = a0; a<times.size(); ++a){
	  double dt_cft = (CFT + toff - tcorr) - TICKTDC*times[a];
	  if(dt_cft > 10*(ALIGN_HIGH + aligncorr) && a >= 1) a0 = a-1;

	  if(dt_cft < 2*(ALIGN_HIGH + aligncorr) && abs(dt_cft) < abs(prior)){
	    vvhTDCWFDMatch_Align[icrate][ich]->Fill(dt_cft, max);
	    prior = dt_cft;
	    plot = true;
	  }

	  if(dt_cft < 5*(ALIGN_LOW-aligncorr) ) { /*a0++;*/ break; }
	  else if(dt_cft < ALIGN_HIGH + aligncorr) {
	    if(abs(dt_cft) <= abs(prior)) {
	      prior = dt_cft;
	      tdiff_align = dt_cft;
	      pulses[p]->SetTDCTime(times[a]);
	      pulses[p]->SetWFDTime(CFT + toff);
	      a1 = times[a];
	    }
	    nMatch++;
	    if(det == "TSc"){  	    //check this pulse for pileup
	      if(p+1 < pulses.size() ) {
		float pileupTime = TICKWFD[icrate] * (pulses[p+1]->GetTimeStamp() - pulses[p]->GetTimeStamp() );
		if(PILEUP_LOW < pileupTime && PILEUP_HIGH > pileupTime){
		  if(pulses[p]->GetPileupPulse() != true)
		    pulses[p]->SetPileupPulse(true);
		  pulses[p+1]->SetPileupPulse(true);
		}
	      }
	    
	      if(p-1 >= 0){
		float pileupTime = TICKWFD[icrate] * (pulses[p]->GetTimeStamp() - pulses[p-1]->GetTimeStamp() );
		if(PILEUP_LOW < pileupTime && PILEUP_HIGH > pileupTime){
		  pulses[p]->SetPileupPulse(true);
		  pulses[p-1]->SetPileupPulse(true);
		}
	      }
	    }   // end pileup checks, TSc specific
	  }  //end aligned pulse

	} // end alignment scan

	//back in pulse loop
	//check for pulse discard
	
	if(nMatch >= 2 && det != "TSc"){
	  pulses[p]->SetDoublePulse(true);
	  //continue;
	}

	//if(det == "GeCHEH" && tdiff_align > -500  && tdiff_align < -200) plot = true;
	//if(det == "GeCHEH" && tdiff_align > -1200  && tdiff_align < -1000) plotLow = true;

	for(int v = 0; v<vetos.size(); ++v){
	  double dt = TICKTDC*(a1 - vetos[v]) - vcorr; // a1 = pulse TDC time

	  if(dt < VETO_LOW + vcorr - vRange) break;
	  else if(dt < VETO_HIGH + vcorr + vRange){
	    pulses[p]->SetVetoPulse(true);
	    if(det == "TSc") pulses[p]->SetPileupPulse(true);
	    break;
	  }

	}


	if(tdiff_align < -1e5) continue;

	//if(pulses[p]->GetVetoPulse()) continue;
	//if(pulses[p]->GetPileupPulse() ) continue;
	vvhTDCWFDMatch_CutAlign[icrate][ich]->Fill(tdiff_align, max);

      }
    }
  }



  return SUCCESS;
} 
  


float MTDCWFDMatch_nSamples(std::string detname){
  float nSamples = 0;
  if(detname == "TSc") nSamples = 5;//12
  else if(detname == "NdetD" || detname == "NdetU") nSamples = 7.5;//150
  else if(detname == "GeCHEH" || detname == "GeCHEL") nSamples = 80;//300
  else if(detname == "GeCHT") nSamples = 7.5;//150
  else if(detname == "LaBr3") nSamples = 24;//128
  else nSamples = 100;
  return nSamples;
}

float MTDCWFDMatch_Threshold(std::string detname){
  float thresh = 0;
  if(detname == "GeCHEH") thresh = 1500; //original
  //if(detname == "GeCHEH") thresh = 500; //test
  else if(detname == "GeCHEL") thresh = 1048;
  else if(detname == "GeCHT") thresh = 15150;
  else if(detname == "NdetD") thresh = 15460;
  else if(detname == "NdetU") thresh = 15510;
  else if(detname == "LaBr3") thresh = 15645;
  else if(detname == "TSc") thresh = 14800;
  else thresh = 0;
  return thresh;
}

float MTDCWFDMatch_calc_cft(std::vector<int> samples, float pedestal, float max, int polarity){
  float cf = 0.2*max;

  float cft = 0, cft_init = 0, cf_init = 0;
  uint nSamp = samples.size();
  for(uint i = 0; i < nSamp; i++){
    double tmp = polarity * (samples.at(i) - pedestal);
    if(tmp > cf){
      cft_init = i;
      cf_init = tmp;
      break;
    }
  }

  if(cft_init < 1) return 0;
  double x1 = cft_init-1, x2 = cft_init;
  double y1 = polarity * (samples.at(x1) - pedestal), y2 = cf_init;
  cft = ( (cf - y1)*(x2 - x1)/(y2 - y1) ) + x1;  //linear interpolation
  return cft;
}

float MTDCWFDMatch_calc_threshold(std::vector<int> samples, int polarity, float threshold, int max_raw){

  //find timestamp location, does this affect all detectors?

  uint nSamp = samples.size();
  int tThreshTmp = 0;

  if(max_raw < threshold && polarity == 1) return 0;
  if(max_raw > threshold && polarity == -1) return 0;

  for(uint i = 0; i< nSamp; ++i) {
    float samp = samples.at(i);
    if(samp > threshold && polarity == 1) {
      tThreshTmp = i;
      break;
    }
    if(samp < threshold && polarity == -1) {
      tThreshTmp = i;
      break;
    }
  }

  if(tThreshTmp < 1) return 0;
  //linear interpolation
  float x1 = tThreshTmp-1, x2 = tThreshTmp;
  float y1 = samples.at((int)x1), y2 = samples.at((int)x2);
  float tThreshold =  x1 + ( (threshold - y1)*(x2 - x1)/(y2 - y1) );  
  //tThreshold = 0;

  return tThreshold;

}

