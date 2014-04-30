#include "DataDir.hh"

#include <string>
#include <iostream>
#include <sstream>

DataDir::DataDir() : fRawExt(".mid"), fODBExt(".odb"), fHistExt(".root"), fCorrExt(".dat"),
		     fRawDir(""), fODBDir(""), fHistDir(""), fCorrDir(""),
		     fRawPre("run"), fODBPre("run"), fHistPre("hist"), fCorrPre("correct") {}

std::string DataDir::GetCanonicalRun(int run) {
  const std::string def("00000");
  std::stringstream num;
  num << run;
  if (run < 0)
    return def;
  else if (run < 10)
    return std::string("0000") + num.str();
  else if (run < 100)
    return std::string("000") + num.str();
  else if (run < 1000)
    return std::string("00") + num.str();
  else if (run < 10000)
    return std::string("0") + num.str();
  else if (run < 100000)
    return num.str();
  return def;
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
  std::stringstream s;
  s << run;
  return fCorrDir + fCorrPre + s.str() + fCorrExt;
}

