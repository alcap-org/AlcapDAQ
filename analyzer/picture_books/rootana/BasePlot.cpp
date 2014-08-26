#include "BasePlot.h"

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
