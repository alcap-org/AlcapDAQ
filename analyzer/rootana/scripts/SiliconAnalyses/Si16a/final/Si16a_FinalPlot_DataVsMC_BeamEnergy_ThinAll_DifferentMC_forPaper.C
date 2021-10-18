void Si16a_FinalPlot_DataVsMC_BeamEnergy_ThinAll_DifferentMC_forPaper(std::string outdir = "") {

  int run_number = 9740;
  double scale_factor = 1.02;
  
  const int n_data_runs = 1;
  double data_input_beam_energy[n_data_runs] = {1.37};
  std::string data_postpostfilenames[n_data_runs] = {""};
  std::string data_prefilename = "~/data/results/Si16a/raw_spectra";
  std::string data_postfilename = "_newPP";
  std::string data_histname = "Thin_All_TimeSlice-200_200/hRawSpectrum";
  Int_t data_marker_style = kFullDotLarge;
  Int_t data_colour = kBlack;
  std::string data_leglabels[n_data_runs] = {"Data"};
  
  const int n_mc_runs = 3;
  double mc_input_beam_energy[n_mc_runs] = {1.37, 1.27, 1.47};
  std::string mc_postpostfilenames[n_mc_runs] = {"", "_midLowE", "_midHighE"};
  std::string mc_prefilename = "~/data/mc/Si16a/MC_raw_spectra";
  std::string mc_postfilename = "_moreStats";
  std::string mc_histname = "hThinEnergy_noVeto_SiL";
  Int_t mc_marker_style = kDot;
  Int_t mc_colours[n_mc_runs] = {kRed, kBlue, kSpring};
  std::string mc_leglabels[n_mc_runs] = {"MC (nominal)", "MC (-100 keV beam energy)", "MC (+100 keV beam energy)"};
  
  TCanvas* c = new TCanvas();
  
  TLegend* leg = new TLegend(0.45,0.40,0.95,0.60);
  leg->SetBorderSize(0);
  leg->SetTextSize(0.04);//0.07);
  leg->SetFillStyle(0);
  leg->SetFillColor(kWhite);

  std::stringstream run_str, sf_str, axislabel, leglabel, input_beam_str;
  run_str.str("");
  run_str << "0" << run_number;
  sf_str.str("");
  sf_str << std::fixed << std::setprecision(1) << scale_factor*25.4;

  for (int i_data_run = 0; i_data_run < n_data_runs; ++i_data_run) {
    input_beam_str.str("");
    input_beam_str << data_input_beam_energy[i_data_run];
    
    std::string infilename = data_prefilename + run_str.str() + data_postfilename + data_postpostfilenames[i_data_run] + ".root";
    TFile* infile = new TFile(infilename.c_str(), "READ");
    if (infile->IsZombie()) {
      std::cout << "ERROR: Can't find file " << infilename << std::endl;
      return;
    }
    
    TH1F* hEnergy = (TH1F*) infile->Get(data_histname.c_str());
    if (!hEnergy) {
      std::cout << "ERROR: Can't find histogram " << data_histname << std::endl;
      return;
    }

    std::string histtitle;
    histtitle = "Si16a Dataset, Run " + run_str.str() + ", SF = " + sf_str.str();
    hEnergy->SetTitle(histtitle.c_str());
    hEnergy->Sumw2();
    hEnergy->SetStats(false);
    hEnergy->SetMarkerStyle(data_marker_style);
    hEnergy->SetMarkerColor(data_colour);
    hEnergy->SetLineColor(data_colour);
    hEnergy->SetLineStyle(1);
    hEnergy->SetLineWidth(2);
    //      hEnergy->SetFillStyle(3004);
    //      hEnergy->SetFillColor(fill_colours[i_setting]);
    hEnergy->GetXaxis()->SetTitle("Energy [keV]");
    hEnergy->Rebin(5);
    hEnergy->Scale(1.0 / hEnergy->GetMaximum());
    hEnergy->GetXaxis()->SetRangeUser(0,2500);
    //      hEnergy->GetXaxis()->SetRangeUser(0,1500);
    
    hEnergy->GetXaxis()->SetTitleSize(0.05);
    hEnergy->GetXaxis()->SetLabelSize(0.05);
    hEnergy->GetYaxis()->SetTitleSize(0.05);
    hEnergy->GetYaxis()->SetLabelSize(0.05);
    hEnergy->GetXaxis()->SetTitleOffset(0.95);
    hEnergy->GetYaxis()->SetTitleOffset(1.0);
    
    axislabel.str("");
    //      axislabel << "Counts / " << hEnergy->GetXaxis()->GetBinWidth(1) << " keV";
    axislabel << "Peak Normalized / " << hEnergy->GetXaxis()->GetBinWidth(1) << " keV";
    hEnergy->GetYaxis()->SetTitle(axislabel.str().c_str());
    
    hEnergy->Draw("HIST E SAME");
    
    //    text = "E_{MC} = " + input_beam_str.str() + " MeV";
    //    latex->DrawLatexNDC(0.65, 0.68, text.c_str());
    leg->AddEntry(hEnergy, data_leglabels[i_data_run].c_str(), "pl");
  }

  for (int i_mc_run = 0; i_mc_run < n_mc_runs; ++i_mc_run) {
    input_beam_str.str("");
    input_beam_str << mc_input_beam_energy[i_mc_run];
    
    std::string infilename = mc_prefilename + run_str.str() + mc_postfilename + mc_postpostfilenames[i_mc_run] + ".root";
    TFile* infile = new TFile(infilename.c_str(), "READ");
    if (infile->IsZombie()) {
      std::cout << "ERROR: Can't find file " << infilename << std::endl;
      return;
    }
    
    TH1F* hEnergy = (TH1F*) infile->Get(mc_histname.c_str());
    if (!hEnergy) {
      std::cout << "ERROR: Can't find histogram " << mc_histname << std::endl;
      return;
    }

    std::string histtitle;
    histtitle = "Si16a Mcset, Run " + run_str.str() + ", SF = " + sf_str.str();
    hEnergy->SetTitle(histtitle.c_str());
    hEnergy->Sumw2();
    hEnergy->SetStats(false);
    hEnergy->SetMarkerStyle(mc_marker_style);
    hEnergy->SetMarkerColor(mc_colours[i_mc_run]);
    hEnergy->SetLineColor(mc_colours[i_mc_run]);
    hEnergy->SetLineStyle(1);
    hEnergy->SetLineWidth(2);
    //      hEnergy->SetFillStyle(3004);
    //      hEnergy->SetFillColor(fill_colours[i_setting]);
    hEnergy->GetXaxis()->SetTitle("Energy [keV]");
    hEnergy->Rebin(5);
    hEnergy->Scale(1.0 / hEnergy->GetMaximum());
    hEnergy->GetXaxis()->SetRangeUser(0,2500);
    //      hEnergy->GetXaxis()->SetRangeUser(0,1500);
    
    hEnergy->GetXaxis()->SetTitleSize(0.05);
    hEnergy->GetXaxis()->SetLabelSize(0.05);
    hEnergy->GetYaxis()->SetTitleSize(0.05);
    hEnergy->GetYaxis()->SetLabelSize(0.05);
    hEnergy->GetXaxis()->SetTitleOffset(0.95);
    hEnergy->GetYaxis()->SetTitleOffset(1.0);
    
    axislabel.str("");
    //      axislabel << "Counts / " << hEnergy->GetXaxis()->GetBinWidth(1) << " keV";
    axislabel << "Peak Normalized / " << hEnergy->GetXaxis()->GetBinWidth(1) << " keV";
    hEnergy->GetYaxis()->SetTitle(axislabel.str().c_str());
    
    hEnergy->Draw("HIST E SAME");
    
    //    text = "E_{MC} = " + input_beam_str.str() + " MeV";
    //    latex->DrawLatexNDC(0.65, 0.68, text.c_str());
    leg->AddEntry(hEnergy, mc_leglabels[i_mc_run].c_str(), "pl");
  }

  leg->Draw();
  
  TLatex* latex = new TLatex();
  latex->SetTextSize(0.07);
  latex->SetTextAlign(22);
  //    latex->DrawLatexNDC(0.7, 0.55, "AlCap #bf{#it{Preliminary}}");
  //    std::string text = "Run " + run_str.str() + ", SF = " + sf_str.str();
  std::string text = "p_{beam} = " + sf_str.str() + " keV/c";
  latex->DrawLatexNDC(0.65, 0.78, text.c_str());

  if (outdir != "") {
    std::string savename = outdir + "/AlCapDataVsMC_Si16aDataset_Run" + run_str.str() + "_BeamEnergy_ThinAll_DifferentMC_forPaper";
    std::string pdfname = savename + ".pdf";
    c->SaveAs(pdfname.c_str());
  }
}
