#include <iostream>
#include <sstream>

#include "TFile.h"

#include "CommandLine.h"
#include "PictureBook.h"
#include "BaseChapter.h"

std::vector<BaseChapter*> gChapters; // a global container for the chapters (sorry, Ben)

int main(int argc, char **argv) {

  // Read in the command line arguments to find out where the input files are,
  // where the output should go, which runs to analyze and which format file to use
  ARGUMENTS arguments;
  int ret = analyze_command_line (argc, argv,arguments);
  if(ret!=0) return ret;

  print_arguments(arguments);

  ret = load_config_file(arguments.mod_file.c_str());
  if(ret!=0) return ret;

  // Create the picture book
  PictureBook* pic_book = new PictureBook(arguments.outfile);

  // Loop through the chapters
  for (std::vector<BaseChapter*>::const_iterator chapterIter = gChapters.begin(); chapterIter != gChapters.end(); ++chapterIter) {
    std::string chapter_name = (*chapterIter)->GetChapterName();
    pic_book->StartNewSection(chapter_name);

    std::string module_name = (*chapterIter)->GetModuleName();
    if (module_name == "0") {
      std::cout << "Problem with the module name for chapter " << chapter_name << ". Aborting..." << std::endl;
      return 0;
    }

    std::cout << "\nModule Name = " << module_name << std::endl;
  }

  std::stringstream filename;
  filename << arguments.infilelocation << "/out0" << arguments.start << ".root";

  TFile* file = new TFile(filename.str().c_str(), "READ");
  file->ls();

  delete pic_book;
  return 0;
}
