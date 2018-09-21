#include <sstream>

void Si16a_SiTSpectraComparison() {

  std::string basename = "~/data/results/Si16a_useful/plots";

  TLegend* leg = new TLegend(0.5,0.55,0.9,0.85);
  leg->SetBorderSize(0);
  leg->SetTextSize(0.03);
  leg->SetFillColor(kWhite);
  TH1F* hEmpty = new TH1F("hEmpty", "", 1,0,1);
  hEmpty->SetLineColor(kWhite);
  leg->AddEntry(hEmpty, "Run  SF  PeakBin   Mean     RMS", "l");
  
  const int n_runs = 8;
  int run_numbers[n_runs] = {9737, 9736, 9739, 9740, 9741, 9735, 9742, 9743};
  double scale_factors[n_runs] = {0.98, 0.99, 1.01, 1.02, 1.03, 1.035, 1.04, 1.05};

  std::string histtitlebase = "SiT Energy Spectrum";
  std::string histname = "hSiT_All";
  Int_t colours[n_runs] = {kBlack, kRed, kBlue, kMagenta, kGreen, kYellow, kGray, kViolet};
  std::stringstream run_str, histtitle, label;

  std::string canvasname = "c_SiT";
  TCanvas* c = new TCanvas(canvasname.c_str(), canvasname.c_str());

  for (int i_run = 0; i_run < n_runs; ++i_run) {
    
    run_str.str("");
    run_str << "0" << run_numbers[i_run];

    std::string filename = basename + run_str.str() + ".root";
    TFile* i_file = new TFile(filename.c_str(), "READ");

    TH2F* hTarget2D = (TH2F*) i_file->Get(histname.c_str());
    std::string newhistname = histname + "_" + run_str.str();
    TH1D* hTarget = hTarget2D->ProjectionY(newhistname.c_str());
    hTarget->SetYTitle("Normalised to Peak Bin");
    hTarget->SetLineWidth(2);
    hTarget->SetStats(false);
    hTarget->SetDirectory(0);
    hTarget->Scale(1.0 / hTarget->GetMaximum());
    hTarget->GetYaxis()->SetRangeUser(0,2);
    hTarget->SetLineColor(colours[i_run]);
    //      hTarget->GetYaxis()->SetRangeUser(0, 0.12);
    hTarget->Draw("SAME");

    histtitle.str("");
    histtitle << histtitlebase;
    hTarget->SetTitle(histtitle.str().c_str());

    label.str("");
    label << run_numbers[i_run] << "  " << scale_factors[i_run] << "  " << hTarget->GetXaxis()->GetBinCenter(hTarget->GetMaximumBin()) << "  " << hTarget->GetMean() << "  " << hTarget->GetRMS();
    //    if (br) {
    //      label << " within " << layer_coincidence_time << " ns";
    //    }
    //    label << " (" << hTarget->GetEntries() << " entries)";
    leg->AddEntry(hTarget, label.str().c_str(), "l");

    i_file->Close();
  }

        
  leg->Draw();
    
  std::string pdfname = "~/plots/2018-04-18/AlCapData_Si16aDataset-Run" + run_str.str() + "_SiL3Spectra.pdf";  

}
