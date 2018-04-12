void UnfoldProtonSpectrum() {

  gROOT->ProcessLine(".L scripts/Unfolding/bayesian-unfolding/libRooUnfold.so");

  /////////////////////////////////////
  // User parameters
  std::string data_filename = "~/data/results/Si16b_passive/EvdEPlots_NewCuts.root";
  std::string histname = "hEvdE_Si16b_Proton_Veto";
  std::string response_filename = "~/data/mc/Si16b_passive/R15b_Si16b_response-matrix_100keV-bins.root";
  std::string lifetime_filename = "~/data/results/Si16b_passive/LifetimeFit_NewCuts.root";
  std::string outfilename = "~/data/results/Si16b_passive/unfolded-wVeto_NewCuts.root";
  int rebin_factor = 10; // from 10 keV to the response matrix binning

  TCanvas* c1 = new TCanvas("c1", "c1");
  //  c1->Divide(2);

  TFile* response_file = new TFile(response_filename.c_str(), "READ");

  TFile* lifetime_file = new TFile(lifetime_filename.c_str(), "READ");
  TF1* lifetime_fn = (TF1*) lifetime_file->Get("expo_fit_fn");
  
  TFile* data_file = new TFile(data_filename.c_str(), "READ");
  TTree* cuttree = (TTree*) data_file->Get("cuts");
  double time_cut = 0;
  cuttree->SetBranchAddress("time_cut", &time_cut);
  cuttree->GetEntry(0);

  double time_cut_efficiency = (lifetime_fn->Eval(time_cut) / lifetime_fn->GetParameter(0));
  std::cout << time_cut_efficiency << std::endl;
  
    
  const int n_arms = 1;
  std::string arm_names[n_arms] = {"SiR"};


  TFile* outfile = new TFile(outfilename.c_str(), "RECREATE");
  
  for (int i_arm = 0; i_arm < n_arms; ++i_arm) {
    c1->cd(i_arm+1);
    //    c1->GetPad(i_arm+1)->SetLogy();

    std::string arm_name = arm_names[i_arm];
    
    std::string response_name = arm_name + "_response";
    RooUnfoldResponse* response = (RooUnfoldResponse*) response_file->Get(response_name.c_str()); // what I'm trying to reconstruct


    TH2F* hEvdE_proton = (TH2F*) data_file->Get(histname.c_str());
    TH1D* folded_spectrum = hEvdE_proton->ProjectionX();
    folded_spectrum->SetName("hFoldedSpectrum");
    folded_spectrum->Rebin(rebin_factor);

    TH2D* response_matrix = response->Hresponse();
    response_matrix->SetTitle("hResponseMatrix");
    
    RooUnfoldBayes unfold (response, folded_spectrum);

    TH1D* unfolded_spectrum = (TH1D*) unfold.Hreco();
    unfolded_spectrum->SetName("hUnfoldedSpectrum");
    unfolded_spectrum->Draw("HIST E SAMES");
    unfolded_spectrum->SetLineColor(kBlue);
    unfolded_spectrum->SetLineWidth(2);
    unfolded_spectrum->Scale(1.0 / time_cut_efficiency);
    folded_spectrum->SetLineColor(kRed);
    folded_spectrum->SetLineWidth(2);
    folded_spectrum->Draw("HIST E SAMES");
    unfold.PrintTable(cout);

    outfile->cd();
    response->Write();
    hEvdE_proton->Write();
    folded_spectrum->Write();
    unfolded_spectrum->Write();
  }

  outfile->Write();
  outfile->Close();
}
