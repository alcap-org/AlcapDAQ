#include "TMEViewer.h"
#include "RegisterModule.inc"
#include "TGlobalData.h"
#include "TSetupData.h"
#include "ModulesOptions.h"
#include "definitions.h"
#include "TMuonEvent.h"
#include "debug_tools.h"
#include "SetupNavigator.h"

#include "TPaletteAxis.h"

#include <TH1F.h>
#include <TH2F.h>
#include <TCanvas.h>
#include <TApplication.h>

#include <iostream>
using std::cout;
using std::endl;
#include <sstream>

extern SourceAnalPulseMap gAnalysedPulseMap;
extern MuonEventList gMuonEvents;

TMEViewer::TMEViewer(modules::options* opts):
   BaseModule("TMEViewer",opts),fNullCount(0),fTdpCount(0){
}

TMEViewer::~TMEViewer(){
}

int TMEViewer::BeforeFirstEntry(TGlobalData* gData,const TSetupData *setup){
    using namespace IDs;
    fDetectors.push_back(IDs::channel (kSiT_1    , kNotApplicable ));
    fDetectors.push_back(IDs::channel (kSiT_2    , kNotApplicable ));
    fDetectors.push_back(IDs::channel (kSiT_3    , kNotApplicable ));
    fDetectors.push_back(IDs::channel (kSiT_4    , kNotApplicable ));
    fDetectors.push_back(IDs::channel (kSiL1_1 , kNotApplicable ));
    fDetectors.push_back(IDs::channel (kSiL1_2 , kNotApplicable ));
    fDetectors.push_back(IDs::channel (kSiL1_3 , kNotApplicable ));
    fDetectors.push_back(IDs::channel (kSiL1_4 , kNotApplicable ));
    fDetectors.push_back(IDs::channel (kSiL1_5 , kNotApplicable ));
    fDetectors.push_back(IDs::channel (kSiL1_6 , kNotApplicable ));
    fDetectors.push_back(IDs::channel (kSiL1_7 , kNotApplicable ));
    fDetectors.push_back(IDs::channel (kSiL1_8 , kNotApplicable ));
    fDetectors.push_back(IDs::channel (kSiL1_9 , kNotApplicable ));
    fDetectors.push_back(IDs::channel (kSiL1_10 , kNotApplicable ));
    fDetectors.push_back(IDs::channel (kSiL1_11 , kNotApplicable ));
    fDetectors.push_back(IDs::channel (kSiL1_12 , kNotApplicable ));
    fDetectors.push_back(IDs::channel (kSiL1_13 , kNotApplicable ));
    fDetectors.push_back(IDs::channel (kSiL1_14 , kNotApplicable ));
    fDetectors.push_back(IDs::channel (kSiL1_15 , kNotApplicable ));
    fDetectors.push_back(IDs::channel (kSiL1_16 , kNotApplicable ));
    //    fDetectors.push_back(IDs::channel (kSiR1_1 , kNotApplicable ));
    //    fDetectors.push_back(IDs::channel (kSiR1_2 , kNotApplicable ));
    //    fDetectors.push_back(IDs::channel (kSiR1_3 , kNotApplicable ));
    //    fDetectors.push_back(IDs::channel (kSiR1_4 , kNotApplicable ));
    //    fDetectors.push_back(IDs::channel (kSiR2   , kNotApplicable ));
    //    fDetectors.push_back(IDs::channel (kSiL2   , kNotApplicable ));
    //    fDetectors.push_back(IDs::channel (kSiR3   , kNotApplicable ));
    fDetectors.push_back(IDs::channel (kSiL3   , kNotApplicable ));
    fDetectors.push_back(IDs::channel (kGeLoGain , kNotApplicable ));
    //    fDetectors.push_back(IDs::channel (kGeHiGain , kNotApplicable ));

    fTDiffPerDetector=new TH2F("hTDiffPerChannel", "TDiff to central muon for each channel per TME", 
			       4000, -2e4,2e4,fDetectors.size(),0,fDetectors.size());
    fTDiffPerDetector->SetXTitle("TDiff to central muon (ns)");
    fTDiffPerDetector->SetDrawOption("COLZ");
    fTDiffPerDetector->SetStats(false);
    fTDiffPerDetector->SetZTitle("Energy [keV]");
    for(DetectorList::const_iterator i_det=fDetectors.begin();
	i_det!=fDetectors.end(); ++i_det){ 
      // label in reverse so that the detector names appear in the correct order on the plot
      fTDiffPerDetector->GetYaxis()->SetBinLabel(fDetectors.end()-i_det, i_det->str().c_str());
    }


    fTDiffPerDetector_ZoomIn=new TH2F("hTDiffPerChannel_ZoomIn", "TDiff to central muon for each channel per TME", 
			       400, -2e3,2e3,fDetectors.size(),0,fDetectors.size());
    fTDiffPerDetector_ZoomIn->SetXTitle("TDiff to central muon (ns)");
    fTDiffPerDetector_ZoomIn->SetDrawOption("COLZ");
    fTDiffPerDetector_ZoomIn->SetStats(false);
    fTDiffPerDetector_ZoomIn->SetZTitle("Energy [keV]");
    for(DetectorList::const_iterator i_det=fDetectors.begin();
            i_det!=fDetectors.end(); ++i_det){
      // label in reverse so that the detector names appear in the correct order on the plot
        fTDiffPerDetector_ZoomIn->GetYaxis()->SetBinLabel(fDetectors.end()-i_det, i_det->str().c_str());
    }

    fQuit = false;
    char** args;
    fApp = new TApplication("app",0,args); //  so we can see the Canvas when we draw it
    fCanvas = new TCanvas("fCanvas", "fCanvas", 1366,768);
    fPad_TimeWindow = new TPad("fPad_TimeWindow", "", 0.0, 0.5, 0.5, 1.0);
    fPad_TimeWindowZoomIn = new TPad("fPad_TimeWindowZoomIn", "", 0.0, 0.0, 0.5, 0.5);
    fPad_Pulses = new TPad("fPad_Pulses", "", 0.5, 0.0, 1.0, 1.0);

    fPad_TimeWindow->Draw();
    fPad_TimeWindowZoomIn->Draw();
    fPad_Pulses->Draw();

  return 0;
}

int TMEViewer::ProcessEntry(TGlobalData* gData,const TSetupData *setup){
  std::stringstream histname;
  // Loop over each TME
  for(MuonEventList::const_iterator i_tme=gMuonEvents.begin(); i_tme!=gMuonEvents.end(); ++i_tme){

    std::string input;
    bool found_interesting_event = false;

    // Quickly check for interestingness
    /*    int n_pulses_all_ch = (*i_tme)->NumPulses(fDetectors);
        if (n_pulses_all_ch >= 5) {
          found_interesting_event = true;
        }
    */
    /*int sit3_source_index = (*i_tme)->GetSourceIndex(IDs::channel(IDs::kSiT_3, IDs::kNotApplicable));
    if (sit3_source_index > -1) {
      const IDs::source& sit3_source=(*i_tme)->GetSource(sit3_source_index);
      int n=(*i_tme)->NumPulses(sit3_source);
      for(int i=0; i<n; ++i){
	const TDetectorPulse* tdp=(*i_tme)->GetPulse(sit3_source,i);
	if (tdp->GetEnergy() > 7000) {
	  found_interesting_event = true;
	}
      }
    }
    */
    double central_muon_energy = (*i_tme)->GetCentralMuon()->GetEnergy();
    if (central_muon_energy>7000) {
      found_interesting_event = true;
    }
    

    
    if (!found_interesting_event) {
      continue; // to next TME
    }


    // Now get all the pulses
    fTDiffPerDetector->Reset();
    fTDiffPerDetector_ZoomIn->Reset();
    fTPIHists.clear();

    // Loop through the detecotrs
    for(DetectorList::const_iterator i_det=fDetectors.begin();i_det!=fDetectors.end(); ++i_det){

      // pulses per channel
      int N=0, n;
      double tme_time= (*i_tme)->GetTime();
      int source_index=(*i_tme)->GetSourceIndex(*i_det);
      while(source_index>-1){
	const IDs::source& source=(*i_tme)->GetSource(source_index);
	n=(*i_tme)->NumPulses(source);
	//	std::cout << source.Channel() << ": " << n << " pulses" << std::endl;
	N+=n;
	for(int i=0; i<n; ++i){
	  const TDetectorPulse* tdp=(*i_tme)->GetPulse(source,i);
	  if(tdp && (tdp->IsPairedPulse() || !tdp->CouldBePaired()) ) {
	    fTDiffPerDetector->Fill(tdp->GetTime() - tme_time, fDetectors.end()-i_det-1, tdp->GetEnergy());
	    fTDiffPerDetector_ZoomIn->Fill(tdp->GetTime() - tme_time, fDetectors.end()-i_det-1, tdp->GetEnergy());
	    const TAnalysedPulse* tap = tdp->GetTAP(TDetectorPulse::kSlow);
	    if (!tap) {
	      std::cout << "TMEViewer: Couldn't find TAP" << std::endl;
	      continue;
	    }
	    int tpi_id = tap->GetParentID();
	    const std::string& tap_channel = tap->GetSource().Channel().str();
	    const std::string& bankname = TSetupData::Instance()->GetBankName(tap_channel);
	    TPulseIsland* tpi = (gData->fPulseIslandToChannelMap[bankname]).at(tpi_id);
	    if (!tpi) {
	      std::cout << "TMEViewer: Couldn't find TPI: ID = " << tpi_id << ", channel = " << tap_channel << ", bank name = " << bankname << std::endl;
	      continue;
	    }
	    const std::vector<int>& tpi_samples = tpi->GetSamples();
	    histname.str("");
	    histname << "fTPIHist_" << fTPIHists.size();
	    std::cout << histname.str() << std::endl;

	    int downsampling = TSetupData::GetDownSampling(bankname.c_str(), SetupNavigator::Instance()->GetRunNumber());
	    size_t num_samples = tpi->GetPulseLength();
	    double min=0;
	    double max=num_samples*TSetupData::Instance()->GetClockTick(bankname)*downsampling;
	    double pedestal = SetupNavigator::Instance()->GetPedestal(tap_channel);

	    TH1F* hist = new TH1F(histname.str().c_str(), tap_channel.c_str(), num_samples,min,max);
	    hist->SetXTitle("Pulse Time [ns]");
	    hist->SetYTitle("Pedestal Subtracted [ADC]");
	    size_t bin=0;
	    for ( size_t i=0; i < num_samples; ++i) {
	      for (int i_downsample = 0; i_downsample < downsampling; ++i_downsample) {
		bin=i+1+i_downsample;
		
		hist->SetBinContent(bin, tpi_samples.at(i) - pedestal);
	      }
	    }
	    fTPIHists.push_back(hist);
	  }
	}
	source_index=(*i_tme)->GetSourceIndex(*i_det,source_index+1);
      }
    }

    fPad_TimeWindow->Clear();
    fPad_TimeWindowZoomIn->Clear();
    fPad_Pulses->Clear();


    fPad_TimeWindow->cd();
    fTDiffPerDetector->Draw("COLZ TEXT");
    fPad_TimeWindow->SetGrid(1);
    fPad_TimeWindow->SetTickx();
    fPad_TimeWindow->SetTicky();
    fPad_TimeWindow->Modified();
    fPad_TimeWindow->Update();
    fCanvas->Modified();
    fCanvas->Update();
    
    TPaletteAxis *pal = (TPaletteAxis*) fTDiffPerDetector->GetListOfFunctions()->FindObject("palette");
    pal->SetX2NDC(0.925);
    pal->SetY1NDC(0.15);
    pal->GetAxis()->SetLabelSize(0.03);
    fPad_TimeWindow->Modified();
    fPad_TimeWindow->Update();
    
    fPad_TimeWindowZoomIn->cd();
    fTDiffPerDetector_ZoomIn->Draw("COLZ TEXT");
    fPad_TimeWindowZoomIn->SetGrid(1);
    fPad_TimeWindowZoomIn->SetTickx();
    fPad_TimeWindowZoomIn->SetTicky();
    fPad_TimeWindowZoomIn->Modified();
    fPad_TimeWindowZoomIn->Update();
    
    TPaletteAxis *pal_zoomin = (TPaletteAxis*) fTDiffPerDetector_ZoomIn->GetListOfFunctions()->FindObject("palette");
    pal_zoomin->SetX2NDC(0.925);
    pal_zoomin->SetY1NDC(0.15);
    pal_zoomin->GetAxis()->SetLabelSize(0.03);
    fPad_TimeWindowZoomIn->Modified();
    fPad_TimeWindowZoomIn->Update();
    
    int n_hists = fTPIHists.size();
    int division = (n_hists+1)/2; // round up
    fPad_Pulses->Divide(2,division);
    for (std::vector<TH1F*>::const_iterator i_hist = fTPIHists.begin(); i_hist != fTPIHists.end(); ++i_hist) {
      int i_hist_id = i_hist - fTPIHists.begin();
      fPad_Pulses->cd(i_hist_id+1);
      (*i_hist)->Draw("HIST");
    }
    fPad_Pulses->Modified();
    fPad_Pulses->Update();
    fCanvas->Modified();
    fCanvas->Update();
    
    std::cout << "Press any key to go to next TME (ctrl-c to quit)" << std::endl;
    std::getline( std::cin, input);
  }

  return 0;
}

int TMEViewer::AfterLastEntry(TGlobalData* gData,const TSetupData *setup){

  return 0;
}

ALCAP_REGISTER_MODULE(TMEViewer);
