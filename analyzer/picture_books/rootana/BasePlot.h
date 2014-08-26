#ifndef BasePlot_h_
#define BasePlot_h_

#include "TH1.h"
#include "TCanvas.h"
#include "TStyle.h"

#include "CommandLine.h"

extern TStyle* gStyle;

class BasePlot {
 public:
  BasePlot() {}; // empty constructor (for TrendPlot)
  BasePlot(TCanvas* canvas, TH1* plot);
  ~BasePlot() {};

  TH1* GetPlot() { return fPlot; }

 protected:
  /// \brief
  /// These are mainly for TrendPlot to use
  void SetCanvas(TCanvas* canvas) {fCanvas=canvas;}
  void SetPlot(TH1* plot) {fPlot=plot;}

 public:
  /// \brief
  /// Sets the log scale on each axis
  void SetLogScales(bool log_x, bool log_y, bool log_z);

  /// \brief
  /// Save the plot
  void Save(std::string draw_option, std::string filename);

  /// \brief
  /// Set the stats box how we like
  void SetStats(bool stats_box) { gStyle->SetOptStat(111111);	fPlot->SetStats(stats_box); };

  /// \brief
  /// Auto zoom the axes in to make things a sensible range
  virtual void AutoZoom();

  /// \brief
  /// Try and set the limits to what the user wants
  void SetLimits(std::vector<int> low_limts, std::vector<int> high_limits);
 private:
  /// \brief
  /// The actual plot
  TH1* fPlot;

  /// \brief
  /// The canvas that the plot gets printed on (for log scales)
  TCanvas* fCanvas;
};

#endif
