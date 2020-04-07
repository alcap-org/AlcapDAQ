//=====================================================================-*-C++-*-
// File and Version Information:
//      $Id$
//
// Description:
//      Response Matrix
//
// Author: Tim Adye <T.J.Adye@rl.ac.uk>
//
//==============================================================================

#ifndef ROOUNFOLDRESPONSE_HH
#define ROOUNFOLDRESPONSE_HH

#include "TNamed.h"
#include "TMatrixD.h"
#include "TH1.h"
#if ROOT_VERSION_CODE >= ROOT_VERSION(5,0,0)
#include "TVectorDfwd.h"
#else
class TVectorD;
#endif
class TF1;
class TH2;
class TH2D;
class TAxis;
class TCollection;

class RooUnfoldResponse : public TNamed {

public:

  // Standard methods

  RooUnfoldResponse(); // default constructor
  RooUnfoldResponse (const char* name, const char* title); // named constructor
  RooUnfoldResponse (const TString& name, const TString& title); // named constructor
  RooUnfoldResponse (const RooUnfoldResponse& rhs); // copy constructor
  virtual ~RooUnfoldResponse(); // destructor
  virtual RooUnfoldResponse& operator= (const RooUnfoldResponse& rhs); // assignment operator

  // Special constructors

  RooUnfoldResponse (Int_t nb, Double_t xlo, Double_t xhi, const char* name= 0, const char* title= 0);  // constructor -  simple 1D case with same binning, measured vs truth
  RooUnfoldResponse (Int_t nm, Double_t mlo, Double_t mhi, Int_t nt, Double_t tlo, Double_t thi, const char* name= 0, const char* title= 0);  // constructor -  simple 1D case
  RooUnfoldResponse (const TH1* measured, const TH1* truth, const char* name= 0, const char* title= 0);  // constructor - measured and truth only used for shape
  RooUnfoldResponse (const TH1* measured, const TH1* truth, const TH2* response, const char* name= 0, const char* title= 0);  // create from already-filled histograms

  // Set up an existing object

  virtual RooUnfoldResponse& Reset ();  // clear an existing object
  virtual RooUnfoldResponse& Setup (const RooUnfoldResponse& rhs);  // set up based on another instance
  virtual RooUnfoldResponse& Setup (Int_t nb, Double_t xlo, Double_t xhi);  // set up simple 1D case with same binning, measured vs truth
  virtual RooUnfoldResponse& Setup (Int_t nm, Double_t mlo, Double_t mhi, Int_t nt, Double_t tlo, Double_t thi);  // set up simple 1D case
  virtual RooUnfoldResponse& Setup (const TH1* measured, const TH1* truth);  // set up - measured and truth only used for shape
  virtual RooUnfoldResponse& Setup (const TH1* measured, const TH1* truth, const TH2* response);  // set up from already-filled histograms

  // Fill with training data

  virtual Int_t Fill (Double_t xr, Double_t xt, Double_t w= 1.0);  // Fill 1D Response Matrix
  virtual Int_t Fill (Double_t xr, Double_t yr, Double_t xt, Double_t yt, Double_t w= 1.0);  // Fill 2D Response Matrix
  virtual Int_t Fill (Double_t xr, Double_t yr, Double_t zr, Double_t xt, Double_t yt, Double_t zt, Double_t w= 1.0);  // Fill 3D Response Matrix

          Int_t Miss (Double_t xt);  // Fill missed event into 1D Response Matrix
          Int_t Miss (Double_t xt, Double_t w);  // Fill missed event into 1D (with weight) or 2D Response Matrix
          Int_t Miss (Double_t xt, Double_t yt, Double_t w);  // Fill missed event into 2D (with weight) or 3D Response Matrix
  virtual Int_t Miss (Double_t xt, Double_t yt, Double_t zt, Double_t w);  // Fill missed event into 3D Response Matrix

          Int_t Fake (Double_t xr);  // Fill fake event into 1D Response Matrix
          Int_t Fake (Double_t xr, Double_t w);  // Fill fake event into 1D (with weight) or 2D Response Matrix
          Int_t Fake (Double_t xr, Double_t yr, Double_t w);  // Fill fake event into 2D (with weight) or 3D Response Matrix
  virtual Int_t Fake (Double_t xr, Double_t yr, Double_t zr, Double_t w);  // Fill fake event into 3D Response Matrix

  virtual void Add (const RooUnfoldResponse& rhs);
  virtual Long64_t Merge (TCollection* others);

  // Accessors

  Int_t        GetDimensionMeasured() const;   // Dimensionality of the measured distribution
  Int_t        GetDimensionTruth()    const;   // Dimensionality of the truth distribution
  Int_t        GetNbinsMeasured()     const;   // Total number of bins in the measured distribution
  Int_t        GetNbinsTruth()        const;   // Total number of bins in the truth distribution

  const TH1*   Hmeasured()            const;   // Measured distribution, including fakes
  TH1*         Hmeasured();                    // Measured distribution, including fakes
  const TH1*   Hfakes()               const;   // Fakes distribution
  TH1*         Hfakes();                       // Fakes distribution
  const TH1*   Htruth()               const;   // Truth distribution, used for normalisation
  TH1*         Htruth();                       // Truth distribution, used for normalisation
  const TH2*   Hresponse()            const;   // Response matrix as a 2D-histogram: (x,y)=(measured,truth)
  TH2*         Hresponse();                    // Response matrix as a 2D-histogram: (x,y)=(measured,truth)
  TH2D*        HresponseNoOverflow()  const;   // Response matrix with under/overflow bins moved into histogram body

  const TVectorD& Vmeasured()         const;   // Measured distribution as a TVectorD
  const TVectorD& Emeasured()         const;   // Measured distribution errors as a TVectorD
  const TVectorD& Vfakes()            const;   // Fakes distribution as a TVectorD
  const TVectorD& Vtruth()            const;   // Truth distribution as a TVectorD
  const TVectorD& Etruth()            const;   // Truth distribution errors as a TVectorD
  const TMatrixD& Mresponse()         const;   // Response matrix as a TMatrixD: (row,column)=(measured,truth)
  const TMatrixD& Eresponse()         const;   // Response matrix errors as a TMatrixD: (row,column)=(measured,truth)

  Double_t operator() (Int_t r, Int_t t) const;// Response matrix element (measured,truth)

  void   UseOverflow (Bool_t set= kTRUE);      // Specify to use overflow bins
  Bool_t UseOverflowStatus() const;            // Get UseOverflow setting
  Double_t FakeEntries() const;                // Return number of bins with fakes
  virtual void Print (Option_t* option="") const;

  static TH1D*     H2H1D(const TH1*  h, Int_t nb);
  static TVectorD* H2V  (const TH1*  h, Int_t nb, Bool_t overflow= kFALSE);
  static TVectorD* H2VE (const TH1*  h, Int_t nb, Bool_t overflow= kFALSE);
  static TMatrixD* H2M  (const TH2*  h, Int_t nx, Int_t ny, const TH1* norm= 0, Bool_t overflow= kFALSE);
  static TMatrixD* H2ME (const TH2*  h, Int_t nx, Int_t ny, const TH1* norm= 0, Bool_t overflow= kFALSE);
  static void      V2H  (const TVectorD& v, TH1* h, Int_t nb, Bool_t overflow= kFALSE);
  static Int_t   FindBin(const TH1*  h, Double_t x);  // return vector index for bin containing (x)
  static Int_t   FindBin(const TH1*  h, Double_t x, Double_t y);  // return vector index for bin containing (x,y)
  static Int_t   FindBin(const TH1*  h, Double_t x, Double_t y, Double_t z);  // return vector index for bin containing (x,y,z)
  static Int_t   GetBin (const TH1*  h, Int_t i, Bool_t overflow= kFALSE);  // vector index (0..nx*ny-1) -> multi-dimensional histogram global bin number (0..(nx+2)*(ny+2)-1) skipping under/overflow bins
  static Double_t GetBinContent (const TH1* h, Int_t i, Bool_t overflow= kFALSE); // Bin content by vector index
  static Double_t GetBinError   (const TH1* h, Int_t i, Bool_t overflow= kFALSE); // Bin error   by vector index
  static void PrintMatrix (const TMatrixD& m, const char* name="matrix", const char* format=0, Int_t cols_per_sheet=10);

  TH1* ApplyToTruth (const TH1* truth= 0, const char* name= "AppliedResponse") const; // If argument is 0, applies itself to its own truth
  TF1* MakeFoldingFunction (TF1* func, Double_t eps=1e-12, Bool_t verbose=false) const;

  RooUnfoldResponse* RunToy() const;

private:

  virtual RooUnfoldResponse& Init();
  virtual RooUnfoldResponse& Setup();
  virtual void ClearCache();
  virtual void SetNameTitleDefault (const char* defname= 0, const char* deftitle= 0);
  virtual Int_t Miss1D (Double_t xt, Double_t w= 1.0);  // Fill missed event into 1D Response Matrix (with weight)
  virtual Int_t Miss2D (Double_t xt, Double_t yt, Double_t w= 1.0);  // Fill missed event into 2D Response Matrix (with weight)
  virtual Int_t Fake1D (Double_t xr, Double_t w= 1.0);  // Fill fake event into 1D Response Matrix (with weight)
  virtual Int_t Fake2D (Double_t xr, Double_t yr, Double_t w= 1.0);  // Fill fake event into 2D Response Matrix (with weight)

  static Int_t GetBinDim (const TH1* h, Int_t i);
  static void ReplaceAxis(TAxis* axis, const TAxis* source);

  // instance variables

  Int_t _mdim;     // Number of measured  dimensions
  Int_t _tdim;     // Number of truth     dimensions
  Int_t _nm;       // Total number of measured  bins (not counting under/overflows)
  Int_t _nt;       // Total number of truth     bins (not counting under/overflows)
  TH1*  _mes;      // Measured histogram
  TH1*  _fak;      // Fakes    histogram
  TH1*  _tru;      // Truth    histogram
  TH2*  _res;      // Response histogram
  Int_t _overflow; // Use histogram under/overflows if 1

  mutable TVectorD* _vMes;   //! Cached measured vector
  mutable TVectorD* _eMes;   //! Cached measured error
  mutable TVectorD* _vFak;   //! Cached fakes    vector
  mutable TVectorD* _vTru;   //! Cached truth    vector
  mutable TVectorD* _eTru;   //! Cached truth    error
  mutable TMatrixD* _mRes;   //! Cached response matrix
  mutable TMatrixD* _eRes;   //! Cached response error
  mutable Bool_t    _cached; //! We are using cached vectors/matrices

public:

  ClassDef (RooUnfoldResponse, 1) // Respose Matrix
};

// Inline method definitions

inline
RooUnfoldResponse::RooUnfoldResponse()
  : TNamed()
{
  // RooUnfoldResponse default constructor. Use Setup() to set values.
  Init();
}

inline
RooUnfoldResponse::RooUnfoldResponse (const char*    name, const char*    title)
  : TNamed(name,title)
{
  // RooUnfoldResponse default named constructor. Use Setup() to set values.
  Init();
}

inline
RooUnfoldResponse::RooUnfoldResponse (const TString& name, const TString& title)
  : TNamed(name,title)
{
  // RooUnfoldResponse default named constructor. Use Setup() to set values.
  Init();
}

inline
RooUnfoldResponse::~RooUnfoldResponse()
{
  // RooUnfoldResponse destructor
  Reset();
}


inline
RooUnfoldResponse& RooUnfoldResponse::Setup (Int_t nb, Double_t xlo, Double_t xhi)
{
  // constructor -  simple 1D case with same binning, measured vs truth
  return Setup (nb, xlo, xhi, nb, xlo, xhi);
}


inline
Int_t RooUnfoldResponse::GetDimensionMeasured() const
{
  // Dimensionality of the measured distribution (1=1D, 2=2D, 3=3D)
  return _mdim;
}

inline
Int_t RooUnfoldResponse::GetDimensionTruth() const
{
  // Dimensionality of the truth distribution (1=1D, 2=2D, 3=3D)
  return _tdim;
}

inline
Int_t RooUnfoldResponse::GetNbinsMeasured() const
{
  // Total number of bins in the measured distribution
  return _nm;
}

inline
Int_t RooUnfoldResponse::GetNbinsTruth() const
{
  // Total number of bins in the truth distribution
  return _nt;
}


inline
const TH1* RooUnfoldResponse::Hmeasured() const
{
  // Measured distribution, including fakes
  return _mes;
}


inline
TH1*         RooUnfoldResponse::Hmeasured()
{
  return _mes;
}


inline
const TH1* RooUnfoldResponse::Hfakes() const
{
  // Fakes distribution
  return _fak;
}


inline
TH1*         RooUnfoldResponse::Hfakes()
{
  return _fak;
}

inline
const TH1*   RooUnfoldResponse::Htruth() const
{
  // Truth distribution, used for normalisation
  return _tru;
}

inline
TH1*         RooUnfoldResponse::Htruth()
{
  return _tru;
}

inline
const TH2*   RooUnfoldResponse::Hresponse() const
{
  // Response matrix as a 2D-histogram: (x,y)=(measured,truth)
  return _res;
}

inline
TH2*         RooUnfoldResponse::Hresponse()
{
  return _res;
}


inline
const TVectorD& RooUnfoldResponse::Vmeasured() const
{
  // Measured distribution as a TVectorD
  if (!_vMes) _cached= (_vMes= H2V  (_mes, _nm, _overflow));
  return *_vMes;
}

inline
const TVectorD& RooUnfoldResponse::Vfakes() const
{
  // Fakes distribution as a TVectorD
  if (!_vFak) _cached= (_vFak= H2V  (_fak, _nm, _overflow));
  return *_vFak;
}

inline
const TVectorD& RooUnfoldResponse::Emeasured() const
{
  // Measured distribution errors as a TVectorD
  if (!_eMes) _cached= (_eMes= H2VE (_mes, _nm, _overflow));
  return *_eMes;
}

inline
const TVectorD& RooUnfoldResponse::Vtruth() const
{
  // Truth distribution as a TVectorD
  if (!_vTru) _cached= (_vTru= H2V  (_tru, _nt, _overflow)); 
  return *_vTru;
}

inline
const TVectorD& RooUnfoldResponse::Etruth() const
{
  // Truth distribution errors as a TVectorD
  if (!_eTru) _cached= (_eTru= H2VE (_tru, _nt, _overflow)); 
  return *_eTru;
}

inline
const TMatrixD& RooUnfoldResponse::Mresponse() const
{
  // Response matrix as a TMatrixD: (row,column)=(measured,truth)
  if (!_mRes) _cached= (_mRes= H2M  (_res, _nm, _nt, _tru, _overflow)); 
  return *_mRes;
}

inline
const TMatrixD& RooUnfoldResponse::Eresponse() const
{
  // Response matrix errors as a TMatrixD: (row,column)=(measured,truth)
  if (!_eRes) _cached= (_eRes= H2ME (_res, _nm, _nt, _tru, _overflow)); 
  return *_eRes;
}


inline
Double_t RooUnfoldResponse::operator() (Int_t r, Int_t t) const
{
  // Response matrix element (measured,truth)
  return Mresponse()(r,t);
}

inline
Int_t    RooUnfoldResponse::GetBin (const TH1* h, Int_t i, Bool_t overflow)
{
  // vector index (0..nx*ny-1) -> multi-dimensional histogram
  // global bin number (0..(nx+2)*(ny+2)-1) skipping under/overflow bins
  return (h->GetDimension()<2) ? i+(overflow ? 0 : 1) : GetBinDim(h,i);
}

inline
Double_t RooUnfoldResponse::GetBinContent (const TH1* h, Int_t i, Bool_t overflow)
{
  // Bin content by vector index
  return h->GetBinContent (GetBin (h, i, overflow));
}

inline
Double_t RooUnfoldResponse::GetBinError   (const TH1* h, Int_t i, Bool_t overflow)
{
  // Bin error   by vector index
  return h->GetBinError   (GetBin (h, i, overflow));
}


inline
Int_t RooUnfoldResponse::Miss (Double_t xt)
{
  // Fill missed event into 1D Response Matrix
  return Miss1D(xt);
}

inline
Int_t RooUnfoldResponse::Miss (Double_t xt, Double_t w)
{
  // Fill missed event into 1D (with weight) or 2D Response Matrix
  return _tdim==2 ? Miss2D(xt,w) : Miss1D(xt,w);
}

inline
Int_t RooUnfoldResponse::Miss (Double_t xt, Double_t yt, Double_t w)
{
  // Fill missed event into 2D (with weight) or 3D Response Matrix
  return _tdim==3 ? Miss(xt,yt,w,1.0) : Miss2D(xt,yt,w);
}


inline
Int_t RooUnfoldResponse::Fake (Double_t xr)
{
  // Fill fake event into 1D Response Matrix
  return Fake1D(xr);
}

inline
Int_t RooUnfoldResponse::Fake (Double_t xr, Double_t w)
{
  // Fill fake event into 1D (with weight) or 2D Response Matrix
  return _mdim==2 ? Fake2D(xr,w) : Fake1D(xr,w);
}

inline
Int_t RooUnfoldResponse::Fake (Double_t xr, Double_t yr, Double_t w)
{
  // Fill fake event into 2D (with weight) or 3D Response Matrix
  return _mdim==3 ? Fake(xr,yr,w,1.0) : Fake2D(xr,yr,w);
}


inline
void RooUnfoldResponse::UseOverflow (Bool_t set)
{
  // Specify to use overflow bins. Only supported for 1D truth and measured distributions.
  _overflow= (set ? 1 : 0);
}

inline
Bool_t RooUnfoldResponse::UseOverflowStatus() const
{
  // Get UseOverflow setting
  return _overflow;
}

inline
Double_t RooUnfoldResponse::FakeEntries() const
{
  // Return number of fake entries
  return _fak ? _fak->GetEntries() : 0.0;
}

inline
Int_t RooUnfoldResponse::FindBin (const TH1* h, Double_t x)
{
  // Return vector index for bin containing (x)
  return h->GetXaxis()->FindBin(x) - 1;
}

#endif
