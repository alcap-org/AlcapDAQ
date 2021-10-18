//#define USE_PRINT_OUT 

#include "PlotTAP_ConsecutiveTDiff.h"
#include <iostream>
#include <string>
#include <sstream>
#include <map>
//#include <utility>
//#include <algorithm>
#include <cmath>
#include "definitions.h"
#include "SetupNavigator.h"

#include "TAnalysedPulse.h"
#include "TDetectorPulse.h"
#include "RegisterModule.inc"
//#include "debug_tools.h"

using std::string;
using std::map;
using std::vector;
using std::pair;

extern SourceAnalPulseMap gAnalysedPulseMap;

PlotTAP_ConsecutiveTDiff::PlotTAP_ConsecutiveTDiff(modules::options* opts) :
    BaseModule("PlotTAP_ConsecutiveTDiff",opts) {
    }

PlotTAP_ConsecutiveTDiff::~PlotTAP_ConsecutiveTDiff(){  
}

int PlotTAP_ConsecutiveTDiff::ProcessEntry(TGlobalData *gData, const TSetupData* gSetup){

  double min_time = 0;
  double max_time = 100e6;
  double n_bins = 1e3;

  double min_zoom_time = 0;
  double max_zoom_time = 100e3;
    // Loop over each TAP list
    for (SourceAnalPulseMap::const_iterator i_det = gAnalysedPulseMap.begin();
            i_det != gAnalysedPulseMap.end();
            i_det++) {


        const std::string& detname = i_det->first.str();
        std::string keyname = i_det->first.str() + GetName();

        // Create the histogram if it's not been created yet
        if ( fConsecutiveTDiffPlots.find(keyname) == fConsecutiveTDiffPlots.end() ) {

            // hConsecutiveTDiff
            std::string histname = "h" + detname + "_ConsecutiveTDiff";
            std::stringstream histtitle;
            histtitle<<"ConsecutiveTDiff of pulses from source " << i_det->first;
            histtitle<<" for run "<<SetupNavigator::Instance()->GetRunNumber();
            TH1F* hConsecutiveTDiff = new TH1F(histname.c_str(), histtitle.str().c_str(), n_bins,min_time,max_time);
            hConsecutiveTDiff->GetXaxis()->SetTitle("ConsecutiveTDiff (ns)");
            hConsecutiveTDiff->GetYaxis()->SetTitle("Arbitrary Units");
            fConsecutiveTDiffPlots[keyname] = hConsecutiveTDiff;
	    //	    fLastTimes[keyname] = 0;

	    // hConsecutiveTDiffZoom
            histname = "h" + detname + "_ConsecutiveTDiffZoom";
            histtitle.str("");
            histtitle<<"ConsecutiveTDiff of pulses from source " << i_det->first;
            histtitle<<" for run "<<SetupNavigator::Instance()->GetRunNumber();
            TH1F* hConsecutiveTDiffZoom = new TH1F(histname.c_str(), histtitle.str().c_str(), n_bins,min_zoom_time,max_zoom_time);
            hConsecutiveTDiffZoom->GetXaxis()->SetTitle("ConsecutiveTDiff (ns)");
            hConsecutiveTDiffZoom->GetYaxis()->SetTitle("Arbitrary Units");
            fConsecutiveTDiffZoomPlots[keyname] = hConsecutiveTDiffZoom;

	    // hConsecutiveTDiffWAmpCut
            histname = "h" + detname + "_ConsecutiveTDiffWAmpCut";
            histtitle.str("");
            histtitle<<"ConsecutiveTDiff (with amp cut) of pulses from source " << i_det->first;
            histtitle<<" for run "<<SetupNavigator::Instance()->GetRunNumber();
            TH1F* hConsecutiveTDiffWAmpCut = new TH1F(histname.c_str(), histtitle.str().c_str(), n_bins,min_time,max_time);
            hConsecutiveTDiffWAmpCut->GetXaxis()->SetTitle("ConsecutiveTDiff (ns)");
            hConsecutiveTDiffWAmpCut->GetYaxis()->SetTitle("Arbitrary Units");
            fConsecutiveTDiffWAmpCutPlots[keyname] = hConsecutiveTDiffWAmpCut;
	    //	    fLastTimesWAmpCut[keyname] = 0;

	    // hConsecutiveTDiffZoomWAmpCut
            histname = "h" + detname + "_ConsecutiveTDiffZoomWAmpCut";
            histtitle.str("");
            histtitle<<"ConsecutiveTDiff (with amp cut) of pulses from source " << i_det->first;
            histtitle<<" for run "<<SetupNavigator::Instance()->GetRunNumber();
            TH1F* hConsecutiveTDiffZoomWAmpCut = new TH1F(histname.c_str(), histtitle.str().c_str(), n_bins,min_zoom_time,max_zoom_time);
            hConsecutiveTDiffZoomWAmpCut->GetXaxis()->SetTitle("ConsecutiveTDiff (ns)");
            hConsecutiveTDiffZoomWAmpCut->GetYaxis()->SetTitle("Arbitrary Units");
            fConsecutiveTDiffZoomWAmpCutPlots[keyname] = hConsecutiveTDiffZoomWAmpCut;

	    // fScatterPlots
	    histname = "h" + detname + "_ScatterPlots";
            histtitle.str("");
            histtitle<<"Scatter (with amp cut) of pulses from source " << i_det->first;
            histtitle<<" for run "<<SetupNavigator::Instance()->GetRunNumber();
            TH2F* hScatterPlot = new TH2F(histname.c_str(), histtitle.str().c_str(), n_bins,min_time,max_time, n_bins,min_time,max_time);
            hScatterPlot->GetXaxis()->SetTitle("t_{1} [ns]");
            hScatterPlot->GetYaxis()->SetTitle("t_{2} [ns]");
            fScatterPlots[keyname] = hScatterPlot;
        }

        const AnalysedPulseList *pulses =& i_det->second;
        //if(Debug() && pulses->empty()) DEBUG_PREFIX<<" no pulses to fill for "<<i_det->first<<std::endl;
	//	double amp_threshold = 455;//90;//455; GeLoGain
	//	double amp_threshold = 180;//980;//180; // GeHiGain
	//	double amp_threshold = 1500; // muSc, run 9040
	double amp_threshold = 0; // SiR2-S
	if (pulses->size()>1) {
	  double last_time = -1000;
	  
	  for (AnalysedPulseList::const_iterator pulseIter = pulses->begin(); pulseIter != pulses->end(); ++pulseIter) {
	    double time = (*pulseIter)->GetTime();
	    if (last_time < 0) {
	      last_time = time;
	      continue;
	    }
	    
	    double tdiff = time - last_time;
	    fConsecutiveTDiffPlots[keyname]->Fill(tdiff);
	    fConsecutiveTDiffZoomPlots[keyname]->Fill(tdiff);
	    fScatterPlots[keyname]->Fill(last_time, time);
	    last_time = time;
	  }

	  last_time = -1000;

	  for (AnalysedPulseList::const_iterator pulseIter = pulses->begin(); pulseIter != pulses->end(); ++pulseIter) {
	    if ( (*pulseIter)->GetAmplitude() < amp_threshold) {
	      continue;
	    }

	    double time = (*pulseIter)->GetTime();
	    if (last_time < 0) {
	      last_time = time;
	      continue;
	    }

	    double tdiff = time - last_time;
	    fConsecutiveTDiffWAmpCutPlots[keyname]->Fill(tdiff);
	    fConsecutiveTDiffZoomWAmpCutPlots[keyname]->Fill(tdiff);
	    last_time = time;
	  }
	}


    } // end loop through detectors
    return 0;
}

ALCAP_REGISTER_MODULE(PlotTAP_ConsecutiveTDiff)
