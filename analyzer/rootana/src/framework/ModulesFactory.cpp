#include "ModulesFactory.h"
#include "FillHistBase.h"

modules::BaseModule* modules::factory::createModule(const std::string& name, modules::options* opts){
	BaseModule* module=TemplateFactory<BaseModule,modules::options>::createModule(name,opts);
	if(module){
	   module->SetAlias(opts->GetAlias());
	}
	return module;
}

