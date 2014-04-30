#ifndef DATADIR_H__
#define DATADIR_H__

#include <string>

class DataDir {

private:
  std::string fRawExt, fODBExt, fHistExt, fCorrExt;
  std::string fRawDir, fODBDir, fHistDir, fCorrDir;
  std::string fRawPre, fODBPre, fHistPre, fCorrPre;

public:
  DataDir();

private:
  static std::string GetCanonicalRun(int run);
  
public:
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
