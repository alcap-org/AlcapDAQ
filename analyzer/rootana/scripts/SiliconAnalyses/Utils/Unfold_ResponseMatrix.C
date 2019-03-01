#include "TFile.h"
#include "TTree.h"
#include "TH2.h"
#include "TF1.h"

#include "RooUnfoldResponse.h"
#include "RooUnfoldBayes.h"
#include "RooUnfoldBinByBin.h"
#include "RooUnfoldSvd.h"

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
  int rebin_factor; // to get the folded spectrum to match the binning of the response matrix
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

  TH1D* folded_spectrum  = (TH1D*) data_file->Get(args.datahistname.c_str());
  if (!folded_spectrum) {
    std::cout << "Error: Problem finding histogram " << args.datahistname << std::endl;
    return;
  }
  std::string newname = "hFoldedSpectrum";
  folded_spectrum->SetName(newname.c_str());
  folded_spectrum->Rebin(args.rebin_factor);
  std::stringstream axislabel;
  axislabel << "Folded Count / " << folded_spectrum->GetBinWidth(1) << " keV";
  folded_spectrum->SetYTitle(axislabel.str().c_str());

  TH2D* response_matrix = (TH2D*) response->Hresponse();
  newname = "hResponseMatrix";
  response_matrix->SetName(newname.c_str());
  
  RooUnfoldBayes unfold (response, folded_spectrum);
  //  RooUnfoldBinByBin unfold (response, folded_spectrum);
  //  RooUnfoldSvd unfold (response, folded_spectrum);
  
  TH1D* unfolded_spectrum = (TH1D*) unfold.Hreco();
  newname = "hUnfoldedSpectrum";
  unfolded_spectrum->SetName(newname.c_str());
  //  unfolded_spectrum->Draw("HIST E SAMES");
  unfolded_spectrum->SetLineColor(kBlue);
  unfolded_spectrum->SetLineWidth(2);
  axislabel.str("");
  axislabel << "Unfolded Count / " << unfolded_spectrum->GetBinWidth(1) << " keV";
  unfolded_spectrum->SetYTitle(axislabel.str().c_str());

  //  std::cout << "Before Time Cut Efficiency Correction: " << std::endl;
  printIntegrals(unfolded_spectrum);

  //  unfolded_spectrum->Scale(1.0 / time_cut_efficiency);

  //  std::cout << "After Time Cut Efficiency Correction: " << std::endl;
  //  printIntegrals(unfolded_spectrum);

  folded_spectrum->SetLineColor(kRed);
  folded_spectrum->SetLineWidth(2);
  //  folded_spectrum->Draw("HIST E SAMES");
  unfold.PrintTable(std::cout);

  TFile* outfile = new TFile(args.outfilename.c_str(), "UPDATE");
  TDirectory* outdir = outfile->mkdir(args.outdirname.c_str());
  outdir->cd();
  response->Write();
  response_matrix->Write();
  folded_spectrum->Write();
  unfolded_spectrum->Write();

  outfile->Write();
  outfile->Close();
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
