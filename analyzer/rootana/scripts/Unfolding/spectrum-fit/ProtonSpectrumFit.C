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

struct ProtonSpectrumPdfParams {
  double T0_min;
  double T0_max;
  double k_min;
  double k_max;
  double Q_min;
  double Q_max;
  double Q2_min;
  double Q2_max;
  double ARatio_min;
  double ARatio_max;
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


class ProtonSpectrumFit {

public:
  ProtonSpectrumFit(RooWorkspace* workspace, std::string id_tag, std::string x_var) : ws(workspace), id(id_tag), xVar(x_var) { }

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
    double latex_y = hist_params.paramBox_y_max - 0.40;
    TLatex *t1 = new TLatex(latex_x, latex_y, Form("#chi^{2} / ndf = %f", chi_2));
    t1->SetNDC();
    t1->SetTextColor(hist_params.colour);
    t1->SetTextSize(0.04);
    t1->SetTextAlign(22);
    plot->addObject(t1);
    
    plot->Draw();
    t1->Draw();
  }

  ProtonSpectrumPdfParams pdf_params;
  EDepHistParams hist_params;

  double getT0Value() {
    return ws->var(T0Name.c_str())->getVal();
  }

  double getkValue() {
    return ws->var(kName.c_str())->getVal();
  }

  double getQValue() {
    return ws->var(QName.c_str())->getVal();
  }

  double getQ2Value() {
    return ws->var(Q2Name.c_str())->getVal();
  }

  double getARatioValue() {
    return ws->var(ARatioName.c_str())->getVal();
  }

private:
  RooWorkspace* ws;
  std::string id;

  std::string xVar;

  std::string fullPdfName;
  std::string T0Name;
  std::string kName;
  std::string QName;
  std::string Q2Name;
    std::string ARatioName;

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

    T0Name = "T0_" + id;
    kName = "k_" + id;
    QName = "Q_" + id;
    Q2Name = "Q2_" + id;
    ARatioName = "ARatio_" + id;

    fullPdfName = "proton_" + id;
    
    std::stringstream cmd;
    // Construct the punch-through pdf first (landau X gauss)
    cmd << "ProtonESpectrum::" << fullPdfName << "(" << xVar << ", " << T0Name << "[" << pdf_params.T0_min << "," << pdf_params.T0_max << "], " << kName << "[" << pdf_params.k_min << "," << pdf_params.k_max << "], " << QName << "[" << pdf_params.Q_min << "," << pdf_params.Q_max << "])";//, " << Q2Name << "[" << pdf_params.Q2_min << "," << pdf_params.Q2_max << "], " << ARatioName << "[" << pdf_params.ARatio_min << "," << pdf_params.ARatio_max << "])";
    ws->factory(cmd.str().c_str());
  }
  
  void niceParNames() {
    /*    std::string varname = "landau_punch_" + id + "_mean";
    ws->var(varname.c_str())->SetName("Landau Punch Mean");
    varname = "landau_punch_" + id + "_sigma";
    ws->var(varname.c_str())->SetName("Landau Punch Sigma");
  
    varname = "gauss_punch_" + id + "_sigma";
    ws->var(varname.c_str())->SetName("Gauss Punch Sigma");
    */
  }
};
