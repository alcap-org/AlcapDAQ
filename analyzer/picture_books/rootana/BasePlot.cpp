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
