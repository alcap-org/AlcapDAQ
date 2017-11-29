//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Mon Nov  6 12:20:30 2017 by ROOT version 6.07/06
// from TTree TMETree/
// found on file: out09682.root
//////////////////////////////////////////////////////////

#ifndef TMETree_h
#define TMETree_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include <TTree.h>
#include <TNtuple.h>
#include <TH1.h>
#include <TH2.h>
#include <TF1.h>
#include <TSelector.h>
#include <TTreeReader.h>
#include <TTreeReaderValue.h>

// Headers needed by this particular selector
#include <string>
#include <vector>

#include "AlCapConstants.h"
#include "SimplePulse.h"

class TMETree : public TSelector {
public :

   map<const char *, vector<TH1D *> > hSiT;  
   map<const char *, TH1D *> hGeLoGain;  
   map<const char *, TH1D *> hGeHiGain;  

/** 
 * map contains, time, energy, likelihood probability cut, neural network cut as the key
 * and histograms of the result, raw and cut
 * */
   map<const char *, vector<TH2D *> > hSiL;
   map<const char *, vector<TH2D *> > hSiR;

   map<const char *, TH1D *> hProbabilityDistribution;
   map<const char *, TF1 *> fitFunctions;

   TFile *fOutputFile;
   Double_t SiL1_E, SiL3_E, SiR1_E, SiR2_E, SiR3_E;
   TTree *tNNUnfolding;

   TH1D *hSiT_t;
   TH1D *hSiT_E;
   TH1D *hSiL_tDiff;
   TH1D *hSiR_tDiff;
   TH2D *hSiL_EvdE;
   TH2D *hSiL_EvdE_proton;
   TH2D *hSiL_EvdE_deuteron;
   TH2D *hSiL_EvdE_triton;
   TH2D *hSiR_EvdE;
   TH2D *hSiR_EvdE_proton;
   TH2D *hSiR_EvdE_deuteron;
   TH2D *hSiR_EvdE_triton;

   TTreeReader     fReader;  //!the tree reader
   TTree          *fChain = 0;   //!pointer to the analyzed TTree or TChain

   // Readers to access the data (delete the ones you do not need).
   TTreeReaderValue<Int_t> runId = {fReader, "runId"};
   TTreeReaderValue<Int_t> blockId = {fReader, "blockId"};
   TTreeReaderValue<Int_t> TMEId = {fReader, "TMEId"};
   TTreeReaderValue<Double_t> TMEWindowWidth = {fReader, "TMEWindowWidth"};
//   TTreeReaderValue<string> centralMuonChannel = {fReader, "centralMuonChannel"}; //currently null data
   TTreeReaderValue<Int_t> centralMuonTPIID = {fReader, "centralMuonTPIID"};
   TTreeReaderValue<Double_t> centralMuonEnergy = {fReader, "centralMuonEnergy"};
   TTreeReaderValue<Double_t> centralMuonTime = {fReader, "centralMuonTime"};
   TTreeReaderValue<Double_t> timeToPrevTME = {fReader, "timeToPrevTME"};
   TTreeReaderValue<Double_t> timeToNextTME = {fReader, "timeToNextTME"};
   TTreeReaderValue<Bool_t> anyDoubleCountedPulses = {fReader, "anyDoubleCountedPulses"};
   TTreeReaderValue<vector<SimplePulse> > SiT_1 = {fReader, "SiT_1"};
   TTreeReaderValue<vector<SimplePulse> > SiT_2 = {fReader, "SiT_1"};
   TTreeReaderValue<vector<SimplePulse> > SiT_3 = {fReader, "SiT_1"};
   TTreeReaderValue<vector<SimplePulse> > SiT_4 = {fReader, "SiT_1"};
   TTreeReaderValue<vector<SimplePulse> > SiL1_2 = {fReader, "SiL1_2"};
   TTreeReaderValue<vector<SimplePulse> > SiL1_3 = {fReader, "SiL1_3"};
   TTreeReaderValue<vector<SimplePulse> > SiL1_4 = {fReader, "SiL1_4"};
   TTreeReaderValue<vector<SimplePulse> > SiL1_5 = {fReader, "SiL1_5"};
   TTreeReaderValue<vector<SimplePulse> > SiL1_6 = {fReader, "SiL1_6"};
   TTreeReaderValue<vector<SimplePulse> > SiL1_7 = {fReader, "SiL1_7"};
   TTreeReaderValue<vector<SimplePulse> > SiL1_8 = {fReader, "SiL1_8"};
   TTreeReaderValue<vector<SimplePulse> > SiL1_9 = {fReader, "SiL1_9"};
   TTreeReaderValue<vector<SimplePulse> > SiL1_10 = {fReader, "SiL1_10"};
   TTreeReaderValue<vector<SimplePulse> > SiL1_11 = {fReader, "SiL1_11"};
   TTreeReaderValue<vector<SimplePulse> > SiL1_12 = {fReader, "SiL1_12"};
   TTreeReaderValue<vector<SimplePulse> > SiL1_13 = {fReader, "SiL1_13"};
   TTreeReaderValue<vector<SimplePulse> > SiL1_14 = {fReader, "SiL1_14"};
   TTreeReaderValue<vector<SimplePulse> > SiL1_15 = {fReader, "SiL1_15"};
   TTreeReaderValue<vector<SimplePulse> > SiL3 = {fReader, "SiL3"};
   TTreeReaderValue<vector<SimplePulse> > SiR1_1 = {fReader, "SiR1_1"};
   TTreeReaderValue<vector<SimplePulse> > SiR1_2 = {fReader, "SiR1_2"};
   TTreeReaderValue<vector<SimplePulse> > SiR1_3 = {fReader, "SiR1_3"};
   TTreeReaderValue<vector<SimplePulse> > SiR1_4 = {fReader, "SiR1_4"};
   TTreeReaderValue<vector<SimplePulse> > SiR2 = {fReader, "SiR2"};
   TTreeReaderValue<vector<SimplePulse> > SiR3 = {fReader, "SiR3"};
   TTreeReaderValue<vector<SimplePulse> > GeLoGain = {fReader, "GeLoGain"};
   TTreeReaderValue<vector<SimplePulse> > GeHiGain = {fReader, "GeHiGain"};

   TMETree(TTree * /*tree*/ =0) { }
   virtual ~TMETree() { }
   virtual Int_t   Version() const { return 2; }
   virtual void    Begin(TTree *tree);
   virtual void    SlaveBegin(TTree *tree);
   virtual void    Init(TTree *tree);
   virtual Bool_t  Notify();
   virtual Bool_t  Process(Long64_t entry);
   virtual Int_t   GetEntry(Long64_t entry, Int_t getall = 0) { return fChain ? fChain->GetTree()->GetEntry(entry, getall) : 0; }
   virtual void    SetOption(const char *option) { fOption = option; }
   virtual void    SetObject(TObject *obj) { fObject = obj; }
   virtual void    SetInputList(TList *input) { fInput = input; }
   virtual TList  *GetOutputList() const { return fOutput; }
   virtual void    SlaveTerminate();
   virtual void    Terminate();

   Double_t LikelihoodProbabilityCut(Double_t X, Double_t mu, Double_t sigma);

   ClassDef(TMETree,0);

};

#endif

#ifdef TMETree_cxx
void TMETree::Init(TTree *tree)
{
   // The Init() function is called when the selector needs to initialize
   // a new tree or chain. Typically here the reader is initialized.
   // It is normally not necessary to make changes to the generated
   // code, but the routine can be extended by the user if needed.
   // Init() will be called many times when running on PROOF
   // (once per file to be processed).

   fReader.SetTree(tree);
}

Bool_t TMETree::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

#endif // #ifdef TMETree_cxx
