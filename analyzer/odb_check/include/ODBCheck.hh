#ifndef ODBCHECK_H__
#define ODBCHECK_H__

#include "WireMap.hh"
#include "DataDir.hh"
#include "PulseEstimate.hh"

#include "TH1.h"
#include "TH2.h"

#include <string>
#include <fstream>


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

  void SetDirs(const std::string& raw, const std::string& odb, const std::string& hist, const std::string& corr);
  void SetDirs();

  void SetOutputCorrections();
  void ResetOutputCorrections();

private:
  void LoadODBValues();
  void InitiateCorrectionsFile();
  void OutputCorrectionsIfNeeded(unsigned int bank_index, TH2* shapes, TH1* timing);

public:
  void Check(int run);

  void LoadODBFromODBFile();
  void LoadODBFromDataFile();
};

#endif
