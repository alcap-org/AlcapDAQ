//=====================================================================-*-C++-*-
// File and Version Information:
//      $Id: RooUnfoldBasisSplines.h $
//
// Description:
//      Unfolding class implementing Blobels RUN algorithm
//      using uniform cubic basis splines
//
// Author: Stefan Kluth (skluth@mpp.mpg.de)
//
//==============================================================================

#ifndef ROOUNFOLDBASISSPLINES_H_
#define ROOUNFOLDBASISSPLINES_H_

#include "RooUnfold.h"

class testHelperRooUnfoldBasisSplines;

class RooUnfoldResponse;
class TH1;
class TH1D;
class TH2D;

#include "TMatrixD.h"
#include "TMatrixDSym.h"
#include "TVectorD.h"

class RooUnfoldBasisSplines : public RooUnfold {

  // friend class to get an handle on the private members for testing
  friend class testHelperRooUnfoldBasisSplines;

public:

  // Ctors:
  RooUnfoldBasisSplines(); 
  RooUnfoldBasisSplines( const char* name, const char* title );
  RooUnfoldBasisSplines( const TString& name, const TString& title );
  RooUnfoldBasisSplines( const RooUnfoldBasisSplines& );
  RooUnfoldBasisSplines( const RooUnfoldResponse* res, const TH1* meas, 
			 Int_t nrebin=1,
			 const Double_t tau=1.0e-8, const Int_t m0=0,
			 const Int_t iauto=0,
			 const Double_t tol=1.0e-12,
			 const char* name=0, const char* title=0 );

  // Dtor:
  virtual ~RooUnfoldBasisSplines();
  virtual void Reset();

  // Cloning:
  virtual RooUnfoldBasisSplines* Clone ( const char* newname= 0 ) const;

  // Assignment:
  RooUnfoldBasisSplines& operator= ( const RooUnfoldBasisSplines& rhs );

  const TMatrixD* Impl();

  // Basis splines integrated and derivative
  // for calculation of basis spline matrices
  TMatrixD makeBasisSplineMatrix( const TVectorD& bins, const TVectorD& cppos );
  Double_t bfunint( Double_t tmin, Double_t tmax, Double_t tshiftindx=0.0 );
  Double_t bfunintegrated( Double_t t, Int_t isegment );
  TMatrixDSym makeCurvatureMatrix( Int_t np );
  TVectorD makeControlpoints ( const TVectorD& bins, Int_t np );
  TVectorD transformV( const TVectorD& bins, const TVectorD& cppos );
  Double_t transform( Double_t t, const TVectorD& vx );
  Double_t step( const TVectorD& vx );
  Double_t optTau( const TVectorD& eigenvalues, Double_t m0 );
  Double_t optTauNoise( const TVectorD& eigenvalues, 
			const TVectorD& qprime );
  Int_t findM0noise( const TVectorD& bins, const TVectorD& y, 
		     const TMatrixDSym& Vinv, 
		     Int_t npstart, 
		     Double_t& opttau,
		     Int_t maxiter=10 );
  // Double_t findTauFromNoise( const TVectorD& bins, 
  Double_t findTauFromNoise( const TMatrixD& AB, 
			     const TVectorD& y, 
			     const TMatrixDSym& Vinv, 
			     Int_t np, TVectorD& );
  Int_t m0FromTau( Double_t opttaunoise, const TVectorD& Cpeigenvalues,
		   Int_t offset=1 );
  TMatrixD makeRebinMatrix( Int_t nbin, Int_t nrebin );


  // Overrides from RooUnfold:
  virtual void PrintTable( std::ostream& o, const TH1* hTrue, 
			   ErrorTreatment withError );
  virtual TH1* Hreco( ErrorTreatment withError );
  virtual TH1* HrecoMeasured();
  // Set/get Regularisation Parameter tau:
  virtual void SetRegParm( Double_t tau );
  virtual Double_t GetRegParm() const; 
  Double_t Chi2measured();


protected:

  virtual void Unfold();
  virtual void GetCov();
  virtual void GetSettings();

private:

  // Helpers:
  void Init();
  void SubtractFakes();

  // Instance variables:
  TMatrixD _resm;
  Int_t _nrebin;
  Double_t _tau;
  Int_t _m0;
  Int_t _iauto;
  Double_t _tol;
  TVectorD _measured;
  TVectorD _reconstructed;
  TMatrixDSym _vinv;

public:
  ClassDef( RooUnfoldBasisSplines, 1 )

};


#endif


