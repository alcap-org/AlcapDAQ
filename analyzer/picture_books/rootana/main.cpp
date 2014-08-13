#include <iostream>

#include "CommandLine.h"

int main(int argc, char **argv) {

  ARGUMENTS arguments;
  int ret = analyze_command_line (argc, argv,arguments);
  if(ret!=0) return ret;

  print_arguments(arguments);
}
