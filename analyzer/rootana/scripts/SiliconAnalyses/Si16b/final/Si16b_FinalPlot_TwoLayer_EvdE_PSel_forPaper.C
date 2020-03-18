#include "TCanvas.h"
#include "TLatex.h"
#include "TH2.h"
#include "TF2.h"
#include "TFile.h"
#include "../../..//Al50/psel.h"

std::vector<ParticleLikelihood::PSelPow> pls_r;
//Double_t pfcn_r(Double_t* x, Double_t* par) { return pls_r[0](x[1], x[0]); }

void Si16b_FinalPlot_TwoLayer_EvdE_PSel_forPaper(std::string outdir = "") {

  std::string infilename = "~/data/results/Si16b/plots_newPP_geq1TgtPulse.root";
  TFile* infile = new TFile(infilename.c_str(), "READ");

  std::string dirname = "all_SiR_timecut0_10000ns_layerCoinc";
  //  std::string dirname = "all_SiR_timecut400_10000ns_layerCoinc_wSiL1_14Coinc";
  std::string layer1 = "SiR1"; std::string layer2 = "SiR2"; std::string layer3 = "SiR3";
  //  std::string dirname = "all_SiR_nolayercoinc"; std::string layer1 = "SiR1"; std::string layer2 = "SiR2"; std::string layer3 = "SiR3";

  TCanvas* c_EvdE = new TCanvas("c_EvdE", "c_EvdE");
  std::string inhistname = dirname + "/hEvdE_TwoLayer_12not3";
  //  std::string inhistname = dirname + "/hEvdE_TwoLayer_123";
  TH2F* hEvdE = (TH2F*) infile->Get(inhistname.c_str());
  std::string histtitle = "Si16b Dataset, Right Arm (" + layer1 + " && " + layer2 + " && !" + layer3 + " && SiL1)";
  hEvdE->SetTitle(histtitle.c_str());
  hEvdE->SetStats(false);
  //  hEvdE->Rebin2D(5, 5);
  hEvdE->GetXaxis()->SetRangeUser(0, 20000);
  hEvdE->GetYaxis()->SetRangeUser(0, 10000);
  hEvdE->GetXaxis()->SetTitleOffset(1.0);
  hEvdE->GetYaxis()->SetTitleOffset(1.0);
  hEvdE->GetXaxis()->SetLabelSize(0.04);
  hEvdE->GetYaxis()->SetLabelSize(0.04);
  hEvdE->Draw();

  TLatex* latex = new TLatex();
  latex->SetTextColor(kBlack);
  latex->SetTextAlign(22);
  latex->SetTextSize(0.05);
  latex->DrawLatexNDC(0.6, 0.8, "AlCap #bf{#it{Preliminary}}");
  latex->SetTextSize(0.04);
  latex->DrawLatexNDC(0.6, 0.75, "Si Analysis");

  // latex->SetTextColor(kRed);
  // latex->DrawLatex(1000, 1200, "#mu^{-}");
  // latex->DrawLatex(1500, 2100, "p");
  // latex->DrawLatex(2100, 2500, "d");
  // latex->DrawLatex(2700, 2900, "t");
  // latex->DrawLatex(8000, 7500, "#alpha");

  bool is_si16b_analysis = true;
  pls_r = ParticleLikelihood::LoadParticleLikelihoodsPow('r', is_si16b_analysis);
  //  TF2* fpr = new TF2("fpr", pfcn_r, 0., 20.e3, 0., 10.e3);
  //  Double_t fcont[1] = { 1.e-4 };
  //  fpr->SetNpx(200); fpr->SetNpy(200); fpr->SetContour(1, fcont);
  //  fpr->Draw("SAME");
  const int n_particles = 4;
  Int_t colours[n_particles] = {kRed, kCyan, kMagenta, kSpring};
  for (int i_pls = 0; i_pls < n_particles; ++i_pls) {
    TF1* lower = pls_r[i_pls].getFunc(0);
    lower->SetLineColor(colours[i_pls]);

    TF1* upper = pls_r[i_pls].getFunc(1);
    upper->SetLineColor(colours[i_pls]);
    
    double minE = 0;
    double maxE = 0;
    pls_r[i_pls].getFunc(0)->GetRange(minE, maxE);
    std::cout << minE << " " << maxE << std::endl;
    std::cout << pls_r[i_pls].getFunc(0)->Eval(minE) << std::endl;
    std::cout << pls_r[i_pls].getFunc(0)->GetMaximum() << std::endl;
    TLine* minE_line = new TLine(minE, pls_r[i_pls].getFunc(0)->Eval(minE), minE, pls_r[i_pls].getFunc(1)->Eval(minE));
    minE_line->SetLineWidth(2);
    minE_line->SetLineColor(colours[i_pls]);
    minE_line->Draw("LSAME");

    TLine* maxE_line = new TLine(maxE, pls_r[i_pls].getFunc(0)->Eval(maxE), maxE, pls_r[i_pls].getFunc(1)->Eval(maxE));
    maxE_line->SetLineWidth(2);
    maxE_line->SetLineColor(colours[i_pls]);
    maxE_line->Draw("LSAME");

    lower->DrawF1(minE-100, maxE+100, "LSAME");
    upper->DrawF1(minE-100, maxE+100, "LSAME");

  }

  if (outdir != "") {
    std::string savename = outdir + "AlCapData_Si16bDataset_TwoLayer_EvdE_PSel_forPaper";
    std::string pngname = savename + ".png";
    c_EvdE->SaveAs(pngname.c_str());
  }
}
