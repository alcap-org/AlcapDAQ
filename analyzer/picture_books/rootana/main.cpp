#include <iostream>
#include <sstream>
#include <algorithm>

#include "TFile.h"
#include "TKey.h"
#include "TCanvas.h"
#include "TH1.h"
#include "TH2.h"
#include "TStyle.h"
#include "TError.h"

#include "CommandLine.h"
#include "PictureBook.h"
#include "BaseChapter.h"
#include "BasePlot.h"
#include "TrendPlot.h"

std::vector<BaseChapter*> gChapters; // a global container for the chapters (sorry, Ben)

extern Int_t gErrorIgnoreLevel;

void RecursiveHistogramPlotting(TDirectoryFile* dir, std::vector<BaseChapter*>::const_iterator chapterIter, int i_run);

// Keep track of the number of plots so that we can insert new pages when we want
int n_plots = 0;
const int n_plots_per_page = 2;

PictureBook* pic_book = NULL;

ARGUMENTS arguments;

// Create the trend plot outside of the loop through the runs
std::map<std::string, TrendPlot*> trend_plots;

int main(int argc, char **argv) {

  // Don't want ROOT to print out a line for every plot it saves
  gErrorIgnoreLevel = kFatal;

  // Read in the command line arguments to find out where the input files are,
  // where the output should go, which runs to analyze and which format file to use
  int ret = analyze_command_line (argc, argv,arguments);
  if(ret!=0) return ret;

  print_arguments(arguments);

  ret = load_config_file(arguments.mod_file.c_str());
  if(ret!=0) return ret;

  // Create the picture book
  pic_book = new PictureBook(arguments.outfile);

  // Open all the TFiles that we will want
  std::vector<TFile*> run_files;
  for (int i_run = arguments.start; i_run < arguments.stop; ++i_run) {
    std::stringstream filename;
    filename << arguments.infilelocation << "out0" << i_run << ".root";
    
    TFile* file = new TFile(filename.str().c_str(), "READ");
    run_files.push_back(file);
  }

  // Loop through the chapters
  for (std::vector<BaseChapter*>::const_iterator chapterIter = gChapters.begin(); chapterIter != gChapters.end(); ++chapterIter) {
    
    // Start the new chapter
    std::string chapter_name = (*chapterIter)->GetChapterName();
    pic_book->StartNewSection(chapter_name);

    // Get the name of the rootana module that we will want to get the histograms from
    std::string module_name = (*chapterIter)->GetModuleName();
    if (module_name == "0") {
      std::cout << "Problem with the module name for chapter " << chapter_name << ". Aborting..." << std::endl;
      return 0;
    }
    
    // Now loop through the runs we want to run over
    const int first_run = arguments.start;
    const int n_runs = arguments.stop - arguments.start;
    for (int i_run = 0; i_run < n_runs; ++i_run) {

      TFile* file = run_files.at(i_run);

      if ( file->IsZombie() ) {
	//	std::cout << "Problems opening file for run " << first_run+i_run << ", ignoring it." << std::endl;
	continue;
      }

      // Get the module directory
      TDirectoryFile* dir = (TDirectoryFile*) file->Get(module_name.c_str());

      if (!dir) {
	std::cout << "The directory " << module_name << " doesn't seem to exist in the input file. Aborting..." << std::endl;
	return 0;
      }

      RecursiveHistogramPlotting(dir, chapterIter, i_run);

    } // end loop through runs
  } // end loop through chapters

  delete pic_book;
  return 0;
}

void RecursiveHistogramPlotting(TDirectoryFile* dir, std::vector<BaseChapter*>::const_iterator chapterIter, int i_run) {

  // Get everything we could possibly want from the BaseChapter
  std::string chapter_name = (*chapterIter)->GetChapterName();
  std::string plot_type = (*chapterIter)->GetPlotType();
  std::string draw_option = (*chapterIter)->GetDrawOption();
  std::string sub_plot_type = (*chapterIter)->GetSubPlotType();

  bool log_x = (*chapterIter)->GetLogX();
  bool log_y = (*chapterIter)->GetLogY();
  bool log_z = (*chapterIter)->GetLogZ();

  bool stats_box = (*chapterIter)->GetStatsBox();
  bool is_trend_plot = (*chapterIter)->GetIsTrendPlot();
  bool auto_zoom = (*chapterIter)->GetAutoZoom();

  std::vector<int> low_limits, high_limits;
  low_limits.push_back((*chapterIter)->GetXLow()); low_limits.push_back((*chapterIter)->GetYLow()); low_limits.push_back((*chapterIter)->GetZLow());
  high_limits.push_back((*chapterIter)->GetXHigh()); high_limits.push_back((*chapterIter)->GetYHigh()); high_limits.push_back((*chapterIter)->GetZHigh());

  const int n_runs = arguments.stop - arguments.start;

  TIter nextDirKey(dir->GetListOfKeys()); // get the list of keys in the directory (should include the histograms)
  TKey *dirKey;
  
  while ( (dirKey = (TKey*)nextDirKey()) ) {

    TObject* obj = dirKey->ReadObj();

    if (obj->IsA() == TDirectoryFile::Class()) {
      RecursiveHistogramPlotting( (TDirectoryFile*) obj, chapterIter, i_run);
    }

    std::string histogram_name = obj->GetName();

    // If we have been told that we want a specific plot type for this chapter,
    // check that this is one of the plots we want
    if (plot_type != "all") {

      if (histogram_name.find(plot_type) == std::string::npos) { // if it isn't
	continue;
      }
      // Check for the next string
      if (sub_plot_type != "all") {
	if (histogram_name.find(sub_plot_type) == std::string::npos) { // if it isn't
	  continue;
	}
      }
    }
    else {
      // Check that it's not the TString that we store in each directory
      if (strcmp(obj->ClassName(), "TObjString") == 0) {
	continue;
      }
    }

    // Set up the canvas and get the histogram
    BasePlot* run_plot = new BasePlot(new TCanvas(), (TH1*) obj);
    TrendPlot* trend_plot = NULL;

    // Create the trend plot if we want (but only for plots that are initially one-dimensional)
    if (is_trend_plot) {

      if (trend_plots.find(histogram_name) == trend_plots.end()) {
	if (obj->IsA() == TH1F::Class()) {
	  // Create the trend plot and add it to the map
	  trend_plot = new TrendPlot(histogram_name, new TCanvas(), run_plot->GetPlot(), arguments);
	  trend_plots[histogram_name] = trend_plot;
	}
	else {
	  std::cout << "Error: Currently trend plots are only available for 1D-dimensional histograms. Skipping this histogram... (" << chapter_name << ")" << std::endl;
	  continue;
	}
      }
      else {
	trend_plot = trend_plots.find(histogram_name)->second;
      }
      trend_plot->FillTrendPlot(run_plot->GetPlot(), arguments, i_run);
    }

    // Specify which plot wants to be printed out
    BasePlot* plot = NULL;
    if (is_trend_plot && trend_plot != NULL) {
      plot = trend_plot;
    }
    else {
      plot = run_plot;
    }

    // Draw the plot as we want it
    plot->SetLogScales(log_x, log_y, log_z);
    plot->SetStats(stats_box);

    // Set the limits on each axis
    if (auto_zoom) {
      plot->AutoZoom();
    }
    else { // try and use the limits
      plot->SetLimits(low_limits, high_limits);
    }
	
    // Save the plot as a PNG (if it's a run plot or it's a trend plot and we are at the last run)
    if (!is_trend_plot || (is_trend_plot && i_run == n_runs-1)) {

      std::stringstream pngname;
      pngname << "plots/";
      if (is_trend_plot) {
	pngname << "TrendPlot_";
      }
      pngname << histogram_name << "_" << i_run << ".png";

      std::string pngname_str = pngname.str();
      std::replace(pngname_str.begin(), pngname_str.end(), '#', '_'); // need to replace # so that latex works
      std::replace(pngname_str.begin(), pngname_str.end(), ' ', '_'); // need to replace # so that latex works

      plot->Save(draw_option, pngname_str);
	
      // Add the figure to the latex document
      pic_book->InsertFigure(pngname_str);
      ++n_plots;
    }

    if (n_plots % n_plots_per_page == 0) {
      pic_book->StartNewPage();
    }
  } // end loop through keys
}
