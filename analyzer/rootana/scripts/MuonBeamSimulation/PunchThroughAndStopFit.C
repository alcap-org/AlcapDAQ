#include "RooPlot.h"
#include "RooDataHist.h"
#include "RooRealVar.h"

#include "TFile.h"
#include "TH1.h"
#include "TCanvas.h"
#include "TLatex.h"

#include "RooHist.h"

#include "RooWorkspace.h"
#include "RooAbsPdf.h"
#include "RooLandau.h"
#include "RooGaussian.h"
#include "RooFFTConvPdf.h"
#include "RooAddPdf.h"
#include "scripts/MuonBeamSimulation/RooGaussExp.h"

#include <sstream>

struct PunchThroughAndStopPdfParams {
  double landau_mean_min;
  double landau_mean_max;
  double landau_sigma_min;
  double landau_sigma_max;
  double gauss_sigma_min;
  double gauss_sigma_max;

  double stop_edep0_min;
  double stop_edep0_max;
  double stop_sigma_min;
  double stop_sigma_max;
  double stop_alpha_min;
  double stop_alpha_max;

  int n_punch_min;
  int n_punch_max;
  int n_stop_min;
  int n_stop_max;
};

struct EDepHistParams {
  int rebin_factor;
  std::string histname;
  std::string histtitle;
  Int_t colour;
  double paramBox_x_min;
  double paramBox_x_max;
  double paramBox_y_max;
};


class PunchThroughAndStopFit {

public:
  PunchThroughAndStopFit(RooWorkspace* workspace, std::string id_tag, std::string x_var) : ws(workspace), id(id_tag), xVar(x_var) { }

  void initialise(std::string filename, std::string histname) {
    createHistogram(filename, histname);
    createPdf();
  }
  
  void performFit() {
    ws->pdf(fullPdfName.c_str())->fitTo(*rooHist);
  }

  void draw() {
    //    niceParNames();
    
    std::string canvasname = "c_"+id;
    TCanvas* c1 = new TCanvas(canvasname.c_str(), canvasname.c_str());
    
    RooPlot* plot = ws->var(xVar.c_str())->frame(RooFit::Title(hist_params.histtitle.c_str()));
    
    ws->data(histName.c_str())->plotOn(plot, RooFit::Name(histName.c_str()));
    
    ws->pdf(fullPdfName.c_str())->plotOn(plot, RooFit::Name(fullPdfName.c_str()), RooFit::LineColor(hist_params.colour));
    ws->pdf(fullPdfName.c_str())->plotOn(plot, RooFit::Name(fullStopName.c_str()), RooFit::Components(ws->argSet(fullStopName.c_str())), RooFit::LineColor(hist_params.colour), RooFit::LineStyle(kDashed));
    ws->pdf(fullPdfName.c_str())->plotOn(plot, RooFit::Name(fullPunchName.c_str()), RooFit::Components(ws->argSet(fullPunchName.c_str())), RooFit::LineColor(hist_params.colour), RooFit::LineStyle(kDashed));
    ws->pdf(fullPdfName.c_str())->paramOn(plot, RooFit::Layout(hist_params.paramBox_x_min, hist_params.paramBox_x_max, hist_params.paramBox_y_max));
    plot->getAttText()->SetTextSize(0.03);
    plot->getAttLine()->SetLineWidth(0);
    plot->getAttFill()->SetFillStyle(0);
    plot->getAttLine()->SetLineColor(kWhite);
    plot->getAttText()->SetTextColor(hist_params.colour);

    RooArgSet* floatingPars = ws->pdf(fullPdfName.c_str())->getParameters(*(ws->data(histName.c_str())));
    int n_fit_params = (floatingPars->selectByAttrib("Constant", kFALSE))->getSize();
    std::cout << "AE: n_fit_params = " << n_fit_params << std::endl;
    double chi_2 = plot->chiSquare(fullPdfName.c_str(), histName.c_str(), n_fit_params);
    double latex_x = hist_params.paramBox_x_min + (hist_params.paramBox_x_max - hist_params.paramBox_x_min) / 2 + 0.05;
    double latex_y = hist_params.paramBox_y_max - 0.53;
    TLatex *t1 = new TLatex(latex_x, latex_y, Form("#chi^{2} / ndf = %f", chi_2));
    t1->SetNDC();
    t1->SetTextColor(hist_params.colour);
    t1->SetTextSize(0.04);
    t1->SetTextAlign(22);
    plot->addObject(t1);
    
    plot->Draw();
    t1->Draw();
  }

  void setParConstant(std::string parname) {
    ws->var(parname.c_str())->setConstant(kTRUE);
  }
  
  double getPunchLandauMeanValue() {
    return ws->var(punchLandauMeanName.c_str())->getVal();
  }

  double getPunchLandauSigmaValue() {
    return ws->var(punchLandauSigmaName.c_str())->getVal();
  }

  double getPunchGaussMeanValue() {
    return ws->var(punchGaussMeanName.c_str())->getVal();
  }

  double getPunchGaussSigmaValue() {
    return ws->var(punchGaussSigmaName.c_str())->getVal();
  }

  double getStopGaussExpEDep0Value() {
    return ws->var(stopGaussExpEDep0Name.c_str())->getVal();
  }

  double getStopGaussExpSigmaValue() {
    return ws->var(stopGaussExpSigmaName.c_str())->getVal();
  }

  double getStopGaussExpAlphaValue() {
    return ws->var(stopGaussExpAlphaName.c_str())->getVal();
  }

  double getNPunchValue() {
    return ws->var(nPunchName.c_str())->getVal();
  }

  double getNStopValue() {
    return ws->var(nStopName.c_str())->getVal();
  }



  PunchThroughAndStopPdfParams pdf_params;
  EDepHistParams hist_params;

  std::string punchLandauMeanName;
  std::string punchLandauSigmaName;
  std::string punchLandauName;

  std::string punchGaussMeanName;
  std::string punchGaussSigmaName;
  std::string punchGaussName;

  std::string fullPunchName;

  std::string stopGaussExpEDep0Name;
  std::string stopGaussExpSigmaName;
  std::string stopGaussExpAlphaName;  
  std::string stopGaussExpName;
  
  std::string fullStopName;

  std::string nPunchName;
  std::string nStopName;
  
  std::string fullPdfName;

private:
  RooWorkspace* ws;
  std::string id;

  std::string xVar;

  
  RooDataHist* rooHist;

  std::string histName;

  void createHistogram(std::string filename, std::string histname) {
    TFile* file = new TFile(filename.c_str(), "READ");
    TH1F* hist = (TH1F*) file->Get(histname.c_str());
    hist->Rebin(hist_params.rebin_factor);

    histName = hist_params.histname + "_" + id;
    rooHist = new RooDataHist(histName.c_str(), hist_params.histtitle.c_str(), (ws->argSet(xVar.c_str())), hist);
    ws->import(*rooHist);
  }

  void createPdf() {

    punchLandauMeanName = "landau_punch_" + id + "_mean";
    punchLandauSigmaName = "landau_punch_" + id + "_sigma";
    punchLandauName = "landau_punch_" + id;
    
    punchGaussMeanName = "gauss_punch_" + id + "_mean";
    punchGaussSigmaName = "gauss_punch_" + id + "_sigma";
    punchGaussName = "gauss_punch_" + id;

    fullPunchName = "punch_" + id;

    stopGaussExpEDep0Name = "stop_edep0_" + id;
    stopGaussExpSigmaName = "stop_sigma_" + id;
    stopGaussExpAlphaName = "stop_alpha_" + id;
    stopGaussExpName = "stop_gaussexp_" + id;
    fullStopName = "stop_" + id;

    nPunchName = "n_punch_" + id;
    nStopName = "n_stop_" + id;
    fullPdfName = "punchandstop_" + id;
    std::stringstream cmd;
    // Construct the punch-through pdf first (landau X gauss)
    cmd << "Landau::" << punchLandauName << "(" << xVar << ", " << punchLandauMeanName << "[" << pdf_params.landau_mean_min << "," << pdf_params.landau_mean_max << "], " << punchLandauSigmaName << "[" << pdf_params.landau_sigma_min << "," << pdf_params.landau_sigma_max << "])";
    ws->factory(cmd.str().c_str());

    cmd.str("");
    cmd << "Gaussian::" << punchGaussName << "(" << xVar << ", " << punchGaussMeanName << "[0], " << punchGaussSigmaName << "[" << pdf_params.gauss_sigma_min << "," << pdf_params.gauss_sigma_max << "])";
    ws->factory(cmd.str().c_str());

    cmd.str("");
    cmd << "FCONV::" << fullPunchName << "(" << xVar << "," << punchLandauName << ", " << punchGaussName << ")";
    ws->factory(cmd.str().c_str());

    // Now construct the stop pdf (GaussExp)
    cmd.str("");
    cmd << "GaussExp::" << stopGaussExpName << "(" << xVar << ", " << stopGaussExpEDep0Name << "[" << pdf_params.stop_edep0_min << ", " << pdf_params.stop_edep0_max << "], " << stopGaussExpSigmaName << "[" << pdf_params.stop_sigma_min << ", " << pdf_params.stop_sigma_max << "], " << stopGaussExpAlphaName << "[" << pdf_params.stop_alpha_min << ", " << pdf_params.stop_alpha_max << "])";
    ws->factory(cmd.str().c_str());

    cmd.str("");
    cmd << "FCONV::" << fullStopName << "(" << xVar << "," << stopGaussExpName << ", " << punchGaussName << ")";
    ws->factory(cmd.str().c_str());

    // Now sum them...
    cmd.str("");
    cmd << "SUM::" << fullPdfName << "(" << nPunchName << "[" << pdf_params.n_punch_min << ", " << pdf_params.n_punch_max << "]*" << fullPunchName << ", " << nStopName << "[" << pdf_params.n_stop_min << ", " << pdf_params.n_stop_max << "]*" << fullStopName << ")";
    ws->factory(cmd.str().c_str());
  }
  
  void niceParNames() {
    std::string varname = "landau_punch_" + id + "_mean";
    ws->var(varname.c_str())->SetName("Landau Punch Mean");
    varname = "landau_punch_" + id + "_sigma";
    ws->var(varname.c_str())->SetName("Landau Punch Sigma");
  
    varname = "gauss_punch_" + id + "_sigma";
    ws->var(varname.c_str())->SetName("Gauss Punch Sigma");

    varname = "stop_edep0_" + id;
    ws->var(varname.c_str())->SetName("Stop Peak");
    varname = "stop_sigma_" + id;
    ws->var(varname.c_str())->SetName("Stop Sigma");
    varname = "stop_alpha_" + id;
    ws->var(varname.c_str())->SetName("Stop Alpha");
    
    varname = "n_punch_" + id;
    ws->var(varname.c_str())->SetName("N Punch");
    varname = "n_stop_" + id;
    ws->var(varname.c_str())->SetName("N Stop");
  }
};
