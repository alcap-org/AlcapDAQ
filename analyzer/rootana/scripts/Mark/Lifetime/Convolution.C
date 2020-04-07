//  Convolution.c
//  Created by AurÃ©lie Flandi on 09.09.14.
//  Modified 05.19.18

//#include <stdio.h>
//#include <TMath.h>
//#include <TCanvas.h>
//#include <iostream>
//#include <TROOT.h>
//#include <TChain.h>
//#include <TObject.h>
//#include <TRandom.h>
//#include <TFile.h>
//#include <math.h>
//#include <TF1Convolution.h>
//#include <TF1.h>
//#include <TH1F.h>
//#include <TGraph.h>
//#include <TStopwatch.h>

using namespace std;

void Convolution(const char *filename = "/home/m-wong/data/R15b/al100.root")
{
    gStyle->SetOptFit();
    ROOT::Math::MinimizerOptions::SetDefaultMaxFunctionCalls(10000);
    ROOT::Math::MinimizerOptions::SetDefaultMinimizer("Minuit");
    Double_t min = -120;
    Double_t max = 3800;
    TFile *fData = new TFile(filename, "READ");
    Double_t t1, t2;
    TString *channel = new TString("");
    TString *pid = new TString("");
    TH1D *hMeasDataLeft = new TH1D("hMeasDataLeft", "MeasDataLeft;t [ns]", 1000, -10000, 10000);
    TH1D *hMeasDataRight = new TH1D("hMeasDataRight", "MeasDataRight;t [ns]", 1000, -10000, 10000);
    TH1D *hMeasData = new TH1D("hMeasData", "MeasData;t [ns]", 1000, -10000, 10000);
    TH1D *hTimeDiffLeft = new TH1D("hTimeDiffLeft", "TimeDiffLeft;t [ns]", 1000, -10000, 10000);
    TH1D *hTimeDiffRight = new TH1D("hTimeDiffRight", "TimeDiffRight;t [ns]", 1000, -10000, 10000);
    TTree *tree = (TTree *)fData->Get("tree");
    tree->SetBranchAddress("t1", &t1);
    tree->SetBranchAddress("t2", &t2);
    tree->SetBranchAddress("channel", &channel);
    tree->SetBranchAddress("pid", &pid);
    for(Long64_t i=0; i < tree->GetEntries(); i++) {
        tree->GetEntry(i);
        if(!pid->Contains("p") ) continue;
        if(t2-t1 > 300 || t2-t1 < -300) continue;
        if(channel->Contains("SiL1") ) {
            hTimeDiffLeft->Fill(t2-t1);
//            if(t2-t1 > -300 || t2-t1 < -800) continue;
            hMeasDataLeft->Fill(t2);
            hMeasData->Fill(t2);
        } else if(channel->Contains("SiR1") ) {
            hTimeDiffRight->Fill(t2-t1);
//            if(t2-t1 > 500 || t2-t1 < 0) continue;
            hMeasDataRight->Fill(t2);
            hMeasData->Fill(t2);
        }
    }

    TH1D *h1 = (TH1D *)hMeasDataRight->Clone();
    h1->Rebin(1);
    h1->Scale(1./h1->Integral() );
    h1->SetTitle("Proton time;t [ns];Normalized counts/10ns");

    Double_t defaultFitParameters[7] = { 5.56073e-04,
        -0.018205504,
        2.20218e-02,
        -0.0011574074,
        9.73980e-03,
        1.38458e+02,
        3.37901e+01};

    TF1Convolution *f_conv = new TF1Convolution("[0]*exp([1]*x)+[2]*exp([3]*x)", "gaus", min, max, true);
    f_conv->SetRange(min, max);
    f_conv->SetNofPointsFFT(3360);
    TF1 *f = new TF1("f",*f_conv, min, max, f_conv->GetNpar() );
    TCanvas *c = new TCanvas();
    h1->SetTitle("Data");
    Bool_t fitSuccess = false;
    if(!fitSuccess) {
        for(int i=0; i<f_conv->GetNpar(); i++) {
            f->SetParName(i, f_conv->GetParName(i) );
            f->SetParameter(i, defaultFitParameters[i]);
        }
//        f->SetParLimits(0, 0., 0.2*h1->GetMaximum() );
//        f->SetParLimits(1, -0.014, -0.009);
//        f->SetParLimits(2, 0., h1->GetMaximum() );
//        f->SetParLimits(3, -0.0014, -0.0009);
        //fit
        TFitResultPtr res = h1->Fit("f", "SR");
        fitSuccess = res;
    }
    //   Show confidence band
    //   TH1D *hint = new TH1D("hint", ".95 conf.band", 1000, min, max);
    //   (TVirtualFitter::GetFitter())->GetConfidenceIntervals(hint);
    //   hint->SetFillColor(kRed);
    //   hint->Draw("E3 SAME");

    h1->GetXaxis()->SetRangeUser(-500, 5000);
    Double_t par[7];
    f->GetParameters(par);
    printf("Decay parameters\n");
    printf("Pb: %f±%f\n", -1./par[1], 1./(par[1]*par[1]) * f->GetParError(1) ); //74ns
    printf("Al: %f±%f\n", -1./par[3], 1./(par[3]*par[3]) * f->GetParError(3) ); //864ns

    TF1 *fAl = new TF1("fAl", "[0]*exp([1]*x)", 0, max);
    fAl->SetTitle("Aluminium");
    fAl->SetParameters(par[2], par[3]);
    fAl->SetLineColor(kBlue);
    TF1 *fPb = new TF1("fPb", "[0]*exp([1]*x)", 0, max);
    fPb->SetTitle("Lead");
    fPb->SetParameters(par[0], par[1]);
    fPb->SetLineColor(kMagenta);

    fAl->Draw("SAME");
    fPb->Draw("SAME");
    c->BuildLegend();
    c->Draw();
    h1->SetTitle("Proton time");

    Double_t pAll = fAl->Integral(0, max);
    Double_t p300 = fAl->Integral(300, max);
    Double_t p400 = fAl->Integral(400, max);
    Double_t p500 = fAl->Integral(500, max);
    printf("300ns cut correction factor: %f\n", p300/pAll);
    printf("400ns cut correction factor: %f\n", p400/pAll);
    printf("500ns cut correction factor: %f\n", p500/pAll);

//    gROOT->ProcessLine(".L /Volumes/MARK/AlCap/BABARSmartLabel.C");
//    gROOT->ProcessLine("BABARSmartLabel(.8, .3, .9,\"~1\",-1,-1);");
}
