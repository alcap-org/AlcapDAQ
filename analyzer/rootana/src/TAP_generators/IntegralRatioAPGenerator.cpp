#include "TAPGeneratorFactory.h"
#include "TAPAlgorithms.h"
#include "SetupNavigator.h"
#include "IntegralRatioAPGenerator.h"
#include "TPulseIsland.h"
#include "TIntegralRatioAnalysedPulse.h"
#include <iostream>
#include <stdexcept>
using std::cout;
using std::endl;
#include "debug_tools.h"

IntegralRatioAPGenerator::IntegralRatioAPGenerator(TAPGeneratorOptions* opts):
    TVAnalysedPulseGenerator("IntegralRatio",opts),
        fStartIntegral(opts->GetInt("start_int","x>=0")),
        fStopIntegral(opts->GetInt("stop_int","x>=0")),
        fStartTail(opts->GetInt("start_tail",Form("x>%g",fStartIntegral))),
        fPedestal(GetChannel().isFast()?900:2728),
        fPolarity(GetChannel().isFast()?1:-1),
        fFullIntegrator( fPedestal, fPolarity, fStartIntegral, fStopIntegral),
        fTailIntegrator( fPedestal, fPolarity, fStartTail, fStopIntegral){
    }
IntegralRatioAPGenerator::~IntegralRatioAPGenerator(){}

int IntegralRatioAPGenerator::ProcessPulses( 
        const PulseIslandList& pulseList,
        AnalysedPulseList& analysedList){

    // Loop over all the TPIs given to us
    double integral, tail;
    TIntegralRatioAnalysedPulse* tap;
    for (PulseIslandList::const_iterator tpi=pulseList.begin();
            tpi!=pulseList.end(); tpi++){

        // Analyse each TPI
        try{
            integral=fFullIntegrator(*tpi);
            tail=fTailIntegrator(*tpi);
        }catch(std::out_of_range& e){
            continue;
        }

        // Now that we've found the information we were looking for make a TAP to
        // hold it.  This method makes a TAP and sets the parent TPI info.  It needs
        // the index of the parent TPI in the container as an argument
        tap = MakeNewTAP<TIntegralRatioAnalysedPulse>(tpi-pulseList.begin());
        tap->SetIntegral(integral);
        tap->SetIntegralSmall(tail);
        tap->SetIntegralRatio(tail/integral);
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

ALCAP_TAP_GENERATOR(IntegralRatio,start_int,start_tail,stop_int);
