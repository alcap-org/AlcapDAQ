#include "ModulesParser.h"
#include <sstream>
#include <string.h>
#include <algorithm>
#include <stdio.h>
#include <iostream>
#include <TString.h>
#include <ctype.h>
using std::cout;
using std::endl;

#define PrintHelp std::cout<<__FILE__<<":"<<__LINE__<<": "

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
	std::stringstream ss(in.substr(start,stop-start));
	std::string word;
	ss>>word;
	return word;
}


modules::parser::Constructor_t modules::parser::ParseConstructor(const std::string& input, char open, char close)
		throw(modules::parser::errors::unmatched_parenthesis){
    size_t end_br=std::string::npos;
    size_t start_br=input.find(open);
    Constructor_t retVal;
    // cut out the string before the first open character
    retVal.before=input.substr(0,start_br);
    // trim whitespace from the start and end of retVal.before string
    TrimWhiteSpaceBeforeAfter(retVal.before);
    // Look for the requested brackets
    if(start_br!=std::string::npos){
       end_br=input.rfind(close);
       if(end_br==std::string::npos){
	       // No matching close, although there was an open
	       throw modules::parser::errors::unmatched_parenthesis(open,close);
       }
       retVal.inside=input.substr(start_br+1,end_br-start_br-1);
       TrimWhiteSpaceBeforeAfter(retVal.inside);
    }
    return retVal;
}

size_t modules::parser::RemoveWhitespace(std::string& input){
	return RemoveWhitespace(input, input.begin(),input.end());
}

size_t modules::parser::RemoveWhitespace(std::string& input, std::string::iterator start,std::string::iterator end){
	std::string::iterator new_end=std::remove_if(start,end,modules::parser::IsWhitespace);
	input.erase(new_end,end);
	return input.size();
}

void modules::parser::TrimWhiteSpaceBeforeAfter(std::string& line){
	size_t not_white=line.find_first_not_of(" \t");
	if(not_white!=std::string::npos)
	   RemoveWhitespace(line, line.begin(),line.begin()+not_white+1);

	not_white=line.find_last_not_of(" \t");
	if(not_white!=std::string::npos)
	   RemoveWhitespace(line, line.begin()+not_white+1,line.end());
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
		case '0': case '1': case '2': case '3': case '4':
		case '5': case '6': case '7': case '8': case '9':
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

double modules::parser::GetDouble(const std::string& input, size_t start, size_t stop){
	TString tstr=input.substr(start,stop-start);
	double value=tstr.Atof();
	return value;
}

bool modules::parser::iequals(const char a, const char b){
    return ( a=='_' && b=='-' ) ||
           ( a=='-' && b=='_' ) ||
           ( tolower(a) == tolower(b));
}

bool modules::parser::iequals(const std::string& a, const std::string& b){
  unsigned int sz = a.size();
  if (b.size() != sz) return false;
  for (unsigned int i = 0; i < sz; ++i){
      if (! iequals(a[i],b[i])) return false;
  }
  return true;
}
