//=====================================================================-*-C++-*-
// File and Version Information:
//      $Id$
//
// Description:
//      Optimisation of regularisation parameter class
//
// Author: Richard Claridge <richard.claridge@stfc.ac.uk>
//
//==============================================================================

#ifndef ROOUNFOLDPARMS_H_
#define ROOUNFOLDPARMS_H_

#include "TNamed.h"
#include "RooUnfold.h"

class TH1;
class RooUnfold;
class TProfile;

class RooUnfoldParms : public TNamed {
    public:
    RooUnfoldParms(const RooUnfold* unfold_in=0,RooUnfold::ErrorTreatment err=RooUnfold::kCovariance,const TH1* truth=0);
    virtual ~RooUnfoldParms();
    TProfile* GetChi2();
    TProfile* GetRMSError();
    TProfile* GetMeanResiduals();
    TH1* GetRMSResiduals();
    const RooUnfold* unfold; // Input object from RooUnfold
    RooUnfold::ErrorTreatment doerror; // Set error calculation method
    const TH1* hTrue; // Truth Distribution
    void SetMinParm(double min);
    void SetMaxParm(double max);
    void SetStepSizeParm(double size);
    
    private:
    bool _done_math;
    TH1* hrms; // Output plot
    TProfile* hch2; // Output plot
    TProfile* herr; // Output plot
    TProfile* hres; // Output plot
    void DoMath();
    void Init();
    Double_t _maxparm; //Maximum parameter
    Double_t _minparm; //Minimum parameter
    Double_t _stepsizeparm; //Step size
public:
    ClassDef (RooUnfoldParms, 0)  // Optimisation of unfolding regularisation parameter
};
#endif /*ROOUNFOLDPARMS_H_*/
