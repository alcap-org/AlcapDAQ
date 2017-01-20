#include "TFile.h"
#include "TH1.h"
#include "TCanvas.h"
#include "TDirectory.h"
#include "TList.h"
#include "TRandom3.h"
#include "TLatex.h"
#include "TTree.h"
#include "TLine.h"

#include <iostream>
#include <cmath>

// This macro draws one exported pulse from each channel
void DrawTPIsAndTAPs_fromPlotTPI_CutOffPulses(std::string filename, int seed = 0) {

  // Get the trend tree so I can draw the thresholds
  TFile* trendfile = new TFile("scripts/ExportPulse/odbtrends.root", "READ");
  TTree* trendtree = (TTree*) trendfile->Get("odbtrend");

  TFile* file = new TFile(filename.c_str(), "READ");
  if (file->IsZombie()) {
    std::cout << "Problem opening file " << filename << std::endl;
    return;
  }

  // Get the main plot so that we know how many channels there are
  TH1F* hMain = (TH1F*) file->Get("PlotTPI_CutOffPulses/fPulsesPlottedPerDetectorHist");

  int n_pulses_to_draw = hMain->GetNbinsX();
  TCanvas* c1 = new TCanvas("c1", "c1");
  int x_pads = std::floor(std::sqrt(n_pulses_to_draw));
  int y_pads = std::ceil(std::sqrt(n_pulses_to_draw));
  c1->Divide(x_pads, y_pads);
  //  std::cout << "AE: n_pulses_to_draw = " << n_pulses_to_draw << ", c1 divided into " << x_pads << " x " << y_pads << std::endl;

  TDirectory* tpi_dir = file->GetDirectory("ExportPulse/TPIs");
  TList* tpi_hists = tpi_dir->GetListOfKeys();
  //  int n_max_hists = tpi_hists->GetSize();

  TRandom3 random(seed);

  for (int i_det_bin = 1; i_det_bin <= n_pulses_to_draw; ++i_det_bin) {
    c1->cd(i_det_bin);

    std::string detname = hMain->GetXaxis()->GetBinLabel(i_det_bin);
    if (detname == "") {
      continue; // some bins have blank labels
    }

    // need to replace hyphens with underscores since there are no hyphens in the names stored in the ROOT file
    size_t start_pos = 0;
    std::string to = "_";
    std::string from = "-";
    //    std::cout << "detname before = " << detname << std::endl;
    while((start_pos = detname.find(from, start_pos)) != std::string::npos) {
      detname.replace(start_pos, from.length(), to);
      start_pos += to.length(); // Handles case where 'to' is a substring of 'from'
    }
    //    std::cout << "detname after = " << detname << std::endl;

    int n_pulses_plotted_this_bin = hMain->GetBinContent(i_det_bin);
    int rndm = (random.Rndm())*(n_pulses_plotted_this_bin); // want to get this plot for this detector
    //    std::cout << detname << ": " << n_pulses_plotted_this_bin << " pulses exported and picking number " << rndm << std::endl;

    // loop through the histograms
    TIter next_tpi_hist(tpi_hists);
    TH1F* tpi_hist;
    int i_counter = 0;
    bool done = false;
    while ((tpi_hist = (TH1F*)next_tpi_hist()) && !done ) {

      std::string pulse_name = tpi_hist->GetName();

      if (pulse_name.find(detname) != std::string::npos) {
	//      std::cout << "This pulse is " << pulse_name << ", which does contain " << detname << " ( i_counter = " << i_counter << ")" << std::endl;
	if (i_counter != rndm) {
	  ++i_counter;
	}
	else {
	  //	  std::cout << "Plotting at i_counter = " << i_counter << std::endl;
	  std::string tpi_histname = "ExportPulse/TPIs/" + pulse_name;

	  TH1F* hTPI = (TH1F*) file->Get(tpi_histname.c_str());
	  hTPI->GetXaxis()->SetLabelOffset(0.03);
	  if (!hTPI) {
	    std::cout << "Problem obtaining TPI histogram " << tpi_histname << std::endl;
	    return;
	  }
	  
	  hTPI->Draw();

	  TLatex* latex = new TLatex(hTPI->GetBinCenter(hTPI->GetNbinsX()/2), hTPI->GetMinimum() + (hTPI->GetMaximum() - hTPI->GetMinimum()) / 2, detname.c_str());
	  latex->Draw();
	  latex->SetTextSize(0.1);

	  // Now find the threshold using the bank name that is written in brackets in the title
	  std::string histtitle = hTPI->GetTitle();
	  size_t open_bracket_pos = histtitle.find("(");
	  size_t close_bracket_pos = histtitle.find(")");
	  std::string bankname = histtitle.substr(open_bracket_pos+1, close_bracket_pos - open_bracket_pos-1);
	  //	  std::cout << "AE: " << histtitle << " --> " << bankname << std::endl;
	  std::string drawcmd;
	  if (bankname.find("SIS3") != std::string::npos) {
	    drawcmd = "C1_";
	    drawcmd += bankname;
	    if (bankname.find("SIS3300") != std::string::npos) {
	      drawcmd += "_Threshhold>>hThresh";
	    }
	    else if (bankname.find("SIS3350") != std::string::npos) {
	      drawcmd += "_TrigThreshOn>>hThresh";
	    }
	  }
	  else if(bankname.find("D4") != std::string::npos) {
	    char channel = bankname.at(bankname.find("D4")+3);
	    drawcmd = "C4_WFD_Ch";
	    drawcmd += channel;
	    drawcmd += "_TrigThresh>>hThresh";
	  }
	  else if(bankname.find("D7") != std::string::npos) {
	    char channel = bankname.at(bankname.find("D7")+3);
	    drawcmd = "C7_Ch";
	    drawcmd += channel;
	    drawcmd += "_SelfTrigThreshDPP>>hThresh";
	  }

	  TH1F* hThresh = new TH1F("hThresh", "", 16384,0,16385);
	  //	  std::cout << "AE: drawcmd = " << drawcmd << std::endl;
	  trendtree->Draw(drawcmd.c_str(), "Run==10404", "goff");
	  double threshold = hThresh->GetMean();
	  TLine* line = new TLine(hTPI->GetXaxis()->GetBinLowEdge(1), threshold, hTPI->GetXaxis()->GetBinUpEdge(hTPI->GetNbinsX()), threshold);
	  line->SetLineColor(kRed);
	  line->Draw();

	  done = true;
	}
      }
    }
  }

  c1->SaveAs("~/plots/2017-01-12/ExampleOddPulses_4.pdf");
}
