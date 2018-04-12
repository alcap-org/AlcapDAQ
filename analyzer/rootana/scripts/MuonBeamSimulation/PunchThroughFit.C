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

#include <sstream>

struct PunchThroughPdfParams {
  double landau_mean_min;
  double landau_mean_max;
  double landau_sigma_min;
  double landau_sigma_max;
  double gauss_sigma_min;
  double gauss_sigma_max;
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


class PunchThroughFit {

public:
  PunchThroughFit(RooWorkspace* workspace, std::string id_tag, std::string x_var) : ws(workspace), id(id_tag), xVar(x_var) { }

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
    
    ws->data(histName.c_str())->plotOn(plot);//, RooFit::MarkerColor(hist_params.colour));
    
    ws->pdf(fullPdfName.c_str())->plotOn(plot, RooFit::LineColor(hist_params.colour));
    ws->pdf(fullPdfName.c_str())->paramOn(plot, RooFit::Layout(hist_params.paramBox_x_min, hist_params.paramBox_x_max, hist_params.paramBox_y_max));
    plot->getAttText()->SetTextSize(0.03);
    plot->getAttLine()->SetLineWidth(0);
    plot->getAttFill()->SetFillStyle(0);
    plot->getAttLine()->SetLineColor(kWhite);
    plot->getAttText()->SetTextColor(hist_params.colour);
    
    int n_fit_params = 3;
    double chi_2 = plot->chiSquare(n_fit_params);
    double latex_x = hist_params.paramBox_x_min + (hist_params.paramBox_x_max - hist_params.paramBox_x_min) / 2 + 0.05;
    double latex_y = hist_params.paramBox_y_max - 0.20;
    TLatex *t1 = new TLatex(latex_x, latex_y, Form("#chi^{2} / ndf = %f", chi_2));
    t1->SetNDC();
    t1->SetTextColor(hist_params.colour);
    t1->SetTextSize(0.04);
    t1->SetTextAlign(22);
    plot->addObject(t1);
    
    plot->Draw();
    t1->Draw();
  }

  PunchThroughPdfParams pdf_params;
  EDepHistParams hist_params;

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

private:
  RooWorkspace* ws;
  std::string id;

  std::string xVar;

  std::string fullPdfName;
  std::string punchLandauName;
  std::string punchGaussName;

  std::string punchLandauMeanName;
  std::string punchLandauSigmaName;
  std::string punchGaussMeanName;
  std::string punchGaussSigmaName;

  std::string histName;
  RooDataHist* rooHist;

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

    fullPdfName = "punch_" + id;
    
    std::stringstream cmd;
    // Construct the punch-through pdf first (landau X gauss)
    cmd << "Landau::" << punchLandauName << "(" << xVar << ", " << punchLandauMeanName << "[" << pdf_params.landau_mean_min << "," << pdf_params.landau_mean_max << "], " << punchLandauSigmaName << "[" << pdf_params.landau_sigma_min << "," << pdf_params.landau_sigma_max << "])";
    ws->factory(cmd.str().c_str());

    cmd.str("");
    cmd << "Gaussian::" << punchGaussName << "(" << xVar << ", " << punchGaussMeanName << "[0], " << punchGaussSigmaName << "[" << pdf_params.gauss_sigma_min << "," << pdf_params.gauss_sigma_max << "])";
    ws->factory(cmd.str().c_str());

    cmd.str("");
    cmd << "FCONV::" << fullPdfName << "(" << xVar << "," << punchLandauName << ", " << punchGaussName << ")";
    ws->factory(cmd.str().c_str());

  }
  
  void niceParNames() {
    std::string varname = "landau_punch_" + id + "_mean";
    ws->var(varname.c_str())->SetName("Landau Punch Mean");
    varname = "landau_punch_" + id + "_sigma";
    ws->var(varname.c_str())->SetName("Landau Punch Sigma");
  
    varname = "gauss_punch_" + id + "_sigma";
    ws->var(varname.c_str())->SetName("Gauss Punch Sigma");
  }
};
