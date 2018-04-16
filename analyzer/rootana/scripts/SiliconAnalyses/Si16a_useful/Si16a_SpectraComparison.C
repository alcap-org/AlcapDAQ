#include <sstream>

void Si16a_SpectraComparison() {

  std::string basename = "~/data/results/Si16a_useful/plots";

  TLegend* leg = new TLegend(0.44,0.44,0.69,0.78);
  leg->SetBorderSize(0);
  leg->SetTextSize(0.03);
  leg->SetFillColor(kWhite);
  
  const int n_runs = 8;
  int run_numbers[n_runs] = {9735, 9736, 9737, 9739, 9740, 9741, 9742, 9743};
  double scale_factors[n_runs] = {1.035, 0.99, 0.98, 1.01, 1.02, 1.03, 1.04, 1.05};

  const int n_hists = 3;
  //  std::string histnames[n_hists] = {"hTarget_All", "hTarget_woutSiL3", "hTarget_wSiL3"};
  //  std::string labels[n_hists] = {"All", "with no SiL3 hit", "with an SiL3 hit"};
  std::string histnames[n_hists] = {"hThick_All", "hThick_woutSiL1", "hThick_wSiL1"};
  std::string labels[n_hists] = {"All", "with no SiL1 hit", "with an SiL1 hit"};
  Int_t colours[n_hists] = {kBlack, kRed, kBlue};
  std::stringstream run_str, histtitle;
  for (int i_run = 0; i_run < n_runs; ++i_run) {
    leg->Clear();
    
    run_str.str("");
    run_str << "0" << run_numbers[i_run];

    std::string canvasname = "c_" + run_str.str();
    TCanvas* c = new TCanvas(canvasname.c_str(), canvasname.c_str());
    
    std::string filename = basename + run_str.str() + ".root";
    
    TFile* i_file = new TFile(filename.c_str(), "READ");

    for (int i_hist = 0; i_hist < n_hists; ++i_hist) {
      std::string i_histname = histnames[i_hist];
      std::string newhistname = i_histname + run_str.str();

      TH2F* hTarget2D = (TH2F*) i_file->Get(i_histname.c_str());
      int min_bin = hTarget2D->GetXaxis()->FindBin(-200);
      int max_bin = hTarget2D->GetXaxis()->FindBin(200);
      TH1D* hTarget = hTarget2D->ProjectionY(newhistname.c_str(), min_bin, max_bin);
      hTarget->SetYTitle("Raw Count");
      hTarget->SetLineWidth(2);
      hTarget->SetStats(false);
      hTarget->SetDirectory(0);
      //      hTarget->Scale(1.0 / hTarget->Integral());
      hTarget->SetLineColor(colours[i_hist]);
      //      hTarget->GetYaxis()->SetRangeUser(0, 0.12);
      hTarget->Draw("SAME");

      histtitle.str("");
      //      histtitle << "SiL1 Energy Spectrum (Run " << run_numbers[i_run] << ", SF = " << scale_factors[i_run] << ")";
      histtitle << "SiL3 Energy Spectrum (Run " << run_numbers[i_run] << ", SF = " << scale_factors[i_run] << ")";
      hTarget->SetTitle(histtitle.str().c_str());
      
      leg->AddEntry(hTarget, labels[i_hist].c_str(), "l");
    }

    leg->Draw();
    
    //    std::string pdfname = "~/plots/2018-04-16/AlCapData_Si16aDataset-Run" + run_str.str() + "_SiL1Spectra.pdf";
    std::string pdfname = "~/plots/2018-04-16/AlCapData_Si16aDataset-Run" + run_str.str() + "_SiL3Spectra.pdf";
    c->SaveAs(pdfname.c_str());
    
    i_file->Close();
  }
}
