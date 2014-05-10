#include "ModulesManager.h"
#include "ExportPulse.h"
#include "TVAnalysedPulseGenerator.h"
#include "MaxBinAPGenerator.h"
#include <iostream>
#include <utility>
#include "RegisterModule.inc"

using std::cout;
using std::endl;
using std::string;

extern std::map<std::string, std::vector<TAnalysedPulse*> > gAnalysedPulseMap;

ExportPulse::ExportPulse(modules::options* opts):
   FillHistBase("ExportPulse",opts),fOptions(opts){
	dir->cd("/");
}

ExportPulse::~ExportPulse(){
}

int ExportPulse::BeforeFirstEntry(TGlobalData* gData,TSetupData *setup){
    return 0;
}

int ExportPulse::ProcessEntry(TGlobalData *gData, TSetupData *gSetup){
  return 0;
}

ALCAP_REGISTER_MODULE(ExportPulse);
