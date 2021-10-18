// A first example macro
// -- it loops through the first 100 TMEs in the TMETree and will print all its information
// To use (from analyzer/rootana):
// > root -l -b
// > .L scripts/TMETree/FirstMacro.C+
// > FirstMacro("out_tme-tree.root")

#include "TFile.h"
#include "TTree.h"

#include "AlCapConstants.h"
#include "TMETreeBranches.h"

#include <iostream>

void DoubleCountCheck(std::string filename) {

  TFile* file = new TFile(filename.c_str(), "READ");
  TTree* tmetree = (TTree*) file->Get("TMETree/TMETree");

  SetTMEBranchAddresses(tmetree); // setup the branch addresses

  int n_entries = tmetree->GetEntries();
  for (int i_tme = 0; i_tme < n_entries; ++i_tme) {

    if (i_tme % 100000 == 0) {
      std::cout << i_tme << " / " << n_entries << std::endl;
    }

    tmetree->GetEntry(i_tme);
    CollectChannels(); // collect all channels into various groups (e.g. SiL1, SiR1, All) for easy looping

    if (anyDoubleCountedPulses) {
      continue; // to next TME
    }
    //    std::cout << "Run " << runId << " Block " << blockId << " TME " << TMEId << ":" << std::endl;
    //    std::cout << "This TME uses a +/- " << TMEWindowWidth << " ns window" << std::endl;
    //    std::cout << "Previous TME was " << timeToPrevTME << " ns ago and next TME will be in " << timeToNextTME << " ns (both times muon-to-muon)" << std::endl << std::endl;
    //    std::cout << "Central Muon: E = " << centralMuonEnergy << " keV, t = " << centralMuonTime << " ns (" << *centralMuonChannel << ", TPI ID " << centralMuonTPIID << ")" << std::endl << std::endl;

    //    std::cout << "All pulses in this TME (E [keV], t [ns], bitmask):" << std::endl;
    std::vector<SimplePulse>* i_pulse_list = SiL3;
    int n_pulses = i_pulse_list->size();
    //    std::cout << "N SiL3 Pulses = " << n_pulses << std::endl;
    if (n_pulses > 1) {
      std::vector<int> tpi_ids_seen;
      std::vector<int> tap_ids_seen;
      std::vector<double> tTMEs_seen;
      for (int i_pulse = 0; i_pulse < n_pulses; ++i_pulse) {
	int this_tpi_id = i_pulse_list->at(i_pulse).tpi_id;
	int this_tap_id = i_pulse_list->at(i_pulse).tap_id;
	double this_tTME = i_pulse_list->at(i_pulse).tTME;
	//	std::cout << "Pulse #" << i_pulse << ": " << this_tpi_id << " " << this_tap_id << " " << this_tTME << std::endl;
	for (int i_id = 0; i_id < tpi_ids_seen.size(); ++i_id) {
	  int prev_tpi_id = tpi_ids_seen.at(i_id);
	  int prev_tap_id = tap_ids_seen.at(i_id);
	  double prev_tTME = tTMEs_seen.at(i_id);
	  //	  std::cout << "Checking " << prev_tpi_id << " against " << this_tpi_id << ", " << prev_tap_id << " against " << this_tap_id << ", " << prev_tTME << " against " << this_tTME << std::endl;
	  if (this_tpi_id==prev_tpi_id && this_tap_id==prev_tap_id && std::fabs(this_tTME - prev_tTME)<1e-5) {
	    std::cout << "Duplicate: Run " << runId << ", Block " << blockId << ", TME " << TMEId << ", TPI " << this_tpi_id << ", TAP " << this_tap_id << ", tTME " << this_tTME << std::endl;
	  }
	}
	tpi_ids_seen.push_back(this_tpi_id);
	tap_ids_seen.push_back(this_tap_id);
	tTMEs_seen.push_back(this_tTME);
      }
    }

    //    std::cout << "=======================================" << std::endl;
    //    std::cout << std::endl;
  }
}
