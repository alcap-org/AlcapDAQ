#ifndef TMEGENERATORFACTORY__HH_
#define TMEGENERATORFACTORY__HH_
#include "TemplateFactory.h"

class TVMuonEventGenerator;
class TMEGeneratorOptions;

class TMEGeneratorFactory:public TemplateFactory<TVMuonEventGenerator,TMEGeneratorOptions>{
	private:
		TMEGeneratorFactory():
			TemplateFactory<TVMuonEventGenerator,TMEGeneratorOptions>("TMEGeneratorFactory"){};
		~TMEGeneratorFactory(){};
	 public:
		// Get the single instance of this class
		static TMEGeneratorFactory* Instance();
};

inline TMEGeneratorFactory* TMEGeneratorFactory::Instance(){
    static TMEGeneratorFactory* instance=NULL;
    if(!instance) {
	instance=new TMEGeneratorFactory();
    }
    return instance;
}

#define ALCAP_TME_GENERATOR( NAME , ... ) \
RegistryProxy<NAME##MEGenerator,TVMuonEventGenerator,TMEGeneratorOptions,TMEGeneratorFactory> p_ME##NAME(#NAME, #__VA_ARGS__);


#endif // TMEGENERATORFACTORY__HH_
