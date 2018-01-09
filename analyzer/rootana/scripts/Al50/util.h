#ifndef AL50_UTIL_H_
#define AL50_UTIL_H_

#include "TMath.h"

#include <cmath>
#include <string>

enum Side  { kRight, kLeft };
enum Layer { kThin,  kThick };

struct Particle { std::string Name; double M; /*GeV*/ };
bool operator==(const Particle& l, const Particle& r) { return l.Name == r.Name; }
const Particle PHOTON   = {"gamma",     0.};
const Particle ELECTRON = {"electron",  511.999e-6};
const Particle MUON     = {"muon",      105.658e-3};
const Particle PROTON   = {"proton",    938.272e-3};
const Particle DEUTERON = {"deuteron",  1876.124e-3};
const Particle TRITON   = {"triton",    2809.432e-3};
const Particle ALPHA    = {"alpha",     3727.379e-3};
enum ParticleType {
  kNotAParticle, kPhoton, kElectron, kMuon, kProton, kDeuteron, kTriton,
  kAlpha, kAllParticleTypes
};

namespace GeFcn {
  // A:  Amplitude of gaussian
  // S:  Sigma of gaussian
  // db: Bin width (except in efficiency function, where it's error on b)
  // E:  Mean energy of photon peak
  // I:  Intensity (probably 0.798 +/- 0.008)
  // d*: Error on valud (except db, see above)
  static const double rt2pi = sqrt(2.*TMath::Pi());
  double GaussArea(double A, double S, double db) {
    return rt2pi*A*S/db;
  }
  double GaussAreaErr(double A, double dA, double S, double dS, double db) {
    return GaussArea(A, S, db) * sqrt(dA*dA/(A*A) + dS*dS/(S*S));
  }
  double GeHiEff(double E) {
    static const double a = 0.244223,   b = -0.931899;
    return a*pow(E, b);
  }
  double GeHiEffErr(double E) {
    static const double a = 0.244223,   b = -0.931899, c = -0.995306,
                       da = 0.0228416, db = 0.0148333;
    return sqrt(pow(E, 2*b) * (da*da + log(E)*a*db*(a*db*log(E) + 2*c*da)));
  }
  double GeLoEff(double E) {
    static const double a = 0.223417,   b = -0.919075;
    return a*pow(E, b);
  }
  double GeLoEffErr(double E) {
    static const double a = 0.223417,   b = -0.919075, c = -0.995269,
                       da = 0.0213393, db = 0.0151079;
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
#endif
