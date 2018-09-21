#include "TFile.h"
#include "TH2.h"
#include "TF1.h"
#include "TCutG.h"
#include "TCanvas.h"

#include <sstream>

struct BandExtractionEfficiencyArgs {
  std::string indatafilename;
  std::string indatahistname;

  std::vector<std::string> inmcfilenames;
  std::vector<std::string> inmchistnames;
  std::vector<std::string> incutnames;

  std::string outfilename;
};

void BandExtractionEfficiency(const BandExtractionEfficiencyArgs& args) {
  TFile* data_file = new TFile(args.indatafilename.c_str(), "READ");
  if (data_file->IsZombie()) {
    std::cout << "Problem opening file " << args.indatafilename << std::endl;
    return;
  }
  TH2F* hEvdEData = (TH2F*) data_file->Get(args.indatahistname.c_str());
  if (!hEvdEData) {
    std::cout << "Problem getting histogram " << args.indatahistname << std::endl;
    return;
  }
  hEvdEData->GetYaxis()->SetRangeUser(0, 3000);
  hEvdEData->GetXaxis()->SetRangeUser(0, 17000);
  hEvdEData->SetMarkerColor(kRed);
  hEvdEData->SetLineColor(kRed);

  hEvdEData->Draw("");
  
  int n_particles = args.inmchistnames.size();
  for (int i_particle = 0; i_particle < n_particles; ++i_particle) {

    std::string inmcfilename = args.inmcfilenames[i_particle];
    std::string inmchistname = args.inmchistnames[i_particle];
    std::string incutname = args.incutnames[i_particle];
    
    TFile* mc_file = new TFile(inmcfilename.c_str(), "READ");
    if (mc_file->IsZombie()) {
      std::cout << "Problem opening file " << inmcfilename << std::endl;
      return;
    }

    TH2F* hEvdEMC = (TH2F*) mc_file->Get(inmchistname.c_str());
    if (!hEvdEMC) {
      std::cout << "Problem getting histogram " << inmchistname << std::endl;
      return;
    }
    TCutG* tcutg = (TCutG*) data_file->Get(incutname.c_str());
    if (!tcutg) {
      std::cout << "Problem getting cut " << incutname << std::endl;
      return;
    }

    hEvdEMC->GetYaxis()->SetRangeUser(0, 3000);
    hEvdEMC->GetXaxis()->SetRangeUser(0, 17000);

    hEvdEMC->Draw("P SAME");
    
    tcutg->SetLineWidth(2);
    tcutg->SetLineColor(kMagenta);
    tcutg->Draw("LSAME");
  }
  /*  TH1F* hBandExtractionEfficiency = new TH1F("hBandExtractionEff", "", hEvdEMC->GetXaxis()->GetNbins(), hEvdEMC->GetXaxis()->GetXmin(), hEvdEMC->GetXaxis()->GetXmax());
  for (int i_bin = 1; i_bin <= hBandExtractionEfficiency->GetNbinsX(); ++i_bin) {
    double contents_inside_cut = 0;
    double total_contents = 0;
    for (int j_bin = 1; j_bin <= hEvdEMC->GetYaxis()->GetNbins(); ++j_bin) {
      double x = hEvdEMC->GetXaxis()->GetBinCenter(i_bin);
      double y = hEvdEMC->GetYaxis()->GetBinCenter(j_bin);

      double bin_content = hEvdEMC->GetBinContent(i_bin, j_bin);
      if (tcutg->IsInside(x, y)) {
	contents_inside_cut += bin_content;
      }

      total_contents += bin_content;
    }

    if (total_contents > 0) {
      double eff = contents_inside_cut / total_contents;
      hBandExtractionEfficiency->SetBinContent(i_bin, eff);
    }
  }
  */
  /*  double min_energy_slice = 4000;
  double max_energy_slice = 4500;
  int min_energy_slice_bin = hEvdEData->GetXaxis()->FindBin(min_energy_slice);
  int max_energy_slice_bin = hEvdEData->GetXaxis()->FindBin(max_energy_slice);
  hEvdEData->ProjectionY("_py", min_energy_slice_bin, max_energy_slice_bin)->Draw();
  hEvdEMC->ProjectionY("_py", min_energy_slice_bin, max_energy_slice_bin)->Draw("SAME");
  */
  //  TCanvas* c = new TCanvas("c", "c");
  //  hBandExtractionEfficiency->Draw();
  
  TFile* outfile = new TFile(args.outfilename.c_str(), "UPDATE");
  outfile->Write();
  outfile->Close();

};
