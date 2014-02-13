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

namespace ODB {
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
  
  class PulseEstimate {
  private:
    int fPedestal;
    int fPolarity;
  public:
    PulseEstimate() : fPedestal(0), fPolarity(0) {
    }
    void Estimate(TH2* pulses) {
      TProfile* x = pulses->ProfileX();
      TProfile* y = pulses->ProfileY();
      
    }
    void Estimate2(TH2* pulses) {
      std::map<int,int> sampcnt;
      TProfile* prof;
      prof = pulses->ProfileX();
      prof->Rebin(4);
      int bc;
      int min, max;
      min = max = (int)prof->GetBinContent(1);
      for (int i = 1; i <= prof->GetNbinsX(); ++i) {
	bc = (int)prof->GetBinContent(i);
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
      fPedestal = j->first;
      
      if (max - fPedestal > fPedestal - min)
	fPolarity = 1;
      else
	fPolarity = -1;
    }
    int GetPedestal() const {
      return fPedestal;
    }
    int GetPolarity() const {
      return fPolarity;
    }
  };
  
  class ODBDraw {
  private:
    PolaritySign fDPol;
    Pedestals fDPed;
    TH2* fPulses;
    int fPolarity_ODB;
  public:
    ODBDraw() : fDPol(), fDPed(), fPulses(NULL),
		fPolarity_ODB(1) {
    }
  public:
    void Set(TH2* pulses, const WireMap& odb, int i, const PulseEstimate& est) {
      fPulses = pulses;
      double x1 = fPulses->GetXaxis()->GetBinLowEdge(1);
      double y1 = fPulses->GetYaxis()->GetBinLowEdge(1);
      double x2 = fPulses->GetXaxis()->GetBinLowEdge(fPulses->GetNbinsX()+1);
      double y2 = fPulses->GetYaxis()->GetBinLowEdge(fPulses->GetNbinsY()+1);
      double dx = x2-x1;
      double dy = y2-y1;
      fPolarity_ODB = odb.polarity[i];
      fDPol.Set(x2-0.1*dx, y2-0.1*dy, x2, y2);
      fDPed.Set(x1, x2, (double)odb.pedestal[i], (double)est.GetPedestal());
    }
    void Draw() {
      fPulses->Draw("COLZ");
      fDPol.Draw(fPolarity_ODB);
      fDPed.Draw();
    }
  };
  
  class DataDir {
  private:
    std::string fRawExt;
    std::string fODBExt;
    std::string fHistExt;
    std::string fRawDir;
    std::string fODBDir; 
    std::string fHistDir;
    std::string fRawPre;
    std::string fODBPre;
    std::string fHistPre;
  public:
    DataDir() : fRawExt(".mid"), fODBExt(".odb"), fHistExt(".root"),
		fRawDir(""), fODBDir(""), fHistDir(""),
		fRawPre("run"), fODBPre("run"), fHistPre("hist") {
    }
  private:
    std::string GetCanonicalRun(int run) {
      const std::string def("00000");
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
  public:
    void SetRawDir(const std::string& raw_dir) {
      fRawDir = raw_dir;
    }
    void SetODBDir(const std::string& odb_dir) {
      fODBDir = odb_dir;
    }
    void SetHistDir(const std::string& hist_dir) {
      fHistDir = hist_dir;
    }
    std::string GetRawFileName(int run) {
      return fRawDir + fRawPre + GetCanonicalRun(run) + fRawExt;
    }
    std::string GetODBFileName(int run) {
      return fODBDir + fODBPre + GetCanonicalRun(run) + fODBExt;
    }
    std::string GetHistFileName(int run) {
      return fHistDir + fHistPre + GetCanonicalRun(run) + fHistExt;
    }
  };
  
  
  class ODBCheck {
  private:
    int fRun;
    TCanvas fCanvas;
    ODBDraw fODBDraw;
    WireMap fODB;
    DataDir fDataDirs;
    bool fLoadODBFile;
    PulseEstimate fEstimate;
  public:
    ODBCheck() : fRun(0), fCanvas("c","Check"), fODBDraw(),
		 fODB(), fDataDirs(), fLoadODBFile(true),
		 fEstimate() {
    }
  private:
    void LoadODBValues() {
      static const std::string header("[/Analyzer/WireMap]");
      static const std::string bankname_key("BankName");
      static const std::string detname_key("DetectorName");
      static const std::string enabled_key("Enabled");
      static const std::string polarity_key("TriggerPolarity");
      static const std::string pedestal_key("Pedestal");
      static const std::string timeshift_key("TimeShift");
      static char tmp[256];
      std::string ifname;
      if (fLoadODBFile)
	ifname = fDataDirs.GetODBFileName(fRun);
      else
	ifname = fDataDirs.GetRawFileName(fRun);
      // Clear WireMap
      fODB.n = 0;
      fODB.bankname.clear();
      fODB.detname.clear();
      fODB.pedestal.clear();
      fODB.polarity.clear();
      // Look through MIDAS file for appropriate banks
      std::cout << ifname << std::endl;
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
	    }
	    if (fODB.n != 0 &&
		fODB.bankname.size() == fODB.n &&
		fODB.detname.size() == fODB.n &&
		fODB.pedestal.size() == fODB.n &&
		fODB.polarity.size() == fODB.n)
	      return;
	  }
	}
      }
    }
  public:
    void Check(int run) {
      std::string opt;
      fDataDirs.SetRawDir("/home/jrquirk/alcap/data/raw/");
      fDataDirs.SetODBDir("/home/jrquirk/alcap/data/odb/");
      fDataDirs.SetHistDir("/home/jrquirk/alcap/data/hist/");
      if (fRun != run) {
	fRun = run;
	LoadODBValues();
      }
      std::cout << fODB.n << std::endl;
      TFile hist_file(fDataDirs.GetHistFileName(fRun).c_str(), "READ");
      for (unsigned int i = 0; i < fODB.n; ++i) {
	TH2* shapes;
	hist_file.GetObject(("h" + fODB.bankname[i] + "_Shapes").c_str(), shapes);
	if (shapes) {
	  shapes->SetStats(0);
	  fEstimate.Estimate(shapes);
	  fODBDraw.Set(shapes, fODB, i, fEstimate);
	  fCanvas.cd();
	  fODBDraw.Draw();
	  fCanvas.Update();
	  std::cout << "[q]uit, [a]djust: ";
	  std::cin >> opt;
	  if (opt == "q") {
	    hist_file.Close();
	    return;
	  }
	}
      } 
    }
    void LoadODBFromODBFile() {
      fLoadODBFile = true;
    }
    void LoadODBFromDataFile() {
      fLoadODBFile = false;
    }
  };
}

void odb_check(int run) {
  using namespace ODB;
  ODBCheck x;
  x.Check(run);
}
