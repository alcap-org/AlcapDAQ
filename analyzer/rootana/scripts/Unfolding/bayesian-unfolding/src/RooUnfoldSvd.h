//=====================================================================-*-C++-*-
// File and Version Information:
//      $Id$
//
// Description:
//      SVD unfolding. Just an interface to RooUnfHistoSvd.
//
// Author: Tim Adye <T.J.Adye@rl.ac.uk>
//
//==============================================================================

#ifndef ROOUNFOLDSVD_HH
#define ROOUNFOLDSVD_HH

#include "RooUnfold.h"
#if defined(HAVE_TSVDUNFOLD) || ROOT_VERSION_CODE < ROOT_VERSION(5,34,99)
#define TSVDUnfold TSVDUnfold_130729
#endif

class RooUnfoldResponse;
class TH1;
class TH1D;
class TH2D;
class TSVDUnfold;

class RooUnfoldSvd : public RooUnfold {

public:

  // Standard methods

  RooUnfoldSvd(); // default constructor
  RooUnfoldSvd (const char*    name, const char*    title); // named constructor
  RooUnfoldSvd (const TString& name, const TString& title); // named constructor
  RooUnfoldSvd (const RooUnfoldSvd& rhs); // copy constructor
  virtual ~RooUnfoldSvd(); // destructor
  RooUnfoldSvd& operator= (const RooUnfoldSvd& rhs); // assignment operator
  virtual RooUnfoldSvd* Clone (const char* newname= 0) const;

  // Special constructors

  RooUnfoldSvd (const RooUnfoldResponse* res, const TH1* meas, Int_t kreg= 0,
                const char* name= 0, const char* title= 0);
  // compatibility constructor
  RooUnfoldSvd (const RooUnfoldResponse* res, const TH1* meas, Int_t kreg, Int_t ntoyssvd,
                const char* name= 0, const char* title= 0);

  void SetKterm (Int_t kreg);
  Int_t GetKterm() const;
  virtual void  SetRegParm (Double_t parm);
  virtual Double_t GetRegParm() const;
  virtual void Reset();
  TSVDUnfold* Impl();

  void SetNtoysSVD (Int_t ntoyssvd);  // no longer used
  Int_t GetNtoysSVD() const;          // no longer used

protected:
  void Assign (const RooUnfoldSvd& rhs); // implementation of assignment operator
  virtual void Unfold();
  virtual void GetCov();
  virtual void GetWgt();
  virtual void GetSettings();

private:
  void Init();
  void Destroy();
  void CopyData (const RooUnfoldSvd& rhs);

protected:
  // instance variables
  TSVDUnfold* _svd;  //! Implementation in TSVDUnfold object (no streamer)
  Int_t _kreg;
  Int_t _nb;

  TH1D *_meas1d, *_train1d, *_truth1d;
  TH2D *_reshist, *_meascov;

public:
  ClassDef (RooUnfoldSvd, 1) // SVD Unfolding (interface to TSVDUnfold)
};

// Inline method definitions

inline
RooUnfoldSvd::RooUnfoldSvd()
  : RooUnfold()
{
  // Default constructor. Use Setup() to prepare for unfolding.
  Init();
}

inline
RooUnfoldSvd::RooUnfoldSvd (const char* name, const char* title)
  : RooUnfold(name,title)
{
  // Basic named constructor. Use Setup() to prepare for unfolding.
  Init();
}

inline
RooUnfoldSvd::RooUnfoldSvd (const TString& name, const TString& title)
  : RooUnfold(name,title)
{
  // Basic named constructor. Use Setup() to prepare for unfolding.
  Init();
}

inline
RooUnfoldSvd& RooUnfoldSvd::operator= (const RooUnfoldSvd& rhs)
{
  // Assignment operator for copying RooUnfoldSvd settings.
  Assign(rhs);
  return *this;
}

inline
RooUnfoldSvd::~RooUnfoldSvd()
{
  Destroy();
}


inline
void RooUnfoldSvd::SetKterm (Int_t kreg)
{
  // Set regularisation parameter
  _kreg= kreg;
}


inline
Int_t RooUnfoldSvd::GetKterm() const
{
  // Return regularisation parameter
  return _kreg;
}

inline void RooUnfoldSvd::SetNtoysSVD (Int_t ntoyssvd) {_NToys=ntoyssvd;}  // no longer used
inline Int_t RooUnfoldSvd::GetNtoysSVD() const { return _NToys; }  // no longer used

inline
void  RooUnfoldSvd::SetRegParm (Double_t parm)
{
  // Set regularisation parameter
  SetKterm(Int_t(parm+0.5));
}

inline
Double_t RooUnfoldSvd::GetRegParm() const
{
  // Return regularisation parameter
  return GetKterm();
}

#endif
