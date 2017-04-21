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
static INT MTDCWFDMatchEff2_init(void);
static INT MTDCWFDMatchEff2(EVENT_HEADER*, void*);
INT MTDCWFDMatchEff2_eor(int);
float MTDCWFDMatchEff2_Threshold(std::string);
int MTDCWFDMatchEff2_nSamples(std::string);


extern HNDLE hDB;
extern TGlobalData* gData;
extern TSetupData* gSetup;


using namespace AlCap;
namespace {
  std::string WFDBANKS[NCRATE][MAXNCHANWFD];
  std::string TDCBANKS[NCRATE][MAXNCHANWFD];
  //std::string VETOBANKS[NCRATE][MAXNCHANWFD];
  //TH2D* vvhTDCWFDMatchEff_TDCvWFD[NCRATE][MAXNCHANWFD];
  TH1D* vhTDCWFDMatchEff_MatchRate[NCRATE][MAXNCHANWFD];
  TH1D* vhTDCWFDMatchEff_TriggerlessRate[NCRATE][MAXNCHANWFD];
  TH1D* vhTDCWFDMatchEff_OverflowRate[NCRATE][MAXNCHANWFD];
  TH1D* vhTDCWFDMatchEff_PileupRate[NCRATE][MAXNCHANWFD];
  TH1D* vhTDCWFDMatchEff_VetoRate[NCRATE][MAXNCHANWFD];
  TH1D* vhTDCWFDMatchEff_MatchedVetoRate[NCRATE][MAXNCHANWFD];
  TH1D* vhTDCWFDMatchEff_AcceptRate[NCRATE][MAXNCHANWFD];
  TH1D* vhTDCWFDMatchEff_ValidMatchRate[NCRATE][MAXNCHANWFD];
  const double TIME_LOW = -5e3, TIME_HIGH = 1e4;
  const double ALIGN_LOW = -500, ALIGN_HIGH = 500;
  float total[NCRATE][MAXNCHANWFD], lowMax[NCRATE][MAXNCHANWFD];
  float overflow[NCRATE][MAXNCHANWFD], pileup[NCRATE][MAXNCHANWFD];
  float veto[NCRATE][MAXNCHANWFD], matchedVeto[NCRATE][MAXNCHANWFD];
  float matchedTDC[NCRATE][MAXNCHANWFD], matchedWFD[NCRATE][MAXNCHANWFD];
  float accept[NCRATE][MAXNCHANWFD], validTot[NCRATE][MAXNCHANWFD];
}


ANA_MODULE MTDCWFDMatchEff2_module =
{
  "MTDCWFDMatchEff2",        /* module name           */
  "Damien Alexander",  /* author                */
  MTDCWFDMatchEff2,          /* event routine         */
  NULL,                /* BOR routine           */
  MTDCWFDMatchEff2_eor,      /* EOR routine           */
  MTDCWFDMatchEff2_init,     /* init routine          */
  NULL,                /* exit routine          */
  NULL,                /* parameter structure   */
  0,                   /* structure size        */
  NULL,                /* initial parameters    */
};


INT MTDCWFDMatchEff2_init() {
  TDirectory* cwd = gDirectory;
  gDirectory->mkdir("TDCWFDMatchEff2")->cd();
  //select channels and corresponding vetos
  for(int icrate = 0; icrate < NCRATE; ++icrate){
  
    for(int ich = 0; ich < NCHANWFD[icrate]; ++ich) {

      lowMax[icrate][ich] = 0;
      veto[icrate][ich] = 0;
      total[icrate][ich] = 0;
      pileup[icrate][ich] = 0;
      overflow[icrate][ich] = 0;
      matchedWFD[icrate][ich] = 0;
      matchedTDC[icrate][ich] = 0;
      matchedVeto[icrate][ich] = 0;
      accept[icrate][ich] = 0;
      validTot[icrate][ich] = 0;
      
      char bank[5]; sprintf(bank, "D%d%02d", icrate, ich);
      std::string det = gSetup->GetDetectorName(bank);

      if(det == "NdetD" || det == "NdetU" /*|| det == "LaBr3"*/){
	WFDBANKS[icrate][ich] = bank;
	TDCBANKS[icrate][ich] = gSetup->GetBankName("T" + det);
      }
 
      else if(det == "GeCHEH" || det == "GeCHEL" || det == "GeCHT"){
	WFDBANKS[icrate][ich] = bank;
	TDCBANKS[icrate][ich] = gSetup->GetBankName("TGeCHT");
      }
      else if(det == "TSc") {
	WFDBANKS[icrate][ich] = bank;
	TDCBANKS[icrate][ich] = gSetup->GetBankName("TTSc");
      }
      else{
	WFDBANKS[icrate][ich] = " ";
	continue;  // ignoring SYNC pulses, Rollover, LYSO, GeCHT, and empty
      }

      int nBits = gSetup->GetNBits(bank);
      int max_adc = std::pow(2, nBits);
      int n_Samp = MTDCWFDMatchEff2_nSamples(det);

      //setup histograms
      char histname[64];
      char histtitle[64]; 
 
      /*
      /////////////// TDC v WFD  //////////////////
      sprintf(histname, "hTDCWFDMatchTest_TDCvWFD_%s", det.c_str());
      sprintf(histtitle, "TDC time vs WFD time for %s", det.c_str());
      vvhTDCWFDMatch_TDCvWFD[icrate][ich] = new TH2D(histname, histtitle, 5000, 0, 2e8, 5000, 0, 2e8);
      vvhTDCWFDMatch_TDCvWFD[icrate][ich]->GetXaxis()->SetTitle("TDC Time (ns)");
      vvhTDCWFDMatch_TDCvWFD[icrate][ich]->GetYaxis()->SetTitle("WFD Time (ns)");
      */

      /////////////////// Match Rate ////////////////////
      sprintf(histname, "hTDCWFDMatchEff_MatchRate_%s", det.c_str());
      sprintf(histtitle, "Match Rate (WFD) for %s", det.c_str());
      vhTDCWFDMatchEff_MatchRate[icrate][ich] = new TH1D(histname, histtitle, 2000, 5500, 7500);
      vhTDCWFDMatchEff_MatchRate[icrate][ich]->GetYaxis()->SetTitle("nMatch/nTot");

      ////////////// Triggerless rate (high ped or pulse too low) //////////
      sprintf(histname, "hTDCWFDMatchEff_TriggerlessRate_%s", det.c_str());
      sprintf(histtitle, "Triggerless Pulse Rate for %s", det.c_str());
      vhTDCWFDMatchEff_TriggerlessRate[icrate][ich] = new TH1D(histname, histtitle, 2000, 5500, 7500);
      vhTDCWFDMatchEff_TriggerlessRate[icrate][ich]->GetYaxis()->SetTitle("nTriggerless/nTot");

      ////////////// Overflow Ratio ////////////////
      sprintf(histname, "hTDCWFDMatchEff_OverflowRate_%s", det.c_str());
      sprintf(histtitle, "Ratio of overflowed pulses for %s", det.c_str());
      vhTDCWFDMatchEff_OverflowRate[icrate][ich] = new TH1D(histname, histtitle, 2000, 5500, 7500);
      vhTDCWFDMatchEff_OverflowRate[icrate][ich]->GetYaxis()->SetTitle("ratio (highped/total)");

      ////////////// Pileup Ratio ////////////////
      sprintf(histname, "hTDCWFDMatchEff_PileupRate_%s", det.c_str());
      sprintf(histtitle, "Ratio of pileup protected pulses for %s", det.c_str());
      vhTDCWFDMatchEff_PileupRate[icrate][ich] = new TH1D(histname, histtitle, 2000, 5500, 7500);
      vhTDCWFDMatchEff_PileupRate[icrate][ich]->GetYaxis()->SetTitle("ratio (nPileup/nTotal)");

      ////////////// Veto Ratio ////////////////
      sprintf(histname, "hTDCWFDMatchEff_VetoRate_%s", det.c_str());
      sprintf(histtitle, "Ratio of vetoed pulses for %s", det.c_str());
      vhTDCWFDMatchEff_VetoRate[icrate][ich] = new TH1D(histname, histtitle, 2000, 5500, 7500);
      vhTDCWFDMatchEff_VetoRate[icrate][ich]->GetYaxis()->SetTitle("ratio (nVeto/nTotal)");

      ////////////// Matched Veto Ratio ////////////////
      sprintf(histname, "hTDCWFDMatchEff_MatchedVetoRate_%s", det.c_str());
      sprintf(histtitle, "Ratio of vetoed (matched) pulses for %s", det.c_str());
      vhTDCWFDMatchEff_MatchedVetoRate[icrate][ich] = new TH1D(histname, histtitle, 2000, 5500, 7500);
      vhTDCWFDMatchEff_MatchedVetoRate[icrate][ich]->GetYaxis()->SetTitle("ratio (nVeto|nMatch/nTotal)");

      
      ////////////// Accepted Ratio ////////////////
      sprintf(histname, "hTDCWFDMatchEff_AcceptRate_%s", det.c_str());
      sprintf(histtitle, "Ratio of Accepted pulses for %s", det.c_str());
      vhTDCWFDMatchEff_AcceptRate[icrate][ich] = new TH1D(histname, histtitle, 2000, 5500, 7500);
      vhTDCWFDMatchEff_AcceptRate[icrate][ich]->GetYaxis()->SetTitle("ratio (nVeto|nMatch/nTotal)");
      

      ////////////// Valid Match Ratio ////////////////
      sprintf(histname, "hTDCWFDMatchEff_ValidMatchRate_%s", det.c_str());
      sprintf(histtitle, "Ratio of Matched pulses (valid only) for %s", det.c_str());
      vhTDCWFDMatchEff_ValidMatchRate[icrate][ich] = new TH1D(histname, histtitle, 2000, 5500, 7500);
      vhTDCWFDMatchEff_ValidMatchRate[icrate][ich]->GetYaxis()->SetTitle("ratio (nVeto|nMatch/nTotal)");
 
    }
  
  }
  cwd->cd();
  return SUCCESS;
}


INT MTDCWFDMatchEff2_eor(int run_number){
  const map< string, vector<TPulseIsland*> >& wfd_map = gData->fPulseIslandToChannelMap;
  const map<string, vector<int64_t> >& tdc_map = gData->fTDCHitsToChannelMap;
      

  for(int icrate = 0; icrate < NCRATE; ++icrate){
    for(int ich = 0; ich < NCHANWFD[icrate]; ++ich) {
      if(WFDBANKS[icrate][ich] == " ") continue;
      std::string det=gSetup->GetDetectorName(WFDBANKS[icrate][ich]);

      vhTDCWFDMatchEff_MatchRate[icrate][ich]->Fill(run_number,(matchedWFD[icrate][ich])/(total[icrate][ich]) );
      vhTDCWFDMatchEff_TriggerlessRate[icrate][ich]->Fill(run_number,(lowMax[icrate][ich])/(total[icrate][ich]) );
      vhTDCWFDMatchEff_VetoRate[icrate][ich]->Fill(run_number,(veto[icrate][ich])/(total[icrate][ich]) );
      vhTDCWFDMatchEff_OverflowRate[icrate][ich]->Fill(run_number,(overflow[icrate][ich])/(total[icrate][ich]) );
      vhTDCWFDMatchEff_PileupRate[icrate][ich]->Fill(run_number,(pileup[icrate][ich])/(total[icrate][ich]) );
      vhTDCWFDMatchEff_MatchedVetoRate[icrate][ich]->Fill(run_number,(matchedVeto[icrate][ich])/(accept[icrate][ich]) );
      vhTDCWFDMatchEff_AcceptRate[icrate][ich]->Fill(run_number, (accept[icrate][ich])/(total[icrate][ich]) );
      vhTDCWFDMatchEff_ValidMatchRate[icrate][ich]->Fill(run_number, (accept[icrate][ich])/(validTot[icrate][ich]) );
    }
  }
  return 0;
}

INT MTDCWFDMatchEff2(EVENT_HEADER *pheader, void *pevent) {
  const map< string, vector<TPulseIsland*> >& wfd_map = gData->fPulseIslandToChannelMap;
  const map<string, vector<int64_t> >& tdc_map = gData->fTDCHitsToChannelMap;
      

  for(int icrate = 0; icrate < NCRATE; ++icrate){
    for(int ich = 0; ich < NCHANWFD[icrate]; ++ich) {
      if(WFDBANKS[icrate][ich] == " ") continue;
      std::string det=gSetup->GetDetectorName(WFDBANKS[icrate][ich]);

      const int polarity = gSetup->GetTriggerPolarity(WFDBANKS[icrate][ich]);
      const int nBits = gSetup->GetNBits(WFDBANKS[icrate][ich]);
      const int max_adc = std::pow(2, nBits);

      //////////// retrieve banks  ///////////////////

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

      //if(!tdc_map.count(VETOBANKS[icrate][ich])){
      //printf("MTDCWFDMatch: No hits in channel %s!\n", VETOBANKS[icrate][ich].c_str());
      //return SUCCESS;
      //}
      //const std::vector<int64_t>& vetos = tdc_map.at(VETOBANKS[icrate][ich]);


      ///////////// set time modifiers ///////////////////

      double toff = gData->fTDCSynchronizationPulseOffset[icrate];
      //toff is a standard alignment parameter for the crate.

      double tcorr = 0, aligncorr = 0;
      //tcorr corrects spectrum to place value near zero in correlation
      //aligncorr is a correction to the align range size, mostly for Ge
      //vcorr is the time difference between veto and channel in MTTScTcorrTDC
      if(det == "GeCHEH" || det == "GeCHEL"){ tcorr = -1600; aligncorr = 1500;}
      if(det == "TSc") {aligncorr = -60; tcorr = -70;}
      if(det == "GeCHT") { tcorr = -60;}



      ///////  pulse loop begins  ///////////////
      int a0 = 0;
      for(int p = 0; p < pulses.size(); ++p){

	///  analyze pulse characteristics  ////////////

	float pedestal = pulses[p]->GetPedestal(8);
	if(det == "TSc") pedestal = pulses[p]->GetPedestal(4);
	const std::vector<int>& samples = pulses[p]->GetSamples();
	const int nSamp = samples.size();
	if(nSamp < 8) continue;

	total[icrate][ich] += 1;

	float max = 0;
	int tMax = 0, max_r = 0;
	bool OF = false, HP = false;

	//get max for CFT
	for(int i = 0; i< nSamp; i++){
	  float adc = samples.at(i);
	  if( (adc == 0 && polarity == -1) || (adc >= max_adc-1 && polarity == 1) ){ overflow[icrate][ich] += 1; OF = true; break; }
	  float tmp = polarity*(adc - pedestal);
	  if(tmp > max){
	    max = tmp; tMax = i; max_r = adc;
	  }
	}


	float threshold = MTDCWFDMatchEff2_Threshold(det);
	float threshold_ps = polarity * (threshold - pedestal);

	if(max < threshold_ps) {
	  lowMax[icrate][ich] += 1;
	  HP = true;
	  //continue;
	}
	else if(pedestal >= threshold && polarity == 1){
	  lowMax[icrate][ich] += 1; HP = true; }
	else if(pedestal <= threshold && polarity == -1){
	  lowMax[icrate][ich] += 1; HP = true; }

	if(pulses[p]->GetPileupPulse() ) pileup[icrate][ich] += 1;
	if(pulses[p]->GetVetoPulse() ) veto[icrate][ich] += 1;

	if(pulses[p]->GetTDCTime() > 0 ) matchedWFD[icrate][ich] += 1;

	if(!( pulses[p]->GetPileupPulse() || OF || HP)/* && pulses[p]->GetTDCTime() > 0 */){
	  validTot[icrate][ich] += 1;
	  if(pulses[p]->GetTDCTime() > 0) {
	    accept[icrate][ich] += 1;
	    if(pulses[p]->GetVetoPulse() )
	      matchedVeto[icrate][ich] += 1;
	  }
	}


	//////////  check for pulse errors  ///////////////

	//if(tdiff_align < ALIGN_LOW - aligncorr) continue;
	//if(pulses[p]->GetVetoPulse()) continue;
	//if(pulses[p]->GetPileupPulse() ) continue;




      }
    }
  }



  return SUCCESS;
} 
  


int MTDCWFDMatchEff2_nSamples(std::string detname){
  int nSamples = 0;
  if(detname == "TSc") nSamples = 12;//12
  else if(detname == "NdetD" || detname == "NdetU") nSamples = 7.5;//150
  else if(detname == "GeCHEH" || detname == "GeCHEL") nSamples = 300;//300
  else if(detname == "GeCHT") nSamples = 150;//150
  else if(detname == "LaBr3") nSamples = 24;//128
  else nSamples = 100;
  return nSamples;
}

float MTDCWFDMatchEff2_Threshold(std::string detname){
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


