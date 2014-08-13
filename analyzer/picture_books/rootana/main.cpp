#include <iostream>
#include <sstream>
#include <algorithm>

#include "TFile.h"
#include "TKey.h"
#include "TCanvas.h"
#include "TH1.h"

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

    // Now loop through the runs we want to run over
    for (int i_run = arguments.start; i_run < arguments.stop; ++i_run) {

      std::stringstream filename;
      filename << arguments.infilelocation << "out0" << i_run << ".root";
      
      TFile* file = new TFile(filename.str().c_str(), "READ");

      // Get the module directory
      TDirectoryFile* dir = (TDirectoryFile*) file->Get(module_name.c_str());

      if (!dir) {
	std::cout << "The directory " << module_name << " doesn't seem to exist in the input file. Aborting..." << std::endl;
	return 0;
      }

      TIter nextDirKey(dir->GetListOfKeys()); // get the list of keys in the directory (should include the histograms)
      TKey *dirKey;
  
      while ( (dirKey = (TKey*)nextDirKey()) ) {
	// At the moment, get all the histograms
	if (strcmp(dirKey->ReadObj()->ClassName(), "TH1F") == 0) {

	  // Set up the canvas
	  TCanvas *c1 = new TCanvas();

	  std::string histogram_name = dirKey->ReadObj()->GetName();

	  TH1F* hPlot = (TH1F*) dirKey->ReadObj();
	  hPlot->Draw();
	  
	  // Save the plot as a PNG
	  std::string pngname = "plots/" + histogram_name + ".png";
	  std::replace(pngname.begin(), pngname.end(), '#', '_'); // need to replace # so that latex works
	  c1->SaveAs(pngname.c_str());

	  delete c1;

	  // Add the figure to the latex document
	  pic_book->InsertFigure(pngname);
	}
      }
    }
  }

  delete pic_book;
  return 0;
}
