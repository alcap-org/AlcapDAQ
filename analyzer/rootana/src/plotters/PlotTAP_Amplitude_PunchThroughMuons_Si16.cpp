//#define USE_PRINT_OUT 

#include "PlotTAP_Amplitude_PunchThroughMuons_Si16.h"
#include <iostream>
#include <string>
#include <sstream>
#include <map>
//#include <utility>
//#include <algorithm>
#include <cmath>

#include "TAnalysedPulse.h"
//#include "TDetectorPulse.h"
#include "RegisterModule.inc"
#include "definitions.h"
#include "SetupNavigator.h"
//#include "debug_tools.h"

using std::string;
using std::map;
using std::vector;
using std::pair;

extern SourceAnalPulseMap gAnalysedPulseMap;

PlotTAP_Amplitude_PunchThroughMuons_Si16::PlotTAP_Amplitude_PunchThroughMuons_Si16(modules::options* opts) : 
    BaseModule("PlotTAP_Amplitude_PunchThroughMuons_Si16",opts) {

    }

PlotTAP_Amplitude_PunchThroughMuons_Si16::~PlotTAP_Amplitude_PunchThroughMuons_Si16(){  
}

int PlotTAP_Amplitude_PunchThroughMuons_Si16::BeforeFirstEntry(TGlobalData *gData, const TSetupData *gSetup){
  
  fSiL1.push_back(IDs::channel (IDs::kSiL1_1 , IDs::kSlow ));
  fSiL1.push_back(IDs::channel (IDs::kSiL1_2 , IDs::kSlow ));
  fSiL1.push_back(IDs::channel (IDs::kSiL1_3 , IDs::kSlow ));
  fSiL1.push_back(IDs::channel (IDs::kSiL1_4 , IDs::kSlow ));
  fSiL1.push_back(IDs::channel (IDs::kSiL1_5 , IDs::kSlow ));
  fSiL1.push_back(IDs::channel (IDs::kSiL1_6 , IDs::kSlow ));
  fSiL1.push_back(IDs::channel (IDs::kSiL1_7 , IDs::kSlow ));
  fSiL1.push_back(IDs::channel (IDs::kSiL1_8 , IDs::kSlow ));
  fSiL1.push_back(IDs::channel (IDs::kSiL1_9 , IDs::kSlow ));
  fSiL1.push_back(IDs::channel (IDs::kSiL1_10 , IDs::kSlow ));
  fSiL1.push_back(IDs::channel (IDs::kSiL1_11 , IDs::kSlow ));
  fSiL1.push_back(IDs::channel (IDs::kSiL1_12 , IDs::kSlow ));
  fSiL1.push_back(IDs::channel (IDs::kSiL1_13 , IDs::kSlow ));
  fSiL1.push_back(IDs::channel (IDs::kSiL1_14 , IDs::kSlow ));
  fSiL1.push_back(IDs::channel (IDs::kSiL1_15 , IDs::kSlow ));
  fSiL1.push_back(IDs::channel (IDs::kSiL1_16 , IDs::kSlow ));
  fSiL3 = new IDs::channel (IDs::kSiL3   , IDs::kSlow );
  
  return 0;
}

int PlotTAP_Amplitude_PunchThroughMuons_Si16::ProcessEntry(TGlobalData *gData, const TSetupData* gSetup){

  const AnalysedPulseList *SiL3_pulses;
  // Get the SiL3 pulses first
  for (SourceAnalPulseMap::const_iterator i_det = gAnalysedPulseMap.begin();
       i_det != gAnalysedPulseMap.end();
       i_det++) {
    
    const std::string& detname = i_det->first.str();
    std::string keyname = i_det->first.str() + GetName();

    IDs::channel ch = i_det->first.Channel();
    if (ch == *fSiL3) {
      SiL3_pulses = &i_det->second;
      break;
    }
  }

  // Now loop through teh SiL1 pulses and plot their amplitudes if they pass a cut
  for (SourceAnalPulseMap::const_iterator i_det = gAnalysedPulseMap.begin();
       i_det != gAnalysedPulseMap.end();
       i_det++) {
    
    const std::string& detname = i_det->first.str();
    std::string keyname = i_det->first.str() + GetName();

    IDs::channel ch = i_det->first.Channel();

    for (DetectorList::const_iterator i_si_channel = fSiL1.begin(); i_si_channel != fSiL1.end(); ++i_si_channel) {
      if (ch == *i_si_channel) {

        // Create the histogram if it's not been created yet
        if ( fAmplitudePlots_PunchThrough.find(keyname) == fAmplitudePlots_PunchThrough.end() ) {

            // hAmplitude
            std::string histname = "h" + detname + "_Amplitude_PunchThroughMuons_Si16";
            std::stringstream histtitle;
            histtitle<<"Amplitude of pulses from source " << i_det->first << " with punch-through muon cut";
            histtitle<<" for run "<<SetupNavigator::Instance()->GetRunNumber();
            int n_bits = gSetup->GetNBits(gSetup->GetBankName(i_det->first.Channel().str()));
            double max_adc_value = std::pow(2, n_bits);
            TH1F* hAmplitude = new TH1F(histname.c_str(), histtitle.str().c_str(), max_adc_value,0,max_adc_value);
            hAmplitude->GetXaxis()->SetTitle("Amplitude (ADC value)");
            hAmplitude->GetYaxis()->SetTitle("Arbitrary Units");
            fAmplitudePlots_PunchThrough[keyname] = hAmplitude;
        }
        if ( fAmplitudePlots_NotPunchThrough.find(keyname) == fAmplitudePlots_NotPunchThrough.end() ) {

            // hAmplitude
            std::string histname = "h" + detname + "_Amplitude_NotPunchThroughMuons_Si16";
            std::stringstream histtitle;
            histtitle<<"Amplitude of pulses from source " << i_det->first << " without punch-through muon cut";
            histtitle<<" for run "<<SetupNavigator::Instance()->GetRunNumber();
            int n_bits = gSetup->GetNBits(gSetup->GetBankName(i_det->first.Channel().str()));
            double max_adc_value = std::pow(2, n_bits);
            TH1F* hAmplitude = new TH1F(histname.c_str(), histtitle.str().c_str(), max_adc_value,0,max_adc_value);
            hAmplitude->GetXaxis()->SetTitle("Amplitude (ADC value)");
            hAmplitude->GetYaxis()->SetTitle("Arbitrary Units");
            fAmplitudePlots_NotPunchThrough[keyname] = hAmplitude;
        }

        const AnalysedPulseList *pulses = &i_det->second;
        //if(Debug() && pulses->empty()) DEBUG_PREFIX<<" no pulses to fill for "<<i_det->first<<std::endl;

        for (AnalysedPulseList::const_iterator pulseIter = pulses->begin(); pulseIter != pulses->end(); ++pulseIter) {
	  double amplitude = (*pulseIter)->GetAmplitude();
	  double SiL1_pulse_time = (*pulseIter)->GetTime();
	  bool coincidence_found = false;
	  double coincidence_cut = 200; 

	  // Now loop through the SiL3 pulses and find the time difference
	  for (AnalysedPulseList::const_iterator SiL3PulseIter = SiL3_pulses->begin(); SiL3PulseIter != SiL3_pulses->end(); ++SiL3PulseIter) {
	    double SiL3_pulse_time = (*SiL3PulseIter)->GetTime();
	    double time_difference = std::fabs(SiL3_pulse_time - SiL1_pulse_time);
	    if (time_difference < coincidence_cut) {
	      coincidence_found = true;
	      //	      std::cout << "AE: " << detname << ": Pulse #" << pulseIter - pulses->begin() << ": Coincidence found |" << SiL3_pulse_time << " - " << SiL1_pulse_time << "| = " << time_difference << " ns (<" << coincidence_cut << " ns): Amplitude = " << amplitude << std::endl;
	    }
	  }
	  if (coincidence_found) {
	    fAmplitudePlots_PunchThrough[keyname]->Fill(amplitude);
	  }
	  else {
	    fAmplitudePlots_NotPunchThrough[keyname]->Fill(amplitude);
	  }

        } // end loop through pulses

      } // end loop through detectors
    }
  }

  return 0;
}

ALCAP_REGISTER_MODULE(PlotTAP_Amplitude_PunchThroughMuons_Si16)
