#include "../Al50/final/john_al_proton_e_reco_avg.C"

void SiVsAl() {

  double x_min = 2000;
  double x_max = 12000;

  TLegend* leg = new TLegend(0.5,0.5,0.80,0.70);
  leg->SetBorderSize(0);
  leg->SetTextSize(0.035);
  leg->SetFillColor(kWhite);
  
  // Si (protons)
  std::string filename = "~/data/results/Si16b/systematics_newPP_geq1TgtPulse_3sigma.root";
  TFile* file = new TFile(filename.c_str(), "READ");

  std::string particle = "proton";
  std::string Particle = "Proton";
  Int_t si_colour = kRed;

  std::string i_dirname = "FinalSystPlot_" + particle + "_TCutG";

  std::string i_statsystname = i_dirname + "/hFinalStatSyst";
  TGraphAsymmErrors* statsyst_spectrum = (TGraphAsymmErrors*) file->Get(i_statsystname.c_str());
  if (!statsyst_spectrum) {
    std::cout << "Error: Problem getting statsyst_spectrum " << i_statsystname << std::endl;
    return;
  }

  statsyst_spectrum->SetLineColor(si_colour);
  statsyst_spectrum->SetLineWidth(2);
    
  statsyst_spectrum->GetXaxis()->SetRangeUser(x_min, x_max);
  statsyst_spectrum->GetXaxis()->SetTitle("Energy [keV]");
  statsyst_spectrum->GetYaxis()->SetTitle("Protons / muon capture / keV");
  statsyst_spectrum->GetXaxis()->SetTitleOffset(1.0);
  statsyst_spectrum->GetYaxis()->SetTitleOffset(1.0);
    //    stat_spectrum->Draw("PE SAME");
  statsyst_spectrum->Draw("APE");

  TH1D* hAl = john_al_proton_e_reco_avg();
  hAl->Draw("E SAME");

  leg->AddEntry(statsyst_spectrum, "Si", "l");
  leg->AddEntry(hAl, "Al", "l");
  
  TLatex* latex = new TLatex();
  latex->SetTextAlign(22);
  latex->DrawLatexNDC(0.6, 0.8, "AlCap #bf{#it{Preliminary} }");
  latex->SetTextSize(0.035);
  latex->DrawLatexNDC(0.6, 0.75, "(stat.+syst. errors)");
  
  leg->Draw();
}
