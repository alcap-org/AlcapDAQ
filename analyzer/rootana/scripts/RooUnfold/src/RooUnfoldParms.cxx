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

//____________________________________________________________
/* BEGIN_HTML

<p>Allows user to optimise the regularisation parameter of an unfolding technique.</p>
<p>If the true distribution is unknown, a plot of the rms of the errors is returned for each regularisation parameter (GetErr()).</p>
<p>If the true distribution is known, the following plots can be returned:
<ul>
<li> Chi squared values vs regularisation parameter (GetChi2())
<li> RMS of the residuals given by the true and the unfolded distrbutions vs regularisation parameter (GetRes())
<li> RMS spread of the residuals vs regularisation parameter (GetRMSSpread())
</ul>
<p>For each regularisaion parameter in the predefined range, the measured distribution is unfolded. For each unfolded distribution residuals are plotted and rms found for the 
rms spread. The sum of the residuals over the whole distribution are calculated,divided by the number of bins and then rooted in order to 
return an rms. The chi squared values are calculated using the chi2() method in RooUnfold.</p>

 END_HTML */
////////////////////////////////////////////////////////////////

#include "RooUnfoldParms.h"

#include <cfloat>
#include <iostream>
#include <cmath>
#include <vector>

#include "TROOT.h"
#include "TStyle.h"
#include "TH1D.h"
#include "TProfile.h"
#include "RooUnfold.h"
#include "TRandom.h"
#include "RooUnfoldResponse.h"
#include "TLatex.h"
using std::cout;
using std::cerr;
using std::endl;
using std::vector;

ClassImp (RooUnfoldParms);

RooUnfoldParms::RooUnfoldParms(const RooUnfold* unfold_in,RooUnfold::ErrorTreatment err,const TH1* truth)
:unfold(unfold_in),doerror(err),hTrue(truth)
{
    Init();
}

void
RooUnfoldParms::Init()
{
    //Initialises variables. Uses default values for parameters/stepsize if none are set by user.
    herr=0;
    hch2=0;
    hres=0;  
    hrms=0;
    _done_math=0;
    _maxparm=unfold->GetMaxParm();
    _minparm=unfold->GetMinParm();
    _stepsizeparm=unfold->GetStepSizeParm();
}

RooUnfoldParms::~RooUnfoldParms()
{
  delete herr;
  delete hch2;
  delete hres;  
  delete hrms;
}

TProfile*
RooUnfoldParms::GetChi2()
{
    /*Returns TProfile of Chi squared values for each regularisation parameter.
     Requires a known truth distribution*/
    if (!_done_math){DoMath();} 
    hch2->SetMarkerStyle(4);
    hch2->SetMinimum(0);
    return dynamic_cast<TProfile*>(hch2->Clone());
}
TProfile*
RooUnfoldParms::GetRMSError()
{
    //Returns TProfile of errors for each regularisation parameter//
    if (!_done_math){DoMath();} 
    herr->SetMarkerStyle(4);
    herr->SetMinimum(0);
    return dynamic_cast<TProfile*>(herr->Clone());
}

TProfile*
RooUnfoldParms::GetMeanResiduals()
{
    /*Returns TProfile of RMS Residuals for each regularisation parameter.
     Requires a known truth distribution*/
     if (!_done_math){DoMath();} 
    hres->SetMarkerStyle(4);
    return dynamic_cast<TProfile*>(hres->Clone());
}

TH1*
RooUnfoldParms::GetRMSResiduals()
{
    /*Returns TH1D of RMS spread of Residuals for each regularisation parameter.
     Requires a known truth distribution*/
     if (!_done_math){DoMath();} 
    hrms->SetMarkerStyle(4);
    hrms->SetMinimum(0);
    return dynamic_cast<TH1D*>(hrms->Clone());
}

void
RooUnfoldParms::DoMath()
{
    //Loops over many regularisation parameters and creates plots.
    //Uses minimum, maximum and step size parameters for range of the loop.
    Int_t nobins=Int_t((_maxparm-_minparm)/_stepsizeparm);
    Double_t xlo=_minparm;
    Double_t xhi=_maxparm;
    hch2=new TProfile("hch2","#chi^{2} vs regparm",nobins,xlo,xhi);
    herr=new TProfile("herr","Mean error vs regparm",nobins,xlo,xhi);
    hres=new TProfile("hres","Mean residual vs regparm",nobins,xlo,xhi);
    hrms=new TH1D("hrms","RMS of residuals",nobins,xlo,xhi);
    
    if (_minparm>_maxparm || _stepsizeparm==0){
        cerr<<"Error: Invalid inputs to RooUnfoldParms ";
        if (_minparm>_maxparm){
            cerr<<"minimum parameter > maximum parameter"<<endl;
        }
        if (_stepsizeparm==0){
            cerr<<"stepsize=0"<<endl;
        }
        hch2->Fill(0.,0.);
        herr->Fill(0.,0.);
        hres->Fill(0.,0.);
        hrms->Fill(0);
    }
    
    else{ 
        vector<TH1D*> graph_vector;    
        for(Double_t a = _minparm; a<=_maxparm; a+=_stepsizeparm) {
        TString graph_title("Residuals at k= ");
        graph_title+=a;
        TH1D* graph_name = new TH1D (graph_title,graph_title, 200,0,1000);
        graph_vector.push_back(graph_name);
        }
        Int_t gvl=0;
        Int_t _overflow=unfold->Overflow();
        Int_t nt = unfold->response()->GetNbinsTruth();
        if (_overflow) nt += 2;
    
        for (Double_t k=_minparm;k<=_maxparm;k+=_stepsizeparm)
        {   
            RooUnfold* unf = unfold->Clone("unfold_toy");
            unf->SetRegParm(k);
            Double_t sq_err_tot=0;
            TH1* hReco=unf->Hreco(doerror); 
            for (Int_t i= 0; i < nt; i++)
            {
              sq_err_tot += RooUnfoldResponse::GetBinError (hReco, i, _overflow);
            }
            herr->Fill(k,sq_err_tot/nt);
            if (hTrue)
            {   
                Double_t rsqt=0;    
                Double_t res_tot=0;
                for (int i=0;i<nt;i++){
                    Int_t j= RooUnfoldResponse::GetBin (hReco, i, _overflow);
                    if (hReco->GetBinContent(j)!=0.0 || (hReco->GetBinError(j)>0.0)) 
                    {
                        Double_t res=hReco->GetBinContent(j) - hTrue->GetBinContent(j);
                        //cout <<"res="<<res<<endl;
                        Double_t rsq=res*res;
                        rsqt+=rsq;
                        graph_vector[gvl]->Fill(res);
                        res_tot+=res;
                        hres->Fill(k,res);
                    }
                }
                double chi2=unf->Chi2(hTrue,doerror);
                if (chi2<=1e10){
                    hch2->Fill(k,chi2);
                }
                
            }
            gvl++;
            delete unf;
        }
        Double_t bn=_minparm;
        for (int i=0; i<hres->GetNbinsX(); i++){
            Double_t spr=hres->GetBinError(i);
            hrms->Fill(bn,spr);
            bn+=_stepsizeparm;
        }
        for (unsigned int i=0; i<graph_vector.size(); i++){
            delete graph_vector[i];
        }
    }
    _done_math=true;
}

void
RooUnfoldParms::SetMinParm(double min)
{
    //Sets minimum parameter
    _minparm=min;
}

void
RooUnfoldParms::SetMaxParm(double max)
{
    //Sets maximum parameter
    _maxparm=max;
}

void
RooUnfoldParms::SetStepSizeParm(double size)
{
    //Sets step size.
    _stepsizeparm=size;
}
