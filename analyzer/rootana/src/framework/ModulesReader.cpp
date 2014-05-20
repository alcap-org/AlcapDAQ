#include "ModulesReader.h"
#include "ModulesFactory.h"
#include <iostream>
#include <sstream>
#include <string.h>
#include <stdio.h>

const char* modules::reader::fGlobalModule="global";

static std::string getOneWord(const std::string& in, size_t start=0, size_t stop=std::string::npos){
	//int begin=in.find_first_not_of(' ',start);
	//int end=in.find_last_not_of(' ',stop);
	//int begin=start, end=stop;
	//std::cout<<"trim: "<<in<<'\t'<<in[begin]<<'\t'<<in[end]<<std::endl;
	std::stringstream ss(in.substr(start,stop));
	std::string word;
	ss>>word;
	return word;
}

static void getSeveralWords(const std::string& in, std::vector<std::string> &vect, size_t start=0, size_t stop=std::string::npos){
	//int begin=in.find_first_not_of(' ',start);
	//int end=in.find_last_not_of(' ',stop);
	//int begin=start, end=stop;
	//std::cout<<"trim: "<<in<<'\t'<<in[begin]<<'\t'<<in[end]<<std::endl;
	char line[400];
	strcpy(line,in.substr(start,stop-start).c_str());
	char* word = strtok(line,", ");
	while(word != NULL){ 
	    vect.push_back(word);
	    word = strtok(NULL,", ");
	}
}

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
	new_section=findSectionName(line);
	if(new_section!="") {
	    section=new_section;
	    if(fShouldPrint) std::cout<<"Found new section: "<<section<<std::endl;
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

std::string modules::reader::findSectionName(std::stringstream& line){
    // sections are marked with [] either side of the name
    static std::string word;
    word="";
    char start=0,stop=0;
    line>>start>>word>>stop;
    //std::cout<<line.good()<<std::endl;
    //std::cout<<line.str()<<'\t'<< start <<'\t'<< word<<'\t'<<stop<<std::endl;
    if(start=='['){
	if( stop != ']') {// occurs if no whitespace between closing brace and word
	    stop = word[word.size()-1];
	    if(stop == ']') word.resize(word.size()-1);
	}
	if(stop == ']') return word;
    }
    return "";
}

int modules::reader::AddModule(std::string line){
    std::string name, type;

    // How is this module specified ? 
    // If  the line contains an equals sign, assume we have a name and a type
    // (and possibly arguments), if not, assume it's just a type (and possibly
    // arguments)
    size_t equ_pos=line.find('=');
    if(equ_pos==std::string::npos) {
	// module is not named
	name="";
    }else{
	// module is named
	name=getOneWord(line,0,equ_pos);
	line=line.substr(equ_pos+1);
    }

    // Get the type of the module to be used
    size_t br_pos=line.find('(');
    type=getOneWord(line,0,br_pos);

    // Get arguments to the module if they're provided
    std::vector<std::string> args;
    if(br_pos!=std::string::npos) {
	size_t br_close_pos=line.rfind(')');
	if(br_close_pos ==std::string::npos) {
	    std::cout<<"Error on line "<<fLineNumber<<": no closing parenthesis for arguments of '"<<type<<"'"<<std::endl;
	    return 2;
	} else {
	    getSeveralWords(line,args,br_pos+1, br_close_pos);
	}
    }

    if(fShouldPrint){
	std::cout<<"Adding module: '"<<type<<"'"<<std::endl;
	std::cout<<"   with name: '"<<name<<"'"<<std::endl;
	std::cout<<"   and "<<args.size()<<" arguments:"<<std::endl;
	for(unsigned i=0;i<args.size() ;i++)
		std::cout<<"     "<<i<<'\t'<<args[i]<<std::endl;
    }

    //Add the module to the list of modules
    modules::options* opts;
    if(name==""){
        opts=new modules::options(type);
    }else{
	// since the module has been named, create a section to store other
	// options that may be provided subsequently
	if(!AddSection(name,type)){
	    std::cout<<"Error on line "<<fLineNumber<<": module name is being reused"<<std::endl;
	    return 1;
	}
        opts=fAllOptions[name];
    }
    for(unsigned i=0;i<args.size();i++) {
	opts->AddArgument(i,args[i]);
    }
    fModules.push_back(std::make_pair(type,opts));
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

void modules::reader::AddOption(const std::string& module, const Option_t& opt){
    bool exists;
    switch (opt.mode){
	case kSet:
            fAllOptions[module]->SetOption(opt.key,opt.value);
	    break;
	case kAppend:
            exists= fAllOptions[module]->AppendToOption(opt.key,opt.value);
	    if(!exists &&fShouldPrint){
		    std::cout<<"Cannot append to '"<<opt.key<<"' as it doesn't already exist"<<std::endl;
	    }
	    break;
	default: 
	    std::cout<<"modules::reader::AddOption(): That's odd!  How did I end up here?"<<std::endl;
	    break;
    }
}

void modules::reader::AddOption(const std::string& module, const std::string& flag){
    fAllOptions[module]->SetOption(flag,"");
}

void modules::reader::PrintAllOptions()const{
    // Global options that were set
    SectionsList::const_iterator it_sec=fAllOptions.find(fGlobalModule);
    if(it_sec!=fAllOptions.end() && it_sec->second->GetNumOptions()>0){
       std::cout<<"__ Global otions __"<<std::endl;
       it_sec->second->DumpOptions();
       std::cout<<std::endl;
    }

    // Modules that were requested
    std::cout<<"__ All modules __"<<std::endl;
    ModuleList::const_iterator it_mod;
    for(it_mod=fModules.begin(); it_mod != fModules.end();it_mod++){
	std::cout<<"Module: "<<it_mod->first<<std::endl;
	it_mod->second->DumpOptions();
        std::cout<<std::endl;
    }
    
    // Sections that were added, but don't correspond to a module
    std::cout<<"__ Unused sections __"<<std::endl;
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
    SetDebug();
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
     if (opt.value=="all"){
        SetDebugAll();
     } else{
        SetDebug();
     }
  }else {
     if(fShouldPrint) std::cout<<"Warning: Unknown global option given, '"<<opt.key<<"'"<<std::endl;
     return;
  }
}
