void PrintEvdEPlots() {

  std::string savelocation = "~/data/out/v36/plots";
  const int n_arms = 2;

  TFile* file = new TFile("~/data/out/v36/total.root");
  TH1F* SiL_EvdE = (TH1F*) file->Get("TME_EvdE/hSiL_EvdE");
  TH1F* SiR_EvdE = (TH1F*) file->Get("TME_EvdE/hSiR_EvdE");

  TH1F* evde_hists[n_arms] = {SiL_EvdE, SiR_EvdE};
  std::string arm_names[n_arms] = {"SiL", "SiR"};

  for (int i_arm = 0; i_arm < n_arms; ++i_arm) {

    evde_hists[i_arm]->Draw("COLZ");
    c1->SetLogz(0);
    std::string plotname = savelocation+"/"+arm_names[i_arm]+"_EvdE";
    std::string pdfname = plotname+".pdf";
    std::string pngname = plotname+".png";
    c1->SaveAs(pdfname.c_str());
    c1->SaveAs(pngname.c_str());

    c1->SetLogz(1);
    plotname = savelocation+"/"+arm_names[i_arm]+"_EvdE_LogZ";
    pdfname = plotname+".pdf";
    pngname = plotname+".png";
    c1->SaveAs(pdfname.c_str());
    c1->SaveAs(pngname.c_str());

  }
}
