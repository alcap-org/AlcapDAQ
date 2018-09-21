#include "TFile.h"
#include "TTree.h"
#include "TH2.h"
#include "TF1.h"
#include "TCutG.h"
#include "TCanvas.h"

#include <iostream>
#include <sstream>

void Si16a_MuonBeamPlots() {

  const int n_runs = 8;
  int run_numbers[n_runs] = {9737, 9736, 9739, 9740, 9741, 9735, 9742, 9743};
  double scale_factors[n_runs] = {0.98, 0.99, 1.01, 1.02, 1.03, 1.035, 1.04, 1.05};

  std::stringstream run_str, sf_str;

  for (int i_run = 0; i_run < n_runs; ++i_run) {
    run_str.str("");
    run_str << "0" << run_numbers[i_run];

    sf_str.str("");
    sf_str << scale_factors[i_run];
    

    std::string infilename = "~/data/results/Si16a_useful/subtrees" + run_str.str() + ".root";
    std::string treename = "siBlockTree_SiT";
    std::string outfilename = "~/data/results/Si16a_useful/plotsMuonBeam" + run_str.str() + ".root";
    double coinc_time = 200;
  
    TFile* in_file = new TFile(infilename.c_str(), "READ");
    if (in_file->IsZombie()) {
      std::cout << "Problem opening file " << infilename << std::endl;
    }
    TTree* armtree = (TTree*) in_file->Get(treename.c_str());
    if (!armtree) {
      std::cout << "Problem getting tree " << treename << std::endl;
    }

    double min_x_energy = 0;
    double max_x_energy = 5000;
    double x_energy_width = 10;
    double min_y_energy = 0;
    double max_y_energy = 2000;
    double y_energy_width = 10;
    int n_x_energy_bins = (max_x_energy - min_x_energy) / x_energy_width;
    int n_y_energy_bins = (max_y_energy - min_y_energy) / y_energy_width;
    std::string histname = "hTwoLayer";
    TH2F* hTwoLayer = new TH2F(histname.c_str(), histname.c_str(), n_x_energy_bins,min_x_energy,max_x_energy, n_y_energy_bins,min_y_energy,max_y_energy);
    std::string histtitle = "Run " + run_str.str() + ", SF = " + sf_str.str();
    hTwoLayer->SetTitle(histtitle.c_str());
    hTwoLayer->SetXTitle("E_{SiT} + E_{SiL1} [keV]");
    hTwoLayer->SetYTitle("E_{SiT} [keV]");

    TH2F* hTwoLayer_Veto = (TH2F*) hTwoLayer->Clone("hTwoLayer_Veto");
    TH2F* hThreeLayer = (TH2F*) hTwoLayer->Clone("hThreeLayer");
    hThreeLayer->SetXTitle("E_{SiT} + E_{SiL1} + E_{SiL3} [keV]");
    hThreeLayer->SetYTitle("E_{SiT} + E_{SiL1} [keV]");

    TH2F* hThirdLayer = (TH2F*) hTwoLayer->Clone("hThirdLayer");
    hThirdLayer->SetXTitle("E_{SiL3} [keV]");
    hThirdLayer->SetYTitle("E_{SiL1} [keV]");

    TH1F* hTotalEnergy = new TH1F("hTotalEnergy", "", n_x_energy_bins,min_x_energy,max_x_energy);

    double thin_energy;
    double thin_time;
    int thin_tpi_id;
    double thick_energy;
    double thick_time;
    int thick_tpi_id;
    double third_energy;
    double third_time;
    int third_tpi_id;
    armtree->SetBranchAddress("thin_energy", &thin_energy);
    armtree->SetBranchAddress("thin_time", &thin_time);
    armtree->SetBranchAddress("thin_tpi_id", &thin_tpi_id);
    armtree->SetBranchAddress("thick_energy", &thick_energy);
    armtree->SetBranchAddress("thick_time", &thick_time);
    armtree->SetBranchAddress("thick_tpi_id", &thick_tpi_id);
    armtree->SetBranchAddress("third_energy", &third_energy);
    armtree->SetBranchAddress("third_time", &third_time);
    armtree->SetBranchAddress("third_tpi_id", &third_tpi_id);
  
    int n_entries = armtree->GetEntries();
    for (int i_entry = 0; i_entry < n_entries; ++i_entry) {
      armtree->GetEntry(i_entry);
      if (i_entry % 10000 == 0) {
	std::cout << i_entry << " / " << n_entries << std::endl;
      }

      if (thin_tpi_id<0 || thick_tpi_id<0) {
	continue;
      }
    
      if (std::fabs(thin_time) >= coinc_time) {
	continue; // to next arm event
      }

      if (std::fabs(thin_time - thick_time) >= coinc_time) {
	continue; // to next arm event
      }

      hTwoLayer->Fill(thin_energy+thick_energy, thin_energy);
      if (third_tpi_id<0) {
	hTwoLayer_Veto->Fill(thin_energy+thick_energy, thin_energy);
	hTotalEnergy->Fill(thin_energy+thick_energy);
      }
      else {
	if (std::fabs(thick_time - third_time) < coinc_time) {
	  hThreeLayer->Fill(thin_energy+thick_energy+third_energy, thin_energy+thick_energy);
	  hTotalEnergy->Fill(thin_energy+thick_energy+third_energy);
	  hThirdLayer->Fill(third_energy, thick_energy);
	}
      } 
    }

    std::string pdfbasename = "~/plots/2018-05-09/AlCapData_Run";
    TCanvas* c_two_layer = new TCanvas("c_two_layer", "c_two_layer");
    hTwoLayer->Draw("COLZ");
    std::string pdfname = pdfbasename + run_str.str() + "_MuonBeamPlots_TwoLayer.pdf";
    c_two_layer->SaveAs(pdfname.c_str());

    TCanvas* c_two_layer_veto = new TCanvas("c_two_layer_veto", "c_two_layer_veto");
    hTwoLayer_Veto->Draw("COLZ");
    pdfname = pdfbasename + run_str.str() + "_MuonBeamPlots_TwoLayer_Veto.pdf";
    c_two_layer_veto->SaveAs(pdfname.c_str());

    TCanvas* c_three_layer = new TCanvas("c_three_layer", "c_three_layer");
    hThreeLayer->Draw("COLZ");
    pdfname = pdfbasename + run_str.str() + "_MuonBeamPlots_ThreeLayer.pdf";
    c_three_layer->SaveAs(pdfname.c_str());


    TFile* outfile = new TFile(outfilename.c_str(), "UPDATE");
    hTwoLayer->Write();
    hTwoLayer_Veto->Write();
    hThreeLayer->Write();
    hTotalEnergy->Write();
    hThirdLayer->Write();
    //  cuttree->Fill();
    //  cuttree->Write();

    outfile->Write();
    outfile->Close();
  }
}
