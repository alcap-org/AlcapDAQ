// Should not be included directly, but only through the TemplateFactory.h file
// vim: set syn=cpp:

template <typename BaseModule, typename OptionsType>
inline void TemplateFactory<BaseModule,OptionsType>::addArgument(
       const std::string& argument){
    addArgument(fMostRecentRegister,argument);
}

template <typename BaseModule, typename OptionsType>
inline void TemplateFactory<BaseModule,OptionsType>::addArgument(
       const std::string& module,const std::string& argument){
    fModules[module].arguments.push_back(argument);
}

template <typename BaseModule, typename OptionsType>
inline std::string TemplateFactory<BaseModule,OptionsType>::GetArgumentName(
	const std::string& module,const int& argument){
        return fModules.at(module).arguments.at(argument);
}

template <typename BaseModule, typename OptionsType>
BaseModule* TemplateFactory<BaseModule,OptionsType>::createModule(
        const std::string& name, OptionsType* opts){
    // get the maker for the requested module
    BaseModule* module=NULL;
    const PerModule& details=GetModuleDetails(name);
    // make the module
    try{
        module=details.maker(opts?opts:details.opts);
    }catch(modules::missing_option& e){
        std::cout<<fName<<"::createModule: Error: "<<e.what()<<std::endl;
        return NULL;
    }catch(modules::bad_value& e){
        std::cout<<fName<<"::createModule: Error: "<<e.what()<<std::endl;
        return NULL;
    }
    return module;
}

template <typename BaseModule, typename OptionsType>
void TemplateFactory<BaseModule,OptionsType>::registerModule(
	 const std::string& name,
     TemplateFactory<BaseModule,OptionsType>::ModuleMaker make,
     const std::string& out){
    fModules[name].maker=make;
    fModules[name].product=out;
    fMostRecentRegister=name;
}

template <typename BaseModule, typename OptionsType>
std::string TemplateFactory<BaseModule,OptionsType>::GetProduct(const std::string& module){
    return GetModuleDetails(module).product;
}

template <typename BaseModule, typename OptionsType>
const typename TemplateFactory<BaseModule,OptionsType>::PerModule& TemplateFactory<BaseModule,OptionsType>::GetModuleDetails(
     const std::string& name)const{
    typename ModuleList::const_iterator it  = fModules.find(name);
    if(it == fModules.end() ){
        std::cout<<fName<<"::GetModuleDetails: Error: I don't know how to make : "<<name<<std::endl;
        PrintPossibleModules();
        throw std::out_of_range(("unknown: "+name).c_str());
    }
    return it->second;
}

template <typename BaseModule, typename OptionsType>
void TemplateFactory<BaseModule,OptionsType>::addOptions(
         const std::string& name, OptionsType *opts){
    fModules[name].opts=opts;
}

template <typename BaseModule, typename OptionsType>
void TemplateFactory<BaseModule,OptionsType>::addArguments(const std::string& all_args){
    std::vector<std::string> args;
    modules::parser::TokeniseByDelimiter(all_args,args,", ");
    for(unsigned i=0;i<args.size();i++){
        addArgument(args[i]);
    }
}

template <typename BaseModule, typename OptionsType>
void TemplateFactory<BaseModule,OptionsType>::PrintPossibleModules()const{
  std::cout<<"Available modules are:"<<std::endl;
  for(typename ModuleList::const_iterator it=fModules.begin();
				 it!=fModules.end(); it++){
				 std::cout<<"  "<<it->first<<std::endl;
 }
}
