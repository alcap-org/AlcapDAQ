#include "TROOT.h"
#include "TSystem.h"
#include "TStyle.h"
#include "TString.h"
#include "TChain.h"
#include "TFile.h"
#include "TTree.h"
#include "TH2F.h"

#include <vector>
#include <iostream>

using std::cout;
using std::endl;

double Adc2keV(double adc, double slope = 7.49, double constant = 29.04)
{
  return slope*adc + constant;
}

int main(int argc, char *argv[])
{
  //gROOT->ProcessLine(".L libAnalysis.so");
  gStyle->SetPalette(55);

  //TString data_path = "/home/nam/work/RunPSI2013/data/root/dq3_rootanahist/";
  TString data_path = "/home/nam/work/localRunPSI2013/data/root/dq3_rootanahist/";
  int firstRun =3763;
  int lastRun = 3770;
  int runNo = firstRun;

  TChain *chain = new TChain("MuEvt/mutree");
  for (runNo = firstRun; runNo <= lastRun; ++runNo)
  {
    TString data_file = data_path + Form("ranahist%.5d.root", runNo);
    if (gSystem->AccessPathName(data_file))
      continue;

    std::cout<<"Add run "<<runNo<< " into the chain ..."<<std::endl;
    chain->Add(data_file);
  }

  //chain->Draw("E_SiR2_S:(t_SiR2_S - t_muSc)>>h2(1024, -2000, 12000, 512, 0, 2500)",
     //"", "colz");
  //chain->Draw("E_SiR2_F:(t_SiR2_F - t_muSc)>>h2(1024, -2000, 12000, 512, 0, 2500)",
     //"", "colz");

  if (!chain)
  {
    std::cout<<"Zombie chain ...\n";
    return -1;
  }

  chain->SetMakeClass(1);
  double E_muSc, t_muSc;
  std::vector<double> *E_SiR2_S = NULL;
  std::vector<double> *t_SiR2_S = NULL;
  std::vector<double> *E_SiR2_F = NULL;
  std::vector<double> *t_SiR2_F = NULL;
  chain->SetBranchAddress("E_muSc", &E_muSc);
  chain->SetBranchAddress("t_muSc", &t_muSc);
  chain->SetBranchAddress("E_SiR2_S", &E_SiR2_S);
  chain->SetBranchAddress("t_SiR2_S", &t_SiR2_S);
  chain->SetBranchAddress("E_SiR2_F", &E_SiR2_F);
  chain->SetBranchAddress("t_SiR2_F", &t_SiR2_F);

  int ntbins = 2048;
  int nebins = 1024;
  int tlow = -4.e3;
  int thigh = ntbins*8 + tlow;
  TH2F * hSiR2S_muSc = new TH2F("hSiR2S_muSc", "hSiR2S_muSc", 
      ntbins, tlow, thigh,
      nebins, 0, 25.6e3);
  TH2F * hSiR2S_selfTdiff = new TH2F("hSiR2S_selfTdiff", "hSiR2S_selfTdiff",
      ntbins, 0, 14e3,
      nebins, 0, 25.6e3);
  TH2F * hSiR2F_muSc = new TH2F("hSiR2F_muSc", "hSiR2F_muSc", 
      ntbins, tlow, thigh,
      nebins, 0, 2500);
  TH2F * hSiR2F_selfTdiff = new TH2F("hSiR2F_selfTdiff", "hSiR2F_selfTdiff",
      ntbins, 0, 10.24e3,
      nebins, 0, 2500);

  long int nentries = chain->GetEntries();
  for (int i = 0; i < nentries; ++i)
  {
    if (i%(int)1e7 == 1)
    {
      printf("%4.2f %: ", 100*(float)i/nentries);
      std::cout<<i<<" events out of "<<nentries<<std::endl;
    }

    chain->GetEntry(i);
    for (unsigned int j = 0; j < E_SiR2_S->size(); ++j)
      hSiR2S_muSc->Fill(t_SiR2_S->at(j) - t_muSc, Adc2keV(E_SiR2_S->at(j)));

    for (unsigned int j = 0; j < E_SiR2_F->size(); ++j)
      hSiR2F_muSc->Fill(t_SiR2_F->at(j) - t_muSc, E_SiR2_F->at(j));

    if (t_SiR2_S->size()>1)
      for (unsigned int j = 1; j < t_SiR2_S->size(); ++j)
        hSiR2S_selfTdiff->Fill(t_SiR2_S->at(j) - t_SiR2_S->at(0),
            Adc2keV(E_SiR2_S->at(j)));

    if (t_SiR2_F->size()>1)
    {
      double E_1st_hit = E_SiR2_F->at(0);
      double t_1st_hist = t_SiR2_F->at(0);

      if (E_1st_hit>300 && E_1st_hit<600
          && (t_1st_hist - t_muSc)<170 && (t_1st_hist - t_muSc)>50)
      {
        for (unsigned int j = 1; j < t_SiR2_F->size(); ++j)
          hSiR2F_selfTdiff->Fill(t_SiR2_F->at(j) - t_SiR2_F->at(0), 
              E_SiR2_F->at(j));
      }
    }
  }

  TFile *of = new TFile("of.root", "recreate");
  hSiR2S_muSc->Write();
  hSiR2F_muSc->Write();
  hSiR2S_selfTdiff->Write();
  hSiR2F_selfTdiff->Write();
  of->Write();
  of->Close();

  return 0;
}
