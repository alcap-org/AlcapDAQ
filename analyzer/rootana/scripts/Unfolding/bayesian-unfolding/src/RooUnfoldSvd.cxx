//=====================================================================-*-C++-*-
// File and Version Information:
//      $Id$
//
// Description:
//      SVD unfolding. Just an interface to TSVDUnfold.
//
// Author: Tim Adye <T.J.Adye@rl.ac.uk>
//
//==============================================================================

//____________________________________________________________
/* BEGIN_HTML
<p>Links to TSVDUnfold class which unfolds using Singular Value Decomposition (SVD).</p>
<p>Regularisation parameter defines the level at which values are deemed to be due to statistical fluctuations and are cut out. (Default= number of bins/2)
<p>Returns errors as a full matrix of covariances
<p>Can only handle 1 dimensional distributions
<p>Can account for both smearing and biasing
END_HTML */

/////////////////////////////////////////////////////////////

#include "RooUnfoldSvd.h"

#include <iostream>
#include <iomanip>

#include "TClass.h"
#include "TNamed.h"
#include "TH1.h"
#include "TH2.h"
#include "TVectorD.h"
#include "TMatrixD.h"
#if defined(HAVE_TSVDUNFOLD) || ROOT_VERSION_CODE < ROOT_VERSION(5,34,99)
#include "TSVDUnfold_local.h"  /* Use local copy of TSVDUnfold.h */
#else
#include "TSVDUnfold.h"
#if ROOT_VERSION_CODE < ROOT_VERSION(5,34,0)
#define TSVDUNFOLD_LEAK 1
#endif
#endif

#include "RooUnfoldResponse.h"

using std::cout;
using std::cerr;
using std::endl;

ClassImp (RooUnfoldSvd);

RooUnfoldSvd::RooUnfoldSvd (const RooUnfoldSvd& rhs)
  : RooUnfold (rhs)
{
  // Copy constructor.
  Init();
  CopyData (rhs);
}

RooUnfoldSvd::RooUnfoldSvd (const RooUnfoldResponse* res, const TH1* meas, Int_t kreg,
                            const char* name, const char* title)
  : RooUnfold (res, meas, name, title), _kreg(kreg ? kreg : res->GetNbinsTruth()/2)
{
  // Constructor with response matrix object and measured unfolding input histogram.
  // The regularisation parameter is kreg.
  Init();
}

RooUnfoldSvd::RooUnfoldSvd (const RooUnfoldResponse* res, const TH1* meas, Int_t kreg, Int_t ntoyssvd,
                            const char* name, const char* title)
  : RooUnfold (res, meas, name, title), _kreg(kreg ? kreg : res->GetNbinsTruth()/2)
{
  // Constructor with old ntoyssvd argument. No longer required.
  Init();
  _NToys = ntoyssvd;
}

RooUnfoldSvd*
RooUnfoldSvd::Clone (const char* newname) const
{
  RooUnfoldSvd* unfold= new RooUnfoldSvd(*this);
  if (newname && strlen(newname)) unfold->SetName(newname);
  return unfold;
}

void
RooUnfoldSvd::Reset()
{
  Destroy();
  Init();
  RooUnfold::Reset();
}

void
RooUnfoldSvd::Destroy()
{
  delete _svd;
  delete _meas1d;
#ifdef TSVDUNFOLD_LEAK
  delete _meascov;
#endif
  delete _train1d;
  delete _truth1d;
  delete _reshist;
}

void
RooUnfoldSvd::Init()
{
  _svd= 0;
  _meas1d= _train1d= _truth1d= 0;
  _reshist= _meascov= 0;
  GetSettings();
}

void
RooUnfoldSvd::Assign (const RooUnfoldSvd& rhs)
{
  RooUnfold::Assign (rhs);
  CopyData (rhs);
}

void
RooUnfoldSvd::CopyData (const RooUnfoldSvd& rhs)
{
  _kreg= rhs._kreg;
}

TSVDUnfold*
RooUnfoldSvd::Impl()
{
  return _svd;
}

void
RooUnfoldSvd::Unfold()
{
  if (_res->GetDimensionTruth() != 1 || _res->GetDimensionMeasured() != 1) {
    cerr << "RooUnfoldSvd may not work very well for multi-dimensional distributions" << endl;
  }
  if (_kreg < 0) {
    cerr << "RooUnfoldSvd invalid kreg: " << _kreg << endl;
    return;
  }

  _nb= _nm > _nt ? _nm : _nt;

  if (_kreg > _nb) {
    cerr << "RooUnfoldSvd invalid kreg=" << _kreg << " with " << _nb << " bins" << endl;
    return;
  }

  Bool_t oldstat= TH1::AddDirectoryStatus();
  TH1::AddDirectory (kFALSE);
  _meas1d=  HistNoOverflow (_meas,             _overflow);
  _train1d= HistNoOverflow (_res->Hmeasured(), _overflow);
  _truth1d= HistNoOverflow (_res->Htruth(),    _overflow);
  _reshist= _res->HresponseNoOverflow();
  Resize (_meas1d,  _nb);
  Resize (_train1d, _nb);
  Resize (_truth1d, _nb);
  Resize (_reshist, _nb, _nb);

  // Subtract fakes from measured distribution
  if (_res->FakeEntries()) {
    TVectorD fakes= _res->Vfakes();
    Double_t fac= _res->Vmeasured().Sum();
    if (fac!=0.0) fac=  Vmeasured().Sum() / fac;
    if (_verbose>=1) cout << "Subtract " << fac*fakes.Sum() << " fakes from measured distribution" << endl;
    for (Int_t i= 1; i<=_nm; i++)
      _meas1d->SetBinContent (i, _meas1d->GetBinContent(i)-(fac*fakes[i-1]));
  }

  _meascov= new TH2D ("meascov", "meascov", _nb, 0.0, 1.0, _nb, 0.0, 1.0);
  const TMatrixD& cov= GetMeasuredCov();
  for (Int_t i= 0; i<_nm; i++)
    for (Int_t j= 0; j<_nm; j++)
      _meascov->SetBinContent (i+1, j+1, cov(i,j));

  if (_verbose>=1) cout << "SVD init " << _reshist->GetNbinsX() << " x " << _reshist->GetNbinsY()
                        << " bins, kreg=" << _kreg << endl;
  _svd= new TSVDUnfold (_meas1d, _meascov, _train1d, _truth1d, _reshist);

  TH1D* rechist= _svd->Unfold (_kreg);

  _rec.ResizeTo (_nt);
  for (Int_t i= 0; i<_nt; i++) {
    _rec[i]= rechist->GetBinContent(i+1);
  }

  if (_verbose>=2) {
    PrintTable (cout, _truth1d, _train1d, 0, _meas1d, rechist, _nb, _nb, kFALSE, kErrors);
    TMatrixD* resmat= RooUnfoldResponse::H2M (_reshist, _nb, _nb);
    RooUnfoldResponse::PrintMatrix(*resmat,"TSVDUnfold response matrix");
    delete resmat;
  }

  delete rechist;
  TH1::AddDirectory (oldstat);

  _unfolded= true;
  _haveCov=  false;
}

void
RooUnfoldSvd::GetCov()
{
  if (!_svd) return;
  Bool_t oldstat= TH1::AddDirectoryStatus();
  TH1::AddDirectory (kFALSE);

  TH2D *unfoldedCov= 0, *adetCov= 0;
  //Get the covariance matrix for statistical uncertainties on the measured distribution
  if (_dosys!=2) unfoldedCov= _svd->GetXtau();
  //Get the covariance matrix for statistical uncertainties on the response matrix
  //Uses Poisson or Gaussian-distributed toys, depending on response matrix histogram's Sumw2 setting.
  if (_dosys)        adetCov= _svd->GetAdetCovMatrix (_NToys);

  _cov.ResizeTo (_nt, _nt);
  for (Int_t i= 0; i<_nt; i++) {
    for (Int_t j= 0; j<_nt; j++) {
      Double_t v  = 0;
      if (unfoldedCov) v  = unfoldedCov->GetBinContent(i+1,j+1);
      if (adetCov)     v += adetCov    ->GetBinContent(i+1,j+1);
      _cov(i,j)= v;
    }
  }

  delete adetCov;
#ifdef TSVDUNFOLD_LEAK
  delete unfoldedCov;
#endif
  TH1::AddDirectory (oldstat);

  _haveCov= true;
}

void RooUnfoldSvd::GetWgt()
{
  // Get weight matrix
  if (_dosys) RooUnfold::GetWgt();   // can't add sys errors to weight, so calculate weight from covariance
  if (!_svd) return;
  Bool_t oldstat= TH1::AddDirectoryStatus();
  TH1::AddDirectory (kFALSE);

  //Get the covariance matrix for statistical uncertainties on the measured distribution
  TH2D* unfoldedWgt= _svd->GetXinv();

  _wgt.ResizeTo (_nt, _nt);
  for (Int_t i= 0; i<_nt; i++) {
    for (Int_t j= 0; j<_nt; j++) {
      _wgt(i,j)= unfoldedWgt->GetBinContent(i+1,j+1);
    }
  }

#ifdef TSVDUNFOLD_LEAK
  delete unfoldedWgt;
#endif
  TH1::AddDirectory (oldstat);

  _haveWgt= true;
}

void RooUnfoldSvd::GetSettings(){
    _minparm=0;
    _maxparm= _meas ? _meas->GetNbinsX() : 0;
    _stepsizeparm=1;
    _defaultparm=_maxparm/2;
}

void RooUnfoldSvd::Streamer (TBuffer &R__b)
{
  // Stream an object of class RooUnfoldSvd.
  if (R__b.IsReading()) {
    // Don't add our histograms to the currect directory.
    // We own them and we don't want them to disappear when the file is closed.
    Bool_t oldstat= TH1::AddDirectoryStatus();
    TH1::AddDirectory (kFALSE);
    RooUnfoldSvd::Class()->ReadBuffer  (R__b, this);
    TH1::AddDirectory (oldstat);
  } else {
    RooUnfoldSvd::Class()->WriteBuffer (R__b, this);
  }
}
