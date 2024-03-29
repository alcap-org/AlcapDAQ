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
#include "TrendPlot.h"

std::vector<BaseChapter*> gChapters; // a global container for the chapters (sorry, Ben)

extern TStyle* gStyle;
extern Int_t gErrorIgnoreLevel;

int main(int argc, char **argv) {

  gStyle->SetStatX(0.9);
  gStyle->SetStatY(0.9);

  // Don't want ROOT to print out a line for every plot it saves
  gErrorIgnoreLevel = kFatal;

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

  // Keep track of the number of plots so that we can insert new pages when we want
  int n_plots = 0;
  const int n_plots_per_page = 2;

  // Open all the TFiles that we will want
  std::vector<TFile*> run_files;
  for (int i_run = arguments.start; i_run < arguments.stop; ++i_run) {
    std::stringstream filename;
    filename << arguments.infilelocation << "out0" << i_run << ".root";
    
    TFile* file = new TFile(filename.str().c_str(), "READ");
    run_files.push_back(file);
  }

  // open a root file to save all plots to as well as the picture book
  TFile* outFile=TFile::Open("plots/all_plots.root","Recreate");

  // Loop through the chapters
  for (std::vector<BaseChapter*>::const_iterator chapterIter = gChapters.begin(); chapterIter != gChapters.end(); ++chapterIter) {
    
    // Get everything we could possibly want from the BaseChapter
    std::string chapter_name = (*chapterIter)->GetChapterName();
    std::string plot_type = (*chapterIter)->GetPlotType();
    std::string draw_option = (*chapterIter)->GetDrawOption();
    std::string sub_plot_type = (*chapterIter)->GetSubPlotType();

    bool log_x = (*chapterIter)->GetLogX();
    bool log_y = (*chapterIter)->GetLogY();
    bool log_z = (*chapterIter)->GetLogZ();

    std::string stats_box = (*chapterIter)->GetStatsBox();
    bool is_trend_plot = (*chapterIter)->GetIsTrendPlot();
    bool auto_zoom = (*chapterIter)->GetAutoZoom();

    const int n_axes = 3;
    int low_limits[n_axes] = { (*chapterIter)->GetXLow(), (*chapterIter)->GetYLow(), (*chapterIter)->GetZLow() };
    int high_limits[n_axes] = { (*chapterIter)->GetXHigh(), (*chapterIter)->GetYHigh(), (*chapterIter)->GetZHigh() };


    // Start the new chapter
    pic_book->StartNewSection(chapter_name);

    // Get the name of the rootana module that we will want to get the histograms from
    std::string module_name = (*chapterIter)->GetModuleName();
    if (module_name == "0") {
      std::cout << "Problem with the module name for chapter " << chapter_name << ". Aborting..." << std::endl;
      return 0;
    }
    
    // Create the trend plot outside of the loop through the runs
    std::map<std::string, TrendPlot*> trend_plots;

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

      TIter nextDirKey(dir->GetListOfKeys()); // get the list of keys in the directory (should include the histograms)
      TKey *dirKey;
  
      while ( (dirKey = (TKey*)nextDirKey()) ) {

	std::string histogram_name = dirKey->ReadObj()->GetName();
	std::cout << histogram_name << std::endl;

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
	  if (strcmp(dirKey->ReadObj()->ClassName(), "TObjString") == 0) {
	    continue;
	  }
	}

	// Set up the canvas and get the histogram
	TCanvas *c1 = new TCanvas();
	TH1* hRunPlot = (TH1*) dirKey->ReadObj();
	TrendPlot* trend_plot = NULL;

	// Create the trend plot if we want (but only for plots that are initially one-dimensional)
	if (is_trend_plot) {

	  if (trend_plots.find(histogram_name) == trend_plots.end()) {
	    if (hRunPlot->InheritsFrom("TH1") && !hRunPlot->InheritsFrom("TH2") && !hRunPlot->InheritsFrom("TH3")) {
	      // Create the trend plot and add it to the map
	      trend_plot = new TrendPlot(histogram_name, hRunPlot, arguments);
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
	  trend_plot->FillTrendPlot(hRunPlot, arguments, i_run);
	}

	// Specify which plot wants to be printed out
	TH1* hPlot = NULL;
	if (is_trend_plot && trend_plot != NULL) {
	  hPlot = trend_plot->GetTrendPlot();
	}
	else {
	  hPlot = hRunPlot;
	}

	// Draw the plot as we want it
	c1->SetLogx(log_x);
	c1->SetLogy(log_y);
	c1->SetLogz(log_z);

        if(stats_box=="0"){
	  hPlot->SetStats(0);
	} else {
	  gStyle->SetOptStat(stats_box.c_str());
	}

	// Set the limits on each axis
	if (auto_zoom) {
	  // Auto zoom the axes here

	  // If it's a trend plot then we want to zoom on both axes
	  if (is_trend_plot) {
	    // Zoom on both
	    int max_bin = 0;
	    
	    // Loop through the y-axis first and then see which run has the highest bin
	    for (int j_bin = hPlot->GetNbinsY(); j_bin > 0; --j_bin) {
	      
	      for (int i_bin = hPlot->GetNbinsX(); i_bin > 0; --i_bin) {
		
		if (hPlot->GetBinContent(i_bin, j_bin) > 0) {
		  
		  // See if this bin is higher than the previous
		  if (j_bin > max_bin) {
		    max_bin = j_bin;
		    break;
		  }
		}
	      }
	    }
	    
	    hPlot->GetYaxis()->SetRange(1, max_bin + 5); // set the range based on bin number
	  }
	  else {
	    // Zoom on x-axis only
	    int min_bin = 1;
	    int max_bin = 1;
	    
	    for (int i_bin = 1; i_bin <= hPlot->GetNbinsX(); ++i_bin) {
	      if (hPlot->GetBinContent(i_bin) >= 1) {
		if (i_bin > min_bin) {
		  min_bin = i_bin;
		  break;
		}
	      }
	    }

	    for (int i_bin = hPlot->GetNbinsX(); i_bin > 0; --i_bin) {
	      if (hPlot->GetBinContent(i_bin) >= 1) {
		// See if this bin is higher than the previous
		if (i_bin > max_bin) {
		  max_bin = i_bin;
		  break;
		}
	      }
	    }
	    
	    hPlot->GetXaxis()->SetRange(min_bin, max_bin); // set the range based on bin number
	  }
	}
	else { // try and use the limits
	  // First, get the current limits on the plot so we can provide warnings if the user asks for a range outside of these
	  int current_low_limits[n_axes] = { hPlot->GetXaxis()->GetXmin(), hPlot->GetYaxis()->GetXmin(), hPlot->GetZaxis()->GetXmin() };
	  int current_high_limits[n_axes] = { hPlot->GetXaxis()->GetXmax(), hPlot->GetYaxis()->GetXmax(), hPlot->GetZaxis()->GetXmax() };
	  TAxis* axis[n_axes] = { hPlot->GetXaxis(), hPlot->GetYaxis(), hPlot->GetZaxis() };
	  // Now loop through the axes and set the limits
	  for (int i_axis = 0; i_axis < n_axes; ++i_axis) {
	    
	    // Give a warning if the user has specified an out of range limit
	    if (low_limits[i_axis] != -999999 && low_limits[i_axis] < current_low_limits[i_axis]) {
	      std::cout << "Warning: Lower limit on axis " << i_axis << " specified by the user (" << low_limits[i_axis] 
			<< ") is outside of the current range of the plots (" << current_low_limits[i_axis] 
			<< ") and so won't have any effect. (" << chapter_name << ")" << std::endl;
	    }
	    if (high_limits[i_axis] != -999999 && high_limits[i_axis] > current_high_limits[i_axis]) {
	      std::cout << "Warning: Upper limit on axis " << i_axis << " specified by the user (" << high_limits[i_axis] 
			<< ") is outside of the current range of the plots (" << current_high_limits[i_axis] 
			<< ") and so won't have any effect. (" << chapter_name << ")" << std::endl;
	    }
	    
	    // Now set the axis limits
	    if (low_limits[i_axis] != -999999 && high_limits[i_axis] != -999999) {
	      axis[i_axis]->SetRangeUser(low_limits[i_axis], high_limits[i_axis]);
	    }
	    else if (low_limits[i_axis] == -999999 && high_limits[i_axis] != -999999) {
	      axis[i_axis]->SetRangeUser(current_low_limits[i_axis], high_limits[i_axis]);
	    }
	    else if (low_limits[i_axis] != -999999 && high_limits[i_axis] == -999999) {
	      axis[i_axis]->SetRangeUser(low_limits[i_axis], current_high_limits[i_axis]);
	    }
	  }
	}

	hPlot->Draw(draw_option.c_str());
	c1->Update();
	  
	
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
	  c1->SaveAs(pngname_str.c_str());
	  delete c1;
	
	  // Add the figure to the latex document
	  pic_book->InsertFigure(pngname_str);
	  ++n_plots;
	}

	if (n_plots % n_plots_per_page == 0) {
	  pic_book->StartNewPage();
	}
      } // end loop through keys
    } // end loop through runs
  } // end loop through chapters

  outFile->Write();
  outFile->Close();

  delete pic_book;
  std::cout<<"Finished"<<std::endl;
  return 0;
}
