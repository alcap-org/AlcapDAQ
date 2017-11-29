//=====================================================================-*-C++-*-
// File and Version Information:
//
//
// Description:
//
//==============================================================================

#include "RooUnfoldBasisSplines.h"

#include <iostream>
#include <iomanip>
#include <math.h>

#include "TH1.h"
#include "TH2.h"
#include "TF1.h"
#include "TMath.h"
#include "Math/WrappedTF1.h"
#include "Math/BrentRootFinder.h"

#include "TDecompSVD.h"

#include "RooUnfoldResponse.h"

using std::cout;
using std::cerr;
using std::endl;

ClassImp( RooUnfoldBasisSplines );

// Ctors:
RooUnfoldBasisSplines::RooUnfoldBasisSplines( const RooUnfoldBasisSplines& rhs )
  : RooUnfold( rhs ) {
  Init();
}
RooUnfoldBasisSplines::RooUnfoldBasisSplines( const RooUnfoldResponse* res, 
					      const TH1* meas,
					      Int_t nrebin,
					      const Double_t tau,
					      const Int_t m0,
					      const Int_t iauto,
					      const Double_t tol,
					      const char* name, 
					      const char* title )
  : RooUnfold( res, meas, name, title ), _nrebin(nrebin), _tau(tau), 
    _m0(m0), _iauto(iauto), _tol(tol) {
  Init();
}
RooUnfoldBasisSplines::RooUnfoldBasisSplines() : RooUnfold() {
  Init();
}
RooUnfoldBasisSplines::RooUnfoldBasisSplines( const char* name, 
					      const char* title )
  : RooUnfold( name, title ) {
  Init();
}
RooUnfoldBasisSplines::RooUnfoldBasisSplines( const TString& name, 
					      const TString& title )
  : RooUnfold( name, title ) {
  Init();
}

// Dtor:
RooUnfoldBasisSplines::~RooUnfoldBasisSplines() {}

// Assignment:
RooUnfoldBasisSplines& 
RooUnfoldBasisSplines::operator= ( const RooUnfoldBasisSplines& rhs ) {
  Assign( rhs );
  return *this;
}

// Cloning:
RooUnfoldBasisSplines* RooUnfoldBasisSplines::Clone( const char* newname ) 
  const {
  RooUnfoldBasisSplines* unfold= new RooUnfoldBasisSplines( *this );
  if( newname && strlen(newname) ) unfold->SetName( newname );
  return unfold;
}

// Initialise:
void RooUnfoldBasisSplines::Init() {
  GetSettings();
}

// Reset:
void RooUnfoldBasisSplines::Reset() {
  Init();
  RooUnfold::Reset();
}

// Return response matrix:
const TMatrixD* RooUnfoldBasisSplines::Impl() {
  return &_resm;
}

// Subtract fakes:
void RooUnfoldBasisSplines::SubtractFakes() {
  _measured.ResizeTo( _nm );
  _measured= Vmeasured();
  if( _res->FakeEntries() ) {
    TVectorD fakes= _res->Vfakes();
    Double_t fac= _res->Vmeasured().Sum();
    if( fac != 0.0 ) {
      fac= Vmeasured().Sum() / fac;
    }
    if( verbose() >= 1 ) {
      cout << "Subtract " << fac*fakes.Sum() 
	   << " fakes from measured distribution" << endl;
    }
    fakes*= fac;
    _measured-= fakes;
  }
}

// Overrides to get/set tau:
void RooUnfoldBasisSplines::SetRegParm( Double_t tau ) {
  _tau= tau;
  return;
}
Double_t RooUnfoldBasisSplines::GetRegParm() const {
  return _tau;
}

// Overrides for printing and returning of unfolded:
void RooUnfoldBasisSplines::PrintTable( std::ostream& o, const TH1* hTrue, 
					ErrorTreatment withError ) {
  // Prints entries from truth, measured, and reconstructed data for each bin.
  const TH1* hReco= Hreco( withError );
  if( !_unfolded ) return;
  TH1* hTrueclone= (TH1*) hTrue->Clone();
  const TH1* hTruerebin= hTrueclone->Rebin( _nrebin, hTrueclone->GetName() );
  Double_t chi_squ= -999.0;
  if( hTrue && ( withError==kCovariance || withError==kCovToy ) ) {
    chi_squ= Chi2( hTruerebin, withError );
  }
  TH1* truthresponseclone= (TH1*) response()->Htruth()->Clone();
  const TH1* truthresponse= truthresponseclone->Rebin( _nrebin, truthresponseclone->GetName() );
  RooUnfold::PrintTable( o, truthresponse, response()->Hmeasured(), 
			 hTruerebin, Hmeasured(), hReco,
			 _nm, _nt, _overflow, withError, chi_squ );
  return;
}

// Creates reconstructed distribution. Error calculation varies by withError:
// 0: No errors
// 1: Errors from the square root of the diagonals of the covariance matrix given by the unfolding
// 2: Errors from the square root of of the covariance matrix given by the unfolding
// 3: Errors from the square root of the covariance matrix from the variation of the results in 
//    toy MC tests
TH1* RooUnfoldBasisSplines::Hreco( ErrorTreatment withError ) {
  TH1* reco= (TH1*) _res->Htruth()->Clone( GetName() ); 
  reco= reco->Rebin( _nrebin );
  reco->Reset();
  reco->SetTitle( GetTitle() );
  if( !UnfoldWithErrors( withError ) ) withError= kNoError;
  if( !_unfolded ) return reco;
  for( Int_t i= 0; i < _nt; i++ ) {
    Int_t j= RooUnfoldResponse::GetBin( reco, i, _overflow );
    reco->SetBinContent( j, _rec(i) );
    if( withError==kErrors ) {
      reco->SetBinError( j, sqrt( fabs( _variances(i) ) ) );
    } else if( withError==kCovariance ) {
      reco->SetBinError( j, sqrt( fabs( _cov(i,i) ) ) );
    } else if( withError==kCovToy ) {
      reco->SetBinError( j, sqrt( fabs( _err_mat(i,i) ) ) );
    }
  }
  return reco;
}

// Get reconstructed distribution folded up to measured level:
TH1* RooUnfoldBasisSplines::HrecoMeasured() {
  TH1* hist= (TH1*) _meas->Clone( GetName() );
  hist->Reset();
  hist->SetTitle( GetTitle() );
  TVectorD yreco= _resm*_reconstructed;
  for( Int_t i= 0; i < _nm; i++ ) {
    Int_t j= RooUnfoldResponse::GetBin( hist, i, _overflow );
    hist->SetBinContent( j, yreco(i) );
  }
  return hist;
}

// Override for measured chi^2:
Double_t RooUnfoldBasisSplines::Chi2measured() {
  TVectorD yreco= _resm*_reconstructed;
  TVectorD delta= _measured - yreco;
  Double_t chisq= _vinv.Similarity( delta );
  return chisq;
}

// Do the unfolding steps:
void RooUnfoldBasisSplines::Unfold() {

  // Deal with fakes first:
  SubtractFakes();

  // Initial number of control points:
  Int_t nbinm= _nm;
  Int_t nbint= _nt;
  Int_t np= _m0;
  if( _m0 == 0 ) {
    np= nbint/_nrebin+2;
  }
  if( np > nbinm ) {
    cerr << "RooUnfoldBasisSplines::Unfold: number of control points " << np
	 << " > number of measured points " << nbinm << endl;
    return;
  }
  if( np < nbint/_nrebin + 2 ) {
    cerr << "RooUnfoldBasisSplines::Unfold: number of control points " << np
	 << " < number of truth bins/nrebin + 2 " << nbint/_nrebin+2
	 << ", error matrix may be unreliable" << endl;
  }

  // Get normalised response matrix:
  _resm.ResizeTo( nbinm, nbint );
  _resm= _res->Mresponse();

  // Get inverted measured error matrix:
  TMatrixD covm= GetMeasuredCov();
  RooUnfold::InvertMatrix( covm, covm );
  _vinv.ResizeTo( nbinm, nbinm );
  for( Int_t ibin= 0; ibin < nbinm; ibin++ ) {
    for( Int_t jbin= 0; jbin < nbinm; jbin++ ) {
      _vinv(ibin,jbin)= covm(ibin,jbin);
    }
  }

  // Control point values, basis spline and curvature matrices:
  const TH1* hTruth= _res->Htruth();
  TVectorD bins( nbint+1 );
  for( Int_t ibin= 0; ibin < nbint+1; ibin++ ) {
    bins[ibin]= hTruth->GetBinLowEdge( ibin+1 );
  }
  TVectorD cppos= makeControlpoints( bins, np );
  TMatrixD B= makeBasisSplineMatrix( bins, cppos );
  TMatrixD AB= _resm*B;
  TMatrixDSym C= makeCurvatureMatrix( np );

  // Test and possibly set regularisation parameter:
  Int_t npeff= np;
  if( _iauto > 0 ) {
    TVectorD cpeigenvalues;
    Double_t opttau= findTauFromNoise( AB, _measured, _vinv, np, 
				       cpeigenvalues );
    if( verbose() >= 1 ) {
      npeff= m0FromTau( opttau, cpeigenvalues, 0 );
      cout << "RooUnfoldBasisSplines::Unfold: recommended tau from noise: " 
	   << std::scientific << std::setw(9) << opttau;
      cout << ", eff. np " << npeff << endl;
    }
    if( _iauto == 2 ) _tau= opttau;
    npeff= m0FromTau( _tau, cpeigenvalues, 0 );
  }
  if( verbose() >= 1 ) {
    cout << "RooUnfoldBasisSplines::Unfold: iauto: " << _iauto
	 << ", np and tau values: " 
	 << np << ", " << std::scientific << std::setw(9) << _tau 
	 << ", eff. np: " << npeff << endl;
  }
  
  // Solution for control point values p and unfolded bin contents t
  // from LLS between splined truth x response and measured distribution.
  // AB is product of response matrix with basis spline matrix:
  TMatrixD ABT( AB );
  ABT.T();
  TMatrixD h= ABT*_vinv;
  TMatrixDSym H( _vinv );
  H.SimilarityT( AB );
  TMatrixDSym ABC= H + _tau*C;
  // Use explicit SVD inversion:
  TDecompSVD ABCsvd( ABC, _tol );
  TMatrixD ABCsvdinv= ABCsvd.Invert();
  TVectorD s= ABCsvd.GetSig();
  Int_t nrejsvd= 0;
  for( Int_t ip= 0; ip < np; ip++ ) {
    if( s[ip] < _tol*s[0] ) {
      nrejsvd++;
      if( verbose() >= 2 ) {
	cout << "RooUnfoldBasisSplines::Unfold:Reject singular value " 
	     << ip << " " << s[ip] << endl;
      }
    }
  }
  TMatrixDSym ABCinv( np, ABCsvdinv.GetMatrixArray() );
  TMatrixD ABCinvh= ABCinv*h;
  TVectorD p= ABCinvh*_measured;
  TVectorD t= B*p;
  _reconstructed.ResizeTo( nbint );
  _reconstructed= t;

  // Rebin solution:
  TMatrixD rbm= makeRebinMatrix( nbint, _nrebin );
  TVectorD trb= rbm*t;
  _rec.ResizeTo( nbint/_nrebin );
  _rec= trb;

  // Covariance matrices of control point values and of unfolding result:
  TMatrixDSym covp( ABCinv );
  TMatrixDSym covt= covp.Similarity( B );
  _nt= nbint/_nrebin;
  _cov.ResizeTo( _nt, _nt );
  _cov= covt.Similarity( rbm );
  _variances.ResizeTo( _nt );
  for( Int_t it= 0; it < _nt; it++ ) {
    _variances(it)= _cov(it,it);
  }

  // Chi^2 values:
  // TVectorD delta= _measured - AB*p;
  // Double_t chisq= _vinv.Similarity( delta );
  Double_t chisq= Chi2measured();
  Double_t chisqcurv= _tau*C.Similarity( p );
  Int_t ndof= nbinm-np;
  cout << "RooUnfoldBasisSplines::Unfold: Chi^2 of solution=";
  cout.precision(2);
  cout << std::fixed << std::setw(7) << chisq;
  cout << ", from curvature " << std::setw(7) << chisqcurv;
  cout << ", for " << ndof << " d.o.f." << endl;
  cout << "P(chi^2)=" << std::scientific << std::setw(9) 
       << TMath::Prob( chisq, ndof ) << endl;
  if( _iauto > 0 and _tau > 0.0 ) {
    ndof= nbinm-npeff;
    cout << "Eff. d.o.f.: " << ndof << ", P(chi^2)=" << std::scientific 
	 << std::setw(9) << TMath::Prob( chisq, ndof ) << endl;
  }
  ndof= nbinm-np+nrejsvd;
  cout << "SVD d.o.f.: " << ndof << ", P(chi^2)=" << std::scientific 
       << std::setw(9) << TMath::Prob( chisq, ndof ) << endl;
  cout << std::fixed;
  if( verbose() >= 2 ) {
    cout << "RooUnfoldBasisSplines::Unfold: regularised inversion solution p:"
	 << endl;
    p.Print();
  }

  // Set state:
  _unfolded= true;
  _haveCov= true;
  _haveErrors= true;

  // Eigenvalue solutions, only if verbose>1 since inversion solution
  // is equivalent.  The eigenvalue solutions give more information
  // on the regularisation behaviour:
  if( verbose() >= 2 ) {
    // Unreg. solution with eigenvalues of H:
    TVectorD Heigenvalues( np );
    TMatrixD Uh= H.EigenVectors( Heigenvalues );
    Double_t mineigenvalue= Heigenvalues.Min();
    if( mineigenvalue < 0.0 ) Heigenvalues-= 2.0*mineigenvalue;
    TMatrixD UhT( Uh );
    UhT.T();
    TMatrixDSym D( np );
    TMatrixDSym Dhalf( np );
    TMatrixDSym Dminushalf( np );
    for( Int_t ip= 0; ip < np; ip++ ) {
      D(ip,ip)= Heigenvalues[ip];
      Dhalf(ip,ip)= sqrt(Heigenvalues[ip]);
      Dminushalf(ip,ip)= 1.0/Dhalf(ip,ip);
    }
    TVectorD q= Dminushalf*UhT*h*_measured;
    TVectorD pp= Uh*Dminushalf*q;
    cout << "RooUnfoldBasisSplines::Unfold: eigenvalues of H" << endl;
    Heigenvalues.Print();
    cout << "RooUnfoldBasisSplines::Unfold: unreg. solution q:" << endl;
    q.Print();
    cout << "RooUnfoldBasisSplines::Unfold: unreg. solution p:" << endl;
    pp.Print();
    // Reg. solution with eigenvalues of curvature matrix:
    TMatrixD UhDminushalfT= Dminushalf*UhT;
    TMatrixDSym Cp= C.Similarity( UhDminushalfT );
    TVectorD cpeigenvalues( np );
    TMatrixD Ucp= Cp.EigenVectors( cpeigenvalues );
    TMatrixD UcpT( Ucp );
    UcpT.T();
    TMatrixDSym S( np );
    for( Int_t ip= 0; ip < np; ip++ ) {
      S(ip,ip)= cpeigenvalues[ip];
    }
    cout << "RooUnfoldBasisSplines::Unfold: diagonal curvature matrix:" << endl;
    cpeigenvalues.Print();
    TMatrixDSym UnitplustauS( np );
    UnitplustauS.UnitMatrix();
    UnitplustauS+= _tau*S;
    TMatrixDSym UnitplustauSinv( np );
    Double_t regfsum= 0.0;
    for( Int_t ip= 0; ip < np; ip++ ) {
      UnitplustauSinv(ip,ip)= 1.0/UnitplustauS(ip,ip);
      regfsum+= UnitplustauSinv(ip,ip);
    }
    cout << "RooUnfoldBasisSplines::Unfold: reg. factors:" << endl;
    for( Int_t ip= 0; ip < np; ip++ ) {
      cout << UnitplustauSinv(ip,ip) << " ";
    }
    cout << endl;
    cout << "RooUnfoldBasisSplines::Unfold: reg. factors sum: " 
	 << regfsum << endl;
    TMatrixD Q= UcpT*Dminushalf*UhT*h;
    TVectorD qp= Q*_measured;
    TVectorD qpp= UnitplustauSinv*qp;
    TVectorD ppp= Uh*Dminushalf*Ucp*qpp;
    cout << "RooUnfoldBasisSplines::Unfold: reg. eigenvalue solution q':" 
	 << endl;
    qpp.Print();
    cout << "RooUnfoldBasisSplines::Unfold: reg. eigenvalue solution p:" 
	 << endl;
    ppp.Print();
  }

  // The End:
  return;

}

// Rebinning matrix:
TMatrixD RooUnfoldBasisSplines::makeRebinMatrix( Int_t nbin, 
						 Int_t nrebin ) {
  Int_t nbinrb= nbin/nrebin;
  TMatrixD rbm( nbinrb, nbin );
  for( Int_t ibinrb= 0; ibinrb < nbinrb; ibinrb++ ) {
    for( Int_t i= ibinrb*nrebin; i < (ibinrb+1)*nrebin; i++ ) {
      rbm(ibinrb,i)= 1.0;
    }
  }
  return rbm;
}

// Determination of number of control points from "noise" in 
// unreg. solution q' from eigenvalues of C.  See "Note on Blobels 
// unfolding method", G. Cowan, 2/1998 (www.desy.de/~blobel/runcowan.ps)
Double_t RooUnfoldBasisSplines::findTauFromNoise( const TMatrixD& AB, 
						  const TVectorD& y, 
						  const TMatrixDSym& Vinv, 
						  Int_t np,
						  TVectorD& Cpeigenvalues ) {
  TMatrixD ABT( AB );
  ABT.T();
  TMatrixD h= ABT*Vinv;
  TMatrixDSym H( Vinv );
  H.SimilarityT( AB );
  TVectorD Heigenvalues( np );
  TMatrixD Uh= H.EigenVectors( Heigenvalues );
  Double_t mineigenvalue= Heigenvalues.Min();
  if( mineigenvalue < 0.0 ) Heigenvalues-= 2.0*mineigenvalue;
  TMatrixD UhT( Uh );
  UhT.T();
  TMatrixDSym Dminushalf( np );
  for( Int_t ip= 0; ip < np; ip++ ) {
    Dminushalf(ip,ip)= 1.0/sqrt(Heigenvalues[ip]);
  }
  TMatrixD UhDminushalfT= Dminushalf*UhT;
  TMatrixDSym Cp= makeCurvatureMatrix( np );
  Cp.Similarity( UhDminushalfT );
  Cpeigenvalues.ResizeTo( np );
  TMatrixD Ucp= Cp.EigenVectors( Cpeigenvalues );
  TMatrixD UcpT( Ucp );
  UcpT.T();
  TMatrixD Q= UcpT*Dminushalf*UhT*h;
  TVectorD qp= Q*y;
  Double_t opttaunoise= optTauNoise( Cpeigenvalues, qp );
  Int_t m0noise= m0FromTau( opttaunoise, Cpeigenvalues );
  Double_t opttau= optTau( Cpeigenvalues, std::min(m0noise,np-1) );
  if( verbose() >= 2 ) {
    cout << "RooUnfoldBasisSplines::findTauFromNoise: Optimal tau, m0, tau from noise in q': " 
	 << opttaunoise << " " << m0noise << " " << opttau << endl;
  }
  return opttau;
}
Int_t RooUnfoldBasisSplines::m0FromTau( Double_t opttaunoise, 
					const TVectorD& Cpeigenvalues,
					Int_t offset ) {
  Double_t m0sum( offset );
  for( Int_t ip= 0; ip < Cpeigenvalues.GetNoElements(); ip++ ) {
    m0sum+= 1.0/(1.0+opttaunoise*Cpeigenvalues[ip]);
  }
  Int_t m0noise( m0sum );
  return m0noise;
}
// Iterative version of tau/m0 optimisation:
Int_t RooUnfoldBasisSplines::findM0noise( const TVectorD& bins, 
					  const TVectorD& y, 
					  const TMatrixDSym& Vinv, 
					  Int_t npstart, 
					  Double_t& opttau,
					  Int_t maxiter ) {
  Int_t m0= npstart;
  Int_t m0final= 0;
  for( Int_t iiter= 0; iiter < maxiter; iiter++ ) {
    TVectorD cppos= makeControlpoints( bins, m0 );
    TMatrixD B= makeBasisSplineMatrix( bins, cppos );
    TMatrixD AB= _resm*B;
    TMatrixD ABT( AB );
    ABT.T();
    TMatrixD h= ABT*Vinv;
    TMatrixDSym H( Vinv );
    H.SimilarityT( AB );
    TVectorD Heigenvalues( m0 );
    TMatrixD Uh= H.EigenVectors( Heigenvalues );
    Double_t mineigenvalue= Heigenvalues.Min();
    if( mineigenvalue < 0.0 ) Heigenvalues-= 2.0*mineigenvalue;
    TMatrixD UhT( Uh );
    UhT.T();
    TMatrixDSym Dminushalf( m0 );
    for( Int_t ip= 0; ip < m0; ip++ ) {
      Dminushalf(ip,ip)= 1.0/sqrt(Heigenvalues[ip]);
    }
    TMatrixD UhDminushalfT= Dminushalf*UhT;
    TMatrixDSym Cp= makeCurvatureMatrix( m0 );
    Cp.Similarity( UhDminushalfT );
    TVectorD Cpeigenvalues( m0 );
    TMatrixD Ucp= Cp.EigenVectors( Cpeigenvalues );
    TMatrixD UcpT( Ucp );
    UcpT.T();
    TMatrixD Q= UcpT*Dminushalf*UhT*h;
    TVectorD qp= Q*y;
    Double_t opttaunoise= optTauNoise( Cpeigenvalues, qp );
    Double_t m0sum= 0.0;
    for( Int_t ip= 0; ip < m0; ip++ ) {
      m0sum+= 1.0/(1.0+opttaunoise*Cpeigenvalues[ip]);
    }
    Int_t m0noise= int( m0sum+2.0 );
    opttau= optTau( Cpeigenvalues, std::min(m0noise,m0-1) );
    if( verbose() >= 1 ) {
      cout << "RooUnfoldBasisSplines::findM0noise: Optimal tau, m0, tau from noise in q': " << opttaunoise 
	   << " " << m0noise << " " << opttau << " iteration " << iiter << endl;
    }
    m0final= m0noise;
    if( m0noise >= m0 ) break;
    m0= m0noise;
  }
  return m0final;
}
// Helper for numerical solution of tau from noise formula:
Double_t taufunNoise( Double_t* x, Double_t* par ) {
  Double_t tau= x[0];
  Int_t npar= par[0];
  Double_t sum= 0.0;
  for( Int_t ipar= 0; ipar < npar; ipar++ ) {
    Double_t tausii= tau*par[ipar+1];
    Double_t qp= par[npar+ipar+1];
    sum+= ( pow( qp, 2 )*( 2.0*tausii + pow( tausii, 2 ) )/
	    pow( 1.0+tausii, 2 ) - 1.0 )*( 3.0/( 3.0 + tausii ) );
  }
  return sum;
}
// Numerical solution of noise formula:
Double_t RooUnfoldBasisSplines::optTauNoise( const TVectorD& eigenvalues, 
					     const TVectorD& qprime ) {
  Int_t npar= eigenvalues.GetNrows();
  TF1 tautf1( "taufunNoise", &taufunNoise, 0.0, 0.001, 2*npar+1 );
  tautf1.SetParameter( 0, npar );
  for( Int_t ipar= 0; ipar < npar; ipar++ ) {
    tautf1.SetParameter( ipar+1, eigenvalues[ipar] );
    tautf1.SetParameter( npar+ipar+1, qprime[ipar] );
  }
  ROOT::Math::WrappedTF1 wf1( tautf1 );
  ROOT::Math::BrentRootFinder brf;
  brf.SetFunction( wf1, 0.0, 0.001 );
  brf.Solve();
  return brf.Root();
}
// Calculation of optimal tau from elements of diagonalised
// curvature matrix a la equ. (4.41) of Blobels CSC paper:
// (cdsweb.cern.ch/record/161223/files/CERN-85-09.pdf?version=1)
Double_t taufun( Double_t* x, Double_t* par ) {
  Double_t tau= x[0];
  Int_t npar= par[0];
  Double_t m0= par[1];
  Double_t sum= 0.0;
  for( Int_t ipar= 0; ipar < npar; ipar++ ) {
    sum+= 1.0/(1.0+tau*par[ipar+2]);
  }
  return sum-m0;
}
Double_t RooUnfoldBasisSplines::optTau( const TVectorD& eigenvalues, 
					Double_t m0 ) {
  Int_t npar= eigenvalues.GetNrows();
  TF1 tautf1( "taufun", &taufun, 0.0, 0.001, npar+2 );
  tautf1.SetParameter( 0, npar );
  tautf1.SetParameter( 1, m0 );
  for( Int_t ipar= 0; ipar < npar; ipar++ ) {
    tautf1.SetParameter( ipar+2, eigenvalues[ipar] );
  }
  ROOT::Math::WrappedTF1 wf1( tautf1 );
  ROOT::Math::BrentRootFinder brf;
  brf.SetFunction( wf1, 0.0, 0.001 );
  brf.Solve();
  return brf.Root();
}

// Place control points evenly over range of bins with one point 
// outside both range edges:
TVectorD RooUnfoldBasisSplines::makeControlpoints( const TVectorD& bins, 
						   Int_t np ) {
  Double_t d= (bins.Max()-bins.Min())/(np-3);
  TVectorD cppos( np );
  for( Int_t ip= 0; ip < np; ip++ ) {
    cppos[ip]= bins.Min() + d*(ip-1);
  }
  return cppos;
}

// Make error matrix of unfolded (calculation is done in unfold):
void RooUnfoldBasisSplines::GetCov() {
  return;
}

// Get settings (makes superclass happy):
void RooUnfoldBasisSplines::GetSettings() {
  _minparm=0;
  _maxparm=0;
  _stepsizeparm=0;
  _defaultparm=0;
}

// Matrix of basis splines integrated over bins:
TMatrixD RooUnfoldBasisSplines::makeBasisSplineMatrix( const TVectorD& bins, 
						       const TVectorD& cppos ) {
  TVectorD tbins= transformV( bins, cppos );
  Int_t nbin= tbins.GetNrows()-1;
  Int_t np= cppos.GetNrows();
  TMatrixD B( nbin, np );
  Double_t binw= step( tbins );
  for( Int_t ip= 0; ip < np; ip++ ) {
    for( Int_t ibin= 0; ibin < nbin; ibin++ ) {
      B(ibin,ip)= bfunint( tbins[ibin], tbins[ibin+1], ip )/binw;
    }
  }
  return B;
}

// Integrated basis spline for bin integrals:
Double_t RooUnfoldBasisSplines::bfunint( Double_t tmin, Double_t tmax, 
					 Double_t tshiftindx ) {
  if( tmin > tmax ) {
    cerr << "RooUnfoldBasisSplines::bfunint: tmin > tmax" << endl;
    return -999.0;
  }
  tmin-= tshiftindx;
  tmax-= tshiftindx;
  Double_t tlist[7]= { -999.0, -3.0, -2.0, -1.0, 0.0, 1.0, 999.0 };
  Double_t tvalues[2]= { tmin, tmax };
  Int_t tindex[2];
  for( Int_t i= 0; i < 2; i++ ) {
    Double_t t= tvalues[i];
    if( t < -3.0 ) tindex[i]= 0;
    else if( -3.0 <= t and t < -2.0 ) tindex[i]= 1;
    else if( -2.0 <= t and t < -1.0 ) tindex[i]= 2;
    else if( -1.0 <= t and t <  0.0 ) tindex[i]= 3;
    else if(  0.0 <= t and t <  1.0 ) tindex[i]= 4;
    else if(  1.0 <= t ) tindex[i]= 5;
  }
  Int_t itmin= 0;
  Int_t itmax= 1;
  Double_t result= 0.0;
  if( tindex[itmax] == tindex[itmin] ) {
    // Integrate from tmin to tmax:
    result= bfunintegrated( tmax, tindex[itmin] ) - 
      bfunintegrated( tmin, tindex[itmin] );
  }
  else if( tindex[itmax] > tindex[itmin] ) {
    // Integrate in first and last segment:
    result= bfunintegrated( tlist[tindex[itmin]+1], tindex[itmin] )
      - bfunintegrated( tmin, tindex[itmin] );
    result+= bfunintegrated( tmax, tindex[itmax] )  
      - bfunintegrated( tlist[tindex[itmax]], tindex[itmax] );
    // Integrate segments between first and last:
    for( Int_t k= tindex[itmin]+1; k < tindex[itmax]; k++ ) {
      result+= bfunintegrated( tlist[k+1], k )
	- bfunintegrated( tlist[k], k );
    }
  }
  return result/6.0;
}
Double_t RooUnfoldBasisSplines::bfunintegrated( Double_t t, 
						Int_t isegment ) {
  if( isegment == 0 ) return 0.0;
  else if( isegment == 1 ) return pow( t+3.0, 4 )/4.0;
  else if( isegment == 2 ) return - 0.75*pow( t, 4 ) - 5.0*pow( t, 3 ) 
			     - 10.5*pow( t, 2 ) - 5.0*t;
  else if( isegment == 3 ) return 0.75*pow( t, 4 ) + pow( t, 3 ) 
			     - 1.5*pow( t, 2 ) + t; 
  else if( isegment == 4 ) return - pow( 1.0-t, 4 )/4.0;
  else if( isegment == 5 ) return 0.0;
  return 0.0;
}

// Curvature matrix:
TMatrixDSym RooUnfoldBasisSplines::makeCurvatureMatrix( Int_t np ) {
  Double_t r0[4]= {  2.0, -3.0,  0.0,  1.0 };
  Double_t r1[5]= { -3.0,  8.0, -6.0,  0.0,  1.0 };
  Double_t r2[5]= { -6.0, 14.0, -9.0,  0.0,  1.0 };
  Double_t r0r[4]= { 1.0, 0.0, -3.0,  2.0 };
  Double_t r1r[5]= { 1.0, 0.0, -6.0,  8.0, -3.0 };
  Double_t r2r[5]= { 1.0, 0.0, -9.0, 14.0, -6.0 };
  Double_t rn[7]=  { 1.0, 0.0, -9.0, 16.0, -9.0, 0.0, 1.0 };
  TMatrixD mr0( 0, 0, 0, 3, r0 );
  TMatrixD mr1( 0, 0, 0, 4, r1 );
  TMatrixD mr2( 0, 0, 0, 4, r2 );
  TMatrixD mr0r( 0, 0, 0, 3, r0r );
  TMatrixD mr1r( 0, 0, 0, 4, r1r );
  TMatrixD mr2r( 0, 0, 0, 4, r2r );
  TMatrixD mrn( 0, 0, 0, 6, rn );
  TMatrixDSym C( np );
  for( Int_t irow= 0; irow < np; irow++ ) {
    if( irow == 0 ) C.SetSub( 0, 0, mr0 );
    else if( irow == 1 ) C.SetSub( 1, 0, mr1 );
    else if( irow == 2 ) C.SetSub( 2, 1, mr2 );
    else if( irow > 2 and irow < np-3 ) C.SetSub( irow, irow-3, mrn );
    else if( irow == np-3 ) C.SetSub( irow, np-6, mr2r );
    else if( irow == np-2 ) C.SetSub( irow, np-5, mr1r );
    else if( irow == np-1 ) C.SetSub( irow, np-4, mr0r );
  }
  return C;
}

// Transform vector of bin edge values to normalised range for basis splines:
TVectorD RooUnfoldBasisSplines::transformV( const TVectorD& bins, 
					    const TVectorD& cppos ) {
  Int_t nbin= bins.GetNrows();
  TVectorD tbins( nbin );
  for( Int_t ibin= 0; ibin < nbin; ibin++ ) {
    tbins[ibin]= transform( bins[ibin], cppos );
  }
  return tbins;
}
Double_t RooUnfoldBasisSplines::transform( Double_t t, 
					   const TVectorD& cppos ) {
  Double_t stepsize= step( cppos );
  Double_t offset= cppos.Min()/stepsize + 1.0;
  return t/stepsize - offset;
}
Double_t RooUnfoldBasisSplines::step( const TVectorD& vx ) {
  return (vx.Max()-vx.Min())/Double_t(vx.GetNrows()-1);
}
