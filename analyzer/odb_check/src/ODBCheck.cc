#include "ODBCheck.hh"

#include "TFile.h"
#include "TH1.h"

#include <string>
#include <iostream>

ODBCheck::ODBCheck() : fRun(0), fODB(), fCorrections(), fDataDirs(),
		       fEstimate(), fCorrectionsFile() {
}

ODBCheck::~ODBCheck() {
}

void ODBCheck::SetDirs(const std::string& raw, const std::string& odb, const std::string& hist, const std::string& corr) {
  fDataDirs.SetRawDir(raw);
  fDataDirs.SetODBDir(odb);
  fDataDirs.SetHistDir(hist);
  fDataDirs.SetCorrDir(corr);
}

void ODBCheck::OutputCorrections() {
  // Some constants used in this function
  static const std::string header("[/Analyzer/Wiremap]");
  static const std::string pol_key("TriggerPolarity = INT");
  static const std::string ped_key("Pedestal = INT");
  static const std::string time_key("TimeShift = FLOAT");

  static const std::string key_tail_sm("[48] :"); // < Run 2173
  static const std::string key_tail_lg("[52] :"); // >= Run 2173
  static std::string key_tail;
  
  // Open the file
  if (fCorrectionsFile.is_open()) {
    std::cout << "ODBCheck WARNING: Corrections file already open. Closing old file..." << std::endl;
    fCorrectionsFile.close();
  }
  fCorrectionsFile.open(fDataDirs.GetCorrFileName(fRun).c_str());
  if (!fCorrectionsFile.is_open()) {
    std::cout <<
      "ODBCheck ERROR: Cannot open correction file (" <<
      fDataDirs.GetCorrFileName(fRun) << ")!" <<
      std::endl <<
      "                Corrections file not written!" <<
      std::endl;
    return;
  }
  
  // Resize corrected wiremap for consistency
  //fCorrections.ResizeToBanks();

  // Sanity check
  // If the number of detectors is 52, the run number must be greater than 2173
  // If it is 48, the run must have been before then
  // Otherwise, there's an error
  int ndets = fCorrections.GetNDets();
  if ((ndets == 48 && fCorrections.GetRun() >= 2173) ||
      (ndets == 52 && fCorrections.GetRun() < 2173)) {
    std::cout << "ODBCheck ERROR: Corrected wiremap has run (" << fCorrections.GetRun() <<
      ") and number of detectors (" << ndets << ") that disagree with how the run went!" <<
      std::endl << "                Not outputting corrections." << std::endl;
    return;
  }
    
  // The tail of the keys depends on the size
  if (ndets == 48) {
    key_tail = key_tail_sm;
  } else if (ndets == 52) {
    key_tail = key_tail_lg;
  } else {
    std::cout <<
      "ODBCheck ERROR: Corrected WireMap has detector count not 48 or 52! (" <<
      fCorrections.GetNDets() << ")" << std::endl;
    return;
  }

  // Write the file
  // The lines must be of the form
  // [INDEX] VALUE
  fCorrectionsFile << header << std::endl;
  fCorrectionsFile << pol_key << key_tail << std::endl;
  for (int idet = 0; idet < ndets; ++idet)
    fCorrectionsFile << "[" << idet << "]" <<
      fCorrections.GetPolarities()[idet] <<
      std::endl; 
  fCorrectionsFile << ped_key << key_tail << std::endl;
  for (int idet = 0; idet < ndets; ++idet)
    fCorrectionsFile << "[" << idet << "]" <<
      fCorrections.GetPedestals()[idet] <<
      std::endl; 
  fCorrectionsFile << time_key << key_tail << std::endl;
  for (int idet = 0; idet < ndets; ++idet)
    fCorrectionsFile << "[" << idet << "]" <<
      fCorrections.GetOffsets()[idet] - fODB.GetOffsets()[idet] <<
      std::endl; 
  fCorrectionsFile << std::endl;

  fCorrectionsFile.close();
}

void ODBCheck::Check(int run) {
  TFile hist_file(fDataDirs.GetHistFileName(run).c_str(), "READ");

  if (!hist_file.IsOpen()) {
    std::cout <<
      "ODBCheck ERROR: Cannot open histogram file! (" <<
      fDataDirs.GetHistFileName(run).c_str() << ")" <<
      std::endl;
    return;
  }

  if (fRun == run)
    std::cout <<
      "ODBCheck MESSAGE: It seems you've already checked this run (" <<
      fRun << "). Checking again...";

  fRun = run;
  std::string fname(fDataDirs.GetODBFileName(fRun));
  WireMap run_odb(fRun, fname);
  fODB = WireMap::Default();
  fODB.LoadOver(run_odb);
  fCorrections.Clear();
  fCorrections.SetRun(fRun);

  // The value we're looping over is the bank names in the ODB.
  // Some corresponding detector names are not real,
  // such as "blank". We hope these histograms do not exist so
  // that we don't process them unnecessarily.
  for (unsigned int i = 0; i < fODB.GetNDets(); ++i) {
    // Ignore certian things
    if (fODB.GetBanks()[i] == "ZZZZ" || fODB.GetDets()[i] == "blank") {
      fCorrections.Add(fODB, i);
      continue;
    }
    // We look for the shapes and timing histograms
    TH1* shapes;
    TH1* timing;
    hist_file.GetObject(("DataQuality_LowLevel/hDQ_PulseShapes_ProjectionY_" + fODB.GetDets()[i] + "_" + fODB.GetBanks()[i]).c_str(), shapes);
    hist_file.GetObject(("DataQuality_LowLevel/hDQ_muScTDiff_" + fODB.GetDets()[i] + "_" + fODB.GetBanks()[i]).c_str(), timing);

    // Only if both histograms are present and filled are corrections estimated
    if (!shapes) {
      std::cout <<
	"ODBCheck WARNING: Shapes histogram not found! Corrections not included for " <<
	fODB.GetDets()[i] << "_" << fODB.GetBanks()[i] << "..." <<
	std::endl;
      fCorrections.Add(fODB, i);
    } else if (!timing) {
      std::cout <<
	"ODBCheck WARNING: Timing histogram not found! Corrections not included for " <<
	fODB.GetDets()[i] << "_" << fODB.GetBanks()[i] << "..." <<
	std::endl;
      fCorrections.Add(fODB, i);
    } else if (!shapes->GetEntries()) {
      std::cout <<
	"ODBCheck WARNING: Shapes histogram empty! Corrections not included for " <<
	fODB.GetDets()[i] << "_" << fODB.GetBanks()[i] << "..." <<
	std::endl;
      fCorrections.Add(fODB, i);
    } else if (!timing->GetEntries()) {
      std::cout <<
	"ODBCheck WARNING: Timing histogram empty! Corrections not included for " <<
	fODB.GetDets()[i] << "_" << fODB.GetBanks()[i] << "..." <<
	std::endl;
      fCorrections.Add(fODB, i);
    } else {
      fEstimate.Estimate(shapes, timing);
      fCorrections.Add(fODB.GetBanks()[i], fODB.GetDets()[i], fEstimate.GetPedestal(), fEstimate.GetPolarity(), 0, fEstimate.GetOffset());
    }
    delete shapes;
    delete timing;
    shapes = NULL;
    timing = NULL;
  }
  hist_file.Close();

  OutputCorrections();
}
