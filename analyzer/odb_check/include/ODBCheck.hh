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
  WireMap fCorrections;
  DataDir fDataDirs;
  PulseEstimate fEstimate;
  std::ofstream fCorrectionsFile;

public:
  ODBCheck();
  ~ODBCheck();

  // Set the directories to look for the different files we might need. Store in DataDir object.
  // The directories are, in order:
  // Raw data directory (MIDAS files; input)
  // ODB data directory (ODB files; input)
  // Histograms directory (hist?????.root files; input)
  // Corrections directory (correction*.dat files for rootana; output)
  void SetDirs(const std::string& raw, const std::string& odb, const std::string& hist, const std::string& corr);

private:
  // Output the estimates to a correction file (ODB format)
  void OutputCorrections();

public:
  /*** The bulk ***/
  // Using the histogram output from alcapana, make estimates of the time offset from
  // MuSC, the pedestal, and the polarity. Output a corrections file for any
  // estimates different from the ODB values and keep track of everything in a trends
  // file.
  void Check(int run);
};

#endif
