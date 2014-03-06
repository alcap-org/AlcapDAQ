#ifndef __UTILS__H
#define __UTILS__H

#include <string.h>
static char correction_file[300];

typedef struct {
  char *infile;
  char *outfile;
  char *correction_file;
  int start;
  int stop;
  int run;
} ARGUMENTS;

void help_command_line(char *my_name){
  fprintf (stderr,"\nusage: %s  [flags] \n\n",my_name);
  fprintf (stderr,"valid options are:\n");
  fprintf (stderr,"  -i <filename>\t\t Input root tree file.\n");
  fprintf (stderr,"  -o <filename>\t\t Output root tree file.\n");  
  fprintf (stderr,"  -n <count>\t\t Analyze only <count> events.\n");
  fprintf (stderr,"     <first> <last>\t Analyze only events from "
	   "<first> to <last>.\n");
  fprintf (stderr,"  -r <PSI run number>\t\t Run number specification for the shrubs.\n");
  fprintf (stderr,"\n");
  return;
}

bool isNumber(char *c){
  for(size_t i=0; i<strlen(c); i++){
    if(!isdigit(c[i])) return false;
  }
  return true;
}

int GetRunNumber(char* input_file){
        std::string run_number=input_file;
        //remove the extension
        run_number=run_number.substr(0,run_number.find_last_of('.'));
        //find the last numbers
        run_number=run_number.substr(run_number.find_last_not_of("01234566789")+1);
        return atoi(run_number.c_str());
}

int check_arguments(ARGUMENTS& arguments){
  if(arguments.stop >0){
    if(arguments.stop <= arguments.start){
      printf("ERROR: Cannot process events from %d to %d because %d<=%d\n",
	     arguments.start, arguments.stop, arguments.stop, arguments.start);
      return 1;
    }
  }

  if(strcmp(arguments.infile,"")==0){
    printf("ERROR: Empty input file name. Did you specify the -i option?\n");
    return 2;
  }  
  if(strcmp(arguments.outfile,"")==0){
    printf("ERROR: Empty output file name. Did you specify the -o option?\n");
    return 3;
  }  
  if(arguments.run==-1){
    // No run number has been set, obtain it from the filename
    arguments.run=GetRunNumber(arguments.infile);
  }
  if(strcmp(arguments.correction_file,"")==0){
    sprintf(correction_file,"wiremap_corrections/correct%d.dat",arguments.run);
    arguments.correction_file=correction_file;
  }
  return 0; //success
}

void PrintLocation(){
    static int count=0;
    printf("At location %d\n",count++);
}

int analyze_command_line (int argc, char **argv, ARGUMENTS& arguments){
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
  if(strcmp(argv[1],"--help")==0){
      help_command_line(argv[0]);
      return 1;
  }

  for(int i=1; i<argc; /* incrementing of i done in the loop! */){
    if(argv[i][0] != '-'){
      printf("ERROR: Wrong argument %s\n",argv[i]);
      help_command_line(argv[0]); return 1;
    }

   if(strlen(&argv[i][1]) != 1){ 
     printf("ERROR: All options must be single characters, "
        "separated with a space and prefixed with a '-'\n");
      help_command_line(argv[0]); return 1;
   }
   switch(argv[i][1]){
   case 'i':
     if(i+1 < argc){
       arguments.infile = argv[i+1];
       i+=2;
     }
     else{
       printf("ERROR: No argument for input file specified\n");
       help_command_line(argv[0]); return 1;
     }
     break;
   case 'o':
     if(i+1 < argc){
       arguments.outfile = argv[i+1];
       i+=2;
     }
     else{
       printf("ERROR: No argument for input file specified\n");
       help_command_line(argv[0]); return 1;
     }
     break;
        case 's':
     if(i+1 < argc){
       arguments.correction_file = argv[i+1];
       i+=2;
     }
     else{
       printf("ERROR: No argument for TSetupData correction file specified\n");
       help_command_line(argv[0]); return 1;
     }
     break;
   case 'n':
     if(i+1 < argc){
       if(isNumber(argv[i+1])){
         arguments.start = atoi(argv[i+1]);
         i+=2;
       }
       else{
         printf("ERROR: Argument %s for option -n is not a number\n",argv[i+1]);
         help_command_line(argv[0]); return 1;
       }
       if(i < argc && argv[i][0]!='-'){
         if(isNumber(argv[i])){
      arguments.stop = atoi(argv[i]);
      i+=1;
         }
         else{
      printf("ERROR: Argument %s for option -n is not a number\n", argv[i]);
      help_command_line(argv[0]); return 1;
         }
       }
     }
     else{
       printf("ERROR: No argument for number of processed events specified\n");
       help_command_line(argv[0]); return 1;
     }
     break;
   case 'r':
     if(i+1 < argc){
       if(isNumber(argv[i+1])){
         arguments.run = atoi(argv[i+1]);
         i+=2;
       }
       else{
         printf("ERROR: Argument %s for option -r is not a number\n",argv[i+1]);
         help_command_line(argv[0]); return 1;
       }
     }
     break;
   default:
     printf("ERROR: Argument %s not recognized\n",argv[i]);
     help_command_line(argv[0]); return 1;
   } // End switch block
  } // End for loop over all args

  // Everything looks ok, so we now check the arguments
  int ret = check_arguments( arguments);
  return ret; 

}

#endif
