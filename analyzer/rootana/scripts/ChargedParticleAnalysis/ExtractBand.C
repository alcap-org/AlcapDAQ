#include "SiArm.h"
#include "Cut.h"

#include "TFile.h"
#include "TDirectoryFile.h"
#include "TF1.h"

#include <iostream>

int ExtractBand(std::string filename, std::vector<Cut*>& cuts) {

  TFile* file = new TFile(filename.c_str(), "READ");
  if (file->IsZombie()) {
    std::cout << "Error: Problem opening file " << filename.c_str() << std::endl;
    return 1;
  }
  // Check that the correct EvdE folder was created
  TDirectoryFile* evde_dir = (TDirectoryFile*) file->Get("TME_Al50_EvdE");
  if (!evde_dir) {
    std::cout << "Error: TME_Al50_EvdE folder doesn't exist in output file" << std::endl;
    return 1;
  }

  // Create the two arms so we can have a loop
  SiArm* left_arm = new SiArm("SiL");
  SiArm* right_arm = new SiArm("SiR");
  std::vector<SiArm*> arms;
  arms.push_back(left_arm); arms.push_back(right_arm);

  for (std::vector<SiArm*>::iterator i_arm = arms.begin(); i_arm != arms.end(); ++i_arm) {

    // Get the EvdE plot for this arm
    std::string histname = "all_particles/" + (*i_arm)->GetArmName() + "_EvdE";
    TH2F* hEvdE = (TH2F*) evde_dir->Get(histname.c_str());
    if (!hEvdE) {
      std::cout << "Error: Could not find " << histname << std::endl;
      return 1;
    }
    (*i_arm)->SetEvdEPlot(hEvdE);
    
    // Create a new histogram to store the band itself
    histname = (*i_arm)->GetArmName() + "_EvdEBand";
    TH2F* hEvdEBand = (TH2F*) hEvdE->Clone(histname.c_str());
    hEvdEBand->Reset();
    // Now go through and apply the cuts
    int n_bins_x = hEvdE->GetXaxis()->GetNbins();
    int n_bins_y = hEvdE->GetYaxis()->GetNbins();
    for (int i_bin = 1; i_bin <= n_bins_x; ++i_bin) {
      for (int j_bin = 1; j_bin <= n_bins_y; ++j_bin) {

	double x_coord = hEvdE->GetXaxis()->GetBinCenter(i_bin);
	double y_coord = hEvdE->GetYaxis()->GetBinCenter(j_bin);
	double bin_content = hEvdE->GetBinContent(i_bin, j_bin);

	bool remove = false;
	for (std::vector<Cut*>::const_iterator i_cut = cuts.begin(); i_cut != cuts.end(); ++i_cut) {
	  TF1* fn = (*i_cut)->eqn;

	  // If we want to remove entries below this line
	  if ( (*i_cut)->remove_below ) {
	    if (y_coord < fn->Eval(x_coord) ) {
	      remove = true;
	      break;
	    }
	  }
	  else { // if we want to remove entries above this line
	    if (y_coord > fn->Eval(x_coord) ) {
	      remove = true;
	      break;
	    }
	  }
	}
	if (bin_content < 10) {
	  remove = true;
	}

	if (remove) {
	  hEvdEBand->SetBinContent(i_bin, j_bin, 0);
	}
	else {
	  hEvdEBand->SetBinContent(i_bin, j_bin, bin_content);
	}
      }
    }
    hEvdEBand->ResetStats();
    std::cout << (*i_arm)->GetArmName() << ": " << hEvdEBand->GetEntries() << " entries" << std::endl;

    // Integrate
    double integral_low = 2500;
    double integral_high = 10000;
    double bin_integral_low = hEvdEBand->GetXaxis()->FindBin(integral_low);
    double bin_integral_high = hEvdEBand->GetXaxis()->FindBin(integral_high);
    std::cout << "Integral (" << integral_low/1000 << " - " << integral_high/1000 << " MeV) = " 
	      << hEvdEBand->ProjectionX()->Integral(bin_integral_low, bin_integral_high) << std::endl;
    hEvdEBand->Draw("COLZ");
  }
  return 0;
}
