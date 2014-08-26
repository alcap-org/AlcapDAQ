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
        fHeadIntegrator( fPedestal, fPolarity, fStartIntegral, fStartTail),
        fTailIntegrator( fPedestal, fPolarity, fStartTail, fStopIntegral){
    }
IntegralRatioAPGenerator::~IntegralRatioAPGenerator(){}

int IntegralRatioAPGenerator::ProcessPulses( 
        const PulseIslandList& pulseList,
        AnalysedPulseList& analysedList){

    // Loop over all the TPIs given to us
    double head, tail,min, max, mean;
    int start_tail;
    TIntegralRatioAnalysedPulse* tap;
    for (PulseIslandList::const_iterator tpi=pulseList.begin();
            tpi!=pulseList.end(); tpi++){

        min=*std::min_element((*tpi)->GetSamples().begin(), (*tpi)->GetSamples().end());
        max=*std::max_element((*tpi)->GetSamples().begin(), (*tpi)->GetSamples().end());
        fHeadIntegrator.pedestal=min;
        fTailIntegrator.pedestal=min;

        if(fStartTailAsFraction){
           start_tail=fStartTail*(*tpi)->GetPulseLength();
           fHeadIntegrator.stop=start_tail;
           fTailIntegrator.start=start_tail;
        }

        // Analyse each TPI
        try{
            head=fHeadIntegrator(*tpi);
            tail=fTailIntegrator(*tpi);
        }catch(std::out_of_range& e){
            continue;
        }
        mean=(head+tail)/(fStopIntegral - fStartIntegral);

        // Now that we've found the information we were looking for make a TAP to
        // hold it.  This method makes a TAP and sets the parent TPI info.  It needs
        // the index of the parent TPI in the container as an argument
        tap = MakeNewTAP<TIntegralRatioAnalysedPulse>(tpi-pulseList.begin());
        tap->SetIntegral(head+tail);
        tap->SetIntegralSmall(tail);
        tap->SetIntegralRatio(tail/(head+tail));
        tap->SetMean(mean);
        tap->SetMax(max);
        tap->SetMin(min);

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

ALCAP_TAP_GENERATOR(IntegralRatio,start_int,start_tail,stop_int,start_tail_as_fraction);
