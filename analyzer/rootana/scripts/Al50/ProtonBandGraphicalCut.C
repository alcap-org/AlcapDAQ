void ProtonBandGraphicalCut() {

  TCanvas* c1 = new TCanvas("c1", "c1");
  c1->SetLogz(1);

  std::string version = "v69";
  std::string savelocation = "~/data/out/"+version+"/plots";
  const int n_arms = 2;

  std::string filename = "~/data/out/"+version+"/total.root";
  TFile* file = new TFile(filename.c_str(), "READ");
  TH2F* SiL_EvdE = (TH2F*) file->Get("TME_EvdE/all_particles/SiL_EvdE");
  TH2F* SiR_EvdE = (TH2F*) file->Get("TME_EvdE/all_particles/SiR_EvdE");

  TH2F* evde_hists[n_arms] = {SiL_EvdE, SiR_EvdE};
  std::string arm_names[n_arms] = {"SiL", "SiR"};

  TFile* output_file = new TFile("result.root", "RECREATE");

  for (int i_arm = 0; i_arm < n_arms; ++i_arm) {
  
    // straight line cut to remove electrons
    double x_1 = 0, y_1 = 2000, x_2 = 4000, y_2 = 0;
    double gradient = (y_2 - y_1) / (x_2 - x_1);
    double offset = y_1;
    TF1* electron_spot_cut = new TF1("electron_spot_cut", "[0]*x + [1]", 0, 15000);
    electron_spot_cut->SetLineColor(kBlue);
    electron_spot_cut->SetParameter(0, gradient);
    electron_spot_cut->SetParameter(1, offset);

    double punch_through_yoffset = 300;
    TF1* punch_through_cut = new TF1("punch_through_cut", "[0]", 0, 25000);
    punch_through_cut->SetLineColor(kBlue);
    punch_through_cut->SetParameter(0, punch_through_yoffset);


    // Cut to remove the remaining deuteron band
    TF1* deuteron_cut = new TF1("deuteron_cut", "[0]*TMath::Exp([1]*x) + [2]", 0, 25000);
    deuteron_cut->SetLineColor(kBlue);
    deuteron_cut->SetParameter(0, 4500);
    deuteron_cut->SetParameter(1, -0.0004);
    deuteron_cut->SetParameter(2, 500);
    //    evde_hists[i_arm]->Fit(deuteron_cut, "R");

    for (int i_bin = 1; i_bin <= evde_hists[i_arm]->GetNbinsX(); ++i_bin) {
      for (int j_bin = 1; j_bin <= evde_hists[i_arm]->GetNbinsY(); ++j_bin) {
	double x_coord = evde_hists[i_arm]->GetXaxis()->GetBinCenter(i_bin);
	double y_coord = evde_hists[i_arm]->GetYaxis()->GetBinCenter(j_bin);
	
	if (evde_hists[i_arm]->GetBinContent(i_bin, j_bin) < 20 ||
	  y_coord < electron_spot_cut->Eval(x_coord) || 
	  y_coord < punch_through_cut->Eval(x_coord) ||
	    y_coord > deuteron_cut->Eval(x_coord) ) {
	  evde_hists[i_arm]->SetBinContent(i_bin, j_bin, 0);
	}
      }
    }
    evde_hists[i_arm]->Draw("COLZ");
    //    electron_spot_cut->Draw("LSAME");
    //    punch_through_cut->Draw("LSAME");
    //    deuteron_cut->Draw("LSAME");

    std::string plotname = savelocation+"/"+arm_names[i_arm]+"_EvdE_ProtonBand";
    std::string pdfname = plotname+".pdf";
    std::string pngname = plotname+".png";
    c1->SaveAs(pdfname.c_str());
    c1->SaveAs(pngname.c_str());

    TH1D* hProjection = evde_hists[i_arm]->ProjectionX();
    hProjection->Rebin(5);
    hProjection->Draw("HIST E");
    plotname = savelocation+"/"+arm_names[i_arm]+"_EvdE_ProtonBand_ProjectionX";
    pdfname = plotname+".pdf";
    pngname = plotname+".png";
    c1->SaveAs(pdfname.c_str());
    c1->SaveAs(pngname.c_str());

    hProjection->Write();

    double energy_range_low = 4000;
    double energy_range_high = 8000;
    int bin_low = hProjection->FindBin(energy_range_low);
    int bin_high = hProjection->FindBin(energy_range_high);
    std::cout << arm_names[i_arm] << " Proton Integral (" << energy_range_low << " - " << energy_range_high << " keV) = " << hProjection->Integral(bin_low, bin_high) << std::endl;
  }
}
