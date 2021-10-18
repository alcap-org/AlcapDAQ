#ifndef Unfold_ResponseMatrix_
#define Unfold_ResponseMatrix_

#include "TFile.h"
#include "TTree.h"
#include "TH2.h"
#include "TF1.h"

#include "RooUnfoldResponse.h"
#include "RooUnfoldBayes.h"
#include "RooUnfoldBinByBin.h"
#include "RooUnfoldSvd.h"
#include "RooUnfoldIds.h"

#include <iostream>
#include <sstream>

struct Unfold_ResponseMatrixArgs {

  std::string datafilename;
  std::string datahistname;
  std::string datacuttreename;
  //  std::string lifetimefilename;
  //  std::string lifetimefitfnname;
  std::string mcfilename;
  std::string mcresponsename;
  std::string outfilename;
  std::string outdirname;
  //  int rebin_factor; // to get the folded spectrum to match the binning of the response matrix
  double bin_width; // the bin width we want to get to
  double lowE_cut; // where to cut spectrum and response matrix at
  double highE_cut; // where to cut spectrum and response matrix at
  double n_gen_events; // number of events that were generated

  std::string method;
  int reg_parameter; // either the number of iterations for Bayes, or the regularisation parameter for SVD
};

void printIntegrals(TH1D* hUnfolded);

void Unfold_ResponseMatrix(Unfold_ResponseMatrixArgs& args) {

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

  /*
  TFile* lifetime_file = new TFile(args.lifetimefilename.c_str(), "READ");
  if (lifetime_file->IsZombie()) {
    std::cout << "Error: Problem opening lifetime file " << args.lifetimefilename << std::endl;
    return;
  }

  TF1* lifetime_fn = (TF1*) lifetime_file->Get(args.lifetimefitfnname.c_str());
  if (!lifetime_fn) {
    std::cout << "Error: Problem getting lifetime function " << args.lifetimefitfnname << std::endl;
    return;
  }

  TTree* cuttree = (TTree*) data_file->Get(args.datacuttreename.c_str());
  if (!cuttree) {
    std::cout << "Error: Problem getting cuttree " << args.datacuttreename << std::endl;
    return;
  }
  double time_cut = 0;
  cuttree->SetBranchAddress("early_time_cut", &time_cut);
  cuttree->GetEntry(0);

  double time_cut_efficiency = (lifetime_fn->Eval(time_cut) / lifetime_fn->GetParameter(0));
  std::cout << "Muonic Atom Lifetime (from fit) = " << lifetime_fn->GetParameter(1) << std::endl;
  std::cout << "For a " << time_cut << " ns time cut, efficiency = " << time_cut_efficiency << std::endl;
  */
  RooUnfoldResponse* response = (RooUnfoldResponse*) mc_file->Get(args.mcresponsename.c_str());
  if (!response) {
    std::cout << "Problem finding RooUnfoldResponse " << args.mcresponsename.c_str() << std::endl;
    return;
  }

  TH1D* folded_spectrum  = (TH1D*) data_file->Get(args.datahistname.c_str());
  if (!folded_spectrum) {
    std::cout << "Error: Problem finding histogram " << args.datahistname << std::endl;
    return;
  }
  std::string newname = "hInputSpectrum";
  folded_spectrum->SetName(newname.c_str());
  for (int i_bin = 1; i_bin <= folded_spectrum->GetNbinsX(); ++i_bin) {
    if (folded_spectrum->GetBinCenter(i_bin) < args.lowE_cut ||
	folded_spectrum->GetBinCenter(i_bin) > args.highE_cut)  {
      folded_spectrum->SetBinContent(i_bin, 0);
      folded_spectrum->SetBinError(i_bin, 0);
    }
  }
  int spec_rebin_factor = args.bin_width / folded_spectrum->GetBinWidth(1);
  //  folded_spectrum->Rebin(args.rebin_factor);
  folded_spectrum->Rebin(spec_rebin_factor);
  std::stringstream axislabel;
  axislabel << "Folded Count / " << folded_spectrum->GetBinWidth(1) << " keV";
  folded_spectrum->SetYTitle(axislabel.str().c_str());

  // if (args.bin_width == 0.5){
  //   for (int i_bin = 1; i_bin <= folded_spectrum->GetNbinsX(); ++i_bin) {
  //     double bin_center = folded_spectrum->GetBinCenter(i_bin);
  //     if (bin_center > 15.5 && bin_center < 16) {
  // 	folded_spectrum->SetBinContent(i_bin, folded_spectrum->GetBinContent(i_bin)*(1/0.90));
  //     }
  //     else if (bin_center > 16 && bin_center < 16.5) {
  // 	folded_spectrum->SetBinContent(i_bin, folded_spectrum->GetBinContent(i_bin)*(1/1.04));
  //     }
  //     else if (bin_center > 16.5 && bin_center < 17) {
  // 	folded_spectrum->SetBinContent(i_bin, folded_spectrum->GetBinContent(i_bin)*(1/1.06));
  //     }
  //   }
  // }

  TH2D* response_matrix = (TH2D*) response->Hresponse();
  newname = "hResponseMatrix";
  response_matrix->SetName(newname.c_str());
  int resp_rebin_factor = args.bin_width / response_matrix->GetXaxis()->GetBinWidth(1);
  response_matrix->Rebin2D(resp_rebin_factor, resp_rebin_factor);
  for (int i_bin = 1; i_bin <= response_matrix->GetXaxis()->GetNbins(); ++i_bin) {
    for (int j_bin = 1; j_bin <= response_matrix->GetYaxis()->GetNbins(); ++j_bin) {
      double true_E = response_matrix->GetYaxis()->GetBinCenter(j_bin);
      if (true_E <= args.lowE_cut || true_E >= args.highE_cut) {
	response_matrix->SetBinContent(i_bin, j_bin, 0);
	response_matrix->SetBinError(i_bin, j_bin, 0);
      }
    }
  }
  TH1D* hMeasured = (TH1D*) response->Hmeasured();
  hMeasured->Rebin(resp_rebin_factor);
  TH1D* hTruth = (TH1D*) response->Htruth();
  hTruth->Rebin(resp_rebin_factor);
  //  RooUnfoldResponse* new_response = new RooUnfoldResponse(response_matrix->ProjectionX(), response_matrix->ProjectionY(), response_matrix); 
  RooUnfoldResponse* new_response = new RooUnfoldResponse(hMeasured, hTruth, response_matrix);

  std::cout << "Unfolding ResponseMatrix: Input Bin Width = " << folded_spectrum->GetXaxis()->GetBinWidth(1) << ", Response Matrix Bin Width = " << response_matrix->GetXaxis()->GetBinWidth(1) << std::endl;

  RooUnfold* unfold = 0;
  if (args.method == "bayes") {
    //    unfold = new RooUnfoldBayes(response, folded_spectrum, args.reg_parameter);
    unfold = new RooUnfoldBayes(new_response, folded_spectrum, args.reg_parameter);
  }
  else if (args.method == "svd") {
    unfold = new RooUnfoldSvd(response, folded_spectrum, args.reg_parameter);
  }
  else if (args.method == "bin-by-bin") {
    unfold = new  RooUnfoldBinByBin(response, folded_spectrum);
  }
  else {
    std::cout << "Unfold_ResponseMatrix: Unknown unfolding method " << args.method << std::endl;
    return;
  }
  //  unfold->IncludeSystematics();
  TH1D* unfolded_spectrum = (TH1D*) unfold->Hreco(RooUnfold::kCovariance);
  //TH1D* unfolded_spectrum = (TH1D*) unfold->Hreco(RooUnfold::kCovToy);
  newname = "hCorrectedSpectrum";
  unfolded_spectrum->SetName(newname.c_str());
  //  unfolded_spectrum->Draw("HIST E SAMES");
  unfolded_spectrum->SetLineColor(kBlue);
  unfolded_spectrum->SetLineWidth(2);
  axislabel.str("");
  axislabel << "Unfolded Count / " << unfolded_spectrum->GetBinWidth(1) << " keV";
  unfolded_spectrum->SetYTitle(axislabel.str().c_str());
  
  //  double geom_efficiency = response_matrix->GetEntries() / args.n_gen_events;
  std::cout << args.outdirname << std::endl;
  //  std::cout << unfolded_spectrum->GetMaximum() << std::endl;
  //  std::cout << "Geom Eff = " << geom_efficiency << std::endl;
  std::cout << "Spectrum Rebin Factor = " << spec_rebin_factor << std::endl;
  std::cout << "Response Matrix Rebin Factor = " << resp_rebin_factor << std::endl;
  //  unfolded_spectrum->Scale(1.0 / geom_efficiency);
  //  std::cout << unfolded_spectrum->GetMaximum() << std::endl;

  folded_spectrum->SetLineColor(kRed);
  folded_spectrum->SetLineWidth(2);
  //  folded_spectrum->Draw("HIST E SAMES");
  //  unfold->PrintTable(std::cout);

  TFile* outfile = new TFile(args.outfilename.c_str(), "UPDATE");
  TDirectory* outdir = outfile->mkdir(args.outdirname.c_str());
  outdir->cd();
  unfold->SetName("unfold");
  unfold->Write();
  response->Write();
  new_response->Write();
  response_matrix->Write();
  folded_spectrum->Write();
  unfolded_spectrum->Write();
  if (args.method == "svd") {
    TSVDUnfold* tsvd = ((RooUnfoldSvd*)unfold)->Impl();
    if (!tsvd) {
      std::cout << "AE: An Issue!" << std::endl;
      return;
    }
    TH1D* hD = (TH1D*) tsvd->GetD()->Clone("hD");
    hD->Write();
  }
  
  outfile->Write();
  outfile->Close();

  data_file->Close();
  mc_file->Close();
}

void printIntegrals(TH1D* hUnfolded) {
  double integral_low = 0;
  double integral_high = 10000;
  int integral_bin_low = hUnfolded->FindBin(integral_low);
  int integral_bin_high = hUnfolded->FindBin(integral_high);
  std::cout << integral_low << " -- " << integral_high << " keV: Integral = " << hUnfolded->Integral(integral_bin_low, integral_bin_high) << std::endl;

  integral_low = 3000;
  integral_high = 10000;
  integral_bin_low = hUnfolded->FindBin(integral_low);
  integral_bin_high = hUnfolded->FindBin(integral_high);
  std::cout << integral_low << " -- " << integral_high << " keV: Integral = " << hUnfolded->Integral(integral_bin_low, integral_bin_high) << std::endl;

  integral_low = 4000;
  integral_high = 8000;
  integral_bin_low = hUnfolded->FindBin(integral_low);
  integral_bin_high = hUnfolded->FindBin(integral_high);
  std::cout << integral_low << " -- " << integral_high << " keV: Integral = " << hUnfolded->Integral(integral_bin_low, integral_bin_high) << std::endl;

  integral_low = 0;
  integral_high = 15000;
  integral_bin_low = hUnfolded->FindBin(integral_low);
  integral_bin_high = hUnfolded->FindBin(integral_high);
  std::cout << integral_low << " -- " << integral_high << " keV: Integral = " << hUnfolded->Integral(integral_bin_low, integral_bin_high) << std::endl;
}
#endif
