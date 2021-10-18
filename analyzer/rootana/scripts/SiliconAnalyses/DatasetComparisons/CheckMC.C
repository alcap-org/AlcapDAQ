void CheckMC() {

  const int n_datasets = 2;
  std::string datasets[n_datasets] = {"Al50", "Ti50"};

  const int n_settings = 3;
  std::string settings[n_settings] = {"", "_misCalibUp", "_misCalibDown"};
  Int_t colours[n_settings] = {kBlack, kRed, kBlue};

  TCanvas* c_1D = new TCanvas();
  c_1D->Divide(2);

  std::string particle = "alpha";
  for (int i_setting = 0; i_setting < n_settings; ++i_setting) {
    std::string setting = settings[i_setting];
    
    TCanvas* c_2D = new TCanvas();
    c_2D->Divide(2);

    for (int i_dataset = 0; i_dataset < n_datasets; ++i_dataset) {

      std::string dataset = datasets[i_dataset];
      
      std::string filename = "/data/R15bMC/respMatrix_100M_" + dataset + "_" + particle + "_100keVBins" + setting + ".root";
      TFile* file = new TFile(filename.c_str(), "READ");

      TH2D* hELoss = (TH2D*) file->Get("SiR_hELoss");

      c_2D->cd(i_dataset+1);
      hELoss->Draw("COLZ");

      TLatex latex;
      std::string label = dataset + ", " + particle;
      latex.DrawLatex(5, 8, label.c_str());

      std::string settinglabel = setting;
      if (settinglabel == "") {
	settinglabel = "no miscalibration";
      }
      latex.DrawLatex(5, 7, settinglabel.c_str());

      std::cout << dataset << ", " << settinglabel << ": " << hELoss->Integral() << std::endl;

      c_1D->cd(i_dataset+1);

      Int_t colour = colours[i_setting];
      std::string profname = "hProf_" + dataset + setting;
      auto hProfile = hELoss->ProfileX(profname.c_str(), 1, -1, "");
      hProfile->GetYaxis()->SetRangeUser(0, 10);
      hProfile->SetLineColor(colour);
      hProfile->Draw("SAME");
      
      latex.DrawLatex(5, 8, label.c_str());

      latex.SetTextColor(colour);
      latex.DrawLatex(5, 8-(i_setting+1), settinglabel.c_str());
    }
  }
}
