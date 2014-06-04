#ifndef MODULESFACTORY__HH_
#define MODULESFACTORY__HH_
#include "TemplateFactory.h"

class FillHistBase;

namespace modules{
    // In the future I plan to rename the FillHistBase class, so this will help
   typedef FillHistBase BaseModule;
    class factory;
    class options;
}

class modules::factory:public TemplateFactory<BaseModule,modules::options>{
	private:
		factory():TemplateFactory<BaseModule,modules::options>(){};
		~factory(){};
	 public:
		// Get the single instance of this class
		static factory* Instance();
};

inline modules::factory* modules::factory::Instance(){
    static modules::factory* instance=NULL;
    if(!instance) {
	instance=new factory();
    }
    return instance;
}

#endif // MODULESFACTORY__HH_
