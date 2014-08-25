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
#include "definitions.h"

#include "TAnalysedPulse.h"
#include "TDetectorPulse.h"
#include "RegisterModule.inc"

using std::string;
using std::map;
using std::vector;
using std::pair;

extern StringAnalPulseMap gAnalysedPulseMap;

PlotAmpVsTDiff::PlotAmpVsTDiff(char *HistogramDirectoryName, std::string det_name_a, std::string det_name_b) :
  BaseModule(HistogramDirectoryName){ 

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
  amp_vs_tdiff_plot_fine = new TH2F(histname.c_str(), histtitle.c_str(), 5000,-5000,20000, max_adc_value,0,max_adc_value);
  amp_vs_tdiff_plot_fine->GetXaxis()->SetTitle(x_axis_title.c_str());
  amp_vs_tdiff_plot_fine->GetYaxis()->SetTitle(y_axis_title.c_str());

  dir->cd("/");
}

PlotAmpVsTDiff::PlotAmpVsTDiff(modules::options* opts) : BaseModule( (opts->GetString("0")).c_str() ) {

  fDetNameA = opts->GetString("1");
  fDetNameB = opts->GetString("2");

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
  amp_vs_tdiff_plot_fine = new TH2F(histname.c_str(), histtitle.c_str(), 5000,-5000,20000, max_adc_value,0,max_adc_value);
  amp_vs_tdiff_plot_fine->GetXaxis()->SetTitle(x_axis_title.c_str());
  amp_vs_tdiff_plot_fine->GetYaxis()->SetTitle(y_axis_title.c_str());

  dir->cd("/");
}

PlotAmpVsTDiff::~PlotAmpVsTDiff(){  
}

int PlotAmpVsTDiff::ProcessEntry(TGlobalData *gData, const TSetupData* gSetup){

  // Get the detA and detB pulses ready but make sure they exist first
  AnalysedPulseList detA_pulses;
  AnalysedPulseList detB_pulses;

  if (gAnalysedPulseMap.find(fDetNameA) == gAnalysedPulseMap.end()) {
    std::cout << fDetNameA << " pulses not found" << std::endl;
  }
  else {
      detA_pulses = gAnalysedPulseMap[fDetNameA];
  }

  if (gAnalysedPulseMap.find(fDetNameB) == gAnalysedPulseMap.end()) {
    std::cout << fDetNameB << " pulses not found" << std::endl;
  }
  else {
      detB_pulses = gAnalysedPulseMap[fDetNameB];
  }
  

  AnalysedPulseList::iterator currentDetAPulse = detA_pulses.begin(); // want to keep track of how far we are through the detA pulses

  // Loop through the detB pulses
  for (AnalysedPulseList::iterator detBPulseIter = detB_pulses.begin(); detBPulseIter != detB_pulses.end(); ++detBPulseIter) {

    // Loop through the detA pulses
    //bool coinc_found = false;
    for (AnalysedPulseList::iterator detAPulseIter = currentDetAPulse; detAPulseIter != detA_pulses.end(); ++detAPulseIter) {

      double detB_time = (*detBPulseIter)->GetTime();
      double detA_time = (*detAPulseIter)->GetTime();
      double t_diff = detB_time - detA_time;
      double detB_amplitude = (*detBPulseIter)->GetAmplitude();

      amp_vs_tdiff_plot_coarse->Fill(t_diff, detB_amplitude);
      amp_vs_tdiff_plot_fine->Fill(t_diff, detB_amplitude);

    } // end loop through detA pulses
  } // end loop through detB pulses

  return 0;
}

ALCAP_REGISTER_MODULE(PlotAmpVsTDiff)
