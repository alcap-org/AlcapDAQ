
/*** frequently used inlcudes ***/

#include <iostream>
#include <fstream>
#include <iomanip>

#include "TH1.h"
#include "TH2.h"
#include "TCanvas.h"
#include "TPad.h"
#include "TROOT.h"
#include "TStyle.h"
#include "TSystem.h"
#include "TColor.h"

void init()
{

  gStyle->SetPalette(1,0); // pretty palette
  gROOT->GetColor( 0)->SetRGB(255.,255.,255.0);
  gROOT->GetColor(10)->SetRGB(255.,255.,255.0);
  gROOT->GetColor(41)->SetRGB(204./255.,100./255.,0);
  gROOT->GetColor(44)->SetRGB(102./255.,0,150./255.);
  gROOT->GetColor(43)->SetRGB(0,0.5,0);
  gROOT->GetColor(42)->SetRGB(0,0,.5);
  gROOT->GetColor(45)->SetRGB(.5,0,0);
  gROOT->GetColor(46)->SetRGB(1,0.7,0);
  gROOT->GetColor(47)->SetRGB(102./255.,51./255.,0);
  gROOT->GetColor(48)->SetRGB(1,.5,1);

  gStyle->SetPadBorderMode(0);
  gStyle->SetCanvasBorderMode(0);

  gStyle->SetPadRightMargin(0.06);
  gStyle->SetPadLeftMargin(0.09);
  gStyle->SetPadTopMargin(0.08);
  gStyle->SetPadBottomMargin(0.09);

}

