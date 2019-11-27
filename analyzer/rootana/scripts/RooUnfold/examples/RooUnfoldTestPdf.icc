//=====================================================================-*-C++-*-
// File and Version Information:
//      $Id$
//
// Description:
//      MC Generation routines implemented without RooFit.
//      NB. This version generates peaked signal events over their full
//      range, so will have a fewer events within the range than requested,
//      especially if the |mean +/- width| is too large.
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
//        7: Gaussian Resonance on exponential decay background
//
// Author: Tim Adye <T.J.Adye@rl.ac.uk>
//
//==============================================================================

#ifndef ROOUNFOLDTESTPDF_ICC
#define ROOUNFOLDTESTPDF_ICC

#if !defined(__CINT__) || defined(__MAKECINT__)
#include <iostream>
#include <cmath>

#include "TH1D.h"
#include "TVectorD.h"
#include "TRandom.h"

using std::fabs;
using std::exp;
using std::log;
using std::pow;
using std::sqrt;
using std::cerr;
using std::endl;
#endif

Int_t RooUnfoldTestPdf (Int_t fpdf, Int_t nx, Double_t xlo, Double_t xhi, TVectorD& x,
                        TH1* h= 0, Double_t mean= 0.0, Double_t width= 2.5, Double_t bkg= 0.0, Int_t verbose= 0)
{
  static const Double_t pi= atan2 (0.0, -1.0);
  if (bkg<0.0) bkg= 0.0;
  if (bkg>=1.0) fpdf= 0;
  if (fpdf==0) bkg= 0.0;

  Int_t nb= h->GetNbinsX();
  const Double_t unorm= h ? nx * ((h->GetXaxis()->GetXmax() - h->GetXaxis()->GetXmin()) / nb) : 0;
  const Double_t xwidth= xhi-xlo, norm= unorm*(1.0-bkg), bnorm= unorm*bkg/xwidth;

  switch (fpdf) {

    case 0: {
      for (Int_t i= 0; i < nx; i++) {
        x[i]= xlo + xwidth*gRandom->Rndm();
      }
      if (h) {
        const Double_t fnorm= norm / xwidth;
        for (Int_t i= 1; i<=nb; i++) {
          h->SetBinContent(i, fnorm);
        }
      }
      return nx;
    }

    case 1: {
      for (Int_t i= 0; i < nx; i++) {
        if (gRandom->Rndm() < bkg)
          x[i]= xlo + xwidth*gRandom->Rndm();
        else
          x[i]= mean + 3.0*width * (2.0*gRandom->Rndm() - 1.0);
      }
      if (h) {
        const Double_t fnorm= bnorm + norm / (6.0*width);
        for (Int_t i= 1; i<=nb; i++) {
          Double_t z= h->GetBinCenter(i);
          if (fabs (z-mean) < 3.0*width) {
            h->SetBinContent(i, fnorm);
          } else {
            h->SetBinContent(i, bnorm);
          }
        }
      }
      return nx;
    }

    case 2: {
      for (Int_t i= 0; i < nx; i++) {
        if (gRandom->Rndm() < bkg)
          x[i]= xlo + xwidth*gRandom->Rndm();
        else
          x[i]= gRandom->Gaus (mean, width);
      }
      if (h) {
        const Double_t fnorm= norm / (sqrt(2*pi) * width);
        for (Int_t i= 1; i<=nb; i++) {
          Double_t z= h->GetBinCenter(i);
          if (fabs ((z-mean)/width) < 10.0) {
            h->SetBinContent(i, fnorm * exp (-0.5*(pow((z-mean)/width,2))) + bnorm);
          } else {
            h->SetBinContent(i, bnorm);
          }
        }
      }
      return nx;
    }

    case 3: {
      for (Int_t i= 0; i < nx; i++) {
        if (gRandom->Rndm() < bkg)
          x[i]= xlo + xwidth*gRandom->Rndm();
        else {
          Double_t z= gRandom->Exp (width);
          x[i]= gRandom->Rndm() < 0.5 ? mean - z : mean + z;
        }
      }
      if (h) {
        const Double_t fnorm= norm / (2.0*width);
        for (Int_t i= 1; i<=nb; i++) {
          Double_t z= h->GetBinCenter(i);
          h->SetBinContent (i, fnorm * exp (-fabs(z-mean)/width) + bnorm);
        }
      }
      return nx;
    }

    case 4: {
      for (Int_t i= 0; i < nx; i++) {
        if (gRandom->Rndm() < bkg)
          x[i]= xlo + xwidth*gRandom->Rndm();
        else
          x[i]= gRandom->BreitWigner (mean, width);
      }
      if (h) {
        const Double_t fnorm= norm * width / (2*pi);
        for (Int_t i= 1; i<=nb; i++) {
          Double_t z= h->GetBinCenter(i);
          h->SetBinContent (i, fnorm / (pow(z-mean,2) + 0.25*pow(width,2)) + bnorm);
        }
      }
      return nx;
    }

    case 5: {
      for (Int_t i= 0; i < nx; i++) {
        if (gRandom->Rndm() < bkg)
          x[i]= xlo + xwidth*gRandom->Rndm();
        else {
          Double_t m= gRandom->Rndm() < 0.5 ? mean - width : mean + width;
          x[i]= gRandom->BreitWigner (m, width);
        }
      }
      if (h) {
        const Double_t fnorm= norm * width / (4*pi);
        for (Int_t i= 1; i<=nb; i++) {
          Double_t z= h->GetBinCenter(i);
          h->SetBinContent (i, fnorm * (1 / (pow(z-mean+width,2) + 0.25*pow(width,2)) +
                                        1 / (pow(z-mean-width,2) + 0.25*pow(width,2))) + bnorm);
        }
      }
      return nx;
    }

    case 6: {
      const Double_t tau = mean-xlo;
      const Double_t ehi= exp(-xhi/tau), edif= exp(-xlo/tau)-ehi;
      for (Int_t i= 0; i < nx; i++) {
        if (gRandom->Rndm() < bkg)
          x[i]= xlo + xwidth*gRandom->Rndm();
        else {
          x[i]= -tau * log (ehi + edif * gRandom->Rndm());
        }
      }
      if (h) {
        const Double_t fnorm= norm / (tau * (exp(-xlo/tau) - exp(-xhi/tau)));
        if (verbose>=1) std::cout << "tau=" << tau << ", fnorm/norm=" << fnorm/norm << endl;
        for (Int_t i= 1; i<=nb; i++) {
          Double_t z= h->GetBinCenter(i);
          h->SetBinContent (i, fnorm * exp(-z/tau) + bnorm);
        }
      }
      return nx;
    }

    case 7: {
      const Double_t tau     = mean-xlo;
      const Double_t ehi     = exp(-xhi/tau), edif= exp(-xlo/tau)-ehi;
      const Double_t mywidth = width/4.0;
      const Double_t fexp    = 0.9;

      for (Int_t i= 0; i < nx; i++) {
        if (gRandom->Rndm() < bkg)
          x[i]= xlo + xwidth*gRandom->Rndm();
        else {
          if (gRandom->Rndm() < fexp)
            x[i]= -tau * log (ehi + edif * gRandom->Rndm());
          else
            x[i]= gRandom->Gaus (mean, mywidth);
        }
      }
      if (h) {
        const Double_t rnorm= norm / (sqrt(2*pi) * mywidth);
        const Double_t enorm= norm / (tau * (exp(-xlo/tau) - exp(-xhi/tau)));
        for (Int_t i= 1; i<=nb; i++) {
          Double_t z= h->GetBinCenter(i);
          Double_t resonance;
          if (fabs ((z-mean)/mywidth) < 10.0)
            resonance= rnorm * exp (-0.5*(pow((z-mean)/mywidth,2)));
          else
            resonance= 0.0;
          Double_t background = enorm * exp(-z/tau);
          h->SetBinContent (i, ((1.0-fexp)*resonance + fexp*background) + bnorm);
        }
      }
      return nx;
    }

    default: {
      cerr << "PDF " << fpdf << " not defined" << endl;
      return 0;
    }

  }
}
#endif
