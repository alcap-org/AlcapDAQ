#include "ModulesReader.h"
#include <iostream>
#include <sstream>

const char* modules::reader::fGlobalModule="global";

int modules::reader::ReadFile(const char* name){
    // Open config file
    std::ifstream infile;
    int ret = OpenFile(name,infile);
    if(ret!=0) return ret;

    // make sure we have a default module set up
    AddSection(fGlobalModule);

    // Loop over each line
    std::string full_line;
    std::string module=fGlobalModule, new_module;
    while(std::getline(infile,full_line)){
	// tokenize the line straight away
	std::stringstream line(full_line);

	// check if this line is a comment
	if(isComment(line) ) continue;

	new_module=findModuleName(line);
	if(new_module!="") {
	    module=new_module;
	    std::cout<<"Found new module: "<<module<<std::endl;
	    AddSection(module);
	    continue;
	}

	// options use key and values separated by '='
	std::cout<<"Found option: "<<full_line<<std::endl;
	AddOption(module,full_line);
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

std::string modules::reader::findModuleName(std::stringstream& line){
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
    SectionsList::const_iterator it;
    for(it=fAllOptions.begin(); it != fAllOptions.end();it++){
	    std::cout<<"Section: "<<it->first<<std::endl;
	    it->second->DumpOptions();
    }
}
