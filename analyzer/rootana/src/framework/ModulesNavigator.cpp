#include "ModulesNavigator.h"
#include "ModulesReader.h"
#include <iostream>
using std::cout;
using std::endl;

int modules::navigator::LoadConfigFile(const char* filename){
    // Check we haven't already opened a modules file
    if(fModulesLoaded ==true) {
	    cout<<"Error: Loading a new modules file, when one was already loaded"<<endl;
	    return 1;
    }
    // Read in the file and dump the processed options
    modules::reader modules_file;
    modules_file.ReadFile(filename);
    modules_file.PrintAllOptions();

    // Check the file contained at least one module
    size_t num_modules=modules_file.GetNumModules();
    if(num_modules==0){
            cout<<"Warning: No modules were requested for use, so there's nothing to be done!"<<endl;
            return 2;
    }
    cout<<"Number of modules requested: "<<num_modules<<endl;

    // Loop over each requested module and build it
    // If we have any problems making a module then stop running
    modules::factory* mgr = modules::factory::Instance();
    std::string name;
    modules::options* opts;
    FillHistBase* mod=NULL;
    for(unsigned i=0;i<num_modules;i++){
	    name = modules_file.GetModule(i);
	    opts =  modules_file.GetOptions(i);
	    cout<<"Creating module: "<<name<<endl;
	    cout<<"With options:"<<endl;
	    opts->DumpOptions("  ");
	    mod = mgr->createModule(name,opts);
	    if(mod) fModules[name]=mod;
	    else return 3;
    }

    // Everything finished ok
    fModulesLoaded=true;
    return 0;
}
