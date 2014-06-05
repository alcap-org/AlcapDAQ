#ifndef TDPGENERATORFACTORY__HH_
#define TDPGENERATORFACTORY__HH_
#include "TemplateFactory.h"

class TVDetectorPulseGenerator;
class TDPGeneratorOptions;

class TDPGeneratorFactory:public TemplateFactory<TVDetectorPulseGenerator,TDPGeneratorOptions>{
	private:
		TDPGeneratorFactory():
			TemplateFactory<TVDetectorPulseGenerator,TDPGeneratorOptions>(){};
		~TDPGeneratorFactory(){};
	 public:
		// Get the single instance of this class
		static TDPGeneratorFactory* Instance();
};

inline TDPGeneratorFactory* TDPGeneratorFactory::Instance(){
    static TDPGeneratorFactory* instance=NULL;
    if(!instance) {
	instance=new TDPGeneratorFactory();
    }
    return instance;
}

#define ALCAP_TDP_GENERATOR( NAME , ... ) \
RegistryProxy<NAME##DPGenerator,TVDetectorPulseGenerator,TDPGeneratorOptions,TDPGeneratorFactory> p_DP##NAME(#NAME, #__VA_ARGS__);


#endif // TDPGENERATORFACTORY__HH_
