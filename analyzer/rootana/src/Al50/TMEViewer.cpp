#include "TMEViewer.h"
#include "RegisterModule.inc"
#include "TGlobalData.h"
#include "TSetupData.h"
#include "ModulesOptions.h"
#include "definitions.h"
#include "TMuonEvent.h"
#include "debug_tools.h"

#include <TH1F.h>
#include <TH2F.h>
#include <TCanvas.h>
#include <TApplication.h>

#include <iostream>
using std::cout;
using std::endl;

extern MuonEventList gMuonEvents;

TMEViewer::TMEViewer(modules::options* opts):
   BaseModule("TMEViewer",opts),fNullCount(0),fTdpCount(0){
}

TMEViewer::~TMEViewer(){
}

int TMEViewer::BeforeFirstEntry(TGlobalData* gData,const TSetupData *setup){
    using namespace IDs;
    fDetectors.push_back(IDs::channel (kScL , kNotApplicable ));
    fDetectors.push_back(IDs::channel (kSiL1_1 , kNotApplicable ));
    fDetectors.push_back(IDs::channel (kSiL1_2 , kNotApplicable ));
    fDetectors.push_back(IDs::channel (kSiL1_3 , kNotApplicable ));
    fDetectors.push_back(IDs::channel (kSiL1_4 , kNotApplicable ));
    fDetectors.push_back(IDs::channel (kSiL2   , kNotApplicable ));
    fDetectors.push_back(IDs::channel (kScR , kNotApplicable ));
    fDetectors.push_back(IDs::channel (kSiR1_1 , kNotApplicable ));
    fDetectors.push_back(IDs::channel (kSiR1_2 , kNotApplicable ));
    fDetectors.push_back(IDs::channel (kSiR1_3 , kNotApplicable ));
    fDetectors.push_back(IDs::channel (kSiR1_4 , kNotApplicable ));
    fDetectors.push_back(IDs::channel (kSiR2   , kNotApplicable ));
    fDetectors.push_back(IDs::channel (kMuSc   , kNotApplicable ));

    fTDiffPerDetector=new TH2F("hTDiffPerChannel", "TDiff to muSc for each channel per TME", 
            5000, -1.2e4,1.2e4,fDetectors.size(),0,fDetectors.size());
    fTDiffPerDetector->SetXTitle("TDiff to central muon (ns)");
    fTDiffPerDetector->SetDrawOption("COLZ");
    fTDiffPerDetector->SetStats(false);
    for(DetectorList::const_iterator i_det=fDetectors.begin();
            i_det!=fDetectors.end(); ++i_det){
        fTDiffPerDetector->GetYaxis()->SetBinLabel(i_det-fDetectors.begin()+1, i_det->str().c_str());
    }

    fQuit = false;
    char** args;
    fApp = new TApplication("app",0,args); //  so we can see the Canvas when we draw it
    fCanvas = new TCanvas("fCanvas", "fCanvas");
  return 0;
}

int TMEViewer::ProcessEntry(TGlobalData* gData,const TSetupData *setup){
  // Loop over each TME
  MuonEventList::const_iterator i_tme = gMuonEvents.begin();

  std::string input;
  bool found_interesting_event = false;
  while (!fQuit || !found_interesting_event) {
    found_interesting_event = false;
    fTDiffPerDetector->Reset();

    // Loop through the detecotrs
    for(DetectorList::const_iterator i_det=fDetectors.begin();i_det!=fDetectors.end(); ++i_det){

      // pulses per channel
      int N=0, n;
      double tme_time= (*i_tme)->GetTime();
      int source_index=(*i_tme)->GetSourceIndex(*i_det);
      while(source_index>-1){
	const IDs::source& source=(*i_tme)->GetSource(source_index);
	n=(*i_tme)->NumPulses(source);
	N+=n;
	for(int i=0; i<n; ++i){
	  const TDetectorPulse* tdp=(*i_tme)->GetPulse(source,i);
	  if(tdp && (tdp->IsPairedPulse() || !tdp->CouldBePaired()) )
	    fTDiffPerDetector->Fill(tdp->GetTime() - tme_time, i_det - fDetectors.begin());
	}
	source_index=(*i_tme)->GetSourceIndex(*i_det,source_index+1);
      }
    }
    if (fTDiffPerDetector->GetEntries() > 5 && !(*i_tme)->HasMuonPileup()) {
      std::cout << "TME #" << i_tme - gMuonEvents.begin() << std::endl;
      found_interesting_event = true;
      fTDiffPerDetector->Draw("COLZ");
      fCanvas->Update();
      
      std::cout << "Press any key to go to next TME (q to quit)" << std::endl;
      std::getline( std::cin, input);
      if (input == "q") {
	fQuit = true;
	break;
      }
    }
    //    std::cout << "TME #" << i_tme - gMuonEvents.begin() << std::endl;
    ++i_tme;
    if (i_tme == gMuonEvents.end() ) {
      std::cout << "At the end" << std::endl;
      break;
    }
  }
  /*    for(MuonEventList::const_iterator i_tme=gMuonEvents.begin();
            i_tme!=gMuonEvents.end(); ++i_tme){

      // First, check for pile-up in the muSc
      if ( (*i_tme)->HasMuonPileup()) {
	continue;
      }

      double tme_time= (*i_tme)->GetTime(); // this is the same as the muSc time
  */
      /*      int source_index = (*i_tme)->GetSourceIndex(*fMuSc);
      const IDs::source& source=(*i_tme)->GetSource(source_index);
      const TDetectorPulse* muSc_tdp = (*i_tme)->GetPulse(source,0);
      double muSc_time = muSc_tdp->GetTime();
      std::cout << tme_time << " - " << muSc_time << " = " << tme_time - muSc_time << std::endl;
      */
  /*
      // Now loop through the SiL1
      for(DetectorList::const_iterator i_det=fSiL1.begin();
                i_det!=fSiL1.end(); ++i_det){
	// pulses per channel
	int SiL1_source_index=(*i_tme)->GetSourceIndex(*i_det);
	while(SiL1_source_index>-1){
	  const IDs::source& SiL1_source=(*i_tme)->GetSource(SiL1_source_index);
	  
	  int n_SiL1 = (*i_tme)->NumPulses(SiL1_source);
	  //	      std::cout << SiL1_source << " has " << n_SiL1 << " pulses" << std::endl;
	  for(int i=0; i<n_SiL1; ++i){
	    const TDetectorPulse* tdp=(*i_tme)->GetPulse(SiL1_source,i);
	    double thin_amplitude = tdp->GetAmplitude();

	    // Loop trhough the SiL2 pulses
	    int SiL2_source_index=(*i_tme)->GetSourceIndex(*fSiL2);
	    const IDs::source& SiL2_source=(*i_tme)->GetSource(SiL2_source_index);
	    int n_SiL2 = (*i_tme)->NumPulses(SiL2_source);
	    for (int j=0; j<n_SiL2; ++j) {
	      const TDetectorPulse* tdp_SiL2=(*i_tme)->GetPulse(SiL2_source,j);
	      double thick_amplitude = tdp_SiL2->GetAmplitude();
	      std::cout << "(i, j) = (" << i << ", " << j << ") = " << thin_amplitude << ", " << thick_amplitude << std::endl;
	      fAvdA_SiL->Fill(thin_amplitude, thin_amplitude+thick_amplitude);
	    }
	  }
	  SiL1_source_index=(*i_tme)->GetSourceIndex(*i_det,SiL1_source_index+1);
	}
      }
    }
*/
  return 0;
}

int TMEViewer::AfterLastEntry(TGlobalData* gData,const TSetupData *setup){

  return 0;
}

ALCAP_REGISTER_MODULE(TMEViewer);
