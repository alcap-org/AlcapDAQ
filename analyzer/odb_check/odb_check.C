#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <map>

#include "TFile.h"
#include "TCanvas.h"
#include "TH1.h"
#include "TH2.h"
#include "TProfile.h"
#include "TLine.h"

struct WireMap {
  unsigned int n;
  std::vector<std::string> bankname;
  std::vector<std::string> detname;
  std::vector<int> pedestal;
  std::vector<int> polarity;
};

class PlusSign {
private:
  TLine lHor;
  TLine lVer;
public:
  PlusSign() {
    lHor.SetLineColor(kBlack);
    lVer.SetLineColor(kBlack);
    lHor.SetLineStyle(1);
    lVer.SetLineStyle(1);
  }
  void Set(double x1, double y1, double x2, double y2) {
    lHor.SetX1(x1);
    lHor.SetX2(x2);
    lHor.SetY1((y2+y1)/2.);
    lHor.SetY2((y2+y1)/2.);
    lVer.SetX1((x1+x2)/2.);
    lVer.SetX2((x1+x2)/2.);
    lVer.SetY1(y1);
    lVer.SetY2(y2);
  }
  void Draw() {
    lHor.Draw("SAME");
    lVer.Draw("SAME");
  }
};

class MinusSign {
private:
  TLine lHor;
public:
  MinusSign() {
    lHor.SetLineColor(kRed);
    lHor.SetLineStyle(1);
  }
  void Set(double x1, double y1, double x2) {
    lHor.SetX1(x1);
    lHor.SetY1(y1);
    lHor.SetX2(x2);
    lHor.SetY2(y1);
  }
  void Draw() {
    lHor.Draw("SAME");
  }
};

class PolaritySign {
private:
  PlusSign p;
  MinusSign m;
public:
  void Set(double x1, double y1, double x2, double y2) {
    p.Set(x1, y1, x2, y2);
    m.Set(x1, y1, x2);
  }
  void Draw(int pol) {
    if (pol >= 0)
      p.Draw();
    else
      m.Draw();
  }
};

class Pedestals {
private:
  TLine odb;
  TLine guess;
public:
  Pedestals() {
    odb.SetLineColor(kBlack);
    guess.SetLineColor(kBlack);
    odb.SetLineStyle(1);
    guess.SetLineStyle(2);
  }
  void Set(double x1, double x2, double p_file, double p_guess) {
    odb.SetX1(x1);
    odb.SetY1(p_file);
    odb.SetX2(x2);
    odb.SetY2(p_file);
    guess.SetX1(x1);
    guess.SetY1(p_guess);
    guess.SetX2(x2);
    guess.SetY2(p_guess);
  }
  void Draw() {
    odb.Draw("SAME");
    guess.Draw("SAME");
  }
};

class ODBDraw {
private:
  PolaritySign fDPol;
  Pedestals fDPed;
  TH2* fPulses;
  TProfile* fProfile;
  int fPol_File;
  int fPol_Guess;
  int fPed_File;
  int fPed_Guess;
public:
  ODBDraw() {
    fPulses = NULL;
    fProfile = NULL;
  }
private:
  void Guess() {
    std::map<int,int> sampcnt;
    if (fProfile) {
      delete fProfile;
      fProfile = NULL;
    }
    fProfile = fPulses->ProfileX();
    fProfile->Rebin(4);
    int bc;
    int min, max;
    min = max = (int)fProfile->GetBinContent(1);
    for (int i = 1; i <= fProfile->GetNbinsX(); ++i) {
      bc = (int)fProfile->GetBinContent(i);
      if (bc == 0)
	continue;
      if (bc > max)
	max = bc;
      else if (bc < min)
	min = bc;
      if (sampcnt.count(bc) == 0)
	sampcnt[bc] = 0;
      sampcnt[bc]++;
    }
    std::map<int,int>::iterator i, j;
    i = j = sampcnt.begin();
    for (; i != sampcnt.end(); ++i)
      if (i->second > j->second)
	j = i;
    fPed_Guess = j->first;
    
    if (max - fPed_Guess > fPed_Guess - min)
      fPol_Guess = 1;
    else
      fPol_Guess = -1;
  }
public:
  void Set(TH2* pulses, const WireMap& odb, int i) {
    fPulses = pulses;
    double x1 = fPulses->GetXaxis()->GetBinLowEdge(1);
    double y1 = fPulses->GetYaxis()->GetBinLowEdge(1);
    double x2 = fPulses->GetXaxis()->GetBinLowEdge(fPulses->GetNbinsX()+1);
    double y2 = fPulses->GetYaxis()->GetBinLowEdge(fPulses->GetNbinsY()+1);
    double dx = x2-x1;
    double dy = y2-y1;
    Guess();
    fPed_File = odb.pedestal[i];
    fPol_File = odb.polarity[i];
    fDPol.Set(x2-0.1*dx, y2-0.1*dy, x2, y2);
    fDPed.Set(x1, x2, (double)fPed_File, (double)fPed_Guess);
  }
  void Draw() {
    fPulses->Draw("COLZ");
    fDPol.Draw(fPol_File);
    fDPed.Draw();
  }
  void Print() {
    std::cout <<
      "Polarity (ODB): " << fPol_File << std::endl <<
      "Polarity (Guess): " << fPol_Guess << std::endl <<
      "Pedestal (ODB): " << fPed_File << std::endl <<
      "Pedestal (Guess): " << fPed_Guess << std::endl;
  }
};
  

std::string getCanonicalRun(int run);
void getODBValues(std::string& fname, WireMap* wm);

void odb_check(int run) {
  TCanvas c("c", "Check");
  ODBDraw odb_draw;;
  WireMap odb;
  // Make these variable in the future. A struct maybe?
  bool odb_load = false;     // Load ODB from MIDAS file if true, otherwise from ODB file in odb_dir
  const std::string raw_ext(".mid");
  const std::string odb_ext(".odb");
  const std::string hist_ext(".root");
  const std::string raw_dir("/home/jrquirk/alcap/data/raw/");
  const std::string odb_dir("/home/jrquirk/alcap/data/odb/"); 
  const std::string hist_dir("/home/jrquirk/alcap/data/hist/");
  const std::string raw_pre("run");
  const std::string odb_pre("run");
  const std::string hist_pre("hist");
   std::string run_name = getCanonicalRun(run);
  std::string raw_fname(raw_dir + raw_pre + run_name + raw_ext);
  std::string odb_fname(odb_dir + odb_pre + run_name + odb_ext);
  std::string hist_fname(hist_dir + hist_pre + run_name + hist_ext);

  // Values to check: Polarity, Pedestal
  std::string opt;
  if (odb_load)
    getODBValues(raw_fname, &odb);
  else
    getODBValues(odb_fname, &odb);

  TFile hist_file(hist_fname.c_str(), "READ");
  for (unsigned int i = 0; i < odb.n; ++i) {
    TH2* shapes;
    hist_file.GetObject(("h" + odb.bankname[i] + "_Shapes").c_str(), shapes);
    if (shapes) {
      shapes->SetStats(0);
      odb_draw.Set(shapes, odb, i);
      c.cd();
      odb_draw.Draw();
      c.Update();
      odb_draw.Print();
      std::cout << "[q]uit, [a]djust: ";
      std::cin >> opt;
      if (opt == "q") {
	hist_file.Close();
	return;
      }
    }
  }
}

std::string getCanonicalRun(int run) {
  static const std::string def("00000");
  stringstream num;
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

void getODBValues(std::string& fname, WireMap* wm) {
  static const std::string header("[/Analyzer/WireMap]");
  static const std::string bankname_key("BankName");
  static const std::string detname_key("DetectorName");
  static const std::string enabled_key("Enabled");
  static const std::string polarity_key("TriggerPolarity");
  static const std::string pedestal_key("Pedestal");
  static const std::string timeshift_key("TimeShift");
  static char tmp[256];
  // Clear WireMap
  wm->n = 0;
  wm->bankname.clear();
  wm->detname.clear();
  wm->pedestal.clear();
  wm->polarity.clear();
  // Look through MIDAS file for appropriate banks
  std::ifstream f(fname.c_str());
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
	  ss >> wm->n;
	  for (unsigned int i = 0; i < wm->n; ++i) {
	    f >> str;
	    f >> str;
	    wm->bankname.push_back(str);
	  }
	} else if (str == detname_key) {
	  f.getline(tmp, 256);
	  for (unsigned int i = 0; i < wm->n; ++i) {
	    f >> str;
	    f >> str;
	    wm->detname.push_back(str);
	  }
	} else if (str == pedestal_key) {
	  f.getline(tmp, 256);
	  for (unsigned int i = 0; i < wm->n; ++i) {
	    f >> str;
	    f >> x;
	    wm->pedestal.push_back(x);
	  }
	} else if (str == polarity_key) {
	  f.getline(tmp, 256);
	  for (unsigned int i = 0; i < wm->n; ++i) {
	    f >> str;
	    f >> x;
	    wm->polarity.push_back(x);
	  }
	}
	if (wm->n != 0 &&
	    wm->bankname.size() == wm->n &&
	    wm->detname.size() == wm->n &&
	    wm->pedestal.size() == wm->n &&
	    wm->polarity.size() == wm->n)
	  return;
      }
    }
  }
}
