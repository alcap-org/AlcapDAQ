void Rates() {

  const int n_materials = 1;
  std::string materials[n_materials] = {"Al"};

  const int n_particles = 2;
  std::string particles[n_particles] = {"protons", "deuterons"};
  Int_t colours[n_particles] = {kRed, kBlue};

  const int n_rates = 5;
  double rate_ids[n_rates] = {0, 1, 2, 3, 4};
  //  std::string rate_ids[n_rates] = {"main analysis", "SiL - single-exp fit", "SiR - single-exp-fit"};
  double rates[n_materials][n_particles][n_rates] = { {
      {28.07, 25.86, 27.08, 0, 27.94}, {8.07, 7.00, 7.25, 0, 0} // Al
    } };
  double errors[n_materials][n_particles][n_rates] = { {
      { 0.17, 0.25, 0.25, 0, 0.34}, {0.08, 0.13, 0.11, 0, 0} // Al
    } };

  for (int i_material = 0; i_material < n_materials; ++i_material) {
    TCanvas* c = new TCanvas();
    c->Divide(n_particles);

    for (int i_particle = 0; i_particle < n_particles; ++i_particle) {
      c->cd(i_particle+1);
      
      TGraphErrors* gre = new TGraphErrors(n_rates, rate_ids, rates[i_material][i_particle], 0, errors[i_material][i_particle]);
      gre->SetMarkerStyle(kFullCircle);
      gre->SetMarkerColor(colours[i_particle]);
      gre->SetLineColor(colours[i_particle]);
      gre->Draw("APE");
      gre->GetXaxis()->SetLimits(-1, n_rates);
      gre->GetXaxis()->SetLabelSize(0.035);
      gre->GetXaxis()->ChangeLabel(1, -1, 0, -1, -1, -1, "");
      gre->GetXaxis()->ChangeLabel(2, -1, 0, -1, -1, -1, "");
      gre->GetXaxis()->ChangeLabel(3, -1, -1, -1, -1, -1, "main analysis");
      gre->GetXaxis()->ChangeLabel(4, -1, 0, -1, -1, -1, "");
      gre->GetXaxis()->ChangeLabel(5, -1, -1, -1, -1, -1, "#splitline{      SiL}{single-exp fit}");
      gre->GetXaxis()->ChangeLabel(6, -1, 0, -1, -1, -1, "");
      gre->GetXaxis()->ChangeLabel(7, -1, -1, -1, -1, -1, "#splitline{      SiR}{single-exp fit}");
    }
  }  
}
