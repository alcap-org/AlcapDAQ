#include "PlotTDiff.h"
#include <iostream>
#include <string>
#include <sstream>
#include "TGlobalData.h"
#include "TSetupData.h"

#include "TAnalysedPulse.h"
#include "TDetectorPulse.h"
#include "RegisterModule.inc"
#include "definitions.h"
#include "SetupNavigator.h"
//#include "ModulesOptions.h"


#include <cmath>
#include <TH1F.h>
#include <TH2F.h>
//#include <debug_tools.h>


using modules::parser::GetOneWord;
using namespace std;
using std::string;

extern SourceAnalPulseMap gAnalysedPulseMap;

PlotTDiff::PlotTDiff(modules::options* opts):
   BaseModule("PlotTDiff",opts){    
  fDetNameA = opts->GetString("det1");
  fDetNameB = opts->GetString("det2");
}



PlotTDiff::~PlotTDiff(){
}

////////////////////////////////////////////////

int PlotTDiff::BeforeFirstEntry(TGlobalData* gData,const TSetupData *setup){

  //call my source finders and build histogram for each source with
  //a time component.

  //should I continue to work on just two? or should I have this loop 
  //through all pairs

  for(SourceAnalPulseMap::const_iterator sourceIt = gAnalysedPulseMap.begin();
      sourceIt != gAnalysedPulseMap.end(); sourceIt++)
    {
      //std::string fBankNameB = setup->GetBank(fDetNameB);
      if(Debug() && sourceIt->first.Channel() != fDetNameA)
	{
	  cout << sourceIt->first.Channel() << "   " << fDetNameA << endl;
	  continue;   //check for detector A
	}

      
      //I need to check that this source generates a time
      AnalysedPulseList pulses = sourceIt->second;

      



      for(SourceAnalPulseMap::const_iterator sourceIt2 = gAnalysedPulseMap.begin(); sourceIt2 != gAnalysedPulseMap.end(); sourceIt2++)
	{
	  if(sourceIt2->first.Channel() != fDetNameB)
	    continue;  //check for detector B
	  if(sourceIt2->first.Generator() != sourceIt->first.Generator())
	    continue;  //make sure they have the same generator


 
	  //I should make a vector of the sources for detA and detB
	  fDetASources.push_back(sourceIt->first);
	  fDetBSources.push_back(sourceIt2->first);



	}
    }
  return 0;
}

// Called once for each event in the main event loop
// Return non-zero to indicate a problem and terminate the event loop
int PlotTDiff::ProcessEntry(TGlobalData* gData,const TSetupData *setup){

  for(SourceVector::const_iterator sourceIt = fDetASources.begin(); sourceIt != fDetASources.end(); sourceIt++)
    {
      const AnalysedPulseList& detAPulses = gAnalysedPulseMap[*sourceIt];

      //check for GetTime.
      AnalysedPulseList::const_iterator pulseIt = detAPulses.begin();
      pulseIt++;
      if((*pulseIt)->GetTime() == definitions::DefaultValue)      
	continue;


      //Get DetB pulses for the same Generator
      for(SourceVector::const_iterator sourceIt2 = fDetBSources.begin(); sourceIt2 != fDetBSources.end(); sourceIt2++)
	if((*sourceIt2).Generator() == (*sourceIt).Generator())
	  {
	    const AnalysedPulseList& detBPulses = gAnalysedPulseMap[*sourceIt2];


	    //now I should build my histograms, need some keyname(source1?)
	    //std::string keyname = sourceIt;
	    std::string keyname =  sourceIt->str();

	    int nBits = setup->GetNBits(setup->GetBankName(fDetNameA));
	    int maxAmpA = std::pow(2, nBits);

	    nBits = setup->GetNBits(setup->GetBankName(fDetNameB));
	    int maxAmpB = std::pow(2, nBits);

	    //if histogram does not exist, make histograms (amp and/or int, or 1D)
	    if((ampA_plots.find(keyname) == ampA_plots.end()) && ((*pulseIt)->GetAmplitude() != definitions::DefaultValue))
	      {
		//ampA plots
		std::string histname = "h" + fDetNameB + "_" + keyname + "TDiff_AmpA";
		std::string histtitle = "Amplitude of " + fDetNameA + " vs time difference with " + fDetNameB + " detectors with the " + (*sourceIt).Generator().str() + " generator";
		TH2F* AAplots = new TH2F(histname.c_str(), histtitle.c_str(), 1000, -10000, 10000, maxAmpA, 0, maxAmpA);
		AAplots->GetXaxis()->SetTitle("Time Difference (ns)");
		AAplots->GetYaxis()->SetTitle("Amplitude (ADC counts)");
		ampA_plots[keyname] = AAplots;
	    

		//ampB plots
		histname = "h" + fDetNameB + "_" + keyname + " TDiff_AmpB";
		histtitle = "Amplitude of " + fDetNameB + " vs time difference with " + fDetNameA + " detectors with the " + (*sourceIt).Generator().str() + " generator";
		TH2F* ABplots = new TH2F(histname.c_str(), histtitle.c_str(), 1000, -10000, 10000, maxAmpB, 0, maxAmpB);
		ABplots->GetXaxis()->SetTitle("Time Difference (ns)");
		ABplots->GetYaxis()->SetTitle("Amplitude (ADC counts)");
		ampB_plots[keyname] = ABplots;

	      }

	    if((intA_plots.find(keyname) == intA_plots.end()) && ((*pulseIt)->GetIntegral() != definitions::DefaultValue))
	      {
		//intA plots
		std::string histname = "h" + fDetNameB + "_" + keyname + " TDiff_IntA";
		std::string histtitle = "Integral of " + fDetNameA + " vs time difference with " + fDetNameB + " detectors with the " + (*sourceIt).Generator().str() + " generator";
		TH2F* IAplots = new TH2F(histname.c_str(), histtitle.c_str(), 1000, -10000, 10000, maxAmpA, 0, 5*maxAmpA);
		IAplots->GetXaxis()->SetTitle("Time Difference (ns)");
		IAplots->GetYaxis()->SetTitle("Integral (ADC counts)");
		intA_plots[keyname] = IAplots;

		//intB plots
		histname = "h" + fDetNameB + "_" + keyname + " TDiff_IntB";
		histtitle = "Integral of " + fDetNameB + " vs time difference with " + fDetNameA + " detectors with the " + (*sourceIt).Generator().str() + " generator";
		TH2F* IBplots = new TH2F(histname.c_str(), histtitle.c_str(), 1000, -10000, 10000, maxAmpB, 0, 5*maxAmpB);
		IBplots->GetXaxis()->SetTitle("Time Difference (ns)");
		IBplots->GetYaxis()->SetTitle("Integral (ADC counts)");
		intB_plots[keyname] = IBplots;
	      }

	    if(((*pulseIt)->GetIntegral() == definitions::DefaultValue) && ((*pulseIt)->GetAmplitude() == definitions::DefaultValue) && (oned_plots.find(keyname) == oned_plots.end()))
	      {
		std::string histname = "h" + fDetNameB + "_" + keyname + "TDiff_1D";
		std::string histtitle = "Time difference between  " + fDetNameA + " and " + fDetNameB + " detectors with the " + (*sourceIt).Generator().str() + " generator";
		TH1F* dplots = new TH1F(histname.c_str(), histtitle.c_str(), 1000, -10000, 10000);
		dplots->GetXaxis()->SetTitle("Time Difference (ns)");
		dplots->GetYaxis()->SetTitle("Count");
		oned_plots[keyname] = dplots;
	      }


	    for(AnalysedPulseList::const_iterator pulseIt = detAPulses.begin(); pulseIt != detAPulses.end(); pulseIt++)
	      {

		for(AnalysedPulseList::const_iterator pulseIt2 = detBPulses.begin(); pulseIt2 != detBPulses.end(); pulseIt2++)
		  {
		    double timeA = (*pulseIt)->GetTime(), timeB = (*pulseIt2)->GetTime();
		    double tDiff = timeA - timeB;

		    double ampA = (*pulseIt)->GetAmplitude(), ampB = (*pulseIt2)->GetAmplitude();

		    double intA = (*pulseIt)->GetIntegral(), intB = (*pulseIt2)->GetIntegral();


		    if(ampA != definitions::DefaultValue)
		      {
			ampA_plots[keyname]->Fill(tDiff, ampA);
			ampB_plots[keyname]->Fill(-tDiff, ampB);
		      }
	      
		    if(intA != definitions::DefaultValue)
		      {
			intA_plots[keyname]->Fill(tDiff, intA);
			intB_plots[keyname]->Fill(-tDiff, intB);
		      }
	      
		    
		    if((intA == definitions::DefaultValue) && (ampA ==definitions::DefaultValue))
                      oned_plots[keyname]->Fill(tDiff);
		    
		  }//end detBPulse loop
	      }//end detAPulse loop
	  }//end sourceIt2 loop
    }//end sourceIt loop

  return 0;
}

// Called just after the main event loop
// Can be used to write things out, dump a summary etc
// Return non-zero to indicate a problem
int PlotTDiff::AfterLastEntry(TGlobalData* gData,const TSetupData *setup){
  return 0;
}

// The following macro registers this module to be useable in the config file.
// The first argument is compulsory and gives the name of this module
// All subsequent arguments will be used as names for arguments given directly 
// within the modules file.  See the github wiki for more.
ALCAP_REGISTER_MODULE(PlotTDiff,det1,det2);
