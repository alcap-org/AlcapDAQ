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

#ifndef ROOUNFOLD_HH
#define ROOUNFOLD_HH

#include "TNamed.h"
#include "TVectorD.h"
#include "TMatrixD.h"
#include "RooUnfoldResponse.h"

class TH1;
class TH1D;

class RooUnfold : public TNamed {

public:

  enum Algorithm {       // Selection of unfolding algorithm:
    kNone,               //   no unfolding (dummy routines in RooUnfold)
    kBayes,              //   RooUnfoldBayes
    kSVD,                //   RooUnfoldSvd
    kBinByBin,           //   RooUnfoldBinByBin
    kTUnfold,            //   RooUnfoldTUnfold
    kInvert,             //   RooUnfoldInvert
    kDagostini,          //   RooUnfoldDagostini
    kIDS                 //   RooUnfoldIds
  };

  enum ErrorTreatment {  // Error treatment:
    kNoError,            //   no error treatment: returns sqrt(N)
    kErrors,             //   bin-by-bin errors (diagonal covariance matrix)
    kCovariance,         //   covariance matrix from unfolding
    kCovToy,             //   covariance matrix from toy MC
    kDefault=-1          //   not specified
  };

  static RooUnfold* New (Algorithm alg, const RooUnfoldResponse* res, const TH1* meas, Double_t regparm= -1e30,
                         const char* name= 0, const char* title= 0);

  // Standard methods

  RooUnfold(); // default constructor
  RooUnfold (const char*    name, const char*    title); // named constructor
  RooUnfold (const TString& name, const TString& title); // named constructor
  RooUnfold (const RooUnfold& rhs); // copy constructor
  virtual ~RooUnfold(); // destructor
  RooUnfold& operator= (const RooUnfold& rhs); // assignment operator
  virtual RooUnfold* Clone (const char* newname= 0) const;

  // Special constructors

  RooUnfold (const RooUnfoldResponse* res, const TH1* meas, const char* name= 0, const char* title= 0);

  // Set up an existing object

  virtual RooUnfold& Setup (const RooUnfoldResponse* res, const TH1* meas);
  virtual void SetMeasured (const TH1* meas);
  virtual void SetMeasured (const TVectorD& meas, const TMatrixD& cov);
  virtual void SetMeasured (const TVectorD& meas, const TVectorD& err);
  virtual void SetMeasuredCov (const TMatrixD& cov);
  virtual void SetResponse (const RooUnfoldResponse* res);
  virtual void SetResponse (RooUnfoldResponse* res, Bool_t takeOwnership);

  virtual void Reset ();

  // Accessors

  virtual const RooUnfoldResponse* response() const;
  virtual const TH1* Hmeasured() const;
  virtual       TH1* Hreco (ErrorTreatment withError=kErrors);
  const    TVectorD& Vmeasured() const;   // Measured distribution as a TVectorD
  const    TVectorD& Emeasured() const;   // Measured distribution errors as a TVectorD
  const    TMatrixD& GetMeasuredCov() const;   // Measured distribution covariance matrix

  virtual TVectorD&  Vreco();
  virtual TMatrixD   Ereco  (ErrorTreatment witherror=kCovariance);
  virtual TVectorD   ErecoV (ErrorTreatment witherror=kErrors);
  virtual TMatrixD   Wreco  (ErrorTreatment witherror=kCovariance);

  virtual Int_t      verbose() const;
  virtual void       SetVerbose (Int_t level);
  virtual void       IncludeSystematics (Int_t dosys= 1);
  virtual Int_t      SystematicsIncluded() const;
  virtual Int_t      NToys() const;         // Number of toys
  virtual void       SetNToys (Int_t toys); // Set number of toys
  virtual Int_t      Overflow() const;
  virtual void       PrintTable (std::ostream& o, const TH1* hTrue= 0, ErrorTreatment withError=kDefault);
  virtual void       SetRegParm (Double_t parm);
  virtual Double_t   GetRegParm() const; // Get Regularisation Parameter
  Double_t Chi2 (const TH1* hTrue,ErrorTreatment DoChi2=kCovariance);
  Double_t GetMinParm() const;
  Double_t GetMaxParm() const;
  Double_t GetStepSizeParm() const;
  Double_t GetDefaultParm() const;
  RooUnfold* RunToy() const;
  void Print(Option_t* opt="") const;

  static void PrintTable (std::ostream& o, const TH1* hTrainTrue, const TH1* hTrain,
                          const TH1* hTrue, const TH1* hMeas, const TH1* hReco,
                          Int_t _nm=0, Int_t _nt=0, Bool_t _overflow=kFALSE,
                          ErrorTreatment withError=kDefault, Double_t chi_squ=-999.0);

  static void PrintTable (std::ostream& o, const TVectorD& vTrainTrue, const TVectorD& vTrain,
                          const TVectorD& vMeas, const TVectorD& vReco, Int_t nm, Int_t nt);

protected:
  void Assign (const RooUnfold& rhs); // implementation of assignment operator
  virtual void SetNameTitleDefault();
  virtual void Unfold();
  virtual void GetErrors();
  virtual void GetCov(); // Get covariance matrix using errors on measured distribution
  virtual void GetErrMat(); // Get covariance matrix using errors from residuals on reconstructed distribution
  virtual void GetWgt(); // Get weight matrix using errors on measured distribution
  virtual void GetSettings();
  virtual Bool_t UnfoldWithErrors (ErrorTreatment withError, bool getWeights=false);

  static TMatrixD CutZeros     (const TMatrixD& ereco);
  static TH1D*    HistNoOverflow (const TH1* h, Bool_t overflow);
  static TMatrixD& ABAT (const TMatrixD& a, const TMatrixD& b, TMatrixD& c);
  static TMatrixD& ABAT (const TMatrixD& a, const TVectorD& b, TMatrixD& c);
  static TH1*     Resize (TH1* h, Int_t nx, Int_t ny=-1, Int_t nz=-1);
  static Int_t    InvertMatrix (const TMatrixD& mat, TMatrixD& inv, const char* name="matrix", Int_t verbose=1);

private:
  void Init();
  void Destroy();
  void CopyData (const RooUnfold& rhs);

protected:
  // instance variables
  Double_t _minparm;       // Minimum value to be used in RooUnfoldParms
  Double_t _maxparm;       // Maximum value to be used in RooUnfoldParms
  Double_t _stepsizeparm;  // StepSize value to be used in RooUnfoldParms
  Double_t _defaultparm;   // Recommended value for regularisation parameter
  Int_t    _verbose;       // Debug print level
  Int_t    _nm;            // Total number of measured bins (including under/overflows if _overflow set)
  Int_t    _nt;            // Total number of truth    bins (including under/overflows if _overflow set)
  Int_t    _overflow;      // Use histogram under/overflows if 1 (set from RooUnfoldResponse)
  Int_t    _NToys;         // Number of toys to be used
  Bool_t   _unfolded;      // unfolding done
  Bool_t   _haveCov;       // have _cov
  Bool_t   _haveWgt;       // have _wgt
  Bool_t   _have_err_mat;  // have _err_mat
  Bool_t   _fail;          // unfolding failed
  Bool_t   _haveErrors;    // have _variances
  Bool_t   _haveCovMes;    // _covMes was set, not just cached
  Int_t    _dosys;         // include systematic errors from response matrix? use _dosys=2 to exclude measurement errors
  const RooUnfoldResponse* _res;   // Response matrix (not owned)
  RooUnfoldResponse* _resmine;     // Owned response matrix
  const TH1*               _meas;  // Measured distribution (not owned)
  TH1*     _measmine;      // Owned measured histogram
  TVectorD _rec;           // Reconstructed distribution
  TMatrixD _cov;           // Reconstructed distribution covariance
  TMatrixD _wgt;           // Reconstructed distribution weights (inverse of _cov)
  TVectorD _variances;     // Error matrix diagonals
  TMatrixD _err_mat;       // Error matrix from toys
  mutable TVectorD* _vMes; //! Cached measured vector
  mutable TVectorD* _eMes; //! Cached measured error
  mutable TMatrixD* _covMes;       // Measurement covariance matrix
  mutable TMatrixD* _covL; //! Cached lower triangular matrix for which _covMes = _covL * _covL^T.
  ErrorTreatment _withError; // type of error last calulcated

public:

  ClassDef (RooUnfold, 2) // Unfolding base class: implementations in RooUnfoldBayes, RooUnfoldSvd, RooUnfoldBinByBin, RooUnfoldTUnfold, RooUnfoldInvert, RooUnfoldIds
};

//==============================================================================
// Inline method definitions
//==============================================================================

inline
RooUnfold::RooUnfold()
  : TNamed()
{
  // Default constructor. Use Setup() to prepare for unfolding.
  Init();
}

inline
RooUnfold::RooUnfold (const char*    name, const char*    title)
  : TNamed(name,title)
{
  // Basic named constructor. Use Setup() to prepare for unfolding.
  Init();
}

inline
RooUnfold::RooUnfold (const TString& name, const TString& title)
  : TNamed(name,title)
{
  // Basic named constructor. Use Setup() to prepare for unfolding.
  Init();
}

inline
RooUnfold::~RooUnfold()
{
  Destroy();
}

inline
RooUnfold& RooUnfold::operator= (const RooUnfold& rhs)
{
  // Assignment operator for copying RooUnfold settings.
  Assign(rhs);
  return *this;
}

inline
Int_t RooUnfold::verbose() const
{
  // Get verbosity setting which controls amount of information to be printed
  return _verbose;
}

inline
Int_t RooUnfold::NToys()     const
{
  // Get number of toys used in kCovToy error calculation.
  return _NToys;
}

inline
Int_t RooUnfold::Overflow()  const
{
  // Histogram under/overflow bins are used?
  return _overflow;
}

inline
const RooUnfoldResponse* RooUnfold::response()  const
{
   // Response matrix object
  return _res;
}

inline
const TH1*               RooUnfold::Hmeasured() const
{
  // Measured Distribution as a histogram
  return _meas;
}

inline
TVectorD&                RooUnfold::Vreco()
{
  // Unfolded (reconstructed) distribution as a vector
  if (!_unfolded) {
    if (!_fail) Unfold();
    if (!_unfolded) {
      _fail= true;
      if (_nt > 0 && _rec.GetNrows() == 0) _rec.ResizeTo(_nt);   // need something
    }
  }
  return _rec;
}

inline
const TVectorD&          RooUnfold::Vmeasured() const
{
  // Measured distribution as a vector.
  if (!_vMes)
    _vMes= RooUnfoldResponse::H2V  (_meas, _res->GetNbinsMeasured(), _overflow);
  return *_vMes;
}

inline
const TVectorD&          RooUnfold::Emeasured() const
{
  // Measured errors as a vector.
  if (!_eMes)
    _eMes= RooUnfoldResponse::H2VE (_meas, _res->GetNbinsMeasured(), _overflow);
  return *_eMes;
}

inline
void  RooUnfold::SetVerbose (Int_t level)
{
  // Set verbosity level which controls amount of information to be printed
  _verbose= level;
}

inline
void  RooUnfold::SetNToys (Int_t toys)
{
  // Set number of toys used in kCovToy error calculation.
  _NToys= toys;
}

inline
void  RooUnfold::SetRegParm (Double_t)
{
  // Set Regularisation parameter
}

inline
Double_t RooUnfold::GetRegParm() const
{
  // Get regularisation parameter.
  return -1;
}

inline
void RooUnfold::IncludeSystematics (Int_t dosys)
{
  // Include systematic errors from response matrix?
  // Use dosys=2 to exclude measurement errors.
  if (dosys!=_dosys) _haveWgt= _haveErrors= _haveCov= _have_err_mat= kFALSE;
  _dosys= dosys;
}

inline
Int_t RooUnfold::SystematicsIncluded() const
{
  // return setting for whether to include systematic errors from response matrix
  return _dosys;
}

#endif
