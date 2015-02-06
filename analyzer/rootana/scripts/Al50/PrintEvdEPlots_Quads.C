void PrintEvdEPlots_Quads() {

  TCanvas* c1 = new TCanvas("c1", "c1");

  std::string version = "v68";
  std::string savelocation = "~/data/out/"+version+"/plots";
  const int n_arms = 2;
  const int n_quads = 4;

  std::string filename = "~/data/out/"+version+"/total.root";
  TFile* file = new TFile(filename.c_str());
  TH2F* SiL_Quad1_EvdE = (TH2F*) file->Get("TME_EvdE/all_particles/SiL_Quad1_EvdE");
  TH2F* SiL_Quad2_EvdE = (TH2F*) file->Get("TME_EvdE/all_particles/SiL_Quad2_EvdE");
  TH2F* SiL_Quad3_EvdE = (TH2F*) file->Get("TME_EvdE/all_particles/SiL_Quad3_EvdE");
  TH2F* SiL_Quad4_EvdE = (TH2F*) file->Get("TME_EvdE/all_particles/SiL_Quad4_EvdE");
  TH2F* SiR_Quad1_EvdE = (TH2F*) file->Get("TME_EvdE/all_particles/SiR_Quad1_EvdE");
  TH2F* SiR_Quad2_EvdE = (TH2F*) file->Get("TME_EvdE/all_particles/SiR_Quad2_EvdE");
  TH2F* SiR_Quad3_EvdE = (TH2F*) file->Get("TME_EvdE/all_particles/SiR_Quad3_EvdE");
  TH2F* SiR_Quad4_EvdE = (TH2F*) file->Get("TME_EvdE/all_particles/SiR_Quad4_EvdE");

  Int_t colours[n_quads] = {kRed, kBlack, kBlue, kMagenta};

  TH2F* evde_hists[n_arms][n_quads] = { {SiL_Quad1_EvdE, SiL_Quad2_EvdE, SiL_Quad3_EvdE, SiL_Quad4_EvdE}, {SiR_Quad1_EvdE, SiR_Quad2_EvdE, SiR_Quad3_EvdE, SiR_Quad4_EvdE} };
  std::string arm_names[n_arms] = {"SiL", "SiR"};
  std::string quad_names[n_quads] = {"Quad1", "Quad2", "Quad3", "Quad4"};

  for (int i_arm = 0; i_arm < n_arms; ++i_arm) {

    for (int i_quad = 0; i_quad < n_quads; ++i_quad) {
      evde_hists[i_arm][i_quad]->SetMarkerColor(colours[i_quad]);
      if (i_quad == 0) {
	evde_hists[i_arm][i_quad]->Draw();
      }
      else {
	evde_hists[i_arm][i_quad]->Draw("SAME");
      }
    }

    c1->SetLogz(1);
    std::string plotname = savelocation+"/"+arm_names[i_arm]+"_Quads_EvdE_LogZ";
    std::string pdfname = plotname+".pdf";
    std::string pngname = plotname+".png";
    c1->SaveAs(pdfname.c_str());
    c1->SaveAs(pngname.c_str());

    /*
    c1->SetLogz(1);
    plotname = savelocation+"/"+arm_names[i_arm]+"_EvdE_LogZ";
    pdfname = plotname+".pdf";
    pngname = plotname+".png";
    c1->SaveAs(pdfname.c_str());
    c1->SaveAs(pngname.c_str());
    */
  }
}
