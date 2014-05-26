/****************************************
Class: DataDir
Author: John R. Quirk

DataDir keeps track of all directories related to processing
files for the ODBChecker.
****************************************/

#ifndef DATADIR_H__
#define DATADIR_H__

#include <string>

class DataDir {

private:
  // The header and footer of file names are *Pre and *Ext respectively
  // We keep track of the raw dat, the ODB files, the histogram files,
  // And the correction files (which will also be ODB files)
  std::string fRawExt, fODBExt, fHistExt, fCorrExt;
  std::string fRawDir, fODBDir, fHistDir, fCorrDir;
  std::string fRawPre, fODBPre, fHistPre, fCorrPre;

public:
  DataDir();

private:
  // Returns string representing the run number (5 characters)
  static std::string GetCanonicalRun(int run);
  
public:
  // The methods with arguments silently set
  // Those without arguments prompt
  void SetRawDir(const std::string& raw_dir);
  void SetRawDir();
  void SetODBDir(const std::string& odb_dir);
  void SetODBDir();
  void SetHistDir(const std::string& hist_dir);
  void SetHistDir();
  void SetCorrDir(const std::string& corr_dir);
  void SetCorrDir();
  
  std::string GetRawFileName(int run) const;
  std::string GetODBFileName(int run) const;
  std::string GetHistFileName(int run) const;
  std::string GetCorrFileName(int run) const;
};

#endif
