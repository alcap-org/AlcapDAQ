void GetPIDCut() {

  TCanvas* c1 = new TCanvas("c1", "c1");
  c1->SetLogz(1);

  std::string version = "v66";
  std::string savelocation = "~/data/out/"+version+"/plots";
  const int n_arms = 2;

  std::string filename = "~/data/out/"+version+"/total.root";
  TFile* file = new TFile(filename.c_str(), "READ");
  TH2F* SiL_EvdE = (TH2F*) file->Get("TME_EvdE/all_particles/SiL_EvdE");
  TH2F* SiR_EvdE = (TH2F*) file->Get("TME_EvdE/all_particles/SiR_EvdE");

  TH2F* evde_hists[n_arms] = {SiL_EvdE, SiR_EvdE};
  std::string arm_names[n_arms] = {"SiL", "SiR"};

  //  for (int i_arm = 0; i_arm < n_arms; ++i_arm) {

  Double_t contours[6] = {10, 20, 30, 40, 50, 60};
  TF2* pid_cut_fn = new  TF2("pid_cut_fn", "[0]*TMath::Exp([2]*y + [1]*x)", 2500, 8000, 100, 3000);    
  pid_cut_fn->SetParameter(0, 15);
  pid_cut_fn->SetParameter(1, -0.0001);
  pid_cut_fn->SetParameter(2, 5);
  //  pid_cut_fn->SetParameter(3, 5);
  evde_hists[0]->SetContour(6, contours);
  evde_hists[0]->Draw("CONT Z LIST");
  evde_hists[0]->GetZaxis()->SetRangeUser(contours[0], contours[5]);
  //    evde_hists[0]->Fit(pid_cut_fn);
  c1->Update();

  // Get Contours
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

  int i = 1;
  contLevel = (TList*)conts->At(i);
  printf("Contour %d has %d Graphs\n", i, contLevel->GetSize());
  curv = (TGraph*)contLevel->At(13);
  curv->GetYaxis()->SetRangeUser(0,10000);
  curv->GetXaxis()->SetRangeUser(0,25000);
  Double_t* cont_x = curv->GetX();
  Double_t* cont_y = curv->GetY();
  /*  for (int i_point = 0; i_point < curv->GetN(); ++i_point) {
    std::cout << "Point #" << i_point << ": (x, y) = (" << x[i_point] << ", " << y[i_point] << ")" << std::endl;
  }
  */
  curv->Draw("AL");
  nGraphs += contLevel->GetSize();
  //  }

  // Loop through bins and get anything that is within this contour
  bool inside = false;
  int j = curv->GetN() - 1;
  for (int x_bin = 1; x_bin <= evde_hists[0]->GetNbinsX(); ++x_bin) {
    for (int y_bin = 1; y_bin <= evde_hists[0]->GetNbinsY(); ++y_bin) {
      inside = false;
      double x_coord = evde_hists[0]->GetXaxis()->GetBinCenter(x_bin);
      double y_coord = evde_hists[0]->GetYaxis()->GetBinCenter(y_bin);

      if (evde_hists[0]->GetBinContent(x_bin, y_bin) >= 20) {
	inside = true;
      }
      /*      for (int i=0; i<curv->GetN(); i++) {
	//	std::cout << "AE: Testing (" << x_coord << ", " << y_coord << ") against contour points (" << cont_x[i] << ", " << cont_y[i] << ") and (" << cont_x[j] << ", " << cont_y[j] << ")" << std::endl;
	if (cont_y[i]<y_coord && cont_y[j]>=y_coord
	    ||  cont_y[j]<y_coord && cont_y[i]>=y_coord) {
	  if (cont_x[i]+(y_coord-cont_y[i])/(cont_y[j]-cont_y[i])*(cont_x[j]-cont_x[i])<x_coord) {
	    inside = !inside; 
	  }
	}
	j=i; 
      }
      */
      if (inside == false) {
	evde_hists[0]->SetBinContent(x_bin, y_bin, 0);
      }
    }
  }
  evde_hists[0]->Draw("COLZ");

}
