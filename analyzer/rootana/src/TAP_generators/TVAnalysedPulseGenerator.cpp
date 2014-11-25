#include "TVAnalysedPulseGenerator.h"
#include <iostream>
#include "debug_tools.h"

TVAnalysedPulseGenerator::TVAnalysedPulseGenerator(const char* name, TAPGeneratorOptions* opts):
   fAdcToEnergyGain(1), fAdcToEnergyOffset(0){
   if(opts){
       fDebug=( opts->HasOption("debug") || opts->Debug());
   }
   fSource.Generator()=IDs::generator(name,opts->StringDescription());
   SetChannel(opts->GetChannel());
   try{
      fAdcToEnergyGain  = SetupNavigator::Instance()->GetAdcToEnergyGain(    GetChannel());
      fAdcToEnergyOffset= SetupNavigator::Instance()->GetAdcToEnergyConstant(GetChannel());
      fCanCalibrate=true;
   } catch(Except::InvalidDetector& e){
      std::cout<<"TVAnalysedPulseGenerator: WARNING: No calibration data an be found for the '"<<GetChannel()<<"' channel."
                 "  Will continue, but TAPs will not have an energy value filled."<<std::endl;
      fCanCalibrate=false;
   }
}

void TVAnalysedPulseGenerator::CalibratePulses(AnalysedPulseList& theAnalysedPulses)const{
   if(fCanCalibrate){
      for(AnalysedPulseList::iterator i_tap=theAnalysedPulses.begin(); i_tap != theAnalysedPulses.end(); ++i_tap){
         const double amplitude=(*i_tap)->GetAmplitude();
         (*i_tap)->SetEnergy(fAdcToEnergyGain * amplitude + fAdcToEnergyOffset);
      }
   }
}

