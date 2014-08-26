#ifndef BasePlot_h_
#define BasePlot_h_

#include "TH1.h"
#include "TCanvas.h"

#include "CommandLine.h"

class BasePlot {
 public:
  BasePlot() {}; // empty constructor (for TrendPlot)
  BasePlot(TCanvas* canvas, TH1* plot);
  ~BasePlot() {};

  TH1* GetPlot() { return fPlot; }

  void SetCanvas(TCanvas* canvas) {fCanvas=canvas;}

  /// \brief
  /// Sets the log scale on each axis
  void SetLogScales(bool log_x, bool log_y, bool log_z);

 private:
  /// \brief
  /// The actual plot
  TH1* fPlot;

  /// \brief
  /// The canvas that the plot gets printed on (for log scales)
  TCanvas* fCanvas;
};

#endif
