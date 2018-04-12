#include "RooWorkspace.h"
#include "RooPlot.h"
#include "RooDataHist.h"
#include "RooAbsPdf.h"
#include "RooRealVar.h"

#include "TFile.h"
#include "TH1.h"
#include "TCanvas.h"

#include "RooLandau.h"
#include "RooGaussian.h"
#include "RooFFTConvPdf.h"
#include "RooAddPdf.h"
#include "RooGaussExp.h"
#include "RooExponential.h"

void Andy_Si16b_IncomingMuons_Fit() {
  
  std::string filename = "Andy_Si16b_IncomingMuons.root";
  //  std::string filename = "Andy_Si16_MC_SiTMuons.root";

  TFile* file = new TFile(filename.c_str(), "READ");

  TH1F* hCentralMuonEnergies = (TH1F*) file->Get("hCentralMuonEnergies");
  //  hCentralMuonEnergies->Rebin(2);
  //  TH1F* hCentralMuonEnergies = (TH1F*) file->Get("hSiT_mu");
  //  hCentralMuonEnergies->Rebin(5);

  //  RooWorkspace ws("ws");
  RooRealVar edep("edep", "edep", 200,1200);

  RooRealVar landau_punch_mean("landau_punch_mean", "landau_punch_mean", 200, 500);
  RooRealVar landau_punch_sigma("landau_punch_sigma", "landau_punch_sigma", 0, 100);
  RooLandau landau_punch("landau_punch", "landau_punch", edep, landau_punch_mean, landau_punch_sigma);

  RooRealVar gauss_punch_mean("gauss_punch_mean", "gauss_punch_mean", 0);
  RooRealVar gauss_punch_sigma("gauss_punch_sigma", "gauss_punch_sigma", 0, 500);
  RooGaussian gauss_punch("gauss_punch", "gauss_punch", edep, gauss_punch_mean, gauss_punch_sigma);

  RooFFTConvPdf punch("punch", "punch", edep, landau_punch, gauss_punch);
  
  //  ws.factory("Landau::landau_punch(edep[200,1200], landau_punch_mean[200,500], landau_punch_sigma[0,100])");
  //  ws.factory("Gaussian::gauss_punch(edep, gauss_punch_mean[0], gauss_punch_sigma[0,500])");
  //  ws.factory("FCONV::punch(edep,landau_punch, gauss_punch)");

  //  //  ws.factory("Landau::landau_stop(edep[700,1100], landau_stop_mean[800,1100], landau_stop_sigma[0,100])");
  //  //  ws.factory("Exponential::exp_stop(edep[700,1100], exp_stop_slope[0,1000])");
  //  //  ws.factory("Gaussian::gauss_stop(edep, gauss_stop_mean[0], gauss_stop_sigma[0,500])");
  //  //  ws.factory("FCONV::stop(edep, exp_stop, gauss_stop)");
  //  ws.factory("GaussExp::stop(edep, stop_edep[800,1000], stop_sigma[1,500], stop_alpha[700,1000])");
  RooRealVar stop_edep("stop_edep", "stop_edep", 850,1000);
  RooRealVar stop_sigma("stop_sigma", "stop_sigma", 1,500);
  RooRealVar stop_alpha("stop_alpha", "stop_alpha", 100,800);
  RooGaussExp stop("stop", "stop", edep, stop_edep, stop_sigma, stop_alpha);

  
  //  ws.factory("SUM::sum(nbkg[1,10000000]*punch, nsig[1,1000000]*stop)");
  RooRealVar nsig("nsig", "nsig", 1, 1000000000);
  RooRealVar nbkg("nbkg", "nbkg", 1, 1000000000);
  RooAddPdf sum("sum", "sum", RooArgList(punch, stop), RooArgList(nsig, nbkg));
  
  //  RooDataHist data("data", "SiT Energy", (ws.argSet("edep")), hCentralMuonEnergies);
  RooDataHist data("data", "SiT Energy", edep, hCentralMuonEnergies);
  //  ws.import(data);
  //  (ws.pdf("stop"))->fitTo(data);
  sum.fitTo(data);

  TCanvas* c1 = new TCanvas("c1", "c1");
  //  RooPlot* plot = ws.var("edep")->frame();
  RooPlot* plot = edep.frame();
  data.plotOn(plot);
  sum.plotOn(plot);
  sum.plotOn(plot, RooFit::Components(punch), RooFit::LineStyle(kDashed));
  sum.plotOn(plot, RooFit::Components(stop), RooFit::LineStyle(kDashed), RooFit::LineColor(kRed));

  //  (ws.pdf("stop"))->plotOn(plot);
  //  (ws.pdf("sum"))->plotOn(plot, RooFit::Components(ws.argSet("punch")), RooFit::LineStyle(kDashed));
  //  (ws.pdf("sum"))->plotOn(plot, RooFit::Components(ws.argSet("stop")), RooFit::LineStyle(kDashed), RooFit::LineColor(kRed));
  
  plot->Draw();
}
