#ifndef BrentDeckerMinimizer_hh_
#define BrentDeckerMinimizer_hh_

#include "AlcapExcept.h"

#include "Minuit2/FCNBase.h"
#include "TFitterMinuit.h"

#include <iostream>
#include <vector>

#include "TH1.h"

class BrentDeckerMinimizer {

  struct Parameter_t {
    std::string fName;
    double fInitialGuess;
    double fStep;
    double fMin;
    double fMax;
  };

 public:
 BrentDeckerMinimizer(int n_pars) : fNPars(n_pars) {
    fParameters.resize(n_pars);
    fCurrentParValues.resize(n_pars);
    fLimit = 1e-7;

    a_k.resize(fNPars);
    b_k.resize(fNPars);
    lower.resize(fNPars);
    higher.resize(fNPars);

    //    if (!hInitialParAttempts) {
    //      hInitialParAttempts = new TH1F("hInitialParAttempts", "", 1000,0,1000);
    //    }
    //    std::cout << "NB: BrentDeckerMinimizer does nothing in Clear() and CreateMinimizer()" << std::endl;
  };

  void SetMinuitFCN(ROOT::Minuit2::FCNBase* fcn) { fFCN = fcn; }
  void SetPrintLevel(int level) { fPrintLevel = level; }
  void Clear() { };
  void CreateMinimizer(TFitterMinuit::EMinimizerType type) { };

  void SetParameter(int index, std::string name, double guess, double step, double min, double max) {
    if (fPrintLevel > 0) {
      std::cout << "Setting Parameter: " << name << "(" << guess << ", " << step << ", " << min << ", " << max << ")" << std::endl;
    }
    fParameters.at(index).fName = name;
    fParameters.at(index).fInitialGuess = guess;
    fParameters.at(index).fStep = step;
    fParameters.at(index).fMin = min;
    fParameters.at(index).fMax = max;

    fCurrentParValues.at(index) = guess;
  }

  double GetParameter(int index) {
    return fCurrentParValues.at(index);
  }
  
  int Minimize(int n_max_tries);
  double Gradient(std::vector<double>& pars);
  
  void PrintParameters(std::vector<double>& pars);
 private:

  int fNPars;
  int fPrintLevel;
  std::vector<Parameter_t> fParameters;
  std::vector<double> fCurrentParValues;
  ROOT::Minuit2::FCNBase* fFCN;
  double fLimit;

  
  std::vector<double> a_k;
  std::vector<double> b_k;
  double f_b;
  std::vector<double> b_prev;
  double f_b_prev;
  
  // For the gradient calculation
  std::vector<double> lower;
  std::vector<double> higher;

  inline void SecantMethod(std::vector<double>& s) {
    for (int i_par = 0; i_par < fNPars; ++i_par) {
      SecantMethod(s, i_par);
    }
  }
  
  inline void SecantMethod(std::vector<double>& s, int i_par) {
    double b_p = b_prev.at(i_par);
    double b = b_k.at(i_par);
      
    s.at(i_par) = b - ( (b - b_p)/(f_b - f_b_prev) )*f_b;
  }

  inline void BisectionMethod(std::vector<double>& s) {
    for (int i_par = 0; i_par < fNPars; ++i_par) {
      BisectionMethod(s, i_par);
    }
  }

  inline void BisectionMethod(std::vector<double>& s, int i_par) {
    s.at(i_par) = (a_k.at(i_par) + b_k.at(i_par))/2;
  }

  //  static TH1F* hInitialParAttempts;
};



#endif
