//#define USE_PRINT_OUT 

#include "AppraisePulseFinder.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <map>
#include <utility>
#include <algorithm>
#include <cmath>
#include <sstream>

#include "TF1.h"

#include "TAnalysedPulse.h"
#include "TDetectorPulse.h"

#include "RegisterModule.inc"

using std::string;
using std::map;
using std::vector;
using std::pair;

extern std::map<std::string, std::vector<TAnalysedPulse*> > gAnalysedPulseMap;

AppraisePulseFinder::AppraisePulseFinder(char *HistogramDirectoryName) :
  FillHistBase(HistogramDirectoryName){  
  dir->cd("/");
}

AppraisePulseFinder::AppraisePulseFinder(modules::options* opts) : FillHistBase(opts->GetString("mod_name").c_str()) {

  fDetName = opts->GetString("det_name");
  fPulseNumber = opts->GetInt("pulse_number");
  fPulseFinderType = opts->GetString("pulse_finder");

  fPulseFinder = MakeFinder(fPulseFinderType);
  dir->cd("/");
}

AppraisePulseFinder::~AppraisePulseFinder(){  
}

TVPulseFinder* AppraisePulseFinder::MakeFinder(const std::string& finderType){

    // Select the finder type
    TVPulseFinder* finder=NULL;
    // As we develop newer techniques we can add to the list here
    if (finderType == "first"){
	finder = new FirstPulseFinder();
    } else if( finderType == "null") {
      finder = new NullPulseFinder();
    } else {
      std::cout<<"Unknown finder requested: "<<finderType<<std::endl;	
	return NULL;
    }
    return finder;
}

int AppraisePulseFinder::ProcessEntry(TGlobalData *gData, TSetupData *gSetup){
  typedef map<string, vector<TPulseIsland*> > TStringPulseIslandMap;
  typedef pair<string, vector<TPulseIsland*> > TStringPulseIslandPair;
  typedef map<string, vector<TPulseIsland*> >::iterator map_iterator;


  // Loop through and find the correct detector, while counting up the number of pulses in all the previous detectors
  int pulse_counter = 0;
  for(std::map<std::string, std::vector<TPulseIsland*> >::iterator bankIter = gData->fPulseIslandToChannelMap.begin(); bankIter != gData->fPulseIslandToChannelMap.end(); ++bankIter){
    
    std::string bankname = bankIter->first;
    std::string detname = TSetupData::Instance()->GetDetectorName(bankname);
    std::vector<TPulseIsland*> pulses = bankIter->second;
    
    // if this isn't the detector we're looking for, just add the number of pulses to the counter
    if (detname != fDetName) {
      pulse_counter += pulses.size();
      continue;
    }

    // now we have the pulses for the detector we want so calculate what the index should be
    int index = fPulseNumber - pulse_counter - 1;

    // the index should be a number between 0 and the size of the TPulseIsland vector
    if ( index < 0 || index > pulses.size()) {
      std::cout << "Pulse #" << fPulseNumber << " is not from detector " << fDetName << std::endl;
      std::cout << "Check the parameters to this module" << std::endl;
      return 1;
    }

    // Get the TPulseIsland we want to use to appraise the pulse finder 
    // and the samples so that we can add noise to them
    TPulseIsland* thePulse = pulses.at(index);
    std::vector<int> theSamples = thePulse->GetSamples();

    // Create a histogram to plot the information
    TH1F* appraise_hist = new TH1F("appraise_hist", "appraise_hist", 10,0,100);
    appraise_hist->GetXaxis()->SetTitle("RMS of Noise");
    appraise_hist->GetYaxis()->SetTitle("Number of Pulses Found");

    // Iterate throught this process a few times
    int max_iteration = 50;
    for (int iIteration = 0; iIteration < max_iteration; ++iIteration) {
      // Add some known noise and create a new TPulseIsland
      for (int rms = 0; rms <= 100; rms += 10) {
	std::vector<int> theNoisySamples = AddNoise(theSamples, rms);
	TPulseIsland* theNoisyPulse = new TPulseIsland(thePulse->GetTimeStamp(), theNoisySamples, bankname);
	
	// Add it to the vector of pulse islands and then test
	std::vector<TPulseIsland*> thePulseIslandVector;
	thePulseIslandVector.push_back(theNoisyPulse);

	bool plot_pulses = false;
	if (iIteration == 0)
	  plot_pulses = true;

	std::vector<TPulseIsland*> theSubPulses = fPulseFinder->FindPulses(thePulseIslandVector, plot_pulses);
	
	appraise_hist->Fill(rms, theSubPulses.size());
      }
    }
    appraise_hist->Scale(1./max_iteration);

    break; // no need to loop through detectors any more

  } // end loop through detectors
  return 0;
}

std::vector<int> AppraisePulseFinder::AddNoise(const std::vector<int> samples, int RMS) {

  // if the RMS is set to 0, just return the input
  if (RMS == 0)
    return samples;

  std::vector<int> output;

  // The function that we will generate noise from
  std::stringstream funcname;
  funcname << "gaus_" << RMS;
  TF1* gaus = new TF1(funcname.str().c_str(), "TMath::Gaus(x, 0, [0])", -5*RMS,5*RMS);
  gaus->SetParameter(0, RMS);

  // Store the histogram
  TH1* hist = gaus->GetHistogram();
  hist->SetTitle(funcname.str().c_str());
  //  hist->Write();


  // Loop through the samples and add the noise
  for (std::vector<int>::const_iterator inputIter = samples.begin(); inputIter != samples.end(); ++inputIter) {
    output.push_back(*(inputIter) + gaus->GetRandom());
  }

  return output;
}

ALCAP_REGISTER_MODULE(AppraisePulseFinder,mod_name,det_name,pulse_number,pulse_finder);
