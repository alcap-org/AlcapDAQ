void StoppedMuonDiscrepancy_PunchThroughRate() {

  const int n_runs = 10;
  int first_run = 9735;
  int last_run = first_run+n_runs;
  double scale_factors[n_runs] = {1.035, 0.99, 0.98, 1.00, 1.01, 1.02, 1.03, 1.04, 1.05, 0};
  double n_midas_blocks[n_runs] = {3068, 3755, 3429, 3509, 3704, 2907, 2845, 3025, 2993, 0};

  TH1F* hPunchThroughRate = new TH1F("hPunchThroughRate", "", n_runs,0,n_runs);
  std::stringstream binlabel;
  for (int i_bin = 1; i_bin <= hPunchThroughRate->GetNbinsX(); ++i_bin) {
    binlabel.str("");
    binlabel << "SF = " << scale_factors[i_bin-1];
    hPunchThroughRate->GetXaxis()->SetBinLabel(i_bin, binlabel.str().c_str());
  }
  hPunchThroughRate->SetTitle("Si16a Dataset");
  hPunchThroughRate->GetXaxis()->LabelsOption("a");
  hPunchThroughRate->GetYaxis()->SetTitle("(All - Stops) / All");

  TH1F* hDirectCountVsSF = (TH1F*) hPunchThroughRate->Clone("hDirectCountVsSF");
  hDirectCountVsSF->GetYaxis()->SetTitle("Stopped Muons per Midas Block");
  TH1F* hGeCountVsSF = (TH1F*) hPunchThroughRate->Clone("hGeCountVsSF");
  hGeCountVsSF->SetLineColor(kRed);

  std::stringstream filename;
  for (int i_run = first_run; i_run < last_run; ++i_run) {
    double i_scale_factor = scale_factors[i_run - first_run];
    binlabel.str("");
    binlabel << "SF = " << i_scale_factor;
    int i_bin = hPunchThroughRate->GetXaxis()->FindBin(binlabel.str().c_str());

    filename.str("");
    filename << "/gpfs/home/edmonds_a/data/out/v19/out" << std::setw(5) << std::setfill('0') << i_run << ".root";

    TFile* file = new TFile(filename.str().c_str(), "READ");
    if (file->IsZombie()) {
      continue;
    }

    TH1F* hAll = (TH1F*) file->Get("PlotTAP_StoppedMuonDiscrepancy/hSiL1_Energy_NoCuts");
    TH1F* hStops = (TH1F*) file->Get("PlotTAP_StoppedMuonDiscrepancy/hSiL1_Energy_wNoSiL3Coincidence");
    double punch_through_rate = (hAll->GetEntries() - hStops->GetEntries()) / hAll->GetEntries();    
    hPunchThroughRate->SetBinContent(i_bin, punch_through_rate);

    filename.str("");
    filename << "stopped-muon-discrepancy-plots_run" << std::setw(5) << std::setfill('0') << i_run << "_eff09685.root";

    file = new TFile(filename.str().c_str(), "READ");
    if (file->IsZombie()) {
      continue;
    }
    TH1F* hDirectCount = (TH1F*) file->Get("hNMuonsPerChannel");
    double sil1_direct_count = (hDirectCount->GetBinContent(1) / n_midas_blocks[i_run - first_run]);
    double sil1_direct_count_error = (hDirectCount->GetBinError(1) / hDirectCount->GetBinContent(1)) * sil1_direct_count;
    hDirectCountVsSF->SetBinContent(i_bin, sil1_direct_count);
    hDirectCountVsSF->SetBinError(i_bin, sil1_direct_count_error);

    TH1F* hGeCount = (TH1F*) file->Get("hNMuonsPerChannel_Ge");
    double sil1_ge_count = (hGeCount->GetBinContent(1) / n_midas_blocks[i_run - first_run]);
    double sil1_ge_count_error = (hGeCount->GetBinError(1) / hGeCount->GetBinContent(1)) * sil1_ge_count;
    hGeCountVsSF->SetBinContent(i_bin, sil1_ge_count);
    hGeCountVsSF->SetBinError(i_bin, sil1_ge_count_error);
  }

  TCanvas* c1 = new TCanvas("c1", "c1");
  hPunchThroughRate->Draw("HIST TEXT");

  TCanvas* c2 = new TCanvas("c2", "c2");
  TLegend *leg = new TLegend(0.5,0.8,0.7,0.6);
  leg->SetBorderSize(0);
  leg->SetTextSize(0.05);
  leg->SetFillColor(0);
  leg->AddEntry(hDirectCountVsSF, "from SiL1", "l");
  leg->AddEntry(hGeCountVsSF, "from GeLoGain", "l");

  hDirectCountVsSF->Draw("HIST E");
  hGeCountVsSF->Draw("HIST E SAME");
  leg->Draw();
}
