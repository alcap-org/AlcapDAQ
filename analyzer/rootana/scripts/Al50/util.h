#ifndef AL50_UTIL_H_
#define AL50_UTIL_H_

#include "./scripts/TMETree/TMEUtils.h"
#include "./src/plotters/SimplePulse.h"

#include "RooUnfoldResponse.h"

#include "TArrayD.h"
#include "TAxis.h"
#include "TF1.h"
#include "TH1.h"
#include "TH2.h"
#include "TH1D.h"
#include "TMath.h"
#include "TVector2.h"

#include <cmath>
#include <iostream>
#include <string>
#include <vector>


void PrintAndThrow(const char* err) {
  std::cout << err << std::endl;
  throw err;
}

enum Side  { kRight, kLeft, kNSides, kInvalidSide };
enum Layer { kThin,  kThick };

struct Particle {
  std::string Name;
  double M; /*GeV*/
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

namespace Normalization {
  double nmu_hi_al50_p102 = 109e6;
  double nmu_hi_al50_p103 = 27.2e6;
  double nmu_hi_al50      = 137e6;
  double nmu_hi_al100     = 105e6;
  double caprate          = 0.609;
  double TCutEfficiency(double prot_tcut,
                        double sig=52.7, double lifetime=864) {
    static TF1* ftime = new TF1("f", "exp([0]^2/(2*[1]^2)-x/[1])*TMath::Erfc(([0]^2-[1]*x)/(sqrt(2)*[0]*[1]))", 0, 10000);
    ftime->SetParameters(sig, lifetime);
    return ftime->Integral(400, 1e6)/ftime->Integral(0, 1e6);
  }
  double Al50_p102(double prot_tcut=400) {
    return nmu_hi_al50_p102*caprate*TCutEfficiency(prot_tcut);
  }
  double Al50_p103(double prot_tcut=400) {
    return nmu_hi_al50_p103*caprate*TCutEfficiency(prot_tcut);
  }
  double Al50(double prot_tcut=400) {
    return nmu_hi_al50*caprate*TCutEfficiency(prot_tcut);
  }
  double Al100() {
    return nmu_hi_al100*caprate*1;
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
}

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
  double GeHiEff(double E) {
    static const double a = 0.193,   b = -0.944;
    return a*pow(E, b);
  }
  double GeHiEffErr(double E) {
    static const double a = 0.193, b = -0.944, c = -0.995306,
                       da = 0.016, db = 0.013;
    return sqrt(pow(E, 2*b) * (da*da + log(E)*a*db*(a*db*log(E) + 2*c*da)));
  }
  double GeLoEff(double E) {
    static const double a = 0.179, b = -0.933;
    return a*pow(E, b);
  }
  double GeLoEffErr(double E) {
    static const double a = 0.179, b = -0.933, c = -0.995269,
                       da = 0.015, db = 0.013;
    return sqrt(pow(E, 2*b) * (da*da + log(E)*a*db*(a*db*log(E) + 2*c*da)));
  }
  double NMuHi(double A, double S, double db, double I, double E) {
    return GaussArea(A, S, db)/I/GeHiEff(E);
  }
  double NMuErrHi(double A, double dA, double S, double dS, double db,
                  double I, double dI, double E) {
    double area  = GaussArea(A, S, db);
    double darea = GaussAreaErr(A, dA, S, dS, db);
    double e     = GeHiEff(E);
    double de    = GeHiEffErr(E);
    return NMuHi(A, S, db, I, E)*sqrt(darea*darea/(area*area)+
                                      dI*dI/(I*I)+de*de/(e*e));
  }
  double NMuLo(double A, double S, double db, double I, double E) {
    return GaussArea(A, S, db)/I/GeLoEff(E);
  }
  double NMuErrLo(double A, double dA, double S, double dS, double db,
                  double I, double dI, double E) {
    double area  = GaussArea(A, S, db);
    double darea = GaussAreaErr(A, dA, S, dS, db);
    double e     = GeLoEff(E);
    double de    = GeLoEffErr(E);
    return NMuLo(A, S, db, I, E)*sqrt(darea*darea/(area*area)+
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
  // E in units of ke
  bool OverThreshold(const std::string det, int seg, double E /*keV*/) {
    double Eth = 100e3;
    if (det == "SiL1") {
      switch (seg) {
        case 2:  Eth = 289.; break;
        case 3:  Eth = 268.; break;
        case 4:  Eth = 219.; break;
        case 5:  Eth = 200.; break;
        case 6:  Eth = 194.; break;
        case 7:  Eth = 215.; break;
        case 8:  Eth = 287.; break;
        case 9:  Eth = 305.; break;
        case 10: Eth = 227.; break;
        case 11: Eth = 211.; break;
        case 12: Eth = 207.; break;
        case 13: Eth = 227.; break;
        case 14: Eth = 227.; break;
        case 15: Eth = 139.; break;
        default: PrintAndThrow("Invalid SiL1 segment!");
      }
    } else if (det == "SiL3") {
      Eth = 155.;
    } else if (det == "SiR1") {
      switch (seg) {
        case 1: Eth = 80.;
        case 2: Eth = 100;
        case 3: Eth = 80;
        case 4: Eth = 85;
      }
    } else if (det == "SiR2") {
      Eth = 195.;
    } else if (det == "SiR3") {
      Eth = 250.;
    } else {
      PrintAndThrow("Invalid detector");
    }
    return E > Eth;
  }
  double BetheStoppingPower(double dx/*m*/, double E/*keV*/, int pid) {
    // SiR1 dx = 58e-6 m
    // SiL1 dx = 52e-6 m
    double N_A   = 6.022140857e23;      // atoms per mol
    double Z     = 14;
    double rho   = 2.328;               // g/cm3
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
      case 1:  z = 1; m = 938272.03; break;
      case 2:  z = 1; m = 1875612.9; break;
      case 3:  z = 1; m = 2809432.;  break;
      case 4:  z = 2; m = 3727379.;  break;
      default: throw "Invalid particle selection.";
    }
    double a = 2*pi*n*z*z*m/(m_e*e*e)*Q*Q;
    double b = 4*m_e/(m*I);
    std::cout << "a: " << a << std::endl << "b: " << b << std::endl;
    return 2*pi*n*z*z*m/(m_e*E*e*e)*Q*Q*std::log(4*E*m_e/(m*I))*dx;
  }
  double dE(double t/*m*/, double E/*keV*/, int pid) {
    double dx = 1e-8;
    double edep = 0;
    while (t > 0 && E > edep) {
      edep += BetheStoppingPower(dx, E-edep, pid);
      t -= dx;
    }
    return edep;
  }
  Double_t dE(Double_t x[], Double_t par[]) {
    return dE(par[0], x[0], (int)par[1]);
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

// Transfer Matrix Utilities
namespace TMUtils {
  void Rebin(RooUnfoldResponse* r, int n) {
    r->Htruth()   ->Rebin(n);
    r->Hmeasured()->Rebin(n);
    r->Hfakes()   ->Rebin(n);
    r->Hresponse()->Rebin2D(n, n);
  }

  // Requires fixed binning, anfd same bins in truth/measured dimensions.
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
