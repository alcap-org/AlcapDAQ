#include "TAPGeneratorFactory.h"
#include "TAPAlgorithms.h"
#include "SetupNavigator.h"
#include "IntegralRatioAPGenerator.h"
#include "TPulseIsland.h"
#include "TIntegralRatioAnalysedPulse.h"

#include <iostream>
#include <algorithm>
#include <stdexcept>
using std::cout;
using std::endl;
#include "debug_tools.h"

IntegralRatioAPGenerator::IntegralRatioAPGenerator(TAPGeneratorOptions* opts):
    TVAnalysedPulseGenerator("IntegralRatio",opts),
        fStartTailAsFraction(opts->GetBool("start_tail_as_fraction",false)),
        fStartIntegral(opts->GetInt("start_int","x>=0")),
        fStopIntegral(opts->GetInt("stop_int","x>=0")),
        fStartTail(opts->GetDouble("start_tail",
                                  fStartTailAsFraction?
                                  "x<1 && x>0":
                                  Form("x>%g",fStartIntegral))),
        fPedestal(GetChannel().isFast()?900:2728),
        fPolarity(GetChannel().isFast()?1:-1),
        fIntegralRatioAlgo( fStartIntegral, fStartTail, fStopIntegral,  fPolarity,fPedestal){
    }

IntegralRatioAPGenerator::~IntegralRatioAPGenerator(){}

int IntegralRatioAPGenerator::ProcessPulses( 
        const PulseIslandList& pulseList,
        AnalysedPulseList& analysedList){

    // Loop over all the TPIs given to us
    int start_tail;
    TIntegralRatioAnalysedPulse* tap;
    for (PulseIslandList::const_iterator tpi=pulseList.begin();
            tpi!=pulseList.end(); tpi++){

        if(fStartTailAsFraction){
           start_tail=fStartTail*(*tpi)->GetPulseLength();
           fIntegralRatioAlgo.SetTailStart(start_tail);
        }

        // Analyse each TPI
        try{
            fIntegralRatioAlgo(*tpi);
        }catch(std::out_of_range& e){
            continue;
        }

        // Now that we've found the information we were looking for make a TAP to
        // hold it.  This method makes a TAP and sets the parent TPI info.  It needs
        // the index of the parent TPI in the container as an argument
        tap = MakeNewTAP<TIntegralRatioAnalysedPulse>(tpi-pulseList.begin());
        tap->SetIntegral(fIntegralRatioAlgo.GetTotal());
        tap->SetIntegralSmall(fIntegralRatioAlgo.GetTail());
        tap->SetIntegralRatio(fIntegralRatioAlgo.GetRatio());

        // Finally add the new TAP to the output list
        analysedList.push_back(tap);
    }

    // Generators have a Debug method similar to modules
    if(Debug()){
        // They also have a unique ID, retrievable by GetSource and
        // a GetChannel method to get the ID of just the channel
        cout<<"Now running source: "<<GetSource().str()<<" which looks for TAPs on "
            "channel: "<<GetChannel().str()<<'\n';
    }

    // returning 0 tells the caller that we were successful, return non-zero if not
    return 0;
}

const char* IntegralRatioAPGenerator::TapType() {
    return TIntegralRatioAnalysedPulse::Class()->GetName();
}

ALCAP_TAP_GENERATOR(IntegralRatio,start_int,start_tail,stop_int,start_tail_as_fraction);
