#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>

#include "TROOT.h"
#include "TClass.h"
#include "TH1.h"
#include "TFile.h"
#include "TCollection.h"
#include "TKey.h"

using std::cout;
using std::endl;
using std::ifstream;
using std::vector;
using std::string;
typedef std::stringstream sstream;

// Wildcard matching in pat to a str.
bool match(const char* pat, const char* str);
// Get cannonical filename from run number.
// The format is hist#####.root.
string filename(int run);
// Get fully qualified histogram names from in a ROOT file.
vector<string> gettdirhists(const TDirectoryFile* dir);
// Get list of histograms we want to average,
// each on its own line, in a fully qualified patch,
// where wildcards * and ? are allowed.
vector<string> gettextfilehists(const string& fname);
// Using a ROOT file and a list of histogram, determine
// which histograms in the ROOT file, assumed to be in all
// later ROOT files, match the requested histograms listed
// in the file named in requested_hist_list.
vector<string> histlist(const string& requested_hist_list, const TFile* f);
// Average histogram h into histogram havg.
void average_histogram(TH1*& havg, TH1* h);


bool match(const char* pat, const char* str) {
  if (*pat == '\0' && *str == '\0')
    return true;
  if (*pat == '*' && *(pat+1) != '\0' && *str == '\0')
    return false;
  if (*pat == '?' || *pat == *str)
    return match(pat+1, str+1);
  if (*pat == '*')
    return match(pat+1, str) || match(pat, str+1);
  return false;
}

string filename(int r) {
  char fname[16];
  sprintf(fname, "hist%05d.root", r);
  return string(fname);
}

vector<string> gettdirhists(const TDirectoryFile* dir) {
  vector<string> v;
  TIter next(dir->GetListOfKeys());
  TKey* key;
  while ((key = (TKey*)next())) {
    TClass* cl = gROOT->GetClass(key->GetClassName());
    if (cl->InheritsFrom("TDirectoryFile")) {
      vector<string> v2 = gettdirhists((TDirectoryFile*)key->ReadObj());
      string dirname(key->GetName());
      dirname += '/';
      for (int i = 0; i < v2.size(); ++i)
	v.push_back(dirname+v2[i]);
    } else if (cl->InheritsFrom("TH1")) {
      v.push_back(key->GetName());
    } else {
      cout << "ERROR: Unhandled class " << key->GetClassName() << endl;
    }
  }
  return v;
}

vector<string> gettextfilehists(const string& fname) {
  vector<string> v;
  ifstream hfile(fname.c_str());
  while (!hfile.eof()) {
    char hist[256];
    if (hfile.getline(hist, 255))
      v.push_back(string(hist));
  }
  return v;
}

vector<string> histlist(const string& hfname, const TFile* f) {
  vector<string> v;
  const vector<string> reqst_hists = gettextfilehists(hfname);
  const vector<string> avail_hists = gettdirhists((TDirectoryFile*)f);
  for (int i = 0; i < avail_hists.size(); ++i)
    for (int j = 0; j < reqst_hists.size(); ++j)
      if (match(reqst_hists[j].c_str(), avail_hists[i].c_str()))
	v.push_back(avail_hists[i]);
  return v;
}  

void average_histogram(TH1*& havg, TH1* h) {
  if (!h) {
    std::cout << "NULL histogram..." << std::endl;
    return;
  }
  if (!havg) {
    string name(h->GetName());
    name += "_Avg";
    havg = (TH1*)h->Clone(name.c_str());
    havg->Sumw2();
    havg->SetBit(TH1::kIsAverage);
    havg->SetDirectory(0);
  } else {
    h->Sumw2();
    h->SetBit(TH1::kIsAverage);
    havg->Add(h);
  }
}

// Plot averages of normalized histograms
// Also plot run vs max or centroid for some
void trends(string sfHists, string sfRuns) {
  ifstream fRuns(sfRuns.c_str());
  vector<int> vRuns;

  while (!fRuns.eof()) {
    char line[256];
    if (fRuns.getline(line, 255)) {
      sstream sline(line);
      int r1 = -1, r2 = -1;
      sline >> r1 >> r2;
      if (r1 > 0) {
	if (r2 > 0)
	  for (int r = r1; r <= r2; ++r)
	    vRuns.push_back(r);
	else
	  vRuns.push_back(r1);
      }
    }
  }
  if (vRuns.size() == 0) return;

  TFile* tmp_file = new TFile(filename(vRuns.front()).c_str(), "READ");
  vector<string> vHistNames = histlist(sfHists, tmp_file);
  tmp_file->Close();
  delete tmp_file;
  vector<TH1*> vHists(vHistNames.size(), NULL);
  

  for (int ir = 0; ir < vRuns.size(); ++ir) {
    TFile ifile(filename(vRuns[ir]).c_str(), "READ");
    for (int ih = 0; ih < vHists.size(); ++ih) {
      TH1* h; ifile.GetObject(vHistNames[ih].c_str(), h);
      average_histogram(vHists[ih], h);
    }
    ifile.Close();
  }
  TFile ofile("trend.root", "RECREATE");
  for (int ih = 0; ih < vHists.size(); ++ih)
    vHists[ih]->SetDirectory(&ofile);
  ofile.Write();
  ofile.Close();
}
