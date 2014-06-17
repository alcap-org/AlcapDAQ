#include <string.h>
#include "ModulesReader.h"
#include "ModulesFactory.h"
#include "CommandLine.h"
#include <stdio.h>
#include <string>
#include <stdlib.h>
#include <iostream>

void help_command_line(const char* my_name)
{
  std::cerr << "\nUsage: " << my_name << "  [options]\n"
            << "    Positional arguments: None\n\n"
            << "Valid options are:\n"
            << "  -i <filename>\t\t Input root tree file.\n"
            << "  -o <filename>\t\t Output root tree file.\n"
            << "  -n <count>\t\t Analyze only <count> events.\n"
            << "  -n <first> <last>\t Analyze only events from "
            << "<first> to <last>.\n"
            << "  -r <PSI run number>\t Run number specification for the shrubs.\n"
            << "  -s <correction file>\t Name of the correction file to be used.\n"
            << "  -m <modules file>\t Name of the MODULES file to be used.\n"
            << std::endl;
  return;
}


//bool isNumber(std::string s);
//----------------------------------------------------------------------
bool isNumber(const char* c)
{
  for ( std::size_t i=0; i<strlen(c); i++){
    if(!isdigit(c[i])) return false;
  }
  return true;
}

//----------------------------------------------------------------------
int GetRunNumber(const char* input_file)
{
  ///Assumes the file name is of the format aaaaaaSnnnnnn.xxx
  /// where 'a' is any character
  ///       'S' is any non-numeric character
  ///       'n' is the run number (includes '+' '-' '.')
  ///       'x' is any character excluding '.' 

  std::string run_number=input_file;
  //remove the extension
  run_number=run_number.substr(0,run_number.find_last_of('.'));
  //find the last numbers
  run_number=run_number.substr(run_number.find_last_not_of("01234566789")+1);
  return atoi(run_number.c_str());
}

//----------------------------------------------------------------------
static char correction_file[300];
static char mod_file[300];
int check_arguments(ARGUMENTS& arguments){
  if(arguments.stop > 0){
    if (arguments.stop <= arguments.start){
      std::cerr << "ERROR: Cannot process events from "
                << arguments.start << " to " << arguments.stop 
                << " because " << arguments.start << ">=" << arguments.stop 
                << std::endl;
      return 1;
    }
  }
  
  if(std::string(arguments.infile).size() == 0){
    std::cerr << "ERROR: Empty input file name."
              <<"  Did you specify the -i option?"  << std::endl;
    return 2;
  }
  
  if(std::string(arguments.outfile).size() == 0){
    std::cerr << "ERROR: Empty output file name."
              << "  Did you specify the -o option?\n" << std::endl;
    return 3;
  }  

  if(arguments.run == -1){
    // No run number has been set, obtain it from the filename
    arguments.run = GetRunNumber(arguments.infile);
  }

  if(std::string(arguments.mod_file).size() == 0){
    //sprintf(mod_file,"MODULES");
    sprintf(mod_file,"testModule.txt");
    arguments.mod_file=mod_file;
  }
  if(std::string(arguments.correction_file).size() == 0){
    sprintf(correction_file,"wiremap_corrections/correct%d.dat",arguments.run);
    arguments.correction_file=correction_file;
  }
  return 0; //success
}

//----------------------------------------------------------------------
void PrintLocation()
{
  static int count=0;
  std::cout << "At location" << count++ << std::endl;
}
//----------------------------------------------------------------------
int analyze_command_line (int argc, char **argv, ARGUMENTS& arguments)
{
  // Inialise the arguments to be consistently meaningless
  arguments.infile="";
  arguments.outfile="";
  arguments.correction_file="";
  arguments.start=0;
  arguments.stop=0;
  arguments.run=-1;

  // Now loop over all the arguments 
  if(argc==1) {
    help_command_line(argv[0]);
    return 1;
  }
  if(std::string(argv[1]) == "--help" ){
    help_command_line(argv[0]);
    return 1;
  }

  for(int i=1; i<argc; /* incrementing of i done in the loop! */){
    if(argv[i][0] != '-'){
      std::cerr << "ERROR: Wrong argument " << argv[i] << std::endl;
      help_command_line(argv[0]);
      return 1;
    }
    
   if(strlen(&argv[i][1]) != 1){ 
     std::cerr << "ERROR: All options must be single characters, "
               << "separated with a space and prefixed with a '-'"
               << std::endl;
      help_command_line(argv[0]);
      return 1;
   }

   switch(argv[i][1]){
   case 'm':
     if(i+1 < argc){
       arguments.mod_file = argv[i+1];
       i+=2;
     }
     else {
       std::cerr << "ERROR: No argument for module file specified\n";
       help_command_line(argv[0]);   return 1;
     }
     break;

     //----------
   case 'i':
     if(i+1 < argc){
       arguments.infile = argv[i+1];
       i+=2;
     }
     else{
       std::cerr << "ERROR: No argument for input file specified\n";
       help_command_line(argv[0]);   return 1;
     }
     break;
     
     //----------
   case 'o':
     if(i+1 < argc){
       arguments.outfile = argv[i+1];
       i+=2;
     }
     else{
       std::cerr << "ERROR: No argument for input file specified\n";
       help_command_line(argv[0]);   return 1;
     }
     break;

     //----------
   case 's':
     if(i+1 < argc){
       arguments.correction_file = argv[i+1];
       i+=2;
     }
     else{
       std::cerr << "ERROR: No argument for TSetupData correction file specified\n";
       help_command_line(argv[0]);   return 1;
     }
     break;

     //----------
   case 'n':
     if(i+1 < argc){
       if(isNumber(argv[i+1])){
         arguments.start = atoi(argv[i+1]);
         i+=2;
       }
       else {
         std::cerr << "ERROR: Argument " << argv[i+1]
                   << " for option -n is not a number\n";
         help_command_line(argv[0]);   return 1;
       }
       if(i < argc && argv[i][0] != '-'){
         if(isNumber(argv[i])){
           arguments.stop = atoi(argv[i]);
           i+=1;
         }
         else{
           std::cerr << "ERROR: Argument " << argv[i+1]
                     << " for option -n is not a number\n";
           help_command_line(argv[0]);   return 1;
         }
       }
     }
     else{
       std::cerr << "ERROR: No argument for number of processed events specified\n";
       help_command_line(argv[0]);   return 1;
     }
     break;

     //----------
   case 'r':
     if(i+1 < argc){
       if(isNumber(argv[i+1])){
         arguments.run = atoi(argv[i+1]);
         i+=2;
       }
       else{
         std::cerr << "ERROR: Argument " << argv[i+1]
                   << " for option -r is not a number\n";
         help_command_line(argv[0]);   return 1;
       }
     }
     break;

     //----------
   default:
     std::cerr << "ERROR: Argument " << argv[i] << " not recognized\n";
     help_command_line(argv[0]);   return 1;
   } // End switch block
  } // End for loop over all args

  // Everything looks ok, so we now check the arguments
  return check_arguments(arguments);
}

//----------------------------------------------------------------------
int load_config_file(const char* filename){
  modules::reader modules_file;
  modules_file.ReadFile(filename);
  modules_file.PrintAllOptions();

  modules::factory* mgr = modules::factory::Instance();
  size_t num_modules = modules_file.GetNumModules();
  std::cout << "number of modules requested: " << num_modules << std::endl;
  std::string name;
  modules::options* opts;
  modules::BaseModule** mods=NULL;
  for(unsigned i = 0; i < num_modules; i++){
    name = modules_file.GetModule(i);
    opts = modules_file.GetOptions(i);
    std::cout << "Creating module: " << name
              << "\nWith options:" << std::endl;
    opts->DumpOptions("  ");
    mods[i] = mgr->createModule(name,opts);
  }
  
  return 0;
}

//----------------------------------------------------------------------
void print_arguments(const ARGUMENTS& args){
  std::cout << "ARGUMENTS struct:"
            << "\n    infile:\t\t" << args.infile
            << "\n    outfile:\t\t" << args.outfile
            << "\n    correction file:\t" << args.correction_file
            << "\n    mod file:\t\t" << args.mod_file
            << "\n    start event:\t" << args.start
            << "\n    stop event:\t\t" << args.stop
            << "\n    run number:\t\t" << args.run
            << std::endl;
}
