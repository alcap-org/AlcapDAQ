#include "RooUnfoldResponse.h"

void Al50_FinalPlot_DeadLayerSystematic(std::string savedir = "") {

  // const int n_settings = 2;
  // std::string settings[n_settings] = {"", "_10umSiR3DL"};//"_doubleSiR3DL"};
  // Int_t colours[n_settings] = {kBlack, kRed};
  // std::string leglabels[n_settings] = {"SiR3 front dead-layer = 0.5 #mum", "SiR3 front dead-layer = 10 #mum"};

  
  const int n_settings = 3;
  std::string settings[n_settings] = {"", "_left200keV", "_right200keV"};
  Int_t colours[n_settings] = {kBlack, kRed, kBlue};
  std::string leglabels[n_settings] = {"nominal", "shift left 200 keV", "shift right 200 keV"};

  // const int n_settings = 7;
  // std::string settings[n_settings] = {"_doubleSiR3DL_0-5umDL", "_doubleSiR3DL_1umDL", "_doubleSiR3DL_5umDL", "_doubleSiR3DL_10umDL", "_doubleSiR3DL_25umDL", "_doubleSiR3DL_50umDL", "_doubleSiR3DL_100umDL"};
  // Int_t colours[n_settings] = {kRed,
  // 			       kBlack,
  // 			       kMagenta,
  // 			       kCyan,
  // 			       kSpring+3,
  // 			       kGray+2,
  // 			       kGreen
  // };
  // std::string leglabels[n_settings] = {"MC (0.5 #mum DL)",
  // 				       "MC (1 #mum DL)",
  // 				       "MC (5 #mum DL)",
  // 				       "MC (10 #mum DL)",
  // 				       "MC (25 #mum DL)",
  // 				       "MC (50 #mum DL)",
  // 				       "MC (100 #mum DL)"};

  TH1F* hMeasureds[n_settings];
  TLegend* leg = new TLegend(0.30,0.2,0.7,0.4);
  leg->SetBorderSize(0);
  leg->SetTextSize(0.035);
  leg->SetFillColor(kWhite);

  std::string armname = "SiR";
  TCanvas* c1 = new TCanvas();
  for (int i_setting = 0; i_setting < n_settings; ++i_setting) {
    std::string setting = settings[i_setting];
    Int_t colour = colours[i_setting];
    
    //    std::string infilename = "/data/R15bMC/respMatrix_100M_Al50_proton_100keVBins" + setting + ".root";
    std::string infilename = "~/g4sim/alcap/respMatrix_100M_Al50_proton_100keVBins" + setting + ".root";
    std::string responsename = armname + "_three_layer_response";
  
    TFile* infile = new TFile(infilename.c_str(), "READ");
    RooUnfoldResponse* response = (RooUnfoldResponse*) infile->Get(responsename.c_str());

    TH1F* hMeasured = (TH1F*) response->Hmeasured();
    hMeasured->Rebin(10);
    hMeasured->SetLineWidth(2);
    hMeasured->SetLineColor(colour);
    //    hMeasured->SetMaximum(hMeasured->GetMaximum()*1.5);
    hMeasured->Draw("HIST E SAME");
    hMeasured->SetYTitle("Count / 1 MeV");
    hMeasured->SetXTitle("Observed Energy [MeV]");
    leg->AddEntry(hMeasured, leglabels[i_setting].c_str(), "l");
    hMeasureds[i_setting] = hMeasured;
  }
  leg->Draw();

  TCanvas* c2 = new TCanvas();
  for (int i_setting = 1; i_setting < n_settings; ++i_setting) {
    TH1F* hRatio = (TH1F*) hMeasureds[0]->Clone("hRatio");
    hRatio->Divide(hMeasureds[i_setting]);
    hRatio->SetMaximum(2);
    hRatio->SetLineColor(colours[i_setting]);
    hRatio->Draw("SAME");
  }
  leg->Draw();
  //  TLatex* latex = new TLatex();
  //  latex->DrawLatexNDC(0.55, 0.45, "AlCap Monte Carlo");
}
