void Si16a_MuonBeamEnergy() {

  //  const int n_runs = 8;
  //  int run_numbers[n_runs] = {9737, 9736, 9739, 9740, 9741, 9735, 9742, 9743};
  //  double scale_factors[n_runs] = {0.98, 0.99, 1.01, 1.02, 1.03, 1.035, 1.04, 1.05};
  const int n_runs = 1;
  int run_numbers[n_runs] = {9740};
  double scale_factors[n_runs] = {1.02};

  double timecut = 200;

  //  TFile* outfile = new TFile("~/data/results/Si16a_useful/muon_beam_energies.root", "RECREATE");
  
  std::stringstream run_str, sf_str, timecut_str;

  for (int i_run = 0; i_run < n_runs; ++i_run) {
    run_str.str("");
    run_str << run_numbers[i_run];

    sf_str.str("");
    sf_str << scale_factors[i_run];

    timecut_str.str("");
    timecut_str << timecut;

    std::string filename = "~/data/out/v10/out0" + run_str.str() + ".root";
    TFile* file = new TFile(filename.c_str(), "READ");
    TTree* tmetree = (TTree*) file->Get("TMETree/TMETree");

    std::string histname = "hTotalMuonBeamEnergy_" + run_str.str();
    std::string histtitle = "Total Energy (SiL1 + SiL3 + SiT) for hits within " + timecut_str.str() + " ns of SiT (Run " + run_str.str() + ", SF = " + sf_str.str() + ")";
    TH1F* hTotalMuonBeamEnergy = new TH1F(histname.c_str(), histtitle.c_str(), 100,0,5000);
    hTotalMuonBeamEnergy->SetXTitle("SiL1.E + SiL3.E + SiT.E [keV]");
    const int n_sil1_channels = 16;
    const int n_sit_channels = 4;
    double sit_gains[n_sit_channels] = {2.02316, 1.91508, 1.94223, 1.95524};
    double sit_offsets[n_sit_channels] = {-8.02, 12.48, -11.79, 0.85};
    std::stringstream drawcmd, cutcmd;
    for (int i_sil1_channel = 0; i_sil1_channel < n_sil1_channels; ++i_sil1_channel) {
      
      for (int i_sit_channel = 0; i_sit_channel < n_sit_channels; ++i_sit_channel) {
	drawcmd.str("");
	drawcmd << "SiL1_" << i_sil1_channel+1 << ".E+SiL3.E+" << sit_gains[i_sit_channel] << "*SiT_" << i_sit_channel+1 << ".Amp+" << sit_offsets[i_sit_channel] << ">>+" << histname;
	
	cutcmd.str("");
	cutcmd << "abs(SiL1_" << i_sil1_channel+1 << ".tTME)<" << timecut << " && abs(SiL1_" << i_sil1_channel+1 << ".tTME-SiL3.tTME)<" << timecut;
	
	for (int j_sit_channel = 0; j_sit_channel < n_sit_channels; ++j_sit_channel) {
	  if (j_sit_channel == i_sit_channel) {
	    cutcmd << " && @SiT_" << j_sit_channel+1 << ".size()==1";
	  }
	  else {
	    cutcmd << " && @SiT_" << j_sit_channel+1 << ".size()==0";
	  }
	}
	//	std::cout << "AE: " << cutcmd.str() << std::endl;
	tmetree->Draw(drawcmd.str().c_str(), cutcmd.str().c_str());
	//	std::cout << "AE: " << hTotalMuonBeamEnergy->GetEntries() << std::endl;

	/*	// Now do stops in SiL1
	drawcmd.str("");
	drawcmd << "SiL1_" << i_sil1_channel+1 << ".E+" << sit_gains[i_sit_channel] << "*SiT_" << i_sit_channel+1 << ".Amp+" << sit_offsets[i_sit_channel] << ">>+" << histname;
	
	cutcmd.str("");
	cutcmd << "abs(SiL1_" << i_sil1_channel+1 << ".tTME)<" << timecut << " && abs(SiL1_" << i_sil1_channel+1 << ".tTME-SiL3.tTME)>" << timecut;
	
	for (int j_sit_channel = 0; j_sit_channel < n_sit_channels; ++j_sit_channel) {
	  if (j_sit_channel == i_sit_channel) {
	    cutcmd << " && @SiT_" << j_sit_channel+1 << ".size()==1";
	  }
	  else {
	    cutcmd << " && @SiT_" << j_sit_channel+1 << ".size()==0";
	  }
	}
	tmetree->Draw(drawcmd.str().c_str(), cutcmd.str().c_str());
	*/
      }
    }
    hTotalMuonBeamEnergy->SetDirectory(0);
    file->Close();
    hTotalMuonBeamEnergy->Draw();
    //    outfile->cd();
    //    hTotalMuonBeamEnergy->Write();
  }

  //  outfile->Write();
  //  outfile->Close();
}
