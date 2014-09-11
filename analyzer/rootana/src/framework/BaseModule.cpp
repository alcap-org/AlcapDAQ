#include "BaseModule.h"
#include "ModulesOptions.h"
#include "ModulesNavigator.h"
#include <iostream>
#include "TObjString.h"

BaseModule::BaseModule(const char *name,modules::options* opts,bool with_directory):
	fSetup(TSetupData::Instance()),fDebug(false),fName(name),fDirectory(NULL){
  // Pass common options
  if(opts){
    fDebug=(opts->HasOption("debug") && (opts->GetOption("debug").empty() || opts->GetBool("debug")));
    SetAlias(opts->GetAlias());
  }
  // Make a directory for this module if desired
  if(with_directory){
    // check if one exists
    fDirectory=gDirectory->GetDirectory(fName.c_str());
    // if not create one
    if(!fDirectory) fDirectory=gDirectory->mkdir(fName.c_str());
    // now make check if there's more than one instance of this module
    if(modules::navigator::Instance()->HowMany(fName)>1) {
        // and if there is, make a subdirectory and change into it
        fDirectory=fDirectory->mkdir(GetAlias().c_str());
    }
    fDirectory->SetTitle(opts->StringDescription().c_str());
    fDirectory->cd();
    // write the config into the file as a string
    TObjString *title=new TObjString(opts->StringDescription().c_str());
    title->Write();
  } else {
      // Set this module to use the top-level directory of the file
      fDirectory=gDirectory->GetDirectory("/");
  }
  /// Many modules use 'dir' still which was the old protected pointer to the
  /// modules directory.  To prevent things being broken so soon, we keep this
  /// pointer available, but be warned that it will be removed shortly...
  dir=fDirectory;
}

BaseModule::~BaseModule()
{
}

int BaseModule::ProcessGenericEntry(TGlobalData *gData, const TSetupData *gSetup){
  // This is called by our main routine and would allow later to split into different 
  // process routines if we have more than one Tree and hence different tpyes of data input.

  if(fDirectory) fDirectory->cd();
  int ret = ProcessEntry(gData, gSetup);
  gDirectory->cd("/");

  return ret;
}

int BaseModule::Preprocess(TGlobalData *gData, const TSetupData *gSetup){
  // This is called by our main routine and would allow later to split into different 
  // process routines if we have more than one Tree and hence different tpyes of data input.

  if(fDirectory) fDirectory->cd();
  int ret = BeforeFirstEntry(gData, gSetup);
  gDirectory->cd("/");

  return ret;
}

int BaseModule::Postprocess(TGlobalData *gData, const TSetupData *gSetup){
  // This is called by our main routine and would allow later to split into different 
  // process routines if we have more than one Tree and hence different tpyes of data input.

  if(fDirectory) fDirectory->cd();
  int ret = AfterLastEntry(gData, gSetup);
  gDirectory->cd("/");

  return ret;
}

TDirectory* BaseModule::GetDirectory(const std::string& name){
  if(name.empty()) return fDirectory;
  TDirectory* dir=fDirectory->GetDirectory(name.c_str());
  if(dir) return dir;
  return fDirectory->mkdir(name.c_str());
}
