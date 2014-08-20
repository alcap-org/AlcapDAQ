#include "debug_tools.h"
#include "ModulesParser.h"
#include <sstream>
#include <string.h>
#include <algorithm>
#include <functional>
#include <stdio.h>
#include <iostream>
#include <TString.h>
#include <ctype.h>
using std::cout;
using std::endl;

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

const std::string& modules::parser::ToCppValid(const std::string& input){
    return ReplaceAll(input," :{}#*()-=+$%^&!.,/?","_" );
}

void modules::parser::ToCppValid(std::string& input){
    ReplaceAll(input," :{}#*()-=+$%^&!.,/?","_" );
}

const std::string& modules::parser::ReplaceAll(const std::string& input, const std::string& search,const std::string& replace ){
    static std::string output;
    output.clear();
    for(std::string::const_iterator i_char=input.begin(); i_char!=input.end();++i_char){
        if(std::find(search.begin(),search.end(), *i_char)!=search.end()){
            output+=replace;
        } else{
            output+=*i_char;
        }
    }
    return output;
}

void modules::parser::ReplaceAll(std::string& input, const std::string& search, const std::string& replace ){
    input=ReplaceAll(const_cast<const std::string&>(input),search,replace);
}

void modules::parser::ReplaceWords(std::string& input, const std::string& search, const std::string& replace){
    size_t match_start;
    for( match_start=input.find(search);
         match_start!=std::string::npos;
         match_start=input.find(search)){
        input=input.substr(0,match_start) + replace + input.substr(match_start+search.length());
    }
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

bool modules::parser::IsTrue(const std::string& val){
    return  (val=="true")|| (val=="TRUE")|| (val=="YES")|| (val=="yes")|| (val=="on")|| (val=="ON")|| (val=="1");
}
