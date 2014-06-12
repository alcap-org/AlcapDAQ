#ifndef ODBCHECK_H__
#define ODBCHECK_H__

#include "WireMap.hh"
#include "DataDir.hh"
#include "PulseEstimate.hh"

#include <string>
#include <fstream>

////////////////////////////////////////////////////////////////////////
/// \brief Provides corrected ODB files for alcapana production.
///
/// ODBCheck looks simply for inconsistencies between the alcapana
/// data quality output histograms and the original ODB files,
/// then outputs a corrected ODB file for the run to be loaded on the
/// next alcapana production.
////////////////////////////////////////////////////////////////////////
class ODBCheck {
private:
  int fRun; ///< Run number
  WireMap fODB; ///< Original ODB
  WireMap fCorrections; ///< Corrected ODB; produced in the course of running Check()
  DataDir fDataDirs; ///< Location of necessary files
  PulseEstimate fEstimate; ///< Estimates based off of data quality histograms for corrected ODB.
  std::ofstream fCorrectionsFile; ///< The corrected ODB file

public:
  ODBCheck();
  ~ODBCheck();

  /**
   * Set the directories to look for the different files we might need.
   * The information is stored in a DataDir object.
   * 
   * @param[in] raw Raw data directory (MIDAS files)
   * @param[in] odb ODB data directory (ODB files)
   * @param[in] hist Histograms directory (hist?????.root files)
   * @param[in] corr Corrections directory (correction*.dat files for rootana)
   */
  void SetDirs(const std::string& raw, const std::string& odb, const std::string& hist, const std::string& corr);

private:
  /**
   * Calls the function to output the corrections file.
   * All the corrections should be done by this point.
   */
  void OutputCorrections();

public:
  /**
   * Correct a run.
   *
   * Using the histogram output from alcapana, make estimates as to the correct
   * values of some ODB keys.  Outputs a corrections file when
   * done.
   *
   * @param[in] run Run number to process.
   */
  void Check(int run);
};

#endif
