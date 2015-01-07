//#define USE_PRINT_OUT 

#include "PlotAmpVsTDiff.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <map>
#include <utility>
#include <algorithm>
#include <cmath>

#include "TAnalysedPulse.h"
#include "TDetectorPulse.h"

using std::string;
using std::map;
using std::vector;
using std::pair;

extern std::map<std::string, std::vector<TAnalysedPulse*> > gAnalysedPulseMap;

PlotAmpVsTDiff::PlotAmpVsTDiff(char *HistogramDirectoryName, std::string det_name_a, std::string det_name_b) :
  FillHistBase(HistogramDirectoryName){ 

  fDetNameA = det_name_a;
  fDetNameB = det_name_b;

  // hAmpVsTDiff
  std::string histname = "h" + fDetNameA + "-" + fDetNameB + "_AmpVsTDiff_Coarse";
  std::string histtitle = "Plot of the amplitude vs time difference for the " + fDetNameA + " and " + fDetNameB + " detector";
  int n_bits = TSetupData::Instance()->GetNBits(TSetupData::Instance()->GetBankName(fDetNameB));
  double max_adc_value = std::pow(2, n_bits);

  std::string x_axis_title = "Time Difference (" + fDetNameB + " - " + fDetNameA + ") [ns]";
  std::string y_axis_title = "Amplitude of " + fDetNameB + " [ADC Value]";

  amp_vs_tdiff_plot_coarse = new TH2F(histname.c_str(), histtitle.c_str(), 100,-50000,50000, max_adc_value,0,max_adc_value);
  amp_vs_tdiff_plot_coarse->GetXaxis()->SetTitle(x_axis_title.c_str());
  amp_vs_tdiff_plot_coarse->GetYaxis()->SetTitle(y_axis_title.c_str());

  histname = "h" + fDetNameA + "-" + fDetNameB + "_AmpVsTDiff_Fine";
  amp_vs_tdiff_plot_fine = new TH2F(histname.c_str(), histtitle.c_str(), 200,-1000,1000, max_adc_value,0,max_adc_value);
  amp_vs_tdiff_plot_fine->GetXaxis()->SetTitle(x_axis_title.c_str());
  amp_vs_tdiff_plot_fine->GetYaxis()->SetTitle(y_axis_title.c_str());


  histname = "h" + fDetNameA + "-" + fDetNameB + "_1D_TDiff";
  histtitle = "Plot of the time difference for the " + fDetNameA + " and " + fDetNameB + " detector";
  oned_tdiff_plot = new TH1F(histname.c_str(), histtitle.c_str(), 1000,-500,3000);
  oned_tdiff_plot->GetXaxis()->SetTitle(x_axis_title.c_str());


  dir->cd("/");
}

PlotAmpVsTDiff::~PlotAmpVsTDiff(){  
}

int PlotAmpVsTDiff::ProcessEntry(TGlobalData *gData, TSetupData *gSetup){
  //typedef map<string, vector<TPulseIsland*> > TStringPulseIslandMap;
  //typedef pair<string, vector<TPulseIsland*> > TStringPulseIslandPair;
  //typedef map<string, vector<TPulseIsland*> >::iterator map_iterator;

  // Get the detA pulses ready for later
  std::vector<TAnalysedPulse*> detA_pulses = gAnalysedPulseMap[fDetNameA];
  std::vector<TAnalysedPulse*>& detB_pulses = gAnalysedPulseMap[fDetNameB];
  

  std::vector<TAnalysedPulse*>::iterator currentDetAPulse = detA_pulses.begin(); // want to keep track of how far we are through the detA pulses

  // Loop through the detB pulses
  for (std::vector<TAnalysedPulse*>::iterator detBPulseIter = detB_pulses.begin(); detBPulseIter != detB_pulses.end(); ++detBPulseIter) {

    // Loop through the detA pulses
    //bool coinc_found = false;
    for (std::vector<TAnalysedPulse*>::iterator detAPulseIter = currentDetAPulse; detAPulseIter != detA_pulses.end(); ++detAPulseIter) {

      double detB_time = (*detBPulseIter)->GetTime();
      double detA_time = (*detAPulseIter)->GetTime();
      double t_diff = detB_time - detA_time;
      double detB_amplitude = (*detBPulseIter)->GetAmplitude();

      amp_vs_tdiff_plot_coarse->Fill(t_diff, detB_amplitude);
      amp_vs_tdiff_plot_fine->Fill(t_diff, detB_amplitude);
      oned_tdiff_plot->Fill(t_diff);

    } // end loop through detA pulses
  } // end loop through detB pulses

  return 0;
}
