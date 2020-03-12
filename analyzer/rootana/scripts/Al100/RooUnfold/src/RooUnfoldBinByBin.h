//=====================================================================-*-C++-*-
// File and Version Information:
//      $Id$
//
// Description:
//      Unfolding class using the bin by bin method of conversion factors.
//
// Authors: Richard Claridge <richard.claridge@stfc.ac.uk> & Tim Adye <T.J.Adye@rl.ac.uk>
//
//==============================================================================

#ifndef ROOUNFOLDBINBYBIN_H_
#define ROOUNFOLDBINBYBIN_H_

#include "RooUnfold.h"
#include "TVectorD.h"

class RooUnfoldResponse;
class TH1;
class TH1D;
class TH2D;

class RooUnfoldBinByBin : public RooUnfold {

public:
  RooUnfoldBinByBin(); // default constructor
  RooUnfoldBinByBin (const char*    name, const char*    title); // named constructor
  RooUnfoldBinByBin (const TString& name, const TString& title); // named constructor
  RooUnfoldBinByBin (const RooUnfoldBinByBin& rhs); // copy constructor
  virtual ~RooUnfoldBinByBin(); // destructor
  RooUnfoldBinByBin& operator= (const RooUnfoldBinByBin& rhs); // assignment operator
  virtual RooUnfoldBinByBin* Clone (const char* newname= 0) const;
  RooUnfoldBinByBin (const RooUnfoldResponse* res, const TH1* meas, const char* name=0, const char* title=0);

  TVectorD* Impl();

protected:
  virtual void Unfold();
  virtual void GetCov();
  virtual void GetSettings();

protected:
  // instance variables
  TVectorD _factors;

public:
  ClassDef (RooUnfoldBinByBin, 1)  // Bin-by-bin unfolding
};


inline
RooUnfoldBinByBin::RooUnfoldBinByBin()
  : RooUnfold()
{
  // Default constructor. Use Setup() to prepare for unfolding.
  GetSettings();
}

inline
RooUnfoldBinByBin::RooUnfoldBinByBin (const char* name, const char* title)
  : RooUnfold(name,title)
{
  // Basic named constructor. Use Setup() to prepare for unfolding.
  GetSettings();
}

inline
RooUnfoldBinByBin::RooUnfoldBinByBin (const TString& name, const TString& title)
  : RooUnfold(name,title)
{
  // Basic named constructor. Use Setup() to prepare for unfolding.
  GetSettings();
}

inline
RooUnfoldBinByBin& RooUnfoldBinByBin::operator= (const RooUnfoldBinByBin& rhs)
{
  // Assignment operator for copying RooUnfoldBinByBin settings.
  Assign(rhs);
  return *this;
}

#endif /*ROOUNFOLDBINBYBIN_H_*/
