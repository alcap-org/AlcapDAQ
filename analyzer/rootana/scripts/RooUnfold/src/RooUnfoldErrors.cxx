//=====================================================================-*-C++-*-
// File and Version Information:
//      $Id$
//
// Description:
//      Unfolding errors class
//
// Author: Richard Claridge <richard.claridge@stfc.ac.uk>
//
//==============================================================================

//____________________________________________________________
/* BEGIN_HTML

<p> A graph drawing class to view the errors associated with an unfolding technique</p>
<p>Before these can be run, the RooUnfoldErrors object must be created and the operation Plotting() run on the object in order to do the 
maths needed to plot these graphs. The object requires the number of toys over which the errors are calculated and a RooUnfold object.</p>
<p>For each iteration each bin in the measured distribution is added to a random number from a gaussian with a width based on the error in that bin. This is then unfolded and the results plotted for each bin. The rms in each bin is then used as the spread of the values in every bin. This gives errors that are slightly larger than those returned by RooUnfold, but are a better representation of the spread in the data.</p> 
<p> If the true distribution is not known, the following can be returned:</p>
<ul>
<li> A graph of the errors from the unfolding (Unf_err())
<li> A graph of the errors due to the spread of the reconstructed points (Spread())
<li> An error matrix based on the spread of the reconstructed points (True_err())
</ul>
<p> If the true distribution is known then a plot of the chi squared values can also be returned (Chi2()).
 This requires the inclusion of the truth distribution and the error method on which the chi squared is based 
 (0 for a simple calculation, 1 or 2 for a method based on the covariance matrix, depending on the method used for calculation of errors.). </p>
<p>On some occasions the chi squared value can be very large. This is due to the covariance matrices being near singular and thus 
difficult to invert reliably. A warning will be displayed if this is the case. To plot the chi squared distribution use the option Draw("chi2"), to filter out the larger values use Draw("chi2","abs(chi2 < max") where max is the largest value to be included.</p> 
END_HTML */
/////////////////////////////////////////////////////////////////

#include "RooUnfoldErrors.h"

#include <iostream>
#include <cmath>
#include <vector>

#include "TString.h"
#include "TStyle.h"
#include "TH1D.h"
#include "TProfile.h"
#include "TNtuple.h"
#include "TAxis.h"

#include "RooUnfoldResponse.h"
#include "RooUnfold.h"
#include "RooUnfoldResponse.h"

using std::cout;
using std::cerr;
using std::endl;

ClassImp (RooUnfoldErrors);

RooUnfoldErrors::RooUnfoldErrors (int NToys,  RooUnfold* unfold_in, const TH1* Truth)
  : toys(NToys),unfold(unfold_in),hTrue(Truth)
{
    h_err=0;
    h_err_res=0;
    hchi2=0; 
    GraphParameters(); 
    if (hTrue) CreatePlotsWithChi2();
    else       CreatePlots();
}



RooUnfoldErrors::~RooUnfoldErrors()
{
  delete h_err;
  delete h_err_res;
  delete hchi2;  
}

void 
RooUnfoldErrors::GraphParameters()
{
    //Gets graph size parameters//
    const TH1* HR=unfold->response()->Htruth();
    ntx=unfold->response()->GetNbinsTruth();
    if (HR->GetDimension()==1) {
      xlo=HR->GetXaxis()->GetXmin();
      xhi=HR->GetXaxis()->GetXmax();
    } else {
      xlo=0.0;
      xhi=ntx;
    }
}


TNtuple*
RooUnfoldErrors::Chi2()
{   
    if (!hchi2) return hchi2;
    //Returns TNtuple of chi squared values. 
    hchi2->SetFillColor(4);
    return hchi2;
}

TH1*
RooUnfoldErrors::RMSResiduals(){
    if (!h_err_res) return h_err_res;
    //Returns a TH1D of the spread of the reconstructed points//
    h_err_res->SetMarkerColor(kRed);
    h_err_res->SetMarkerStyle(4);
    h_err_res->SetMinimum(0);
    return dynamic_cast<TH1*>(h_err_res->Clone());
}

TH1* 
RooUnfoldErrors::UnfoldingError(){
    if (!h_err) return h_err;
    //Returns a TH1D of the errors from the unfolding// 
    h_err->SetMarkerColor(kBlue);
    h_err->SetLineColor(kBlue);
    h_err->SetMarkerStyle(24);
    h_err->SetMinimum(0);
    return dynamic_cast<TH1*>(h_err->Clone());
}


void
RooUnfoldErrors::CreatePlots()
{
    /*Gets the values for plotting. Compares unfolding errors with errors calculated from toy MC.*/

    Bool_t oldstat= TH1::AddDirectoryStatus();
    TH1::AddDirectory (kFALSE);
    h_err     = new TH1D ("unferr", "Unfolding errors", ntx, xlo, xhi); 
    h_err_res = new TH1D ("toyerr", "Toy MC RMS",       ntx, xlo, xhi); 
    TH1::AddDirectory (oldstat);

    unfold->SetNToys(toys);
    const TVectorD errunf= unfold->ErecoV(RooUnfold::kErrors);
    const TVectorD errtoy= unfold->ErecoV(RooUnfold::kCovToy);
    for (int i= 0; i<ntx; i++) {
      h_err    ->SetBinContent(i+1,errunf[i]);
      h_err_res->SetBinContent(i+1,errtoy[i]);
    }
    return;
}
    

void
RooUnfoldErrors::CreatePlotsWithChi2()
{
    /*Gets the values for plotting. Uses the Runtoy method from RooUnfold to get plots to analyse for
    spread and error on the unfolding. Can also give values for a chi squared plot if a truth distribution is known*/

    const Double_t maxchi2=1e10;

    Bool_t oldstat= TH1::AddDirectoryStatus();
    TH1::AddDirectory (kFALSE);

    h_err     = new TProfile ("unferr", "Unfolding errors", ntx, xlo, xhi); 
    h_err_res = new TH1D     ("toyerr", "Toy MC RMS",       ntx, xlo, xhi); 
    hchi2     = new TNtuple  ("chi2", "chi2", "chi2");
    std::vector<TH1D*> graph_vector(ntx);
    for (int a=0; a<ntx; a++) {
      TString graph_name;
      graph_name.Form("resbin%d",a);
      graph_vector[a]= new TH1D (graph_name,graph_name, 100,0,10000);
    }
    
    TH1::AddDirectory (oldstat);
    
    int odd_ch=0;
    for (int k=0; k<toys;k++){  
        RooUnfold* toy= unfold->RunToy();
        Double_t chi2=       toy->Chi2 (hTrue);
        const TVectorD reco= toy->Vreco();
        const TVectorD err=  toy->ErecoV();
        for (int i=0; i<ntx; i++) {    
            graph_vector[i]->Fill(reco[i]);
            h_err->Fill(h_err->GetBinCenter(i+1),err[i]);
        } 
        if (hTrue){
            hchi2->Fill(chi2);
            if (fabs(chi2)>=maxchi2 && toy->verbose()>=1){
                cerr<<"Large |chi^2| value: "<< chi2 << endl;
                odd_ch++;
            }
        }
        delete toy;
    }
    for (int i=0; i<ntx; i++){
      TH1D* graph= graph_vector[i];
        Double_t n= graph->GetEntries();
        if (n<=0.0) continue;
        Double_t spr= graph->GetRMS();
        h_err_res->SetBinContent (i+1, spr);
        h_err_res->SetBinError   (i+1, spr/sqrt(2*n));
        delete graph;
    }
    
    if (odd_ch){
        cout <<"There are " << odd_ch << " bins over outside the range of 0 to "<<maxchi2 <<endl;
    }

}

