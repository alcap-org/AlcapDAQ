#include "./scripts/Al50/util.h"
#include "./scripts/TMETree/TMETreeBranches.h"
#include "./scripts/TMETree/TMEUtils.h"
#include "./src/plotters/SimplePulse.h"

#include "TChain.h"
#include "TF1.h"
#include "TFile.h"
#include "TGraph.h"
#include "TH1.h"
#include "TH2.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TH3.h"
#include "TH3F.h"
#include "TProfile.h"
#include "TTree.h"
#include "TVector2.h"

#include <algorithm>
#include <cctype>
#include <cmath>
#include <iostream>
#include <numeric>
#include <string>
#include <vector>

using SiUtils::SiEvent;

using std::string;
using std::vector;

////////////////////////////////////////////////////////////////////////////////
// USER MODIFIABLE VARIABLES
// Compile only: root -l -b -q R15b_Al50_psel.C+g
// Serially run on all runs: root -l -b -q R15b_Al50_psel.C+g\(-1\)
// Run on a single run #: root -l -b -q R15b_Al50_psel.C+g\(#\)
// Input data files should be TME tree files output from rootana.
// Input simulation (MC) files should be output from the
// script R15b_Al50_MC_EvdE.C.
static const char* IFNAMEFMT  = "~/R15bTME/Al50/tme%05d.root";
static const string DATAIFNAME("~/data/R15b/evdeal50.root");
static const string PFNAME("~/data/R15b/Proton_EvdE_MC.root");
static const string DFNAME("~/data/R15b/Deuteron_EvdE_MC.root");
static const string TFNAME("~/data/R15b/Triton_EvdE_MC.root");
static const string AFNAME("~/data/R15b/Alpha_EvdE_MC.root");
static const char* OFNAMEFMT = "~/data/R15b/psel%05d.root";
static const double EMIN_P   = 2e3;
static const double EMIN_D   = 2.5e3;
static const double EMIN_T   = 4e3;
static const double EMIN_A   = 10e3;
////////////////////////////////////////////////////////////////////////////////

// p, d, t, a, unclassified
static const int       NPTYPE       = 5;
static const int       NSIL         = 16;
static const int       NSIR         = 4;
static const string*   MCIFNAMES[4] = { &PFNAME, &DFNAME,   &TFNAME, &AFNAME };
static const Particle* PARTICLES[4] = { &PROTON, &DEUTERON, &TRITON, &ALPHA };
static const double    EMIN[4]      = { EMIN_P,  EMIN_D,    EMIN_T,  EMIN_A };

class ParticleLikelihoodData {
  Particle p;
  TH2* H;
  TF1* f;
  double df;
  vector<TF1*> fits;
  vector<TH1*> hists;
  double lim, Emin, Emax;
  vector<double> E0, Ep0, dEp0, sig, phi;
  TF1* Fit(TH1* H, double x0) {
    double A = H->GetBinContent(H->FindFixBin(x0));
    double s = 100;
    TF1* fit = new TF1("fitfnc", "gaus(0)", x0-150, x0+150);
    fit->SetParLimits(1, x0-500, x0+500);
    fit->SetParLimits(2, 50., 200.);
    fit->SetParameters(A, x0, s);
    H  ->Fit(fit, "0R");
    return fit;
  }
  int IndexOfNearestEnergy(double E) const {
    vector<double>::const_iterator j = std::lower_bound(E0.begin(), E0.end(),
                                                        E);
    vector<double>::const_iterator i = j-1;
    if ( j == E0.end() ||
        (j != E0.begin() && (E - *i <= *j - E))) return i - E0.begin();
    else                                         return j - E0.begin();
  }
  void GlobalFit() {
    double X[256], Y[256];
    for (int i = 0; i < E0.size(); ++i) {
      TVector2 x = TVector2(Ep0[i], dEp0[i]).Rotate(-phi[i]);
      X[i] = x.X();
      Y[i] = x.Y();
    }
    TGraph* g = new TGraph(E0.size(), X, Y);
    g->Fit(f, "0");
    // Calculate average sigma
    vector<double> s;
    for (int i = 0; i < sig.size(); ++i)
      if (50. < sig[i] && sig[i] < 200.)
        s.push_back(sig[i]);
    df = std::accumulate(s.begin(), s.end(), 0.)/s.size();
  }
  TVector2 NearestPoint(double x, double y) const {
    TF1 df("df", "x-[2]+[1]*[0]*x^([1]-1)*([0]*x^[1]-[3])", Emin, Emax);
    df.SetParameters(f->GetParameter(0), f->GetParameter(1), x, y);
    double X = df.GetX(0);
    return TVector2(X, f->Eval(X));
  }
  TVector2 NearestPoint(const TVector2& x) const {
    return NearestPoint(x.X(), x.Y());
  }
 public:
  ParticleLikelihoodData() {}
  ParticleLikelihoodData(const Particle* p, TH2* h, TF1* f, double lim,
                         double step, double Ebnd[2]) :
  p(*p), H(h), f(f), lim(lim), Emin(Ebnd[0]), Emax(Ebnd[1]) {
    f->SetRange(h->GetXaxis()->GetXmin(), h->GetXaxis()->GetXmax());
    TVector2 x(Emin, f->Eval(Emin));
    while (x.X() < Emax) {
      TVector2 v  = TVector2(1, f->Derivative(x.X())).Unit();
      TVector2 xp = x.Rotate(-v.Phi());
      TH1* hp     = HistUtils::ProjectionXY(h, -v.Phi(), xp.X(), step);
      char title[256], name[256];
      sprintf(title, "%s (%d keV);dEp (keV')" , hp->GetTitle(), (int)x.X());
      sprintf(name,  "%s_%s_%d", hp->GetName(), p->Name.c_str(), (int)x.X());
      hp->SetTitle(title);
      hp->SetName(name);
      TF1* fit = Fit(hp, xp.Y());
      E0   .push_back(x.X());
      Ep0  .push_back(xp.X());
      phi  .push_back(-v.Phi());
      dEp0 .push_back(fit->GetParameter(1));
      sig  .push_back(fit->GetParameter(2));
      hists.push_back(hp);
      fits .push_back(fit);
      x += v*step;
      x.Set(x.X(), f->Eval(x.X()));
    }
    GlobalFit();
  }
  double Prob(double dE, double E) const {
    if (E < Emin || Emax < E)
      return 0;
    TVector2 X = NearestPoint(E, dE);
    double   p = -TVector2(1, f->Derivative(X.X())).Phi();
    double   x = (TVector2(E, dE).Rotate(p).Y() - X.Rotate(p).Y())/df;
    double   A = 1./std::sqrt(2*TMath::Pi()*df*df);
    return A*std::exp(-0.5*x*x);
  }
  double operator()(double dE, double E) const { return Prob(dE, E);        }
  bool   IsParticle(double dE, double E) const { return Prob(dE, E) > lim;  }
  TH2*   GetHist()                             { return H;                  }
  double GetLim()                        const { return lim;                }
  TF1*   GetFcn()                              { return f;                  }
  vector<TH1*>& GetHists()                     { return hists;              }
  vector<TF1*>& GetFits()                      { return fits;               }
  TGraph* SigGraph() const { return new TGraph(E0.size(), &E0[0], &sig[0]); }
};

class ParticleLikelihoodMC {
  Particle p;
  TH2* H;
  TProfile* h;
  double lim, emin;
  double res;
public:
  ParticleLikelihoodMC() {}
  // Takes ownership of H
  ParticleLikelihoodMC(const Particle* p, TH2* H=0x0,
                     double lim=0., double emin=0., double res=0.) :
  p(*p), H(H), h(0x0), lim(lim), emin(emin), res(res) {
    h = H->ProfileX("_pfx", 1, -1, "S");
    H->SetDirectory(0);
    h->SetDirectory(0);
  }
  double Prob(double dE, double E) const {
    if (E < emin) return 0.;
    int b       = h->FindFixBin(E);
    double mu   = h->GetBinContent(b);
    double std  = h->GetBinError(b);
    double sig2 = res*res+std*std;
    double A    = 1./std::sqrt(2*TMath::Pi()*sig2);
    return A*std::exp(-0.5*(dE-mu)*(dE-mu)/sig2);
  }
  double operator()(double dE, double E) const {
    return Prob(dE, E);
  }
  bool IsParticle(double dE, double E) const {
    return Prob(dE, E) > lim;
  }
  TH2*      GetHist()    { return H;   }
  TProfile* GetProfile() { return h;   }
  double    GetLim()     { return lim; }
};

void BookEvdEHistograms(TFile* f, char lr, vector< vector<TH3*> >& hs) {
  const int NX = 2000, NY = 1000, NZ = 2;
  Double_t* X       = HistUtils::ConstructBins(NX, 0, 20e3);
  Double_t* Y       = HistUtils::ConstructBins(NY, 0, 10e3);
  Double_t  Z[NZ+1] = {-200, 400, 4500};
  TH1::SetDefaultSumw2(kTRUE);
  char hname[128], htitle[128];
  for (int i = 0; i < hs.size(); ++i) {
    for (int j = 0; j < hs[i].size()-1; ++j) {
      sprintf(hname,  "evde_%c%d_%s", lr, i, PARTICLES[j]->Name.c_str());
      sprintf(htitle, "E vs dE %c%d", std::toupper(lr), i);
      hs[i][j] = new TH3F(hname, htitle, NX, X, NY, Y, NZ, Z);
      hs[i][j]->SetDirectory(f);
    }
  }
  for (int i = 0; i < hs.size(); ++i) {
    sprintf(hname,  "evde_%c%d_unclassified", lr, i);
    sprintf(htitle, "E vs dE %c%d", std::toupper(lr), i);
    hs[i].back() = new TH3F(hname, htitle, NX, X, NY, Y, NZ, Z);
    hs[i].back()->SetDirectory(f);
  }
}
void BookTvEHistograms(TFile* f, char lr, vector< vector<TH2*> >& hs) {
  TH1::SetDefaultSumw2(kTRUE);
  char hname[128], htitle[128];
  for (int i = 0; i < hs.size(); ++i) {
    for (int j = 0; j < hs[i].size()-1; ++j) {
      sprintf(hname,  "tve_%c%d_%s", lr, i, PARTICLES[j]->Name.c_str());
      sprintf(htitle, "T vs E %c%d", std::toupper(lr), i);
      hs[i][j] = new TH2F(hname, htitle, 1000, -5e3, 5e3, 200, 0, 20e3);
      hs[i][j]->SetDirectory(f);
    }
  }
  for (int i = 0; i < hs.size(); ++i) {
    sprintf(hname,  "tve_%c%d_unclassified", lr, i);
    sprintf(htitle, "T vs E %c%d", std::toupper(lr), i);
    hs[i].back() = new TH2F(hname, htitle, 1000, -5e3, 5e3, 200, 0, 20e3);
    hs[i].back()->SetDirectory(f);
  }
}
void BookdTHistograms(TFile* f, char lr, vector< vector<TH1*> >& hs) {
  TH1::SetDefaultSumw2(kTRUE);
  char hname[128], htitle[128];
  for (int i = 0; i < hs.size(); ++i) {
    for (int j = 0; j < hs[i].size()-1; ++j) {
      sprintf(hname,  "dt_%c%d_%s", lr, i, PARTICLES[j]->Name.c_str());
      sprintf(htitle, "T_{thick}-T_{thin} %c%d", std::toupper(lr), i);
      hs[i][j] = new TH1F(hname, htitle, 1000, -5e3, 5e3);
      hs[i][j]->SetDirectory(f);
    }
  }
  for (int i = 0; i < hs.size(); ++i) {
    sprintf(hname,  "dt_%c%d_unclassified", lr, i);
    sprintf(htitle, "T_{thick} vs T_{thin} %c%d", std::toupper(lr), i);
    hs[i].back() = new TH1F(hname, htitle, 1000, -5e3, 5e3);
    hs[i].back()->SetDirectory(f);
  }
}

vector<ParticleLikelihoodData> LoadParticleLikelihoodsData(char lr) {
  static TFile* ifile = new TFile(DATAIFNAME.c_str(), "READ");
  TH3* H3 = (TH3*)ifile->Get(lr == 'l' ? "evde_l0" : "evde_r0");
  TH2* H2 = (TH2*)H3->Project3D("yx");
  H2->Rebin2D(5, 5);
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
  vector<ParticleLikelihoodData> pls;
  for (int i = 0; i < NPTYPE-1; ++i)
    pls.push_back(ParticleLikelihoodData(PARTICLES[i], H2, f[i], 1e-4, 500.,
                                         (lr=='l') ? L_BND[i] : R_BND[i]));
  return pls;
}

vector<ParticleLikelihoodMC> LoadParticleLikelihoodsMC(char lr) {
  vector<ParticleLikelihoodMC> pls;
  for (int i = 0; i < NPTYPE-1; ++i) {
    TFile f(MCIFNAMES[i]->c_str());
    char hname[64];
    sprintf(hname, "hEvdE_Si%c_stopped", std::toupper(lr));
    TH2F* H = (TH2F*)f.Get(hname);
    pls.push_back(ParticleLikelihoodMC(PARTICLES[i], H, 1e-4, EMIN[i], 30.));
  }
  return pls;
}

template <class T>
int WhichParticle(const vector<T>& pls, double dE, double E) {
  for (int i = 0; i < pls.size(); ++i)
    if (pls[i].IsParticle(dE, E))
      return i;
  return pls.size();
}

template <class T>
void CombineHistograms(vector< vector<T*> >& hs) {
  for (int i = 1; i < hs.size(); ++i)
    for (int j = 0; j < hs[i].size(); ++j)
      hs[0][j]->Add(hs[i][j]);
}

vector< vector<SimplePulse>* >& SiTs  = all_SiT_channels;
vector< vector<SimplePulse>* >& SiR1s = all_SiR1_channels;
vector< vector<SimplePulse>* >& SiL1s = all_SiL1_channels;
void psel(TTree* tr, const char* ofname, bool usealllayers=true,
          bool verbose=false) {
  TFile* ofile = new TFile(ofname, "RECREATE");
  SetTMEBranchAddresses(tr);
  CollectChannels();
  TMECal::Init();
  vector< vector<TH3*> > hrevde(NSIR+1, vector<TH3*>(NPTYPE));
  vector< vector<TH3*> > hlevde(NSIL+1, vector<TH3*>(NPTYPE));
  vector< vector<TH2*> > hrtve (NSIR+1, vector<TH2*>(NPTYPE));
  vector< vector<TH2*> > hltve (NSIL+1, vector<TH2*>(NPTYPE));
  vector< vector<TH1*> > hrdt  (NSIR+1, vector<TH1*>(NPTYPE));
  vector< vector<TH1*> > hldt  (NSIL+1, vector<TH1*>(NPTYPE));
  BookEvdEHistograms(ofile, 'r', hrevde);
  BookEvdEHistograms(ofile, 'l', hlevde);
  BookTvEHistograms (ofile, 'r', hrtve);
  BookTvEHistograms (ofile, 'l', hltve);
  BookdTHistograms  (ofile, 'r', hrdt);
  BookdTHistograms  (ofile, 'l', hldt);
  vector<ParticleLikelihoodData> pls_r = LoadParticleLikelihoodsData('r');
  vector<ParticleLikelihoodData> pls_l = LoadParticleLikelihoodsData('l');
  for (int i = 0; i < tr->GetEntries(); ++i) {
    tr->GetEntry(i);
    if (verbose && i % 10000 == 0)
      std::cout << i << "/" << tr->GetEntries() << std::endl;
    if (TMECuts::OnlyOneHit(SiR2)) {
      for (int j = 0; j < 4; ++j) {
        SiEvent ev(SiR1s[j], SiR2, (usealllayers ? SiR3 : nullptr), nullptr);
        if (ev.Valid()) {
          int k = WhichParticle(pls_r, ev.dE(), ev.E());
          hrevde[j+1][k]->Fill(ev.E(), ev.dE(), ev.T());
          hrtve [j+1][k]->Fill(ev.T(), ev.E());
          hrdt  [j+1][k]->Fill(ev.dT());
        }
      }
    }
    if (TMECuts::OnlyOneHit(SiL3)) {
      for (int j = 0; j < 14; ++j) {
        SiEvent ev(SiL1s[j], SiL3, nullptr, &TMECal::SiL1A2E[j+2]);
        if (ev.Valid()) {
          int k = WhichParticle(pls_l, ev.dE(), ev.E());
          hlevde[j+2][k]->Fill(ev.E(), ev.dE(), ev.T());
          hltve [j+2][k]->Fill(ev.T(), ev.E());
          hldt  [j+2][k]->Fill(ev.dT());
        }
      }
    }
  }
  CombineHistograms(hrevde);
  CombineHistograms(hlevde);
  CombineHistograms(hrtve);
  CombineHistograms(hltve);
  CombineHistograms(hrdt);
  CombineHistograms(hldt);
  ofile->Write();
}

// run = 0: compile only
// run < 0: serially run on all runs
// run is an Al50 run number: run on that single run in Al50 dataset
// otherwise: fail
void R15b_Al50_psel(int run=0, bool usealllayers=true, bool verbose=false) {
  char ifname[128], ofname[128];
  if (run == 0) {
    return;
  } else if (run < 0) {
    for (int i = 9890; i <= 10128; ++i)
      R15b_Al50_psel(i, usealllayers, verbose);
    return;
  } else if (9890 <= run && run <= 10128) {
    sprintf(ifname, IFNAMEFMT, run);
  } else {
    throw "Unrecognized run";
  }
  sprintf(ofname, OFNAMEFMT, run);
  TChain* ch = new TChain("TMETree/TMETree");
  ch->Add(ifname);
  psel(ch, ofname, usealllayers, verbose);
}
