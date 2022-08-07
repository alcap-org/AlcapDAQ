//=====================================================================-*-C++-*-
// File and Version Information:
//      $Id$
//
// Description:
//      Unfolding interface to Fortran routine, BAYES, by G. D'Agostini from
//      http://www.roma1.infn.it/~dagos/bayes_distr.txt
//
// Authors: Tim Adye <T.J.Adye@rl.ac.uk>
//
//==============================================================================

#ifndef ROOUNFOLDDAGOSTINI_H_
#define ROOUNFOLDDAGOSTINI_H_

#include "RooUnfold.h"

class RooUnfoldResponse;
class TH1;
class TH1D;
class TH2D;

class RooUnfoldDagostini : public RooUnfold {

public:
  RooUnfoldDagostini(); // default constructor
  RooUnfoldDagostini (const char*    name, const char*    title); // named constructor
  RooUnfoldDagostini (const TString& name, const TString& title); // named constructor
  RooUnfoldDagostini (const RooUnfoldDagostini& rhs); // copy constructor
  virtual ~RooUnfoldDagostini(); // destructor
  RooUnfoldDagostini& operator= (const RooUnfoldDagostini& rhs); // assignment operator
  virtual RooUnfoldDagostini* Clone (const char* newname= 0) const;
  RooUnfoldDagostini (const RooUnfoldResponse* res, const TH1* meas, Int_t niter= 4, const char* name=0, const char* title=0);

  void SetIterations (Int_t niter= 4);
  Int_t GetIterations() const;
  virtual void  SetRegParm (Double_t parm);
  virtual Double_t GetRegParm() const;

  virtual void Reset();

protected:
  void Assign (const RooUnfoldDagostini& rhs); // implementation of assignment operator
  virtual void Unfold();
  virtual void GetCov();
  virtual void GetSettings();

private:
  void Init();

protected:
  // instance variables
  Int_t _niter;

public:
  ClassDef (RooUnfoldDagostini, 1) // Bayesian Unfolding
};

// Inline method definitions

inline
RooUnfoldDagostini::RooUnfoldDagostini()
  : RooUnfold()
{
  // Default constructor. Use Setup() to prepare for unfolding.
  Init();
}

inline
RooUnfoldDagostini::RooUnfoldDagostini (const char* name, const char* title)
  : RooUnfold(name,title)
{
  // Basic named constructor. Use Setup() to prepare for unfolding.
  Init();
}

inline
RooUnfoldDagostini::RooUnfoldDagostini (const TString& name, const TString& title)
  : RooUnfold(name,title)
{
  // Basic named constructor. Use Setup() to prepare for unfolding.
  Init();
}

inline
RooUnfoldDagostini& RooUnfoldDagostini::operator= (const RooUnfoldDagostini& rhs)
{
  // Assignment operator for copying RooUnfoldDagostini settings.
  Assign(rhs);
  return *this;
}


inline
void RooUnfoldDagostini::SetIterations (Int_t niter)
{
  // Set regularisation parameter (number of iterations)
  _niter= niter;
}

inline
Int_t RooUnfoldDagostini::GetIterations() const
{
  // Return regularisation parameter (number of iterations)
  return _niter;
}

inline
void RooUnfoldDagostini::SetRegParm (Double_t parm)
{
  // Set regularisation parameter (number of iterations)
  SetIterations(Int_t(parm+0.5));
}

inline
Double_t RooUnfoldDagostini::GetRegParm() const
{
  // Return regularisation parameter (number of iterations)
  return GetIterations();
}

#endif /*ROOUNFOLDDAGOSTINI_H_*/
