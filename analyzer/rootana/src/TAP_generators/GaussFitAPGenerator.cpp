#include "TAPGeneratorFactory.h"
#include "GaussFitAPGenerator.h"
#include "TPulseIsland.h"
#include "TGaussFitAnalysedPulse.h"
#include "ExportPulse.h"
#include "Functions.h"
#include <iostream>
#include <sstream>
#include <TF1.h>
#include <TFitResult.h>
#include <TMath.h>
using std::cout;
using std::endl;

const char* GaussFitAPGenerator::fFitName="GausLinear";

GaussFitAPGenerator::GaussFitAPGenerator(TAPGeneratorOptions* opts):
    TVAnalysedPulseGenerator("GaussFit",opts){
        // Do things to set up the generator here. 
        fMean=opts->GetDouble("time",200);
        fWidth=opts->GetDouble("width",60);
        fAmplitude=opts->GetDouble("amplitude",200);
        fPedestal=opts->GetDouble("pedestal",0);
        fGradient=opts->GetDouble("gradient",0);

        fFitFunc=new TF1(fFitName,functions::gauss_lin,0,1000,5);
        fFitFunc->SetParName(kPedestal,"pedestal");
        fFitFunc->SetParName(kAmplitude,"amplitude");
        fFitFunc->SetParName(kWidth,"width");
        fFitFunc->SetParName(kGradient,"gradient");
        fFitFunc->SetParName(kTime,"time");
    }

int GaussFitAPGenerator::ProcessPulses( 
        const PulseIslandList& pulseList,
        AnalysedPulseList& analysedList){
    // Do something here that takes the TPIs in the PulseIslandList and
    // fills the list of TAPS

    // Loop over all the TPIs given to us
    TGaussFitAnalysedPulse* tap;
    FittedVals tap_data;
    int index;
    for (PulseIslandList::const_iterator tpi=pulseList.begin();
            tpi!=pulseList.end(); tpi++){

        // Analyse each TPI
        index=tpi-pulseList.begin();
        FitPulse(*tpi,index,tap_data);

        // Make a TGaussFitAnalysedPulse 
        tap = MakeNewTAP<TGaussFitAnalysedPulse>(index);

        // Now assign all the measure values
        tap->SetAmplitude ( tap_data.value[kAmplitude] , tap_data.error[kAmplitude] );
        tap->SetPedestal  ( tap_data.value[kPedestal]  , tap_data.error[kPedestal]  );
        tap->SetGradient  ( tap_data.value[kGradient]  , tap_data.error[kGradient]  );
        tap->SetWidth     ( tap_data.value[kWidth]     , tap_data.error[kWidth]     );
        tap->SetChi2      ( tap_data.chi2   );
        tap->SetFitStatus ( tap_data.status );

        if(ExportPulse::Instance() && tap_data.status!=0){
            ExportPulse::Instance()->AddToExportList(tap);
        }

        // Finally add the new TAP to the output list
        analysedList.push_back(tap);
    }

    // returning 0 tells the caller that we were successful, return non-zero if not
    return 0;
}

void GaussFitAPGenerator::FitPulse(const TPulseIsland* tpi,const int& id,FittedVals& tap){

    // Fill a TH1 with the TPI samples
    std::stringstream histname("tpi_");
    histname<<id;
    TH1I* hPulse = tpi->GetPulseWaveform(histname.str(), histname.str());

    // Set the parameters for the fit 
    fFitFunc->SetParameters(fPedestal,fGradient,fAmplitude,fMean,fWidth);
    fFitFunc->SetRange(0,tpi->GetPulseLength());

    // Perform the fit
    std::string options ="S R";
    if(!Debug()) options+=" Q";
    TFitResultPtr result=hPulse->Fit(fFitFunc,options.c_str());

    // Get the values from the fit 
    tap.value[kAmplitude] = fFitFunc->GetParameter(kAmplitude);
    tap.value[kTime     ] = fFitFunc->GetParameter(kTime);
    tap.value[kPedestal ] = fFitFunc->GetParameter(kPedestal);
    tap.value[kGradient ] = fFitFunc->GetParameter(kGradient);
    tap.value[kWidth    ] = fFitFunc->GetParameter(kWidth);

    // Get errors from the fit
    tap.error[kAmplitude] = fFitFunc->GetParError(kAmplitude);
    tap.error[kTime     ] = fFitFunc->GetParError(kTime);
    tap.error[kPedestal ] = fFitFunc->GetParError(kPedestal);
    tap.error[kGradient ] = fFitFunc->GetParError(kGradient);
    tap.error[kWidth    ] = fFitFunc->GetParError(kWidth);

    // Set other fit properties
    tap.chi2          = result->Chi2();
    tap.status        = result;

    delete hPulse;

}

// Similar to the modules, this macro registers the generator with
// MakeAnalysedPulses. The first argument is compulsory and gives the name of
// this generator. All subsequent arguments will be used as names for arguments
// given directly within the modules file.  See the github wiki for more.
//
// NOTE: for TAP generators OMIT the APGenerator part of the class' name
ALCAP_TAP_GENERATOR(GaussFit,time,width,amplitude,pedestal,gradient);
