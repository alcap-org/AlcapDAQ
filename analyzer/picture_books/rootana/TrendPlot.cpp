#include "TrendPlot.h"

TrendPlot::TrendPlot(std::string histogram_name, TCanvas* canvas, TH1* run_plot, ARGUMENTS arguments) {

  std::string trendplotname = histogram_name + "_TrendPlot";
  std::string trendplottitle = "Trend Plot of " + histogram_name;
	      
  fTrendPlot = new TH2F(trendplotname.c_str(), trendplottitle.c_str(), arguments.stop-arguments.start, arguments.start, arguments.stop, 
			run_plot->GetNbinsX(), run_plot->GetXaxis()->GetXmin(), run_plot->GetXaxis()->GetXmax());
  
  fTrendPlot->GetYaxis()->SetTitle(run_plot->GetXaxis()->GetTitle());
  fTrendPlot->GetYaxis()->SetLabelSize(0.03);
  
  fTrendPlot->GetXaxis()->SetTitle("Run Number");
  fTrendPlot->GetXaxis()->SetLabelSize(0.03);
  
  fTrendPlot->GetZaxis()->SetTitle(run_plot->GetYaxis()->GetTitle());
  fTrendPlot->GetZaxis()->SetLabelSize(0.03);
  fTrendPlot->GetZaxis()->SetTitleOffset(0.85);
  fTrendPlot->GetZaxis()->SetTitleSize(0.03);

  SetCanvas(canvas);
  SetPlot(fTrendPlot);
}

void TrendPlot::FillTrendPlot(TH1* run_plot, ARGUMENTS arguments, int i_run) {
  // Copy the entries from the run plot to the trend plot
  for (int i_bin = 1; i_bin <= run_plot->GetNbinsX(); ++i_bin) {
	      
    // Check that the bin labels match before filling the trend plot
    if (strcmp(fTrendPlot->GetYaxis()->GetBinLabel(i_bin), run_plot->GetXaxis()->GetBinLabel(i_bin)) == 0) {
      fTrendPlot->Fill(arguments.start + i_run, run_plot->GetBinCenter(i_bin), run_plot->GetBinContent(i_bin)); // (x = run #, y = time stamp, z = N_TPI)
    }
    else {
      // loop through the bins again until we see the correct label
      bool match_found = false;
      for (int j_bin = 1; j_bin <= fTrendPlot->GetNbinsY(); ++j_bin) {
	if (strcmp(fTrendPlot->GetYaxis()->GetBinLabel(j_bin), run_plot->GetXaxis()->GetBinLabel(i_bin)) == 0) {
	  fTrendPlot->Fill(arguments.start + i_run, run_plot->GetBinCenter(j_bin), run_plot->GetBinContent(i_bin)); // (x = run #, y = time stamp, z = N_TPI)
	  match_found = true;
	  break;
	}
      }
		
      if (match_found == false) {
	// Find the last emtpy y-axis bin, fill it and re-label it
	for (int j_bin = 1; j_bin <= fTrendPlot->GetNbinsY(); ++j_bin) {
	  if (strcmp(fTrendPlot->GetYaxis()->GetBinLabel(j_bin), "") == 0) {
	    fTrendPlot->Fill(arguments.start + i_run, run_plot->GetBinCenter(j_bin), run_plot->GetBinContent(i_bin));
	    fTrendPlot->GetYaxis()->SetBinLabel(j_bin, run_plot->GetXaxis()->GetBinLabel(i_bin));
	    break;
	  }
	}
      }
    }
  }
}

