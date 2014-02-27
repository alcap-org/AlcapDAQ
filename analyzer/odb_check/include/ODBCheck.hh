#ifndef ODBCHECK_H__
#define ODBCHECK_H__

#include "WireMap.hh"
#include "DataDir.hh"
#include "PulseEstimate.hh"

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
  void OutputCorrectionsIfNeeded(unsigned int i);

public:
  void Check(int run);

  void LoadODBFromODBFile();
  void LoadODBFromDataFile();
};

#endif
