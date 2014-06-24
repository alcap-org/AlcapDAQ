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
    fModuleArguments[module].push_back(argument);
}

template <typename BaseModule, typename OptionsType>
inline std::string TemplateFactory<BaseModule,OptionsType>::GetArgumentName(
	const std::string& module,const int& argument){
    try{
       return fModuleArguments[module].at(argument);
    }catch(...){
       char num[5];
       sprintf(num,"%d",argument);
       return std::string(num);
    }
}

template <typename BaseModule, typename OptionsType>
BaseModule* TemplateFactory<BaseModule,OptionsType>::createModule(
      const std::string& name, OptionsType* opts){
    // get the maker for the requested module
    BaseModule* module=NULL;
    typename MakersList::iterator it  = fModuleMakers.find(name);
    if(it != fModuleMakers.end() ){
	// make the module
	maker make= it->second;
	module=make(opts);
    }else{
        std::cout<<"Unknown module requested: "<<name<<std::endl;
        return NULL;
    }
    return module;
}

template <typename BaseModule, typename OptionsType>
BaseModule* TemplateFactory<BaseModule,OptionsType>::createModule(const std::string& name){
    // get the options for this module
    typename OptionsList::iterator it_opt=fModuleOptions.find(name);
    OptionsType* opts = NULL;
    if(it_opt != fModuleOptions.end()) opts=it_opt->second;

    // make the module
    return createModule(name,opts);
}

template <typename BaseModule, typename OptionsType>
void TemplateFactory<BaseModule,OptionsType>::registerModule(
	 const std::string& name, TemplateFactory<BaseModule,OptionsType>::maker make){
    fModuleMakers[name]=make;
    fMostRecentRegister=name;
}

template <typename BaseModule, typename OptionsType>
void TemplateFactory<BaseModule,OptionsType>::addOptions(
         const std::string& name, OptionsType *opts){
    fModuleOptions[name]=opts;
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
  for(typename MakersList::const_iterator it=fModuleMakers.begin();
				 it!=fModuleMakers.end(); it++){
				 std::cout<<"  "<<it->first<<std::endl;
 }
}
