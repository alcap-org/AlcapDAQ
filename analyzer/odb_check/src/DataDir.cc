#include "DataDir.hh"

#include <string>
#include <iostream>
#include <cstdio>

DataDir::DataDir() : fRawExt(".mid"), fODBExt(".odb"), fHistExt(".root"), fCorrExt(".odb"),
		     fRawDir(""), fODBDir(""), fHistDir(""), fCorrDir(""),
		     fRawPre("run"), fODBPre("run"), fHistPre("hist"), fCorrPre("corr") {}

std::string DataDir::GetCanonicalRun(int run) {
  static const std::string def("00000");
  char num[8];
  if (run < 0 || run > 99999)
    return def;
  sprintf(num, "%05d", run);
  return std::string(num);
}

void DataDir::SetRawDir(const std::string& raw_dir) {
  fRawDir = raw_dir;
}
void DataDir::SetRawDir() {
  std::cout << "Enter raw data directory: ";
  std::cin >> fRawDir;
}

void DataDir::SetODBDir(const std::string& odb_dir) {
  fODBDir = odb_dir;
}

void DataDir::SetODBDir() {
  std::cout << "Enter ODB directory: ";
  std::cin >> fODBDir;
}

void DataDir::SetHistDir(const std::string& hist_dir) {
  fHistDir = hist_dir;
}

void DataDir::SetHistDir() {
  std::cout << "Enter histogram directory: ";
  std::cin >> fHistDir;
}

void DataDir::SetCorrDir(const std::string& corr_dir) {
  fCorrDir = corr_dir;
}

void DataDir::SetCorrDir() {
  std::cout << "Enter corrections directory: ";
  std::cin >> fCorrDir;
}

std::string DataDir::GetRawFileName(int run) const {
  return fRawDir + fRawPre + GetCanonicalRun(run) + fRawExt;
}

std::string DataDir::GetODBFileName(int run) const {
  return fODBDir + fODBPre + GetCanonicalRun(run) + fODBExt;
}

std::string DataDir::GetHistFileName(int run) const {
  return fHistDir + fHistPre + GetCanonicalRun(run) + fHistExt;
}

std::string DataDir::GetCorrFileName(int run) const {
  return fCorrDir + fCorrPre + GetCanonicalRun(run) + fCorrExt;
}

