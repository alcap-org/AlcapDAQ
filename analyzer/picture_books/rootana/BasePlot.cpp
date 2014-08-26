#include "BasePlot.h"

#include <iostream>

BasePlot::BasePlot(TCanvas* canvas, TH1* plot): fCanvas(canvas), fPlot(plot) {
}

void BasePlot::SetLogScales(bool log_x, bool log_y, bool log_z) {
  fCanvas->SetLogx(log_x);
  fCanvas->SetLogy(log_y);
  fCanvas->SetLogz(log_z);
}

void BasePlot::Save(std::string draw_option, std::string filename) {
  fPlot->Draw(draw_option.c_str());
  fCanvas->Update();
  fCanvas->SaveAs(filename.c_str()); 
}

void BasePlot::AutoZoom() {

  // Zoom on x-axis only
  int min_bin = 1;
  int max_bin = 1;
  
  for (int i_bin = 1; i_bin <= fPlot->GetNbinsX(); ++i_bin) {
    if (fPlot->GetBinContent(i_bin) >= 1) {
      if (i_bin > min_bin) {
	min_bin = i_bin;
	break;
      }
    }
  }
  
  for (int i_bin = fPlot->GetNbinsX(); i_bin > 0; --i_bin) {
    if (fPlot->GetBinContent(i_bin) >= 1) {
      // See if this bin is higher than the previous
      if (i_bin > max_bin) {
	max_bin = i_bin;
	break;
      }
    }
  }
  
  fPlot->GetXaxis()->SetRange(min_bin, max_bin); // set the range based on bin number
}

void BasePlot::SetLimits(std::vector<int> low_limits, std::vector<int> high_limits) {

  // First, get the current limits on the plot so we can provide warnings if the user asks for a range outside of these
  const int n_axes = 3;
  int current_low_limits[n_axes] = { fPlot->GetXaxis()->GetXmin(), fPlot->GetYaxis()->GetXmin(), fPlot->GetZaxis()->GetXmin() };
  int current_high_limits[n_axes] = { fPlot->GetXaxis()->GetXmax(), fPlot->GetYaxis()->GetXmax(), fPlot->GetZaxis()->GetXmax() };
  TAxis* axis[n_axes] = { fPlot->GetXaxis(), fPlot->GetYaxis(), fPlot->GetZaxis() };
  // Now loop through the axes and set the limits
  for (int i_axis = 0; i_axis < n_axes; ++i_axis) {
	    
    // Give a warning if the user has specified an out of range limit
    if (low_limits[i_axis] != -999999 && low_limits[i_axis] < current_low_limits[i_axis]) {
      std::cout << "Warning: Lower limit on axis " << i_axis << " specified by the user (" << low_limits[i_axis] 
		<< ") is outside of the current range of the plots (" << current_low_limits[i_axis] 
		<< ") and so won't have any effect. (" << fPlot->GetName() << ")" << std::endl;
    }
    if (high_limits[i_axis] != -999999 && high_limits[i_axis] > current_high_limits[i_axis]) {
      std::cout << "Warning: Upper limit on axis " << i_axis << " specified by the user (" << high_limits[i_axis] 
		<< ") is outside of the current range of the plots (" << current_high_limits[i_axis] 
		<< ") and so won't have any effect. (" << fPlot->GetName() << ")" << std::endl;
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
