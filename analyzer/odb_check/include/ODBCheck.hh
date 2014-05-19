/************************************
Class: ODBCheck
Author: John R. Quirk

ODBCheck looks simply for inconsistencies between the alcapana
data quality output histograms and the original ODB files,
then outputs a corrected ODB file for the run to be loaded on the
next alcapana production.
************************************/

#ifndef ODBCHECK_H__
#define ODBCHECK_H__

#include "WireMap.hh"
#include "DataDir.hh"
#include "PulseEstimate.hh"

#include "TH1.h"
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
  // Check if the estimates are different than the ODB values,
  // and output the estimates to a correction file if so,
  // or the original ODB values if not.
  void OutputCorrections(WireMap corr);

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
