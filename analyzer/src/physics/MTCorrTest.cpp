/* Standard includes */
#include <cstdio>
#include <cstdlib>
#include <string>
#include <map>
#include <utility>
#include <cmath>

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
  //std::string VETOBANKS[NCRATE][MAXNCHANWFD];
  TH2D* vvhTCorrTest_Align[NCRATE][MAXNCHANWFD];
  TH2D* vvhTCorrTest_CutAlign[NCRATE][MAXNCHANWFD];
  TH2D* vvhTCorrTest_EvTDiff[NCRATE][MAXNCHANWFD];
  TH2D* vvhTCorrTest_IEvTDiff[NCRATE][MAXNCHANWFD];
  const double TIME_LOW = -5e3, TIME_HIGH = 1e4;
  const double ALIGN_LOW = -100, ALIGN_HIGH = 100;
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
      //std::cout << "testing MTCorrTest_init " << bank << std::endl;
      if(det == "NdetD" || det == "NdetU" || det == "LaBr3"){
	WFDBANKS[icrate][ich] = bank;
	TDCBANKS[icrate][ich] = gSetup->GetBankName("T" + det);
	//VETOBANKS[icrate][ich] = "T" + det + "V";
      }
      else if(det == "GeCHEH" || det == "GeCHEL"){
	WFDBANKS[icrate][ich] = bank;
	TDCBANKS[icrate][ich] = gSetup->GetBankName("TGeCHT");
	//VETOBANKS[icrate][ich] = "TGeV";
      }
      else if(det == "TSc") {
	WFDBANKS[icrate][ich] = bank;
	TDCBANKS[icrate][ich] = gSetup->GetBankName("TTSc");
	//VETOBANKS[icrate][ich] = "TVSc";
      }
      else{
	WFDBANKS[icrate][ich] = " ";
	continue;  // ignoring SYNC pulses, Rollover, LYSO, GeCHT, and empty
      }

      int nBits = gSetup->GetNBits(bank);
      int max_adc = std::pow(2, nBits);
      float max_amp_e = MTCorrTest_Energy(det, max_adc);
      float max_int = max_adc * MTCorrTest_nSamples(det); 
      float max_int_e = MTCorrTest_IntEnergy(det, max_int);

      
      //setup histograms
      ////////////////Alignment//////////////////
      char histname[64]; sprintf(histname, "hTCorrTest_Align_%s", det.c_str());
      char histtitle[64]; sprintf(histtitle, "TDC WFD Alignment for %s", det.c_str());
      vvhTCorrTest_Align[icrate][ich] = new TH2D(histname, histtitle, (ALIGN_HIGH-ALIGN_LOW), ALIGN_LOW*2, ALIGN_HIGH*2, max_adc/4, 0, max_amp_e);
      vvhTCorrTest_Align[icrate][ich]->GetXaxis()->SetTitle("Alignment Difference (ns)");
      vvhTCorrTest_Align[icrate][ich]->GetYaxis()->SetTitle("Energy (MeV)");

      /////////////// Cut Alignment //////////////////
      sprintf(histname, "hTCorrTest_CutAlign_%s", det.c_str());
      sprintf(histtitle, "TDC WFD Alignment for %s after cut", det.c_str());
      vvhTCorrTest_CutAlign[icrate][ich] = new TH2D(histname, histtitle, (ALIGN_HIGH-ALIGN_LOW), ALIGN_LOW*2, ALIGN_HIGH*2, max_adc/4, 0, max_amp_e);
      vvhTCorrTest_CutAlign[icrate][ich]->GetXaxis()->SetTitle("Alignment Difference (ns)");
      vvhTCorrTest_CutAlign[icrate][ich]->GetYaxis()->SetTitle("Energy (MeV)");

      ///////////// E vs TDiff  //////////////////////
      sprintf(histname, "hTCorrTest_EvTDiff_%s", det.c_str());
      sprintf(histtitle, "Energy vs TSC TDiff for %s", det.c_str());
      vvhTCorrTest_EvTDiff[icrate][ich] = new TH2D(histname, histtitle, (TIME_HIGH - TIME_LOW)/8, TIME_LOW, TIME_HIGH, max_adc/4, 0, max_amp_e);
      vvhTCorrTest_EvTDiff[icrate][ich]->GetXaxis()->SetTitle("TDiff (TDC) (ns)");
      vvhTCorrTest_EvTDiff[icrate][ich]->GetYaxis()->SetTitle("Energy (amplitude) (MeV)");

      ///////////// IE v TDiff //////////////////////
      sprintf(histname, "hTCorrTest_IEvTDiff_%s", det.c_str());
      sprintf(histtitle, "Energy vs TSC TDiff for %s", det.c_str());
      vvhTCorrTest_IEvTDiff[icrate][ich] = new TH2D(histname, histtitle, (TIME_HIGH - TIME_LOW)/8, TIME_LOW, TIME_HIGH, max_adc/4, 0, max_int_e);
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
    

  //std::cout << "testing MTCorrTest main" << std::endl;
  for(int icrate = 0; icrate < NCRATE; ++icrate){
    for(int ich = 0; ich < NCHANWFD[icrate]; ++ich) {
      if(WFDBANKS[icrate][ich] == " ") continue;
      if(!wfd_map.count(WFDBANKS[icrate][ich])) continue;
      std::string det=gSetup->GetDetectorName(WFDBANKS[icrate][ich]);
      //std::cout << WFDBANKS[icrate][ich] << "   " << det << "   " << TDCBANKS[icrate][ich] << "   " << VETOBANKS[icrate][ich] << std::endl;
      const int polarity = gSetup->GetTriggerPolarity(WFDBANKS[icrate][ich]);
      const int nBits = gSetup->GetNBits(WFDBANKS[icrate][ich]);
      const int max_adc = std::pow(2, nBits);

      const std::vector<TPulseIsland*>& pulses = wfd_map.at(WFDBANKS[icrate][ich]);

      if(!tdc_map.count(TDCBANKS[icrate][ich])){
	printf("MTCorrTest: No hits in channel %s!\n", TDCBANKS[icrate][ich].c_str());
	return SUCCESS;
      }
      const std::vector<int64_t>& times = tdc_map.at(TDCBANKS[icrate][ich]);
      double toff = gData->fTDCSynchronizationPulseOffset[icrate];
      double tcorr = 0, aligncorr = 0;;
      if(det == "GeCHEH" || det == "GeCHEL"){ tcorr = 1000; aligncorr = 200; }
      if(det == "TSc") aligncorr = -60;
      if(det == "LaBr3") aligncorr = 0;

      //two loops, pulse loop
      int a0 = 0, t0 = 0;
      for(int p = 0; p < pulses.size(); ++p){
	//setup variables
	float pedestal = pulses[p]->GetPedestal(8);
	if(det == "TSc") pedestal = pulses[p]->GetPedestal(4);
	const std::vector<int>& samples = pulses[p]->GetSamples();
	const int nSamp = samples.size();
	if(nSamp < 8) continue;

	float integral = 0;
	float integral_ps = 0;
	bool overflow = false;
	float max = 0;
	int tMax = 0;
	//bool aligned = false;
	float pedBegin = 0, pedEnd = 0, pedBeginSamp = 0, pedEndSamp = 0;
	float pedSlope = 0, pedInt = 0, pedMid = 0;
	float tThreshold = 0;
	float threshold = MTCorrTest_Threshold(det);
	float cf = 0, cft_init = 0, cf_init = 0, cft = 0;
	int nMatch = 0;
	float tTDC = 0, tdiff_align = 0;

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

	//find timestamp location
	for(int i = 0; i< tMax; i++) {
	  float tmp = polarity * (samples.at(i) - pedestal);
	  if(tmp < thresh_tmp) {
	    tThreshold = i;
	    break;
	  }
	}

	//GetCF timing
	cf = 0.1*max;
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

	//get integral pedestal
	for(std::vector<int>::const_iterator pulse_begin = samples.begin() + 1; pulse_begin != samples.begin() + 9; pulse_begin++){
	  pedBegin += (*pulse_begin);
	  pedBeginSamp += 1;
	}

	for(std::vector<int>::const_iterator pulse_end = samples.end() - 9; pulse_end != samples.end(); pulse_end++){
	  pedEnd += (*pulse_end);
	  pedEndSamp += 1;
	}

	pedBegin /= pedBeginSamp;
	pedEnd /= pedEndSamp;
	pedSlope = (pedEnd - pedBegin)/(float)nSamp;

	//integrate the pedestal
	pedMid = pedSlope * ((float)nSamp/2) + pedBegin;
	if(det == "TSc"){
	  pedBegin = (samples.at(0) + samples.at(1) + samples.at(2) + samples.at(3))/4;
	  pedMid = pedBegin; pedEnd = pedBegin;   
	}
	pedInt = (float)(nSamp-1)/6 * (pedBegin + (4*pedMid) + pedEnd);

	//integrate wave
	int nInt = 0;
	int lastSamp = 0;
	for(int i = 0; i+2 < nSamp; i += 2){
	  integral += (samples.at(i) + (4*samples.at(i+1)) + samples.at(i+2))/3;
	  nInt += 2;
	  lastSamp = i+2;
	}
	if(nSamp % 2 == 0){
	  integral += 0.5 * (samples.at(lastSamp) + samples.at(lastSamp + 1));
	  nInt ++;
	  lastSamp++;
	}
	if(nSamp % 2 == 1){
	  std::cout << "Non-modulo 2 check" << std:: endl;
	  integral += 0.5 * (samples.at(nSamp-1) + samples.at(nSamp));
	  nInt++;
	}
	integral_ps = polarity * (integral - pedInt);

	//max = amp, cft = time, integral_ps = int
	float energy_amp = MTCorrTest_Energy(det, max);
	float energy_int = MTCorrTest_IntEnergy(det, integral_ps);
	double CFT = (pulses[p]->GetTimeStamp() - tThreshold + cft) * TICKWFD[icrate];

	//a0 set to zero before pulse loop
	// alignment loop

	for(int a = a0; a<times.size(); ++a){
	  double dt_cft = TICKTDC*times[a] - CFT - toff+tcorr;

	  if(dt_cft > 2*(ALIGN_LOW - aligncorr)) vvhTCorrTest_Align[icrate][ich]->Fill(dt_cft, energy_amp);

	  if(dt_cft < ALIGN_LOW-aligncorr) { a0++; continue; }
	  else if(dt_cft < ALIGN_HIGH + aligncorr) {
	    nMatch++;
	    tdiff_align = dt_cft;
	    tTDC = TICKTDC*times[a];
	  }
	  else {
	    if(a0 > 1) a0--;//just in case we passed the next pulse
	    
	    break;
	  }
	}
	//back in pulse loop
	//check for pulse discard
	if(nMatch != 1) continue;
	vvhTCorrTest_CutAlign[icrate][ich]->Fill(tdiff_align, energy_amp);

	//loop over TTSc times
	for(int t = t0; t<ref_times.size(); ++t){
	  double dt = tTDC - TICKTDC*ref_times[t];

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

