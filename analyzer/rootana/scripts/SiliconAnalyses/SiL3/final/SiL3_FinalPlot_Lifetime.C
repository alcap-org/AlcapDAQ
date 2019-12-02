void SiL3_FinalPlot_Lifetime() {

  bool save = false;
  std::string base_savename = "~/plots/2019-07-17/";
    
  std::string infilename = "~/data/results/SiL3/lifetime_geq2TgtPulse_newPP20us.root";
  TFile* infile = new TFile(infilename.c_str(), "READ");

  const int n_fits = 2;
  std::string histtags[n_fits] = {"", "_wFlatBkg"};
  std::string fit_fns[n_fits] = {"expo only", "expo + pol0"};
  Int_t fit_colours[n_fits] = {kBlue, kRed};
  
  double min_energy_cut = 15000;
  double max_energy_cut = 30500;
  double energy_cut_step = 30500;
  std::stringstream energy_cut_str;
  for (double energy_cut = min_energy_cut; energy_cut < max_energy_cut; energy_cut += energy_cut_step) {

    std::stringstream cut_str;
    cut_str.str("");
    cut_str << energy_cut;

    TCanvas* c_Lifetime = new TCanvas("c_Lifetime", "c_Lifetime");
    c_Lifetime->SetLogy();

    //    TLegend* leg = new TLegend(0.15,0.55,0.30,0.85); // left-hand side
    TLegend* leg = new TLegend(0.60,0.65,0.80,0.90); // right-hand side
    leg->SetBorderSize(0);
    leg->SetTextSize(0.035);
    leg->SetFillColor(kWhite);
    leg->SetFillStyle(0);

    for (int i_fit = 0; i_fit < n_fits; ++i_fit) {
      std::string base_indirname = "Target_" + cut_str.str() + "keVCut" + histtags[i_fit];
      std::string fit_fn = fit_fns[i_fit];
      Int_t fit_colour = fit_colours[i_fit];
        
      std::string indirname = base_indirname;
      std::string inhistname = indirname + "/hTime";
      std::string infitname = indirname + "/muonic_atom_lifetime";
      std::string inflatname = indirname + "/flatbkg";

      if (i_fit == 0) {
	TH1F* hLifetime = (TH1F*) infile->Get(inhistname.c_str());
	hLifetime->SetLineColor(kBlack);
	std::string histtitle = "SiL3 Dataset, Target, E > " + cut_str.str() + " keV (with flat bkg)";
	hLifetime->SetTitle(histtitle.c_str());
	hLifetime->SetStats(false);
	//  gStyle->SetOptFit(1);
	hLifetime->GetXaxis()->SetRangeUser(0, 20000);
	//    hLifetime->GetXaxis()->SetRangeUser(1000, 15000);
	hLifetime->SetMinimum(0.001);
	hLifetime->SetMaximum(2e7);
	hLifetime->GetFunction("muonic_atom_lifetime")->SetBit(TF1::kNotDraw);
	hLifetime->Draw("HIST E");
      }

      TF1* fit = (TF1*) infile->Get(infitname.c_str());
      
      double min_range, max_range;
      fit->GetRange(min_range, max_range);
      std::cout << "AE: " << min_range << ", " << max_range << std::endl;
      TF1* draw_fit = new TF1("draw_fit", "[0]*TMath::Exp(-x/[1]) + [2]", min_range, max_range);
      draw_fit->SetParameters(fit->GetParameter(2), fit->GetParameter(3), fit->GetParameter(4));
      draw_fit->SetLineColor(fit_colour);
      draw_fit->SetLineWidth(2);
      draw_fit->SetLineStyle(kDashed);
      draw_fit->Draw("LSAME");

      // want to show the fit ignore region
      TF1* draw_fit_neg = new TF1("draw_fit_neg", "[0]*TMath::Exp(-x/[1]) + [2]", min_range, fit->GetParameter(0));
      draw_fit_neg->SetParameters(fit->GetParameter(2), fit->GetParameter(3), fit->GetParameter(4));
      draw_fit_neg->SetLineColor(fit_colour);
      draw_fit_neg->SetLineWidth(2);
      draw_fit_neg->Draw("LSAME");
      
      TF1* draw_fit_pos = new TF1("draw_fit_pos", "[0]*TMath::Exp(-x/[1]) + [2]", fit->GetParameter(1), max_range);
      draw_fit_pos->SetParameters(fit->GetParameter(2), fit->GetParameter(3), fit->GetParameter(4));
      draw_fit_pos->SetLineColor(fit_colour);
      draw_fit_pos->SetLineWidth(2);
      draw_fit_pos->Draw("LSAME");
      //  fit->Draw("LSAME");

      TF1* draw_fit_flat = new TF1("draw_fit_flat", "[0]", 0, max_range);
      draw_fit_flat->SetParameter(0, fit->GetParameter(4));
      draw_fit_flat->SetLineColor(fit_colour);
      draw_fit_flat->SetLineWidth(2);
      draw_fit_flat->SetLineStyle(kDotted);
      draw_fit_flat->Draw("LSAME");

      std::stringstream leglabel;
      leglabel << "#splitline{" << fit_fn << std::setprecision(3) << "}{#tau = " << fit->GetParameter(3) << " #pm " << std::setprecision(2) << fit->GetParError(3) << " ns}";
      std::cout << "AE: " << leglabel.str() << std::endl;
      leg->AddEntry(draw_fit_pos, leglabel.str().c_str(), "l");
    }

    /*
    TF1* flat = (TF1*) infile->Get(inflatname.c_str());
    if (flat) {
      flat->Print();
      std::cout << flat->GetParameter(0) << " +/- " << flat->GetParError(0) << std::endl;
      Int_t flat_colour = kMagenta;
      double min_range, max_range;
      flat->GetRange(min_range, max_range);
      std::cout << "AE: " << min_range << ", " << max_range << std::endl;
      TF1* draw_flat = new TF1("draw_flat", "[0]", min_range, max_range);
      draw_flat->SetParameter(0, flat->GetParameter(0));
      draw_flat->SetLineColor(flat_colour);
      draw_flat->SetLineWidth(2);
      draw_flat->SetLineStyle(kDashed);
      draw_flat->Draw("LSAME");
      //    leg->AddEntry(draw_flat, "pol0 fit", "l");

      // want to show the flat ignore region
      TF1* draw_flat_neg = new TF1("draw_flat_neg", "[0]", min_range, flat->GetParameter(1));
      draw_flat_neg->SetParameter(0, flat->GetParameter(0));
      draw_flat_neg->SetLineColor(flat_colour);
      draw_flat_neg->SetLineWidth(2);
      draw_flat_neg->Draw("LSAME");

      TF1* draw_flat_pos = new TF1("draw_flat_pos", "[0]", flat->GetParameter(2), max_range);
      draw_flat_pos->SetParameter(0, flat->GetParameter(0));
      draw_flat_pos->SetLineColor(flat_colour);
      draw_flat_pos->SetLineWidth(2);
      draw_flat_pos->Draw("LSAME");

      leg->AddEntry(draw_flat_pos, "pol0 fit", "l");
      leg->AddEntry(draw_flat, "pol0 fit (ignored region)", "l");

      TBox* flat_inc_error = new TBox(min_range, flat->GetParameter(0)-flat->GetParError(0), max_range, flat->GetParameter(0)+flat->GetParError(0));
      flat_inc_error->SetFillStyle(0);
      flat_inc_error->SetLineColor(flat_colour);
      flat_inc_error->Draw("LSAME");
    }

    int start_bin = hLifetime->FindBin(2000);
    int end_bin = hLifetime->FindBin(18000);
    for (int i_bin = start_bin; i_bin < end_bin; ++i_bin) {
      double bin_center = hLifetime->GetBinCenter(i_bin);
      double bin_content = hLifetime->GetBinContent(i_bin);
      double fit_val = draw_fit->Eval(bin_center);
      double diff = bin_content - fit_val;
      //      std::cout << "t = " << bin_center << " ns: hist = " << bin_content << ", fn = " << fit_val << ", diff = " << diff << std::endl;
    }
    */
      
    leg->Draw();

    if (save) {
      //      std::string pdfname = base_savename + "AlCapData_SiL3Dataset_ActiveTarget_LifetimeFit_" + cut_str.str() + "keV.pdf";
      std::string pdfname = base_savename + "AlCapData_SiL3Dataset_ActiveTarget_LifetimeFit_" + cut_str.str() + "keV_PP10us.pdf";
      c_Lifetime->SaveAs(pdfname.c_str());
    }
  }
}
