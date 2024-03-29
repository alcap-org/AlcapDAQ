#ifndef TrendPlot_h_
#define TrendPlot_h_

#include "TH2.h"

#include "CommandLine.h"

class TrendPlot {
 public:
  TrendPlot(std::string histogram_name, TH1* run_plot, ARGUMENTS arguments);
  ~TrendPlot() {};

  TH2F* GetTrendPlot() { return fTrendPlot; }
  void FillTrendPlot(TH1* run_plot, ARGUMENTS arguments, int i_run);

 private:
  /// \brief
  /// The actual trend plot
  TH2F* fTrendPlot;
};

#endif
