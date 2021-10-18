void SimpleStopPeak() {

  const int n_runs = 8;
  int run_numbers[n_runs] = {8987, 8988, 8989, 8990, 8991, 8992, 8993, 8994};//, 8995};//, 8996};
  double scale_factors[n_runs] = {1.21, 1.22, 1.23, 1.24, 1.25, 1.26, 1.27, 1.28};//, 1.29};//, 1.30};
  Int_t colours[n_runs] = {kBlue, kRed, kBlack, kMagenta, kSpring+2, kGray, kYellow, kCyan};
  double peaks[n_runs] = {0};
  double peak_errors[n_runs] = {0};

  double min_energy = 2000;
  double max_energy = 7000;
  double energy_width = 50;
  int n_bins = (max_energy - min_energy) / energy_width;

  TLegend* leg = new TLegend(0.10,0.55,0.50,0.85);
  leg->SetBorderSize(0);
  leg->SetTextSize(0.03);
  leg->SetFillStyle(0);
  leg->SetFillColor(kWhite);

  
  std::stringstream filename, histname, funcname, leglabel;
  for (int i_run = 0; i_run < n_runs; ++i_run) {
    int i_run_number = run_numbers[i_run];
    double i_scale_factor = scale_factors[i_run];
    Int_t i_colour = colours[i_run];

    filename.str("");
    filename << "~/data/out/local/out0" << i_run_number << "_tme-tree_no-templates.root";
    TFile* file = new TFile(filename.str().c_str(), "READ");
    if (file->IsZombie()) {
      std::cout << "Error: Can't open file " << filename.str() << std::endl;
								  return;
    }

    std::string treename = "TMETree/TMETree";
    TTree* tmetree = (TTree*) file->Get(treename.c_str());

    histname.str("");
    histname << "hist_" << i_run;
    TH1F* hist = new TH1F(histname.str().c_str(), "", n_bins,min_energy,max_energy);
    hist->Sumw2();
    hist->SetLineColor(i_colour);
    
    std::string drawcmd = "(4.366*SiL3.Amp + 35.17)>>" + histname.str();
    std::string cutcmd = "!anyDoubleCountedPulses && SiL3.tTME>-200 && SiL3.tTME<200";
    tmetree->Draw(drawcmd.c_str(), cutcmd.c_str(), "goff");
    hist->Scale(1.0 / hist->Integral());

    funcname.str("");
    funcname << "func_" << i_run;

    int max_bin = hist->GetMaximumBin();
    double max_bin_center = hist->GetBinCenter(max_bin);
    double max_content = hist->GetMaximum();
    while (max_bin_center < 4900) {
      max_content = hist->GetMaximum(max_content);
      //      std::cout << "Max Content = " << max_content << ", Second Max Content = " << second_max_content << std::endl;
      hist->GetBinWithContent(max_content, max_bin);
      max_bin_center = hist->GetBinCenter(max_bin);
    }
    
      
    std::cout << i_scale_factor << ": Max Bin = " << max_bin << ", Center = " << max_bin_center << std::endl;
    
    TF1* func = new TF1(funcname.str().c_str(), "[0]*TMath::Gaus(x, [1], [2])", max_bin_center-4*energy_width, max_bin_center+4*energy_width);
    func->SetParameters(0.01, 5500, 50);
    //    func->SetParameters(0.01, 500, 50);
    func->SetLineColor(i_colour);
    hist->Fit(func, "RN");

    hist->Draw("HIST SAMES");
    func->Draw("LSAME");

    peaks[i_run] = func->GetParameter(1);
    peak_errors[i_run] = func->GetParError(1);

    leglabel.str("");
    leglabel << std::fixed << std::setprecision(2) << "SF = " << i_scale_factor << ": Stop Peak = " << std::setprecision(3) << peaks[i_run]/1000 << " #pm " << peak_errors[i_run]/1000 << " MeV";
    leg->AddEntry(hist, leglabel.str().c_str(), "L");
  }
  leg->Draw();

    TCanvas* c2 = new TCanvas("c2", "c2");
    TGraphErrors* gre = new TGraphErrors(n_runs, scale_factors, peaks, NULL, peak_errors);
    gre->Draw("A*E");
}
