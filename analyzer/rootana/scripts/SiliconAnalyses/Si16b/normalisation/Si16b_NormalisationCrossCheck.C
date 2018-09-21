#include "scripts/XRayAnalysis/XRayUtils.h"

#include "TLine.h"
#include "TFile.h"
#include "TTree.h"
#include "TBox.h"
#include "TCanvas.h"
#include "TLatex.h"
#include "TGraphErrors.h"
#include "TF1.h"
#include "TAxis.h"
#include "TH1.h"

#include "RooWorkspace.h"
#include "RooPlot.h"
#include "RooRealVar.h"
#include "RooAbsData.h"
#include "RooAbsPdf.h"

#include <iomanip>
#include <iostream>
#include <sstream>

void Si16b_NormalisationCrossCheck() {

  std::string infilename = "~/data/results/Si16b_passive/normalisation.root";
  TFile* file = new TFile(infilename.c_str(), "READ");

  double min_slice = 500;
  double max_slice = 1500;
  double slice_step = 100;
  const int n_slices = 10;
  double muon_slices[n_slices];
  double muon_slice_errors[n_slices];
  for (int i_slice = 0; i_slice < n_slices; ++i_slice) {
    muon_slices[i_slice] = min_slice + (i_slice+0.5)*slice_step;
    muon_slice_errors[i_slice] = slice_step/2;
  }
  double calc_effs[n_slices];
  double calc_eff_errs[n_slices];

  std::stringstream muon_slice;
  std::string ge_channel = "GeLoGain";
  for (double i_muon_slice_start = min_slice; i_muon_slice_start < max_slice; i_muon_slice_start += slice_step) {
    int i_slice = (i_muon_slice_start - min_slice) / slice_step;
    double i_muon_slice_end = i_muon_slice_start + slice_step;
    muon_slice.str("");
    muon_slice << "MuonSlice" << (int)i_muon_slice_start << "_" << (int)i_muon_slice_end;

    std::string direct_dirname = "Target_" + muon_slice.str();
    //    std::string direct_dirname = "TargetSpectrum";
    std::string direct_histname = direct_dirname + "/hSpectrum";
    std::string direct_count_treename = direct_dirname + "/counttree";

    std::string indirect_dirname = ge_channel + "_wTargetCoinc_" + muon_slice.str();
    //    std::string indirect_dirname = "XRaySpectrum_" + ge_channel + "_wTimeCut";
    std::string indirect_count_treename = indirect_dirname + "/counttree";
    std::string wsname = indirect_dirname + "/ws";

    std::cout << "Trees:" << std::endl;
    std::cout << "\tDirect Count = " << direct_count_treename << std::endl;
    std::cout << "\tIndirect Count = " << indirect_count_treename << std::endl;
    
    TTree* direct_count_tree = (TTree*) file->Get(direct_count_treename.c_str());
    double direct_count = 0;
    double direct_count_error = 0;
    double min_muon_time = 0;
    double max_muon_time = 0;
    double min_muon_energy = 0;
    double max_muon_energy = 0;
    direct_count_tree->SetBranchAddress("n_stopped_muons", &direct_count);
    direct_count_tree->SetBranchAddress("n_stopped_muons_error", &direct_count_error);
    direct_count_tree->SetBranchAddress("min_muon_time", &min_muon_time);
    direct_count_tree->SetBranchAddress("max_muon_time", &max_muon_time);
    direct_count_tree->SetBranchAddress("min_muon_energy", &min_muon_energy);
    direct_count_tree->SetBranchAddress("max_muon_energy", &max_muon_energy);
    direct_count_tree->GetEntry(0);  
    std::cout << "Direct Count = " << direct_count << " +/- " << direct_count_error << " (" << direct_count - direct_count_error << " -- " << direct_count + direct_count_error << ")" << std::endl;
    
    TTree* indirect_count_tree = (TTree*) file->Get(indirect_count_treename.c_str());
    double indirect_count = 0;
    double indirect_count_error = 0;
    double xray_count = 0;
    double xray_count_error = 0;
    double xray_intensity = 0;
    double xray_intensity_error = 0;
    double xray_efficiency = 0;
    double xray_efficiency_error = 0;
    indirect_count_tree->SetBranchAddress("n_stopped_muons", &indirect_count);
    indirect_count_tree->SetBranchAddress("n_stopped_muons_error", &indirect_count_error);
    indirect_count_tree->SetBranchAddress("xray_count", &xray_count);
    indirect_count_tree->SetBranchAddress("xray_count_error", &xray_count_error);
    indirect_count_tree->SetBranchAddress("xray_intensity", &xray_intensity);
    indirect_count_tree->SetBranchAddress("xray_intensity_error", &xray_intensity_error);
    indirect_count_tree->SetBranchAddress("xray_efficiency", &xray_efficiency);
    indirect_count_tree->SetBranchAddress("xray_efficiency_error", &xray_efficiency_error);
    indirect_count_tree->GetEntry(0);
    std::cout << "Indirect Count = " << indirect_count << " +/- " << indirect_count_error << " (" << indirect_count - indirect_count_error << " -- " << indirect_count + indirect_count_error << ")" << std::endl;
    std::cout << "XRay Count = " << xray_count << " +/- " << xray_count_error << std::endl;
    
    double calculated_efficiency = (xray_count / (xray_intensity * direct_count));
    double calculated_efficiency_error = ( (xray_count_error*xray_count_error)/(xray_count*xray_count) ) + ( (xray_intensity_error*xray_intensity_error)/(xray_intensity*xray_intensity) ) + ( (direct_count_error*direct_count_error)/(direct_count*direct_count) );
    calculated_efficiency_error = calculated_efficiency*std::sqrt(calculated_efficiency_error);
    std::cout << "Original Eff = (" << xray_efficiency*1e4 << " +/- " << xray_efficiency_error*1e4 << ") x 10^{-4}" << std::endl;
    std::cout << "Calculated Eff = (" << calculated_efficiency*1e4 << " +/- " << calculated_efficiency_error*1e4 << ") x 10^{-4}" << std::endl;

    calc_effs[i_slice] = calculated_efficiency;
    calc_eff_errs[i_slice] = calculated_efficiency_error;
    
    ///////////////////////////////////////////////////
    // Target Spectrum
    TCanvas* c_direct = new TCanvas("c_direct", "c_direct");
    TH1F* hTarget_Spectrum = (TH1F*) file->Get(direct_histname.c_str());
    hTarget_Spectrum->SetTitle("Target Spectrum");
    hTarget_Spectrum->SetStats(false);
    hTarget_Spectrum->Draw();

    std::stringstream latex_text;
    latex_text << "#splitline{" << min_muon_time << " < t < " << max_muon_time << " ns}{" << min_muon_energy << " < E < " << max_muon_energy << " keV}";
    TLatex* latex = new TLatex();
    latex->SetTextAlign(22);
    latex->DrawLatexNDC(0.5, 0.75, latex_text.str().c_str());
    
    TLine* min_muon_energy_line = new TLine(min_muon_energy, hTarget_Spectrum->GetMinimum(), min_muon_energy, hTarget_Spectrum->GetMaximum());
    min_muon_energy_line->SetLineWidth(2);
    min_muon_energy_line->Draw("LSAME");
    
    TLine* max_muon_energy_line = new TLine(max_muon_energy, hTarget_Spectrum->GetMinimum(), max_muon_energy, hTarget_Spectrum->GetMaximum());
    max_muon_energy_line->SetLineWidth(2);
    max_muon_energy_line->Draw("LSAME");
    
    latex_text.str("");
    latex_text << "N_{stop} = (" << std::fixed << std::setprecision(3) << direct_count/1e6 << " #pm " << direct_count_error/1e6 << ") x 10^{6}";
    latex->DrawLatexNDC(0.5, 0.4, latex_text.str().c_str());
    
    
    ///////////////////////////////////////////////////
    // X-Ray Spectrum
    TCanvas* c_indirect = new TCanvas("c_indirect", "c_indirect");
    RooWorkspace* ws = (RooWorkspace*) file->Get(wsname.c_str());
    ws->Print();
    
    // Draw the fit
    RooPlot* Eframe = (ws->var("edep"))->frame();
    Eframe->SetTitle("X-Ray Spectrum");
    Eframe->GetXaxis()->SetTitle("Energy [keV]");
    //  Eframe->SetMinimum(3000);
    (ws->data("data"))->plotOn(Eframe);
    (ws->pdf("sum"))->plotOn(Eframe);
    Eframe->Draw();
    
    latex_text.str("");
    latex_text << "#splitline{  #varepsilon = (" << std::setprecision(2) << xray_efficiency*1e4 << " #pm " << xray_efficiency_error*1e4 << ") x 10^{-4}}{#rightarrow N_{stop} = (" << std::fixed << std::setprecision(1) << indirect_count/1e6 << " #pm " << indirect_count_error/1e6 << ") x 10^{6}}";
    latex->DrawLatexNDC(0.3, 0.75, latex_text.str().c_str());
    
    latex_text.str("");
    latex_text << "#splitline{  N_{x-ray} = " << std::fixed << std::setprecision(0) << xray_count << " #pm " << xray_count_error << "}{#rightarrow #varepsilon = (" << std::setprecision(2) << calculated_efficiency*1e4 << "#pm" << calculated_efficiency_error*1e4 << ") x 10^{-4}}";
    latex->DrawLatexNDC(0.75, 0.75, latex_text.str().c_str());

    //    std::string pdfname = "~/plots/2018-05-08/AlCapData_NormalisationCrossCheck_SiL3DatasetNoTemplates_" + muon_slice.str();
    //    std::string direct_pdfname = pdfname + "_CrossCheckTargetSpectrum.pdf";
    //    std::string indirect_pdfname = pdfname + "_CrossCheckXRaySpectrum.pdf";
    //    c_direct->SaveAs(direct_pdfname.c_str());
    //    c_indirect->SaveAs(indirect_pdfname.c_str());
  }

  TCanvas* c_gres = new TCanvas("c_gres", "c_gres");
  TGraphErrors* gre = new TGraphErrors(n_slices, muon_slices, calc_effs, muon_slice_errors, calc_eff_errs);
  gre->Draw("A*E");
  gre->SetTitle("Efficiency Comparisons (Si16b Dataset)");
  gre->GetXaxis()->SetTitle("Energy Slice [keV]");
  gre->GetYaxis()->SetTitle("Calculated Efficiency");
  gre->GetYaxis()->SetRangeUser(0, 10e-4);

  const int n_ref_efficiencies = 3;
  std::string ref_labels[n_ref_efficiencies] = {" run 9302", "  run 9685", " run 10139"};
  int ge_eff_runs[n_ref_efficiencies] = {9302, 9685, 10319};
  double ref_eff_val[n_ref_efficiencies];
  double ref_eff_err[n_ref_efficiencies];
  TLatex* latex = new TLatex();
  latex->SetTextSize(0.03);
  double latex_x = 0;
  double latex_y = 0;
  Int_t colours[n_ref_efficiencies] = {kBlack, kRed, kBlue};
  double energy = 400.177;
  
  for (int i_ref_eff = 0; i_ref_eff < n_ref_efficiencies; ++i_ref_eff) {

    double a, b, delta_a, delta_b, corr;
    FillGeEffParams(ge_channel, ge_eff_runs[i_ref_eff], a, b, delta_a, delta_b, corr);

    TF1* ge_eff_fn = 0;
    TF1* ge_eff_err_fn = 0;
    FillGeEffFunctions(a, b, delta_a, delta_b, corr, ge_eff_fn, ge_eff_err_fn);

    ref_eff_val[i_ref_eff] = ge_eff_fn->Eval(energy);
    ref_eff_err[i_ref_eff] = ge_eff_err_fn->Eval(energy);

    Int_t i_colour = colours[i_ref_eff];
    
    TBox* ref_eff_box = new TBox(gre->GetXaxis()->GetXmin(), ref_eff_val[i_ref_eff]-ref_eff_err[i_ref_eff], gre->GetXaxis()->GetXmax(), ref_eff_val[i_ref_eff]+ref_eff_err[i_ref_eff]);
    ref_eff_box->SetFillStyle(3004);
    ref_eff_box->SetFillColor(i_colour);
    ref_eff_box->SetLineColor(i_colour);
    ref_eff_box->Draw("LSAME");

    TLine* ref_eff_line = new TLine(gre->GetXaxis()->GetXmin(), ref_eff_val[i_ref_eff], gre->GetXaxis()->GetXmax(), ref_eff_val[i_ref_eff]);
    ref_eff_line->SetLineColor(i_colour);
    ref_eff_line->SetLineWidth(2);
    ref_eff_line->Draw("LSAME");

    latex_x = gre->GetXaxis()->GetXmax();
    latex_y = ref_eff_val[i_ref_eff];//+ref_eff_err[i_ref_eff];
    latex->SetTextAlign(12);
    latex->SetTextColor(i_colour);
    latex->DrawLatex(latex_x, latex_y, ref_labels[i_ref_eff].c_str());
  }  

  
    //  c_gres->SaveAs("~/plots/2018-04-24/AlCapData_NormalisationCrossCheck_SiL3Dataset.pdf");
}
