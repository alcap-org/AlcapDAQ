//=====================================================================-*-C++-*-
// File and Version Information:
//      $Id$
//
// Description:
//      Unfolding interface to Fortran routine, BAYES, by G. D'Agostini from
//      http://www.roma1.infn.it/~dagos/bayes_distr.txt
//
// Authors: Tim Adye <T.J.Adye@rl.ac.uk>
//
//==============================================================================

//____________________________________________________________
/* BEGIN_HTML
<p>Unfolding interface to Fortran routine, BAYES, by G. D'Agostini from
http://www.roma1.infn.it/~dagos/bayes_distr.txt .
The Makefile only compiles this class if bayes.for and bayes_c.for are placed in
the src directory.
END_HTML */

/////////////////////////////////////////////////////////////

#include "RooUnfoldDagostini.h"

#include <iostream>

#include "TH1.h"
#include "TH2.h"
#include "TVectorD.h"
#include "TMatrixD.h"

#include "RooUnfoldResponse.h"

using std::cout;
using std::cerr;
using std::endl;

// SIZE_C and SIZE_E must agree exactly with PARAMETERs in bayes.for.
#define SIZE_C 100
#define SIZE_E 100

extern "C" {
  // bayes_ arguments must agree with the BAYES subroutine in bayes.for.
  extern void bayes_ (const int* noc, const int* noe, int* nsteps, const int* mode, const int* er_mode, const int* keep, int* ier);
  // bayesc_ must agree with the /BAYESC/ common block in bayes_c.for.
  extern struct {
    float pec[SIZE_C][SIZE_E], pce[SIZE_E][SIZE_C], 
          pc[SIZE_C], ne[SIZE_E], nc[SIZE_C], 
          nec_mc[SIZE_C][SIZE_E], nc_mc[SIZE_C], 
          Vc[SIZE_C][SIZE_C], eff[SIZE_C], eff_0, eff_true,
          Vc0[SIZE_C][SIZE_C], Vc1[SIZE_C][SIZE_C],
          M_unf[SIZE_E][SIZE_C], Vc_u[SIZE_C][SIZE_C],
          Vc0_u[SIZE_C][SIZE_C], Vpc[SIZE_C][SIZE_C],
          Vpc0[SIZE_C][SIZE_C], Vpc1[SIZE_C][SIZE_C];
  } bayesc_;
}


ClassImp (RooUnfoldDagostini);

RooUnfoldDagostini::RooUnfoldDagostini (const RooUnfoldDagostini& rhs)
  : RooUnfold (rhs)
{
  // Copy constructor.
  Init();
  _niter= rhs._niter;
}

RooUnfoldDagostini::RooUnfoldDagostini (const RooUnfoldResponse* res, const TH1* meas,
                            Int_t niter, const char* name, const char* title)
  : RooUnfold (res, meas, name, title), _niter(niter)
{
  // Constructor with response matrix object and measured unfolding input histogram.
  // The regularisation parameter is niter (number of iterations).
  Init();
}

RooUnfoldDagostini*
RooUnfoldDagostini::Clone (const char* newname) const
{
  RooUnfoldDagostini* unfold= new RooUnfoldDagostini(*this);
  if (newname && strlen(newname)) unfold->SetName(newname);
  return unfold;
}


RooUnfoldDagostini::~RooUnfoldDagostini()
{
}

void
RooUnfoldDagostini::Init()
{
  GetSettings();
}

void
RooUnfoldDagostini::Reset()
{
  Init();
  RooUnfold::Reset();
}

void
RooUnfoldDagostini::Assign (const RooUnfoldDagostini& rhs)
{
  RooUnfold::Assign (rhs);
  _niter= rhs._niter;
}

void
RooUnfoldDagostini::Unfold()
{
  Int_t nt= _nt, nm= _nm;
  if (nt > SIZE_C) {
    cerr << nt << " truth bins is too large - maximum is "  << SIZE_C << endl;
    nt= SIZE_C;
  }
  if (nm > SIZE_E) {
    cerr << nm << " measured bins is too large - maximum is "  << SIZE_E << endl;
    nm= SIZE_E;
  }
  if (_haveCovMes) cerr << "Warning: BAYES does not account for bin-bin correlations on measured input" << endl;
  
  const TMatrixD& res= _res->Mresponse();
  const TVectorD& tru= _res->Vtruth();
  const TVectorD& meas= Vmeasured();
  Double_t ntrue= 0;
  for (Int_t i= 0; i < nt; i++) {
    bayesc_.pc[i]=    tru(i);
    bayesc_.nc_mc[i]= tru(i);
    ntrue +=  tru(i);
    for (Int_t j= 0; j < nm; j++)
      bayesc_.nec_mc[i][j]= tru(i) * res(j,i);
  }

  Int_t ntf= nt;
  if (_res->FakeEntries()) {
    TVectorD fakes= _res->Vfakes();
    Double_t nfakes= fakes.Sum();
    if (_verbose>=1) cout << "Add truth bin for " << nfakes << " fakes" << endl;
    ntf++;
    ntrue +=              nfakes;
    bayesc_.pc[ntf-1]=    nfakes;
    bayesc_.nc_mc[ntf-1]= nfakes;
    for (Int_t j= 0; j < nm; j++)
      bayesc_.nec_mc[ntf-1][j]= fakes[j];
  }

  for (Int_t i= 0; i < ntf; i++)
    bayesc_.pc[i] /= ntrue;
  for (Int_t j= 0; j < nm; j++)
    bayesc_.ne[j]= meas(j);

  int mode= 2, er_mode= 20, keep= 0, ier= 0;
  bayes_ (&ntf, &nm, &_niter, &mode, &er_mode, &keep, &ier);
  if (ier != 0) cerr << "BAYES returned error " << ier << endl;

  _rec.ResizeTo (_nt);
  for (Int_t i= 0; i < nt; i++)
    _rec(i)= bayesc_.nc[i];

  _unfolded= true;
  _haveCov=  false;
}

void
RooUnfoldDagostini::GetCov()
{
  Int_t nt= _nt < SIZE_C ? _nt : SIZE_C;
  _cov.ResizeTo(_nt,_nt);
  for (Int_t i= 0; i < nt; i++)
    for (Int_t j= 0; j < nt; j++)
      _cov(i,j)= bayesc_.Vc0_u[j][i];
  _haveCov= true;
}

void
RooUnfoldDagostini::GetSettings(){
  _minparm=1;
  _maxparm=15;
  _stepsizeparm=1;
  _defaultparm=4;
}
