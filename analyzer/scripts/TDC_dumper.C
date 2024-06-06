#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <cmath>
#include <sstream>

#include "TFile.h"
#include "TTree.h"
#include "TBranch.h"
//#include "TH1I.h"
#include "TString.h"
#include "TCanvas.h"
#include "TAxis.h"
#include "TLine.h"

//#include "../src/common/TPulseIsland.h"
#include "../src/common/TPulseIsland.h"
#include "../src/common/TGlobalData.h"
#include "../src/common/TSetupData.h"

// FIXME! Do I actually need this? If so, how to ensure I have enough space?
//const Int_t MAXVEC = 30000;
//const Long64_t MAXVEC = 5e9; // 4.096e9 ticks -- memory overflow
//const Long64_t MAXVEC = 1e7; // 80 MB per allocation
const Long64_t MAXVEC = 1e6; // 8 MB per allocation
//std::cout << "Defined MAXVEC = " << MAXVEC << std::endl;

typedef struct {
  Int_t nhits;
  Long64_t hits[MAXVEC];  
} HitStruct;

//void dump_pulses_hist(std::string fname, std::string outfname, std::string detname) {
void dump_TDC(std::string fname, std::string outfname) {
  std::cout << "Called dump_TDC on file: " << fname << std::endl;
  TGlobalData* gData = NULL;
  TSetupData* gSetup = NULL;

  //TCanvas can;
  
  TFile f(fname.c_str(), "READ");
  TFile* o = NULL;
  
  TTree *events = NULL, *setup = NULL;
  events = (TTree*) f.Get("EventTree");
  setup = (TTree*) f.Get("SetupTree");
  
  events->SetBranchAddress("Event", &gData);
  setup->SetBranchAddress("Setup", &gSetup);
  setup->GetEntry(0);
  
  int nBlocks = events->GetEntries();

  std::string bank="T400";
  //std::vector<int64_t> &hits = gData->fTDCHitsToChannelMap.at(bank);
  HitStruct* hstruct;
  // I don't think we need this -- we can just reuse the same struct as we loop through detectors
  std::map<std::string, HitStruct> fHitStructToChannelMap;

  o = new TFile(outfname.c_str(), "RECREATE");
  std::cout << "Created file " << outfname << " for saving histograms." << std::endl;
  TTree tree("TDC", "A tree with TDC hit time vectors for each block.");
  //std::map<std::string, TBranch[2]> BankToBranchMap;
  //std::map<std::string, TBranch*[2]> BankToBranchMap;
  std::map<std::string, std::vector<TBranch*>> BankToBranchMap;
  //std::map<std::string, TBranch*> BankToBranchMap;
  
  std::string branchname;

  //int det = 0; // to be set in the loop, given the detname in the function
  std::string current_detname = "";
  //int i_det = 0;

  // Build list of detectors based on the banks present in the events
  // and the setup data.
  std::cout << "Building list of detectors present for this run...";
  std::vector< std::string > dets;
  for (int iBlock = 0; iBlock < nBlocks; ++iBlock) {
    events->GetEntry(iBlock);
    //std::map< std::string, std::vector<TPulseIsland*> >::iterator iBank;
    std::map< std::string, std::vector<int64_t> >::iterator iBank;
    //for (iBank = gData->fPulseIslandToChannelMap.begin(); iBank != gData->fPulseIslandToChannelMap.end(); ++iBank){
    for (iBank = gData->fTDCHitsToChannelMap.begin(); iBank != gData->fTDCHitsToChannelMap.end(); ++iBank){
      bank = iBank->first;
      current_detname = gSetup->GetDetectorName(bank);
      // save index if this is the detector we requested
      //if (current_detname == detname)
      //  det = i_det;
      // add to detector list
      if (!std::count(dets.begin(), dets.end(), current_detname)) {
        dets.push_back(current_detname);
        branchname = current_detname + "_" + bank;
        //if ((current_detname=="TSync") || (current_detname=="TRollover")){
        fHitStructToChannelMap[bank] = HitStruct();
        //HitStruct& hstruct = fHitStructToChannelMap[bank];
        //HitStruct* hstruct = &fHitStructToChannelMap[bank];
        //BankToBranchMap[bank].push_back(tree.Branch((branchname+".nhits").c_str(), &hstruct->nhits, "nhits/I"));
        //BankToBranchMap[bank].push_back(tree.Branch((branchname+".hits").c_str(), hstruct->hits, "hits[nhits]/L"));
        BankToBranchMap[bank].push_back(tree.Branch((branchname+".nhits").c_str(), &fHitStructToChannelMap[bank].nhits, "nhits/I"));
        BankToBranchMap[bank].push_back(tree.Branch((branchname+".hits").c_str(), fHitStructToChannelMap[bank].hits, "hits[nhits]/L"));
        /* ---------------------- */
        //BankToBranchMap[bank][0] = tree.Branch((branchname+"_nhits").c_str(), &hstruct.nhits, "nhits/I");
        //BankToBranchMap[bank][1] = tree.Branch((branchname+"_hits").c_str(), hstruct.hits, "hits[nhits]/L");
        //BankToBranchMap[bank].push_back(tree.Branch((branchname+"nhits").c_str(), &hstruct.nhits, branchname+"nhits/I"));
        //BankToBranchMap[bank].push_back(tree.Branch((branchname+".nhits").c_str(), &hstruct.nhits, "nhits/I"));
        //BankToBranchMap[bank].push_back(tree.Branch((branchname+".hits").c_str(), hstruct.hits, "hits[nhits]/L"));
        //BankToBranchMap[bank] = tree.Branch((branchname).c_str(), &hstruct, "nhits/I:hits[nhits]/Long64_t");
        //BankToBranchMap[bank] = tree.Branch((branchname).c_str(), &hstruct, "nhits/I:hits[nhits]/L");
        // THIS ONE WORKS (but can't load into uproot...        
        //BankToBranchMap[bank] = tree.Branch((branchname).c_str(), &hstruct, "nhits/I:hits[nhits]/L");
        //}
        //i_det += 1;
      }
    }
  }
  std::cout << "done." << std::endl;

  std::cout << "Sorting detector list...";
  std::sort(dets.begin(), dets.end());
  std::cout << "done." << std::endl;
  
  //int det = ChooseDetector(dets);
  // find appropriate detector
  int det = 0;
  std::string detname="TSync";
  std::string current_det = "";
  std::cout << "Available detectors:" << std::endl;
  for (int i = 0; i < (int)dets.size(); ++i) {
      current_det = dets.at(i);
      std::cout << i << ":\t" << current_det;      
      //if (current_det == detname) {
      //  det = i;
      //  std::cout << " (SELECTED)";
      //}
      std::cout << std::endl;
  }
  //std::cout << "Selected detector: " << detname << ", grabbed detector index: " << det << std::endl;
  //std::cout << "Selected detector: " << detname << ", grabbed detector index: " << std::to_string(det) << std::endl;
  
  //std::string detname;  
  //std::string bank;

  // switch between save and iterate pulse
  //char in = 's';
  
  //detname
  for (int det = 0; det < (int)dets.size(); ++det) {    
    detname = dets.at(det);
    //if ((detname!="TSync") && (detname!="TRollover")){
    //  continue;
    //}
    std::cout << "Detector: " << detname << ", detector index: " << det << std::endl;
    bank = gSetup->GetBankName(dets.at(det));
    //HitStruct& hstruct = fHitStructToChannelMap[bank];
    //HitStruct* hstruct = &fHitStructToChannelMap[bank];
    hstruct = &fHitStructToChannelMap[bank];
    branchname = detname + "_" + bank;
    //int max = (int)std::pow(2,gSetup->GetNBits(bank));
    //for (int iBlock = 0; iBlock < 10; ++iBlock) { // testing...just do a few blocks
    for (int iBlock = 0; iBlock < nBlocks; ++iBlock) {
      events->GetEntry(iBlock);

      // Go to next block if there's no data for this detector
      if (!gData->fTDCHitsToChannelMap.count(bank)) {
        std::cout << "No hits for block " << iBlock + 1 << "." << std::endl;
        continue;
      }

      //std::vector<TPulseIsland*> &pulses = gData->fPulseIslandToChannelMap.at(bank);
      //int nPulses = (int)pulses.size();
      std::vector<int64_t> &hits = gData->fTDCHitsToChannelMap.at(bank);
      //hits = gData->fTDCHitsToChannelMap.at(bank);
      int nHits = (int)hits.size();
      //hstruct.hits = hits;
      //for (Int_t hind=0; hind<hits.size(); hind++) hstruct.hits[hind] = hits[hind];
      //hstruct.nhits = nHits;
      for (Int_t hind=0; hind<hits.size(); hind++) hstruct->hits[hind] = hits[hind];
      hstruct->nhits = nHits;
      if ((iBlock % 100) == 0)    
          std::cout << "Block: " << iBlock + 1 << "/" << nBlocks << " has " << nHits << " hits." << std::endl;
      //if (iBlock == 0) {
        //o = new TFile(outfname.c_str(), "RECREATE");
	      //std::cout << "Created file " << outfname << " for saving histograms." << std::endl;
        //TTree tree("TDC", "A tree with TDC hit time vectors for each block.");
        //tree.Branch(detname, &hits, 64000, 1);
      //}
      //tree.Fill();
      //BankToBranchMap[bank][0].Fill();
      //BankToBranchMap[bank][1].Fill();
      BankToBranchMap[bank][0]->Fill();
      BankToBranchMap[bank][1]->Fill();
      //BankToBranchMap[bank]->Fill();
    }
  }
  tree.SetEntries(nBlocks);    
/*
      for (int iPulse = 0; iPulse < nPulses; ++iPulse) {
        std::vector<int> samps = pulses.at(iPulse)->GetSamples();
        TH1I pulse(("pulse_"+std::to_string(iBlock)+"_"+std::to_string(iPulse)).c_str(), (dets.at(det) + ";Time (ticks); ADC Value").c_str(), samps.size(), -0.5, samps.size() - 0.5);
        pulse.GetYaxis()->SetRangeUser(0, max);

        //std::cout << "Time per Tick: " << gSetup->GetClockTick(bank) << std::endl;
        //std::cout << "Timestamp: " << pulses.at(iPulse)->GetTimeStamp() << std::endl;
        if ((iPulse == 0) and ((iBlock % 100) == 0))    
          std::cout << "Block (Pulse): " << iBlock + 1 << "/" << nBlocks << " (" << iPulse + 1 << "/" << nPulses << ")" << std::endl;
        for (int iSample = 0; iSample < (int)samps.size(); iSample++)
	  pulse.SetBinContent(iSample + 1, samps[iSample]);
        pulse.Draw();
        // To draw the constant fraction line, uncomment the following line
        //double cft = GetCFTime(gSetup, pulses.at(iPulse)); TLine l(cft, 0, cft, max); l.Draw("SAME");
        //can.Update();
        //char in = GetInput();
        // always go to next pulse
        //char in = 'n';
        //switch (in) {
        //case 'n' : // Next pulse
    //in = 's'; // next time save the pulse
  //	break;
        //case 's' : // Save pulse
    //in = 'n'; // next time want to iterate
	  if (!o) {
	    //o = new TFile("pulses.root", "RECREATE");
      o = new TFile(outfname.c_str(), "RECREATE");
	    std::cout << "Created file " << outfname << " for saving histograms." << std::endl;
    }
    if ((iBlock == 0) and (iPulse == 0)) {
      o->cd();
      //o->mkdir((detname+"/").c_str());
      gDirectory->mkdir((detname).c_str());  
      o->cd((detname).c_str());
    }	
	  //o->cd();
    ////o->mkdir((detname+"/").c_str());
    //gDirectory->mkdir((detname).c_str());  
    //o->cd((detname).c_str());
	  {
	    std::stringstream name;
	    name << dets.at(det) << "_Block" << iBlock+1 << "_Pulse" << iPulse+1 << "_Timestamp" << pulses.at(iPulse)->GetTimeStamp();
	    //pulse.SetDirectory((detname+"/").c_str());
      pulse.Write(name.str().c_str());
	    if ((iPulse == 0) and ((iBlock % 100) == 0))
        std::cout << "Saved pulse histogram " << name.str() << " to " << outfname << "." << std::endl;
	  }
	  //iPulse--;
	  //break;
    //    case 'd' : // Choose a different detector
	  //det = ChooseDetector(dets);
      }
    }
  }
  if (o) {
    o->Close();
    delete o;
    o = NULL;
  }
*/
  if (o) {
    //save the Tree header. The file will be automatically closed
    //when going out of the function scope
    tree.Write();
    o->Close();
    delete o;
    o = NULL;
  }
}
