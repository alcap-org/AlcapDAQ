//=====================================================================-*-C++-*-
// File and Version Information:
//      $Id$
//
// Description:
//      Unfolding framework base class.
//
// Author: Tim Adye <T.J.Adye@rl.ac.uk>
//
//==============================================================================

//____________________________________________________________
/* BEGIN_HTML
<p>A base class for several unfolding methods.
<p>The unfolding method can either use the constructors for individual unfolding algorithms or the New() method, specifiying the algorithm to be used.
<p>The resultant distribution can be displayed as a plot (Hreco) or as a bin by bin breakdown of the true, measured and reconstructed values (PrintTable)
<p>A covariance matrix can be returned using the Ereco() method. A vector of its diagonals can be returned with the ErecoV() method.
<p>A summary of the unfolding algorithms which inherit from this class is below:
<ul>
<li>RooUnfoldBayes: Uses the Bayes method of unfolding based on the method written by D'Agostini (<a href="http://www.slac.stanford.edu/spires/find/hep/www?j=NUIMA,A362,487">NIM A 362 (1995) 487</a>).
<ul>
<li>Works for 1 & 2 dimensional distributions
<li>Returned errors can be either as a diagonal matrix or as a full matrix of covariances
<li>Regularisation parameter sets the number of iterations used in the unfolding (default=4)
<li>Is able to account for bin migration and smearing
<li>Can unfold if test and measured distributions have different binning.
<li>Returns covariance matrices with conditions approximately that of the machine precision. This occasionally leads to very large chi squared values
</ul>
<li> RooUnfoldSVD: Uses the singular value decomposition method of Hocker and Kartvelishvili (<a href="http://arxiv.org/abs/hep-ph/9509307">NIM A 372 (1996) 469</a>)
<ul>
<li>Regularisation parameter defines the level at which values are deemed to be due to statistical fluctuations and are cut out. (Default= number of bins/2)
<li>Returns errors as a full matrix of covariances
<li>Error processing is much the same as with the kCovToy setting with 1000 toys. This is quite slow but can be switched off.
<li>Can only handle 1 dimensional distributions
<li>True and measured distributions must have the same binning
<li>Can account for both smearing and biasing
<li>Returns near singular covariance matrices, again leading to very large chi squared values
</ul>
<li> RooUnfoldIds: Uses the Bayes method of unfolding based on the method written by Malaescu (<a href="http://arxiv.org/abs/1106.3107">CERN-PH-EP-2011-111</a>)
<ul>
<li>Set the number of iterations used to improve the folding matrix
<li>Regularisation parameters define the level at which values are deemed to be due to statistical fluctuations. Used for modifying the folding matrix, as well as unfolding.
<li>Returns errors as a full matrix of covariances
<li>Error processing is much the same as with the kCovToy setting with 1000 toys. This is quite slow but can be switched off.
<li>Can handle 2 dimensional distributions
<li>True and measured distributions must have the same binning
<li>Can account for both smearing and biasing
</ul>
<li> RooUnfoldBinByBin: Unfolds using the method of correction factors.
<ul>
<li>Returns errors as a diagonal matrix.
<li>Is not able to handle bin migration caused by bias/smearing of the distribution
<li>Can only handle 1 dimensional distributions
<li>True and measured distributions must have the same binning
</ul>
<li> RooUnfoldTUnfold: Uses the unfolding method implemented in ROOT's <a href="http://root.cern.ch/root/html/TUnfold.html">TUnfold</a> class
<ul>
<li>Only included in ROOT versions 5.22 and higher
<li>Only able to reconstruct 1 dimensional distributions
<li>Can account for bin migration and smearing
<li>Errors come as a full covariance matrix.
<li>Will sometimes warn of "unlinked" bins. These are bins with 0 entries and do not effect the results of the unfolding
<li>Regularisation parameter can be either optimised internally by plotting log10(chi2 squared) against log10(tau). The 'kink' in this curve is deemed the optimum tau value. This value can also be set manually (FixTau)
<li>The latest version (TUnfold v15) requires that RooUnfoldResponse::SetOverflow=0. ROOT versions 5.26 or below use v13 and so should be safe to use overflows
</ul>
<li> RooUnfoldInvert: The simplest method of unfolding works by simply inverting the response matrix.
<ul>
<li>For small statistics, this method does not produce useful results.
<li>The inversion method is included largely to illustrate the necessity of a more effective method of unfolding</ul>
</ul>
END_HTML */

/////////////////////////////////////////////////////////////

#include "RooUnfold.h"

#include <iostream>
#include <iomanip>
#include <sstream>
#include <cmath>
#include <vector>

#include "TClass.h"
#include "TMatrixD.h"
#include "TNamed.h"
#include "TBuffer.h"
#include "TH1.h"
#include "TH2.h"
#include "TH3.h"
#include "TVectorD.h"
#include "TDecompSVD.h"
#include "TDecompChol.h"
#include "TRandom.h"
#include "TMath.h"

#include "RooUnfoldResponse.h"
#include "RooUnfoldErrors.h"
// Need subclasses just for RooUnfold::New()
#include "RooUnfoldBayes.h"
#include "RooUnfoldSvd.h"
#include "RooUnfoldInvert.h"
#include "RooUnfoldBinByBin.h"
#ifndef NOTUNFOLD
#include "RooUnfoldTUnfold.h"
#endif
#ifdef HAVE_DAGOSTINI
#include "RooUnfoldDagostini.h"
#endif
#include "RooUnfoldIds.h"

using std::vector;
using std::cout;
using std::cerr;
using std::endl;
using std::setw;
using std::setprecision;
using std::sqrt;
using std::fabs;

ClassImp (RooUnfold);

RooUnfold::RooUnfold (const RooUnfoldResponse* res, const TH1* meas, const char* name, const char* title)
  : TNamed (name, title)
{
  // Constructor with response matrix object and measured unfolding input histogram.
  // Should not normally be used directly - instead, create an instance of one of RooUnfold's subclasses,
  // or use the New() static constructor.
  Init();
  Setup (res, meas);
}

RooUnfold* RooUnfold::New (Algorithm alg, const RooUnfoldResponse* res, const TH1* meas,Double_t regparm,
                           const char* name, const char* title)
{
    /*Unfolds according to the value of the alg enum:
    0 = kNone:     dummy unfolding
    1 = kBayes:    Unfold via iterative application of Bayes theorem
    2 = kSVD:      Unfold using singlar value decomposition (SVD)
    3 = kBinByBin: Unfold bin by bin.
    4 = kTUnfold:  Unfold with TUnfold
    5 = kInvert:   Unfold using inversion of response matrix
    7 = kIDS:      Unfold using iterative dynamically stabilized (IDS) method
    */
  RooUnfold* unfold;
  switch (alg) {
    case kNone:
      unfold= new RooUnfold         (res, meas);
      break;
    case kBayes:
      unfold= new RooUnfoldBayes    (res, meas);
      break;
    case kSVD:
      unfold= new RooUnfoldSvd      (res, meas);
      break;
    case kBinByBin:
      unfold= new RooUnfoldBinByBin (res, meas);
      break;
    case kTUnfold:
#ifndef NOTUNFOLD
      unfold= new RooUnfoldTUnfold  (res,meas);
      break;
#else
      cerr << "TUnfold library is not available" << endl;
      return 0;
#endif
    case kInvert:
      unfold = new RooUnfoldInvert  (res,meas);
      break;
    case kDagostini:
#ifdef HAVE_DAGOSTINI
      unfold = new RooUnfoldDagostini (res,meas);
      break;
#else
      cerr << "RooUnfoldDagostini is not available" << endl;
      return 0;
#endif
    case kIDS:
      unfold= new RooUnfoldIds      (res, meas);
      break;
    default:
      cerr << "Unknown RooUnfold method " << Int_t(alg) << endl;
      return 0;
  }
  if (name)  unfold->SetName  (name);
  if (title) unfold->SetTitle (title);
  if (regparm != -1e30){
    unfold->SetRegParm(regparm);
  }
  return unfold;
}

RooUnfold* RooUnfold::Clone (const char* newname) const
{
  // Creates a copy of the unfold object
  RooUnfold* unfold= new RooUnfold(*this);
  if (newname) unfold->SetName(newname);
  return unfold;
}

void RooUnfold::Destroy()
{
  delete _measmine;
  delete _vMes;
  delete _eMes;
  delete _covMes;
  delete _covL;
  delete _resmine;
}

RooUnfold::RooUnfold (const RooUnfold& rhs)
  : TNamed (rhs.GetName(), rhs.GetTitle())
{
  // Copy constructor.
  Init();
  CopyData (rhs);
}

void RooUnfold::Assign (const RooUnfold& rhs)
{
  if (this == &rhs) return;
  Reset();
  SetNameTitle (rhs.GetName(), rhs.GetTitle());
  CopyData (rhs);
}

void RooUnfold::CopyData (const RooUnfold& rhs)
{
  Setup (rhs.response(), rhs.Hmeasured());
  SetVerbose (rhs.verbose());
  SetNToys   (rhs.NToys());
}

void RooUnfold::Reset()
{
  Destroy();
  Init();
}

void RooUnfold::Init()
{
  _res= _resmine= 0;
  _vMes= _eMes= 0;
  _covMes= _covL= 0;
  _meas= _measmine= 0;
  _nm= _nt= 0;
  _verbose= 1;
  _overflow= 0;
  _dosys= _unfolded= _haveCov= _haveCovMes= _fail= _have_err_mat= _haveErrors= _haveWgt= false;
  _withError= kDefault;
  _NToys=50;
  GetSettings();
}

RooUnfold& RooUnfold::Setup (const RooUnfoldResponse* res, const TH1* meas)
{
  Reset();
  SetResponse (res);
  SetMeasured (meas);
  return *this;
}

void RooUnfold::SetMeasured (const TH1* meas)
{
  // Set measured distribution and errors. RooUnfold does not own the histogram.
  _meas= meas;
  delete _vMes; _vMes= 0;
  delete _eMes; _eMes= 0;
}

void RooUnfold::SetMeasured (const TVectorD& meas, const TVectorD& err)
{
  // Set measured distribution and errors. Should be called after setting response matrix.
  if (!_measmine) {
    Bool_t oldstat= TH1::AddDirectoryStatus();
    TH1::AddDirectory (kFALSE);
    _measmine= (TH1*) _res->Hmeasured()->Clone (GetName());
    TH1::AddDirectory (oldstat);
    _measmine->Reset();
    _measmine->SetTitle (GetTitle());
  }
  for (Int_t i= 0; i<_nm; i++) {
    Int_t j= RooUnfoldResponse::GetBin (_measmine, i, _overflow);
    _measmine->SetBinContent(j, meas[i]);
    _measmine->SetBinError  (j, err [i]);
  }
  SetMeasured (_measmine);
}

void RooUnfold::SetMeasured (const TVectorD& meas, const TMatrixD& cov)
{
  // Set measured distribution and its covariance matrix. Should be called after setting response matrix.
  SetMeasuredCov (cov);
  SetMeasured (meas, Emeasured());
}

void RooUnfold::SetMeasuredCov (const TMatrixD& cov)
{
  // Set covariance matrix on measured distribution.
  delete _covL; _covL= 0;
  delete _eMes;
  delete _covMes;
  _eMes= new TVectorD(_nm);
  for (Int_t i= 0; i<_nm; i++) {
    Double_t e= cov(i,i);
    if (e>0.0) (*_eMes)[i]= sqrt(e);
  }
  _covMes= new TMatrixD (cov);
  _haveCovMes= true;
}

const TMatrixD& RooUnfold::GetMeasuredCov() const
{
  // Get covariance matrix on measured distribution.
  if (_covMes) return *_covMes;
  const TVectorD& err= Emeasured();
  _covMes= new TMatrixD (_nm,_nm);
  for (Int_t i= 0 ; i<_nm; i++) {
    Double_t e= err[i];
    (*_covMes)(i,i)= e*e;
  }
  return *_covMes;
}


void RooUnfold::SetResponse (const RooUnfoldResponse* res)
{
  // Set response matrix for unfolding.
  delete _resmine; _resmine= 0;
  _res= res;
  _overflow= _res->UseOverflowStatus() ? 1 : 0;
  _nm= _res->GetNbinsMeasured();
  _nt= _res->GetNbinsTruth();
  if (_overflow) {
    _nm += 2;
    _nt += 2;
  }
  SetNameTitleDefault();
}

void RooUnfold::SetResponse (RooUnfoldResponse* res, Bool_t takeOwnership)
{
  // Set response matrix for unfolding, optionally taking ownership of the RooUnfoldResponse object
  SetResponse (res);
  if (takeOwnership) _resmine= res;
}

void RooUnfold::Unfold()
{
  // Dummy unfolding - just copies input
  cout << "********************** " << ClassName() << ": dummy unfolding - just copy input **********************" << endl;
  _rec.ResizeTo (_nt);
  Int_t nb= _nm < _nt ? _nm : _nt;
  for (Int_t i= 0; i < nb; i++) {
    _rec(i)= Vmeasured()(i);
  }
  _unfolded= true;
}

void RooUnfold::GetErrors()
{
    //Creates vector of diagonals of covariance matrices.
    //This may be overridden if it can be computed more quickly without the covariance matrix.
    if (!_haveCov) GetCov();
    if (!_haveCov) return;
    _variances.ResizeTo(_nt);
    for (Int_t i= 0; i < _nt; i++) {
      _variances(i)= _cov(i,i);
    }
    _haveErrors= true;
}

void RooUnfold::GetCov()
{
    //Dummy routine to get covariance matrix. It should be overridden by derived classes.
  const TMatrixD& covmeas= GetMeasuredCov();
  Int_t nb= _nm < _nt ? _nm : _nt;
  _cov.ResizeTo (_nt, _nt);
  for (int i=0; i<nb; i++)
    for (int j=0; j<nb; j++)
      _cov(i,j)= covmeas(i,j);
  _haveCov= true;
}

void RooUnfold::GetWgt()
{
  // Creates weight matrix
  // This may be overridden if it can be computed directly without the need for inverting the matrix
  if (!_haveCov) GetCov();
  if (!_haveCov) return;
  if (!InvertMatrix (_cov, _wgt, "covariance matrix", _verbose)) return;
  _haveWgt= true;
}

void RooUnfold::GetErrMat()
{
  // Get covariance matrix from the variation of the results in toy MC tests
  if (_NToys<=1) return;
  _err_mat.ResizeTo(_nt,_nt);
  TVectorD xisum (_nt);
  TMatrixD xijsum(_nt,_nt);
  for (Int_t k=0; k<_NToys; k++){
    RooUnfold* unfold= RunToy();
    const TVectorD& x= unfold->Vreco();
    for (Int_t i=0; i<_nt;i++){
      Double_t xi= x[i];
      xisum[i] += xi;
      for (Int_t j=0; j<_nt; j++) xijsum(i,j) += xi * x[j];
    }
    delete unfold;
  }
  for (Int_t i=0; i<_nt; i++){
    for (Int_t j=0; j<_nt; j++){
      _err_mat(i,j)= (xijsum(i,j) - (xisum[i]*xisum[j])/_NToys) / (_NToys-1);
    }
  }
  _have_err_mat=true;
}

Bool_t RooUnfold::UnfoldWithErrors (ErrorTreatment withError, bool getWeights)
{
  if (!_unfolded) {
    if (_fail) return false;
    const TH1* rmeas= _res->Hmeasured();
    if (_meas->GetDimension() != rmeas->GetDimension() ||
        _meas->GetNbinsX()    != rmeas->GetNbinsX()    ||
        _meas->GetNbinsY()    != rmeas->GetNbinsY()    ||
        _meas->GetNbinsZ()    != rmeas->GetNbinsZ()) {
      cerr << "Warning: measured "              << _meas->GetNbinsX();
      if (_meas->GetDimension()>=2) cerr << "x" << _meas->GetNbinsY();
      if (_meas->GetDimension()>=3) cerr << "x" << _meas->GetNbinsZ();
      cerr << "-bin histogram does not match "  << rmeas->GetNbinsX();
      if (rmeas->GetDimension()>=2) cerr << "x" << rmeas->GetNbinsY();
      if (rmeas->GetDimension()>=3) cerr << "x" << rmeas->GetNbinsZ();
      cerr << "-bin measured histogram from RooUnfoldResponse" << endl;
    }
    Unfold();
    if (!_unfolded) {
      _fail= true;
      return false;
    }
  }
  Bool_t ok;
  _withError= withError;
  if (getWeights && (withError==kErrors || withError==kCovariance)) {
      if   (!_haveWgt)      GetWgt();
      ok= _haveWgt;
  } else {
    switch (withError) {
    case kErrors:
      if   (!_haveErrors)   GetErrors();
      ok= _haveErrors;
      break;
    case kCovariance:
      if   (!_haveCov)      GetCov();
      ok= _haveCov;
      break;
    case kCovToy:
      if   (!_have_err_mat) GetErrMat();
      ok= _have_err_mat;
      break;
    default:
      ok= true;
    }
  }
  if (!ok) _fail= true;
  return ok;
}

Double_t RooUnfold::Chi2(const TH1* hTrue,ErrorTreatment DoChi2)
{
    /*Calculates Chi squared. Method depends on value of DoChi2
    0: sum of (residuals/error)squared
    1: use errors propagated through the unfolding
    2: use covariance matrix returned from unfolding
    3: use covariance matrix from the variation of the results in toy MC tests
    Returns warnings for small determinants of covariance matrices and if the condition is very large.
    If a matrix has to be inverted also removes rows/cols with all their elements equal to 0*/

    if (!UnfoldWithErrors (DoChi2)) return -1.0;

    TVectorD res(_nt);
    for (Int_t i = 0 ; i < _nt; i++) {
      Int_t it= RooUnfoldResponse::GetBin (hTrue, i, _overflow);
      if (hTrue->GetBinContent(it)!=0.0 || hTrue->GetBinError(it)>0.0) {
        res[i] = _rec[i] - hTrue->GetBinContent(it);
      }
    }

    Double_t chi2= 0.0;
    if (DoChi2==kCovariance || DoChi2==kCovToy) {
        TMatrixD wgt= Wreco(DoChi2);
        if (_fail) return -1.0;
        TMatrixD resmat(1,_nt), chi2mat(1,1);
        TMatrixDRow(resmat,0)= res;
        ABAT (resmat, wgt, chi2mat);
        chi2= chi2mat(0,0);
    } else {
        TVectorD ereco= ErecoV(DoChi2);
        if (_fail) return -1.0;
        for (Int_t i = 0 ; i < _nt; i++) {
          Double_t e= ereco[i];
          if (e<=0.0) continue;
          Double_t ypull = res[i] / e;
          chi2 += ypull*ypull;
        }
    }
    return chi2;
}


void RooUnfold::PrintTable (std::ostream& o, const TH1* hTrue, ErrorTreatment withError)
{
  // Prints entries from truth, measured, and reconstructed data for each bin.
  if (withError==kDefault) withError= _withError;
  if (withError==kDefault) withError= kErrors;
  const TH1* hReco= Hreco (withError);
  if (!_unfolded) return;
  Double_t chi_squ= -999.0;
  if (hTrue && (withError==kCovariance || withError==kCovToy)) chi_squ = Chi2(hTrue,withError);
  PrintTable (o, response()->Htruth(), response()->Hmeasured(), hTrue, Hmeasured(), hReco,
              _nm, _nt, _overflow, withError, chi_squ);
}


void RooUnfold::PrintTable (std::ostream& o, const TVectorD& vTrainTrue, const TVectorD& vTrain,
                            const TVectorD& vMeas, const TVectorD& vReco,
                            Int_t nm, Int_t nt)
{
  if (nm<=0) nm= vTrain    .GetNrows();
  if (nt<=0) nt= vTrainTrue.GetNrows();
  TH1D hTrainTrue ("", "", nt, 0.0, nt);
  TH1D hTrain     ("", "", nm, 0.0, nm);
  TH1D hMeas      ("", "", nm, 0.0, nm);
  TH1D hReco      ("", "", nt, 0.0, nt);
  RooUnfoldResponse::V2H (vTrainTrue, &hTrainTrue, nt);
  RooUnfoldResponse::V2H (vTrain,     &hTrain,     nm);
  RooUnfoldResponse::V2H (vMeas,      &hMeas,      nm);
  RooUnfoldResponse::V2H (vReco,      &hReco,      nt);
  PrintTable (o, &hTrainTrue, &hTrain, 0, &hMeas, &hReco, nm, nt);
}

void RooUnfold::PrintTable (std::ostream& o, const TH1* hTrainTrue, const TH1* hTrain,
                            const TH1* hTrue, const TH1* hMeas, const TH1* hReco,
                            Int_t _nm, Int_t _nt, Bool_t _overflow,
                            ErrorTreatment withError, Double_t chi_squ)
{
  // Prints entries from truth, measured, and reconstructed data for each bin.
  if (withError==kDefault) withError= hReco->GetSumw2N() ? kErrors : kNoError;
  if (_nm<=0) _nm= hTrain    ->GetNbinsX();
  if (_nt<=0) _nt= hTrainTrue->GetNbinsX();
  std::ostringstream fmt;
  fmt.copyfmt (o);
  Int_t dim= hReco->GetDimension(), ntxb= hReco->GetNbinsX()+2, ntyb= hReco->GetNbinsY()+2;
  if (hMeas->GetDimension() != dim || hMeas->GetNbinsX()+2 != ntxb || hMeas->GetNbinsY()+2 != ntyb) dim= 1;
  Int_t iwid= (dim==3) ? 8 : (dim==2) ? 7 : 5;
  const char* xwid= (dim==3) ? "===" : (dim==2) ? "==" : "";
  o << "===============================================================================" << xwid << endl
    << setw(iwid) << ""      << setw(9) << "Train" << setw(9) << "Train"    << setw(9) << "Test"  << setw(9) << "Test"  << setw(9) << "Unfolded";
  if (withError)
    o << setw(10)<<"Error on"<<setw(9) << "Diff" << setw(9) << "Pull" << endl;
  else
    o << setw(9) << "Diff" << endl;
  o << setw(iwid) << "Bin"   << setw(9) << "Truth" << setw(9) << "Measured" << setw(9) << "Truth" << setw(9) << "Input" << setw(9) << "Output";
  if (withError)
    o << setw(10)<<"Unfolding";
  o << endl;
  o << "===============================================================================" << xwid << endl;
  Double_t true_train_tot=0;
  Double_t meas_train_tot=0;
  Double_t true_test_tot=0;
  Double_t meas_test_tot=0;
  Double_t unf_tot=0;
  Double_t chi2= 0.0;
  Int_t ndf= 0, first= (_overflow ? 0 : 1);
  Int_t maxbin= _nt < _nm ? _nm : _nt;
  for (Int_t i = 0 ; i < maxbin; i++) {
    Int_t it= RooUnfoldResponse::GetBin (hReco, i, _overflow);
    Int_t im= RooUnfoldResponse::GetBin (hMeas, i, _overflow);

    if (dim==2 || dim==3) {
      Int_t iw= (dim==2) ? 3 : 2;
      Int_t ix= it%ntxb;
      Int_t iy= ((it-ix)/ntxb)%ntyb;
      o << setw(iw) << ix << ',' << setw(iw) << iy;
      if (dim==3) o << ',' << setw(iw) << ((it-ix)/ntxb - iy)/ntyb;
    } else
      o << setw(iwid) << i+first;
    o << std::fixed << setprecision(0);
    true_train_tot+=hTrainTrue->GetBinContent(it);
    meas_train_tot+=hTrain->GetBinContent(im);
    if (hTrue) true_test_tot+=hTrue->GetBinContent(it);
    meas_test_tot+=hMeas->GetBinContent(im);
    unf_tot+=hReco->GetBinContent(it);
    if (i<_nt){
      o << ' ' << setw(8) << hTrainTrue->GetBinContent(it);
    }
    else
      o << setw(9) << ' ';
    if (i<_nm)
      o << ' ' << setw(8) << hTrain->GetBinContent(im);
    else
      o << setw(9) << ' ';
    if (hTrue && i<_nt)
      o << ' ' << setw(8) << hTrue->GetBinContent(it);
    else
      o << setw(9) << ' ';
    if (i<_nm)
      o << ' ' << setw(8) << hMeas->GetBinContent(im);
    else
      o << setw(9) << ' ';
    o << setprecision(1);
    if (i<_nt) {
      Double_t y= hReco->GetBinContent(it), yerr = hReco->GetBinError(it);
      o << ' ' << setw(8) << y;
      if (withError)
        o << ' ' << setw(9) << yerr;
      if (hTrue &&
          (                       y!=0.0 || (withError &&                   yerr>0.0)) &&
          (hTrue->GetBinContent(it)!=0.0 || (withError && hTrue->GetBinError(it)>0.0))) {
        Double_t ydiff= y - hTrue->GetBinContent(it);
        o << ' ' << setw(8) << ydiff;
        if (withError && yerr>0.0) {
          ndf++;
          Double_t ypull = ydiff/yerr;
          chi2 += ypull*ypull;
          o << ' ' << setw(8) << ypull;
        }
      }
    }
    o << endl;

  }

  o << "===============================================================================" << xwid << endl
    << setw(iwid) << "" << std::fixed << setprecision(0)
    << ' ' << setw(8) << true_train_tot
    << ' ' << setw(8) << meas_train_tot;
  if (hTrue)
    o << ' ' << setw(8) << true_test_tot;
  else
    o << setw(9) << ' ';
  Double_t toterr= 0.0;
  if (meas_test_tot>0.0 && meas_train_tot>0.0) toterr= sqrt(meas_test_tot)*true_train_tot/meas_train_tot;
  o << ' ' << setw(8) << meas_test_tot << setprecision(1)
    << ' ' << setw(8) << unf_tot;
  if (withError) 
  o << ' ' << setw(9) << toterr;
  o << ' ' << setw(8) << unf_tot-true_test_tot;
  if(withError && toterr>0.0)
  o << ' ' << setw(8) <<(unf_tot-true_test_tot)/toterr;
  o << endl
    << "===============================================================================" << xwid << endl;
  o.copyfmt (fmt);
  if (hTrue) {
    if (chi_squ!=-999.0) {
      o << "Chi^2/NDF=" << chi_squ << "/" << ndf << " (bin-by-bin Chi^2=" << chi2 << ")" << endl;
    } else {
      o << "Bin-by-bin Chi^2/NDF=" << chi2 << "/" << ndf << endl;
      chi_squ= chi2;
    }
    if (chi_squ<=0.0) cerr << "Warning: Invalid Chi^2 Value" << endl;
  }
}

void RooUnfold::SetNameTitleDefault()
{
  if (!_res) return;
  const char* s= GetName();
  if (s[0] == '\0') SetName (_res->GetName());
  s= GetTitle();
  if (s[0] == '\0') {
    TString title= "Unfold ";
    title += _res->GetTitle();
    SetTitle (title);
  }
}

TH1* RooUnfold::Hreco (ErrorTreatment withError)
{
    /*Creates reconstructed distribution. Error calculation varies by withError:
    0: No errors
    1: Errors from the square root of the diagonals of the covariance matrix given by the unfolding
    2: Errors from the square root of of the covariance matrix given by the unfolding
    3: Errors from the square root of the covariance matrix from the variation of the results in toy MC tests
    */
  TH1* reco= (TH1*) _res->Htruth()->Clone(GetName());
  reco->Reset();
  reco->SetTitle (GetTitle());
  if (!UnfoldWithErrors (withError)) withError= kNoError;
  if (!_unfolded) return reco;

  for (Int_t i= 0; i < _nt; i++) {
    Int_t j= RooUnfoldResponse::GetBin (reco, i, _overflow);
    reco->SetBinContent (j,             _rec(i));
    if        (withError==kErrors){
      reco->SetBinError (j, sqrt (fabs (_variances(i))));
    } else if (withError==kCovariance){
      reco->SetBinError (j, sqrt (fabs (_cov(i,i))));
    } else if (withError==kCovToy){
      reco->SetBinError (j, sqrt (fabs (_err_mat(i,i))));
    }
  }

  return reco;
}

void RooUnfold::GetSettings()
{
    //Gets maximum and minimum parameters and step size
    _minparm=0;
    _maxparm=0;
    _stepsizeparm=0;
    _defaultparm=0;
}

Double_t RooUnfold::GetMinParm() const
{
    //Get minimum regularisation parameter for unfolding method
    return _minparm;
}

Double_t RooUnfold::GetMaxParm() const
{
    //Get maximum regularisation parameter for unfolding method
    return _maxparm;
}

Double_t RooUnfold::GetStepSizeParm() const
{
    //Get suggested step size for unfolding distribution
    return _stepsizeparm;
}

Double_t RooUnfold::GetDefaultParm() const
{
    //Get suggested regularisation parameter.
    return _defaultparm;
}

RooUnfold* RooUnfold::RunToy() const
{
  // Returns new RooUnfold object with smeared measurements and
  // (if IncludeSystematics) response matrix for use as a toy.
  // Use multiple toys to find spread of unfolding results.
  TString name= GetName();
  name += "_toy";
  RooUnfold* unfold = Clone(name);

  // Make new smeared response matrix
  if (_dosys) unfold->SetResponse (_res->RunToy(), kTRUE);
  if (_dosys==2) return unfold;

  if (_haveCovMes) {

    // _covL is a lower triangular matrix for which the covariance matrix, V = _covL * _covL^T.
    if (!_covL) {
      TDecompChol c(*_covMes);
      c.Decompose();
      TMatrixD U(c.GetU());
      _covL= new TMatrixD (TMatrixD::kTransposed, U);
      if (_verbose>=2) RooUnfoldResponse::PrintMatrix(*_covL,"decomposed measurement covariance matrix");
    }
    TVectorD newmeas(_nm);
    for (Int_t i= 0; i<_nm; i++) newmeas[i]= gRandom->Gaus(0.0,1.0);
    newmeas *= *_covL;
    newmeas += Vmeasured();
    unfold->SetMeasured(newmeas,*_covMes);

  } else {

    TVectorD newmeas= Vmeasured();
    const TVectorD& err= Emeasured();
    for (Int_t i= 0; i<_nm; i++) {
      Double_t e= err[i];
      if (e>0.0) newmeas[i] += gRandom->Gaus(0,e);
    }
    unfold->SetMeasured(newmeas,err);

  }
  return unfold;
}

void RooUnfold::Print(Option_t* /*opt*/) const
{
  cout << ClassName() << "::" << GetName() << " \"" << GetTitle()
       << "\", regularisation parameter=" << GetRegParm() << ", ";
  if (_haveCovMes) cout << "with measurement covariance, ";
  if (_dosys)      cout << "calculate systematic errors, ";
  if (_meas->GetDimension()==1) cout << _nm;
  else {
    cout <<        _meas->GetNbinsX()
         << "x" << _meas->GetNbinsY();
    if (_meas->GetDimension()>=3)
    cout << "x" << _meas->GetNbinsZ();
    cout << " (" << _nm << ")";
  }
  cout << " bins measured, ";
  const TH1* rtrue= _res->Htruth();
  if (rtrue->GetDimension()==1) cout << _nt;
  else {
    cout <<        rtrue->GetNbinsX()
         << "x" << rtrue->GetNbinsY();
    if (rtrue->GetDimension()>=3)
    cout << "x" << rtrue->GetNbinsZ();
    cout << " (" << _nt << ")";
  }
  cout << " bins truth";
  if (_overflow) cout << " including overflows";
  cout << endl;
}

TMatrixD RooUnfold::CutZeros(const TMatrixD& ereco)
{
    //Removes row & column if all their elements are 0.
    vector<int> diags;
        int missed=0;
        for (int i=0; i<ereco.GetNrows(); i++){
            double coltot=0;
            for (int j=0;j<ereco.GetNrows();j++){
                coltot+=ereco(i,j);
            }
            if (coltot==0){
                diags.push_back(i);
                missed++;
            }
        }
        int x=ereco.GetNrows()-missed;
        int y=ereco.GetNcols()-missed;
        TMatrixD ereco_cut(x,y);
        unsigned int v=0;
        for (int i=0;i<ereco.GetNrows();i++){
            if(v<diags.size() && diags[v]==i){
                v++;
            }
            else{
                for (int j=0; j<ereco_cut.GetNcols();j++){
                    ereco_cut(i-v,j)=ereco(i,j+v);
                    }
                }
        }
    return ereco_cut;
}

TMatrixD RooUnfold::Ereco(ErrorTreatment withError)
{
    /*Returns covariance matrices for error calculation of type withError
    0: Errors are the square root of the bin content
    1: Errors from the diagonals of the covariance matrix given by the unfolding
    2: Errors from the covariance matrix given by the unfolding
    3: Errors from the covariance matrix from the variation of the results in toy MC tests
    */
    TMatrixD Ereco_m(_nt,_nt);
    if (!UnfoldWithErrors (withError)) return Ereco_m;

    switch(withError){
      case kNoError:
        for (int i=0; i<_nt; i++){
          Ereco_m(i,i)=_rec(i);
        }
        break;
      case kErrors:
        for (int i=0; i<_nt;i++){
          Ereco_m(i,i)=_variances(i);
        }
        break;
      case kCovariance:
        Ereco_m=_cov;
        break;
      case kCovToy:
        Ereco_m=_err_mat;
        break;
      default:
        cerr<<"Error, unrecognised error method= "<<withError<<endl;
    }
    return Ereco_m;
}

TVectorD RooUnfold::ErecoV(ErrorTreatment withError)
{
    /*Returns vector of unfolding errors computed according to the withError flag:
    0: Errors are the square root of the bin content
    1: Errors from the diagonals of the covariance matrix given by the unfolding
    2: Errors from the covariance matrix given by the unfolding
    3: Errors from the covariance matrix from the variation of the results in toy MC tests
    */
    TVectorD Ereco_v(_nt);
    if (!UnfoldWithErrors (withError)) return Ereco_v;

    switch(withError){
      case kNoError:
        for (int i=0; i<_nt; i++){
          Ereco_v(i)=sqrt (fabs (_rec(i)));
        }
        break;
      case kErrors:
        for (int i=0; i<_nt; i++){
          Ereco_v(i)=sqrt (fabs (_variances(i)));
        }
        break;
      case kCovariance:
        for (int i=0; i<_nt; i++){
          Ereco_v(i)=sqrt (fabs (_cov(i,i)));
        }
        break;
      case kCovToy:
        for (int i=0; i<_nt; i++){
          Ereco_v(i)=sqrt (fabs (_err_mat(i,i)));
        }
        break;
      default:
        cerr<<"Error, unrecognised error method= "<<withError<<endl;
    }
    return Ereco_v;
}

TMatrixD RooUnfold::Wreco(ErrorTreatment withError)
{
    TMatrixD Wreco_m(_nt,_nt);
    if (!UnfoldWithErrors (withError, true)) return Wreco_m;

    switch(withError){
      case kNoError:
        for (int i=0; i<_nt; i++){
          if (_rec(i)!=0.0) Wreco_m(i,i)=1.0/_rec(i);
        }
        break;
      case kErrors:
        for (int i=0; i<_nt;i++){
          Wreco_m(i,i)=_wgt(i,i);
        }
        break;
      case kCovariance:
        Wreco_m=_wgt;
        break;
      case kCovToy:
        InvertMatrix (_err_mat, Wreco_m, "covariance matrix from toys", _verbose);
        break;
      default:
        cerr<<"Error, unrecognised error method= "<<withError<<endl;
    }
    return Wreco_m;
}

TH1D* RooUnfold::HistNoOverflow (const TH1* h, Bool_t overflow)
{
  if (!overflow) {   // also for 2D+
    TH1D* hx= RooUnfoldResponse::H2H1D (h, h->GetNbinsX()*h->GetNbinsY()*h->GetNbinsZ());
    if (!hx) return hx;
    // clear under/overflow bins for cloned TH1D
    hx->SetBinContent (0,                 0.0);
    hx->SetBinContent (hx->GetNbinsX()+1, 0.0);
    return hx;
  }
  Int_t nb= h->GetNbinsX(), s= h->GetSumw2N();
  Double_t xlo= h->GetXaxis()->GetXmin(), xhi= h->GetXaxis()->GetXmax(), xb= (xhi-xlo)/nb;
  nb += 2;
  TH1D* hx= new TH1D (h->GetName(), h->GetTitle(), nb, xlo-xb, xhi+xb);
  for (Int_t i= 0; i < nb; i++) {
           hx->SetBinContent (i+1, h->GetBinContent (i));
    if (s) hx->SetBinError   (i+1, h->GetBinError   (i));
  }
  return hx;
}

TH1* RooUnfold::Resize (TH1* h, Int_t nx, Int_t ny, Int_t nz)
{
  // Resize a histogram with a different number of bins.
  // Contents and errors are copied to the same bin numbers (the overflow bin
  // is copied to the new overflow bin) in the new histogram.
  // If the new histogram is larger than the old, the extra bins are zeroed.
  Int_t mx= h->GetNbinsX(), my= h->GetNbinsY(), mz= h->GetNbinsZ();
  Int_t nd= h->GetDimension();
  if (nx<0 || nd<1) nx= mx;
  if (ny<0 || nd<2) ny= my;
  if (nz<0 || nd<3) nz= mz;
  TH1* hc= (TH1*) h->Clone("resize_tmp");

  bool mod= false;
  if (nx!=mx) {
    Double_t xlo= h->GetXaxis()->GetXmin(), xhi= h->GetXaxis()->GetXmax();
    h->GetXaxis()->Set (nx, xlo, xlo+((xhi-xlo)/mx)*nx);
    mod= true;
  }
  if (ny!=my) {
    Double_t ylo= h->GetYaxis()->GetXmin(), yhi= h->GetYaxis()->GetXmax();
    h->GetYaxis()->Set (ny, ylo, ylo+((yhi-ylo)/my)*ny);
    mod= true;
  }
  if (nz!=mz) {
    Double_t zlo= h->GetZaxis()->GetXmin(), zhi= h->GetZaxis()->GetXmax();
    h->GetZaxis()->Set (nz, zlo, zlo+((zhi-zlo)/mz)*nz);
    mod= true;
  }

  if (mod) {
    h->SetBinsLength();  // Just copies array, which isn't right for overflows or 2D/3D
    Int_t s= h->GetSumw2N();
    Int_t ox= mx+1, oy= my+1, oz= mz+1;  // old overflow bin
    Int_t px= nx+1, py= ny+1, pz= nz+1;  // new overflow bin

    if        (nd==1) {

      for (Int_t i= 0; i<=nx; i++) {
               h->SetBinContent (i, i>mx ? 0.0 : hc->GetBinContent (i));
        if (s) h->SetBinError   (i, i>mx ? 0.0 : hc->GetBinError   (i));
      }
             h->SetBinContent (px, h->GetBinContent (ox));
      if (s) h->SetBinError   (px, h->GetBinError   (ox));

    } else if (nd==2) {

      for (Int_t i= 0; i<=nx; i++) {
        for (Int_t j= 0; j<=ny; j++) {
                 h->SetBinContent (i, j, i>mx||j>my ? 0.0 : hc->GetBinContent (i, j));
          if (s) h->SetBinError   (i, j, i>mx||j>my ? 0.0 : hc->GetBinError   (i, j));
        }
               h->SetBinContent (i, py, i>mx ? 0.0 : hc->GetBinContent (i, oy));
        if (s) h->SetBinError   (i, py, i>mx ? 0.0 : hc->GetBinError   (i, oy));
      }
      for (Int_t j= 0; j<=ny; j++) {
               h->SetBinContent (px, j, j>my ? 0.0 : hc->GetBinContent (ox, j));
        if (s) h->SetBinError   (px, j, j>my ? 0.0 : hc->GetBinError   (ox, j));
      }
             h->SetBinContent (px, py, hc->GetBinContent (ox, oy));
      if (s) h->SetBinError   (px, py, hc->GetBinError   (ox, oy));

    } else if (nd==3) {

      for (Int_t i= 0; i<=nx; i++) {
        for (Int_t j= 0; j<=ny; j++) {
          for (Int_t k= 0; k<=nz; k++) {
                   h->SetBinContent (i, j, k, i>mx||j>my||k>mz ? 0.0 : hc->GetBinContent (i, j, k));
            if (s) h->SetBinError   (i, j, k, i>mx||j>my||k>mz ? 0.0 : hc->GetBinError   (i, j, k));
          }
                 h->SetBinContent (i, j, pz, i>mx||j>my ? 0.0 : hc->GetBinContent (i, j, oz));
          if (s) h->SetBinError   (i, j, pz, i>mx||j>my ? 0.0 : hc->GetBinError   (i, j, oz));
        }
               h->SetBinContent (i, py, pz, i>mx ? 0.0 : hc->GetBinContent (i, oy, oz));
        if (s) h->SetBinError   (i, py, pz, i>mx ? 0.0 : hc->GetBinError   (i, oy, oz));
      }
      for (Int_t j= 0; j<=ny; j++) {
        for (Int_t k= 0; k<=nz; k++) {
                 h->SetBinContent (px, j, k, j>my||k>mz ? 0.0 : hc->GetBinContent (ox, j, k));
          if (s) h->SetBinError   (px, j, k, j>my||k>mz ? 0.0 : hc->GetBinError   (ox, j, k));
        }
               h->SetBinContent (px, j, pz, j>my ? 0.0 : hc->GetBinContent (ox, j, oz));
        if (s) h->SetBinError   (px, j, pz, j>my ? 0.0 : hc->GetBinError   (ox, j, oz));
      }
      for (Int_t k= 0; k<=nz; k++) {
        for (Int_t i= 0; i<=nx; i++) {
                 h->SetBinContent (i, py, k, i>mx||k>mz ? 0.0 : hc->GetBinContent (i, oy, k));
          if (s) h->SetBinError   (i, py, k, i>mx||k>mz ? 0.0 : hc->GetBinError   (i, oy, k));
        }
               h->SetBinContent (px, py, k, k>mz ? 0.0 : hc->GetBinContent (ox, oy, k));
        if (s) h->SetBinError   (px, py, k, k>mz ? 0.0 : hc->GetBinError   (ox, oy, k));
      }
             h->SetBinContent (px, py, pz, hc->GetBinContent (ox, oy, oz));
      if (s) h->SetBinError   (px, py, pz, hc->GetBinError   (ox, oy, oz));

    }
  }
  delete hc;
  return h;
}

TMatrixD& RooUnfold::ABAT (const TMatrixD& a, const TMatrixD& b, TMatrixD& c)
{
  // Fills C such that C = A * B * A^T. Note that C cannot be the same object as A.
  TMatrixD d (b, TMatrixD::kMultTranspose, a);
  c.Mult (a, d);
  return c;
}

TMatrixD& RooUnfold::ABAT (const TMatrixD& a, const TVectorD& b, TMatrixD& c)
{
  // Fills C such that C = A * B * A^T, where B is a diagonal matrix specified by the vector.
  // Note that C cannot be the same object as A.
  TMatrixD d (TMatrixD::kTransposed, a);
  d.NormByColumn (b, "M");
  c.Mult (a, d);
  return c;
}

Int_t RooUnfold::InvertMatrix(const TMatrixD& mat, TMatrixD& inv, const char* name, Int_t verbose)
{
  // Invert a matrix using Single Value Decomposition: inv = mat^-1.
  // Can use InvertMatrix(mat,mat) to invert in-place.
  Int_t ok= 1;
  TDecompSVD svd (mat);
  const Double_t cond_max= 1e17;
  Double_t cond= svd.Condition();
  if (verbose >= 1) {
    Double_t d1=0,d2=0;
    svd.Det(d1,d2);
    Double_t det= d1*TMath::Power(2.,d2);
    cout << name << " condition="<<cond<<", determinant="<<det;
    if (d2!=0.0) cout <<" ("<<d1<<"*2^"<<d2<<")";
    cout <<", tolerance="<<svd.GetTol()<<endl;
  }
  if        (cond<0.0){
    cerr <<"Warning: bad "<<name<<" condition ("<<cond<<")"<<endl;
    ok= 2;
  } else if (cond>cond_max) {
    cerr << "Warning: poorly conditioned "<<name<<" - inverse may be inaccurate (condition="<<cond<<")"<<endl;
    ok= 3;
  }
  inv.ResizeTo (mat.GetNcols(), mat.GetNrows());  // pseudo-inverse of A(r,c) is B(c,r)
#if ROOT_VERSION_CODE >= ROOT_VERSION(5,13,4)  /* TDecompSVD::Invert() didn't have ok status before 5.13/04. */
  Bool_t okinv= false;
  inv= svd.Invert(okinv);
  if (!okinv) {
    cerr << name << " inversion failed" << endl;
    return 0;
  }
#else
  inv= svd.Invert();
#endif
  if (verbose>=1) {
    TMatrixD I (mat, TMatrixD::kMult, inv);
    if (verbose>=3) RooUnfoldResponse::PrintMatrix(I,"V*V^-1");
    Double_t m= 0.0;
    for (Int_t i= 0; i<I.GetNrows(); i++) {
      Double_t d= fabs(I(i,i)-1.0);
      if (d>m) m= d;
      for (Int_t j= 0; j<i; j++) {
        d= fabs(I(i,j)); if (d>m) m= d;
        d= fabs(I(j,i)); if (d>m) m= d;
      }
    }
    cout << "Inverse " << name << " " << 100.0*m << "% maximum error" << endl;
  }
  return ok;
}

void RooUnfold::Streamer (TBuffer &R__b)
{
  // Stream an object of class RooUnfold.
  if (R__b.IsReading()) {
    // Don't add our histograms to the currect directory.
    // We own them and we don't want them to disappear when the file is closed.
    Bool_t oldstat= TH1::AddDirectoryStatus();
    TH1::AddDirectory (kFALSE);
    RooUnfold::Class()->ReadBuffer  (R__b, this);
    TH1::AddDirectory (oldstat);
  } else {
    RooUnfold::Class()->WriteBuffer (R__b, this);
  }
}
