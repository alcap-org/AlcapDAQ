#include <iostream>
#include <sstream>
#include <iomanip>

#include "TFile.h"
#include "TH1.h"
#include "TF1.h"
#include "TLegend.h"

struct RunInfo {
  RunInfo(int run, bool is_vert, bool is_vac, int ang, double sf, int n_blocks, Int_t col, std::string leglab) : 
    run_number(run), is_vertical_scan(is_vert), is_in_vacuum(is_vac), angle(ang), scale_factor(sf), n_midas_blocks(n_blocks), colour(col), leglabel(leglab) { }

  int run_number;
  bool is_vertical_scan;
  bool is_in_vacuum;
  int angle;
  double scale_factor;
  int n_midas_blocks;
  Int_t colour;
  std::string leglabel;
};
  

void XYProfile_Si16a() {
  bool is_vert = true; bool is_horiz = false;
  bool in_air = false; bool in_vac = true;

  RunInfo run09743(9743, is_horiz, in_vac, 45, 1.02, 4548, kBlack, "Run 9743, H, Vac, 45^{#circ}, SF=1.05");
  
  std::vector<RunInfo*> runs;
  runs.push_back(&run09743);

  TLegend* leg = new TLegend(0.09,0.64,0.29,0.88);
  leg->SetBorderSize(0);
  leg->SetTextSize(0.03);
  leg->SetFillColor(kWhite);

  std::stringstream filename;
  std::stringstream label;
  for (std::vector<RunInfo*>::const_iterator i_run = runs.begin(); i_run != runs.end(); ++i_run) {
    std::cout << "Run " << (*i_run)->run_number << ":" << std::endl;

    filename.str("");
    filename << "out" << std::setw(5) << std::setfill('0') << (*i_run)->run_number << "_si16-scan.root";
    TFile* file = new TFile(filename.str().c_str());
    if ( file->IsZombie() ) {
      std::cout << "Problems opening file " << filename.str() << std::endl;
      continue;
    }

    std::string histname = "PlotTAP_NMuons_Si16/hNMuonsPerStrip";
    TH1F* hNMuonsPerStrip = (TH1F*) file->Get(histname.c_str());
    if ( !hNMuonsPerStrip ) {
      std::cout << "Problems finding " << histname << std::endl;
      continue;
    }
    double integral = hNMuonsPerStrip->Integral();
    std::cout << "N muons = " << integral << std::endl;
    //    hNMuonsPerStrip->Scale(1.0/integral);
    hNMuonsPerStrip->SetStats(false);
    hNMuonsPerStrip->SetMaximum(80000);
    hNMuonsPerStrip->SetLineColor((*i_run)->colour);

    hNMuonsPerStrip->Fit("gaus", "Q0", "", 2, 16);
    hNMuonsPerStrip->GetFunction("gaus")->SetLineColor((*i_run)->colour);
    double mean = hNMuonsPerStrip->GetFunction("gaus")->GetParameter(1);
    double mean_error = hNMuonsPerStrip->GetFunction("gaus")->GetParError(1);
    double sigma = hNMuonsPerStrip->GetFunction("gaus")->GetParameter(2);
    double sigma_error = hNMuonsPerStrip->GetFunction("gaus")->GetParError(2);
    double chi_sq = hNMuonsPerStrip->GetFunction("gaus")->GetChisquare();
    int ndf = hNMuonsPerStrip->GetFunction("gaus")->GetNDF();
    std::cout << "Fit: mean = " << mean << " +/- " << mean_error << ", sigma = " << sigma << " +/- " << sigma_error << ", chi^2 / ndf = " << chi_sq << " / " << ndf << " = " << chi_sq / ndf << std::endl;

    hNMuonsPerStrip->Draw("HIST E SAMES");
    hNMuonsPerStrip->GetFunction("gaus")->Draw("LSAME");

    label.str("");
    label << (*i_run)->leglabel.c_str() << std::fixed << std::setprecision(2) << ", mean = " << mean << " #pm " << mean_error << ", #sigma = " << sigma << " #pm " << sigma_error << " #chi^{2} / ndf = " << chi_sq << " / " << ndf << " = " << chi_sq/ndf;
    leg->AddEntry(hNMuonsPerStrip, label.str().c_str(), "l");

    std::cout << std::endl;
  }
  leg->Draw();
  /*

  double error = 0;
  double n_muons = hNMuonsPerStrip->IntegralAndError(1, 15, error);
  std::cout << "N_muons (strips 1 to 15) = " << n_muons << " +/- " << error << " (stat.)" << std::endl;

  std::cout << "N_muons (strips 1 to 15, from fit) = " << gaus->Integral(1, 16) << std::endl;
  std::cout << "N_muons (strips 1 to 16, from fit) = " << gaus->Integral(1, 17) << std::endl;
  */
}
