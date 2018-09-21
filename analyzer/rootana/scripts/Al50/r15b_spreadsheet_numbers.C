#include "TFile.h"
#include "TH1.h"
#include "TTree.h"

#include <iostream>

void r15b_spreadsheet_numbers(const char* fpselname, const char* fenname, int tcut) {
  TFile* fpsel = new TFile(fpselname);
  TFile* fen   = new TFile(fenname);
  char prot_tcut[16];
  sprintf(prot_tcut, "t>%d", tcut);

  TTree* PID_RP = (TTree*)fpsel->Get("PID_RP");
  TTree* PID_LP = (TTree*)fpsel->Get("PID_LP");
  TH1* hren     = (TH1*)fen    ->Get("hr_e_reco");
  TH1* hlen     = (TH1*)fen    ->Get("hl_e_reco");
  std::cout << "Right raw, no tcut: \t" << PID_RP->GetEntries()                      << std::endl;
  std::cout << "Left raw, no tcut:  \t" << PID_LP->GetEntries()                      << std::endl;
  std::cout << "Right raw, tcut:    \t" << PID_RP->GetEntries(prot_tcut)             << std::endl;
  std::cout << "Left raw, tcut:     \t" << PID_LP->GetEntries(prot_tcut)             << std::endl;
  std::cout << "Right Reco 0-10 MeV:\t" << hren->Integral(0, hren->FindFixBin(10e3)) << std::endl;
  std::cout << "Left Reco 0-10 MeV: \t" << hlen->Integral(0, hlen->FindFixBin(10e3)) << std::endl;
}
