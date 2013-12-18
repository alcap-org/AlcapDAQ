#include "TString.h"
#include "TFile.h"
#include "TH1D.h"
#include "TH2F.h"
#include "TSpectrum.h"
#include "TLine.h"
#include "TBox.h"

#include <vector>

double xraycount(int runlow, int runhigh, bool draw = false) {

  // What we'll be using
  char fname[256];
  int nMuons = 0;
  double tVals[] = { -500., 500. };
  double aVals[] = { 2820., 2860. };
  TH1D* pyGeAmp = NULL;

  // Collect all the histograms of Ge-S heights
  // (pile-up protected and within 500ns of a muon hit on either side)
  for (int r = runlow; r <= runhigh; ++r) {
    sprintf(fname, "/net/abner/data/RunPSI2013/hist/his%05d.root", r);
    TFile fGe(fname, "READ");
    TH2F* hGeTDiffAmp = (TH2F*)fGe.Get("hMuSC_Ge-S_AmplitudeVsTdiff_PP");
    TH1D* hMuSCCount = (TH1D*)fGe.Get("muSC_count_raw");
    nMuons += hMuSCCount->GetBinContent(2);
    static int tBinLow = hGeTDiffAmp->GetXaxis()->FindBin(tVals[0]);
    static int tBinHigh = hGeTDiffAmp->GetXaxis()->FindBin(tVals[1]);
    if (pyGeAmp == NULL) {
      pyGeAmp = (TH1D*)hGeTDiffAmp->ProjectionY("_py",tBinLow,tBinHigh)->Clone("hGeHeights");
      pyGeAmp->SetDirectory(0);
    } else {
      TH1D* tmp = hGeTDiffAmp->ProjectionY("_py",tBinLow,tBinHigh);
      pyGeAmp->Add(tmp);
    }
    fGe.Close();
  }

  TSpectrum s;
  TH1* bgGe = s.Background(pyGeAmp);

  int aBinLow    = pyGeAmp->GetXaxis()->FindBin(aVals[0]);
  int aBinHigh   = pyGeAmp->GetXaxis()->FindBin(aVals[1]);
  double nStops  = pyGeAmp->Integral(aBinLow, aBinHigh);
  double rate    = nStops/(double)nMuons;
  double bg      = bgGe->Integral(aBinLow, aBinHigh);
  double bgsrate = (nStops - bg)/(double)nMuons;
  printf("Saw %d stops seen in Ge (with %d background) and %d muons in muSc\n", (int)nStops, (int)bg, nMuons);
  printf("Rate:    %g xrays/muons    (%g background subtracted)\n", rate, bgsrate);

  if (draw) {
    double geMax = pyGeAmp->GetMaximum();
    TLine l1(aVals[0], 0., aVals[0], geMax);
    TLine l2(aVals[1], 0., aVals[1], geMax);
    TBox b(aVals[0], 0., aVals[1], geMax);
    
    pyGeAmp->GetYaxis()->SetRangeUser(0.,geMax);
    pyGeAmp->SetStats(0);
    bgGe->SetStats(0);
    b.SetFillStyle(3004);
    b.SetFillColor(kBlue);

    pyGeAmp->DrawClone();
    bgGe->DrawClone("SAME");
    l1.DrawClone("SAME");
    l2.DrawClone("SAME");
    b.DrawClone("SAME");
  }
  

  return rate;
}
