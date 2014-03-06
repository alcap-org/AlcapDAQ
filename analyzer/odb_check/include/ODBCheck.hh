#ifndef ODBCHECK_H__
#define ODBCHECK_H__

#include "WireMap.hh"
#include "DataDir.hh"
#include "PulseEstimate.hh"

#include "TH1.h"
#include "TH2.h"
#include "TFile.h"

#include <string>
#include <fstream>
#include <map>


class ODBCheck {
private:
  int fRun;
  WireMap fODB;
  DataDir fDataDirs;
  bool fLoadODBFile;
  PulseEstimate fEstimate;
  std::ofstream fCorrectionsFile;

  TFile* fMonitorPlotsFile;
  // This might be too confusing but the idea is that we have 
  // a map of a string to a second map (where the string here is the fieldname e.g. Pedestal, TimeShift)
  // the second map is then a string to a histogram (where the string is the bank_name)
  std::map<std::string, std::map<std::string, TH1F*> > fHistogramMap;

public:
  ODBCheck();
  ~ODBCheck();

  // Set the directories to look for the different files we might need. Store in DataDir object.
  // First signature sets values according to arguments.
  // Second signature prompts.
  // The directories are, in order:
  // Raw data directory (MIDAS files; input)
  // ODB data directory (ODB files; input)
  // Histograms directory (hist?????.root files; input)
  // Corrections directory (correction*.dat files for rootana; output)
  void SetDirs(const std::string& raw, const std::string& odb, const std::string& hist, const std::string& corr);
  void SetDirs();

private:
  // Load the values from either the ODB file (fLoadODBFile == true) or the raw data file (fLoadODBFile == false).
  // Store in a WireMap object. Used to compare to estimates.
  void LoadODBValues();
  // Open a corrections file for this run and fill with header.
  void InitiateCorrectionsFile();
  // Check if the estimates are different than the ODB values,
  // and output the estimates to a correction file if so.
  void OutputCorrectionsIfNeeded(unsigned int bank_index, TH2* shapes, TH1* timing, int run_number);
  // Fill the appropriate monitoring histogram with the estimated value.
  // This way we can keep track of trends in banks for the pedestal, polarity, and offset.
  void FillMonitorPlots(std::string bank_name, std::string fieldname, int run_number, double value);

public:
  /*** The bulk ***/
  // Using the histogram output from alcapana, make estimates of the time offset from
  // MuSC, the pedestal, and the polarity. Output a corrections file for any
  // estimates different from the ODB values and keep track of everything in a trends
  // file.
  void Check(int run);

  // We can load the ODB values from either the ODB file or
  // the raw data file (ODB is dumped in ASCII format in
  // the header). This tells the ODBCheck which to do.
  void LoadODBFromODBFile();
  void LoadODBFromDataFile();
};

#endif
