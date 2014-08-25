#include <string.h>
#include "ModulesReader.h"
//#include "ModulesFactory.h"
#include "CommandLine.h"
#include <stdio.h>
#include <string>
#include <sstream>
#include <cstdlib>
#include <iostream>
#include <stdexcept>

#include "BaseChapter.h"

extern std::vector<BaseChapter*> gChapters;

void help_command_line(const char* my_name)
{
  std::cerr << "\nUsage: " << my_name << "  [options]\n"
            << "    Positional arguments: None\n\n"
            << "Valid options are:\n"
            << "  -d <directory>\t\t Location of the input root files with histograms.\n"
            << "  -o <filename>\t\t Output .tex name.\n"
            << "  -r <first> <last>\t\t Only analyze run numbers between <first> and <last>.\n"
            << "  -m <modules file>\t Name of the format file to be used.\n"
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
int GetRunNumber(const std::string& file_name)
{
  ///Assumes the file name is of the format aaaaaaSnnnnnn.xxx
  /// where 'a' is any character
  ///       'S' is any non-numeric character
  ///       'n' is the run number (includes '+' '-' '.')
  ///       'x' is any character excluding '.' 

  //remove the extension
  std::string name = file_name.substr(0,file_name.find_last_of('.'));

  //find the last numbers
  name = name.substr(name.find_last_not_of("01234566789")+1);

  //Convert the rest
  int number (-1);
  std::stringstream(name) >> number;
  if (number == -1) {
    std::cerr << "Unable to deduce run number from \"" << name 
              << "\" in \"" << file_name << "\"" << std::endl;
    throw std::invalid_argument("not a valid int");
  }
  return number;
}

//----------------------------------------------------------------------
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
  
  if(arguments.infilelocation.size() == 0){
    std::cerr << "ERROR: Empty input file location name."
              <<"  Did you specify the -i option?"  << std::endl;
    return 2;
  }
  
  if(arguments.outfile.size() == 0){
    std::cerr << "ERROR: Empty output file name."
              << "  Did you specify the -o option?\n" << std::endl;
    return 3;
  }  

  if(arguments.mod_file.size() == 0){
    arguments.mod_file = "testModule.txt";
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
  arguments.infilelocation="";
  arguments.outfile="";
  arguments.start=0;
  arguments.stop=0;

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
   case 'd':
     if(i+1 < argc){
       arguments.infilelocation = argv[i+1];
       i+=2;
     }
     else{
       std::cerr << "ERROR: No argument for input file location specified\n";
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
       std::cerr << "ERROR: No argument for output file specified\n";
       help_command_line(argv[0]);   return 1;
     }
     break;

     //----------
   case 'r': 
     {
       //detemine how many integer arguments are associated with the -n
       int nArgs = 0;
       for (int j = 1; j < argc-i; ++j) {
         if ( !isNumber(argv[i+j]) ) break;
         ++nArgs;
       }
       if ( nArgs > 2 || nArgs < 1) {
         std::cerr << "ERROR: " << nArgs 
                   << " non-negative integer  arguments passed to"
                   << " -n option , which accepts only one or two";
         help_command_line(argv[0]);   return 1;        
       }
       arguments.start = (nArgs==2) ? atoi(argv[i+1]) : 0; 
       arguments.stop = (nArgs==2) ? atoi(argv[i+2]) : atoi(argv[i+1]);
       i += (nArgs+1);
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

  //  modules::factory* mgr = modules::factory::Instance();
  size_t num_chapters = modules_file.GetNumModules();
  std::cout << "number of chapters requested: " << num_chapters << std::endl;
  std::string name;
  modules::options* opts;
  for(unsigned i = 0; i < num_chapters; i++){
    name = modules_file.GetModule(i);
    opts = modules_file.GetOptions(i);
    std::cout << "Creating chapter: " << name
              << "\nWith options:" << std::endl;
    opts->DumpOptions("  ");
    BaseChapter* chapter = new BaseChapter(name,opts);
    gChapters.push_back(chapter);
  }
  
  return 0;
}

//----------------------------------------------------------------------
void print_arguments(const ARGUMENTS& args){
  std::cout << "ARGUMENTS struct:"
            << "\n    infilelocation:\t\t" << args.infilelocation
            << "\n    outfile:\t\t" << args.outfile
            << "\n    mod file:\t\t" << args.mod_file
            << "\n    start run:\t" << args.start
            << "\n    stop run:\t\t" << args.stop
            << std::endl;
}
