#include "TCanvas.h"
#include "TFile.h"
#include "TH1.h"
#include "TH1F.h"
#include "TLegend.h"
#include "TStyle.h"

#include <iostream>
using std::cout;
using std::endl;

void multispec() {
  //input root files for raw histograms
  TFile *AlUFile = new TFile("AlU_base.root", "READ");
  TFile *AlDFile = new TFile("AlD_base.root", "READ");
  TFile *TiUFile = new TFile("TiU_base.root", "READ");
  TFile *TiDFile = new TFile("TiD_base.root", "READ");
  TFile *PbUFile = new TFile("PbU_base.root", "READ");
  TFile *PbDFile = new TFile("PbD_base.root", "READ");
  //TFile *H2OUFile = new TFile("H2OU_base.root", "READ");
  //TFile *H2ODFile = new TFile("H2OD_base.root", "READ");

  //retrieve histograms
  TH1F *AlUMeas, *AlDMeas, *TiUMeas, * TiDMeas, *PbUMeas, *PbDMeas;

  AlUFile->GetObject("unfanflu", AlUMeas);
  AlDFile->GetObject("unfanflu", AlDMeas);
  TiUFile->GetObject("unfanflu", TiUMeas);
  TiDFile->GetObject("unfanflu", TiDMeas);
  PbUFile->GetObject("unfanflu", PbUMeas);
  PbDFile->GetObject("unfanflu", PbDMeas);
  //H2OUFile->GetObject("unfanflu", H2OUMeas);
  //H2ODFile->GetObject("unfanflu", H2ODMeas);

  //extra inputs as needed... ie simulated Pb and Al spectra
  //historic Pb or H2O data is available as well.

  //input parameters... need scaling factor, nMuons for each and binwidth
  double nSimAl = 1.5E7, nSimTi = 1.5E7, nSimPb = 2.5E7;//, nSimH2O = 0;
  double AlUSF = 5.54, AlDSF = 5.91, TiUSF = 5.31, TiDSF = 5.76,
    PbUSF = 0.791, PbDSF = 0.852;//, H2OUSF = 0, H2ODSF = 0;
  double nMuAl = 1.7E8, nMuTi = 3.3E8, nMuPb = 1.3E7;//, nMuH2O = 0;
  double etAl = 0.787, etTi = 0.556, etPb = 0.79;//, etH2O = 0;
  double binWidth = AlUMeas->GetBinWidth(2);

  //check all bin widths the same
  cout << "binwidth = " << binWidth << endl;
  if(binWidth != AlDMeas->GetBinWidth(2) )
    cout << "Error, AlD has different bin width : " << AlDMeas->GetBinWidth(2) << endl;
  if(binWidth != TiUMeas->GetBinWidth(2) )
    cout << "Error, TiU has different bin width : " << TiUMeas->GetBinWidth(2) << endl;
  if(binWidth != TiDMeas->GetBinWidth(2) )
    cout << "Error, TiD has different bin width : " << TiDMeas->GetBinWidth(2) << endl;
  if(binWidth != PbUMeas->GetBinWidth(2) )
    cout << "Error, PbU has different bin width : " << PbUMeas->GetBinWidth(2) << endl;
  if(binWidth != PbDMeas->GetBinWidth(2) )
    cout << "Error, PbD has different bin width : " << PbDMeas->GetBinWidth(2) << endl;
  //if(binWidth != H2OUMeas->GetBinWidth(2) )
  //cout << "Error, H2OU has different bin width" << std::endl;
  //if(binWidth != H2ODMeas->GetBinWidth(2) )
  //cout << "Error, H2OD has different bin width" << std::endl;

  //determine number of hits in each histogram
  double nMeasAlU = AlUMeas->Integral( AlUMeas->GetXaxis()->FindBin(2.0),
	                               AlUMeas->GetXaxis()->FindBin(11.5) );
  double nMeasAlD = AlDMeas->Integral( AlDMeas->GetXaxis()->FindBin(2.0),
	                               AlDMeas->GetXaxis()->FindBin(11.5) );
  double nMeasTiU = TiUMeas->Integral( TiUMeas->GetXaxis()->FindBin(2.0),
	                               TiUMeas->GetXaxis()->FindBin(11.5) );
  double nMeasTiD = TiDMeas->Integral( TiDMeas->GetXaxis()->FindBin(2.0),
	                               TiDMeas->GetXaxis()->FindBin(11.5) );
  double nMeasPbU = PbUMeas->Integral( PbUMeas->GetXaxis()->FindBin(2.0),
	                               PbUMeas->GetXaxis()->FindBin(11.5) );
  double nMeasPbD = PbDMeas->Integral( PbDMeas->GetXaxis()->FindBin(2.0),
	                               PbDMeas->GetXaxis()->FindBin(11.5) );

  // determine multiplicative factor
  // M = (N_s *R_o/m) / (N_mu * e_t) normalized by hits
  // nSim * scale_factor / (nMeas*et*nMu*binwidth)
  double scAlU = (nSimAl * AlUSF) / (nMeasAlU * etAl * nMuAl * binWidth);
  double scAlD = (nSimAl * AlDSF) / (nMeasAlD * etAl * nMuAl * binWidth);
  double scTiU = (nSimTi * TiUSF) / (nMeasTiU * etTi * nMuTi * binWidth);
  double scTiD = (nSimTi * TiDSF) / (nMeasTiD * etTi * nMuTi * binWidth);
  double scPbU = (nSimPb * PbUSF) / (nMeasPbU * etPb * nMuPb * binWidth);
  double scPbD = (nSimPb * PbDSF) / (nMeasPbD * etPb * nMuPb * binWidth);

  //scale histograms
  AlUMeas->Scale(scAlU); AlDMeas->Scale(scAlD);
  TiUMeas->Scale(scTiU); TiDMeas->Scale(scTiD);
  PbUMeas->Scale(scPbU); PbDMeas->Scale(scPbD);

  //Some colors to make it nice
  AlUMeas->SetLineColor(3);
  AlDMeas->SetLineColor(8);
  TiUMeas->SetLineColor(4);
  TiDMeas->SetLineColor(7);
  PbUMeas->SetLineColor(2);
  PbDMeas->SetLineColor(6);


  //Label Axes (only necessary for first one plotted)
  AlUMeas->SetTitle("Unfolded Neutron Spectra");
  AlUMeas->GetXaxis()->SetTitle("Neutron Energy (MeV)");
  AlUMeas->GetYaxis()->SetTitle("count per muon captured per 0.1 MeV");
  //Set ranges (only necessary for first one plotted)
  AlUMeas->GetXaxis()->SetRangeUser(2.0, 11.5);
  AlUMeas->GetYaxis()->SetRangeUser(0.01, 5);



  gStyle->SetOptStat(0);

  //make histogram (log scale)
  TCanvas *c1 = new TCanvas("c1", "new canvas", 900, 600);
  gPad->SetLogy();
  //draw raw histograms
  AlUMeas->Draw();  AlDMeas->Draw("SAME"); TiUMeas->Draw("SAME");
  TiDMeas->Draw("SAME"); PbUMeas->Draw("SAME"); PbDMeas->Draw("SAME");

  //draw optional histograms

  //build Legend
  TLegend *l1 = new TLegend(0.75, 0.50, 0.9, 0.9);
  l1->AddEntry(AlUMeas, "Al1");
  l1->AddEntry(AlDMeas, "Al2");
  l1->AddEntry(TiUMeas, "Ti1");
  l1->AddEntry(TiDMeas, "Ti2");
  l1->AddEntry(PbUMeas, "Pb1");
  l1->AddEntry(PbDMeas, "Pb2");

  l1->Draw("SAME");

  c1->SaveAs("neutron_unfolded.png");
}

