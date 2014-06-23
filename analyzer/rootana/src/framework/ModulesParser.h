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
	struct Constructor_t{
		std::string before;
		std::string inside;
	};
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

	Constructor_t ParseConstructor(const std::string& input, char open='(', char close=')') 
		throw(modules::parser::errors::unmatched_parenthesis);

	size_t RemoveWhitespace(std::string& input);
	size_t RemoveWhitespace(std::string& input, std::string::iterator start,std::string::iterator end);
	void TrimWhiteSpaceBeforeAfter(std::string& input);

	bool IsWhitespace(char in);
        bool IsDigit(char in);
        bool IsDecimal(char in);
        bool IsFloatChar(char in);
        bool IsNotFloatChar(char in);
        bool IsNumber(const std::string& input);
        int GetNumber(const std::string& input);
	double GetDouble(const std::string& input, size_t start=0, size_t stop=std::string::npos);
  bool iequals(const std::string& a, const std::string& b);
  bool iequals(const char a, const char b);
    }
    
}

#endif //MODULESPARSER_H_
