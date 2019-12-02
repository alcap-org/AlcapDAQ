#include "TCanvas.h"
#include "TLatex.h"
#include "TH2.h"
#include "TF2.h"
#include "TFile.h"
#include "../../..//Al50/psel.h"

std::vector<ParticleLikelihood::PSelPow> pls_r;
std::vector<ParticleLikelihood::PSelPow> pls_r_3L;
//Double_t pfcn_r(Double_t* x, Double_t* par) { return pls_r[0](x[1], x[0]); }

void Si16b_FinalPlot_PIDEffs() {

  TFile* outfile = new TFile("~/data/mc/Si16b/pid-effs.root", "RECREATE");
    
  const int n_layer_cfgs = 2;
  int n_layers[n_layer_cfgs] = {2, 3};
  
  const int n_particles = 4;
  std::string particles[n_particles] = {"proton", "deuteron", "triton", "alpha"};
  Int_t colours[n_particles] = {kRed, kCyan, kMagenta, kSpring};
  TH2F* hEvdEs[n_particles][n_layer_cfgs] = {0};
  TH2F* hMCEvdEs[n_particles][n_layer_cfgs] = {0};
  TH1F* hNTrue[n_particles] = {0};
  TH1F* hNTrueSelected[n_particles] = {0};
  TH1F* hNFakeSelected[n_particles] = {0};
  TH1F* hNTrueSelectedErr2[n_particles] = {0};
  TH1F* hNFakeSelectedErr2[n_particles] = {0};
  TH1F* hEffs[n_particles] = {0};
  TH1F* hPurities[n_particles] = {0};
  //  double x_rebin_factors[n_particles] = {50, 50, 50, 50};
  double x_rebin_factors[n_particles] = {100, 100, 100, 100};
  double y_rebin_factors[n_particles] = {5, 5, 5, 5};
  //  double x_rebin_factors_mc[n_particles] = {10, 10, 10, 10};
  double x_rebin_factors_mc[n_particles] = {20, 20, 20, 20};
  double y_rebin_factors_mc[n_particles] = {1, 1, 1, 1};
  //  double x_rebin_factors[n_particles] = {1, 1, 1, 1};
  double x_maxes[n_particles] = {25000, 25000, 25000, 30000};
  
  bool is_si16b_analysis = true;
  pls_r = ParticleLikelihood::LoadParticleLikelihoodsPow('r', is_si16b_analysis);
  pls_r_3L = ParticleLikelihood::LoadThreeLayerParticleLikelihoodsPow('r', is_si16b_analysis);

  for (int i_particle = 0; i_particle < n_particles; ++i_particle) {
    std::string particle = particles[i_particle];
    std::string infilename = "~/data/results/Si16b/plots_newPP_geq1TgtPulse.root";
    TFile* infile = new TFile(infilename.c_str(), "READ");

    for (int i_layer_cfg = 0; i_layer_cfg < n_layer_cfgs; ++i_layer_cfg) {
      int i_n_layers = n_layers[i_layer_cfg];

      std::string inhistname = particle + "_SiR_timecut0_10000ns_layerCoinc_PSel/";
      if (i_n_layers==2) {
	inhistname += "hEvdE_TwoLayer_12not3";
      }
      else if (i_n_layers==3) {
	inhistname += "hEvdE_ThreeLayer_123";
      }
      else {
	continue;
      }

      TH2F* hEvdE = (TH2F*) infile->Get(inhistname.c_str());
      if (!hEvdE) {
	std::cout << "Problem getting " << inhistname << std::endl;
	return;
      }
      //  hEvdE->SetTitle(histtitle.c_str());
      hEvdE->SetStats(false);
      std::string histtitle = "Si16b Data";
      hEvdE->SetTitle(histtitle.c_str());
      hEvdE->SetStats(false);
      hEvdE->Rebin2D(x_rebin_factors[i_particle], y_rebin_factors[i_particle]);
      hEvdE->GetXaxis()->SetRangeUser(0, x_maxes[i_particle]);

      std::string mcfilename = "~/data/mc/Si16b/respMatrix_10M_Geom-P5_" + particle + "_500keVBins.root";
      TFile* mcfile = new TFile(mcfilename.c_str(), "READ");
      std::string mchistname = "SiR_";
      if (i_n_layers==2) {
	mchistname += "hEvdE_TwoLayer";
      }
      else if (i_n_layers==3) {
	mchistname += "EvdE_ThreeLayer";
      }
      else {
	continue;
      }
      mchistname += "_stopped";

      TH2F* hMCEvdE = (TH2F*) mcfile->Get(mchistname.c_str());
      if (!hMCEvdE) {
	std::cout << "Problem getting " << mchistname << std::endl;
	return;
      }
      std::string mchisttitle = "Si16b MC";
      hMCEvdE->SetTitle(mchisttitle.c_str());
      hMCEvdE->SetStats(false);
      hMCEvdE->Rebin2D(x_rebin_factors_mc[i_particle], y_rebin_factors_mc[i_particle]);
      hMCEvdE->GetXaxis()->SetRangeUser(0, x_maxes[i_particle]);
      
      hEvdEs[i_particle][i_layer_cfg] = hEvdE;
      hMCEvdEs[i_particle][i_layer_cfg] = hMCEvdE;
    }
    
    std::string h_eff_name = "eff_" + particle;
    hEffs[i_particle] = (TH1F*) hEvdEs[i_particle][0]->ProjectionX(h_eff_name.c_str());
    hEffs[i_particle]->Reset();
    std::string h_purity_name = "purity_" + particle;
    hPurities[i_particle] = (TH1F*) hEvdEs[i_particle][0]->ProjectionX(h_purity_name.c_str());
    hPurities[i_particle]->Reset();

    std::string h_ntrue_name = "ntrue_" + particle;
    hNTrue[i_particle] = (TH1F*) hEvdEs[i_particle][0]->ProjectionX(h_ntrue_name.c_str());
    hNTrue[i_particle]->Reset();
    std::string h_ntrueselected_name = "ntrueselected_" + particle;
    hNTrueSelected[i_particle] = (TH1F*) hEvdEs[i_particle][0]->ProjectionX(h_ntrueselected_name.c_str());
    hNTrueSelected[i_particle]->Reset();
    std::string h_nfakeselected_name = "nfakeselected_" + particle;
    hNFakeSelected[i_particle] = (TH1F*) hEvdEs[i_particle][0]->ProjectionX(h_nfakeselected_name.c_str());
    hNFakeSelected[i_particle]->Reset();
    std::string h_ntrueselectederr_name = "ntrueselectederr_" + particle;
    hNTrueSelectedErr2[i_particle] = (TH1F*) hEvdEs[i_particle][0]->ProjectionX(h_ntrueselectederr_name.c_str());
    hNTrueSelectedErr2[i_particle]->Reset();
    std::string h_nfakeselectederr_name = "nfakeselectederr_" + particle;
    hNFakeSelectedErr2[i_particle] = (TH1F*) hEvdEs[i_particle][0]->ProjectionX(h_nfakeselectederr_name.c_str());
    hNFakeSelectedErr2[i_particle]->Reset();
  }

  int start_bin = 1;
  int end_bin = hEvdEs[0][0]->GetXaxis()->GetNbins();
  for (int i_particle = 0; i_particle < n_particles; ++i_particle) {
    std::string particle = particles[i_particle];
    
    for (int i_layer_cfg = 0; i_layer_cfg < n_layer_cfgs; ++i_layer_cfg) {
      int i_n_layers = n_layers[i_layer_cfg];

      //  TF2* fpr = new TF2("fpr", pfcn_r, 0., 20.e3, 0., 10.e3);
      //  Double_t fcont[1] = { 1.e-4 };
      //  fpr->SetNpx(200); fpr->SetNpy(200); fpr->SetContour(1, fcont);
      //  fpr->Draw("SAME");
      TH2F* hEvdE = hEvdEs[i_particle][i_layer_cfg];
      std::cout << "AE: " << i_particle << ", " << i_layer_cfg << std::endl;
      std::cout << "AE: " << hEvdE->GetName() << std::endl;
      TH2F* hMCEvdE = hMCEvdEs[i_particle][i_layer_cfg];
      for (int i_bin = start_bin; i_bin <= end_bin; ++i_bin) {
	double E = hEvdE->GetXaxis()->GetBinCenter(i_bin);

	std::stringstream proj_name;
	proj_name.str("");
	proj_name << "mc_" << particle << "_layercfg" << i_layer_cfg << "_E" << E << "_py";
	TH1D* hProjYMC = hMCEvdE->ProjectionY(proj_name.str().c_str(), i_bin, i_bin);
	hProjYMC->SetLineColor(kRed);
	if (hProjYMC->GetEntries()<=0) {
	  continue;
	}
	std::string mc_fit_fn = "gaus";
	TFitResultPtr mc_result = hProjYMC->Fit(mc_fit_fn.c_str(), "QSL");
	//      result->Print();
	//      std::cout << result->Status() << std::endl;
	//      if (result->Status() != 0) {
	if (!hProjYMC->GetFunction(mc_fit_fn.c_str())) {
	  continue;
	}
	//      else if (hProjY->GetFunction(fit_fn.c_str())->GetParameter(1)<0){// || hProjY->GetFunction(fit_fn.c_str())->GetNDF() < 5) {
	//	continue;
	//      }
	//      std::cout << fit_fn << " fitted!" << std::endl;
	//      TCanvas* c = new TCanvas();
	//      hProjY->Draw("HIST E");
	proj_name.str("");
	proj_name << "data_" << particle << "_layercfg" << i_layer_cfg << "_E" << E << "_py";
	TH1D* hProjY = hEvdE->ProjectionY(proj_name.str().c_str(), i_bin, i_bin);
	if (hProjY->GetEntries()<=0) {
	  continue;
	}
	std::vector<ParticleLikelihood::PSelPow>* this_pls_r;
	if (i_n_layers==2) {
	  this_pls_r = &pls_r;
	}
	else if (i_n_layers==3 && i_particle==0) { // only have 3L selection for protons
	  this_pls_r = &pls_r_3L;
	}
	else {
	  continue;
	}
	TF1* data_fit = new TF1("data_fit", "[0]*TMath::Gaus(x, [1], [2])", this_pls_r->at(i_particle).getFunc(0)->Eval(E), this_pls_r->at(i_particle).getFunc(1)->Eval(E));
	data_fit->SetParameter(0, hProjY->GetMaximum());
	//	data_fit->FixParameter(1, hProjYMC->GetFunction(mc_fit_fn.c_str())->GetParameter(1));
	//	data_fit->FixParameter(2, hProjYMC->GetFunction(mc_fit_fn.c_str())->GetParameter(2));
	data_fit->SetLineColor(kBlue);
	data_fit->SetParameter(1, hProjYMC->GetFunction(mc_fit_fn.c_str())->GetParameter(1));
	data_fit->SetParameter(2, hProjYMC->GetFunction(mc_fit_fn.c_str())->GetParameter(2));
	std::string data_fit_fn = "data_fit";
	TFitResultPtr data_result = hProjY->Fit(data_fit_fn.c_str(), "QSLR");
	if (!hProjY->GetFunction(data_fit_fn.c_str())) {
	  continue;
	}
	hNTrue[i_particle]->AddBinContent(i_bin, hProjY->GetFunction(data_fit_fn.c_str())->Integral(0, 40000));
	std::cout << "E = " << E << " keV: True (" << particles[i_particle] << ") = " << hNTrue[i_particle]->GetBinContent(i_bin) << std::endl;
	for (int i_pls = 0; i_pls < n_particles; ++i_pls) {	  
	  std::vector<ParticleLikelihood::PSelPow>* this_pls_r;
	  if (i_n_layers==2) {
	    this_pls_r = &pls_r;
	  }
	  else if (i_n_layers==3 && i_pls==0) { // only have 3L selection for protons
	    this_pls_r = &pls_r_3L;
	  }
	  else {
	    continue;
	  }
	
	  double minE = 0;
	  double maxE = 0;
	  this_pls_r->at(i_pls).getFunc(0)->GetRange(minE, maxE);
	  if (E < minE || E > maxE) {
	    continue;
	  }
	  double min_integral = this_pls_r->at(i_pls).getFunc(0)->Eval(E);
	  double max_integral = this_pls_r->at(i_pls).getFunc(1)->Eval(E);
	  double selected = hProjY->GetFunction(data_fit_fn.c_str())->Integral(min_integral, max_integral);
	  double error = hProjY->GetFunction(data_fit_fn.c_str())->IntegralError(min_integral, max_integral);
	  if (i_pls == i_particle) {
	    hNTrueSelected[i_particle]->AddBinContent(i_bin, selected);
	    hNTrueSelectedErr2[i_particle]->AddBinContent(i_bin, error*error);
	    std::cout << "E = " << E << " keV: Correct (for " << particles[i_particle] << ") = " << selected << std::endl;
	  }
	  else {
	    hNFakeSelected[i_pls]->AddBinContent(i_bin, selected);
	    hNFakeSelectedErr2[i_pls]->AddBinContent(i_bin, error*error);
	    std::cout << "E = " << E << " keV: Incorrect (for " << particles[i_pls] << ") = " << selected << std::endl;
	  }
	}

	outfile->cd();
	hProjYMC->Write();
	hProjY->Write();
      }
    }
  }

  for (int i_particle = 0; i_particle < n_particles; ++i_particle) {
    TH2F* hEvdE = hEvdEs[i_particle][0];
    std::string particle = particles[i_particle];
    for (int i_bin = start_bin; i_bin <= end_bin; ++i_bin) {
      double E = hEvdE->GetXaxis()->GetBinCenter(i_bin);
      
      double n_total_true = hNTrue[i_particle]->GetBinContent(i_bin);
      double n_true_selected = hNTrueSelected[i_particle]->GetBinContent(i_bin);
      double n_true_selected_err = hNTrueSelectedErr2[i_particle]->GetBinContent(i_bin);
      double n_fake_selected_err = hNFakeSelectedErr2[i_particle]->GetBinContent(i_bin);
      double n_fake_selected = hNFakeSelected[i_particle]->GetBinContent(i_bin);
      double n_total_selected = n_true_selected + n_fake_selected;

      double eff = 0;
      double eff_error = 0;
      double purity = 0;
      double purity_error = 0;
      n_true_selected_err = std::sqrt(n_true_selected_err);
      n_fake_selected_err = std::sqrt(n_fake_selected_err);
      double n_total_selected_err = std::sqrt(n_true_selected_err*n_true_selected_err + n_fake_selected_err*n_fake_selected_err);
      if (n_total_true > 0 && n_total_selected>0) {
	std::cout << "True Total = " << n_total_true << ", Selected Total = " << n_total_selected << " (Correct = " << n_true_selected << ", Incorrect = " << n_fake_selected << ")" << std::endl;
	eff = n_true_selected / n_total_true;
	eff_error = std::sqrt( (eff*(1-eff)) / n_total_true );//(n_true_selected_err / n_true_selected) * eff;
	purity = n_true_selected / n_total_selected;
	purity_error = std::sqrt( (purity*(1-purity)) / n_total_selected);
	std::cout << particle << " E = " << E << " keV: Eff = " << eff << " +/- " << eff_error << ", Purity = " << purity << " +/- " << purity_error << std::endl;
      }
      else {
	std::cout << particle << " E = " << E << " keV: no entries" << std::endl;
      }
      hEffs[i_particle]->SetBinContent(i_bin, eff);
      hEffs[i_particle]->SetBinError(i_bin, eff_error);
      hPurities[i_particle]->SetBinContent(i_bin, purity);
      hPurities[i_particle]->SetBinError(i_bin, purity_error);
    }
  }

  /*
  TCanvas* c_EvdE = new TCanvas("c_EvdE", "c_EvdE");
  //  hEvdE->Draw("COLZ");
  TH2F* hEvdEAll = NULL;
  for (int i_particle = 0; i_particle < n_particles; ++i_particle) {
  TH2F* hEvdE = hEvdEs[i_particle][0];
    if (!hEvdEAll) {
      hEvdEAll = (TH2F*) hEvdE->Clone("hEvdEAll");
    }
    else {
      hEvdEAll->Add(hEvdE);
    }
  }
  
  //  hEvdEAll->GetYaxis()->SetRangeUser(0, 10000);
  hEvdEAll->Draw("COLZ");
  //  std::cout << "x-axis bin width = " << hEvdEAll->GetXaxis()->GetBinWidth(1) << " keV" << std::endl;
  //  std::cout << "y-axis bin width = " << hEvdEAll->GetYaxis()->GetBinWidth(1) << " keV" << std::endl;
  for (int i_pls = 0; i_pls < n_particles; ++i_pls) {      
    TF1* lower = pls_r[i_pls].getFunc(0);
    lower->SetLineColor(colours[i_pls]);

    TF1* upper = pls_r[i_pls].getFunc(1);
    upper->SetLineColor(colours[i_pls]);
    
    double minE = 0;
    double maxE = 0;
    pls_r[i_pls].getFunc(0)->GetRange(minE, maxE);
    std::cout << minE << " " << maxE << std::endl;
    std::cout << pls_r[i_pls].getFunc(0)->Eval(minE) << std::endl;
    std::cout << pls_r[i_pls].getFunc(0)->GetMaximum() << std::endl;
    TLine* minE_line = new TLine(minE, pls_r[i_pls].getFunc(0)->Eval(minE), minE, pls_r[i_pls].getFunc(1)->Eval(minE));
    minE_line->SetLineWidth(2);
    minE_line->SetLineColor(colours[i_pls]);
    minE_line->Draw("LSAME");
      
    TLine* maxE_line = new TLine(maxE, pls_r[i_pls].getFunc(0)->Eval(maxE), maxE, pls_r[i_pls].getFunc(1)->Eval(maxE));
    maxE_line->SetLineWidth(2);
    maxE_line->SetLineColor(colours[i_pls]);
    maxE_line->Draw("LSAME");
      
    lower->DrawF1(minE-100, maxE+100, "LSAME");
    upper->DrawF1(minE-100, maxE+100, "LSAME");
  }
  */

  outfile->cd();
  for (int i_particle = 0; i_particle < n_particles; ++i_particle) {
    hEffs[i_particle]->Write();
    hPurities[i_particle]->Write();
  }
  //  outfile->Write();
  outfile->Close();
}
