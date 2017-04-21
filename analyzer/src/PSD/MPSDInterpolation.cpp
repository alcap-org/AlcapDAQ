/* Standard includes */
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <map>
#include <utility>
#include <sstream>
#include <cmath>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <ios>

/* MIDAS includes */
#include "midas.h"

/* ROOT includes */
#include <TH1.h>
#include <TH2.h>
#include <TDirectory.h>
#include "TFile.h"
#include "TApplication.h"
#include "TROOT.h"
#include "TMath.h"
#include "Math/Interpolator.h"

/* AlCap includes */
#include "AlCap.h"
#include "TGlobalData.h"
#include "TSetupData.h"
#include "TPulseIsland.h"

using namespace std;

/*-- Module declaration --------------------------------------------*/
INT  MPSDInterpolation_init(void);
INT  MPSDInterpolation(EVENT_HEADER*, void*);
//INT  MPSDInterpolation_eor(INT);
bool MPSDInterpolation_Neutron(std::string, double, float);

extern HNDLE hDB;
extern TGlobalData* gData;
extern TSetupData* gSetup;

//All histograms plotted using fit energy
using namespace AlCap;
namespace {
  TH2D* NdetSimpsonRatio[NCRATE][10];
  TH2D* NdetInterpolationRatio[NCRATE][10];
  TH2D* NdetInterpolationTRatio[NCRATE][10];
  TH2D* NdetSimpsonRatiovIntegral[NCRATE][10];
  std::string WFD_BANK_NAME[NCRATE][10];
  const int TPILength = 152;
  const int interpolation_factor = 4;
  int PSDInterpolationG_count = 0;
  int PSDInterpolationN_count = 0;
}

//initialize interpolation parameters



ANA_MODULE MPSDInterpolation_module =
{
	"MPSDInterpolation",          /* module name           */
	"Damien Alexander",            /* author                */
	MPSDInterpolation,            /* event routine         */
	NULL,                          /* BOR routine           */
	NULL,                          /* EOR routine           */
        MPSDInterpolation_init,        /* init routine          */
	NULL,                          /* exit routine          */
	NULL,                          /* parameter structure   */
	0,                             /* structure size        */
	NULL,                          /* initial parameters    */
};

/////////////////////////////////////////////////////////////////



INT MPSDInterpolation_init()
{
  TDirectory* cwd = gDirectory;
  gDirectory->mkdir("PSDInterpolation")->cd();

  // const std::map<std::string, std::vector<TPulseIsland*> >& tpi_map = gData->fPulseIslandToChannelMap;

  //for(std::map<std::string, std::vector<TPulseIsland*> >::const_iterator mIter = tpi_map.begin(); mIter != tpi_map.end(); mIter++){
  for(int icrate = 0; icrate < NCRATE; ++icrate) {
    for(int ich = 0; ich < NCHANWFD[icrate]; ++ich) {
      char bank[5];
      sprintf(bank, "D%d%02d", icrate, ich);
      WFD_BANK_NAME[icrate][ich] = bank;
      const std::string detname = gSetup->GetDetectorName(bank);
      //std::cout << bank << " is checked for a neutron bank" << std::endl;

      if(!gSetup->IsNeutron(detname)) continue;
    
      std::string histname = "h" + detname + "RatioS";
      std::string histtitle = "Integral Ratio (Simpson) vs Energy for " + detname;
      NdetSimpsonRatio[icrate][ich] = new TH2D(histname.c_str(), histtitle.c_str(), 600, 0, 7.5, 600, 0, 0.45);
      NdetSimpsonRatio[icrate][ich]->GetYaxis()->SetTitle("Integral Ratio");
      NdetSimpsonRatio[icrate][ich]->GetXaxis()->SetTitle("Energy (MeVee)");
    
      histname = "h" + detname + "InterpolationRatio";
      histtitle = "Integral Ratio (Interpolation) vs Energy for " + detname;
      NdetInterpolationRatio[icrate][ich] = new TH2D(histname.c_str(), histtitle.c_str(), 600, 0, 7.5, 600, 0, 0.45);
      NdetInterpolationRatio[icrate][ich]->GetYaxis()->SetTitle("Integral Ratio");
      NdetInterpolationRatio[icrate][ich]->GetXaxis()->SetTitle("Energy (MeVee)");

      histname = "h" + detname + "InterpolationTRatio";
      histtitle = "Integral Ratio (Interpolation w/ Threshold 15) vs Energy for " + detname;
      NdetInterpolationTRatio[icrate][ich] = new TH2D(histname.c_str(), histtitle.c_str(), 600, 0, 7.5, 600, 0, 0.45);
      NdetInterpolationTRatio[icrate][ich]->GetYaxis()->SetTitle("Integral Ratio");
      NdetInterpolationTRatio[icrate][ich]->GetXaxis()->SetTitle("Energy (MeVee)");


      histname = "h" + detname + "SimpsonRatioIntegral";
      histtitle = "Integral Ratio (Simpson) vs Energy (Integral) for " + detname;
      NdetSimpsonRatiovIntegral[icrate][ich] = new TH2D(histname.c_str(), histtitle.c_str(), 600, 0, 7.5, 600, 0, 0.45);
      NdetSimpsonRatiovIntegral[icrate][ich]->GetYaxis()->SetTitle("Integral Ratio");
      NdetSimpsonRatiovIntegral[icrate][ich]->GetXaxis()->SetTitle("Energy (MeVee) (Integral)");
    }
  }

  cwd->cd();

  return SUCCESS;
  
  }

INT MPSDInterpolation_eor(INT run_number)
{
  return SUCCESS;
}

INT MPSDInterpolation(EVENT_HEADER *pheader, void *pevent)
{
  const std::map<std::string, std::vector<TPulseIsland*> >& tpi_map = gData->fPulseIslandToChannelMap;

  for(int icrate = 0; icrate < NCRATE; ++icrate) {
    for(int ich = 0; ich < NCHANWFD[icrate]; ++ich) {
      std::string bankname = WFD_BANK_NAME[icrate][ich];
      if(!tpi_map.count(bankname) ) continue;
      std::string detname = gSetup->GetDetectorName(bankname);

      if(!gSetup->IsNeutron(detname) ) continue;


      const std::vector<TPulseIsland*>& pulses = tpi_map.at(bankname);
      int polarity = gSetup->GetTriggerPolarity(bankname);
      int nbits = gSetup->GetNBits(bankname);
      int max_adc = std::pow(2, nbits);
   
      //float pct = 0.1;

      for(std::vector<TPulseIsland*>::const_iterator pIter = pulses.begin(); pIter != pulses.end(); pIter++){

	Double_t integral = 0, integralS = 0;
	Double_t lInt = 0, sInt = 0, ratio = 0;
	Double_t lIntI = 0, sIntI = 0, ratioI = 0;
	Double_t lIntIT = 0, sIntIT = 0, ratioIT = 0;
	int tLstart = 0, tSstart = 0, tstop = 0;
	int max = 0, tMax = 0;
	float thresh = 15;

	const std::vector<int>& samples = (*pIter)->GetSamples();
	float fitMax = (*pIter)->GetFitMax();
	const int nSamp = samples.size();

	//////////////Pedestal /////////////////////////////////////

	float pedestal = 0, pedSamp = 0;

	for(std::vector<int>::const_iterator pulse_begin = samples.begin() + 1; pulse_begin != samples.begin() + 16; pulse_begin++){
	  pedestal += (*pulse_begin);
	  pedSamp += 1;
	}
	pedestal /= pedSamp;

	//Initialize arrays
	Double_t xi[nSamp];
	Double_t yi[nSamp];
	Double_t xout[interpolation_factor * nSamp];
	Double_t yout[interpolation_factor * nSamp];

	//fill base array with pedestal subtracted values
	//if this value is negative, set to zero
	//find pulse height max and time

	for(int i = 0; i < nSamp; i++){
	  xi[i] = i;
	  yi[i] = polarity * (samples.at(i) - pedestal);
	  //if(yi[i] < 0) yi[i] = 0;
	  if(yi[i] > max){ max = yi[i]; tMax = i; }
	  integral += yi[i];
	}

	//Simpson integral

	for(int i = 0; i+2 <= nSamp; i+=2){
	  integralS += (yi[i] + 4*yi[i+1] + yi[i+2])/3; 
	}

	tLstart = tMax - 7;
	if(tLstart <= 2) continue;
	if(tLstart > nSamp - 3) continue; //late pulse
      
	tSstart = tMax + 9;
	if(tSstart < 0) continue;

	tstop = tMax + 81;
	if(tstop >= nSamp) continue; 

	int tLstartI = tLstart * interpolation_factor;
	int tSstartI = tSstart * interpolation_factor;
	int tstopI = tstop * interpolation_factor;

	//std::cout << tLstart << "   " << tSstart << "   " << tstop << std::endl;


	/////////  Interpolate   //////////////////////////////////////

	ROOT::Math::Interpolator inter(nSamp, ROOT::Math::Interpolation::kCSPLINE);
	inter.SetData(nSamp, xi, yi );

	for(int i=0; i<(interpolation_factor * (nSamp - 1) + 1); i++) {
	  xout[i] = i*0.25;
	  yout[i] = inter.Eval(xout[i]);
	}

	///////////////  long gate  ////////////////////////////////////

	int lastSamp = 0;
	//integrate bulk, Simpson's rule
	for(int i = tLstart; i+2 <= tstop; i += 2){
	  lInt += (yi[i] + (4*yi[i+1]) + yi[i+2])/3;
	  lastSamp = i+2;
	}
	//assume pulse tail not at endpoint, linearly interpolate last point
	if(lastSamp != tstop){
	  lInt += yi[tstop];
	}

	for(int i = tLstartI; i < tstopI; i++) {
	  lIntI += yout[i];
	  if(yout[i] > thresh) lIntIT += yout[i];
	}

	/////////////  short gate  /////////////////////////////////

	lastSamp = 0;
	//integrate bulk, Simpson's rule
	for(int i = tSstart; i+2 <= tstop; i += 2){
	  sInt += (yi[i] + (4*yi[i+1]) + yi[i+2])/3;
	  lastSamp = i+2;
	}
	//integrate last, trapezoidal with linear interpolation
	if(lastSamp != tstop){
	  sInt += yi[tstop];
	}

	for(int i = tSstartI; i < tstopI; i++) {
	  sIntI += yout[i];
	  if(yout[i] > thresh) sIntIT += yout[i];
	}

	/////////////////////////////////////////////////////////

	//fill the histograms
	ratio = sInt/lInt;
	ratioI = sIntI/lIntI;
	ratioIT = sIntIT/lIntIT;

	// ignore pulses with zero or maximum value for overflow
	if(*(std::min_element( samples.begin(), samples.end() )) <= 0) continue;
	if(*(std::max_element( samples.begin(), samples.end() )) >= max_adc-1) continue;

	double eFit = (*pIter)->GetEnergyFit(fitMax);
	double eInt = (*pIter)->GetEnergyInt(integral);

	NdetSimpsonRatio[icrate][ich]->Fill(eFit, ratio);
	NdetInterpolationRatio[icrate][ich]->Fill(eFit, ratioI);
	NdetInterpolationTRatio[icrate][ich]->Fill(eFit, ratioIT);
	NdetSimpsonRatiovIntegral[icrate][ich]->Fill(eInt, ratio);


	// determine best first
	//(*pIter)->SetPSDParameter(ratio);


	//Plot some questionable pulses
	if(PSDInterpolationG_count >= 10  && PSDInterpolationN_count >= 10) continue;
	bool plot = false;
	if(eFit < 0.25 && ratioIT < 0.15 && max <= 410 && tMax == 21) plot = true;
	if(PSDInterpolationG_count < 10 && detname == "NdetD" && plot){
	  std::stringstream ss;
	  ss << PSDInterpolationG_count + 1;
	  std::string histname = "hPSDIntGPulse_" + ss.str();
	  std::string histtitle = "Gamma Waveform " + ss.str();
	  TH1F* hIPulse = new TH1F(histname.c_str(), histtitle.c_str(), nSamp*interpolation_factor, 0, nSamp*interpolation_factor);
	  for(int j = 0; j < nSamp*interpolation_factor; j++)
	    hIPulse->Fill(j, yout[j]);
	  PSDInterpolationG_count++;
	}

	plot = false;
	if(eFit < 0.25 && ratioIT > 0.15 && max <= 410 && tMax == 21) plot = true;
	if(PSDInterpolationN_count < 10 && detname == "NdetD" && plot){
	  std::stringstream ss;
	  ss << PSDInterpolationN_count + 1;
	  std::string histname = "hPSDIntNPulse_" + ss.str();
	  std::string histtitle = "Neutron Waveform " + ss.str();
	  TH1F* hIPulse = new TH1F(histname.c_str(), histtitle.c_str(), nSamp*interpolation_factor, 0, nSamp*interpolation_factor);
	  for(int j = 0; j < nSamp*interpolation_factor; j++)
	    hIPulse->Fill(j, yout[j]);
	  PSDInterpolationN_count++;
	}

      }

    }

  }

  return SUCCESS;
}

