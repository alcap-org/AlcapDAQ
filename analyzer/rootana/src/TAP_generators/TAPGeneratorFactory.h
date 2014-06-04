#ifndef TAPGENERATORFACTORY__HH_
#define TAPGENERATORFACTORY__HH_
#include "TemplateFactory.h"

class TVAnalysedPulseGenerator;
class TAPGeneratorOptions;

class TAPGeneratorFactory:public TemplateFactory<TVAnalysedPulseGenerator,TAPGeneratorOptions>{
	private:
		TAPGeneratorFactory():
			TemplateFactory<TVAnalysedPulseGenerator,TAPGeneratorOptions>(){};
		~TAPGeneratorFactory(){};
	 public:
		// Get the single instance of this class
		static TAPGeneratorFactory* Instance();
};

inline TAPGeneratorFactory* TAPGeneratorFactory::Instance(){
    static TAPGeneratorFactory* instance=NULL;
    if(!instance) {
	instance=new TAPGeneratorFactory();
    }
    return instance;
}

#define ALCAP_TAP_GENERATOR( CLASS , ... ) \
RegistryProxy<CLASS,TVAnalysedPulseGenerator,TAPGeneratorOptions,TAPGeneratorFactory> p_##CLASS(#CLASS, #__VA_ARGS__);


#endif // TAPGENERATORFACTORY__HH_
