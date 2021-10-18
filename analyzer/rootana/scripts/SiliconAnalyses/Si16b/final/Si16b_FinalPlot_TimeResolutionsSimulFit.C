#include "Fit/Fitter.h"
#include "Fit/BinData.h"
#include "Fit/Chi2FCN.h"
#include "TH1.h"
#include "TList.h"
#include "Math/WrappedMultiTF1.h"
#include "HFitInterface.h"
#include "TCanvas.h"
#include "TStyle.h"


// definition of shared parameter
// par0 = height of hist 1
// par1 = mean of hist 1
int iparPair1[4] = { 0, // height of hist 1
		     1, // mean of hist 1
		     2,   // sigma_det1
		     3    // sigma_det2
};

int iparPair2[4] = { 4, // height of hist 2
		     5, // mean of hist 2
		     3,   // sigma_det2
		     6    // sigma_det3
};

int iparPair3[4] = { 7, // height of hist 3.
		     8, // mean of hist 3
		     6,   // sigma_det3
		     2    // sigma_det1
};

// int iparPair4[4] = { 9, // height of hist 3.
// 		     10, // mean of hist 3
// 		     11,   // sigma_det4
// 		     2    // sigma_det1
// };


// Create the GlobalCHi2 structure

struct GlobalChi2 {
  GlobalChi2(  ROOT::Math::IMultiGenFunction & f1,
	       ROOT::Math::IMultiGenFunction & f2,
	       ROOT::Math::IMultiGenFunction & f3//,
	       //	       ROOT::Math::IMultiGenFunction & f4
	       ) :
    fChi2_1(&f1), fChi2_2(&f2), fChi2_3(&f3) {}//, fChi2_4(&f4) {}

  double operator() (const double *par) const {
    int n_par = 4;
    double p1[n_par];
    for (int i = 0; i < n_par; ++i) p1[i] = par[iparPair1[i] ];

    double p2[n_par];
    for (int i = 0; i < n_par; ++i) p2[i] = par[iparPair2[i] ];

    double p3[n_par];
    for (int i = 0; i < n_par; ++i) p3[i] = par[iparPair3[i] ];

    // double p4[n_par];
    // for (int i = 0; i < n_par; ++i) p4[i] = par[iparPair4[i] ];

    return (*fChi2_1)(p1) + (*fChi2_2)(p2) + (*fChi2_3)(p3);//  + (*fChi2_4)(p4);
  }

  const  ROOT::Math::IMultiGenFunction * fChi2_1;
  const  ROOT::Math::IMultiGenFunction * fChi2_2;
  const  ROOT::Math::IMultiGenFunction * fChi2_3;
  //  const  ROOT::Math::IMultiGenFunction * fChi2_4;
};

void Si16b_FinalPlot_TimeResolutionsSimulFit() {

  std::string filename = "~/data/out/v10/Si16b.root";
  //  std::string filename = "/data/R15b/tme/Ti50/Ti50.root";
  std::string treename = "TMETree/TMETree";

  std::string detname1 = "SiT_3";
  std::string detname2 = "SiR2";
  std::string detname3 = "SiR3";
  //  std::string detname4 = "SiL3";

  std::string central_channel = "SiT-3-S";
  
  TFile* file = new TFile(filename.c_str(), "READ");
  TTree* tree = (TTree*) file->Get(treename.c_str());

  double min_tdiff = -500;
  double max_tdiff = 500;
  double tdiff_width = 10;
  int n_tdiff_bins = (max_tdiff - min_tdiff) / tdiff_width;

  std::string histname = "hTDiff_" + detname1 + "_" + detname2;
  std::string histtitle = detname1 + " - " + detname2;
  TH1F* hTDiffPair1 = new TH1F(histname.c_str(), histtitle.c_str(), n_tdiff_bins,min_tdiff,max_tdiff);
  hTDiffPair1->SetXTitle(histtitle.c_str());  
  std::string drawcmd = detname1 + ".tblock-" + detname2 + ".tblock>>" + histname;
  std::string cutcmd = "(centralMuonChannel==\"" + central_channel + "\" && @" + detname2 + ".size()==1)";
  int n_entries = tree->GetEntries();
  tree->Draw(drawcmd.c_str(), cutcmd.c_str(), "goff", n_entries);

  histname = "hTDiff_" + detname2 + "_" + detname3;
  histtitle = detname2 + " - " + detname3;
  TH1F* hTDiffPair2 = new TH1F(histname.c_str(), histtitle.c_str(), n_tdiff_bins,min_tdiff,max_tdiff);
  hTDiffPair2->SetXTitle(histtitle.c_str());  
  drawcmd = detname2 + ".tblock-" + detname3 + ".tblock>>" + histname;
  cutcmd = "(centralMuonChannel==\"" + central_channel + "\" && @" + detname3 + ".size()==1)";
  tree->Draw(drawcmd.c_str(), cutcmd.c_str(), "goff", n_entries);

  histname = "hTDiff_" + detname3 + "_" + detname1;
  histtitle = detname3 + " - " + detname1;
  TH1F* hTDiffPair3 = new TH1F(histname.c_str(), histtitle.c_str(), n_tdiff_bins,min_tdiff,max_tdiff);
  hTDiffPair3->SetXTitle(histtitle.c_str());  
  drawcmd = detname3 + ".tblock-" + detname1 + ".tblock>>" + histname;
  cutcmd = "(centralMuonChannel==\"" + central_channel + "\" && @" + detname1 + ".size()==1)";
  tree->Draw(drawcmd.c_str(), cutcmd.c_str(), "goff", n_entries);

  // histname = "hTDiff_" + detname4 + "_" + detname1;
  // histtitle = detname4 + " - " + detname1;
  // TH1F* hTDiffPair4 = new TH1F(histname.c_str(), histtitle.c_str(), n_tdiff_bins,min_tdiff,max_tdiff);
  // hTDiffPair4->SetXTitle(histtitle.c_str());  
  // drawcmd = detname4 + ".tblock-" + detname1 + ".tblock>>" + histname;
  // cutcmd = "(centralMuonChannel==\"" + central_channel + "\" && @" + detname1 + ".size()==1)";
  // tree->Draw(drawcmd.c_str(), cutcmd.c_str(), "goff", n_entries);

  double min_fit_pair1 = -50;
  double max_fit_pair1 = 125;
  TF1* fSigPair1 = new TF1("fSigPair1", "[0]*(TMath::Gaus(x,[1],[2]) + TMath::Gaus(x,[1],[3]))", min_fit_pair1,max_fit_pair1);
  fSigPair1->SetParameters(100, 0, 50, 0, 50);
  double min_fit_pair2 = -50;
  double max_fit_pair2 = 50;
  TF1* fSigPair2 = new TF1("fSigPair2", "[0]*(TMath::Gaus(x,[1],[2]) + TMath::Gaus(x,[1],[3]))", min_fit_pair2,max_fit_pair2);
  fSigPair2->SetParameters(100, 0, 50, 0, 50);
  double min_fit_pair3 = -125;
  double max_fit_pair3 = 50;
  TF1* fSigPair3 = new TF1("fSigPair3", "[0]*(TMath::Gaus(x,[1],[2]) + TMath::Gaus(x,[1],[3]))", min_fit_pair3,max_fit_pair3);
  fSigPair3->SetParameters(100, 0, 50, 0, 50);
  // double min_fit_pair4 = -125;
  // double max_fit_pair4 = 10;
  // TF1* fSigPair4 = new TF1("fSigPair4", "[0]*(TMath::Gaus(x,[1],[2]) + TMath::Gaus(x,[1],[3]))", min_fit_pair4,max_fit_pair4);
  // fSigPair4->SetParameters(100, 0, 50, 0, 50);

  ROOT::Math::WrappedMultiTF1 wfSigPair1(*fSigPair1,1);
  ROOT::Math::WrappedMultiTF1 wfSigPair2(*fSigPair2,1);
  ROOT::Math::WrappedMultiTF1 wfSigPair3(*fSigPair3,1);
  //  ROOT::Math::WrappedMultiTF1 wfSigPair4(*fSigPair4,1);

  ROOT::Fit::DataOptions opt;
  ROOT::Fit::DataRange rangePair1;
  // set the data range
  rangePair1.SetRange(min_fit_pair1,max_fit_pair1);
  ROOT::Fit::BinData dataPair1(opt,rangePair1);
  ROOT::Fit::FillData(dataPair1, hTDiffPair1);
  
  ROOT::Fit::DataRange rangePair2;
  rangePair2.SetRange(min_fit_pair2,max_fit_pair2);
  ROOT::Fit::BinData dataPair2(opt,rangePair2);
  ROOT::Fit::FillData(dataPair2, hTDiffPair2);

  ROOT::Fit::DataRange rangePair3;
  rangePair3.SetRange(min_fit_pair3,max_fit_pair3);
  ROOT::Fit::BinData dataPair3(opt,rangePair3);
  ROOT::Fit::FillData(dataPair3, hTDiffPair3);

  // ROOT::Fit::DataRange rangePair4;
  // rangePair4.SetRange(min_fit_pair4,max_fit_pair4);
  // ROOT::Fit::BinData dataPair4(opt,rangePair4);
  // ROOT::Fit::FillData(dataPair4, hTDiffPair4);

  ROOT::Fit::Chi2Function chi2_Pair1(dataPair1, wfSigPair1);
  ROOT::Fit::Chi2Function chi2_Pair2(dataPair2, wfSigPair2);
  ROOT::Fit::Chi2Function chi2_Pair3(dataPair3, wfSigPair3);
  //  ROOT::Fit::Chi2Function chi2_Pair4(dataPair4, wfSigPair4);

  GlobalChi2 globalChi2(chi2_Pair1, chi2_Pair2, chi2_Pair3);//, chi2_Pair4);

  ROOT::Fit::Fitter fitter;

  const int Npar = 9;//12;
  double height_hist1 = hTDiffPair1->GetMaximum();
  double mean_hist1 = hTDiffPair1->GetBinCenter(hTDiffPair1->GetMaximumBin());
  double sigma_det1 = 50;
  double sigma_det2 = 40;
  double height_hist2 = hTDiffPair2->GetMaximum();
  double mean_hist2 = hTDiffPair2->GetBinCenter(hTDiffPair2->GetMaximumBin());
  double sigma_det3 = 30;
  double height_hist3 = hTDiffPair3->GetMaximum();
  double mean_hist3 = hTDiffPair3->GetBinCenter(hTDiffPair3->GetMaximumBin());
  // double height_hist4 = hTDiffPair4->GetMaximum();
  // double mean_hist4 = hTDiffPair4->GetMean();
  // double sigma_det4 = 50;
  double par0[Npar] = { height_hist1, mean_hist1, sigma_det1, sigma_det2, height_hist2, mean_hist2, sigma_det3, height_hist3, mean_hist3};//, height_hist4, mean_hist4, sigma_det4 };

  // create before the parameter settings in order to fix or set range on them
  fitter.Config().SetParamsSettings(Npar,par0);

  // set the names
  fitter.Config().ParSettings(0).SetName("height_hist1");
  fitter.Config().ParSettings(1).SetName("mean_hist1");
  fitter.Config().ParSettings(2).SetName("sigma_det1");
  fitter.Config().ParSettings(3).SetName("sigma_det2");
  fitter.Config().ParSettings(4).SetName("height_hist2");
  fitter.Config().ParSettings(5).SetName("mean_hist2");
  fitter.Config().ParSettings(6).SetName("sigma_det3");
  fitter.Config().ParSettings(7).SetName("height_hist3");
  fitter.Config().ParSettings(8).SetName("mean_hist3");
  //  fitter.Config().ParSettings(9).SetName("height_hist4");
  //  fitter.Config().ParSettings(10).SetName("mean_hist4");
  //  fitter.Config().ParSettings(11).SetName("sigma_det4");

  // // fix 5-th parameter
  //  fitter.Config().ParSettings(1).Fix();
  //  fitter.Config().ParSettings(5).Fix();
  //  fitter.Config().ParSettings(8).Fix();

  // set limits on the third and 4-th parameter
  //  fitter.Config().ParSettings(2).SetLimits(10, 200); // sigma_det1
  //  fitter.Config().ParSettings(3).SetLimits(10, 200); // sigma_det2
  //  fitter.Config().ParSettings(6).SetLimits(10, 200); // sigma_det3
  //  fitter.Config().ParSettings(1).SetLimits(mean_hist1-10, mean_hist1+10);
  //  fitter.Config().ParSettings(5).SetLimits(mean_hist2-10, mean_hist2+10);
  //  fitter.Config().ParSettings(8).SetLimits(mean_hist3-10, mean_hist3+10);
  //  fitter.Config().ParSettings(3).SetLimits(0,10000);
  //  fitter.Config().ParSettings(3).SetStepSize(5);

  fitter.Config().MinimizerOptions().SetPrintLevel(5);
  fitter.Config().SetMinimizer("Minuit2","Migrad");

  // fit FCN function directly
  // (specify optionally data size and flag to indicate that is a chi2 fit)
  //  fitter.SetFCN(globalChi2,par0);
  fitter.FitFCN(Npar,globalChi2,par0,dataPair1.Size()+dataPair2.Size()+dataPair3.Size(),true);//+dataPair4.Size(),true);
  ROOT::Fit::FitResult result = fitter.Result();
  result.Print(std::cout);

  TCanvas* c = new TCanvas();
  c->Divide(2,2);
  c->cd(1);
  hTDiffPair1->Draw("HIST");
  fSigPair1->SetFitResult(result, iparPair1);
  fSigPair1->Draw("LSAME");

  c->cd(2);
  hTDiffPair2->Draw("HIST");
  fSigPair2->SetFitResult(result, iparPair2);
  fSigPair2->Draw("LSAME");

  c->cd(3);
  hTDiffPair3->Draw("HIST");
  fSigPair3->SetFitResult(result, iparPair3);
  fSigPair3->Draw("LSAME");

  // c->cd(4);
  // hTDiffPair4->Draw("HIST");
  // fSigPair4->SetFitResult(result, iparPair4);
  // fSigPair4->Draw("LSAME");

  // for (int i_central_channel = 0; i_central_channel < n_central_channels; ++i_central_channel) {
  //   std::string central_channel = central_channels[i_central_channel];
  //   std::string central_detname = central_detnames[i_central_channel];

  //   for (int i_other_detname = 0; i_other_detname < n_other_detnames; ++i_other_detname) {
  //     std::string other_detname = other_detnames[i_other_detname];

  //     for (int i_other2_detname = 0; i_other2_detname < n_other2_detnames; ++i_other2_detname) {
  // 	std::string other2_detname = other2_detnames[i_other2_detname];

      
  // 	const int n_pairs = 3;
  // 	std::string det1names[n_pairs] = {other_detname, central_detname, other2_detname};
  // 	std::string det2names[n_pairs] = {central_detname, other2_detname, other_detname};
  // 	// double min_fits[n_pairs] = {-125, -25, -50};
  // 	// double max_fits[n_pairs] = {25, 125, 125};
  // 	double min_fits[n_pairs] = {-100, -25, -50};
  // 	double max_fits[n_pairs] = {50, 100, 50};


  // // std::string det1names[n_pairs] = {"SiR2", "SiT_3", "SiR3"};
  // // std::string det2names[n_pairs] = {"SiT_3", "SiR3", "SiR2"};
  // // double min_fits[n_pairs] = {-125, -25, -50};
  // // double max_fits[n_pairs] = {25, 125, 125};

  // // std::string det1names[n_pairs] = {"SiR2", "SiT_3", "SiR1_3"};
  // // std::string det2names[n_pairs] = {"SiT_3", "SiR1_3", "SiR2"};
  // // double min_fits[n_pairs] = {-200, -25, -50};
  // // double max_fits[n_pairs] = {25, 125, 50};

  // 	double sigmas[n_pairs] = {0};
  // 	double sigma_errs[n_pairs] = {0};


  // 	TCanvas* c = new TCanvas();
  // 	c->Divide(2,2);
  // 	for (int i_pair = 0; i_pair < n_pairs; ++i_pair) {
  // 	  c->cd(i_pair+1);
  // 	  std::string det1name = det1names[i_pair];
  // 	  std::string det2name = det2names[i_pair];
	

  // 	  double min_fit = hTDiff->GetBinCenter(hTDiff->GetMaximumBin()) + min_fits[i_pair];
  // 	  double max_fit = hTDiff->GetBinCenter(hTDiff->GetMaximumBin()) + max_fits[i_pair];
  // 	  TF1* fit_fn = new TF1("fit_fn", "[0]*TMath::Gaus(x, [1], [2])", min_fit, max_fit);
  // 	  fit_fn->SetParameters(hTDiff->GetMaximum(), 0, 100);
  // 	  hTDiff->Fit(fit_fn, "R");

  // 	  sigmas[i_pair] = std::fabs(fit_fn->GetParameter(2));
  // 	  sigma_errs[i_pair] = fit_fn->GetParError(2);

  // 	  TLatex* latex = new TLatex();
  // 	  latex->SetTextAlign(22);
  // 	  latex->SetTextSize(0.05);
  // 	  std::stringstream text;
  // 	  text << std::fixed << std::setprecision(1) << "#sigma = " << sigmas[i_pair] << " #pm " << sigma_errs[i_pair] << " ns";
  // 	  latex->DrawLatexNDC(0.7, 0.7, text.str().c_str());
  // 	}

  // 	for (int i_pair = 0; i_pair < n_pairs; ++i_pair) {
  // 	  std::cout << det1names[i_pair] << " - " << det2names[i_pair] << ": sigma = " << sigmas[i_pair] << " +/- " << sigma_errs[i_pair] << " ns" << std::endl;
  // 	}
  // 	// a^2 + b^2 = X^2 (1)
  // 	// b^2 + c^2 = Y^2 (2)
  // 	// c^2 + a^2 = Z^2 (3)
  // 	// (1)-(2): a^2 - c^2 = X^2-Y^2 (4)
  // 	// (4) + (3): 2a^2 = X^2-Y^2+Z^2 => a^2 = 0.5(X^2-Y^2+Z^2)
  // 	// => b^2 = X^2 - 0.5(X^2-Y^2+Z^2)
  // 	// => c^2 = Z^2 - 0.5(X^2-Y^2+Z^2)
  // 	double det1_sig = std::sqrt(0.5*(sigmas[0]*sigmas[0]-sigmas[1]*sigmas[1]+sigmas[2]*sigmas[2]));
  // 	double det2_sig = std::sqrt(sigmas[0]*sigmas[0] - 0.5*(sigmas[0]*sigmas[0]-sigmas[1]*sigmas[1]+sigmas[2]*sigmas[2]));
  // 	double det3_sig = std::sqrt(sigmas[2]*sigmas[2] - 0.5*(sigmas[0]*sigmas[0]-sigmas[1]*sigmas[1]+sigmas[2]*sigmas[2]));
  // 	std::cout << det1names[0] << ": sigma =  "<< det1_sig << std::endl;
  // 	std::cout << det1names[1] << ": sigma =  "<< det2_sig << std::endl;
  // 	std::cout << det1names[2] << ": sigma =  "<< det3_sig << std::endl;

  // 	c->cd(4);
  // 	TLatex* latex = new TLatex();
  // 	latex->SetTextAlign(22);
  // 	latex->SetTextSize(0.05);
  // 	std::stringstream text;
  // 	text.str("");
  // 	text << std::fixed << std::setprecision(1) << det1names[0] << ": #sigma = " << det1_sig << " ns";
  // 	latex->DrawLatexNDC(0.5, 0.7, text.str().c_str());

  // 	text.str("");
  // 	text << std::fixed << std::setprecision(1) << det1names[1] << ": #sigma = " << det2_sig << " ns";
  // 	latex->DrawLatexNDC(0.5, 0.5, text.str().c_str());

  // 	text.str("");
  // 	text << std::fixed << std::setprecision(1) << det1names[2] << ": #sigma = " << det3_sig << " ns";
  // 	latex->DrawLatexNDC(0.5, 0.3, text.str().c_str());
  //     }
  //   }
  // }
}
