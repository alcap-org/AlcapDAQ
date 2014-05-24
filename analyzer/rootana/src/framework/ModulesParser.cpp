#include "ModulesParser.h"
#include <sstream>
#include <string.h>
#include <algorithm>
#include <stdio.h>

int modules::parser::TokeniseByWhiteSpace(const std::string& input, std::vector<std::string>& output){
    std::stringstream ss(input);
    std::string val;
    while(ss>>val) output.push_back(val);
    return output.size();
}

int modules::parser::TokeniseByDelimiter(const std::string& input, std::vector<std::string>& output, const char* delim){
    char line[modules::parser::max_line];
    strcpy(line,input.c_str());
    char* word = strtok(line,delim);
    while(word != NULL){ 
	    //std::cout<<"ModulesOptions:GetVectorStringsByDelimiter() "<<word<<std::endl;
        output.push_back(word);
        word = strtok(NULL,delim);
    }
    return output.size();
}

std::string modules::parser::GetOneWord(const std::string& in, size_t start, size_t stop){
	std::stringstream ss(in.substr(start,stop));
	std::string word;
	ss>>word;
	return word;
}

std::pair<std::string,std::string> modules::parser::ParseConstructor(std::string input, char open, char close)
		throw(modules::parser::errors::unmatched_parenthesis){
    size_t end_br=std::string::npos;
    size_t start_br=input.find(open);
    std::string first=GetOneWord(input,0,start_br);
    if(start_br!=std::string::npos){
       end_br=input.find(close,start_br+1);
       if(end_br==std::string::npos){
	       throw modules::parser::errors::unmatched_parenthesis(open,close);
       }
    }
    std::string second=GetOneWord(input,start_br+1,end_br);
    return std::make_pair(first,second);
}

size_t modules::parser::RemoveWhitespace(std::string& input){
	std::string::iterator new_end=std::remove_if(input.begin(),input.end(),modules::parser::IsWhitespace);
	input.erase(input.begin(),new_end);
	return input.size();
}

bool modules::parser::IsWhitespace(char in){
	bool retval=false;
	switch(in){
		case ' ': case '\t':
			retval=true;
			break;
		default:
			retval=false;
	}
	return retval;
}

bool modules::parser::IsDigit(char in){
	bool retval=false;
	switch(in){
		case 0: case 1: case 2: case 3: case 4:
		case 5: case 6: case 7: case 8: case 9:
			retval=true;
	}
	return retval;
}

bool modules::parser::IsDecimal(char in){
	return in=='.';
}

bool modules::parser::IsFloatChar(char in){
	return IsDecimal(in) || IsDigit(in);
}

bool modules::parser::IsNotFloatChar(char in){
	return !IsFloatChar(in);
}

bool modules::parser::IsNumber(const std::string& input){
	return std::find_if(input.begin(),input.end(),IsNotFloatChar) == input.end();
}

int modules::parser::GetNumber(const std::string& input){
	int val;
	sscanf ( input.c_str(), "%d", &val );
	return val;
}
