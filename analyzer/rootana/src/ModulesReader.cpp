#include "ModulesReader.h"
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
    bool listingModules=false;
    fLineNumber=0;
    while(std::getline(infile,full_line)){
	fLineNumber++;

	// tokenize the line straight away
	std::stringstream line(full_line);

	// check if this line is a comment
	if(isComment(line) ) continue;

	new_section=findSectionName(line);
	if(new_section!="") {
	    section=new_section;
	    std::cout<<"Found new section: "<<section<<std::endl;
	    if(section=="MODULES") listingModules=true;
	    else {
		listingModules=false;
		AddSection(section);
	    }
	    continue;
	}

	if(listingModules) { 
	    ret=AddModule(full_line);
	    if(ret!=0) return ret;
	}else{
	    // options use key and values separated by '='
	    std::cout<<"Found option: "<<full_line<<std::endl;
	    AddOption(section,full_line);
	}
    }
    return 0;
}

int modules::reader::OpenFile(const char* name, std::ifstream& infile){
    infile.close();
    infile.open(name, std::ifstream::in);
    if(infile.is_open()) {
	std::cout<<"Found modules file: "<<name<<std::endl;
	return 0;
    }
    std::cout<<"Problem with modules file: "<<name<<std::endl;
    return 1;
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
    char start,stop;
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
    std::vector<std::string> args;

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

	// since the module has been named, create a section to store other
	// options that may be provided subsequently
	if(!AddSection(name)){
	    std::cout<<"Error on line "<<fLineNumber<<": module name is being reused"<<std::endl;
	    return 1;
	}
    }

    // Get the type of the module to be used
    size_t br_pos=line.find('(');
    type=getOneWord(line,0,br_pos);
    // Get argumenst to the module if they're provided
    if(br_pos!=std::string::npos) {
	size_t br_close_pos=line.rfind(')');
	if(br_close_pos ==std::string::npos) {
	    std::cout<<"Error on line "<<fLineNumber<<": no closing parenthesis for arguments of '"<<type<<"'"<<std::endl;
	    return 2;
	} else {
	    std::cout<<line<<'\t'<<br_close_pos<<'\t'<<line[br_close_pos]<<std::endl;
	    getSeveralWords(line,args,br_pos+1, br_close_pos);
	}
    }

    std::cout<<"Adding module: '"<<type<<"'"<<std::endl;
    std::cout<<"   with name: '"<<name<<"'"<<std::endl;
    std::cout<<"   and "<<args.size()<<" arguments:"<<std::endl;
    for(unsigned i=0;i<args.size();i++) std::cout<<"     "<<i<<'\t'<<args[i]<<std::endl;

    //Add the module to the list of modules
    modules::options* opts;
    if(name==""){
        opts=new modules::options();
    }else{
        opts=fAllOptions[name];
    }
    char number[10];
    for(unsigned i=0;i<args.size();i++) {
	sprintf(number,"%d",i);
	opts->AddOption(number,args[i]);
    }
    fModules.push_back(std::make_pair(type,opts));
    return 0;
}

void modules::reader::AddOption(const std::string& module, const std::string& line){
    std::string key, value;

    //split the line around any equals sign
    size_t equ_pos=line.find('=');
    if(equ_pos==std::string::npos) {
	key=line;
	value="";
    }else{
	key=line.substr(0,equ_pos);
	value=line.substr(equ_pos+1);
    }
    fAllOptions[module]->AddOption(key,value);
}

void modules::reader::PrintAllOptions()const{
    ModuleList::const_iterator it_mod;
    std::cout<<" All modules: "<<std::endl;
    for(it_mod=fModules.begin(); it_mod != fModules.end();it_mod++){
	std::cout<<"Module: "<<it_mod->first<<std::endl;
	it_mod->second->DumpOptions();
    }
    std::cout<<" All sections: "<<std::endl;
    SectionsList::const_iterator it;
    for(it=fAllOptions.begin(); it != fAllOptions.end();it++){
	    std::cout<<"Section: "<<it->first<<std::endl;
	    it->second->DumpOptions();
    }
}
