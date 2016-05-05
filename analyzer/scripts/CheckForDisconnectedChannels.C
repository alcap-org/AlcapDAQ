// CheckForDisconnectedChannels.C
// -- takes an alcapana hist file with DQ_IslandCounter histograms and checks that there are pulses in all detectors

#include <iostream>
#include <cstring>
#include <sstream>
#include <iomanip>

#include "TFile.h"
#include "TCollection.h"
#include "TKey.h"
#include "TDirectory.h"
#include "TH1.h"

int CheckForDisconnectedChannels(std::string filename, bool verbose = false);

void CheckForDisconnectedChannels(int first_run, int last_run, bool verbose = false) {

  std::stringstream filename;
  for (int i_run = first_run; i_run <= last_run; ++i_run) {
    std::cout << "Run #" << i_run << ": ";

    filename.str(""); // clear the stringstream
    filename << "hist" << std::setw(5) << std::setfill('0') << i_run << ".root";

    CheckForDisconnectedChannels(filename.str(), verbose);
    std::cout << std::endl;
  }
}

int CheckForDisconnectedChannels(std::string filename, bool verbose) {

  TFile* file = new TFile(filename.c_str(), "READ");
  if (file->IsZombie()) {
    //    std::cout << std::endl << "ERROR: File " << filename << " is a zombie!" << std::endl;
    return 1;
  }

  TDirectory* dir = file->GetDirectory("DQ_IslandCounter");
  if (!dir) {
    std::cout << std::endl << "ERROR: No DQ_IslandCounter folder! Need to have run the MDQ_IslandCounter module first" << std::endl;
    return 1;
  }

  TIter next_hist(dir->GetListOfKeys());
  TKey *key;
  while ( key = (TKey*)next_hist() ) {
    //    printf("key: %s points to an object of class: %s at %d\n", key->GetName(), key->GetClassName(),key->GetSeekKey());
    std::string keyname = key->GetName();

    // Only want the unnormalised hIslandCounter histograms
    // i.e. not hIslandRate or with suffix _Norm
    if (keyname.find("IslandRate") != std::string::npos || keyname.find("_Norm") != std::string::npos) {
      //      std::cout << keyname << " being skipped" << std::endl;
      continue;
    }

    TH1F* hist = (TH1F*) dir->Get(keyname.c_str());
    if (verbose) {
      std::cout << keyname << " has mean of " << hist->GetMean() << std::endl;
    }
    if (hist->GetMean() == 0) {
      // Get the detector name out of the histogram name
      char * cstr = new char [keyname.length()+1];
      std::strcpy(cstr, keyname.c_str());

      char* detname = strtok(cstr, "_");
      detname = strtok(NULL, "_");
       
      std::cout << detname << " ";
    }
  }

  return 0;
}
