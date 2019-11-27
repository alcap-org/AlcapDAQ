//=====================================================================-*-C++-*-
// File and Version Information:
//      $Id$
//
// Description:
//      Unfolding class using inversion of the response matrix. This does not produce
//      good results and is designed to illustrate the need for more sophisticated
//      unfolding techniques
//
// Authors: Richard Claridge <richard.claridge@stfc.ac.uk> & Tim Adye <T.J.Adye@rl.ac.uk>
//
//==============================================================================

#ifndef ROOUNFOLDINVERT_H_
#define ROOUNFOLDINVERT_H_

#include "RooUnfold.h"

class RooUnfoldResponse;
class TH1;
class TH1D;
class TH2D;
class TDecompSVD;

class RooUnfoldInvert : public RooUnfold {

public:
  RooUnfoldInvert(); // default constructor
  RooUnfoldInvert (const char*    name, const char*    title); // named constructor
  RooUnfoldInvert (const TString& name, const TString& title); // named constructor
  RooUnfoldInvert (const RooUnfoldInvert& rhs); // copy constructor
  virtual ~RooUnfoldInvert(); // destructor
  RooUnfoldInvert& operator= (const RooUnfoldInvert& rhs); // assignment operator
  virtual RooUnfoldInvert* Clone (const char* newname= 0) const;
  RooUnfoldInvert (const RooUnfoldResponse* res, const TH1* meas, const char* name=0, const char* title=0);

  virtual void Reset();
  TDecompSVD* Impl();

protected:
  virtual void Unfold();
  virtual void GetCov();
  virtual void GetSettings();

private:
  void Init();
  Bool_t InvertResponse();

protected:
  // instance variables
  TDecompSVD* _svd;
  TMatrixD*   _resinv;

public:
  ClassDef (RooUnfoldInvert, 1)  // Unregularised unfolding
};

// Inline method definitions

inline
RooUnfoldInvert::RooUnfoldInvert()
  : RooUnfold()
{
  // Default constructor. Use Setup() to prepare for unfolding.
  Init();
}

inline
RooUnfoldInvert::RooUnfoldInvert (const char* name, const char* title)
  : RooUnfold(name,title)
{
  // Basic named constructor. Use Setup() to prepare for unfolding.
  Init();
}

inline
RooUnfoldInvert::RooUnfoldInvert (const TString& name, const TString& title)
  : RooUnfold(name,title)
{
  // Basic named constructor. Use Setup() to prepare for unfolding.
  Init();
}

inline
RooUnfoldInvert& RooUnfoldInvert::operator= (const RooUnfoldInvert& rhs)
{
  // Assignment operator for copying RooUnfoldInvert settings.
  Assign(rhs);
  return *this;
}

#endif /*ROOUNFOLDINVERT_H_*/
