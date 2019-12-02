void Si16b_FinalPlot_TwoLayer_Lifetime_EnergySlices() {

  std::string infilename = "~/data/results/Si16b/lifetime_newPP_geq1TgtPulse.root";
  TFile* infile = new TFile(infilename.c_str(), "READ");

  TCanvas* c_time = new TCanvas("c_time", "c_time");
  //  c_time->SetLogy(1);

  TLegend* leg = new TLegend(0.50,0.45,0.90,0.75);
  leg->SetBorderSize(0);
  leg->SetTextSize(0.03);
  leg->SetFillStyle(0);
  leg->SetFillColor(kWhite);
  std::stringstream leglabel;
  
  //  const int n_particles = 5;
  //  std::string particles[n_particles] = {"all", "proton", "deuteron", "triton", "alpha"};
  //  Int_t colours[n_particles] = {kBlack, kRed, kCyan, kMagenta, kSpring};
  const int n_particles = 1;
  std::string particles[n_particles] = {"proton"};
  Int_t colours[n_particles] = {kRed};
  TGraphErrors* gres[n_particles] = {0};
    
  double min_energy_cut = 0;
  double max_energy_cut = 15000;
  double energy_cut_step = 1000;
  const int n_slices = 15;
  double energies[n_slices] = {0};
  double lifetimes[n_particles][n_slices] = {0};
  double lifetime_errors[n_particles][n_slices] = {0};
  
  for (int i_particle = 0; i_particle < n_particles; ++i_particle) {
    std::string this_particle = particles[i_particle];

    std::stringstream energy_cut_str;
    for (double i_min_energy_cut = min_energy_cut; i_min_energy_cut < max_energy_cut; i_min_energy_cut += energy_cut_step) {
      energy_cut_str.str("");
      //      energy_cut_str << i_min_energy_cut << "_" << i_min_energy_cut+energy_cut_step;
      energy_cut_str << i_min_energy_cut << "_" << max_energy_cut;

      int i_slice = (i_min_energy_cut - min_energy_cut) / energy_cut_step;
      energies[i_slice] = i_min_energy_cut;
      
      //      std::string dirname = this_particle + "_wTimeCut_SingleExp_" + energy_cut_str.str();
      std::string dirname = this_particle + "_wTimeCut_SingleExp_FlatBkg_" + energy_cut_str.str();
      std::string fitname = dirname + "/muonic_atom_lifetime";

      TF1* fit = (TF1*) infile->Get(fitname.c_str());
      //      fit->SetLineColor(colours[i_particle]);
      
      lifetimes[i_particle][i_slice] = fit->GetParameter(3);
      lifetime_errors[i_particle][i_slice] = fit->GetParError(3);
    }

    gres[i_particle] = new TGraphErrors(n_slices, energies, lifetimes[i_particle], NULL, lifetime_errors[i_particle]);
    gres[i_particle]->SetMarkerColor(colours[i_particle]);
    gres[i_particle]->SetLineColor(colours[i_particle]);
    gres[i_particle]->SetMarkerStyle(7);

    if (i_particle == 0) {
      gres[i_particle]->Draw("APE");
    }
    else {
      gres[i_particle]->Draw("PE SAME");
    }
  }

  TLatex* latex = new TLatex();
  latex->DrawLatexNDC(0.35, 0.80, "AlCap Preliminary");

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
}
