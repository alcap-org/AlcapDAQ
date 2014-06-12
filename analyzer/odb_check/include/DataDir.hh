/** \addtogroup odbcheck
 * @{
 */

/****************************************
Class: DataDir
Author: John R. Quirk

DataDir keeps track of all directories related to processing
files for the ODBChecker.
****************************************/

#ifndef DATADIR_H__
#define DATADIR_H__

#include <string>

///////////////////////////////////////////////////////////
/// \brief Keeps track of the directories related to processing
/// files for ODBCheck.
///
/// The raw data, ODB files, histogram files, and correction files
/// are all (possible) required at some point. Once those
/// directories have been set here, with just a run number
/// the methods of %DataDir allow one to get a string that
/// represents the path to any of these files.
///////////////////////////////////////////////////////////
class DataDir {

private:

  /// \name Extensions
  /// The extensions associated with each of the file types.
  //@{
  std::string fRawExt, fODBExt, fHistExt, fCorrExt;
  //@}
  /// \name Directories
  /// The directories where the files are located.
  //@{
  std::string fRawDir, fODBDir, fHistDir, fCorrDir;
  //@}
  /// \name Prepensions
  /// Each file type has certain naming conventions.
  /// These are the first few characters of a file name.
  //@{
  std::string fRawPre, fODBPre, fHistPre, fCorrPre;
  //@}

public:
  DataDir();

private:
  /// Takes run number and returns in a format that can be
  /// included in a file name.
  ///
  /// @param[in] run Run number
  /// @return String representing the run number (5 characters)
  static std::string GetCanonicalRun(int run);
  
public:
  /// \name Setters
  /// Set the directory locations. Either pass the path for the directory
  /// or the user will be prompted.
  //@{
  void SetRawDir(const std::string& raw_dir);
  void SetRawDir();
  void SetODBDir(const std::string& odb_dir);
  void SetODBDir();
  void SetHistDir(const std::string& hist_dir);
  void SetHistDir();
  void SetCorrDir(const std::string& corr_dir);
  void SetCorrDir();
  //@}
  
  /// \name Getters
  /// Get path to files corresponding to run number.
  /// @param[in] run Run number
  //@{
  std::string GetRawFileName(int run) const;
  std::string GetODBFileName(int run) const;
  std::string GetHistFileName(int run) const;
  std::string GetCorrFileName(int run) const;
  //@}
};

#endif
/** @}*/
