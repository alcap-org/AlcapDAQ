#ifndef MODULESFACTORY__HH_
#define MODULESFACTORY__HH_
#include "TemplateFactory.h"

class BaseModule;

namespace modules{
    // In the future I plan to rename the BaseModule class, so this will help
   typedef BaseModule BaseModule;
    class factory;
    class options;
}

class modules::factory:public TemplateFactory<modules::BaseModule,modules::options>{
	private:
		factory():TemplateFactory<modules::BaseModule,modules::options>(){};
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
