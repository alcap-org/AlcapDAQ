#pragma once

#include "./scripts/Al50/util.h"

#include "TF1.h"
#include "TFile.h"
#include "TGraph.h"
#include "TH2.h"
#include "TH2F.h"
#include "TH3.h"
#include "TProfile.h"

#include <string>
#include <vector>


static const string PFNAME("~/data/R15b/Proton_EvdE_MC.root");
static const string DFNAME("~/data/R15b/Deuteron_EvdE_MC.root");
static const string TFNAME("~/data/R15b/Triton_EvdE_MC.root");
static const string AFNAME("~/data/R15b/Alpha_EvdE_MC.root");
static const int NPTYPE  = 5;
static const string* MCIFNAMES[4] = { &PFNAME, &DFNAME, &TFNAME, &AFNAME };
static const string DATAIFNAME("~/data/R15b/evdeal50.root");

static const double EMIN_P  = 2e3;
static const double EMIN_D  = 2.5e3;
static const double EMIN_T  = 4e3;
static const double EMIN_A  = 10e3;
static const double EMIN[4] = { EMIN_P,  EMIN_D,    EMIN_T,  EMIN_A };
namespace ParticleLikelihood {

  class PSelPow {
    double lim[2];
    TF1* f[2];
  public:
    PSelPow(double a1, double a2, double k, double emin, double emax) :
    lim{emin, emax} {
      f[0] = new TF1("", "[0]*x^[1]", emin, emax);
      f[1] = new TF1("", "[0]*x^[1]", emin, emax);
      f[0]->SetParameters(a1, k);
      f[1]->SetParameters(a2, k);
    }
    bool IsParticle(double E, double dE) const {
      return lim[0] < E         && E < lim[1] &&
             f[0]->Eval(E) < dE && dE < f[1]->Eval(E);
    }
  };

  class PSelMZ2 {
    double mz2_lim[2], e_lim[2];
  public:
    PSelMZ2(double mz2_0, double mz2_1, double e0, double e1) :
    mz2_lim{mz2_0, mz2_1}, e_lim{e0, e1} {}
    bool IsParticle(double E, double dE) const {
      return e_lim[0] < E && E < e_lim[1] &&
             mz2_lim[0] < E*dE && E*dE < mz2_lim[1];
    }
  };

  class PSelMC {
    TH2* H;
    TProfile* h;
    double lim, emin;
    double res;
  public:
    PSelMC() {}
    // Takes ownership of H
    PSelMC(TH2* H=0x0, double lim=0., double emin=0., double res=0.) :
    H(H), h(0x0), lim(lim), emin(emin), res(res) {
      h = H->ProfileX("_pfx", 1, -1, "S");
      H->SetDirectory(0);
      h->SetDirectory(0);
    }
    double Prob(double E, double dE) const {
      if (E < emin) return 0.;
      int    b    = h->FindFixBin(E);
      double mu   = h->GetBinContent(b);
      double std  = h->GetBinError(b);
      double sig2 = res*res+std*std;
      double A    = 1./std::sqrt(2*TMath::Pi()*sig2);
      return A*std::exp(-0.5*(dE-mu)*(dE-mu)/sig2);
    }
    double operator()(double E, double dE) const {
      return Prob(E, dE);
    }
    bool IsParticle(double E, double dE) const {
      return Prob(E, dE) > lim;
    }
    TH2*      GetHist()    { return H;   }
    TProfile* GetProfile() { return h;   }
    double    GetLim()     { return lim; }
  };

  class PSelData {
    TH2* H;
    TF1* f;
    TGraph df;
    vector<TF1*> fits;
    vector<TH1*> hists;
    double lim, Emin, Emax;
  public:
    struct EvdEPoint {
      double E0, phi, Ep0, dEp0, sig;
      EvdEPoint(double E0, double phi, double Ep0, double dEp0, double sig) :
      E0(E0), phi(phi), Ep0(Ep0), dEp0(dEp0), sig(sig) {}
    };
  private:
    TF1* Fit(TH1* H, double x0) {
      double A = H->GetBinContent(H->FindFixBin(x0));
      double s = 100;
      TF1* fit = new TF1("fitfnc", "gaus(0)", x0-150, x0+150);
      fit->SetParLimits(1, x0-500, x0+500);
      fit->SetParLimits(2, 50., 200.);
      fit->SetParameters(A, x0, s);
      H  ->Fit(fit, "0RQ");
      return fit;
    }
    void GlobalFit(const vector<EvdEPoint>& ps) {
      double X[256], Y[256];
      for (int i = 0; i < ps.size(); ++i) {
        TVector2 x = TVector2(ps[i].Ep0, ps[i].dEp0).Rotate(-ps[i].phi);
        X[i] = x.X();
        Y[i] = x.Y();
      }
      TGraph g(ps.size(), X, Y);
      g.Fit(f, "0Q");
      // Develop sigma as a function of energy
      vector<double> e, s;
      for (int i = 0; i < ps.size(); ++i) {
        if (50. < ps[i].sig && ps[i].sig < 200.) {
          e.push_back(ps[i].E0);
          s.push_back(ps[i].sig);
        }
      }
      df = TGraph(e.size(), &e[0], &s[0]);
    }
    TVector2 NearestPoint(double X, double Y) const {
      TF1 dF("dF", "x-[2]+[1]*[0]*x^([1]-1)*([0]*x^[1]-[3])", Emin, Emax);
      dF.SetParameters(f->GetParameter(0), f->GetParameter(1), X, Y);
      double x = dF.GetX(0);
      return TVector2(x, f->Eval(x));
    }
    TVector2 NearestPoint(const TVector2& x) const {
      return NearestPoint(x.X(), x.Y());
    }
  public:
    PSelData() {}
    PSelData(const Particle* p, TH2* h, TF1* f, double lim, double step,
             double Ebnd[2]) :
    H(h), f(f), lim(lim), Emin(Ebnd[0]), Emax(Ebnd[1]) {
      f->SetRange(h->GetXaxis()->GetXmin(), h->GetXaxis()->GetXmax());
      TVector2 x(Emin, f->Eval(Emin));
      vector<EvdEPoint> evde;
      while (x.X() < Emax) {
        TVector2 v  = TVector2(1, f->Derivative(x.X())).Unit();
        TVector2 xp = x.Rotate(-v.Phi());
        TH1* hp     = HistUtils::ProjectionXY(h, -v.Phi(), xp.X(), step);
        char title[256], name[256];
        sprintf(title, "%s (%d keV);dEp (keV')" , hp->GetTitle(),  (int)x.X());
        sprintf(name,  "%s_%s_%d", hp->GetName(), p->Name.c_str(), (int)x.X());
        hp->SetTitle(title);
        hp->SetName(name);
        TF1* fit = Fit(hp, xp.Y());
        evde.push_back(EvdEPoint(x.X(), -v.Phi(), xp.X(),
                                 fit->GetParameter(1), fit->GetParameter(2)));
        hists.push_back(hp);
        fits .push_back(fit);
        x += v*step;
        x.Set(x.X(), f->Eval(x.X()));
      }
      GlobalFit(evde);
    }
    double Prob(double E, double dE) const {
      if (E < Emin || Emax < E)
        return 0;
      TVector2 X = NearestPoint(E, dE);
      // double   s = df.Eval(E, 0x0, "S"); // TSpline3 interpolation
      double   s = df.GetMean(2); // Average of Y (2nd) axis
      double   p = -TVector2(1, f->Derivative(X.X())).Phi();
      double   x = (TVector2(E, dE).Rotate(p).Y() - X.Rotate(p).Y())/s;
      double   A = 1./std::sqrt(2*TMath::Pi()*s*s);
      return A*std::exp(-0.5*x*x);
    }
    double operator()(double E, double dE) const { return Prob(E, dE);        }
    bool   IsParticle(double E, double dE) const { return Prob(E, dE) > lim;  }
    TH2*   GetHist()                       const { return H;                  }
    double GetLim()                        const { return lim;                }
    TF1*   GetFcn()                        const { return f;                  }
    vector<TH1*>& GetHists()                     { return hists;              }
    vector<TF1*>& GetFits()                      { return fits;               }
    TGraph        SigGraph()               const { return df;                 }
  };

  vector<PSelPow> LoadParticleLikelihoodsPow(char lr) {
    double L[4][3] = { {85e4, 15e5, -0.85}, {15e5, 22e5, -0.85},
                       {22e5, 30e5, -0.85}, {10e6, 16e6, -0.85} };
    double R[4][3] = { {12e5, 22e5, -0.9}, {22e5, 35e5, -0.9},
                       {35e5, 50e5, -0.9}, {21e6, 30e6, -0.9} };
    vector<PSelPow> pls;
    for (int i = 0; i < NPTYPE-1; ++i)
      if (lr == 'l' || lr == 'L')
        pls.push_back(PSelPow(L[i][0], L[i][1], L[i][2], 1.9e3, 17e3));
      else if (lr == 'r' || lr == 'R')
        pls.push_back(PSelPow(R[i][0], R[i][1], R[i][2], 1.9e3, 17e3));
      else
        PrintAndThrow("ParticleLikelihoodPow: Incorrect detector side!");
    return pls;
  }

  vector<PSelMZ2> LoadParticleLikelihoodsMZ2(char lr) {
    double L[4][2] = {
      {3.0e6, 5.5e6}, {5.5e6, 10.0e6}, {10e6,  15e6}, {40e6,  70e6}
    };
    double R[4][2] = {
      {2.8e6, 5.5e6}, {5.5e6, 8.5e6}, {8.5e6, 12e6}, {40e6, 70e6}
    };
    vector<PSelMZ2> pls;
    for (int i = 0; i < NPTYPE-1; ++i)
      if (lr == 'l' || lr == 'L')
        pls.push_back(PSelMZ2(L[i][0], L[i][1], 1.9e3, 17e3));
      else if (lr == 'r' || lr == 'R')
        pls.push_back(PSelMZ2(R[i][0], R[i][1], 1.9e3, 17e3));
      else
        PrintAndThrow("ParticleLikelihoodMZ2: Incorrect detector side!");
    return pls;
  }

  vector<PSelData> LoadParticleLikelihoodsData(char lr) {
    static TFile* ifile = new TFile(DATAIFNAME.c_str(), "READ");
    TH3* H3 = (TH3*)ifile->Get(lr == 'l' ? "evde_l0" : "evde_r0");
    H3->GetZaxis()->SetRangeUser(400., 100e3);
    TH2* H2 = (TH2*)H3->Project3D("yx");
    // H2->Rebin2D(5, 5);
    TF1* f[NPTYPE-1] = {
      new TF1("fpevde", "[0]*x^[1]"), new TF1("fdevde", "[0]*x^[1]"),
      new TF1("ftevde", "[0]*x^[1]"), new TF1("faevde", "[0]*x^[1]")
    };
    Double_t L_PAR[NPTYPE-1][2] = { {1.1e6, -0.85}, {1.8e6, -0.85},
                                    {2.5e6, -0.85}, {14.e6, -0.85} };
    Double_t R_PAR[NPTYPE-1][2] = { {3.6e6, -0.99}, {6.0e6, -0.99},
                                    {8.5e6, -0.99}, {57.e6, -0.99} };
    double   L_BND[NPTYPE-1][2] = { {2.2e3,  14e3}, {2.8e3,  18e3},
                                    {3e3,    18e3}, {6.5e3,  18e3} };
    double   R_BND[NPTYPE-1][2] = { {2.2e3,  20e3}, {2.8e3,  18e3},
                                    {3.4e3,  18e3}, {8.5e3,  18e3} };
    for (int i = 0; i < NPTYPE-1; ++i)
      f[i]->SetParameters(lr == 'l' ? L_PAR[i] : R_PAR[i]);
    vector<PSelData> pls;
    for (int i = 0; i < NPTYPE-1; ++i)
      pls.push_back(PSelData(PARTICLES[i], H2, f[i], 1e-4, 500.,
                             (lr=='l') ? L_BND[i] : R_BND[i]));
    return pls;
  }

  vector<PSelMC> LoadParticleLikelihoodsMC(char lr) {
    vector<PSelMC> pls;
    for (int i = 0; i < NPTYPE-1; ++i) {
      TFile f(MCIFNAMES[i]->c_str());
      char hname[64];
      sprintf(hname, "hEvdE_Si%c_stopped", std::toupper(lr));
      TH2F* H = (TH2F*)f.Get(hname);
      pls.push_back(PSelMC(H, 1e-4, EMIN[i], 30.));
    }
    return pls;
  }
}
