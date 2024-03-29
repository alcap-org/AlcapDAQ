// EvdE plot for Si16b dataset
// To use (from analyzer/rootana):
// > root -l -b
// > .L scripts/TMETree/FirstMacro.C+
// > FirstMacro("out_tme-tree.root")

#include "TFile.h"
#include "TTree.h"


//#include "AlCapConstants.h"
//#include "TMETreeBranches.h"

#include <iostream>

#include "TH2.h"
#include "TF1.h"
#include "TCanvas.h"

#include "src/plotters/SimplePulse.h"

void Andy_SiL3_Analysis(){

  //  std::string filename = "~/data/out/v??/SiL3.root";
  //  std::string filename = "~/data/out/local/out09040_template-fits_w-tap-tree.root";
  std::string filename = "~/data/out/local/out09040_template-fits.root";
  
  TFile* file = new TFile(filename.c_str(), "READ");
  TTree* taptree = (TTree*) file->Get("TAPTree/TAPTree");

  double min_time = 0;
  double max_time = 20000;
  double time_width = 200;
  int n_time_bins = (max_time - min_time) / time_width;

  double min_energy = 0;
  double max_energy = 20000;
  double energy_width = 100;
  int n_energy_bins = (max_energy - min_energy) / energy_width;

  TH2F* hTimeEnergy = new TH2F("hTimeEnergy", "", n_time_bins,min_time,max_time, n_energy_bins,min_energy,max_energy);
  hTimeEnergy->SetTitle("Andy, SiL3 Dataset");
  hTimeEnergy->SetXTitle("Time Between Consecutive Pulses [ns]");
  hTimeEnergy->SetYTitle("Energy [keV]");

  Int_t blockId;
  TBranch* br_blockId = taptree->GetBranch("blockId");
  br_blockId->SetAddress(&blockId);

  SimplePulse* sil3_s_tap = new SimplePulse();
  taptree->SetBranchAddress("SiL3_S", &sil3_s_tap);
  
  double min_muon_energy = 3000;
  double max_muon_energy = 6000;

  //  bool nearby_muon = true;
  bool muon_found = false;
  Double_t muon_time = 0;
  int muon_tpi_id;
  Double_t prev_time = 0;

  int this_blockId = 0;
  
  std::cout << "Total TAPs = " << taptree->GetEntries() << std::endl;
  int n_entries = taptree->GetEntries();

  for (int i_tap = 0; i_tap < n_entries; ++i_tap) {
    taptree->GetEntry(i_tap);

    if (i_tap % 100000 == 0) {
      std::cout << i_tap << " / " << n_entries << std::endl;
    }

    SimplePulse* this_tap = sil3_s_tap;
    double Amp = this_tap->Amp;
    if (Amp<=0) {
      continue;
    }
    double E = this_tap->E;
    double tblock = this_tap->tblock;
    //    if (E > min_muon_energy && E < max_muon_energy) {
    //      muon_found = true;
    //      muon_time = tblock;
    //      muon_tpi_id = this_tap->tpi_id;
    //      //      std::cout << "Muon! E = " << E << ", t = " << tblock << std::endl;
    //      //      std::cout << "Muon! " << blockId << " " << this_tap->tpi_id << " " << E << " " << muon_time << " " << tblock << " " << tblock-muon_time << std::endl;
    //      //      continue;
    //    }

    //    std::cout << "E = " << E << ", t = " << tblock << ", dt = " << tblock - muon_time << std::endl;
    //    hTimeEnergy->Fill(tblock - muon_time, E);
    //    if (this_tap->tpi_id != 0 && E>2000) {
    if (E > 100) {
      hTimeEnergy->Fill(tblock - prev_time, E);
      prev_time = tblock;
    }
    //    if (tblock - prev_time > 4200 && tblock - prev_time < 4400) {
    //      std::cout << this_tap->tpi_id << std::endl;
    //    }
      //    }
    
      //    } else if (E > max_muon_energy) {
    //    std::cout << " Proton? " << blockId << " " << this_tap->tpi_id << " " << E << " " << muon_time << " " << tblock << " " << tblock-muon_time << std::endl;
    //    if (muon_tpi_id == this_tap->tpi_id){// && E>6000) {
    //      hTimeEnergy->Fill(tblock - muon_time, E);
      //    }
      //    }
  }

  TCanvas* c1 = new TCanvas("c1", "c1");
  hTimeEnergy->Draw("COLZ");

  TCanvas* c2 = new TCanvas("c2", "c2");
  c2->SetLogy();
  TH1D* hTime = hTimeEnergy->ProjectionX("hTime");
  TF1* expo_fit_fn = new TF1("expo_fit_fn", "[0]*TMath::Exp(-x/[1]) + [2]", 2000, 10000);
  expo_fit_fn->SetParameters(hTime->GetMaximum(), 700, 100);
  hTime->Fit(expo_fit_fn, "R");
  hTime->Draw("HIST E");
  expo_fit_fn->Draw("LSAME");

  TF1* actual_lifetime = new TF1("actual_lifetime", "[0]*TMath::Exp(-x/[1]) + [2]", 2000, 10000);
  actual_lifetime->SetParameters(expo_fit_fn->GetParameter(0), 767, expo_fit_fn->GetParameter(2));
  actual_lifetime->SetLineColor(kBlue);
  actual_lifetime->SetLineWidth(2);
  actual_lifetime->Draw("LSAME");


  TCanvas* c3 = new TCanvas("c3", "c3");
  c3->SetLogy();
  TH1D* hEnergy = hTimeEnergy->ProjectionY("hEnergy", hTimeEnergy->GetXaxis()->FindBin(min_time), hTimeEnergy->GetXaxis()->FindBin(max_time)-1);
  TF1* energy_expo_fit_fn = new TF1("energy_expo_fit_fn", "[0]*TMath::Exp(-x/[1])", 6000, 10000);
  energy_expo_fit_fn->SetParameters(hEnergy->GetMaximum(), 700);
  hEnergy->Fit(energy_expo_fit_fn, "R");
  hEnergy->Draw("HIST E");
  energy_expo_fit_fn->Draw("LSAME");
}
