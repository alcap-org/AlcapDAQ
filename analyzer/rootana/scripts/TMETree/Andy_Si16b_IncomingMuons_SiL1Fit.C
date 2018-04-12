#include "RooWorkspace.h"
#include "RooPlot.h"
#include "RooDataHist.h"
#include "RooAbsPdf.h"
#include "RooRealVar.h"

#include "TFile.h"
#include "TH1.h"
#include "TCanvas.h"
#include "TLatex.h"

#include "RooLandau.h"
#include "RooGaussian.h"
#include "RooFFTConvPdf.h"
#include "RooHist.h"
#include "RooGaussExp.h"

std::string ws_histname(std::string id) {
  return "hist_"+id;
}

std::string ws_pdfname(std::string id) {
  return "sum_"+id;
}

std::string landauCommand(std::string id) {
  std::string cmd = "Landau::landau_punch_" + id + "(edep, landau_punch_" + id + "_mean[500,700], landau_punch_" + id + "_sigma[0,100])";
  return cmd;
}

std::string gaussianCommand(std::string id) {
  std::string cmd = "Gaussian::gauss_punch_" + id + "(edep, gauss_" + id + "_mean[0], gauss_" + id + "_sigma[0,500])";
  return cmd;
}

std::string punchPdfCommand(std::string id) {
  std::string cmd = "FCONV::punch_" + id + "(edep,landau_punch_" + id + ", gauss_punch_" + id + ")";
  return cmd;
}

std::string crystalBallCommand(std::string id) {
  std::string cmd = "GaussExp::cball_" + id + "(edep, cball_" + id + "_mean[800,1000], cball_" + id + "_sigma[0,300], cball_" + id + "_alpha[0,1])";//, cball_" + id + "_n[0,10])";
  return cmd;
}

std::string sumCommand(std::string id) {
  //  std::string cmd = "SUM::sum_" + id + "(npunch_"+id+"[0, 10000000]*punch_" + id +", nstop_" + id + "[0,30000000]*cball_" + id + ")";
  std::string cmd = "SUM::sum_" + id + "(npunch_"+id+"[0, 30000000]*punch_" + id +", nstop_" + id + "[0,30000000]*cball_" + id + ")";
  return cmd;
}

void PunchThroughFit(RooWorkspace& ws, std::string filename, std::string histname, std::string id, int rebin_factor = 1) {
  TFile* file = new TFile(filename.c_str(), "READ");
  TH1F* hCentralMuonEnergies = (TH1F*) file->Get(histname.c_str());
  hCentralMuonEnergies->Rebin(rebin_factor);

  ws.factory(landauCommand(id).c_str());
  ws.factory(gaussianCommand(id).c_str());
  ws.factory(punchPdfCommand(id).c_str());

  ws.factory(crystalBallCommand(id).c_str());

  ws.factory(sumCommand(id).c_str());

  RooDataHist hist(ws_histname(id).c_str(), "SiL1 Energy", (ws.argSet("edep")), hCentralMuonEnergies);
  ws.import(hist);
  
  ws.pdf(ws_pdfname(id).c_str())->fitTo(hist);

}

void DrawDataAndFit(RooWorkspace& ws, std::string id, Int_t colour) {

  std::string canvasname = "c_"+id;
  TCanvas* c1 = new TCanvas(canvasname.c_str(), canvasname.c_str());

  std::string title = "SiL1 Energy Deposits (" + id + ")";
  RooPlot* plot = ws.var("edep")->frame(RooFit::Title(title.c_str()));

  ws.data(ws_histname(id).c_str())->plotOn(plot, RooFit::MarkerColor(colour));

  ws.pdf(ws_pdfname(id).c_str())->plotOn(plot, RooFit::LineColor(colour));
  std::string cball_name = "cball_"+id;
  ws.pdf(ws_pdfname(id).c_str())->plotOn(plot, RooFit::Components(ws.argSet(cball_name.c_str())), RooFit::LineColor(colour), RooFit::LineStyle(kDashed));
  std::string punch_name = "punch_"+id;
  ws.pdf(ws_pdfname(id).c_str())->plotOn(plot, RooFit::Components(ws.argSet(punch_name.c_str())), RooFit::LineColor(colour), RooFit::LineStyle(kDashed));

  //  ws.pdf(ws_pdfname(id).c_str())->paramOn(plot, RooFit::Layout(0.40));

  int n_fit_params = 8;
  double chi_2 = plot->chiSquare(n_fit_params);
  std::cout << "AE: Chi^2 = " << chi_2 << std::endl;
  TLatex *t1 = new TLatex(350, 10000, Form("#chi^{2} = %f", chi_2));
  plot->addObject(t1);

  /*
  RooHist* resid = plot->residHist();
  RooPlot* plot_resid = (ws.var("edep")->frame(RooFit::Title("MC Residual")));
  plot_resid->addPlotable(resid);
  */
  //  c1->Divide(1,2);
  //  c1->cd(1);
  
  plot->Draw();
  //  t1->Draw();
}
void Andy_Si16b_IncomingMuons_SiL1Fit() {

  RooWorkspace ws("ws");
  ws.factory("edep[300,1500]");
  PunchThroughFit(ws, "Andy_Si16_MC_SiL1Muons.root", "hSiL1_mu", "MC", 10);
  PunchThroughFit(ws, "Andy_Si16b_IncomingMuons_SiL1.root", "hSiL1Energies", "data", 10);

  DrawDataAndFit(ws, "MC", kRed);
  DrawDataAndFit(ws, "data", kBlue);
  
 
  /*  TCanvas* c3 = new TCanvas("c3", "c3");
  RooPlot* plot_data_vs_mc = ws.var("edep")->frame(RooFit::Title("SiL1 Punch Through Peak Fits (Data and MC)"));
  ws.pdf(ws_pdfname("MC").c_str())->plotOn(plot_data_vs_mc, RooFit::LineColor(kRed));
  ws.pdf(ws_pdfname("data").c_str())->plotOn(plot_data_vs_mc);
  plot_data_vs_mc->Draw();
  */
  ws.Print();
}
