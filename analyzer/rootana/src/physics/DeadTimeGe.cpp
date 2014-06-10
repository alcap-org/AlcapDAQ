//#define USE_PRINT_OUT 

#include "DeadTimeGe.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <map>
#include <utility>
#include <algorithm>
#include <cmath>
#include <vector>
#include "definitions.h"

#include "TAnalysedPulse.h"
#include "TDetectorPulse.h"

using std::string;
using std::map;
using std::vector;
using std::pair;

extern StringAnalPulseMap gAnalysedPulseMap;

std::map<std::string, TH1F*> timediff_plots;

DeadTimeGe::DeadTimeGe(char *HistogramDirectoryName) :
  BaseModule(HistogramDirectoryName){  
  dir->cd("/");
}

DeadTimeGe::~DeadTimeGe(){  
}

int DeadTimeGe::ProcessEntry(TGlobalData *gData, TSetupData *gSetup){
  std::string detname = "Ge-S";
  AnalysedPulseList detA_pulses = gAnalysedPulseMap[detname];
  AnalysedPulseList::iterator currentDetAPulse = detA_pulses.begin(); 

  if (timediff_plots.find(detname) == timediff_plots.end())
  {
  	std::string histname = "h" + detname + "_" + GetName();
  	std::string histtitle = "Time difference between to consecutive pulses on " + detname;
  	TH1F * htimediff = new TH1F(histname.c_str(), histtitle.c_str(), 
  			1e6, 10, 4.2e7);
  	htimediff->GetXaxis()->SetTitle("Time [ns]");
  	htimediff->GetYaxis()->SetTitle("Arbitary unit");
  	timediff_plots[detname] = htimediff;
  }
  
  std::vector<double> detA_times;
  for (AnalysedPulseList::iterator detAPulseIter = currentDetAPulse;
  		detAPulseIter != detA_pulses.end(); ++detAPulseIter) 
  {
  	double detA_time = (*detAPulseIter)->GetTime();
  	detA_times.push_back(detA_time);
  }
  
  for (unsigned int i = 0; i < detA_times.size()-1; ++i)
  {
  	//printf("%.2f %.2f %.2f\n", detA_times.at(i+1), detA_times.at(i), 
  			//detA_times.at(i+1) - detA_times.at(i));
  	timediff_plots[detname]->Fill(detA_times.at(i+1) - detA_times.at(i));
  }
  return 0;
}
