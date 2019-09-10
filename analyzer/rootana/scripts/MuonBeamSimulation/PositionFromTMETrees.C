#include <iostream>
#include <sstream>
#include <iomanip>

#include "TFile.h"
#include "TH1.h"
#include "TF1.h"
#include "TLegend.h"
#include "TTree.h"
#include "TH2.h"
#include "TCanvas.h"
#include "TLine.h"
#include "TTreePlayer.h"

//extern TDirectory* gDirectory;

struct RunInfo {
  RunInfo(int run, bool is_vert, bool is_vac, int ang, double sf, int n_blocks, Int_t col, std::string leglab, double fit_low, double fit_high) : 
    run_number(run), is_vertical_scan(is_vert), is_in_vacuum(is_vac), angle(ang), scale_factor(sf), n_midas_blocks(n_blocks), colour(col), leglabel(leglab),
    fit_range_low(fit_low), fit_range_high(fit_high) { }

  int run_number;
  bool is_vertical_scan;
  bool is_in_vacuum;
  int angle;
  double scale_factor;
  int n_midas_blocks;
  Int_t colour;
  std::string leglabel;
  double fit_range_low;
  double fit_range_high;
};
  
void PositionFromTMETrees() {
  bool is_vert = true; bool is_horiz = false;
  bool in_air = false; bool in_vac = true;

  /*  // Comparison to old work
  double time_cut = 20000;
  double min_energy_cut = 400;
  double max_energy_cut = 10000;
  RunInfo run10404(10404, is_horiz, in_vac, 45, 1.02, 4548, kBlack, "Run 10404, H, Vac, 45^{#circ}, SF=1.02", 2, 16);
  RunInfo run10465(10465, is_horiz, in_air, 45, 1.05, 1180, kRed, "Run 10465, H, Air, 45^{#circ}, SF=1.05", 2, 16);
  RunInfo run10473(10473, is_horiz, in_air, 90, 1.05, 1749, kBlue, "Run 10473, H, Air, 90^{#circ}, SF=1.05", 2, 16);
  RunInfo run10475(10475, is_vert,  in_air, 90, 1.05, 2858, kMagenta, "Run 10475, V, Air, 90^{#circ}, SF=1.05", 2, 16);
  RunInfo run10477(10477, is_vert,  in_vac, 90, 1.02, 2361, kSpring-5, "Run 10477, V, Vac, 90^{#circ}, SF=1.02", 2, 16);
  */
  double time_cut = 200;
  double min_energy_cut = 200;
  double max_energy_cut = 1200;
  
  RunInfo run10404(10404, is_horiz, in_vac, 45, 1.02, 4548, kBlack, "Run 10404, H, Vac, 45^{#circ}, SF=1.02", 2, 15);
  RunInfo run10465(10465, is_horiz, in_air, 45, 1.05, 1180, kRed, "Run 10465, H, Air, 45^{#circ}, SF=1.05", 2, 15);
  RunInfo run10473(10473, is_horiz, in_air, 90, 1.05, 1749, kBlue, "Run 10473, H, Air, 90^{#circ}, SF=1.05", 2, 15);
  RunInfo run10475(10475, is_vert,  in_air, 90, 1.05, 2858, kMagenta, "Run 10475, V, Air, 90^{#circ}, SF=1.05", 2, 15);
  RunInfo run10477(10477, is_vert,  in_vac, 90, 1.02, 2361, kSpring-5, "Run 10477, V, Vac, 90^{#circ}, SF=1.02", 2, 15);
  RunInfo run9740(9740, is_horiz,  in_vac, 45, 1.02, 1, kSpring-5, "Run 9740, H, Vac, 45^{#circ}, SF=1.02", 2, 15);
  RunInfo run9741(9741, is_horiz,  in_vac, 45, 1.03, 1, kSpring-5, "Run 9741, H, Vac, 45^{#circ}, SF=1.03", 2, 15);
  RunInfo run9742(9742, is_horiz,  in_vac, 45, 1.04, 1, kSpring-5, "Run 9742, H, Vac, 45^{#circ}, SF=1.04", 2, 15);
  RunInfo run9743(9743, is_horiz,  in_vac, 45, 1.05, 1, kSpring-5, "Run 9743, H, Vac, 45^{#circ}, SF=1.05", 2, 15);
  
  std::vector<RunInfo*> runs;
  //  runs.push_back(&run10404);
  //  runs.push_back(&run10465);
  //  runs.push_back(&run10473);
  //  runs.push_back(&run10475);
  //  runs.push_back(&run10477);
  runs.push_back(&run9740);
  runs.push_back(&run9741);
  runs.push_back(&run9742);
  runs.push_back(&run9743);

  TLegend* leg = new TLegend(0.09,0.64,0.29,0.88);
  leg->SetBorderSize(0);
  leg->SetTextSize(0.03);
  leg->SetFillColor(kWhite);

  TCanvas* c0 = new TCanvas();

  std::stringstream filename;
  std::stringstream label;
  std::stringstream outfilename;
  for (std::vector<RunInfo*>::const_iterator i_run = runs.begin(); i_run != runs.end(); ++i_run) {
    c0->cd();
    std::cout << "Run " << (*i_run)->run_number << ":" << std::endl;

    filename.str("");
    if ((*i_run)->run_number > 10000) {
      filename << "~/data/out/local/out" << (*i_run)->run_number << "_tme-tree.root";
    }
    else {
      filename << "~/data/out/v10/Si16a.root";
    }
    TFile* file = new TFile(filename.str().c_str(), "READ");
    if ( file->IsZombie() ) {
      std::cout << "Problems opening file " << filename.str() << std::endl;
      continue;
    }
    TTree* tmetree = (TTree*) file->Get("TMETree/TMETree");
    if (!tmetree) {
      std::cout << "Problems getting tmetree" << std::endl;
      continue;
    }
    
    const int n_channels = 16;
    const int n_active_channels = 15;
    TH1F* hPosition = new TH1F("hPosition", "Position", n_channels,1,n_channels+1);
    hPosition->SetXTitle("Strip Number");
    
    double min_energy = 0;
    double max_energy = 5000;//10000;
    double energy_width = 100;//50;
    int n_energy_bins = (max_energy - min_energy) / energy_width;
    TH2F* hEnergy = new TH2F("hEnergy", "Energy", n_channels,1,n_channels+1, n_energy_bins,min_energy,max_energy);
    std::stringstream histtitle;
    histtitle << "Run " << (*i_run)->run_number << ", Energy vs Strip Number";
    hEnergy->SetTitle(histtitle.str().c_str());
    hEnergy->SetXTitle("Strip Number");
    hEnergy->SetYTitle("Energy [keV]");
    
    std::stringstream drawcmd, cutcmd;
    int start_channel = 0;
    if ( (*i_run)->run_number < 10000) {
      start_channel = 1;
    }
    for (int i_channel = start_channel; i_channel < n_active_channels; ++i_channel) {
      cutcmd.str("");
      cutcmd << "abs(SiL1_" << i_channel+1 << ".tTME)<" << time_cut;
      cutcmd << " && SiL1_" << i_channel+1 << ".E>" << min_energy_cut << " && SiL1_" << i_channel+1 << ".E<" << max_energy_cut;
      //      cutcmd << " && SiL1_" << i_channel+1 << ".bit_mask==1";
      cutcmd << " && timeToPrevTME>10000 && timeToNextTME>10000";
      cutcmd << " && runId==" << (*i_run)->run_number;
      hPosition->SetBinContent(i_channel+1, tmetree->GetEntries(cutcmd.str().c_str()));
      
      drawcmd.str("");
      drawcmd << "SiL1_" << i_channel+1 << ".E:" << i_channel+1 << ">>+hEnergy";
      cutcmd.str("");
      cutcmd << "abs(SiL1_" << i_channel+1 << ".tTME)<" << time_cut << " && runId==" << (*i_run)->run_number;;
      tmetree->Draw(drawcmd.str().c_str(), cutcmd.str().c_str(), "goff");
    }
    
    histtitle.str("");
    //    histtitle << "abs(SiL1.tTME)<" << time_cut << " ns && " << min_energy_cut << "<SiL1.E<" << max_energy_cut << " keV && SiL1.bit_mask==1";
    histtitle << "Beam Profiles (Data)";
    hPosition->SetTitle(histtitle.str().c_str());
    hPosition->GetYaxis()->SetRangeUser(0, 70000);
    hPosition->SetLineColor((*i_run)->colour);
    hPosition->SetStats(false);

    std::string fit_fn_name = "gaus";
    std::cout << "AE: " << hPosition->GetEntries() << std::endl;
    hPosition->Fit(fit_fn_name.c_str(), "Q0", "", (*i_run)->fit_range_low, (*i_run)->fit_range_high);
    hPosition->GetFunction(fit_fn_name.c_str())->SetLineColor((*i_run)->colour);
    double mean = hPosition->GetFunction(fit_fn_name.c_str())->GetParameter(1);
    double mean_error = hPosition->GetFunction(fit_fn_name.c_str())->GetParError(1);
    double sigma = hPosition->GetFunction(fit_fn_name.c_str())->GetParameter(2);
    double sigma_error = hPosition->GetFunction(fit_fn_name.c_str())->GetParError(2);
    double chi_sq = hPosition->GetFunction(fit_fn_name.c_str())->GetChisquare();
    int ndf = hPosition->GetFunction(fit_fn_name.c_str())->GetNDF();
    std::cout << "Fit: mean = " << mean << " +/- " << mean_error << ", sigma = " << sigma << " +/- " << sigma_error << ", chi^2 / ndf = " << chi_sq << " / " << ndf << " = " << chi_sq / ndf << std::endl;

    hPosition->Draw("HIST E SAMES");
    hPosition->GetFunction(fit_fn_name.c_str())->Draw("LSAME");

    label.str("");
    label << (*i_run)->leglabel.c_str() << std::fixed << std::setprecision(2) << ", mean = " << mean << " #pm " << mean_error << ", #sigma = " << sigma << " #pm " << sigma_error << " #chi^{2} / ndf = " << chi_sq << " / " << ndf << " = " << chi_sq/ndf;
    leg->AddEntry(hPosition, label.str().c_str(), "l");

    std::cout << std::endl;

    /*
    TCanvas* c1 = new TCanvas();
    hPosition->Draw("HIST E SAME");
    */    
    TCanvas* c2 = new TCanvas();
    hEnergy->SetStats(false);
    hEnergy->Draw("COLZ");

    TLine* min_energy_cut_line = new TLine(1, min_energy_cut, n_channels+1, min_energy_cut);
    min_energy_cut_line->SetLineColor(kRed);
    min_energy_cut_line->SetLineWidth(2);
    min_energy_cut_line->Draw("LSAME");
    TLine* max_energy_cut_line = new TLine(1, max_energy_cut, n_channels+1, max_energy_cut);
    max_energy_cut_line->SetLineColor(kRed);
    max_energy_cut_line->SetLineWidth(2);
    max_energy_cut_line->Draw("LSAME");

    outfilename.str("");
    outfilename << "~/data/results/";
    if ((*i_run)->run_number > 10000) {
      outfilename << "BeamPos";
    }
    else {
      outfilename << "Si16a";
    }
    outfilename << "/positions" << std::fixed << std::setw(5) << std::setfill('0') << (*i_run)->run_number << ".root";
    TFile* outfile = new TFile(outfilename.str().c_str(), "RECREATE");
    hPosition->Write();
    outfile->Write();
    outfile->Close();

    //    file->Close();
    //    delete file;
  }
  c0->cd();
  leg->Draw();
}
