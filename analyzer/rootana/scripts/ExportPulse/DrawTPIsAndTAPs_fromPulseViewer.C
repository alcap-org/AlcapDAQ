#include "TFile.h"
#include "TH1.h"
#include "TCanvas.h"
#include "TDirectory.h"
#include "TList.h"
#include "TRandom3.h"

#include <iostream>

// This macro draws 4 random TPIs and TAPs that were exported using ExportPulse in rootana
bool DrawTPIsAndTAPs_fromPulseViewer_file(TFile* file, std::string pulse_name, TCanvas& c1, std::string histtitle) {
// void DrawTPIsAndTAPs_fromPulseViewer(std::string filename, int seed = 0) {
//   TFile* file = new TFile(filename.c_str(), "READ");
//   if (file->IsZombie()) {
//     std::cout << "Problem opening file " << filename << std::endl;
//     return;
//   }

//   int n_pulses_to_draw = 4;
//   TCanvas* c1 = new TCanvas("c1", "c1");
//  c1->Divide(n_pulses_to_draw/2, n_pulses_to_draw/2);

  c1.SetFillColor(kWhite);
  
  TDirectory* tpi_dir = file->GetDirectory("ExportPulse/TPIs");
  // TList* tpi_hists = tpi_dir->GetListOfKeys();
  // int n_max_hists = tpi_hists->GetSize();

  // TRandom3 random(seed);

  // for (int i_hist = 0; i_hist < n_pulses_to_draw; ++i_hist) {
  //   c1->cd(i_hist+1);

  //   int rndm = (random.Rndm())*n_max_hists;

  //   std::string pulse_name = tpi_hists->At(rndm)->GetName();
    std::string tpi_histname = "ExportPulse/TPIs/" + pulse_name;
 
    TH1F* hTPI = (TH1F*) file->Get(tpi_histname.c_str());
    if (!hTPI) {
      std::cout << "Problem obtaining TPI histogram " << tpi_histname << std::endl;
      return false;
    }
    hTPI->SetLineWidth(2);
    hTPI->SetLineColor(kBlack);
    hTPI->GetXaxis()->SetTitleSize(0.05);
    hTPI->GetXaxis()->SetLabelSize(0.05);
    hTPI->GetXaxis()->SetTitleOffset(1.0);
    hTPI->GetYaxis()->SetTitleSize(0.05);
    hTPI->GetYaxis()->SetLabelSize(0.05);
    hTPI->GetYaxis()->SetTitleOffset(1.07);

    
    std::string tap_histname = "ExportPulse/TAPs/" + pulse_name + "_AP";
    TH1F* hTAP = (TH1F*) file->Get(tap_histname.c_str());
    if (!hTAP) {
      std::cout << "Problem obtaining TAP histogram " << tap_histname << std::endl;
      return false;
    }
    hTAP->SetLineColor(kRed);
    
    hTPI->Draw("HIST");

    TLatex* latex = new TLatex();
    latex->SetTextAlign(22);
    latex->SetTextSize(0.03);
    
    double amplitude = hTAP->GetMaximum(); // pulse amplitude
    double max_peak_pos = hTPI->GetXaxis()->GetBinCenter(hTPI->GetMaximumBin()); // maximum bin
    double time = hTAP->GetXaxis()->GetBinCenter(hTAP->GetMaximumBin()); // pulse time
    double cf_amp = hTPI->GetBinContent(hTAP->GetMaximumBin()); // pulse height at time

    double ped_line_x1 = 0; double ped_line_y1 = 0;
    double ped_line_x2 = 5000; double ped_line_y2 = 0;
    TLine* ped_line = new TLine(ped_line_x1, ped_line_y1, ped_line_x2, ped_line_y2);
    ped_line->SetLineWidth(2);
    ped_line->SetLineStyle(kDashed);
    ped_line->SetLineColor(kRed);
    ped_line->Draw("LSAME");

    double text_x = 500;
    double text_y = 50;
    latex->SetTextColor(kRed);
    latex->SetTextSize(0.05);
    latex->DrawLatex(text_x, text_y, "pedestal");

    double amp_line_x1 = 0; double amp_line_y1 = amplitude;
    double amp_line_x2 = 3000; double amp_line_y2 = amplitude;
    TLine* amp_line = new TLine(amp_line_x1, amp_line_y1, amp_line_x2, amp_line_y2);
    amp_line->SetLineWidth(2);
    amp_line->SetLineStyle(kDashed);
    amp_line->SetLineColor(kRed);
    amp_line->Draw("LSAME");

    double amp_arrow_x1 = amp_line_x2; double amp_arrow_y1 = ped_line_y1;
    double amp_arrow_x2 = amp_line_x2; double amp_arrow_y2 = amp_line_y2;
    TArrow* amp_arrow = new TArrow(amp_arrow_x1, amp_arrow_y1, amp_arrow_x2, amp_arrow_y2,0.01, "<>");
    amp_arrow->SetLineWidth(1);
    amp_arrow->SetLineStyle(kSolid);
    amp_arrow->SetLineColor(kDashed);
    amp_arrow->SetLineColor(kBlack);
    amp_arrow->Draw();

    latex->SetTextAlign(12);
    text_x = amp_arrow_x2 + 50;
    text_y = amp_arrow_y1 + (amp_arrow_y2 - amp_arrow_y1)/2;
    latex->SetTextColor(kBlack);
    latex->SetTextSize(0.05);
    latex->DrawLatex(text_x, text_y, "pulse amplitude");

    double cf_line_x1 = 0; double cf_line_y1 = cf_amp;
    double cf_line_x2 = amp_line_x2-1750; double cf_line_y2 = cf_amp;
    TLine* cf_line = new TLine(cf_line_x1, cf_line_y1, cf_line_x2, cf_line_y2);
    cf_line->SetLineWidth(2);
    cf_line->SetLineStyle(kDashed);
    cf_line->SetLineColor(kRed);
    cf_line->Draw("LSAME");

    double cf_arrow_x1 = cf_line_x2; double cf_arrow_y1 = ped_line_y1;
    double cf_arrow_x2 = cf_line_x2; double cf_arrow_y2 = cf_line_y2;
    TArrow* cf_arrow = new TArrow(cf_arrow_x1, cf_arrow_y1, cf_arrow_x2, cf_arrow_y2,0.01, "<>");
    cf_arrow->SetLineWidth(1);
    cf_arrow->SetLineStyle(kSolid);
    cf_arrow->SetLineColor(kDashed);
    cf_arrow->SetLineColor(kBlack);
    cf_arrow->Draw();

    text_x = cf_arrow_x2 + 50;
    text_y = cf_arrow_y1 + (cf_arrow_y2 - cf_arrow_y1)/2;
    latex->DrawLatex(text_x, text_y, "20% pulse amplitude");

    double time_arrow_x1 = time; double time_arrow_y1 = -75;
    double time_arrow_x2 = time; double time_arrow_y2 = cf_amp;
    TArrow* time_arrow = new TArrow(time_arrow_x1, time_arrow_y1, time_arrow_x2, time_arrow_y2, 0.01, "<");
    time_arrow->SetLineWidth(2);
    time_arrow->SetLineStyle(kSolid);
    time_arrow->SetLineColor(kBlue);
    time_arrow->Draw();

    text_x = time_arrow_x2 - 45;// + 50;
    text_y = time_arrow_y2 + 50;//-75/2;//time_arrow_y1 + (time_arrow_y2 - time_arrow_y1)/2;
    latex->SetTextColor(kBlue);
    latex->SetTextAlign(22);
    latex->DrawLatex(text_x, text_y, "pulse time");

    latex->SetTextAlign(22);
    latex->SetTextSize(0.07);
    latex->SetTextColor(kBlack);
    latex->DrawLatex(4000, 1200, "Standard");
    latex->DrawLatex(4000, 1050, "Pulse Analysis");
    
    
    //    hTAP->Draw("SAME");
    return true;
}
