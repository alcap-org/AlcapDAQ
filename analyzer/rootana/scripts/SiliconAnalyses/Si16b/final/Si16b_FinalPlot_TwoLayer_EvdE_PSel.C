#include "TCanvas.h"
#include "TLatex.h"
#include "TH2.h"
#include "TF2.h"
#include "TFile.h"
#include "scripts/Al50/psel.h"

std::vector<ParticleLikelihood::PSelPow> pls_r;
//Double_t pfcn_r(Double_t* x, Double_t* par) { return pls_r[0](x[1], x[0]); }

void Si16b_FinalPlot_TwoLayer_EvdE_PSel() {

  std::string infilename = "~/data/results/Si16b/plots_newPP.root";
  TFile* infile = new TFile(infilename.c_str(), "READ");

  std::string dirname = "all_SiR_timecut400_10000ns_layerCoinc";
  //  std::string dirname = "all_SiR_timecut400_10000ns_layerCoinc_wSiL1_14Coinc";
  std::string layer1 = "SiR1"; std::string layer2 = "SiR2"; std::string layer3 = "SiR3";
  //  std::string dirname = "all_SiR_nolayercoinc"; std::string layer1 = "SiR1"; std::string layer2 = "SiR2"; std::string layer3 = "SiR3";

  TCanvas* c_EvdE = new TCanvas("c_EvdE", "c_EvdE");
  std::string inhistname = dirname + "/hEvdE_TwoLayer_12not3";
  //  std::string inhistname = dirname + "/hEvdE_TwoLayer_123";
  TH2F* hEvdE = (TH2F*) infile->Get(inhistname.c_str());
  std::string histtitle = "Si16b Dataset, Right Arm (" + layer1 + " && " + layer2 + " && !" + layer3 + ")";
  hEvdE->SetTitle(histtitle.c_str());
  hEvdE->SetStats(false);
  hEvdE->GetXaxis()->SetRangeUser(0, 20000);
  hEvdE->GetYaxis()->SetRangeUser(0, 3000);
  //  hEvdE->GetYaxis()->SetRangeUser(0, 10000);
  hEvdE->Draw("COLZ");

  TLatex* latex = new TLatex();
  latex->DrawLatexNDC(0.55, 0.65, "AlCap Preliminary");

  latex->SetTextColor(kRed);
  latex->DrawLatex(1000, 1200, "#mu^{-}");
  latex->DrawLatex(1600, 2100, "p");
  latex->DrawLatex(2200, 2500, "d");
  latex->DrawLatex(2800, 2900, "t");
  latex->DrawLatex(8000, 7500, "#alpha");

  pls_r = ParticleLikelihood::LoadParticleLikelihoodsPow('r');
  //  TF2* fpr = new TF2("fpr", pfcn_r, 0., 20.e3, 0., 10.e3);
  //  Double_t fcont[1] = { 1.e-4 };
  //  fpr->SetNpx(200); fpr->SetNpy(200); fpr->SetContour(1, fcont);
  //  fpr->Draw("SAME");
  const int n_particles = 3;
  Int_t colours[n_particles] = {kRed, kCyan, kSpring};
  for (int i_pls = 0; i_pls < n_particles; ++i_pls) {
    TF1* lower = pls_r[i_pls].getFunc(0);
    lower->SetLineColor(colours[i_pls]);
    lower->Draw("LSAME");

    TF1* upper = pls_r[i_pls].getFunc(1);
    upper->SetLineColor(colours[i_pls]);
    upper->Draw("LSAME");
  }
}
