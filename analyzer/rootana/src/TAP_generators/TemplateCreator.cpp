#include "ModulesManager.h"
#include "TemplateCreator.h"
#include <iostream>
#include <utility>
#include "RegisterModule.inc"

#include "TemplateArchive.h"

using std::cout;
using std::endl;
using std::string;

extern std::map<std::string, std::vector<TAnalysedPulse*> > gAnalysedPulseMap;

TemplateCreator::TemplateCreator(modules::options* opts):
   FillHistBase("TemplateCreator",opts),fOptions(opts){
	dir->cd("/");
}

TemplateCreator::~TemplateCreator(){
}

int TemplateCreator::BeforeFirstEntry(TGlobalData* gData,TSetupData *setup){
    return 0;
}

int TemplateCreator::ProcessEntry(TGlobalData *gData, TSetupData *gSetup){

  // Loop over each detector
  string bankname, detname;
  PulseIslandList_t thePulseIslands;
  BankPulseList_t::const_iterator it;

  // Open the template archive, ready for saving
  TemplateArchive* archive = new TemplateArchive("templates.root");

  for(it = gData->fPulseIslandToChannelMap.begin(); it != gData->fPulseIslandToChannelMap.end(); it++){
    bankname = it->first;
    detname = gSetup->GetDetectorName(bankname);

    // Get the TPIs
    thePulseIslands = it->second;
    if (thePulseIslands.size() == 0) continue; // no pulses here...

  }

  delete archive; // close the template archive file
  return 0;
}

ALCAP_REGISTER_MODULE(TemplateCreator);
