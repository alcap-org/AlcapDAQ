//=====================================================================-*-C++-*-
// File and Version Information:
//      $Id$
//
// Description:
//      MC Generation routines implemented with RooFit.
//
//      The PDFs are defined by fpdf as follows:-
//
//        0: flat distribution
//        1: top-hat distribution over mean +/- 3*width.
//        2: Gaussian
//        3: Double-sided exponential decay
//        4: Breit-Wigner
//        5: Double Breit-Wigner, peaking at mean-width and mean+width.
//        6: exponential
//        7: Gaussian resonance on an exponential background
//
// Author: Tim Adye <T.J.Adye@rl.ac.uk>
//
//==============================================================================

#ifndef ROOUNFOLDTESTPDFROOFIT_ICC
#define ROOUNFOLDTESTPDFROOFIT_ICC

#if !defined(__CINT__) || defined(__MAKECINT__)
#include <iostream>
using std::cout;
using std::cerr;
using std::endl;

#include "TH1.h"
#include "TVectorD.h"
#endif

#include "RooArgList.h"
#include "RooAbsPdf.h"
#include "RooRealVar.h"
#include "RooArgSet.h"
#include "RooDataSet.h"
#include "RooAddPdf.h"
#include "RooTruthModel.h"

#include "RooGaussian.h"
#include "RooExponential.h"
#include "RooDecay.h"
#include "RooBreitWigner.h"
#include "RooParametricStepFunction.h"
#include "RooChebychev.h"

Int_t mcgenpdf2 (const RooAbsPdf& pdf, RooRealVar& xvar, Int_t nx,
                 TVectorD& x, TH1* h= 0, Double_t offset= 0.0, Int_t verbose= 0)
{
  using namespace RooFit;

  if      (verbose>=2) pdf.Print("t");
  else if (verbose>=1) pdf.Print();
  const RooDataSet* ds= pdf.generate (xvar, nx);

  Int_t ngen= ds->numEntries();
  const RooRealVar* dsx= dynamic_cast<RooRealVar*> (ds->get()->find (xvar.GetName()));
  for (Int_t i= 0; i<ngen; i++) {
    ds->get (i);
    x[i]= dsx->getVal()+offset;
  }
  if (h) {
    Int_t nb= h->GetNbinsX();
    RooArgSet a(xvar);
    Double_t norm= nx * ((h->GetXaxis()->GetXmax() - h->GetXaxis()->GetXmin()) / nb);
    if (verbose>=1) cout << "fill histogram " << h->GetName() << ": norm=" << norm << ", offset=" << offset << endl;
    for (Int_t i= 1; i<=nb; i++) {
      xvar.setVal (h->GetBinCenter(i)-offset);
      Double_t v= pdf.getVal(&a);
      h->SetBinContent(i, norm*v);
    }
    if (verbose>=1) cout << "histogram " << h->GetName() << " filled" << endl;
  }
  delete ds;
  return ngen;
}

Int_t mcgenpdf (const RooAbsPdf& sigpdf, RooRealVar& xvar, Int_t nx,
                TVectorD& x, TH1* h= 0, Double_t bkg= 0.0, Double_t offset= 0.0, Int_t verbose= 0)
{
  if (bkg<=0.0) {
    return mcgenpdf2 (sigpdf,xvar,nx,x,h,offset,verbose);
  } else {
    using namespace RooFit;
    RooChebychev bkgpdf("bkgpdf","background PDF",xvar,RooArgList());
    if (bkg>=1.0) return mcgenpdf2 (bkgpdf,xvar,nx,x,h,offset,verbose);
    RooRealVar fbkg("fbkg","background fraction",bkg,0.0,1.0);
    TString pdfname (sigpdf.GetName());
    pdfname += "+bkg";
    return mcgenpdf2 (RooAddPdf (pdfname,sigpdf.GetName(),RooArgList(bkgpdf,sigpdf),fbkg),
                      xvar,nx,x,h,offset,verbose);
  }
}

// Generate truth distribution.
// RooFit only used inside here and helper routine, mcgenpdf.
Int_t RooUnfoldTestPdf (Int_t fpdf, Int_t nx, Double_t xlo, Double_t xhi, TVectorD& x,
                        TH1* h= 0, Double_t mean= 0.0, Double_t width= 2.5, Double_t bkg= 0.0, Int_t verbose= 0)
{
  using namespace RooFit;

  const Double_t offset= (fpdf==3 ? mean : 0.0);
  RooRealVar xvar("xvar","xvar",xlo-offset,xhi-offset);
  switch (fpdf) {

    case 0: {
      return mcgenpdf2 (RooChebychev("pdf","constant",xvar,RooArgList()),
                        xvar,nx,x,h,offset,verbose);
    }

    case 1: {
      const Int_t stepbins= 3;
      const Double_t stepa[]= { xlo, mean-3*width, mean+3*width, xhi };
      TArrayD steps(stepbins+1,stepa);
      RooRealVar step0("binHeight0","bin 0 Value",0.0);
      RooRealVar step1("binHeight1","bin 1 Value",1.0/(steps[2]-steps[1]));
      RooRealVar step2("binHeight2","bin 2 Value",0.0);
      RooArgList steplist(step0, step1, step2, "steplist");
      return mcgenpdf (RooParametricStepFunction("pdf","top hat",xvar,steplist,steps,stepbins),
                       xvar, nx, x, h, bkg, 0.0, verbose);
    }

    case 2: {
      RooRealVar p51("p51","mean",mean);
      RooRealVar p52("p52","width",width);
      return mcgenpdf (RooGaussian("pdf","Gaussian",xvar,p51,p52),
                       xvar, nx, x, h, bkg, 0.0, verbose);
    }

    case 3: {   // NB. xvar limits offset by mean above
      RooRealVar p21("p21","tau",width);
      RooTruthModel dectm("dectm","decay truth model",xvar);
      return mcgenpdf (RooDecay("pdf","decay",xvar,p21,dectm,RooDecay::DoubleSided),
                       xvar, nx, x, h, bkg, mean, verbose);
    }

    case 4: {
      RooRealVar p11("p11","mean",mean);
      RooRealVar p12("p12","width",width);
      return mcgenpdf (RooBreitWigner("pdf","Breit Wigner",xvar,p11,p12),
                       xvar, nx, x, h, bkg, 0.0, verbose);
    }

    case 5: {
      RooRealVar p31("p31","mean 1",mean-1.5*width);
      RooRealVar p32("p32","width 1",width);
      RooRealVar p33("p33","mean 2",mean+1.5*width);
      RooRealVar p34("p34","width 2",width);
      RooRealVar fbw1("fbw1","Breit Wigner 1 fraction",0.5);
      RooBreitWigner bw1("pdf1","Breit Wigner 1",xvar,p31,p32);
      RooBreitWigner bw2("pdf2","Breit Wigner 2",xvar,p33,p34);
      return mcgenpdf (RooAddPdf("pdf","Two Breit Wigners",RooArgList(bw1,bw2),RooArgList(fbw1)),
                       xvar, nx, x, h, bkg, 0.0, verbose);
    }

    case 6: {
      const Double_t invtau  = -1.0/(mean-xlo);
      RooRealVar ptau("ptau","inverse tau", invtau);
      return mcgenpdf (RooExponential("pdf1","Exponential",xvar,ptau),
                       xvar, nx, x, h, bkg, 0.0, verbose);
   }

    case 7: {
      const Double_t invtau  = -1.0/(mean-xlo);
      const Double_t mywidth = width/4.0;
      const Double_t fexp    = 0.9;

      // Background
      RooRealVar ptau("ptau","inverse tau", invtau);
      RooExponential expon("pdf1","Exponential",xvar,ptau);
      RooRealVar fexpvar("fexp","Background fraction",fexp);

      // Resonance
      RooRealVar p31("p31","mean",mean);
      RooRealVar p32("p32","width", mywidth);
      RooGaussian gaus("pdf2","Gaussian",xvar,p31,p32);

      return mcgenpdf (RooAddPdf("pdf","Resonace+background",RooArgList(expon,gaus),RooArgList(fexpvar)),
                       xvar, nx, x, h, bkg, 0.0, verbose);

   }

    default: {
      cerr << "PDF " << fpdf << " not defined" << endl;
      return 0;
    }
  }
}

#endif
