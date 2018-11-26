void SiL3_LifetimeCheck() {

  std::string infilename = "~/data/results/SiL3/lifetime.root";
  //  std::string infilename = "~/data/results/SiL3_no-templates/lifetime.root";
  TFile* infile = new TFile(infilename.c_str(), "READ");

  TCanvas* c_Lifetime = new TCanvas("c_Lifetime", "c_Lifetime");
  c_Lifetime->SetLogy();

  TLegend* leg = new TLegend(0.5,0.55,0.9,0.85);
  leg->SetBorderSize(0);
  leg->SetTextSize(0.03);
  leg->SetFillColor(kWhite);

  Int_t colours[5] = {kRed, kBlue, kBlack, kMagenta, kGreen};

  double min_energy_cut = 1000;
  double max_energy_cut = 3500;
  double energy_cut_step = 500;
  std::stringstream energy_cut_str;
  for (double i_min_energy_cut = min_energy_cut; i_min_energy_cut < max_energy_cut; i_min_energy_cut += energy_cut_step) {

    int i_element =  (i_min_energy_cut - min_energy_cut)/energy_cut_step;
    Int_t i_colour = colours[i_element];
    
    energy_cut_str.str("");
    energy_cut_str << i_min_energy_cut;
    
    std::string base_indirname = "Target_" + energy_cut_str.str() + "_30000";
    std::string indirname = base_indirname;// + exp_strs[i_exp] + flat_strs[i_flat];
    std::string inhistname = indirname + "/hTime";
    std::string infitname = indirname + "/muonic_atom_lifetime";

    TH1F* hLifetime = (TH1F*) infile->Get(inhistname.c_str());
    TF1* fit = (TF1*) infile->Get(infitname.c_str());

    //    hLifetime->SetMarkerStyle(kFullCircle);
    hLifetime->SetMarkerColor(i_colour);
    hLifetime->SetLineColor(i_colour);
    hLifetime->SetTitle("SiL3 Dataset, Right Arm, Charged Particles");
    hLifetime->SetStats(false);
    //  gStyle->SetOptFit(1);
    hLifetime->GetXaxis()->SetRangeUser(0, 10000);
    hLifetime->Draw("HIST E SAME");

    fit->SetLineStyle(kDashed);
    fit->SetLineColor(i_colour);
    fit->Draw("LSAME");
      
      
    std::stringstream leglabel;
    leglabel << base_indirname << ": " << std::setprecision(3) << "#tau = " << fit->GetParameter(1) << " #pm " << std::setprecision(1) << fit->GetParError(1) << " ns";
    std::cout << "AE: " << leglabel.str() << std::endl;
    leg->AddEntry(fit, leglabel.str().c_str(), "l");
  }
  leg->Draw();
}
