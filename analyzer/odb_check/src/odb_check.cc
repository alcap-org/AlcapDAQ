/**
 * \file
 */
#include "ODBCheck.hh"

#include <iostream>
#include <sstream>

#include "TROOT.h"
#include "TPluginManager.h"

extern TROOT* gROOT;

/// Prints the usage message.
void usage() {
  using namespace std;
  cout << "usage: odb_check [-h | --help] [--usage] -d data_dir [-c corr_dir] [-r first_run last_run] [run1 [run2...]]" << endl;
}

/// Prints the usage message and some information
/// on the arguments.
void help() {
  using namespace std;
  usage();
  cout <<
    "odb_check: Read in shapes histogram from alcapana output files," << endl <<
    "           estimate certain values, and then output to corrections file." << endl <<
    " -d      : Followed by place where hist directory and corr directory are." << endl <<
    "           The corrections files are saved to data_dir/corr/ if" << endl <<
    "           -c flag is no specified." << endl <<
    " -c      : The directory where the correction files are stored. If not specified," << endl <<
    "           data_dir/corr/ is assumed." << endl <<
    " -r      : Followed by two numbers indicating a range of runs to process." << endl <<
    " run1... : Any arguments not associated with a flag are assumed to be run" << endl <<
    "           numbers to process." << endl;
}

/// Homemade argument parser.
bool parse_args(int argc, char* argv[], std::vector<int>& runs, std::string& data_dir, std::string& corr_dir) {
  if (argc == 1) {
    help();
    return false;
  }
  for (int i = 1; i < argc; ++i) {
    std::string iarg(argv[i]);
    if (iarg == "-h" || iarg == "--help") {
      help();
      return false;
    } else if (iarg == "--usage") {
      usage();
      return false;
    } else if (iarg == "-d") {
      if (++i != argc) {
	data_dir = std::string(argv[i]);
	if (data_dir[data_dir.size() - 1] != '/')
	  data_dir += '/';
	continue;
      } else {
	std::cout << "ERROR: Nothing following -d flag!" << std::endl;
	return false;
      }
    } else if (iarg == "-c") {
      if (++i != argc) {
	corr_dir = std::string(argv[i]);
	if (corr_dir[corr_dir.size() - 1] != '/')
	  corr_dir += '/';
	continue;
      } else {
	std::cout << "ERROR: Nothing following -c flag!" << std::endl;
      }
    } else if (iarg == "-r") {
      std::stringstream ss;
      int rlow, rhigh;
      if (i + 2 >= argc) {
	std::cout << "ERROR: Not enough arguments following -r flag!" << std::endl;
	return false;
      }
      ss << argv[i + 1] << " " << argv[i + 2];
      ss >> rlow >> rhigh;
      i += 2;
      if (rlow == 0 || rhigh == 0) {
	std::cout <<
	  "ERROR: Either 0 or a non-numerical value was passed as a run number." << std::endl <<
	  "       Neither can be processed." << std::endl;
	return false;
      } else if (rlow >= rhigh) {
	std::cout << "ERROR: The first run following the -r option must be smaller than the second!" << std::endl;
	return false;
      } else if (rlow < 0) {
	std::cout << "ERROR: Cannot process negative run number!" << std::endl;
	return false;
      }
      for (int r = rlow; r <= rhigh; ++r)
	runs.push_back(r);
    } else {
      std::stringstream ss;
      int r;
      ss << iarg;
      ss >> r;
      if (r == 0) {
	std::cout <<
	  "ERROR: Either 0 or a non-numerical value was passed as a run number." << std::endl <<
	  "       Neither can be processed." << std::endl;
	return false;
      } else if (r < 0) {
	std::cout << "ERROR: Cannot process negative run number!" << std::endl;
	return false;
      }
      runs.push_back(r);
    }
  }
  if (runs.size() == 0) {
    std::cout << "ERROR: No runs to process!" << std::endl;
    return false;
  } else if (data_dir.size() == 0) {
    std::cout << "ERROR: No data directory indicated!" << std::endl;
    return false;
  }
  if (corr_dir.size() == 0)
    corr_dir = data_dir + "corr/";
  return true;
}

/// \brief The program loops through all runs passed to it and produces correction
/// filees and saves them to a target directory.
///
/// odb_check looks at histograms produced by alcapana and
/// the ODB files saved during the run and finds inconsistencies
/// and attempts to correct them. See the WireMap for
/// and indication of what gets read in from the ODB.
/// See the DataDir for an idea of what files are read
/// in and produced. See PulseEstimate to see what
/// how the estimates are made. And finally see ODBCheck
/// to get a feel for how this all comes together.
int main(int argc, char* argv[]) {
  // First and foremost, check that we can read in files
  if(gROOT->GetPluginManager()->FindHandler("TVirtualStreamerInfo") == NULL)
    gROOT->GetPluginManager()->AddHandler("TVirtualStreamerInfo","*","TStreamerInfo","RIO","TStreamerInfo()");

  // Next, process command line arguments
  std::vector<int> runs;
  std::string data_dir;
  std::string corr_dir;

  if (!parse_args(argc, argv, runs, data_dir, corr_dir))
    return 1;

  ODBCheck x;
  std::string raw_dir = data_dir + "raw/";
  std::string odb_dir = data_dir + "odb/";
  std::string hist_dir = data_dir + "hist/";
  x.SetDirs(raw_dir, odb_dir, hist_dir, corr_dir);
  for (int r = 0; r < (int)runs.size(); ++r) {
    std::cout << "Checking run " << runs[r] << "..." << std::endl;
    x.Check(runs[r]);
  }
}
