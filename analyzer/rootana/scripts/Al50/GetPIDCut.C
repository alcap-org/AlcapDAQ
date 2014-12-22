void GetPIDCut() {

  TCanvas* c1 = new TCanvas("c1", "c1");
  c1->SetLogz(1);

  std::string version = "v67";
  std::string savelocation = "~/data/out/"+version+"/plots";
  const int n_arms = 2;

  std::string filename = "~/data/out/"+version+"/total.root";
  TFile* file = new TFile(filename.c_str(), "READ");
  TH2F* SiL_EvdE = (TH2F*) file->Get("TME_EvdE/all_particles/SiL_EvdE");
  TH2F* SiR_EvdE = (TH2F*) file->Get("TME_EvdE/all_particles/SiR_EvdE");

  TH2F* evde_hists[n_arms] = {SiL_EvdE, SiR_EvdE};
  std::string arm_names[n_arms] = {"SiL", "SiR"};

  //  for (int i_arm = 0; i_arm < n_arms; ++i_arm) {
  
  // straight line cut to remove electrons
  double x_1 = 0, y_1 = 2000, x_2 = 4000, y_2 = 0;
  double gradient = (y_2 - y_1) / (x_2 - x_1);
  double offset = y_1;

  for (int i_bin = 1; i_bin <= evde_hists[0]->GetNbinsX(); ++i_bin) {
    for (int j_bin = 1; j_bin <= evde_hists[0]->GetNbinsY(); ++j_bin) {
      double x_coord = evde_hists[0]->GetXaxis()->GetBinCenter(i_bin);
      double y_coord = evde_hists[0]->GetYaxis()->GetBinCenter(j_bin);

      if (evde_hists[0]->GetBinContent(i_bin, j_bin) < 20 ||
	  y_coord < gradient*x_coord + offset || y_coord < 300) {
	evde_hists[0]->SetBinContent(i_bin, j_bin, 0);
      }
    }
  }

  // Get Contours
  evde_hists[0]->Draw("CONT Z LIST");
  c1->Update();
  TObjArray *conts = (TObjArray*)gROOT->GetListOfSpecials()->FindObject("contours");
  TList* contLevel = NULL;
  TGraph* curv     = NULL;
  TGraph* gc       = NULL;

  Int_t nGraphs    = 0;
  Int_t TotalConts = 0;

  if (conts == NULL){
    printf("*** No Contours Were Extracted!\n");
    TotalConts = 0;
    return;
  } else {
    TotalConts = conts->GetSize();
  }

  printf("TotalConts = %d\n", TotalConts);
  
  int i = 0;
  contLevel = (TList*)conts->At(i);
  printf("Contour %d has %d Graphs\n", i, contLevel->GetSize());
  curv = (TGraph*)contLevel->At(0);
  curv->GetYaxis()->SetRangeUser(0,10000);
  curv->GetXaxis()->SetRangeUser(0,25000);
  Double_t* cont_x = curv->GetX();
  Double_t* cont_y = curv->GetY();
  curv->Draw("AL");
  nGraphs += contLevel->GetSize();

}
