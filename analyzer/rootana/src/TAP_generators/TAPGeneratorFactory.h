#ifndef TAPGENERATORFACTORY__HH_
#define TAPGENERATORFACTORY__HH_
#include "TemplateFactory.h"
#include "IdSource.h"
#include "IdGenerator.h"

class TVAnalysedPulseGenerator;
class TAPGeneratorOptions;

class TAPGeneratorFactory:public TemplateFactory<TVAnalysedPulseGenerator,TAPGeneratorOptions>{
	private:
		TAPGeneratorFactory():
			TemplateFactory<TVAnalysedPulseGenerator,TAPGeneratorOptions>("TAPGeneratorFactory"){};
		~TAPGeneratorFactory(){};

	 public:

    	std::string GetTAPType(const IDs::source& s){ 
            return GetTAPType(s.Generator());
        }

    	std::string GetTAPType(const IDs::generator& g){ 
            return GetProduct(g.Type());
        }

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

#define ALCAP_TAP_GENERATOR__( NAME, CLASS, ARGUMENTS) \
RegistryProxy<CLASS,TVAnalysedPulseGenerator,TAPGeneratorOptions,TAPGeneratorFactory> \
   p_##CLASS(NAME, ARGUMENTS, CLASS::TapType() );

#define ALCAP_TAP_GENERATOR( CLASS , ... ) \
    ALCAP_TAP_GENERATOR__( #CLASS, CLASS##APGenerator, #__VA_ARGS__)


#endif // TAPGENERATORFACTORY__HH_
