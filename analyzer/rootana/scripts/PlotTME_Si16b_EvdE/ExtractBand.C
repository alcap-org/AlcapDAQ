#include "scripts/PlotTME_Si16b_EvdE/SiArm.h"
#include "scripts/PlotTME_Si16b_EvdE/Cut.h"

#include "TFile.h"
#include "TDirectoryFile.h"
#include "TF1.h"
#include "TCanvas.h"
#include "TNtuple.h"

#include "RooWorkspace.h"
#include "RooDataHist.h"
#include "RooPlot.h"
#include "RooAbsPdf.h"
#include "RooRealVar.h"

#include <iostream>
#include <sstream>

int ExtractBand(std::string filename, const std::vector<Cut*>& cuts, std::string dirname, std::string ntuplename, std::string x_varname, std::string y_varname) {

  TFile* file = new TFile(filename.c_str(), "READ");
  if (file->IsZombie()) {
    std::cout << "Error: Problem opening file " << filename.c_str() << std::endl;
    return 1;
  }
  // Check that the correct EvdE folder was created
  TDirectoryFile* evde_dir = (TDirectoryFile*) file->Get(dirname.c_str());
  if (!evde_dir) {
    std::cout << "Error: " << dirname << " folder doesn't exist in output file" << std::endl;
    return 1;
  }

  // Form the full TCut
  TCut full_cut;
  for (std::vector<Cut*>::const_iterator i_cut = cuts.begin(); i_cut != cuts.end(); ++i_cut) {
    full_cut += (*i_cut)->cut;
  }
  //  full_cut += "SiR1Time<2000 && SiR1Time>=100";
  std::cout << "Full Cut = " << full_cut << std::endl;
  

  // Create the two arms so we can have a loop
  // (only need one arm for Si16b but keep the loop anyway)
  SiArm* right_arm = new SiArm("SiR");
  std::vector<SiArm*> arms;
  arms.push_back(right_arm);

  for (std::vector<SiArm*>::iterator i_arm = arms.begin(); i_arm != arms.end(); ++i_arm) {

    std::string canvasname = "c_" + (*i_arm)->GetArmName();
    TCanvas* c = new TCanvas(canvasname.c_str(), canvasname.c_str());

    // Get the ntuple of hits for this arm
    TNtuple* arm_hits = (TNtuple*) evde_dir->Get(ntuplename.c_str());
    if (!arm_hits) {
      std::cout << "Error: Could not find " << ntuplename << std::endl;
      return 1;
    }

    // Now create the EvdE plot
    double min_x_energy = 0;
    double max_x_energy = 20000;
    double min_y_energy = 0;
    double max_y_energy = 3000;
    double energy_width = 10;
    int n_x_energy_bins = (max_x_energy - min_x_energy) / energy_width;
    int n_y_energy_bins = (max_y_energy - min_y_energy) / energy_width;

    std::string histname = "hEvdE_" + (*i_arm)->GetArmName();
    TH2F* hEvdE = new TH2F(histname.c_str(), histname.c_str(), n_x_energy_bins,min_x_energy,max_x_energy, n_y_energy_bins,min_y_energy,max_y_energy);
    std::string drawcmd = y_varname + ":" + x_varname + ">>" + histname;
    arm_hits->Draw(drawcmd.c_str(), "", "goff");
    (*i_arm)->SetEvdEPlot(hEvdE);
    
    // Create a new histogram to store the band itself
    histname = (*i_arm)->GetArmName() + "_EvdEBand";
    TH2F* hEvdEBand = (TH2F*) hEvdE->Clone(histname.c_str());
    hEvdEBand->Reset();
    drawcmd = y_varname + ":" + x_varname + ">>" + histname;
    arm_hits->Draw(drawcmd.c_str(), full_cut, "goff");
    hEvdEBand->ResetStats();
    
    std::cout << (*i_arm)->GetArmName() << ": " << hEvdEBand->GetEntries() << " entries" << std::endl;

    // Integrate
    double integral_low = 2500;
    double integral_high = 10000;
    double bin_integral_low = hEvdEBand->GetXaxis()->FindBin(integral_low);
    double bin_integral_high = hEvdEBand->GetXaxis()->FindBin(integral_high);
    std::cout << "Integral (" << integral_low/1000 << " - " << integral_high/1000 << " MeV) = " 
	      << hEvdEBand->ProjectionX()->Integral(bin_integral_low, bin_integral_high) << std::endl;

    // Draw
    //    hEvdE->Draw("COLZ");
    hEvdEBand->Draw("COLZ");
    c->SetLogz();
    c->Update();

    // Also draw the cuts
    for (std::vector<Cut*>::const_iterator i_cut = cuts.begin(); i_cut != cuts.end(); ++i_cut) {
      (*i_cut)->Draw(c);
    }

    TCanvas* c_folded = new TCanvas("c_folded", "c_folded");
    TH1D* hFoldedSpectrum = hEvdEBand->ProjectionX();
    hFoldedSpectrum->Rebin(50); // rebin to 500 keV bins
    hFoldedSpectrum->Draw("HIST E");


    // Now draw the lifetime plot
    TCanvas* c_lifetime = new TCanvas("c_lifetime", "c_lifetime");
    c_lifetime->SetLogy();
    TH1F* hLifetime = new TH1F("hLifetime", "Layer 1 Hit Times of Extracted Band", 120,-5000,10000);
    arm_hits->Draw("SiR1Time>>hLifetime", full_cut, "goff");
    TF1* lifetime_fn = new TF1("lifetime_fn", "[0]*TMath::Exp(-x/[1])", 100,2000);
    lifetime_fn->SetParameters(1000, 750);
    hLifetime->Fit(lifetime_fn, "R");
    hLifetime->Draw("HIST E");
    lifetime_fn->Draw("LSAME");
    //    RooWorkspace* ws = new RooWorkspace("ws", kTRUE);
    //    std::stringstream factory_cmd, sum_factory_cmd;
    //    factory_cmd << "scale[100, 5000]*Exponential::si_life(time[250,5000], lifetime[-2e-3, -1e-3])";
    //    std::cout << factory_cmd.str() << std::endl;
    //    ws->factory(factory_cmd.str().c_str());
    //    factory_cmd.str("");

    //    RooDataHist data("data", "Time Spectrum", (ws->argSet("time")), hLifetime);
    //    (ws->pdf("si_life"))->fitTo(data);

    //    RooPlot* Eframe = (ws->var("time"))->frame();
    //    data.plotOn(Eframe);
    //    (ws->pdf("si_life"))->plotOn(Eframe);
    //    ws->Print();
    //    std::cout << -1.0/ws->var("lifetime")->getValV() << " +/- " << (-1.0/ws->var("lifetime")->getValV())*(ws->var("lifetime")->getError()/ws->var("lifetime")->getValV()) << std::endl;
    //    Eframe->Draw();


  }
  return 0;
}
