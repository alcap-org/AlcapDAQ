void Si16b_FinalPlot_ProtonEnergy_woutSiL1Coinc() {

  std::string infilename = "~/data/results/Si16b/plots.root";
  TFile* infile = new TFile(infilename.c_str(), "READ");

  const double n_analysed_tmes = 122117927;

  //  std::string dirname = "all_SiR_timecut"; std::string layer1 = "SiR1"; std::string layer2 = "SiR2"; std::string layer3 = "SiR3";
  //  std::string dirname = "proton_SiR_timecut"; std::string layer1 = "SiR1"; std::string layer2 = "SiR2"; std::string layer3 = "SiR3";
  //  std::string dirname = "proton_SiR_timecut_wSiL1Coinc"; std::string layer1 = "SiR1"; std::string layer2 = "SiR2"; std::string layer3 = "SiL1";
  std::string dirname = "proton_SiR_timecut2000ns_wSiL1Coinc"; std::string layer1 = "SiR1"; std::string layer2 = "SiR2"; std::string layer3 = "SiL1";

  TLegend* leg = new TLegend(0.55,0.60,0.9,0.79);
  leg->SetBorderSize(0);
  leg->SetTextSize(0.03);
  leg->SetFillColor(kWhite);
  std::stringstream leglabel;
  
  TCanvas* c_Energy = new TCanvas("c_Energy", "c_Energy");
  c_Energy->SetLogy(1);
  std::string inhistname = dirname + "/hEvdE_TwoLayer_12";
  TH1D* hEnergy = ((TH2F*) infile->Get(inhistname.c_str()))->ProjectionX();
  std::string cut = layer1 + " && " + layer2;
  std::string histtitle = "Si16b Dataset, Right Arm, Protons";
  hEnergy->SetTitle(histtitle.c_str());
  hEnergy->SetStats(false);
  hEnergy->Sumw2();
  hEnergy->Rebin(10);
  hEnergy->Scale(1.0 / n_analysed_tmes);
  std::stringstream axistitle;
  axistitle.str("");
  axistitle << "Number of Protons per Analysed TME / " << hEnergy->GetXaxis()->GetBinWidth(1) << " keV";
  hEnergy->SetMaximum(1e-4);
  hEnergy->SetYTitle(axistitle.str().c_str());
  hEnergy->SetLineColor(kBlack);
  hEnergy->SetMarkerColor(kBlack);
  hEnergy->GetXaxis()->SetRangeUser(0, 20000);
  leglabel.str("");
  leglabel << cut;
  leg->AddEntry(hEnergy, leglabel.str().c_str(), "l");

  inhistname = dirname + "/hEvdE_TwoLayer_123";
  TH1D* hEnergy_123 = ((TH2F*) infile->Get(inhistname.c_str()))->ProjectionX();
  cut = layer1 + " && " + layer2 + " && " + layer3;
  histtitle = "Si16b Dataset, Right Arm, Protons";
  hEnergy_123->SetTitle(histtitle.c_str());
  hEnergy_123->SetStats(false);
  hEnergy_123->Sumw2();
  //  hEnergy_123->GetXaxis()->SetRangeUser(-1000, 10000);
  hEnergy_123->Rebin(10);
  hEnergy_123->Scale(1.0 / n_analysed_tmes);
  hEnergy_123->SetLineColor(kRed);
  hEnergy_123->SetMarkerColor(kRed);
  leglabel.str("");
  leglabel << cut;
  leg->AddEntry(hEnergy_123, leglabel.str().c_str(), "l");

  inhistname = dirname + "/hEvdE_TwoLayer_12not3";
  TH1D* hEnergy_12not3 = ((TH2F*) infile->Get(inhistname.c_str()))->ProjectionX();
  cut = layer1 + " && " + layer2 + " && !" + layer3;
  histtitle = "Si16b Dataset, Right Arm, Protons";
  hEnergy_12not3->SetTitle(histtitle.c_str());
  hEnergy_12not3->SetStats(false);
  hEnergy_12not3->Sumw2();
  //  hEnergy_12not3->GetXaxis()->SetRangeUser(-1000, 10000);
  hEnergy_12not3->Rebin(10);
  hEnergy_12not3->Scale(1.0 / n_analysed_tmes);
  hEnergy_12not3->SetLineColor(kBlue);
  hEnergy_12not3->SetMarkerColor(kBlue);
  leglabel.str("");
  leglabel << cut;
  leg->AddEntry(hEnergy_12not3, leglabel.str().c_str(), "l");

  hEnergy->Draw("P E");
  hEnergy_123->Draw("P E SAMES");
  hEnergy_12not3->Draw("P E SAMES");

  TLatex* latex = new TLatex();
  latex->DrawLatexNDC(0.55, 0.80, "AlCap Preliminary");

  leg->Draw();

  TFile* outfile = new TFile("forJohn.root", "RECREATE");
  hEnergy->Write();
  hEnergy_123->Write();
  hEnergy_12not3->Write();
  outfile->Write();
  outfile->Close();
}
