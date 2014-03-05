#ifndef __UTILS__H
#define __UTILS__H

#include <string.h>

typedef struct {
  char *infile;
  char *outfile;
  char *correction_file;
  char *mod_file;
  int start;
  int stop;
  int run;
} ARGUMENTS;

void help_command_line(char *my_name);
bool isNumber(char *c);
int GetRunNumber(char* input_file);
int check_arguments(ARGUMENTS& arguments);
int analyze_command_line (int argc, char **argv, ARGUMENTS& arguments);
int load_config_file(const char* filename);
#endif
