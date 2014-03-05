#include "ModulesReader.h"
#include <iostream>
#include <sstream>

int modules::reader::ReadFile(const char* name){
    // Open config file
    std::ifstream infile;
    int ret = OpenFile(name,infile);
    if(ret!=0) return ret;

    // Loop over each line
    std::string full_line;
    std::string module, new_module;
    while(std::getline(infile,full_line)){
	// tokenize the line straight away
	std::stringstream line(full_line);

	// check if this line is a comment
	if(isComment(line) ) continue;

	new_module=findModuleName(line);
	if(new_module!="") {
	    module=new_module;
	    continue;
	}

	// options use key and values separated by '='
	std::cout<<"Found option: "<<full_line<<std::endl;
	//fAllOptions[module].push_back(full_line);
    }
    return 0;
}

int modules::reader::OpenFile(const char* name, std::ifstream& infile){
    infile.close();
    infile.open(name, std::ifstream::in);
    if(infile.good()) return 0;
    return 1;
}

bool modules::reader::isComment( std::stringstream& line){
    static std::string word;
    word="";
    line>>word;
    if(word=="#"){
	return true;
    } else{
	return false;
    }
}

std::string modules::reader::findModuleName(std::stringstream& line){
    // sections are marked with [] either side of the name
    static std::string word;
    word="";
    char start,stop;
    line>>start>>word>>stop;
    if(start=='[' && stop == ']')
	return word;
    return "";

}
