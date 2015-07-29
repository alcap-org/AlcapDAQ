#include <vector>
#include "TF1.h"
#include "TH1I.h"
#include "TFile.h"
#include "TSpectrum.h"
#include "TGraphErrors.h"
#include "TCanvas.h"
#include <iostream>

TH1 * getCalSpec(TH1 * h, double slope = 0.1219, double offset = 1.621)
{
	TString org_name = h->GetName();
	TString cal_name = org_name.Append("_calibrated");
  TH1 * htmp = h->Clone();
  htmp->SetName(cal_name);

  const int nbins = htmp->GetXaxis()->GetNbins();
  double *new_bins;
  new_bins = malloc((nbins+1)*sizeof(*new_bins));

  for (int i = 0; i <= nbins; ++i)
  {
    new_bins[i] = slope*(htmp->GetBinLowEdge(i+1)) + offset;
    //new_bins[i] = slope*(htmp->GetBinUpEdge(i+1)) + offset;
  }
  htmp->SetBins(nbins, new_bins);

  return htmp;
}

TH2F * getCalSpec2D(TH2F *h, double xslope, double xoffset,
    double yslope, double yoffset)
{
	TString org_name = h->GetName();
	TString cal_name = org_name.Append("_calibrated");
  TH2F * htmp = h->Clone();
  htmp->SetName(cal_name);
  // X axis ...
  const int nbinsX = h->GetXaxis()->GetNbins();
  double *new_binsX;
  new_binsX = malloc((nbinsX + 1)*sizeof(*new_binsX));
  for (int i = 0; i <= nbinsX; ++i)
    new_binsX[i] = xslope*(h->GetXaxis()->GetBinLowEdge(i+1))
      + xoffset;

  // transform this Y axis
  const int nbinsY = h->GetYaxis()->GetNbins();
  double *new_binsY;
  new_binsY = malloc((nbinsY + 1)*sizeof(*new_binsY));
  for (int i = 0; i <= nbinsY; ++i)
    new_binsY[i] = yslope*(h->GetYaxis()->GetBinLowEdge(i+1)) 
      + yoffset;

  htmp->SetBins(nbinsX, new_binsX, nbinsY, new_binsY);
  return htmp;
}

TH1F* getGeCalibratedSpec(TH1F* h, double slope = 0.1219, 
    double constant = 1.1621)
{
	TString org_name = h->GetName();
	TString cal_name = org_name.Append("_calibrated");

	TString org_title = h->GetTitle();
	TString cal_title = org_title;

	TH1F * h_cal = new TH1F(cal_name, cal_title, 4096,0,2048);
	//h_cal->Print();
	//h_cal->SetBit(TH1::kCanRebin);
  //h_cal->SetAxisRange(0,3000);
	//h_cal->GetYaxis()->SetRangeUser(0,110);
	h_cal->SetLineColor(kBlue);
	h_cal->SetXTitle("keV");
	h_cal->SetYTitle("Counts");

	for (int i = 0; i < h->GetNbinsX(); ++i)
	{
    int adc_value = h->GetBinLowEdge(i);
    double cal_value = adc_value*slope + constant;
    int new_bin = h_cal->FindBin(cal_value);
    h_cal->Fill(new_bin, h->GetBinContent(i)); 
	}

	return h_cal;
}

TH1F* getSiR2CalibratedSpec(TH1F* h, double slope = 7.49, 
    double constant = 29.04)
{
	TString org_name = h->GetName();
	TString cal_name = org_name.Append("_calibrated");

	TString org_title = h->GetTitle();
	TString cal_title = org_title;

	TH1F * h_cal = new TH1F(cal_name, cal_title, 1024,0,1024*7.49);
	//h_cal->Print();
  //h_cal->SetBit(TH1::kCanRebin);
  //h_cal->SetAxisRange(0,3000);
	//h_cal->GetYaxis()->SetRangeUser(0,110);
	h_cal->SetLineColor(kBlue);
	h_cal->SetXTitle("Energy [keV]");
	h_cal->SetYTitle("Counts");

	//for (int i = 0; i < h->GetNbinsX(); ++i)
	for (int i = 0; i < h_cal->GetNbinsX(); ++i)
	{
    int adc_value = h->GetBinLowEdge(i);
    double cal_value = adc_value*slope + constant;
    int new_bin = h_cal->FindBin(cal_value);
    //h_cal->SetBinContent(new_bin, h->GetBinContent(i)); 
    h_cal->Fill(new_bin, h->GetBinContent(i)); 
	}

	return h_cal;
}

TH1F* getSiR2Shifted(TH1F* h, double E_mu = 4690,double slope = 7.49, 
    double constant = 29.04)
{
	TString org_name = h->GetName();
	TString cal_name = org_name.Append("_calibrated");

	TString org_title = h->GetTitle();
	TString cal_title = org_title;

	TH1F * h_cal = new TH1F(cal_name, cal_title, 512,-0.5,8*2048 -.5);
	//h_cal->Print();
  //h_cal->SetBit(TH1::kCanRebin);
  //h_cal->SetAxisRange(0,3000);
	//h_cal->GetYaxis()->SetRangeUser(0,110);
	h_cal->SetLineColor(kBlue);
	h_cal->SetXTitle("keV");
	h_cal->SetYTitle("Counts");

	for (int i = 0; i < h->GetNbinsX(); ++i)
	{
    int adc_value = h->GetBinLowEdge(i);
    double cal_value = adc_value*slope + constant;
    int new_bin = h_cal->FindBin(cal_value - E_mu);
    h_cal->SetBinContent(new_bin, h->GetBinContent(i)); 
	}

	return h_cal;
}

TF1 *funcEHpdf()
{
  TF1 *tmpf = new TF1("EHpdf", "[0]*pow(1-[1]/x,[2])*exp(-x/[3])",1.4,30);
  tmpf->SetParNames("A","T#_th","alpha", "T#_0");
  tmpf->SetParameters(0.105, 1.4, 1.3279, 3.1);
  return tmpf;
}

double getArea(TH1* h, double xl, double xh)
{
  int bl = h->FindBin(xl);
  double cl = h->GetBinContent(bl);
  int bh = h->FindBin(xh);
  double ch = h->GetBinContent(bh);
  double gross_area = h->Integral(bl, bh);
  double bg = (cl + ch)/2*(bh-bl);
  cout<<gross_area <<", "<< bg<<endl;
  return h->Integral(bl, bh) - (cl + ch)/2*(bh-bl);
}

TH1F * getResidual(TH1* hist, TString funcname)
{
  //TF1 * fittedfunc = hist->GetFunction(funcname);
  //int nbins = hist->GetNbins
 return NULL;
}

void MSaveBigPNG(TString filename="", double scale=5) {
    TCanvas* old_canv = gPad->GetCanvas();

    gROOT->SetBatch(kTRUE);
    gROOT->ForceStyle(kTRUE);

    Int_t orig_msz = gStyle->GetMarkerSize();
    Int_t orig_mst = gStyle->GetMarkerStyle();
    Int_t orig_lt  = gStyle->GetLineWidth();

    gStyle->SetMarkerSize(1.0+scale/5);
    gStyle->SetMarkerStyle(20);
    gStyle->SetLineWidth(orig_lt*scale);

    if(filename == "") {
            filename = old_canv->GetName();
            filename += ".png";
        }

    Int_t old_width  = old_canv->GetWindowWidth();
    Int_t old_height = old_canv->GetWindowHeight();

    Int_t new_width = old_width * scale;
    Int_t new_height= old_height* scale;

    TCanvas* temp_canvas = new TCanvas("temp", "", new_width, new_height);
    old_canv->DrawClonePad();

    temp_canvas->Draw();
    temp_canvas->SaveAs(filename);
    temp_canvas->Close();

    gStyle->SetMarkerSize(orig_msz);
    gStyle->SetMarkerStyle(orig_mst);
    gStyle->SetLineWidth(orig_lt);

    gROOT->ForceStyle(kFALSE);
    gROOT->SetBatch(kFALSE);

    return;
}
