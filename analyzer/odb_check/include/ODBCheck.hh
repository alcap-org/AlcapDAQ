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

  void SetDirs(const std::string& raw, const std::string& odb, const std::string& hist, const std::string& corr);
  void SetDirs();

  void SetOutputCorrections();
  void ResetOutputCorrections();

private:
  void LoadODBValues();
  void InitiateCorrectionsFile();
  void OutputCorrectionsIfNeeded(unsigned int bank_index, TH2* shapes, TH1* timing, int run_number);
  void FillMonitorPlots(std::string bank_name, std::string fieldname, int run_number, double value);

public:
  void Check(int run);

  void LoadODBFromODBFile();
  void LoadODBFromDataFile();
};

#endif
