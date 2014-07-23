#ifndef COMMANDLINE_H
#define COMMANDLINE_H

#include <string>

struct ARGUMENTS {
  std::string infile;
  std::string outfile;
  std::string correction_file;
  std::string mod_file;
  int start;
  int stop;
  int run;
};// ARGUMENTS;

void help_command_line(const char* my_name);
void print_arguments(const ARGUMENTS& args);
bool isNumber(const char* c);
int GetRunNumber(const std::string& file_name);
int check_arguments(ARGUMENTS& arguments);
int analyze_command_line (int argc, char **argv, ARGUMENTS& arguments);
int load_config_file(const char* filename);

#endif //COMMANDLINE_H

// emacs metadata
// Local Variables:
// mode: c++
// End:
