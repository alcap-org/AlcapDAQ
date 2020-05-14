#ifndef AL50_UTIL_H_
#define AL50_UTIL_H_

#include "./scripts/TMETree/TMEUtils.h"
#include "./src/plotters/SimplePulse.h"

#include "RooUnfoldResponse.h"

#include "TArrayD.h"
#include "TAxis.h"
#include "TColor.h"
#include "TF1.h"
#include "TH1.h"
#include "TH2.h"
#include "TH1D.h"
#include "TMath.h"
#include "TTree.h"
#include "TVector2.h"

#include <cmath>
#include <iostream>
#include <string>
#include <utility>
#include <vector>



void PrintAndThrow(const char* err) {
  std::cout << err << std::endl;
  throw err;
}
void PrintAndThrow(const std::string& err) {
  PrintAndThrow(err.c_str());
}

enum Side      { kRight, kLeft, kNSides, kInvalidSide };
enum Layer     { kThin,  kThick };
enum Dataset_t { kAl50, kAl100, kTi50 };

struct TimeOffset_t {
  // Applied directly, so this is ADDED to the time.
  // Indexed left/right, so L=0, R=1
  double t1[2], t2[2], t3[2];
  TimeOffset_t() : t1{0.,0.}, t2{0.,0.}, t3{0.,0.} {}
  TimeOffset_t(const std::string dataset) {
    if (dataset == "Al50") {
      t1[0] = 0., t2[0] = 0., t3[0] = 0.;
      t1[1] = 0., t2[1] = 0., t3[1] = 0.;
    } else if (dataset == "Al100") {
      t1[0] = 650., t2[0] = 90,   t3[0] = 0.;
      t1[1] = -130, t2[1] = 100., t3[1] = 0.;
    } else if (dataset == "Ti50") {
      t1[0] = 0., t2[0] = 0., t3[0] = 0.;
      t1[1] = 0., t2[1] = 0., t3[1] = 0.;
    } else if (dataset == "Si16b") {
      t1[0] = 0., t2[0] = 0., t3[0] = 0.;
      t1[1] = 0., t2[1] = 0., t3[1] = 0.;
    } else {
      PrintAndThrow("Error: Unknown dataset "+dataset);
    }
  }
  void Apply() {
    // for (int i = 0; i < SiL1_1 ->size(); ++i) SiL1_1 ->at(i).tTME -= t1[0];
    for (int i = 0; i < SiL1_2 ->size(); ++i) SiL1_2 ->at(i).tTME -= t1[0];
    for (int i = 0; i < SiL1_3 ->size(); ++i) SiL1_3 ->at(i).tTME -= t1[0];
    for (int i = 0; i < SiL1_4 ->size(); ++i) SiL1_4 ->at(i).tTME -= t1[0];
    for (int i = 0; i < SiL1_5 ->size(); ++i) SiL1_5 ->at(i).tTME -= t1[0];
    for (int i = 0; i < SiL1_6 ->size(); ++i) SiL1_6 ->at(i).tTME -= t1[0];
    for (int i = 0; i < SiL1_7 ->size(); ++i) SiL1_7 ->at(i).tTME -= t1[0];
    for (int i = 0; i < SiL1_8 ->size(); ++i) SiL1_8 ->at(i).tTME -= t1[0];
    for (int i = 0; i < SiL1_9 ->size(); ++i) SiL1_9 ->at(i).tTME -= t1[0];
    for (int i = 0; i < SiL1_10->size(); ++i) SiL1_10->at(i).tTME -= t1[0];
    for (int i = 0; i < SiL1_11->size(); ++i) SiL1_11->at(i).tTME -= t1[0];
    for (int i = 0; i < SiL1_12->size(); ++i) SiL1_12->at(i).tTME -= t1[0];
    for (int i = 0; i < SiL1_13->size(); ++i) SiL1_13->at(i).tTME -= t1[0];
    for (int i = 0; i < SiL1_14->size(); ++i) SiL1_14->at(i).tTME -= t1[0];
    for (int i = 0; i < SiL1_15->size(); ++i) SiL1_15->at(i).tTME -= t1[0];
    // for (int i = 0; i < SiL1_16->size(); ++i) SiL1_16->at(i).tTME -= t1[0];
    for (int i = 0; i < SiR1_1 ->size(); ++i) SiR1_1 ->at(i).tTME -= t1[1];
    for (int i = 0; i < SiR1_2 ->size(); ++i) SiR1_2 ->at(i).tTME -= t1[1];
    for (int i = 0; i < SiR1_3 ->size(); ++i) SiR1_3 ->at(i).tTME -= t1[1];
    for (int i = 0; i < SiR1_4 ->size(); ++i) SiR1_4 ->at(i).tTME -= t1[1];
    for (int i = 0; i < SiL3   ->size(); ++i) SiL3   ->at(i).tTME -= t2[0];
    // for (int i = 0; i < SiL2   ->size(); ++i) SiL2   ->at(i).tTME -= t3[0];
    for (int i = 0; i < SiR2   ->size(); ++i) SiR2   ->at(i).tTME -= t2[1];
    for (int i = 0; i < SiR3   ->size(); ++i) SiR3   ->at(i).tTME -= t3[1];
  }
};

struct NMuConfig_t {
  int run;
  double prompt_t1, prompt_t2;
  double delayed_t1, delayed_t2;
  double tofflo, toffhi;
  std::string dataset;
  NMuConfig_t(TTree* tr) {
    std::string* pdataset = &dataset;
    tr->SetBranchAddress("run",        &run);
    tr->SetBranchAddress("prompt_t1",  &prompt_t1);
    tr->SetBranchAddress("prompt_t2",  &prompt_t2);
    tr->SetBranchAddress("delayed_t1", &delayed_t1);
    tr->SetBranchAddress("delayed_t2", &delayed_t2);
    tr->SetBranchAddress("tofflo",     &tofflo);
    tr->SetBranchAddress("toffhi",     &toffhi);
    tr->SetBranchAddress("dataset",    &pdataset);
    tr->GetEntry(0);
    tr->ResetBranchAddresses();
  }
  void Print() {
    std::cout << "NMuConfig:"
              << "\nRun: " << run
              << "\nPrompt cut:  " << prompt_t1  << " < t < " << prompt_t2
              << "\nDelayed cut: " << delayed_t1 << " < t < " << delayed_t2
              << "\nGeLo TOff:   " << tofflo
              << "\nGeHi TOff:   " << toffhi
              << "\nDataset:     " << dataset
              << std::endl;
  }
};

struct PSelConfig_t {
  bool usealllayers;
  double pp, bandwidthscale;
  std::string dataset;
  TimeOffset_t terr;
  PSelConfig_t(TTree* tr) {
    std::string *pdataset = &dataset;
    tr->SetBranchAddress("pileup_protection", &pp);
    tr->SetBranchAddress("bandwidthscale",    &bandwidthscale);
    tr->SetBranchAddress("usealllayers",      &usealllayers);
    tr->SetBranchAddress("dataset",           &pdataset);
    tr->SetBranchAddress("t1lerr",            &terr.t1[0]);
    tr->SetBranchAddress("t2lerr",            &terr.t2[0]);
    tr->SetBranchAddress("t3lerr",            &terr.t3[0]);
    tr->SetBranchAddress("t1rerr",            &terr.t1[1]);
    tr->SetBranchAddress("t2rerr",            &terr.t2[1]);
    tr->SetBranchAddress("t3rerr",            &terr.t3[1]);
    tr->GetEntry(0);
    tr->ResetBranchAddresses();
  }
  void Print() const {
    std::cout << "PSelConfig:"
              << "\nThird layer: " << usealllayers
              << "\nPP:          " << pp
              << "\nBandwidth X: " << bandwidthscale
              << "\nDataset:     " << dataset
              << "\nSiL1 toff:   " << terr.t1[0]
              << "\nSiL3 toff:   " << terr.t2[0]
              << "\nSiL2 toff:   " << terr.t3[0]
              << "\nSiR1 toff:   " << terr.t1[1]
              << "\nSiR2 toff:   " << terr.t2[1]
              << "\nSiR3 toff:   " << terr.t3[1]
              << std::endl;
  }
};

struct TMConfig_t {
  double thinres, thickres;
  std::string particle, dataset;
  TMConfig_t(TTree* tr) {
    std::string *pparticle = &particle, *pdataset = &dataset;
    tr->SetBranchAddress("thinressig",  &thinres);
    tr->SetBranchAddress("thickressig", &thickres);
    tr->SetBranchAddress("particle",    &pparticle);
    tr->SetBranchAddress("dataset",     &pdataset);
    tr->GetEntry(0);
    tr->ResetBranchAddresses();
  }
  void Print() const {
    std::cout << "TMConfig:"
              << "\nThin Res.:  " << thinres
              << "\nThick Res.: " << thickres
              << "\nParticle:   " << particle
              << "\nDataset:    " << dataset
              << std::endl;
  }
};

struct UnfoldConfig_t {
  bool pidcorrect, usealllayers;
  int rebin;
  double pp, t, dt, elo, ehi;
  double thinres, thickres, bandwidthscale;
  TimeOffset_t terr;
  std::string noise, particle, dataset;
  UnfoldConfig_t(TTree* tr) {
    std::string *pnoise = &noise, *pparticle = &particle, *pdataset = &dataset;
    tr->SetBranchAddress("pidcorrect",      &pidcorrect);
    tr->SetBranchAddress("usealllayers",    &usealllayers);
    tr->SetBranchAddress("rebin",           &rebin);
    tr->SetBranchAddress("pp",              &pp);
    tr->SetBranchAddress("t",               &t);
    tr->SetBranchAddress("dt",              &dt);
    tr->SetBranchAddress("elo",             &elo);
    tr->SetBranchAddress("ehi",             &ehi);
    tr->SetBranchAddress("thinres",         &thinres);
    tr->SetBranchAddress("thickres",        &thickres);
    tr->SetBranchAddress("bandwidthscale",  &bandwidthscale);
    tr->SetBranchAddress("noisetype",       &pnoise);
    tr->SetBranchAddress("particle",        &pparticle);
    tr->SetBranchAddress("dataset",         &pdataset);
    tr->SetBranchAddress("t1lerr",          &terr.t1[0]);
    tr->SetBranchAddress("t2lerr",          &terr.t2[0]);
    tr->SetBranchAddress("t3lerr",          &terr.t3[0]);
    tr->SetBranchAddress("t1rerr",          &terr.t1[1]);
    tr->SetBranchAddress("t2rerr",          &terr.t2[1]);
    tr->SetBranchAddress("t3rerr",          &terr.t3[1]);
    tr->GetEntry(0);
    tr->ResetBranchAddresses();
  }
  void Print() const {
    std::cout << "TMConfig:"
              << "\nPID Correction: " << pidcorrect
              << "\nThird layer:    " << usealllayers
              << "\nRebin:          " << rebin
              << "\nPP:             " << pp
              << "\nt-cut:          " << t
              << "\ndt-cut:         " << dt
              << "\nEnergy range:   " << elo << "-" << ehi << " keV"
              << "\nThin Res.:      " << thinres
              << "\nThick Res.:     " << thickres
              << "\nBandwidth X:    " << bandwidthscale
              << "\nNoise:          " << noise
              << "\nParticle:       " << particle
              << "\nDataset:        " << dataset
              << std::endl;
  }
};

namespace ACStyle {
  // Colors from http://colorbrewer2.org/?type=diverging&scheme=RdBu&n=3
  Int_t kACBlue = TColor::GetColor("#67a9cf");
  Int_t kACRed  = TColor::GetColor("#ef8a62");
}


namespace Sanity {
  bool IsValidDataset(const std::string ds) {
    if (ds == "Ti50" || ds == "Al50" || ds == "Al100" || ds == "Si16b")
      return true;
    else
      return false;
  }
  bool CompatibleConfigs(const TMConfig_t& tm, const PSelConfig_t& psel) {
    return tm.dataset == psel.dataset;
  }
}

struct Particle {
  std::string Name;
  double M; /*GeV*/
  Particle(const std::string& p="null");
  Particle(const char* name, double m) : Name(name), M(m) {}
  double P2E(double px, double py, double pz) const {
    return sqrt(px*px+py*py+pz*pz+M*M);
  }
  double P2T(double px, double py, double pz) const {
    return P2E(px, py, pz)-M;
  }
  double E2T(double e) const {
    return e - M;
  }
};
bool operator==(const Particle& l, const Particle& r) { return l.Name == r.Name; }
const Particle NULLPARTICLE = {"null",     -1.};
const Particle PHOTON       = {"gamma",    0.};
const Particle ELECTRON     = {"e-",       511.999e-6};
const Particle MUON         = {"mu-",      105.658e-3};
const Particle PROTON       = {"proton",   938.272e-3};
const Particle DEUTERON     = {"deuteron", 1876.124e-3};
const Particle TRITON       = {"triton",   2809.432e-3};
const Particle ALPHA        = {"alpha",    3727.379e-3};
static const Particle* PARTICLES[4] = { &PROTON, &DEUTERON, &TRITON, &ALPHA };
enum ParticleType {
  kNotAParticle, kPhoton, kElectron, kMuon, kProton, kDeuteron, kTriton,
  kAlpha, kAllParticleTypes
};

Particle::Particle(const std::string& p) {
  if      (p == NULLPARTICLE.Name) *this = NULLPARTICLE;
  else if (p == PROTON.Name)       *this = PROTON;
  else if (p == DEUTERON.Name)     *this = DEUTERON;
  else if (p == TRITON.Name)       *this = TRITON;
  else if (p == ALPHA.Name)        *this = ALPHA;
  else PrintAndThrow("ERROR: Unknown particle: " + p);
}

class Dataset {
  double mom;
  class Tranch {
  };
};


namespace Normalization {
  double nmu_hi_al50_p102 = 128e6;
  double nmu_hi_al50_p103 = 37.0e6;
  double nmu_hi_al50      = 160e6;
  double nmu_hi_al100     = 134e6; // UPDATE
  double nmu_hi_ti50      = 62.6e6;
  double caprate          = 0.609;
  double TCutEfficiency(double prot_tcut, double sig=52.7,
                        double lifetime=864.) {
    static TF1* ftime = new TF1("ftime", "exp([0]^2/(2*[1]^2)-x/[1])*TMath::Erfc(([0]^2-[1]*x)/(sqrt(2)*[0]*[1]))", 0, 1e6);
    ftime->SetParameters(sig, lifetime);
    return ftime->Integral(prot_tcut, 1e6)/ftime->Integral(-1e4, 1e6);
  }
  double Al50_p102(double prot_tcut, double sig=52.7, double lt=864.) {
    return nmu_hi_al50_p102*caprate*TCutEfficiency(prot_tcut, sig, lt);
  }
  double Al50_p103(double prot_tcut, double sig=52.7, double lt=864.) {
    return nmu_hi_al50_p103*caprate*TCutEfficiency(prot_tcut, sig, lt);
  }
  double Al50(double prot_tcut, double sig=52.7, double lt=864.) {
    return nmu_hi_al50*caprate*TCutEfficiency(prot_tcut, sig, lt);
  }
  double Al100(double prot_tcut, double sig=52.7, double lt=864.) {
    return nmu_hi_al100*caprate*TCutEfficiency(prot_tcut, sig, lt);
  }
  double Ti50(double prot_tcut, double sig=52.7, double lt=329.3) {
    return nmu_hi_ti50*caprate*TCutEfficiency(prot_tcut, sig, lt);
  }
};

namespace HistUtils {
  TH1* ProjectionXY(const TH2* h, double phi, double XP, double w) {
    std::string pname = h->GetName();
    pname += "_pxy";
    const TAxis *X = h->GetXaxis(), *Y = h->GetYaxis();
    TVector2 o(X->GetXmin(), Y->GetXmin()), d(X->GetXmax(), Y->GetXmax());
    double p_lo = o.Rotate(phi).Y(), p_hi = d.Rotate(phi).Y();
    TH1* p = new TH1D(pname.c_str(), h->GetTitle(),
                      (p_hi-p_lo)/X->GetBinWidth(1), p_lo, p_hi);
    for (int i = 1; i <= h->GetNbinsX(); ++i) {
      for (int j = 1; j <= h->GetNbinsY(); ++j) {
        TVector2 x(X->GetBinCenter(i), Y->GetBinCenter(j));
        TVector2 xp = x.Rotate(phi);
        if ((XP-w/2) < xp.X() && xp.X() < (XP+w/2))
          p->Fill(xp.Y(), h->GetBinContent(i, j));
      }
    }
    return p;
  }
  Double_t* ConstructBins(int n, Double_t xlo, Double_t xhi) {
    Double_t* x = new Double_t[n+1];
    Double_t dx = (xhi-xlo)/n;
    for (int i = 0; i <= n; ++i)
      x[i] = i*dx+xlo;
    return x;
  }
  void ScaleXAxis(TH1* h, Double_t scale) {
    TAxis* ax = h->GetXaxis();
    if (ax->GetXbins()->GetSize()) {
      TArrayD x(*ax->GetXbins());
      for (int i = 0; i < x.GetSize(); ++i)
        x[i] *= scale;
      ax->Set(x.GetSize()-1, x.GetArray());
    } else {
      ax->Set(ax->GetNbins(), scale*ax->GetXmin(), scale*ax->GetXmax());
    }
  }
  // Requires fixed binning, undefined behavior otherwise
  void SetRange(TH1* h, double xlo, double xhi) {
    TAxis* x = h->GetXaxis();
    int bnd[2];
    for (bnd[0] = 1; bnd[0] <= x->GetNbins(); ++bnd[0])
      if (xlo <= x->GetBinLowEdge(bnd[0]))
        break;
    for (bnd[1] = x->GetNbins(); bnd[1] >= 1; --bnd[1])
      if (x->GetBinUpEdge(bnd[1]) <= xhi)
        break;
    TH1* h_new  = new TH1D(h->GetName(), "h_new", bnd[1]-bnd[0]+1,
                           x->GetBinLowEdge(bnd[0]), x->GetBinUpEdge(bnd[1]));
    for (int i = 1; i <= bnd[1]-bnd[0]+1; ++i) {
      int b = h->FindFixBin(h_new->GetXaxis()->GetBinCenter(i));
      h_new->SetBinContent(i, h->GetBinContent(b));
      h_new->SetBinError  (i, h->GetBinError  (b));
    }
    h_new->Copy(*h);
    delete h_new;
  }
  void MatchMaximums(std::vector<TH1*> hs) {
    double y = hs[0]->GetMaximum();
    for (int i = 1; i < hs.size(); ++i)
      if (hs[i]->GetMaximum() > y)
        y = hs[i]->GetMaximum();
    for (int i = 0; i < hs.size(); ++i)
      hs[i]->SetMaximum(y);
  }
  void MatchMaximums(std::vector<TH2*> hs) {
    MatchMaximums(std::vector<TH1*>(hs.begin(), hs.end()));
  }
  template<class T>
  double IdealBinWidth(TTree* tr, const char* brname) {
    tr->Draw(brname, "", "goff");
    int* idxs = new int[tr->GetEntries()];
    TMath::Sort((int)tr->GetEntries(), tr->GetV1(), idxs, false);
    T x;
    double xlo, xhi;
    tr->SetBranchAddress(brname, &x);
    tr->GetEntry(idxs[tr->GetEntries()/4]);
    xlo = x;
    tr->GetEntry(idxs[3*tr->GetEntries()/4]);
    xhi = x;
    tr->GetBranch(brname)->ResetAddress();
    delete[] idxs;
    return 2*(xhi-xlo)/std::cbrt(double(tr->GetEntries()));
  }
  std::pair<double, double> IntegralByUserLimits(TH1* h, double x1, double x2) {
    double err = 0;
    double val = h->IntegralAndError(h->GetXaxis()->FindFixBin(x1),
                                     h->GetXaxis()->FindFixBin(x2)-1,
                                     err);
    return std::make_pair(val, err);
  }
}

// The efficiecy functions are deprecated!!!
// !!!!!
// Use Ge 6.6+/-0.2e4 for Hi and Lo
namespace GeFcn {
  // A:  Amplitude of gaussian
  // S:  Sigma of gaussian
  // db: Bin width (except in efficiency function, where it's error on b)
  // E:  Mean energy of photon peak
  // I:  Intensity (probably 0.798 +/- 0.008)
  // d*: Error on valud (except db, see above)
  static const long double rt2pi = std::sqrt(2.*TMath::Pi());
  double GaussArea(double A, double S, double db) {
    return rt2pi*A*S/db;
  }
  double GaussAreaErr(double A, double dA, double S, double dS, double db) {
    return GaussArea(A, S, db) * sqrt(dA*dA/(A*A) + dS*dS/(S*S));
  }
  double GeHiEff(const std::string& dataset) {
    // static const double a = 0.193,   b = -0.944;
    // return a*pow(E, b);
    if      (dataset.substr(0, 2) == "Al") return 6.6e-4;
    else if (dataset.substr(0, 2) == "Ti") return 2.6e-4;
    PrintAndThrow("Unknown dataset: "+dataset);
  }
  double GeHiEffErr(const std::string& dataset) {
    // static const double a = 0.193, b = -0.944, c = -0.995306,
    //                    da = 0.016, db = 0.013;
    // return sqrt(pow(E, 2*b) * (da*da + log(E)*a*db*(a*db*log(E) + 2*c*da)));
    if      (dataset.substr(0, 2) == "Al") return 0.2e-4;
    else if (dataset.substr(0, 2) == "Ti") return 0.2e-4;
    PrintAndThrow("Unknown dataset: "+dataset);
  }
  double GeLoEff(const std::string& dataset) {
    // static const double a = 0.179, b = -0.933;
    // return a*pow(E, b);
    if      (dataset.substr(0, 2) == "Al") return 6.6e-4;
    else if (dataset.substr(0, 2) == "Ti") return 2.6e-4;
    PrintAndThrow("Unknown dataset: "+dataset);
  }
  double GeLoEffErr(const std::string& dataset) {
    // static const double a = 0.179, b = -0.933, c = -0.995269,
    //                    da = 0.015, db = 0.013;
    // return sqrt(pow(E, 2*b) * (da*da + log(E)*a*db*(a*db*log(E) + 2*c*da)));
    if      (dataset.substr(0, 2) == "Al") return 0.2e-4;
    else if (dataset.substr(0, 2) == "Ti") return 0.2e-4;
    PrintAndThrow("Unknown dataset: "+dataset);
  }
  double NMuHi(double A, double S, double db, double I,
               const std::string& dataset) {
    return GaussArea(A, S, db)/I/GeHiEff(dataset);
  }
  double NMuErrHi(double A, double dA, double S, double dS, double db,
                  double I, double dI, const std::string& dataset) {
    double area  = GaussArea(A, S, db);
    double darea = GaussAreaErr(A, dA, S, dS, db);
    // double e     = 6.6e-4; // Al
    // double de    = 0.2e-4; // Al
    double e     = GeHiEff(dataset);
    double de    = GeHiEffErr(dataset);
    return NMuHi(A, S, db, I, dataset)*sqrt(darea*darea/(area*area)+
                                            dI*dI/(I*I)+de*de/(e*e));
  }
  double NMuLo(double A, double S, double db, double I, const string& dataset) {
    double eff;
    if      (dataset.substr(0, 2) == "Al") eff = 6.6e-4;
    else if (dataset.substr(0, 2) == "Ti") eff = 2.6e-4;
    else    PrintAndThrow("Unknown dataset: "+dataset);
    return GaussArea(A, S, db)/I/eff;
  }
  double NMuErrLo(double A, double dA, double S, double dS, double db,
                  double I, double dI, const std::string& dataset) {
    double area  = GaussArea(A, S, db);
    double darea = GaussAreaErr(A, dA, S, dS, db);
    double e     = GeLoEff(dataset);
    double de    = GeLoEffErr(dataset);
    return NMuLo(A, S, db, I, dataset)*sqrt(darea*darea/(area*area)+
                                            dI*dI/(I*I)+de*de/(e*e));
  }
  Double_t GePeak(Double_t* x, Double_t* par) {
    double y    = (x[0]-par[1])/par[2];
    double F1   = 1/sqrt(2*TMath::Pi()*par[2]*par[2])*exp(-0.5*y*y);
    double F2   = par[3]*exp(par[4]*y)/pow(1+exp(y), 4);
    double F3   = par[5]*exp(par[6]*y)/pow(1+exp(-y), 4);
    double F4   = par[7]/pow(1+exp(y), 2);
    return par[0]*(F1+F2+F3+F4);
  }
  Double_t Ge2PeakWBkg(Double_t* x, Double_t* par) {
    Double_t par1[] = { par[2],  par[3],  par[4],  par[5],
                        par[6],  par[7],  par[8],  par[9] };
    Double_t par2[] = { par[10], par[11], par[4],  par[12],
                        par[13], par[14], par[15], par[16] };
    return par[0]+par[1]*x[0]+GePeak(x, par1)+GePeak(x, par2);
  }
}

namespace SiUtils {
  // E in units of keV
  bool WithinThreshold(const std::string det, int seg, double E /*keV*/) {
    double Emin = 100e3, Emax = 100e3;
    if (det == "SiL1") {
      switch (seg) {
        case 2:  Emin = 289.; break;
        case 3:  Emin = 268.; break;
        case 4:  Emin = 219.; break;
        case 5:  Emin = 200.; break;
        case 6:  Emin = 194.; break;
        case 7:  Emin = 215.; break;
        case 8:  Emin = 287.; break;
        case 9:  Emin = 305.; break;
        case 10: Emin = 227.; break;
        case 11: Emin = 211.; break;
        case 12: Emin = 207.; break;
        case 13: Emin = 227.; break;
        case 14: Emin = 227.; break;
        case 15: Emin = 139.; break;
        default: PrintAndThrow("Invalid SiL1 segment!");
      }
    } else if (det == "SiL3") {
      Emin = 155.; Emax = 17e3;
    } else if (det == "SiR1") {
      switch (seg) {
        case 1: Emin = 80.;
        case 2: Emin = 100;
        case 3: Emin = 80;
        case 4: Emin = 85;
      }
    } else if (det == "SiR2") {
      Emin = 195.; Emax = 17e3;
    } else if (det == "SiR3") {
      Emin = 250.;
    } else {
      PrintAndThrow("Invalid detector");
    }
    return Emin < E && E < Emax;
  }
  double ERes(const std::string& det, int seg=0) {
    // Naming and segment numbering uses experiment schema,
    // not simulation schema (which has different segment/quadrant ordering).
    if (det == "SiL1") {
      switch (seg) {
        case 1:  return 15.01;
        case 2:  return 14.85;
        case 3:  return 15.10;
        case 4:  return 12.69;
        case 5:  return 14.20;
        case 6:  return 13.81;
        case 7:  return 14.46;
        case 8:  return 14.94;
        case 9:  return 14.15;
        case 10: return 13.67;
        case 11: return 15.37;
        case 12: return 13.42;
        case 13: return 14.29;
        case 14: return 14.43;
        case 15: return 15.38;
        case 16: return 18.02;
        default PrintAndThrow("SiUtils::Eres: Invalid SiL1 segment: " +
                              char(i+48));
      }
    } else if (det == "SiR1") {
      switch (seg) {
        case 1: return 23.51;
        case 2: return 20.90;
        case 3: return 22.19;
        case 4: return 23.85;
        default PrintAndThrow("SiUtils::Eres: Invalid SiR1 segment: " +
                              char(i+48));
      }
    } else if (det == "SiL3") {
      return 7.076;
    } else if (det == "SiR2") {
      return 7.503;
    } else if (det == "SiR3") {
      return 10.81;
    }
    PrintAndThrow("SiUtils::ERes: Unrecognized detector: " + det);
  }
  int MCSegToExpSeg(std::string& det, int mcseg) {
    if (det == "SiL1") {
      // Inverted or normal?
      return mcseg;
      // return 17-mcseg;
    } else if (det == "SiR1") {
      static const quadmap[4] = {4, 1, 2, 3}
      return quadmap[mcseg-1];
    }
    PrintAndThrow("SiUtils::MCSegToExpSeg: Unrecognized detector: " + det);
  }
  double BetheStoppingPower(double dx/*m*/, double E/*keV*/, int pid) {
    // SiR1 dx = 58e-6 m
    // SiL1 dx = 52e-6 m
    // SiT  dx = 58e-6 m
    double N_A   = 6.022140857e23;      // atoms per mol
    double Z     = 14;
    double rho   = 2.328e6;             // g/m3
    double A     = 28.0855;             // u (dimensionless)
    double M_u   = 1;                   // g/mol
    double n     = (N_A*Z*rho)/(A*M_u); // electrons per m3
    double m_e   = 510.9989461;         // keV/c2
    double e     = -1.6021766208e-19;   // C
    double pi    = TMath::Pi();
    double eps_0 = 8.854187817620e-12;  // C/(V*m)
    double Q     = e*e/(4.*pi*eps_0);   // C*V*m
    double I     = 173e-3;              // keV
    double c     = 299792458.;          // m/s
    double z = 0., m = 0.;
    switch (pid) { // Set charge z and mass m in keV/c2
      case 1:  z = 1;  m =  938272.03;   break;
      case 2:  z = 1;  m = 1875612.9;    break;
      case 3:  z = 1;  m = 2809432.;     break;
      case 4:  z = 2;  m = 3727379.;     break;
      case 5:  z = -1; m =  105658.3745; break;
      default: PrintAndThrow("Invalid particle selection.");
    }
    double a = 2*pi*n*z*z*m/(m_e*e*e)*Q*Q/1e6;
    double b = 4*m_e/(m*I);
    // std::cout << "a: " << a << std::endl << "b: " << b << std::endl;
    // Divide by e twice to convert V in Q's units to eV, and then
    // divide by 1e6 to convert eV in Q to keV.
    return 2*pi*n*z*z*m/(m_e*E*e*e)*Q*Q*std::log(4*E*m_e/(m*I))*dx/1e6;
  }
  double dE(double t/*m*/, double E/*keV*/, int pid) {
    double dx = 0.5e-8;
    double edep = 0;
    while (t > 0 && E > edep) {
      edep += BetheStoppingPower(dx, E-edep, pid);
      t -= dx;
    }
    return edep;
  }
  Double_t dEfcn(Double_t x[], Double_t par[]) {
    return dE(par[0], x[0], (int)par[1]);
  }
  double E(double t/*m*/, double dE/*keV*/, int pid) {
    static TF1 f("", dEfcn, -1e3, 5e3, 2);
    f.SetParameters(t, pid);
    return f.GetX(dE);
  }
  Double_t Efcn(Double_t x[], Double_t par[]) {
    return E(par[0], x[0], (int)par[1]);
  }
  class SiEvent {
    std::vector<double> e, t;
    bool valid;
   public:
  // Add time cut...?
    SiEvent(const std::vector<SimplePulse>* si1,
            const std::vector<SimplePulse>* si2,
            const std::vector<SimplePulse>* si3,
            const TMECal::ECal* adc2e_si1) : e(3, 0.), t(3, 0.), valid(true) {
      if (!TMECuts::OnlyOneHit(si1) || !TMECuts::OnlyOneHit(si2) ||
          (si3 && !TMECuts::AtMostOneHit(si3))) {
        valid = false;
        return;
      }
      if (adc2e_si1) e[0] = adc2e_si1->Eval(si1->front().Amp);
      else           e[0] = si1->front().E;
      t[0] = si1->front().tTME;
      e[1] = si2->front().E;
      t[1] = si2->front().tTME;
      if (si3 && TMECuts::OnlyOneHit(si3)) {
        e[2] = si3->front().E;
        t[2] = si3->front().tTME;
      }
    }
    bool   Valid()          const { return valid;          }
    double E()              const { return e[0]+e[1]+e[2]; }
    double E(int i)         const { return e[i];           }
    double dE()             const { return E(0);           }
    double T()              const { return t[0];           }
    double T(int i)         const { return t[i];           }
    double dT()             const { return t[1] - t[0];    }
    double dT(int i, int j) const { return t[i] - t[j];    }
    bool ThreeHits() const { return e[0] > 0. && e[1] > 0. && e[2] > 0.; }
  };
}

class ECal {
  double g, b, eg, eb, chi2;
public:
  ECal(double g, double b, double eg, double eb, double chi2=1) :
  g(g), b(b), eg(eg*std::sqrt(chi2)), eb(eb*std::sqrt(chi2)) {}
  double E         (double adc) const { return g*adc+b; }
  double ADC       (double E)   const { return (E-b)/g; }
  double operator()(double adc) const { return E(adc);  }
  double Eerr(double E) const {
    return std::sqrt((eg*eg)/(g*g)*(E-b)*(E-b)+eb*eb);
  }
  double G() const { return g; }
  static ECal ConstructECal(const std::string& det) {
    if      (det == "SiT-1")    return ECal(2.02, -8.01, 0.02, 18.7, 5.   /3);
    else if (det == "SiT-2")    return ECal(1.92, 12.5,  0.02, 18.6, 3.65 /3);
    else if (det == "SiT-3")    return ECal(1.94, -11.8, 0.02, 18.8, 5.42 /3);
    else if (det == "SiT-4")    return ECal(1.96, 0.85,  0.02, 18.7, 3.82 /3);
    else if (det == "SiR1-1")   return ECal(1.77, -23.1, 0.02, 18.8, 7.97 /3);
    else if (det == "SiR1-2")   return ECal(1.76, -9.26, 0.02, 18.7, 4.96 /3);
    else if (det == "SiR1-3")   return ECal(1.79, -36.5, 0.02, 19.0, 11.26/3);
    else if (det == "SiR1-4")   return ECal(1.76, -50.5, 0.02, 19.1, 15.78/3);
    else if (det == "SiR2")     return ECal(4.24, 107.3, 0.04, 18.4, 23.96/2);
    else if (det == "SiR3")     return ECal(5.75, 58.5,  0.06, 18.9, 15.60/2);
    // else if (det == "SiL1-1")   return ECal();
    else if (det == "SiL1-2")   return ECal(2.31, 79.7,  0.04, 61.6);
    else if (det == "SiL1-3")   return ECal(2.49, 78.4,  0.04, 61.6);
    else if (det == "SiL1-4")   return ECal(2.47, 68.3,  0.04, 61.7);
    else if (det == "SiL1-5")   return ECal(2.43, 60.8,  0.04, 61.8);
    else if (det == "SiL1-6")   return ECal(2.41, 54.9,  0.04, 61.9);
    else if (det == "SiL1-7")   return ECal(2.45, 64.1,  0.04, 61.8);
    else if (det == "SiL1-8")   return ECal(2.40, 45.0,  0.04, 62.0);
    else if (det == "SiL1-9")   return ECal(2.44, 59.7,  0.04, 61.8);
    else if (det == "SiL1-10")  return ECal(2.44, 48.2,  0.04, 62.0);
    else if (det == "SiL1-11")  return ECal(2.48, 59.9,  0.04, 61.8);
    else if (det == "SiL1-12")  return ECal(2.52, 58.8,  0.04, 61.9);
    else if (det == "SiL1-13")  return ECal(2.52, 70.6,  0.04, 61.7);
    else if (det == "SiL1-14")  return ECal(2.41, 51.5,  0.04, 62.0);
    else if (det == "SiL1-15")  return ECal(2.45, 52.7,  0.04, 62.0);
    // else if (det == "SiL1-16")  return ECal();
    // else if (det == "SiL2")     return ECal();
    else if (det == "SiL3")     return ECal(4.37, 35.2, 0.04, 18.3, 5.55/3);
    else if (det == "GeLoGain") return ECal(0.396515, 0.261953,  1.29689e-06, 0.020722);
    else if (det == "GeHiGain") return ECal(0.184892, -0.896645, 5.43024e-06, 0.0407438);
    PrintAndThrow("Invalid Ecal selection: " + det);
  }
  // static double WorstSiLError(double e) {
  //   static const ECal l1 = ECal::ConstructECal("SiL1-2");
  //   static const ECal l2 = ECal::ConstructECal("SiL3");
  //   double err1 = l1.Eerr(e);
  //   double err2 = l2.Eerr(e);
  //   return std::sqrt(err1*err1+err2*err2);
  // }
  // static double WorstSiRError(double e) {
  //   static const ECal r1 = ECal::ConstructECal("SiR1-4");
  //   static const ECal r2 = ECal::ConstructECal("SiR3");
  //   double err1 = r1.Eerr(e);
  //   double err2 = r2.Eerr(e);
  //   return std::sqrt(err1*err1+err2*err2);
  // }
};

// Transfer Matrix Utilities
namespace TMUtils {
  void Rebin(RooUnfoldResponse* r, int n) {
    r->Htruth()   ->Rebin(n);
    r->Hmeasured()->Rebin(n);
    r->Hfakes()   ->Rebin(n);
    r->Hresponse()->Rebin2D(n, n);
  }

  // Requires fixed binning, and same bins in truth/measured dimensions.
  // Undefined behavior otherwise
  void SetRange(RooUnfoldResponse* r, double elo, double ehi) {
    TH1* m   = (TH1*)r->Hmeasured()->Clone();
    TH1* t   = (TH1*)r->Htruth()   ->Clone();
    TH2 *tm  = (TH2*)r->Hresponse()->Clone();
    TAxis* x = m->GetXaxis();
    int bnd[2];
    for (bnd[0] = 1; bnd[0] <= x->GetNbins(); ++bnd[0])
      if (elo <= x->GetBinLowEdge(bnd[0]))
        break;
    for (bnd[1] = x->GetNbins(); bnd[1] >= 1; --bnd[1])
      if (x->GetBinUpEdge(bnd[1]) <= ehi)
        break;
    TH1* m_new  = new TH1D("m_new", "m_new", bnd[1]-bnd[0]+1,
                           x->GetBinLowEdge(bnd[0]), x->GetBinUpEdge(bnd[1]));
    TH1* t_new  = new TH1D("t_new", "t_new", bnd[1]-bnd[0]+1,
                           x->GetBinLowEdge(bnd[0]), x->GetBinUpEdge(bnd[1]));
    TH2* tm_new = new TH2D("tm_new", "tm_new", bnd[1]-bnd[0]+1,
                           x->GetBinLowEdge(bnd[0]), x->GetBinUpEdge(bnd[1]),
                           bnd[1]-bnd[0]+1,
                           x->GetBinLowEdge(bnd[0]), x->GetBinUpEdge(bnd[1]));
    for (int i = 1; i <= bnd[1]-bnd[0]+1; ++i) {
      int b = m->FindFixBin(m_new->GetXaxis()->GetBinCenter(i));
      m_new->SetBinContent(i, m->GetBinContent(b));
      m_new->SetBinError  (i, m->GetBinError  (b));
      t_new->SetBinContent(i, t->GetBinContent(b));
      t_new->SetBinError  (i, t->GetBinError  (b));
      for (int j = 1; j <= bnd[1]-bnd[0]+1; ++j) {
        int bb = tm->FindFixBin(tm_new->GetXaxis()->GetBinCenter(i),
                                tm_new->GetYaxis()->GetBinCenter(j));
        tm_new->SetBinContent(i, j, tm->GetBinContent(bb));
        tm_new->SetBinError  (i, j, tm->GetBinError  (bb));
      }
    }
    r->Setup(m_new, t_new, tm_new);
    delete m;
    delete t;
    delete tm;
  }
}

#endif // INCLUDE GUARD
