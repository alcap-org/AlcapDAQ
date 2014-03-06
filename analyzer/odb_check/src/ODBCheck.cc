#include "ODBCheck.hh"

#include "TFile.h"
#include "TH2.h"

#include <string>
#include <fstream>
#include <iostream>
#include <sstream>

ODBCheck::ODBCheck() : fRun(0), fODB(), fDataDirs(),
		       fLoadODBFile(true), fEstimate(), fCorrectionsFile(), fMonitorPlotsFile(NULL) {}

ODBCheck::~ODBCheck() {
  fMonitorPlotsFile->Write();
  fMonitorPlotsFile->Close();
  fMonitorPlotsFile = NULL;
}

void ODBCheck::SetDirs(const std::string& raw, const std::string& odb, const std::string& hist, const std::string& corr) {
  fDataDirs.SetRawDir(raw);
  fDataDirs.SetODBDir(odb);
  fDataDirs.SetHistDir(hist);
  fDataDirs.SetCorrDir(corr);
}

void ODBCheck::SetDirs() {
  fDataDirs.SetRawDir();
  fDataDirs.SetODBDir();
  fDataDirs.SetHistDir();
  fDataDirs.SetCorrDir();
}

void ODBCheck::LoadODBValues() {
  static std::string header("");
  static std::string bankname_key("BankName");
  static std::string detname_key("DetectorName");
  static std::string enabled_key("Enabled");
  static std::string polarity_key("TriggerPolarity");
  static std::string pedestal_key("Pedestal");
  static std::string offset_key("TimeShift");
  static char tmp[256];
  std::string ifname;
  if (fLoadODBFile) {
    ifname = fDataDirs.GetODBFileName(fRun);
    header = "[/Analyzer/WireMap]";
  } else {
    ifname = fDataDirs.GetRawFileName(fRun);
    header = "[Analyzer/WireMap]";
  }

  // Clear WireMap
  fODB.n = 0;
  fODB.bankname.clear();
  fODB.detname.clear();
  fODB.pedestal.clear();
  fODB.polarity.clear();
  fODB.offset.clear();
  
  // Look through MIDAS or ODB file for appropriate banks
  std::ifstream f(ifname.c_str());
  std::string str;
  std::stringstream ss;
  int x;
  while (f.good()) {
    f >> str;
    if (str == header) {
      while (f.good()) {
	f >> str;
	if (str == bankname_key) {
	  f.getline(tmp, 256);
	  str = tmp;
	  int pos[2];
	  pos[0] = str.find('[') + 1;
	  pos[1] = str.find(']');
	  str = str.substr(pos[0], pos[1] - pos[0]);
	  ss << str;
	  ss >> fODB.n;
	  for (unsigned int i = 0; i < fODB.n; ++i) {
	    f >> str;
	    f >> str;
	    fODB.bankname.push_back(str);
	  }
	} else if (str == detname_key) {
	  f.getline(tmp, 256);
	  for (unsigned int i = 0; i < fODB.n; ++i) {
	    f >> str;
	    f >> str;
	    fODB.detname.push_back(str);
	  }
	} else if (str == pedestal_key) {
	  f.getline(tmp, 256);
	  for (unsigned int i = 0; i < fODB.n; ++i) {
	    f >> str;
	    f >> x;
	    fODB.pedestal.push_back(x);
	  }
	} else if (str == polarity_key) {
	  f.getline(tmp, 256);
	  for (unsigned int i = 0; i < fODB.n; ++i) {
	    f >> str;
	    f >> x;
	    fODB.polarity.push_back(x);
	  }
	} else if (str == offset_key) {
	  f.getline(tmp, 256);
	  for (unsigned int i = 0; i < fODB.n; ++i) {
	    f >> str;
	    f >> x;
	    fODB.offset.push_back(x);
	  }
	}
	if (fODB.n != 0 &&
	    fODB.bankname.size() == fODB.n &&
	    fODB.detname.size() == fODB.n &&
	    fODB.pedestal.size() == fODB.n &&
	    fODB.polarity.size() == fODB.n &&
	    fODB.offset.size() == fODB.n)
	  return;
      }
    }
  }
  std::cout << "Warning: Didn't find everything in the ODB!" << std::endl;
}

void ODBCheck::InitiateCorrectionsFile() {
  if (fCorrectionsFile.is_open())
    fCorrectionsFile.close();
  fCorrectionsFile.open(fDataDirs.GetCorrFileName(fRun).c_str());
  if (fCorrectionsFile.is_open())
    fCorrectionsFile <<
      "Bank name | Field | New value" << std::endl <<
      "--------------------------------" << std::endl;
  else
    std::cout <<
      "ODBCheck ERROR: Cannot open correction file (" <<
      fDataDirs.GetCorrFileName(fRun) << ")!" << std::endl;
}

void ODBCheck::OutputCorrectionsIfNeeded(unsigned int i, TH2* s, TH1* t, int run_number) {
  if (fCorrectionsFile.is_open()) {
    if (s && s->GetEntries()) {
      if (i >= fODB.pedestal.size() || fODB.pedestal[i] != fEstimate.GetPedestal()) {
	fCorrectionsFile << fODB.bankname[i] << "\tPedestal\t" << fEstimate.GetPedestal() << std::endl;
	FillMonitorPlots(fODB.bankname[i], "Pedestal", run_number, fEstimate.GetPedestal());
      }
      else {
	FillMonitorPlots(fODB.bankname[i], "Pedestal", run_number, fODB.pedestal[i]);
      }

      if (i >= fODB.pedestal.size() || fODB.polarity[i] != fEstimate.GetPolarity()) {
	fCorrectionsFile << fODB.bankname[i] << "\tPolarity\t" << fEstimate.GetPolarity() << std::endl;
	FillMonitorPlots(fODB.bankname[i], "Polarity", run_number, fEstimate.GetPolarity());
      }
      else {
	FillMonitorPlots(fODB.bankname[i], "Polarity", run_number, fODB.polarity[i]);
      }
    }

    if (t && t->GetEntries()) {
      if (i >= fODB.offset.size() || fODB.offset[i] != fEstimate.GetOffset()) {
	fCorrectionsFile << fODB.bankname[i] << "\tTimeShift\t" << fEstimate.GetOffset() << std::endl;
	FillMonitorPlots(fODB.bankname[i], "TimeShift", run_number, fEstimate.GetOffset());
      }
      else {
	FillMonitorPlots(fODB.bankname[i], "TimeShift", run_number, fODB.offset[i]);
      }
    }
  }
}

void ODBCheck::Check(int run) {
  TFile hist_file(fDataDirs.GetHistFileName(run).c_str(), "READ");

  if (fMonitorPlotsFile == NULL)
    fMonitorPlotsFile = new TFile("MonitorPlots.root", "RECREATE"); 

  if (!hist_file.IsOpen())
    return;
  if (fRun != run) {
    fRun = run;
    LoadODBValues();
    InitiateCorrectionsFile();
  }

  // The value we're looping over is the bank names in the ODB.
  // Some corresponding detector names are not real,
  // such as "blank". We hope these histograms do not exist so
  // that we don't process them unnecessarily.
  for (unsigned int i = 0; i < fODB.n; ++i) {
    TH2* shapes;
    TH1* timing;
    hist_file.GetObject(("h" + fODB.bankname[i] + "_Shapes").c_str(), shapes);
    hist_file.GetObject(("MuSC_TimingCorrelations/hMuSC_" + fODB.detname[i] + "_Timediff").c_str(), timing);
    if (shapes && shapes->GetEntries())
      fEstimate.Estimate(shapes);
    if (timing && timing->GetEntries())
      fEstimate.Estimate(timing);
    OutputCorrectionsIfNeeded(i, shapes, timing, run);
    delete shapes;
    delete timing;
    shapes = NULL;
    timing = NULL;
  }
  hist_file.Close();
}

void ODBCheck::LoadODBFromODBFile() {
  fLoadODBFile = true;
}

void ODBCheck::LoadODBFromDataFile() {
  fLoadODBFile = false;
}

void ODBCheck::FillMonitorPlots(std::string bank_name, std::string fieldname, int run_number, double value) {

  std::string histname = bank_name + "_" + fieldname;
  std::map<std::string, TH1F*>& bank_to_hist_map = fHistogramMap[fieldname]; // want a reference to the actual map so that all the runs are added to the same histogram

  // Try and find the histogram for this bank and if it's not there, then create it
  if (bank_to_hist_map.find(bank_name) == bank_to_hist_map.end()) {
    fMonitorPlotsFile->cd();
    bank_to_hist_map[bank_name] = new TH1F(histname.c_str(), histname.c_str(), 100, run_number, run_number+100);
    bank_to_hist_map[bank_name]->SetBit(TH1::kCanRebin);
    bank_to_hist_map[bank_name]->GetXaxis()->SetTitle("Run Number");
    bank_to_hist_map[bank_name]->GetYaxis()->SetTitle(fieldname.c_str());
  }

  // Fill the histogram
  bank_to_hist_map[bank_name]->Fill(run_number, value);
}
