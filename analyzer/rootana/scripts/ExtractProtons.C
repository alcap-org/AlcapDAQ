#include "SiArm.h"

#include "TFile.h"
#include "TDirectoryFile.h"

#include <iostream>

int ExtractProtons(std::string filename) {

  TFile* file = new TFile(filename.c_str(), "READ");
  if (file->IsZombie()) {
    std::cout << "Error: Problem opening file " << filename.c_str() << std::endl;
    return 1;
  }
  // Check that the GeSpectrum folder was created
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

    hEvdE->Draw("COLZ");
  }
  return 0;
}
