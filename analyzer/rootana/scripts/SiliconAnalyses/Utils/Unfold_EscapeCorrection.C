#ifndef Unfold_EscapeCorrection_
#define Unfold_EscapeCorrection_

#include "TFile.h"
#include "TTree.h"
#include "TH2.h"
#include "TF1.h"
#include "TVirtualFitter.h"
#include "TFitResult.h"
#include "TVectorD.h"
#include "TGraphAsymmErrors.h"
#include "TGraphErrors.h"

#include "RooUnfoldResponse.h"

#include <iostream>
#include <sstream>

struct Unfold_EscapeCorrectionArgs {

  std::string datafilename;
  std::string datahistname;
  std::string datacuttreename;
  std::string mcfilename;
  std::string mcresponsename;
  std::string outfilename;
  std::string outdirname;
  int rebin_factor; // to get the folded spectrum to match the binning of the response matrix
  double bin_width;
  //  std::string fit;
  double min_E;
  double max_E;
  bool hist_input;
};

class fit_fn_class {
public:
  fit_fn_class(TGraphAsymmErrors* hData, TH2D* hResp, std::string fit)
    : hData(hData), fit(fit), respMatrix(hResp->GetNbinsX(), hResp->GetNbinsY()) {

    //    hTruth = (TGraphAsymmErrors*) hData->Clone("hTruth");
    //    hTruth->Reset();
    truthVec = new TVectorD(hData->GetN());

    std::cout << "hResp: = " << hResp->GetNbinsX() << " x  "<< hResp->GetNbinsY() << std::endl;
    
    for (int i_bin = 0; i_bin < hResp->GetNbinsX(); ++i_bin) {
      for (int j_bin = 0; j_bin < hResp->GetNbinsY(); ++j_bin) {
    	respMatrix(i_bin, j_bin) = hResp->GetBinContent(i_bin+1, j_bin+1)/(hResp->ProjectionX("_px", i_bin+1,i_bin+1)->GetEntries());
      }
    }

    std::cout << "Response Matrix = " << respMatrix.GetNcols() << ", Spectrum =" << truthVec->GetNrows() << std::endl;
    // if (respMatrix.GetNrows() != truthVec->GetNrows()) {
    //   //      std::cout << "Different dimensions: Response Matrix = " << respMatrix.GetNrows() << ", Spectrum =" << truthVec->GetNrows() << std::endl;

    //   //      respMatrix = respMatrix.GetSub(0, truthVec->GetNrows(), 0, truthVec->GetNrows());
    //   respMatrix.ResizeTo(truthVec->GetNrows(), truthVec->GetNrows());
    //   //      std::cout << "Different dimensions: Response Matrix = " << respMatrix.GetNrows() << ", Spectrum =" << truthVec->GetNrows() << std::endl;
    //   //      return;
    // }

  }

  double operator()(Double_t* x, Double_t* par) {
  //   func(x, par);
  // }
  // double func(Double_t* x, Double_t* par) {
    // if (x[0] >= 16 && x[0] <=18) {
    //   TF1::RejectPoint();
    //   return 0;
    // }
    // Construct truth
    //    hTruth->Reset();
    for (int i_bin = 0; i_bin < hData->GetN(); ++i_bin) {
      double E = *(hData->GetX()+i_bin);//hTruth->GetBinCenter(i_bin);
      double result = 0;
      if (fit == "Single") {
	double N0 = par[0];
	double T0 = par[1];
	result = N0*(TMath::Exp(-E/T0));
      }
      else if(fit == "Double") {
	double N0 = par[0];
	double T0 = par[1];
	double r = par[2];
	double T1 = par[3];
	result = N0*(TMath::Exp(-E/T0) + r*TMath::Exp(-E/T1));
      }
      else if(fit == "Triple") {
	double N0 = par[0];
	double T0 = par[1];
	double r = par[2];
	double T1 = par[3];	
	double r2 = par[4];
	double T2 = par[5];
	result = N0*(TMath::Exp(-E/T0) + r*TMath::Exp(-E/T1) + r2*TMath::Exp(-E/T2));
      }
      else if (fit == "SingleThresh") {
	double N0 = par[0];
	double T0 = par[1];
	double Tth = par[2];
	double alpha = par[3];
	if (E < Tth) {
	  result = 0;
	}
	else {
	  result = N0*TMath::Power( (1 - (Tth/E)), alpha)*(TMath::Exp(-E/T0));
	}	
	// std::cout << "E = " << E << ", result = " << result << std::endl;
	// if (std::isnan(result)) {
	//   std::cout << "N0 = " << N0 << ", T0 = " << T0 << ", Tth = " << Tth << ", alpha = " << alpha << std::endl;
	// }
      }
      else if (fit == "DoubleThresh") {
	double N0 = par[0];
	double T0 = par[1];
	double Tth = par[2];
	double alpha = par[3];
	double r = par[4];
	double T1 = par[5];
	if (E < Tth) {
	  result = 0;
	}
	else {
	  result = N0*TMath::Power( (1 - (Tth/E)), alpha)*(TMath::Exp(-E/T0) + r*TMath::Exp(-E/T1));
	}	
	// std::cout << "E = " << E << ", result = " << result << std::endl;
	// if (std::isnan(result)) {
	//   std::cout << "N0 = " << N0 << ", T0 = " << T0 << ", Tth = " << Tth << ", alpha = " << alpha << std::endl;
	// }
      }
      //      hTruth->SetBinContent(i_bin, result);
      (*truthVec)[i_bin] = result;
    }

    // int i_bin = hData->FindBin(xx);
    // //    if (xx < E1) {
    // //      result = hData->GetBinContent(i_bin);
    // //    }
    // //    else if (xx < E2) {
    // //      result = result;
    // //    }
    // //    else {
    //   double diagonal_content = (hResp->GetBinContent(i_bin, i_bin) + hResp->GetBinContent(i_bin-1, i_bin) + hResp->GetBinContent(i_bin+1, i_bin));
    //   double total_slice_content = hResp->ProjectionX("_px", i_bin, i_bin)->GetEntries();
    //   if (total_slice_content != 0) {
    // 	result *= (diagonal_content/total_slice_content);
    //   }
    //   //    }
    double xx = x[0];
    TVectorF fold = respMatrix * (*truthVec);
    //    std::cout << respMatrix.GetNrows() << " x " << respMatrix.GetNcols() << std::endl;
    //    std::cout << truthVec->GetNrows() << std::endl;
    int this_bin = -1;
    for (int i_bin = 0; i_bin < hData->GetN(); ++i_bin) {
      double i_xx = *(hData->GetX()+i_bin);
      //      std::cout << "AE: " << i_xx << ", " << xx << std::endl;
      if (i_xx + *(hData->GetEXhigh()+i_bin) >= xx) {
	//	std::cout << "Done" << std::endl;
	this_bin = i_bin;
	break;
      }
    }
    if (this_bin <0) {
      std::cout << "ERROR fold" << std::endl;
      std::cout << "xx = " << xx << std::endl;
    }
    return fold[this_bin];
  }

  // void fcn(Int_t &npar, Double_t *gin, Double_t &f, Double_t *par, Int_t iflag) {
  //   int nbins = hData->GetN();
  //   //calculate chisquare
  //   Double_t chisq = 0;
  //   Double_t delta;
  //   for (int i=0;i<nbins; i++) {
  //     double i_energy[1] = {*(hData->GetX()+i)};
  //     double i_data = *(hData->GetY()+i);
  //     double i_data_err = *(hData->GetErrorYhigh()+i);
  //     delta  = (i_data-func(i_energy, par))/i_data_err;
  //     chisq += delta*delta;
  //   }
  //   f = chisq;
  // }

private:
  TGraphAsymmErrors* hData;
  TGraphAsymmErrors* hTruth;
  //  TH2D* hResp;
  TVectorD* truthVec;
  TMatrixD respMatrix;
  std::string fit;
};

void Unfold_EscapeCorrection(Unfold_EscapeCorrectionArgs& args) {
  
  TFile* data_file = new TFile(args.datafilename.c_str(), "READ");
  if (data_file->IsZombie()) {
    std::cout << "Error: Problem opening data file " << args.datafilename << std::endl;
    return;
  }
  
  TFile* mc_file = new TFile(args.mcfilename.c_str(), "READ");
  if (mc_file->IsZombie()) {
    std::cout << "Error: Problem opening mc file " << args.mcfilename << std::endl;
    return;
  }

  RooUnfoldResponse* response = (RooUnfoldResponse*) mc_file->Get(args.mcresponsename.c_str());
  if (!response) {
    std::cout << "Problem finding RooUnfoldResponse " << args.mcresponsename.c_str() << std::endl;
    return;
  }

  TGraphAsymmErrors* folded_spectrum ;
  if (!args.hist_input) {
    folded_spectrum  = (TGraphAsymmErrors*) data_file->Get(args.datahistname.c_str());
    if (!folded_spectrum) {
      std::cout << "Error: Problem finding histogram " << args.datahistname << std::endl;
      return;
    }
  }
  else {
    TH1D* hist = (TH1D*) data_file->Get(args.datahistname.c_str());
    if (!hist) {
      std::cout << "Error: Problem finding histogram " << args.datahistname << std::endl;
      return;
    }

    // // Apply geom acceptance here so I can do it as a function of energy
    // TH1D* hGeomAcceptance = (TH1D*) response->Hmeasured();
    // int resp_rebin_factor = args.bin_width / hGeomAcceptance->GetXaxis()->GetBinWidth(1);
    // hGeomAcceptance->Rebin(resp_rebin_factor);
    // double n_mc_events = 100e6;
    // double mc_gen_energy_range = 25;
    // double bin_width = hGeomAcceptance->GetXaxis()->GetBinWidth(1);
    // double denom = n_mc_events / (mc_gen_energy_range / bin_width);
    // std::cout << "Geom Acceptance Denom = " << 1.0 / denom << std::endl;
    // hGeomAcceptance->Scale(1.0/denom); // all sims did 100M events
    // hist->Divide(hGeomAcceptance);
    
    int spec_rebin_factor = args.bin_width / hist->GetBinWidth(1);
    hist->Rebin(spec_rebin_factor);
    std::cout << "Hist Rebin Factor = " << spec_rebin_factor << std::endl;
    //    hist->Rebin(args.rebin_factor);
    int n_points = hist->GetNbinsX();
    double x[n_points];
    double y[n_points];
    double xerr[n_points];
    double yerr[n_points];
    for (int i_point = 1; i_point <= n_points; ++i_point) {
      x[i_point-1] = hist->GetBinCenter(i_point);
      xerr[i_point-1] = hist->GetBinWidth(i_point)/2.0;
      y[i_point-1] = hist->GetBinContent(i_point);
      yerr[i_point-1] = hist->GetBinError(i_point);
      //      std::cout << "(x,y) = (" << x[i_point-1] << " +/- " << xerr[i_point-1] << ", " << y[i_point-1] << " +/- " << yerr[i_point-1] << ")" << std::endl;
    }
    folded_spectrum = new TGraphAsymmErrors(n_points, x, y, xerr, xerr, yerr, yerr);
    //    folded_spectrum->Draw("APE");
  }
  std::string newname = "hInputSpectrum";
  folded_spectrum->SetName(newname.c_str());
  //  folded_spectrum->Rebin(args.rebin_factor);
  //  std::stringstream axislabel;
  //  axislabel << "Folded Count / " << folded_spectrum->GetBinWidth(1) << " keV";
  //  folded_spectrum->SetYTitle(axislabel.str().c_str());

  TH2D* response_matrix = (TH2D*) response->Hresponse();
  newname = "hResponseMatrix";
  response_matrix->SetName(newname.c_str());

  int resp_rebin_factor = args.bin_width / response_matrix->GetXaxis()->GetBinWidth(1);
  std::cout << "Resp Rebin Factor = " << resp_rebin_factor << std::endl;
  response_matrix->Rebin2D(resp_rebin_factor, resp_rebin_factor);

  
  newname = "hCorrectedSpectrum";
  TGraphAsymmErrors* refolded_spectrum = (TGraphAsymmErrors*) folded_spectrum->Clone(newname.c_str());

  const int n_fits = 4;
  std::string fits[n_fits] = {"Single", "Double", "SingleThresh", "DoubleThresh"};
  TF1* fit_fns[n_fits] = {0};
  TFitResultPtr fit_results[n_fits] = {0};
  TGraphErrors* fit_intervals[n_fits] = {0};
  for (int i_fit = 0; i_fit < n_fits; ++i_fit) {
    std::string fit = fits[i_fit];
    fit_fn_class model(folded_spectrum, response_matrix, fit);
    TF1* fit_fn = 0;
    double min_E = args.min_E;
    double max_E = args.max_E;
    std::cout << "Fit range: " << min_E << " -- " << max_E << std::endl;
    if (fit == "Single") {
      fit_fn = new TF1(fit.c_str(), model, min_E, max_E, 2);
      fit_fn->SetParName(0, "N0");
      fit_fn->SetParName(1, "T0");
      fit_fn->SetParameters(0.01, 4.8);
    }
    else if (fit == "Double") {
      //    min_E = 5;
      fit_fn = new TF1(fit.c_str(), model, min_E, max_E, 4);
      fit_fn->SetParName(0, "N0");
      fit_fn->SetParName(1, "T0");
      fit_fn->SetParLimits(1, 0, 5);
      fit_fn->SetParName(2, "r");
      fit_fn->SetParLimits(2, 0, 1);
      fit_fn->SetParName(3, "T1");
      fit_fn->SetParLimits(3, 5, 30);
      fit_fn->SetParameters(0.01, 2.8, 0.1, 7.8);
    }
    else if (fit == "Triple") {
      fit_fn = new TF1(fit.c_str(), model, min_E, max_E, 6);
      fit_fn->SetParName(0, "N0");
      fit_fn->SetParName(1, "T0");
      fit_fn->SetParName(2, "r");
      fit_fn->SetParLimits(2, 0, 1);
      fit_fn->SetParName(3, "T1");
      fit_fn->SetParName(4, "r2");
      fit_fn->SetParLimits(4, 0, 1);
      fit_fn->SetParName(5, "T2");
      fit_fn->SetParameters(0.1, 4.8, 0.5, 7.8, 0.1, 5.0);
    }
    else if (fit == "SingleThresh") {
      fit_fn = new TF1(fit.c_str(), model, min_E, max_E, 4);
      fit_fn->SetParName(0, "N0");
      fit_fn->SetParName(1, "T0");
      fit_fn->SetParLimits(0, 0.01, 1);
      fit_fn->SetParLimits(1, 1, 10);
      fit_fn->SetParName(2, "Tth");
      fit_fn->SetParLimits(2, 0, 5.0);
      fit_fn->SetParName(3, "#alpha");
      fit_fn->SetParLimits(3, 0, 100);
      fit_fn->SetParameters(0.1, 4.8, 1.4, 2);
    }
    else if (fit == "DoubleThresh") {
      fit_fn = new TF1(fit.c_str(), model, min_E, max_E, 6);
      fit_fn->SetParName(0, "N0");
      fit_fn->SetParName(1, "T0");
      fit_fn->SetParLimits(0, 0.01, 1);
      fit_fn->SetParLimits(1, 1, 10);
      fit_fn->SetParName(2, "Tth");
      fit_fn->SetParLimits(2, 0, 2);
      fit_fn->SetParName(3, "#alpha");
      fit_fn->SetParLimits(3, 0, 100);
      fit_fn->SetParName(4, "r");
      fit_fn->SetParLimits(4, 0, 1.0);
      fit_fn->SetParName(5, "T1");
      fit_fn->SetParLimits(5, 5, 30);
      fit_fn->SetParameters(0.1, 4.8, 1.4, 0.2, 0.1, 10);
      //      fit_fn->FixParameter(2, 1.4);
      //     fit_fn->FixParameter(0, fit_results[i_fit-1]->Parameter(0));
      //      fit_fn->FixParameter(2, fit_results[i_fit-1]->Parameter(2));
    }
    else {
      std::cout << "ERROR: fit = " << fit << " unrecognised" << std::endl;
      return;
    }
    //    TVirtualFitter::SetFCN(model.fcn);
    TVirtualFitter::SetPrecision(1);
    TVirtualFitter::SetMaxIterations(10000);
    fit_fn->SetNpx(10000);
    fit_results[i_fit] = folded_spectrum->Fit(fit_fn, "R EX0 SV +");
    //    //    fit_results[i_fit] = folded_spectrum->Fit(fit_fn, "RVS");
    fit_fns[i_fit] = fit_fn;

    // get the confidence intervals
    double start = min_E;
    double end = 30;//1500;
    double point_step = 0.1;
    int n_points = (end - start)/point_step;
    fit_intervals[i_fit] = new TGraphErrors(n_points);
    std::string intervalname = "interval_"+fit;
    fit_intervals[i_fit]->SetName(intervalname.c_str());
    for (int i_point = 0; i_point < n_points; ++i_point) {
      fit_intervals[i_fit]->SetPoint(i_point, start + i_point*point_step, 0);
    }
    (TVirtualFitter::GetFitter())->GetConfidenceIntervals(fit_intervals[i_fit]);
    std::cout << "AE:int = " << *(fit_intervals[i_fit]->GetEY()+1) << std::endl;
  }
  /*
  for (int i_bin = 1; i_bin < response_matrix->GetXaxis()->GetNbins(); ++i_bin) {
    for (int j_bin = 1; j_bin < response_matrix->GetYaxis()->GetNbins(); ++j_bin) {
      double true_E = response_matrix->GetYaxis()->GetBinCenter(j_bin);
      if (true_E < 4000) {
	response_matrix->SetBinContent(i_bin, j_bin, 0);
	response_matrix->SetBinError(i_bin, j_bin, 0);
      }
    }
  }
  */

  //  std::cout << "Unfolding EscapeCorrection: Input Bin Width = " << folded_spectrum->GetXaxis()->GetBinWidth(1) << ", Response Matrix Bin Width = " << response_matrix->GetXaxis()->GetBinWidth(1) << std::endl;

  //  refolded_spectrum->Draw("HIST E SAMES");
  refolded_spectrum->SetLineColor(kBlue);
  refolded_spectrum->SetLineWidth(2);
  //  axislabel.str("");
  //  axislabel << "Unfolded Count / " << refolded_spectrum->GetBinWidth(1) << " keV";
  //  refolded_spectrum->SetYTitle(axislabel.str().c_str());
  

  folded_spectrum->SetLineColor(kRed);
  folded_spectrum->SetLineWidth(2);
  //  folded_spectrum->Draw("HIST E SAMES");
  //  unfold->PrintTable(std::cout);

  TFile* outfile = new TFile(args.outfilename.c_str(), "UPDATE");
  TDirectory* outdir = outfile->mkdir(args.outdirname.c_str());
  outdir->cd();
  response->Write();
  response_matrix->Write();
  folded_spectrum->Write();
  //  model->Write();
  for (int i_fit = 0; i_fit < n_fits; ++i_fit) {
    fit_fns[i_fit]->Write();
    fit_results[i_fit]->Write();
    std::cout << "AE: " << (fit_results[i_fit]->GetCorrelationMatrix())(0, 0) << std::endl;
    std::cout << "AE: " << (fit_results[i_fit]->GetCorrelationMatrix())(0, 1) << std::endl;
    fit_intervals[i_fit]->Write();
    std::string cov_matrix_name = "covMatrix_" + fits[i_fit];
    fit_results[i_fit]->GetCovarianceMatrix().Write(cov_matrix_name.c_str());
  }
  refolded_spectrum->Write();
  outfile->Write();
  outfile->Close();

  data_file->Close();
  mc_file->Close();
}

#endif
