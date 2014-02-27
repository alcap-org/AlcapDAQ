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
  /*** Structure to contain values read in from ODB. Threshold not implemented yet. ***/
  struct WireMap {
    unsigned int n;
    std::vector<std::string> bankname;
    std::vector<std::string> detname;
    std::vector<int> pedestal;
    std::vector<int> polarity;
    std::vector<int> threshold;
  };
  
  /*** Draws plus sign with given boundaries ***/
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
    /* Set the boundaries of the plus sign (left, bottom, right, top) */
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
    /* Draw plus sign on current canvas */
    void Draw() {
      lHor.Draw("SAME");
      lVer.Draw("SAME");
    }
  };
  
  /*** Draws minus sign with specified boundaries ***/
  class MinusSign {
  private:
    TLine lHor;
  public:
    MinusSign() {
      lHor.SetLineColor(kRed);
      lHor.SetLineStyle(1);
    }
    /* Set the position of the minus sign (left limit, vertical position, right limit) */
    void Set(double x1, double y1, double x2) {
      lHor.SetX1(x1);
      lHor.SetY1(y1);
      lHor.SetX2(x2);
      lHor.SetY2(y1);
    }
    /* Draw minus sign on current canvas */
    void Draw() {
      lHor.Draw("SAME");
    }
  };
 
  /*** Draws plus/minus sign with specified boundaries. Sign depends on polarity argument passed to Draw ***/
  class PolaritySign {
  private:
    PlusSign p;
    MinusSign m;
  public:
    /* Set the boundaries box of the plus/minus signs to draw */
    void Set(double x1, double y1, double x2, double y2) {
      p.Set(x1, y1, x2, y2);
      m.Set(x1, y1, x2);
    }
    /* Draw a plus sign if the polarity is positive, minus sign if the polairty is negative. */
    void Draw(int pol) {
      if (pol >= 0)
	p.Draw();
      else
	m.Draw();
    }
  };
  
  /*** Draws two horizontal lines at the ODB value and at the estimated value ***/
  class Pedestals {
  private:
    TLine odb;
    TLine guess;
  public:
    /* Both lines are black. ODB value is solid horizontal line, estimate is dashed */
    Pedestals() {
      odb.SetLineColor(kBlack);
      guess.SetLineColor(kBlack);
      odb.SetLineStyle(1);
      guess.SetLineStyle(2);
    }
    /* Set the horizontal limits for lines, and then the pedestal from the ODB (p_file) and the pedestal estimate (p_guess) */ 
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
    /* Draw the two lines on the current canvas */
    void Draw() {
      odb.Draw("SAME");
      guess.Draw("SAME");
    }
  };
  
  /*** This class will someday draw the ODB threshold value, and then the estimated threshold value ***/
  class Threshold {
  private:
    TLine th;
  public:
    Threshold() {
      th.SetLineColor(kRed);
      th.SetLineStyle(kBlack);
    }
    void Set(double x1, double y1, double x2, double y2) {
      th.SetX1(x1);
      th.SetY1(y1);
      th.SetX2(x2);
      th.SetY2(y2);
    }
    void Draw() {
      th.Draw("SAME");
    }
  };

  /*** Class to make certain estimates given a Shapes histogram. Currently estimates pedestal and polarity. ***/
  class PulseEstimate {
  private:
    int fPedestal;
    int fPolarity;
  public:
    PulseEstimate() : fPedestal(0), fPolarity(0) {
    }
    /* Estimates parameters of shapes histogram */
    /*
      Pedestal: Looks for the most common ADC value
      Polarity: Looks at the max and min ADC value and whichever is further from the estimated pedestal sets the polarity.
    */
    void Estimate(TH2* pulses) {
      TH1D* proj_y = pulses->ProjectionY("_py",1);
      fPedestal = (int)proj_y->GetBinCenter(proj_y->GetMaximumBin());
      int min = 0, max = 0;
      bool min_found = false, max_found = false;
      int nbins = proj_y->GetNbinsX();
      for (int i = 1; i < nbins; ++i) {
	if (!min_found && proj_y->GetBinContent(i) > 0.) {
	  min_found = true;
	  min = i;
	}
	if (!max_found && proj_y->GetBinContent(nbins - i) > 0.) {
	  max_found = true;
	  max = nbins - i;
	}
	if (min_found && max_found)
	  break;
	}
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
    void Print() const {
      std::cout <<
	"Pedestal (Est): " << fPedestal << std::endl <<
	"Polarity (Est): " << fPolarity << std::endl;
    }
  };
  
  /*** Class to draw all things associated with a Shapes histogram ***/
  /*
    Sign: Plus or minus sign depending on polairty in ODB
    Pedestals: The ODB pedestal and estimated pedestal
    Thresholds: Not implemented, but supposed to be the thresholds
  */
  class ODBDraw {
  private:
    PolaritySign fDPol;
    Pedestals fDPed;
    Threshold fDThresh;
    TH2* fPulses;
    int fPolarity_ODB;
    int fPolarity_EST;
  public:
    ODBDraw() : fDPol(), fDPed(), fDThresh(),
		fPulses(NULL), fPolarity_ODB(1) {
    }
  public:
    /* Set all the pedestals and polarities based on ODB values and estimates */
    void Set(TH2* pulses, const WireMap& odb, int i, const PulseEstimate& est) {
      fPulses = pulses;
      double x1 = fPulses->GetXaxis()->GetBinLowEdge(1);
      double y1 = fPulses->GetYaxis()->GetBinLowEdge(1);
      double x2 = fPulses->GetXaxis()->GetBinLowEdge(fPulses->GetNbinsX()+1);
      double y2 = fPulses->GetYaxis()->GetBinLowEdge(fPulses->GetNbinsY()+1);
      double dx = x2-x1;
      double dy = y2-y1;
      fPolarity_ODB = odb.polarity[i];
      fPolarity_EST = est.GetPolarity();
      fDPol.Set(x2-0.1*dx, y2-0.1*dy, x2, y2);
      fDPed.Set(x1, x2, (double)odb.pedestal[i], (double)est.GetPedestal());
      //fDThresh.Set(x1, (double)odb.threshold[i], x2, (double)odb.threshold[i]);
    }
    /* Draw pedestals, sign, and Shapes */
    void Draw() {
      fPulses->Draw("COLZ");
      fDPol.Draw(fPolarity_ODB);
      fDPed.Draw();
      //fDThresh.Draw(); // Not implemented yet
    }
  };
  
  /*** Stoes directory information for raw data (in case no ODB file available), ODB location, histograms location, and where to save the correction files ***/
  class DataDir {
  private:
    std::string fRawExt, fODBExt, fHistExt, fCorrExt;
    std::string fRawDir, fODBDir, fHistDir, fCorrDir;
    std::string fRawPre, fODBPre, fHistPre, fCorrPre;
  public:
    DataDir() : fRawExt(".mid"), fODBExt(".odb"), fHistExt(".root"), fCorrExt(".dat"),
		fRawDir(""), fODBDir(""), fHistDir(""), fCorrDir(""),
		fRawPre("run"), fODBPre("run"), fHistPre("hist"), fCorrPre("correct") {
    }
  private:
    /* Return 5 character string with run number. Could be accomplished with a single printf statement */
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
    void SetRawDir() {
      std::cout << "Enter raw data directory: ";
      std::cin >> fRawDir;
    }
    void SetODBDir(const std::string& odb_dir) {
      fODBDir = odb_dir;
    }
    void SetODBDir() {
      std::cout << "Enter ODB directory: ";
      std::cin >> fODBDir;
    }
    void SetHistDir(const std::string& hist_dir) {
      fHistDir = hist_dir;
    }
    void SetHistDir() {
      std::cout << "Enter histogram directory: ";
      std::cin >> fHistDir;
    }
    void SetCorrDir(const std::string& corr_dir) {
      fCorrDir = corr_dir;
    }
    void SetCorrDir() {
      std::cout << "Enter corrections directory: ";
      std::cin >> fCorrDir;
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
    std::string GetCorrFileName(int run) {
      std::stringstream s;
      s << run;
      return fCorrDir + fCorrPre + s.str() + fCorrExt;
    }
  };
  
  /*** The workhorse. Gets ODB information for several values,
       makes estimates based on respective Shapes histogram,
       then outputs correction file if estimates are
       different. If not set to batch mode, draws Shapes
       histogram and superimposes estimates. ***/
  class ODBCheck {
  private:
    int fRun;
    TCanvas fCanvas;
    ODBDraw fODBDraw;
    WireMap fODB;
    DataDir fDataDirs;
    bool fLoadODBFile;
    PulseEstimate fEstimate;
    bool fBatchMode;
    bool fOutputCorrections;
    std::ofstream fCorrectionsFile;

  public:
    ODBCheck() : fRun(0), fCanvas("c","Check"), fODBDraw(),
		 fODB(), fDataDirs(), fLoadODBFile(true),
		 fEstimate(), fBatchMode(false), fOutputCorrections(false),
		 fCorrectionsFile() {
      fCanvas.SetLogz();
    }

    void SetDirs(const std::string& raw, const std::string& odb, const std::string& hist, const std::string& corr) {
      fDataDirs.SetRawDir(raw);
      fDataDirs.SetODBDir(odb);
      fDataDirs.SetHistDir(hist);
      fDataDirs.SetCorrDir(corr);
    }

    void SetDirs() {
      fDataDirs.SetRawDir();
      fDataDirs.SetODBDir();
      fDataDirs.SetHistDir();
      fDataDirs.SetCorrDir();
    }

    /* When in batch mode, do not do any drawings or pause for any input */
    void SetBatchMode() {
      fBatchMode = true;
      fCanvas.Close();
      std::cout << "ODBCheck INFO: Entering batch mode. Canvas is closed and interactive mode is no longer available." << std::endl;
    }
    void SetOutputCorrections() {
      fOutputCorrections = true;
    }
    void ResetOutputCorrections() {
      fOutputCorrections = false;
      if (fCorrectionsFile.is_open())
	fCorrectionsFile.close();
    }
  private:
    /* Read in ODB values from ODB file or raw data file depending on flag*/ 
    void LoadODBValues() {
      static std::string header("");
      static std::string bankname_key("BankName");
      static std::string detname_key("DetectorName");
      static std::string enabled_key("Enabled");
      static std::string polarity_key("TriggerPolarity");
      static std::string pedestal_key("Pedestal");
      static std::string timeshift_key("TimeShift");
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

    /* Open corrections file and write out header */
    void InitiateCorrectionsFile() {
      fCorrectionsFile.open(fDataDirs.GetCorrFileName(fRun).c_str());
      if (fCorrectionsFile.is_open())
	fCorrectionsFile <<
	  "Bank name | Field | New value" << std::endl <<
	  "--------------------------------" << std::endl;
      else
	std::cout << "ODBCheck ERROR: Cannot open correction file (" << fDataDirs.GetCorrFileName(fRun) << ")!" << std::endl;
    }

    /* Check if the estimates are different than the ODB values, and output appropriate line if so to corrections file */
    void OutputCorrectionsIfNeeded(unsigned int i) {
      if (fCorrectionsFile.is_open()) {
	  if (fODB.pedestal[i] != fEstimate.GetPedestal())
	    fCorrectionsFile << fODB.bankname[i] << "\tPedestal\t" << fEstimate.GetPedestal() << std::endl;
	  if (fODB.polarity[i] != fEstimate.GetPolarity())
	    fCorrectionsFile << fODB.bankname[i] << "\tPolarity\t" << fEstimate.GetPolarity() << std::endl;
      }
    }

  public:
    /* Run the check on a run */
    void Check(int run) {
      std::string opt;
      if (fRun != run) {
	fRun = run;
	LoadODBValues();
	InitiateCorrectionsFile();
      }
      TFile hist_file(fDataDirs.GetHistFileName(fRun).c_str(), "READ");
      for (unsigned int i = 0; i < fODB.n; ++i) {
	TH2* shapes;
	hist_file.GetObject(("h" + fODB.bankname[i] + "_Shapes").c_str(), shapes);
	if (shapes) {
	  shapes->SetStats(0);
	  fEstimate.Estimate(shapes);
	  if (!fBatchMode) {
	    fODBDraw.Set(shapes, fODB, i, fEstimate);
	    fCanvas.cd();
	    fODBDraw.Draw();
	    fCanvas.Update();
	    std::cout <<
	      "Pedestal (ODB): " << fODB.pedestal[i] << std::endl <<
	      "Polarity (ODB): " << fODB.polarity[i] << std::endl;
	    fEstimate.Print();
	    std::cout << "[q]uit, [n]ext, [p]rev: ";
	    std::cin >> opt;
	    if (opt == "p") {
	      i -= 2;
	    } else if (opt == "q") {
	      hist_file.Close();
	      return;
	    }
	  }
	  if (shapes->GetEntries()) {
	    OutputCorrectionsIfNeeded(i);
	  }
	}
      } 
    }

    /* Set flag to read ODB values from ODB file */
    void LoadODBFromODBFile() {
      fLoadODBFile = true;
    }
    /* Setflag to read ODB values from raw data file */
    void LoadODBFromDataFile() {
      fLoadODBFile = false;
    }
  };
}


/* The main part of the script */
void odb_check(int run) {
  using namespace ODB;
  ODBCheck x;
  std::string raw_dir("../../../data/raw/");
  std::string odb_dir("../../../data/odb/");
  std::string hist_dir("../../../data/hist/");
  std::string corr_dir("../../../data/corr/");
  x.SetDirs(raw_dir, odb_dir, hist_dir, corr_dir);
  x.LoadODBFromODBFile();
  x.SetBatchMode();
  x.Check(run);
}
