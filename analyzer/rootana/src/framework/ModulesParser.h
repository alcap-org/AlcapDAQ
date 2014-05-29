#ifndef MODULESPARSER_H_
#define MODULESPARSER_H_
#include <string>
#include <vector>
#include <utility>
#include <exception>

namespace modules{
     namespace parser{
	namespace errors{
	// Parsing Errors
	class unmatched_parenthesis;
	}
     }
}

class modules::parser::errors::unmatched_parenthesis:public std::exception{
	public:
		unmatched_parenthesis(char open, char close):
			fOpen(open),fClose(close){};
	virtual const char* what() const throw()
		{
		      std::string msg = "Unmatched parethensis '";
		      msg+=fOpen;
		      msg+="'. Looking for '";
		      msg+=fClose;
		      msg+="'.";
		      return msg.c_str();
		}
	private:
	char fOpen, fClose;

};

namespace modules{
     namespace parser{
        const int max_line=2048;
     
        int TokeniseByDelimiter(const std::string& input, 
     		   std::vector<std::string>& output,
     		   const char* delim);
     
        int TokeniseByWhiteSpace(const std::string& input,
     		   std::vector<std::string>& output);
     
        std::string GetOneWord(const std::string& in,
     		   size_t start=0,
     		   size_t stop=std::string::npos);

	std::pair<std::string,std::string> ParseConstructor(std::string input, char open='(', char close=')') 
		throw(modules::parser::errors::unmatched_parenthesis);

	size_t RemoveWhitespace(std::string& input);

	bool IsWhitespace(char in);
        bool IsDigit(char in);
        bool IsDecimal(char in);
        bool IsFloatChar(char in);
        bool IsNotFloatChar(char in);
        bool IsNumber(const std::string& input);
        int GetNumber(const std::string& input);
    }
}

#endif //MODULESPARSER_H_