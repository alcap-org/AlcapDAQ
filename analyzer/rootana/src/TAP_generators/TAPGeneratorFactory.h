#ifndef TAPGENERATORFACTORY__HH_
#define TAPGENERATORFACTORY__HH_
#include "TemplateFactory.h"

class TVAnalysedPulseGenerator;

class TAPGeneratorFactory:public TemplateFactory<TVAnalysedPulseGenerator>{
	private:
		TAPGeneratorFactory():
			TemplateFactory<TVAnalysedPulseGenerator>(){};
		~TAPGeneratorFactory(){};
	 public:
		// Get the single instance of this class
		static TAPGeneratorFactory* Instance();
};

inline TAPGeneratorFactory* TAPGeneratorFactory::Instance(){
    static TAPGeneratorFactory* instance=NULL;
    if(!instance) {
	instance=new factory();
    }
    return instance;
}

#endif // TAPGENERATORFACTORY__HH_
