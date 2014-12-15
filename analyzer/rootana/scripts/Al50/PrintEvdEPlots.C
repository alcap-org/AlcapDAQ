void PrintEvdEPlots() {

  std::string version = "v55";
  std::string savelocation = "~/data/out/"+version+"/plots";
  const int n_arms = 2;

  std::string filename = "~/data/out/"+version+"/total.root";
  TFile* file = new TFile(filename.c_str());
  TH2F* SiL_EvdE = (TH2F*) file->Get("TME_EvdE/SiL_EvdE");
  TH2F* SiR_EvdE = (TH2F*) file->Get("TME_EvdE/SiR_EvdE");

  TH2F* evde_hists[n_arms] = {SiL_EvdE, SiR_EvdE};
  std::string arm_names[n_arms] = {"SiL", "SiR"};
  bool project_x = true;
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

    if (project_x) {
      TH1D* hProjection = evde_hists[i_arm]->ProjectionX();
      hProjection->Rebin(4);
      hProjection->Draw("HIST E");
      plotname = savelocation+"/"+arm_names[i_arm]+"_EvdE_ProjectionX";
      pdfname = plotname+".pdf";
      pngname = plotname+".png";
      c1->SaveAs(pdfname.c_str());
      c1->SaveAs(pngname.c_str());
    }
  }
}
