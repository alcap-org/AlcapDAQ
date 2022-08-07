// Author: Bogdan Malaescu <bogdan.malaescu@cern.ch>
// Author: Christopher Meyer <chris.meyer@cern.ch>
//
// Inspired by Tim Adye code for RooUnfoldSvd
// For support, contact: chris.meyer@cern.ch

#ifndef IDSUnfoldingTool_RooUnfoldIds
#define IDSUnfoldingTool_RooUnfoldIds

#include "RooUnfold.h"

class RooUnfoldResponse;
class TH1;
class TH1D;
class TH2D;
class TRandom3;

class RooUnfoldIds : public RooUnfold {

public:
   RooUnfoldIds(); // default constructor
   RooUnfoldIds(const char *name, const char *title); // named constructor
   RooUnfoldIds(const TString &name, const TString &title); // named constructor
   RooUnfoldIds(const RooUnfoldIds &rhs); // copy constructor
   virtual ~RooUnfoldIds(); // destructor
   RooUnfoldIds &operator=(const RooUnfoldIds &rhs); // assignment operator
   virtual RooUnfoldIds* Clone(const char *newname = NULL) const;

   // Special constructors

   RooUnfoldIds(const RooUnfoldResponse *res, const TH1 *meas, Int_t niter = 1);

   // Method-neutral method SetRegParm just calls SetNIter
   virtual void  SetRegParm (Double_t parm);
   virtual Double_t GetRegParm() const;

   void SetNIter(Int_t niter);
   Int_t GetNIter() const;

   void SetLambdaM(Double_t lambdaM);
   Double_t GetLambdaM() const;

   void SetLambdaU(Double_t lambdaU);
   Double_t GetLambdaU() const;

   void SetLambdaL(Double_t lambdaL);
   Double_t GetLambdaL() const;

   void SetLambdaS(Double_t lambdaS);
   Double_t GetLambdaS() const;

   virtual void Reset();

   TH2D* GetUnfoldCovMatrix(const TH2D *cov, Int_t ntoys, Int_t seed = 1);
   TH2D* GetAdetCovMatrix(Int_t ntoys, Int_t seed = 1);

protected:
   void Assign(const RooUnfoldIds &rhs); // implementation of assignment operator
   virtual void Unfold();
   virtual void GetCov();
   virtual void GetSettings();

private:
   void Init();
   void Destroy();
   void CopyData(const RooUnfoldIds &rhs);

   TH1* GetIDSUnfoldedSpectrum(const TH1 *h_RecoMC, const TH1 *h_TruthMC, const TH2 *h_2DSmear, const TH1 *h_RecoData, Int_t iter);
   Double_t Probability(Double_t deviation, Double_t sigma, Double_t lambda);
   Double_t MCnormalizationCoeff(const TVectorD *vd, const TVectorD *errvd, const TVectorD *vRecmc, const Int_t dim, const Double_t estNknownd, const Double_t Nmc, const Double_t lambda, const TVectorD *soustr_ );
   Double_t MCnormalizationCoeffIter(const TVectorD *vd, const TVectorD *errvd, const TVectorD *vRecmc, const Int_t dim, const Double_t estNknownd, const Double_t Nmc, const TVectorD *soustr_, Double_t lambdaN = 0., Int_t NiterMax = 5, Int_t messAct = 1);
   void IdsUnfold(const TVectorD &b, const TVectorD &errb, const TMatrixD &A, const Int_t dim, const Double_t lambda, TVectorD *soustr_, TVectorD *unf);
   void ComputeSoustrTrue(const TMatrixD *A, const TVectorD *unfres, const TVectorD *unfresErr, Int_t N, TVectorD *soustr_, Double_t lambdaS);
   void ModifyMatrix(TMatrixD *Am, const TMatrixD *A, const TVectorD *unfres, const TVectorD *unfresErr, Int_t N, const Double_t lambdaM_, TVectorD *soustr_, const Double_t lambdaS_);
   void PerformIterations(const TVectorD &data, const TVectorD &dataErr, const TMatrixD &A_, const Int_t &N_, Double_t lambdaL_, Int_t NstepsOptMin_, Double_t lambdaU_, Double_t lambdaM_, Double_t lambdaS_, TVectorD* unfres1IDS_, TVectorD* unfres2IDS_);
   TMatrixD* GetSqrtMatrix(const TMatrixD& covMat);
   void GenGaussRnd(TArrayD& v, const TMatrixD& sqrtMat, TRandom3& R);

protected:
   Int_t _niter;
   Int_t _nb;

   Double_t _lambdaL; // initial unfolding regularization (before folding matrix improvement)
   Double_t _lambdaUmin; // regularize Unfolding
   Double_t _lambdaMmin; // regularize Modification of folding matrix
   Double_t _lambdaS; // regularize background Subtraction

   TH1D *_meas1d, *_train1d, *_truth1d;
   TH2D *_reshist;

public:
   ClassDef(RooUnfoldIds, 1)
};

// Inline method definitions

inline
RooUnfoldIds::RooUnfoldIds()
: RooUnfold()
{
   // Default constructor. Use Setup() to prepare for unfolding.
   Init();
}

inline
RooUnfoldIds::RooUnfoldIds(const char *name, const char *title)
: RooUnfold(name, title)
{
   // Basic named constructor. Use Setup() to prepare for unfolding.
   Init();
}

inline
RooUnfoldIds::RooUnfoldIds(const TString &name, const TString &title)
: RooUnfold(name, title)
{
   // Basic named constructor. Use Setup() to prepare for unfolding.
   Init();
}

inline
RooUnfoldIds& RooUnfoldIds::operator=(const RooUnfoldIds &rhs)
{
   // Assignment operator for copying RooUnfoldIds settings.
   Assign(rhs);
   return *this;
}

inline
RooUnfoldIds::~RooUnfoldIds()
{
   Destroy();
}

inline
void  RooUnfoldIds::SetRegParm (Double_t parm)
{
  // Set regularisation parameter (number of iterations)
  SetNIter(Int_t(parm+0.5));
}

inline
Double_t RooUnfoldIds::GetRegParm() const
{
  // Return regularisation parameter (number of iterations)
  return GetNIter();
}

inline
void RooUnfoldIds::SetNIter(Int_t niter)
{
   // Set number of iterations
   _niter = niter;
}

inline
Int_t RooUnfoldIds::GetNIter() const
{
   // Return number of iterations
   return _niter;
}

inline
void RooUnfoldIds::SetLambdaM(Double_t lambdaM)
{
   // Set number of iterations
   _lambdaMmin = lambdaM;
}

inline
Double_t RooUnfoldIds::GetLambdaM() const
{
   // Return number of iterations
   return _lambdaMmin;
}

inline
void RooUnfoldIds::SetLambdaU(Double_t lambdaU)
{
   // Set number of iterations
   _lambdaUmin = lambdaU;
}

inline
Double_t RooUnfoldIds::GetLambdaU() const
{
   // Return number of iterations
   return _lambdaUmin;
}

inline
void RooUnfoldIds::SetLambdaL(Double_t lambdaL)
{
   // Set number of iterations
   _lambdaL = lambdaL;
}

inline
Double_t RooUnfoldIds::GetLambdaL() const
{
   // Return number of iterations
   return _lambdaL;
}

inline
void RooUnfoldIds::SetLambdaS(Double_t lambdaS)
{
   // Set number of iterations
   _lambdaS = lambdaS;
}

inline
Double_t RooUnfoldIds::GetLambdaS() const
{
   // Return number of iterations
   return _lambdaS;
}

#endif // IDSUnfoldingTool_RooUnfoldIds
