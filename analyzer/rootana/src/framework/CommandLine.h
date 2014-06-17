#ifndef COMMANDLINE_H
#define COMMANDLINE_H


#include <string.h>

struct ARGUMENTS {
  char *infile;
  char *outfile;
  char *correction_file;
  char *mod_file;
  int start;
  int stop;
  int run;
};// ARGUMENTS;

void help_command_line(const char* my_name);
void print_arguments(const ARGUMENTS& args);
bool isNumber(const char* c);
int GetRunNumber(const char* input_file);
int check_arguments(ARGUMENTS& arguments);
int analyze_command_line (int argc, char **argv, ARGUMENTS& arguments);
int load_config_file(const char* filename);

#endif //COMMANDLINE_H
