#ifndef COMMANDLINE_H
#define COMMANDLINE_H

#include <string>

struct ARGUMENTS {
  std::string infilelocation;
  std::string outfile;
  std::string mod_file;
  int start;
  int stop;
};// ARGUMENTS;

void help_command_line(const char* my_name);
void print_arguments(const ARGUMENTS& args);
bool isNumber(const char* c);

/// @brief Obtains the run number from a file name
/// @details Assumes the file name is of the format
/// aaaaaaSnnnnnn.xxx where:
///       'a' is any character
///       'S' is any non-numeric character
///       'n' is the run number (includes '+' '-' '.')
///       'x' is any character excluding '.' 
int GetRunNumber(const std::string& file_name);
int check_arguments(ARGUMENTS& arguments);
int analyze_command_line (int argc, char **argv, ARGUMENTS& arguments);
int load_config_file(const char* filename);

#endif //COMMANDLINE_H

// emacs metadata
// Local Variables:
// mode: c++
// End:
