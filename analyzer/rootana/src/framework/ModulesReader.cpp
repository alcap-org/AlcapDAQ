#include "ModulesReader.h"
#include "ModulesFactory.h"
#include "ModulesNavigator.h"
#include "ModulesParser.h"
#include <sstream>
#include <string.h>
#include <stdio.h>
using namespace modules::parser;
using std::endl;
using std::cout;

#include "debug_tools.h"

const char* modules::reader::fGlobalModule="global";

int modules::reader::ReadFile(const char* name){

    // Open config file
    std::ifstream infile;
    int ret = OpenFile(name,infile);
    if(ret!=0) return ret;

    // make sure we have a default section set up
    AddSection(fGlobalModule);

    // Loop over each line
    std::string full_line;
    std::string section=fGlobalModule, new_section;
    Option_t current_opt;
    fLineNumber=0;
    while(std::getline(infile,full_line)){
      fLineNumber++;

      // tokenize the line straight away
      std::stringstream line(full_line);

      // check if this line is a comment
      if(isComment(line) ) continue;

      // If this line contains a section name make sure we begin a new section
      new_section=findSectionName(full_line);
      if(new_section!="") {
          section=new_section;
          if(fShouldPrint) std::cout<<"Found new section: '"<<section<<"'"<<std::endl;
          if(section!="MODULES") {
            AddSection(section);
          }
          continue;
      }

      if(section=="MODULES") {
        //  In the MODULES secton each line specifies a module to use
          ret=AddModule(full_line);
          if(ret!=0) return ret;
      }else{
          // Parse the contents of this option
          current_opt=SplitOption(full_line);

          // Handle the option as required
          if(section==fGlobalModule) {
            ProcessGlobalOption(current_opt);
          } else{
            AddOption(section,current_opt);
          }
          if(fShouldPrint) std::cout<<"Found option: "<<full_line<<std::endl;
      }
    }

    // were we asked to dump everything that's been processed?
    if(fDumpContents) PrintAllOptions();
    return 0;
}

int modules::reader::OpenFile(const char* name, std::ifstream& infile){
    infile.close();
    infile.open(name, std::ifstream::in);
    if(!infile.is_open()) {
        std::cout<<"Problem opening modules file: "<<name<<std::endl;
        return 1;
    }
    if(fShouldPrint) std::cout<<"Found modules file: "<<name<<std::endl;
    return 0;
}

bool modules::reader::isComment( std::stringstream& line){
    char word='\0';
    line>>word;
    if(word=='#' || word =='\0'){
	return true;
    } 
    line.clear();
    line.seekg(0,std::stringstream::beg);
    //line<<word;
    return false;
}

std::string modules::reader::findSectionName(const std::string& line){
    // sections are marked with [] either side of the name
	Constructor_t section= modules::parser::ParseConstructor(line,'[',']');
    return section.inside;
}

int modules::reader::AddModule(std::string line){
    std::string alias, type;

    // How is this module specified ? 
    // If  the line contains an equals sign, assume we have a alias and a type
    // (and possibly arguments), if not, assume it's just a type (and possibly
    // arguments)
    Constructor_t constructor=ParseConstructor(line,'(',')');

    std::vector<std::string> name_alias;
    TokeniseByDelimiter(constructor.before, name_alias,"=");
    if(name_alias.size()>2) {
	    PrintProblem()<<"Multiple equals signs (=) in line gives an ambiguous alias / alias."<<std::endl;
	    return 1;
    }else if(name_alias.size()==2) {
	    alias=name_alias[0];
	    type=name_alias[1];
	    TrimWhiteSpaceBeforeAfter(alias);
    }else if(name_alias.size()==1) type=name_alias[0];
    else {
	    PrintProblem()<<"No module requested"<<std::endl;
	    return 2;
    }
    TrimWhiteSpaceBeforeAfter(type);

    // Get arguments to the module if they're provided
    std::vector<std::string> args;
    TokeniseByDelimiter(constructor.inside,args,",");

    if(fShouldPrint){
	std::cout<<"Adding module: '"<<type<<"'"<<std::endl;
	std::cout<<"   with alias: '"<<alias<<"'"<<std::endl;
	std::cout<<"   and "<<args.size()<<" arguments:"<<std::endl;
	for(unsigned i=0;i<args.size() ;i++)
		std::cout<<"     "<<i<<'\t'<<args[i]<<std::endl;
    }

    //Add the module to the list of modules
    modules::options* opts;
    if(alias==""){
        opts=new modules::options(type);
    }else{
	// since the module has been named, create a section to store other
	// options that may be provided subsequently
	if(!AddSection(alias,type)){
	    PrintProblem()<<" module alias is being reused"<<std::endl;
	    return 1;
	}
        opts=fAllOptions[alias];
	opts->SetAlias(alias);
    }
    // Add all arguments passed to this module to it's options
    for(unsigned i=0;i<args.size();i++) {
	opts->AddArgument(i,args[i]);
    }

    // Stick this module into the list of modules
    fModules.push_back(std::make_pair(type,opts));
    fModulesCounts[type]++;

    // Apply current global options to new module
    if(fDebugAll) AddOption(opts,"debug");

    return 0;
}

modules::reader::Option_t modules::reader::SplitOption( const std::string& line){
    Option_t opt;
    opt.value="";
    opt.mode=kSet;

    //split the line around any equals sign
    size_t equ_pos=line.find('=');
    if(equ_pos==std::string::npos) {
      // no equal sign, treat option as bool flag
	opt.key=line;
    }else if(line[equ_pos-1]=='+'){
      // option should be appended to an existing one
	opt.key=line.substr(0,equ_pos-1);
	opt.value=line.substr(equ_pos+1);
	opt.mode=kAppend;
    }else{
      // option is simple key value pair
	opt.key=line.substr(0,equ_pos);
	opt.value=line.substr(equ_pos+1);
    }
    return opt;
}

void modules::reader::AddOption(modules::options* options, const Option_t& opt){
    bool exists;
    switch (opt.mode){
	case kSet:
            options->SetOption(opt.key,opt.value);
	    break;
	case kAppend:
            exists= options->AppendToOption(opt.key,opt.value);
	    if(!exists &&fShouldPrint){
		    std::cout<<"Cannot append to '"<<opt.key<<"' as it doesn't already exist"<<std::endl;
	    }
	    break;
	default: 
	    std::cout<<"modules::reader::AddOption(): That's odd!  How did I end up here?"<<std::endl;
	    break;
    }
}

void modules::reader::AddOption(modules::options* options, const std::string& flag){
    options->SetOption(flag,"");
}

void modules::reader::PrintAllOptions()const{
    // Global options that were set
    SectionsList::const_iterator it_sec=fAllOptions.find(fGlobalModule);
    if(it_sec!=fAllOptions.end() && it_sec->second->GetNumOptions()>0){
       std::cout<<"== Global otions =="<<std::endl;
       it_sec->second->DumpOptions();
       std::cout<<std::endl;
    }

    // Modules that were requested
    std::cout<<"== All modules =="<<std::endl;
    ModuleList::const_iterator it_mod;
    for(it_mod=fModules.begin(); it_mod != fModules.end();it_mod++){
	std::cout<<"Module: "<<it_mod->first<<std::endl;
	it_mod->second->DumpOptions();
        std::cout<<std::endl;
    }
    
    // Sections that were added, but don't correspond to a module
    std::cout<<"== Unused sections =="<<std::endl;
    for(it_sec=fAllOptions.begin(); it_sec != fAllOptions.end();it_sec++){
	if(it_sec->first==fGlobalModule) continue;
	for(it_mod=fModules.begin(); it_mod != fModules.end();it_mod++){
	    if(it_sec->second==it_mod->second ) break;
	}
	if(it_mod!=fModules.end()) continue;
	std::cout<<"Section: "<<it_sec->first<<std::endl;
	it_sec->second->DumpOptions();
        std::cout<<std::endl;
    }
}

void modules::reader::SetDebug(){
    std::cout<<"Debug mode activated"<<std::endl;
    fShouldPrint=true;
    modules::factory::Instance()->SetDebug();
}

void modules::reader::SetDebugAll(){
    //SetDebug();
    fDebugAll=true;
    AddOptionAll("debug");
}

void modules::reader::AddOptionAll(const std::string& key,const std::string& value){
    for(SectionsList::iterator it_sec=fAllOptions.begin(); it_sec != fAllOptions.end();it_sec++){
       if(it_sec->first==fGlobalModule) continue;
       it_sec->second->SetOption(key,value);
    }
}

void modules::reader::ProcessGlobalOption(Option_t opt){
  if (opt.mode==kAppend) {
     if(fShouldPrint) std::cout<<"Warning: over-writing '"<<opt.key<<"' with '"
		     << opt.value<<"' despite append operator being used"<<std::endl;
  }
  if (opt.key=="debug"){
     std::vector<std::string> args;
     modules::parser::TokeniseByWhiteSpace(opt.value,args );
     for(unsigned i=0;i<args.size();i++){
       if (args[i]=="all"){
          SetDebugAll();
       }else if (args[i]=="modules_navigator"){
       modules::navigator::Instance()->SetDebug();
       }else if (args[i]=="modules_reader"){
          SetDebug();
       }
     }
  }else if(opt.key=="list_modules"){
    modules::factory::Instance()->PrintPossibleModules();
  }else if(opt.key=="dump_contents"){
    fDumpContents=true;
  }else {
     if(fShouldPrint) std::cout<<"Warning: Unknown global option given, '"<<opt.key<<"'"<<std::endl;
     return;
  }
}

std::ostream& modules::reader::PrintProblem(){
	return std::cout<<"Problem on line "<<fLineNumber<<": ";
}

int modules::reader::HowMany(const std::string& name)const{
    ModuleCounts::const_iterator it=fModulesCounts.find(name);
    if(it!=fModulesCounts.end()) return it->second;
    return 0;
}

