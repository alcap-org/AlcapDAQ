#include "ModulesNavigator.h"
#include <iostream>
#include <iterator>
#include <algorithm>
#include <TDirectory.h>
#include <TFile.h>
using std::cout;
using std::endl;

modules::navigator::navigator():fModulesLoaded(false),fModulesMade(false),fDebug(false),fDumpInputFile(false),fOutFile(NULL){};

int modules::navigator::LoadConfigFile(const char* filename){
    // Check we haven't already opened a modules file
    if(fModulesLoaded ) {
	    cout<<"Error: Loading a new modules file, when one was already loaded"<<endl;
	    return 1;
    }
    // Read in the file and dump the processed options
    int retVal=fModulesFile.ReadFile(filename);
    if(retVal!=0) return retVal;
    
    // check if we were asked to dump the input file's contents
    if(fModulesFile.DumpInputFile()) fDumpInputFile=true;

    // Check the file contained at least one module
    size_t num_modules=fModulesFile.GetNumModules();
    if(num_modules==0){
            cout<<"Warning: No modules were requested for use, so there's nothing to be done!"<<endl;
            return 2;
    }
    cout<<"Number of modules requested: "<<num_modules<<endl;

    // Everything finished ok
    fModulesLoaded=true;
    return 0;
}

int modules::navigator::MakeModules(){
    // Check we've been given a valid output file
    if(! (fOutFile && fOutFile->IsWritable()) ){
	    cout<<"Error: Output file is not useable"<<endl;
	    return 1;
    }

    // Check that we've already opened a modules file
    if(!fModulesLoaded) {
	    cout<<"Error: MakeModules(): Cannot make modules until you load a Modules file."<<endl;
	    return 1;
    }

    // Check that we've not already called this method
    if(fModulesMade) {
	    cout<<"Error: MakeModules(): You've already called MakeModules."<<endl;
	    return 1;
    }

    // Loop over each requested module and build it
    // If we have any problems making a module then stop running
    modules::factory* mgr = modules::factory::Instance();
    std::string name;
    modules::options* opts;
    BaseModule* mod=NULL;
    size_t num_modules=fModulesFile.GetNumModules();
    for(unsigned i=0;i<num_modules;i++){
	    name = fModulesFile.GetModule(i);
	    opts =  fModulesFile.GetOptions(i);
        if(mgr->canCreate(name)){
            if(Debug()){
                cout<<"Creating module: "<<name<<endl;
                cout<<"With options:"<<endl;
                opts->DumpOptions("  ");
            }
            gDirectory->cd("/");
            mod = mgr->createModule(name,opts);
            gDirectory->cd("/");
            if(mod){
                AddModule(name, mod);
            }
            else return 3;
        }else {
            std::cout<<"Unknown module requested: "<<name<<std::endl;
            return 4;
        }
    }

    // Everything finished ok
    fModulesMade=true;
    return 0;
}

void modules::navigator::AddModule(const std::string& name, BaseModule* mod){
    fModules.push_back(std::make_pair(name,mod));
    fModulesSearch.insert(std::make_pair(name,mod));
}

int modules::navigator::HowMany(const std::string& name)const{
    if(fModulesMade) return fModulesSearch.count(name);
    return fModulesFile.HowMany(name);
}

namespace{
    struct first_in_pair_equals{
    bool operator()(const modules::ordered_element& element){
        return element.first==comp;
    }
    std::string comp;
    first_in_pair_equals(const std::string& c):comp(c){};
    };
}

bool modules::navigator::Before(const std::string& first, const std::string& second)const{

    modules::const_iterator i_first= std::find_if(fModules.begin(),fModules.end(),first_in_pair_equals(first));
    if(i_first==fModules.end()){
        cout<<"Error: Unable to find module "<<first<<endl;
        return false;
    } 
    modules::const_iterator i_second= std::find_if(fModules.begin(),fModules.end(),first_in_pair_equals(second));
    if(i_second==fModules.end()){
        cout<<"Error: Unable to find module "<<second<<endl;
        return false;
    }
    int dist=std::distance(i_first,i_second);
    return dist>0;
}
