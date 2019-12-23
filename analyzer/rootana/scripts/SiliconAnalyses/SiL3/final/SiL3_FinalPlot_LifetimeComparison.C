void SiL3_FinalPlot_LifetimeComparison(std::string savedir = "") {


  TCanvas* c_Lifetime = new TCanvas("c_Lifetime", "c_Lifetime");
  TLegend* leg = new TLegend(0.30,0.75,0.70,0.85);
  leg->SetBorderSize(0);
  leg->SetTextSize(0.03);
  leg->SetFillColor(kWhite);
  leg->SetFillStyle(0);

  const int n_settings = 2;
  //  std::string settings[n_settings] = {"", "20us"};
  //  std::string leglabels[n_settings] = {"t_{PP} < 10 #mus", "t_{PP} < 20 #mus"};
  std::string settings[n_settings] = {"", "_wFlatBkg"};
  std::string leglabels[n_settings] = {"expo only", "expo + pol0"};
  TGraphErrors* gres[n_settings] = {0};
  Int_t colours[n_settings] = {kBlue, kRed};
  Int_t markerstyles[n_settings] = {kFullCircle, kFullTriangleUp};

  double min_energy_cut = 500;
  double max_energy_cut = 30500;
  double energy_cut_step = 500;
  const int n_cuts = 60;
  for (int i_setting = 0; i_setting < n_settings; ++i_setting) {

    std::string infilename = "~/data/results/SiL3/lifetime_geq2TgtPulse_newPP20us.root";
    //    std::string infilename = "~/data/results/SiL3/lifetime_geq2TgtPulse_newPP20us_1750nsStart.root";
    //    std::string infilename = "~/data/results/SiL3/lifetime_geq2TgtPulse_newPP20us_6000nsIgnoreEnd.root";
    //    std::string infilename = "~/data/results/SiL3/lifetime_geq2TgtPulse_newPP" + settings[i_setting] + ".root";
    TFile* infile = new TFile(infilename.c_str(), "READ");
  
    double energy_cuts[n_cuts];
    double lifetimes[n_cuts];
    double lifetime_errors[n_cuts];
    std::stringstream energy_cut_str;
    int i_element = 0;
    for (double i_min_energy_cut = min_energy_cut; i_min_energy_cut < max_energy_cut; i_min_energy_cut += energy_cut_step) {
      
      energy_cut_str.str("");
      energy_cut_str << i_min_energy_cut << "keVCut" << settings[i_setting];
      //      energy_cut_str << i_min_energy_cut << "keVCut_wFlatBkg";
      
      std::string base_indirname = "Target_" + energy_cut_str.str();
      
      std::string indirname = base_indirname;
      std::string infitname = indirname + "/muonic_atom_lifetime";
  
      TF1* fit = (TF1*) infile->Get(infitname.c_str());
      
      energy_cuts[i_element] = i_min_energy_cut;
      lifetimes[i_element] = fit->GetParameter(3);
      lifetime_errors[i_element] = fit->GetParError(3);

      std::cout << "E > " << i_min_energy_cut << " keV: lifetime = " << fit->GetParameter(3) << " +/- " << fit->GetParError(3) << std::endl;
      
      ++i_element;
    }

    TGraphErrors* gre = new TGraphErrors(n_cuts, energy_cuts, lifetimes, 0, lifetime_errors);
    gre->SetMarkerColor(colours[i_setting]);
    gre->SetLineColor(colours[i_setting]);
    gre->SetMarkerStyle(markerstyles[i_setting]);
    gres[i_setting] = gre;

    leg->AddEntry(gre, leglabels[i_setting].c_str(), "p");
  }
  
  gres[0]->Draw("APE");
  gres[0]->SetTitle("Fitted lifetime as a function of energy cut");
  gres[0]->GetXaxis()->SetTitle("Energy Cut [keV]");
  gres[0]->GetYaxis()->SetRangeUser(650,850);

  gres[1]->Draw("PE SAME");

  double literature_lifetime = 756.0;
  double literature_lifetime_error = 1.0;
  TLine* line = new TLine(0, literature_lifetime, max_energy_cut-energy_cut_step, literature_lifetime);
  line->SetLineColor(kBlack);
  line->SetLineWidth(2);
  line->SetLineStyle(kDashed);
  line->Draw("LSAME");
  TBox* box = new TBox(0, literature_lifetime-literature_lifetime_error, max_energy_cut-energy_cut_step, literature_lifetime+literature_lifetime_error);
  box->SetLineColor(kBlack);
  box->SetFillStyle(0);
  box->Draw("LSAME");
  leg->AddEntry(box, "Literature Value", "f");
  
  leg->Draw();

  if (savedir != "") {
    std::string savename = savedir + "AlCapData_SiL3Dataset_ActiveTarget_LifetimeComparison";
    
    std::string pdfname = savename + ".pdf";
    c_Lifetime->SaveAs(pdfname.c_str());
    std::string pngname = savename + ".png";
    c_Lifetime->SaveAs(pngname.c_str());
  }
}
