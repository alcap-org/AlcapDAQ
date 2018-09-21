#include <sstream>

void Si16a_SpectraComparison() {

  std::string basename = "~/data/results/Si16a_useful/plots";

  TLegend* leg = new TLegend(0.44,0.44,0.69,0.78);
  leg->SetBorderSize(0);
  leg->SetTextSize(0.03);
  leg->SetFillColor(kWhite);
  
  //  const int n_runs = 8;
  //  int run_numbers[n_runs] = {9735, 9736, 9737, 9739, 9740, 9741, 9742, 9743};
  //  double scale_factors[n_runs] = {1.035, 0.99, 0.98, 1.01, 1.02, 1.03, 1.04, 1.05};
  const int n_runs = 1;
  int run_numbers[n_runs] = {9740};
  double scale_factors[n_runs] = {1.02};

  double stopping_fractions[n_runs];
  double peak_edeps[n_runs];
  double peak_edep_errs[n_runs];

  const int n_hists = 3;
  std::string histtitlebase = "SiL1 Energy Spectrum";
  std::string histnames[n_hists] = {"hTarget_All", "hTarget_woutSiL3", "hTarget_wSiL3"};
  std::string labels[n_hists] = {"All", "with no SiL3 hit", "with an SiL3 hit"};
  //  std::string histtitlebase = "SiL3 Energy Spectrum";
  //  std::string histnames[n_hists] = {"hThick_All", "hThick_woutSiL1", "hThick_wSiL1"};
  //  std::string labels[n_hists] = {"All", "with no SiL1 hit", "with an SiL1 hit"};
  Int_t colours[n_hists] = {kBlack, kRed, kBlue};
  std::stringstream run_str, histtitle, label;
  for (int i_run = 0; i_run < n_runs; ++i_run) {
    leg->Clear();
    
    run_str.str("");
    run_str << "0" << run_numbers[i_run];

    std::string canvasname = "c_" + run_str.str();
    TCanvas* c = new TCanvas(canvasname.c_str(), canvasname.c_str());
    
    std::string filename = basename + run_str.str() + ".root";
    TFile* i_file = new TFile(filename.c_str(), "READ");

    double n_stopped = 0;
    double n_total = 0;
    for (int i_hist = 0; i_hist < n_hists; ++i_hist) {
      std::string i_histname = histnames[i_hist];
      std::string newhistname = i_histname + run_str.str();

      std::string treename = histnames[i_hist] + "_cuts";
      TTree* cuttree = (TTree*) i_file->Get(treename.c_str());
      double layer_coincidence_time = 0;
      TBranch* br = cuttree->GetBranch("layer_coincidence_time");
      if (br) {
	br->SetAddress(&layer_coincidence_time);
      }
      cuttree->GetEntry(0);

      TH2F* hTarget2D = (TH2F*) i_file->Get(i_histname.c_str());
      int min_bin = hTarget2D->GetXaxis()->FindBin(-200);
      int max_bin = hTarget2D->GetXaxis()->FindBin(200)-1;
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
      histtitle << histtitlebase << " (Run " << run_numbers[i_run] << ", SF = " << scale_factors[i_run] << ")";
      hTarget->SetTitle(histtitle.str().c_str());

      label.str("");
      label << labels[i_hist];
      if (br) {
	label << " within " << layer_coincidence_time << " ns";
      }
      label << " (" << hTarget->GetEntries() << " entries)";
      leg->AddEntry(hTarget, label.str().c_str(), "l");

      if (i_hist == 0) {
	n_total = hTarget->GetEntries();
      }
      else if (i_hist == 1) {
	n_stopped = hTarget->GetEntries();
	//	peak_edeps[i_run] = hTarget->GetBinCenter(hTarget->GetMaximumBin());
	//	peak_edep_errs[i_run] = hTarget->GetXaxis()->GetBinWidth(1)/2;

	double peak_edep_bin_center = hTarget->GetBinCenter(hTarget->GetMaximumBin());
	double bin_width = hTarget->GetXaxis()->GetBinWidth(1);
	TF1* f = new TF1("f", "[0]*TMath::Gaus(x, [1], [2])", peak_edep_bin_center-bin_width, peak_edep_bin_center+bin_width);
	f->SetParameters(hTarget->GetMaximum(), peak_edep_bin_center, bin_width);
	f->SetLineColor(kGreen);
	hTarget->Fit(f, "R");
	f->Draw("LSAME");
	peak_edeps[i_run] = f->GetParameter(1);
	peak_edep_errs[i_run] = f->GetParError(1);
      }
    }
    stopping_fractions[i_run] = n_stopped / n_total;
      
    leg->Draw();
    
    //    std::string pdfname = "~/plots/2018-04-18/AlCapData_Si16aDataset-Run" + run_str.str() + "_SiL1Spectra.pdf";
    std::string pdfname = "~/plots/2018-04-18/AlCapData_Si16aDataset-Run" + run_str.str() + "_SiL3Spectra.pdf";
    //    c->SaveAs(pdfname.c_str());
    
    i_file->Close();
  }

  /*  TCanvas* c_stopFrac = new TCanvas("c_stopFrac", "c_StopFrac");
  //  TGraph* grStoppingFraction = new TGraph(n_runs, scale_factors, stopping_fractions);
  TGraphErrors* grStoppingFraction = new TGraphErrors(n_runs, peak_edeps, stopping_fractions, peak_edep_errs, 0);
  grStoppingFraction->Draw("A*E");
  grStoppingFraction->GetYaxis()->SetTitle("N Red Histogram / N Black Histogram");
  */
}
